/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
============================XARAHEADERSTART================================
           Xara X/Xtreme, a vector drawing and manipulation program.  
                    Copyright (C) 2005 Xara Group Ltd.

This program is free software; you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the Free Software 
Foundation; either version 2 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
this program; if not, write to the Free Software Foundation, Inc., 51 
Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Users wishing to use this library in proprietary products which are not 
themselves to be released under the GNU Public License should contact Xara for 
a license to do so. Such a license will normally be granted free of charge.

      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/ 

===========================XARAHEADEREND===================================*/

//#include "camtypes.h"
#include "xarlib.h"
#include "cxfile.h"
#include "ccfile.h"

class CImporter : public CXarImport
{
public:
	CImporter();
	virtual ~CImporter();

	virtual BOOL PrepareImport();
	virtual BOOL PrepareImport(TCHAR* pFileName);
#if defined(USE_COM_STREAM)
	virtual BOOL PrepareImport(IStream* pStream);
#endif	// defined(USE_COM_STREAM)
	virtual BOOL PrepareImport(BYTE* pBuffer, UINT32 Size);
	virtual BOOL SetHandler(void* pMagic, RecordHandler* pHandlerFunk);
	virtual BOOL DoImport();

protected:
	CXaraFile* m_pCXImpFile;
	CCDiskFile* m_pCCImpDiskFile;
	CCStreamFile* m_pCCImpStreamFile;
	CCMemFile* m_pCCImpMemFile;
#if defined(USE_COM_STREAM)
	CCOleStream* m_pCCImpOleFile;
#endif	// defined(USE_COM_STREAM)
};

class CExporter : public CXarExport
{
public:
	CExporter();
	virtual ~CExporter();

	virtual BOOL StartExport();
	virtual BOOL StartExport(TCHAR* pFileName);
#if defined(USE_COM_STREAM)
	virtual BOOL StartExport(IStream* pStream);
#endif	// defined(USE_COM_STREAM)
	virtual BOOL StartBufferedExport();
	virtual BOOL WriteRecord(CXaraFileRecord* pRecord, UINT32* pRecordNum = NULL);
	virtual BOOL WriteZeroSizedRecord(UINT32 Tag, UINT32* pRecordNum = NULL);
	virtual BOOL GetExportBuffer(BYTE** ppBuffer, UINT32* pSize);

protected:
	CXaraFile* m_pCXExpFile;
	CCDiskFile* m_pCCExpDiskFile;
	CCStreamFile* m_pCCExpStreamFile;
	CCMemFile* m_pCCExpMemFile;
#if defined(USE_COM_STREAM)
	CCOleStream* m_pCCExpOleFile;
#endif	// defined(USE_COM_STREAM)
};


CXarImport* XarLibrary::CreateImporter()
{
	return(new CImporter);
}

CXarExport* XarLibrary::CreateExporter()
{
	return(new CExporter);
}

CImporter::CImporter()
{
	m_pCXImpFile = NULL;
	m_pCCImpDiskFile = NULL;
	m_pCCImpStreamFile = NULL;
	m_pCCImpMemFile = NULL;
#if defined(USE_COM_STREAM)
	m_pCCImpOleFile = NULL;
#endif	// defined(USE_COM_STREAM)
}

CImporter::~CImporter()
{
	if (m_pCXImpFile)
	{
		m_pCXImpFile->Close();
	
		delete m_pCXImpFile;
		m_pCXImpFile = NULL;
	}

	if (m_pCCImpDiskFile)
	{
		m_pCCImpDiskFile->close();
		delete m_pCCImpDiskFile;
		m_pCCImpDiskFile = NULL;
	}

	if (m_pCCImpStreamFile)
	{
		m_pCCImpStreamFile->close();
		delete m_pCCImpStreamFile;
		m_pCCImpStreamFile = NULL;
	}

	if (m_pCCImpMemFile)
	{
		m_pCCImpMemFile->close();
		delete m_pCCImpMemFile;
		m_pCCImpMemFile = NULL;
	}

#if defined(USE_COM_STREAM)
	if (m_pCCImpOleFile)
	{
		m_pCCImpOleFile->close();
		delete m_pCCImpOleFile;
		m_pCCImpOleFile = NULL;
	}
#endif	// defined(USE_COM_STREAM)
}


