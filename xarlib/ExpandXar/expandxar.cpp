// ExpandXar.cpp : A console application for uncompressing and fixing Xar format files.
//

#include <iostream>
#include "xarlib/xarlib.h"

/****************************************************************************

ExpandXar Overview

This utility is designed for the diagnosis and fixing of Xar format files. 
It reads the input file and creates an uncompressed version of the file as 
the output.  This allows the file to be more easily examined to determine 
what is wrong.

If the file is incomplete or contains corrupt data (e.g. a record size is 
larger than the remaining data in the file) then the import is stopped and 
the output file is terminated correctly.  This involves writing out TAG_UP 
records to balance the tree depth followed by a TAG_ENDOFFILE.  This should 
recover the maximum possible from truncated documents.  However it is 
possible that a document could be truncated at a point where a record is 
missing that is required for loading (e.g. Having a TAG_CONTOURCONTROLLER 
record but no TAG_CONTOUR record).  In this case, the output file being 
uncompressed makes it possible to strip the file back to the last complete 
object.

Any fractal fill attributes that have coincident control points are changed 
to avoid the "Spurious Internal Error" message when loaded into XaraX.

This utility will be extended as required to fix other reported problems.

Due to current limitations of the XarLib library, atomic, essential and 
compression records are not passed to the HandleRecord function.  The code 
keeps track of the record numbers written so it can replace the skipped 
over records with dummy ones to ensure that the record references later in 
the file don't get messed up.

****************************************************************************/


// This structure holds the info we need in the HandleRecord function
typedef struct MyData
{
	CXarExport* pExporter;
	INT32 NextRecord;
	INT32 Depth;
} MyData;

/****************************************************************************

>	BOOL FixFractalPoints(CXaraFileRecord* pRecord)

	Author:		Gerry
	Created:	19/01/2005

	Inputs:		pRecord		- pointer to a CXaraFileRecord
	Returns:	TRUE if the record is modified, FALSE if left alone
	Purpose:	Fixes degenerate fractal fill and transparency attributes.
				Any such attributes that have all three control points 
				coincident are modified by moving the endpoints slightly

****************************************************************************/

bool FixFractalPoints(CXaraFileRecord* pRecord)
{
	DocCoord StartPoint;
	DocCoord EndPoint;
	DocCoord EndPoint2;

	BOOL ok = TRUE;
	
	if (ok) ok = pRecord->ReadCoord(&StartPoint);
	if (ok) ok = pRecord->ReadCoord(&EndPoint);
	if (ok) ok = pRecord->ReadCoord(&EndPoint2);

	if (ok)
	{
		if (StartPoint == EndPoint &&
			StartPoint == EndPoint2)
		{
			// We have a degenerate fill mesh that will cause the dreaded 
			// "Spurious Internal Error" message on loading into XaraX1

			// Fix the EndPoints to not be coincident
			EndPoint.x++;
			EndPoint2.y++;

			pRecord->ResetReadPos();	// The read and write pos are the same

			// Write the three points back into the record
			if (ok) ok = pRecord->WriteCoord(StartPoint);
			if (ok) ok = pRecord->WriteCoord(EndPoint);
			if (ok) ok = pRecord->WriteCoord(EndPoint2);

			return(TRUE);				// Indicate we have changed the record (not that anyone cares)
		}
	}

	// No changes
	return(FALSE);
}




/****************************************************************************

>	HRESULT HandleRecord(void* pMagic, CXaraFileRecord* pRecord)

	Author:		Gerry
	Created:	21/01/2005

	Inputs:		pMagic		- pointer to our MyData structure
				pRecord		- pointer to a CXaraFileRecord
	Returns:	
	Purpose:	Handles the records from the Xar format import process

****************************************************************************/

BOOL HandleRecord(void* pMagic, CXaraFileRecord* pRecord)
{
	// Get our data structure pointer
	MyData* pData = (MyData*) pMagic;

	INT32 ThisNum = pRecord->GetRecordNumber();
	INT32 ThisTag = pRecord->GetTag();

	TRACE(_T("HandleRecord %d - %d"), ThisNum, ThisTag);

	// While this record isn't the one we should be writing
	while (pData->NextRecord < ThisNum)
	{
		// Write a dummy record so the record references don't get screwed
		pData->pExporter->WriteZeroSizedRecord(9999);

		// And increment the record counter
		(pData->NextRecord)++;
	}

	BOOL bDoWrite = TRUE;

	switch (ThisTag)
	{
		case TAG_DOWN:
			(pData->Depth)++;
			break;

		case TAG_UP:
			(pData->Depth)--;
			break;

		case TAG_FRACTALFILL:
		case TAG_FRACTALTRANSPARENTFILL:
		case TAG_NOISEFILL:
		case TAG_NOISETRANSPARENTFILL:
			FixFractalPoints(pRecord);
			break;
	}

	if (bDoWrite)
	{
		// Then write out this record
		pData->pExporter->WriteRecord(pRecord);

		// And increment the record counter
		(pData->NextRecord)++;
	}

	return(TRUE);
}