BOOL CImporter::PrepareImport()
{
	if (m_pCXImpFile || m_pCCImpDiskFile || m_pCCImpStreamFile || m_pCCImpMemFile
#if defined(USE_COM_STREAM)
		|| m_pCCImpOleFile
#endif	// defined(USE_COM_STREAM)
		)
	{
		return(FALSE);
	}

	// TODOG: This needs fixing first!!!

	// First off, we have to connect to stdin (cin is the iostream equivalent)
	fstream* pStream = new fstream();
	((iostream*)pStream)->rdbuf(cin.rdbuf());

	m_pCCImpStreamFile = new CCStreamFile(pStream, 1024, FALSE, TRUE);
	if (!m_pCCImpStreamFile)
		return(FALSE);

	// Create our CXaraFile object
	m_pCXImpFile = new CXaraFile;
	if (!m_pCXImpFile)
		return(FALSE);

	// Initialise the standard handlers
	BOOL ok = m_pCXImpFile->SetUpHandlers();

	if (ok)
		ok = m_pCXImpFile->OpenToRead(m_pCCImpStreamFile);

	return(ok);
}



BOOL CImporter::PrepareImport(TCHAR* pFileName)
{
	if (m_pCXImpFile || m_pCCImpDiskFile || m_pCCImpStreamFile || m_pCCImpMemFile
#if defined(USE_COM_STREAM)
		|| m_pCCImpOleFile
#endif	// defined(USE_COM_STREAM)
		)
	{
		return(FALSE);
	}

	// First off, we have to try and open the file
	m_pCCImpDiskFile = new CCDiskFile(1024, FALSE, TRUE);
	if (!m_pCCImpDiskFile)
		return(FALSE);

	PathName FileName(pFileName);

	if (!m_pCCImpDiskFile->open(FileName, ios::in | ios::binary))
	{
		// Failed to open the file...
		TRACE(_T("Failed to open file in PrepareImport\n"));
		return(FALSE);
	}

	// Create our CXaraFile object
	m_pCXImpFile = new CXaraFile;
	if (!m_pCXImpFile)
		return(FALSE);

	// Initialise the standard handlers
	BOOL ok = m_pCXImpFile->SetUpHandlers();

	if (ok)
		ok = m_pCXImpFile->OpenToRead(m_pCCImpDiskFile);

	return(ok);
}


#if defined(USE_COM_STREAM)

BOOL CImporter::PrepareImport(IStream* pStream)
{
	if (m_pCXImpFile || m_pCCImpDiskFile || m_pCCImpStreamFile || m_pCCImpMemFile || m_pCCImpOleFile)
	{
		return(FALSE);
	}

	// First off, we have to try and open the file
	m_pCCImpOleFile = new CCOleStream(pStream, 1024, FALSE, FALSE);
	if (!m_pCCImpOleFile)
		return(E_OUTOFMEMORY);

	// If we get to here then the new CCOleStream has taken ownership of our IStream reference
	// so we must AddRef it to prevent the client's stream being deleted when the CCOleStream goes away
	pStream->AddRef();

//	LARGE_INTEGER Pos = {0};
//	HRESULT hRes = pStream->Seek(Pos, STREAM_SEEK_SET, NULL);

	// Create our CXaraFile object
	m_pCXImpFile = new CXaraFile;
	if (!m_pCXImpFile)
		return(FALSE);

	// Initialise the standard handlers
	BOOL ok = m_pCXImpFile->SetUpHandlers();

	if (ok)
		ok = m_pCXImpFile->OpenToRead(m_pCCImpOleFile);

	return(ok);
}
#endif	// defined(USE_COM_STREAM)