BOOL ProcessFile(TCHAR* pInputFile, TCHAR* pOutputFile)
{
	// First we ask the library to create a Xar importer
	CXarImport* pImporter = XarLibrary::CreateImporter();
	if (!pImporter)
	{
		wxFprintf(stderr, _T("CreateImporter failed\n"));
		return(FALSE);
	}

	// Call PrepareImport passing the name of the output file
	BOOL ok = pImporter->PrepareImport(pInputFile);
	if (!ok)
	{
		wxFprintf(stderr, _T("PrepareImport failed\n"));
		return(FALSE);
	}

	MyData Data;
	Data.pExporter = NULL;
	Data.NextRecord = 2;
	Data.Depth = 0;

	// Ask the library to create a Xar Exporter object
	Data.pExporter = XarLibrary::CreateExporter();
	if (!Data.pExporter)
	{
		wxFprintf(stderr, _T("CreateExporter failed\n"));
		return(FALSE);
	}

	// Call StartExport passing the desired filename of the Xar file
	ok = Data.pExporter->StartExport(pOutputFile);
	if (!ok)
	{
		wxFprintf(stderr, _T("StartExport failed\n"));
		return(FALSE);
	}

	// Create a CXaraFileRecord object for the document header record
	CXaraFileRecord Rec(TAG_FILEHEADER);
	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteBuffer((BYTE*)"CXN",3);
	if (ok) ok = Rec.WriteUINT32(1);					// File size (doesn't actually care though zero may cause an error on loading)
	if (ok) ok = Rec.WriteUINT32(0);					// Native/Web link ID
	if (ok) ok = Rec.WriteUINT32(0);					// Precompression flags
	if (ok) ok = Rec.WriteASCII(_T("ExpandXar"));		// Producer
	if (ok) ok = Rec.WriteASCII(_T("0.01"));			// Producer version
	if (ok) ok = Rec.WriteASCII(_T(""));				// Producer build
	if (ok) ok = Data.pExporter->WriteRecord(&Rec);

	// Set up our record handler function
	ok = pImporter->SetHandler(&Data, ::HandleRecord);
	if (!ok)
	{
		wxFprintf(stderr, _T("SetHandler failed\n"));
		return(FALSE);
	}

	// Do the import
	ok = pImporter->DoImport();
	if (!ok)
	{
		wxFprintf(stderr, _T("DoImport failed\n"));
	}

	if (Data.Depth > 0)
	{
		wxFprintf(stderr, _T("Fixing tree depth (%d)\n"));
		while (Data.Depth > 0)
		{
			ok = Data.pExporter->WriteZeroSizedRecord(TAG_UP);
			(Data.Depth)--;
		}
	}

	// Clean up the importer
	delete pImporter;

	// Write the end of file record because it doesn't get passed to HandleRecord
	ok = Data.pExporter->WriteZeroSizedRecord(TAG_ENDOFFILE);

	delete Data.pExporter;

	return(TRUE);
}


/****************************************************************************

>	int main(int argc, char* argv[])

	Author:		Gerry
	Created:	21/01/2005

	Inputs:		argc		- number of parameters on command line
				argv[]		- array of parameter strings
	Returns:	
	Purpose:	The main entry point of the code.

****************************************************************************/

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		wxFprintf(stderr, _T("Usage : ExpandXar <input file> <output file>\n"));
		return 1;
	}

#if wxUSE_UNICODE
	wxChar **wxArgv = new wxChar *[argc + 1];
	
	{
		int n;

		for (n = 0; n < argc; n++ )
		{
			wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
			wxArgv[n] = wxStrdup(warg);
		}

		wxArgv[n] = NULL;
	}
#else // !wxUSE_UNICODE
	#define wxArgv argv
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

	int RetVal = 0;

	if (!ProcessFile(wxArgv[1], wxArgv[2]))
	{
		RetVal = 1;
	}

#if wxUSE_UNICODE
	{
		for ( int n = 0; n < argc; n++ )
			free(wxArgv[n]);

		delete [] wxArgv;
	}
#endif // wxUSE_UNICODE

	wxUnusedVar(argc);
	wxUnusedVar(argv);
	
	return RetVal;
}