BOOL CImporter::PrepareImport(BYTE* pBuffer, UINT32 Size)
{
	if (m_pCXImpFile || m_pCCImpDiskFile || m_pCCImpStreamFile || m_pCCImpMemFile
#if defined(USE_COM_STREAM)
		|| m_pCCImpOleFile
#endif	// defined(USE_COM_STREAM)
		)
	{
		return(FALSE);
	}

#if FALSE
#if defined(_WIN32)
	char PathBuf[MAX_PATH];
	if (GetTempPath(MAX_PATH, PathBuf) == 0)
		return(FALSE);

	char Buffer[MAX_PATH];
	if (GetTempFileName(PathBuf, "FLI", 0, Buffer) == 0)
		return(FALSE);
#else
	char *PathBuf = ::getenv( "tmp" );
	
	char Buffer[MAX_PATH];
	tsprintf( Buffer, sizeof(Buffer), "%s/libXar-%d-%d-%ld", PathBuf, random(), getpid(), time(NULL) % 1000 );
#endif
#endif

	wxString TempFileName = wxFileName::CreateTempFileName(_T("libXar"));
	
	CCDiskFile TempFile(1024,FALSE,TRUE);
	PathName FileName(TempFileName.c_str());
	if (!TempFile.open(FileName, ios::out | ios::binary))
	{
		// Failed to open the file...
		TRACE(_T("Failed to open temp file in PrepareImport\n"));
		return(FALSE);
	}
	if (TempFile.write(pBuffer, Size).bad())
	{
		// Failed to write the file...
		TRACE(_T("Failed to write temp file in PrepareImport\n"));
		return(FALSE);
	}

	// First off, we have to try and open the file
	m_pCCImpDiskFile = new CCDiskFile(1024, FALSE, TRUE);
	if (!m_pCCImpDiskFile)
		return(FALSE);

	if (!m_pCCImpDiskFile->open(FileName, ios::in | ios::binary))
	{
		// Failed to open the file...
		TRACE(_T("Failed to open temp file in PrepareImport\n"));
		return(FALSE);
	}

//	// First off, we have to try and open the file
//	m_pCCImpMemFile = new CCMemFile(FALSE, TRUE);
//	if (!m_pCCImpMemFile)
//		return(FALSE);

//	if (!m_pCCImpMemFile->open(pBuffer, Size, CCMemRead))
//	{
//		// Failed to open the file...
//		TRACE(_T("Failed to open file in PrepareImport\n"));
//		return(FALSE);
//	}

	// Create our CXaraFile object
	m_pCXImpFile = new CXaraFile;
	if (!m_pCXImpFile)
		return(FALSE);

	// Initialise the standard handlers
	BOOL ok = m_pCXImpFile->SetUpHandlers();

	if (ok)
		ok = m_pCXImpFile->OpenToRead(m_pCCImpDiskFile);

	return(ok);
}

BOOL CImporter::SetHandler(void* pMagic, RecordHandler* pHandlerFunc)
{
	// Inform the standard record handler of the callback function
	if (!m_pCXImpFile)
		return(FALSE);

	m_pCXImpFile->SetExternalRecordHandler(pMagic, pHandlerFunc);

	return(TRUE);
}

BOOL CImporter::DoImport()
{
	BOOL ok = TRUE;

	while (!m_pCXImpFile->IsEndOfFile() && ok)
		ok = m_pCXImpFile->ReadNextRecord();

	if (m_pCCImpDiskFile)
	{
		m_pCCImpDiskFile->close();
		delete m_pCCImpDiskFile;
		m_pCCImpDiskFile = NULL;
	}

	if (m_pCCImpMemFile)
	{
		m_pCCImpMemFile->close();
		delete m_pCCImpMemFile;
		m_pCCImpMemFile = NULL;
	}

	return(ok);
}




CExporter::CExporter()
{
	m_pCXExpFile = NULL;
	m_pCCExpDiskFile = NULL;
	m_pCCExpStreamFile = NULL;
	m_pCCExpMemFile = NULL;
#if defined(USE_COM_STREAM)
	m_pCCExpOleFile = NULL;
#endif	// defined(USE_COM_STREAM)
}

CExporter::~CExporter()
{
	if (m_pCXExpFile)
	{
		m_pCXExpFile->Close();
		delete m_pCXExpFile;
		m_pCXExpFile = NULL;
	}

	if (m_pCCExpDiskFile)
	{
		m_pCCExpDiskFile->close();
		delete m_pCCExpDiskFile;
		m_pCCExpDiskFile = NULL;
	}

	if (m_pCCExpStreamFile)
	{
		m_pCCExpStreamFile->close();
		delete m_pCCExpStreamFile;
		m_pCCExpStreamFile = NULL;
	}

	if (m_pCCExpMemFile)
	{
		m_pCCExpMemFile->close();
		delete m_pCCExpMemFile;
		m_pCCExpMemFile = NULL;
	}

#if defined(USE_COM_STREAM)
	if (m_pCCExpOleFile)
	{
		m_pCCExpOleFile->close();
		delete m_pCCExpOleFile;
		m_pCCExpOleFile = NULL;
	}
#endif	// defined(USE_COM_STREAM)
}


BOOL CExporter::StartExport()
{
	if (m_pCXExpFile || m_pCCExpDiskFile || m_pCCExpStreamFile || m_pCCExpMemFile
#if defined(USE_COM_STREAM)
		|| m_pCCExpOleFile
#endif	// defined(USE_COM_STREAM)
		)
	{
		return(FALSE);
	}

	// First off, we have to connect to stdout (cout is the iostream equivalent)
	fstream* pStream = new fstream;
	((iostream*)pStream)->rdbuf(cout.rdbuf());

	m_pCCExpStreamFile = new CCStreamFile(pStream, 1024, FALSE, FALSE);
	if (!m_pCCExpStreamFile)
		return(FALSE);

	// Create our CXaraFile object
	m_pCXExpFile = new CXaraFile;
	if (!m_pCXExpFile)
		return(FALSE);

	BOOL ok = m_pCXExpFile->OpenToWrite(m_pCCExpStreamFile);

	return(ok);
}



BOOL CExporter::StartExport(TCHAR* pFileName)
{
	if (m_pCXExpFile || m_pCCExpDiskFile || m_pCCExpStreamFile || m_pCCExpMemFile
#if defined(USE_COM_STREAM)
		|| m_pCCExpOleFile
#endif	// defined(USE_COM_STREAM)
		)
	{
		return(FALSE);
	}

	// First off, we have to try and open the file
	m_pCCExpDiskFile = new CCDiskFile(1024, FALSE, FALSE);
	if (!m_pCCExpDiskFile)
		return(FALSE);

	PathName FileName(pFileName);

	if (!m_pCCExpDiskFile->open(FileName, ios::out | ios::binary))
	{
		// Failed to open the file...
		TRACE(_T("Failed to open file in PrepareExport\n"));
		return(FALSE);
	}

	// Create our CXaraFile object
	m_pCXExpFile = new CXaraFile;
	if (!m_pCXExpFile)
		return(FALSE);

	BOOL ok = m_pCXExpFile->OpenToWrite(m_pCCExpDiskFile);

	return(ok);
}


#if defined(USE_COM_STREAM)

BOOL CExporter::StartExport(IStream* pStream)
{
	if (m_pCXExpFile || m_pCCExpDiskFile || m_pCCExpStreamFile || m_pCCExpMemFile || m_pCCExpOleFile)
	{
		return(FALSE);
	}

	// First off, we have to try and open the file
	m_pCCExpOleFile = new CCOleStream(pStream, 1024, FALSE, FALSE);
	if (!m_pCCExpOleFile)
		return(FALSE);

	// If we get to here then the new CCOleStream has taken ownership of our IStream reference
	// so we must AddRef it to prevent the client's stream being deleted when the CCOleStream goes away
	pStream->AddRef();

	// Create our CXaraFile object
	m_pCXExpFile = new CXaraFile;
	if (!m_pCXExpFile)
		return(FALSE);

	BOOL ok = m_pCXExpFile->OpenToWrite(m_pCCExpOleFile);

	return(ok);
}
#endif	// defined(USE_COM_STREAM)


BOOL CExporter::StartBufferedExport()
{
	if (m_pCXExpFile || m_pCCExpDiskFile || m_pCCExpStreamFile || m_pCCExpMemFile
#if defined(USE_COM_STREAM)
		|| m_pCCExpOleFile
#endif	// defined(USE_COM_STREAM)
		)
	{
		return(FALSE);
	}

	// First off, we have to try and open the file
	m_pCCExpMemFile = new CCMemFile(FALSE, FALSE);
	if (!m_pCCExpMemFile)
		return(FALSE);

	if (!m_pCCExpMemFile->open(NULL, 0, CCMemWrite))
	{
		// Failed to open the file...
		TRACE(_T("Failed to open file in PrepareExport\n"));
		return(FALSE);
	}

	// Create our CXaraFile object
	m_pCXExpFile = new CXaraFile;
	if (!m_pCXExpFile)
		return(FALSE);

	BOOL ok = m_pCXExpFile->OpenToWrite(m_pCCExpMemFile);

	return(ok);
}

BOOL CExporter::GetExportBuffer(BYTE** ppBuffer, UINT32* pSize)
{
	if (!ppBuffer || !pSize)
		return(FALSE);
	
	if (!m_pCCExpMemFile)
		return(FALSE);

	if (!m_pCCExpMemFile->GetBuffer(ppBuffer, pSize))
		return(FALSE);

	return(TRUE);
}

BOOL CExporter::WriteRecord(CXaraFileRecord* pRecord, UINT32* pRecordNum)
{
	if (!pRecord)
		return(FALSE);

	if (!m_pCXExpFile)
		return(FALSE);

	UINT32 RecordNum = m_pCXExpFile->Write(pRecord);
	if (pRecordNum)
		*pRecordNum = RecordNum;

	return(TRUE);
}

BOOL CExporter::WriteZeroSizedRecord(UINT32 Tag, UINT32* pRecordNum)
{
	if (!m_pCXExpFile)
		return(FALSE);

	CXaraFileRecord Rec(Tag, 0);
	Rec.Init();
	UINT32 RecordNum = m_pCXExpFile->Write(&Rec);
	if (pRecordNum)
		*pRecordNum = RecordNum;

	return(TRUE);
}

void CamResource::GetBinaryFileInfo(void **pPtr, UINT32 *pSize)
{
	*pPtr = NULL;
	*pSize = 0;
}
