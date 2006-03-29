// $Id$
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               Xara LX, a vector drawing and manipulation program.
                    Copyright (C) 1993-2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

This file is part of Xara LX.

Xara LX is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

Xara LX and its component source files are distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Xara LX (see the file GPL in the root directory of the
distribution); if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


ADDITIONAL RIGHTS
-----------------

Conditional upon your continuing compliance with the GNU General Public
License described above, Xara Group Ltd grants to you certain additional
rights. 

The additional rights are to use, modify, and distribute the software
together with the wxWidgets library, the wxXtra library, and the "CDraw"
library and any other such library that any version of Xara LX relased
by Xara Group Ltd requires in order to compile and execute, including
the static linking of that library to XaraLX. In the case of the
"CDraw" library, you may satisfy obligation under the GNU General Public
License to provide source code by providing a binary copy of the library
concerned and a copy of the license accompanying it.

Nothing in this section restricts any of the rights you have under
the GNU General Public License.


SCOPE OF LICENSE
----------------

This license applies to this program (XaraLX) and its constituent source
files only, and does not necessarily apply to other Xara products which may
in part share the same code base, and are subject to their own licensing
terms.

This license does not apply to files in the wxXtra directory, which
are built into a separate library, and are subject to the wxWindows
license contained within that directory in the file "WXXTRA-LICENSE".

This license does not apply to the binary libraries (if any) within
the "libs" directory, which are subject to a separate license contained
within that directory in the file "LIBS-LICENSE".


ARRANGEMENTS FOR CONTRIBUTION OF MODIFICATIONS
----------------------------------------------

Subject to the terms of the GNU Public License (see above), you are
free to do whatever you like with your modifications. However, you may
(at your option) wish contribute them to Xara's source tree. You can
find details of how to do this at:
  http://www.xaraxtreme.org/developers/

Prior to contributing your modifications, you will need to complete our
contributor agreement. This can be found at:
  http://www.xaraxtreme.org/developers/contribute/

Please note that Xara will not accept modifications which modify any of
the text between the start and end of this header (marked
XARAHEADERSTART and XARAHEADEREND).


MARKS
-----

Xara, Xara LX, Xara X, Xara X/Xtreme, Xara Xtreme, the Xtreme and Xara
designs are registered or unregistered trademarks, design-marks, and/or
service marks of Xara Group Ltd. All rights in these marks are reserved.


      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/

=================================XARAHEADEREND============================
 */

// Filters that are a bit on the OILy side e.g. Metafile, Aldus

#ifndef INC_METAFILT
#define	INC_METAFILT

#include "oilfltrs.h"
#include "clipmap.h"
#include "doccolor.h"

class WinBitmap;
class NodeBitmap;
class MetafileView;



// This is to get access to the 16 bit windows types
typedef short INT16;

#ifdef _MSC_VER
	// Make sure structures correspond to Win16 equivalents.
	// (pack to 1 byte boundaries)
	#pragma pack(push, 1)

	typedef struct
	{
		INT16     lfHeight;
		INT16     lfWidth;
		INT16     lfEscapement;
		INT16     lfOrientation;
		INT16     lfWeight;
		BYTE      lfItalic;
		BYTE      lfUnderline;
		BYTE      lfStrikeOut;
		BYTE      lfCharSet;
		BYTE      lfOutPrecision;
		BYTE      lfClipPrecision;
		BYTE      lfQuality;
		BYTE      lfPitchAndFamily;
		CHAR      lfFaceName[LF_FACESIZE];
	} LOGFONT_16;

	#pragma pack(pop)
#else
	#error Don't know how to pack structures with this compiler!
#endif // _MSC_VER

/********************************************************************************************

>	class MetaFileClipMap : public ClipboardMapping

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	
	Purpose:	A class which describes and implements an available mapping between internal
				and external data formats.

				This class implements clipboard import/export functionality for Windows
				MetaFiles, using the MetaFile Filter.

	SeeAlso:	ExternalClipboard; Filter; MetaFileFilter

*********************************************************************************************/

class MetaFileClipMap : public ClipboardMapping
{
CC_DECLARE_DYNCREATE(MetaFileClipMap)

friend class ExternalClipboard;
friend class OpClipboardExport;
friend class OpClipboardImport;

protected:
	MetaFileClipMap() {}
	MetaFileClipMap(ClipboardMappingType TheType, Filter *TheFilter,
							InternalClipboardFormat &InternalDataType, UINT32 ExternalDataType,
							UINT32 Priority);

public:			// Public method for constructing a mapping object
	static void CreateAndRegister(ClipboardMappingType TheType, Filter *TheFilter,
							InternalClipboardFormat &InternalDataType, UINT32 ExternalDataType,
							UINT32 Priority);


protected:		// Entry points for external clipboard manager
	virtual BOOL HandleImport(SelOperation *Caller, HANDLE ClipboardData, InternalClipboard *Dest);
			// Works out how to call the parent filter to import the given clipboard data
		
	virtual HANDLE HandleExport(Operation *Caller, InternalClipboard *Source);
			// Works out how to call the parent filter to export the given clipboard document
};





// Forward reference for MetaFileFilter (see oilfltrs.cpp for declaration).
class HandleTable;
class CCLexFile;

/********************************************************************************************

< METAHEADER

	Comment:	This structure is used when loading a MetaFile.

				MonoOn
				typedef struct 
				{
					BOOL Placeable;
					METAFILEHEADER Header;
				} METADATA;
				MonoOff

	SeeAlso:	MetaFileFilter; MetaFileFilter::GetMetaFileHandle

********************************************************************************************/ 

typedef struct 
{
	BOOL Placeable;
	METAFILEHEADER Header;
} METADATA;


/********************************************************************************************

>	class MetaFileFilter : public VectorFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	Encapsulates a filter for importing Windows MetaFiles (16-bit only at the
				moment)
	SeeAlso:	Filter

********************************************************************************************/

class MetaFileFilter : public VectorFilter
{
friend class MetaFileClipMap;		// Allow access to GetMetaFileHandle()

	CC_DECLARE_MEMDUMP(MetaFileFilter);
	
public:
	MetaFileFilter();
	~MetaFileFilter();

	BOOL Init();	

	// Works out if opening a file of this type requires a default document to be loaded
	// or does the file format supply the document.
	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);

	INT32 HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
					   UINT32 FileSize );

	BOOL DoImport(SelOperation *Op, CCLexFile*, Document *DestDoc, 
				  BOOL AutoChosen, ImportPosition *Pos = NULL,
				  KernelBitmap** ppImportedBitmap = NULL,
				  DocCoord* pPosTranslate = NULL, String_256* URL=NULL);
	
	// Exporting functions
	BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);
	BOOL DoExport(Operation*, CCLexFile* pFile, Document* pDoc, METAFILEPICT* pMetaInfo);

	// Set the font ready for use
	void SetLogicalFont(LOGFONT_16* pNewFont);

protected:
	BOOL PrepareToImport();
	void CleanUpAfterImport();
		
	// The file we are reading, and the DC for it.
	HMETAFILE 	MetaFile;
	HDC			MetaFileDC;

	BOOL MetaFileHeaderIsOk(METAHEADER *pHeader);

	// this is virtual so that inherited classes can override it
	virtual BOOL OpenAndIterateMetafile( CCLexFile *pDiskFile );

	// Given a metafile filename, return a metafile handle to it.
	HMETAFILE GetMetaFileHandle(LPSTR Filename, METADATA *);

	INT32 DecodeMetafileRecord( METARECORD *pMetaRec );
	// This must be static as it needs to be registered as a Windows callback.
	static INT32 CALLBACK DecodeMetaFile(HDC hdc, HANDLETABLE FAR* pHandleTable, 
		        			  		   METARECORD FAR* pMetaRec, INT32 cObj, LPARAM lParam);


	// Functions for decoding MetaFile records.
	BOOL DecodePolygon(METARECORD* pRec, BOOL FillPolygon);
	BOOL DecodePolyPolygon(METARECORD *);
	BOOL DecodeLineTo(METARECORD* pMetaRec);
	BOOL DecodeRectangle(METARECORD* pMetaRec);
	BOOL DecodeEllipse(METARECORD* pMetaRec);
	BOOL DecodeTextStory(METARECORD* pMetaRec);
	BOOL DecodeExtTextStory(METARECORD* pMetaRec);
	
	// All the bitmap decoding functions
	BOOL DecodeStretchDIB(METARECORD*);
	BOOL DecodeDIBStretchBlt(METARECORD* pMetaRec);
	BOOL DecodeBitBlt(METARECORD* pMetaRec);
	BOOL DecodeStretchBlt(METARECORD* pMetaRec);
	BOOL DecodeDIBToDevice(METARECORD* pMetaRec);
	BOOL DecodeDIBBitBlt(METARECORD* pMetaRec);

	// common code for the bitmap decoding
	KernelBitmap* CreateBitmap(BITMAPINFO* pBitmapStart);
	NodeBitmap* BuildNodeBitmap(KernelBitmap* pBitmap, const DocRect& Position);

	// The Selected Font
	LOGFONT SelectedFont;

	// TransformCoord() - converts a metafile coord to a DocCoord, taking into account the origin.
	void TransformCoord(DocCoord* C);

	// ScaleCoord() - converts a metafile coord to a DocCoord.
	void ScaleCoord(DocCoord* C);

	// Scale factors for coordinates
	INT32 CurrentMappingMode;
	DocCoord MetaFileOrigin;
	BOOL FlipYCoords;
	BOOL IsYExtentNegative;
	INT32 YShift;

	// The layer that objects are placed on
	Layer *pLayer;
	
	// The Node to which new objects are attached (may be a Layer or a Group node)
	Node *pNode;
	
	struct CImportInfo
	{
		// The operation that caused the import
		SelOperation *pOp;

		// Where the file was dropped for drag'n'drop, or NULL if drag'n'drop was not used.
		ImportPosition Pos;
	};

	CImportInfo ImportInfo;

	// Attribute stuff
	BOOL AddAttributes(Node* pNewNode);
	DocColour TextColour;

	// Array of GDI object handles
	HandleTable *Handles;

	// Allow HandleTable objects to set our LineColour/FillColour variables.
	friend HandleTable;

	// For progress cursor
	INT32 FileSize;
	INT32 BytesRead, 
		 LastProgressUpdate;

	// TRUE if we're loading a placeable metafile.
	BOOL Placeable;

	// How many units to the inch for a placeable metafile
	MILLIPOINT ScaleFactor;
	INT32 Dpi;

	// Remembers the current point for operations such as moveto, lineto etc.
	DocCoord CurrentPoint;

	// Need to know if we found anything in this file that we did not understand
	BOOL AllUnderstood;

	//
	// Export related functions
	//
	virtual BOOL PrepareToExport(CCLexFile*, Spread *pSpread);
	virtual void CleanUpAfterExport();
	virtual BOOL WriteToFile(HMETAFILE, METAFILEHEADER*);

	CCLexFile* OutputFile;					// where the metafile is going to (was CCLexFile*)
	CDC ReferenceDC;						// reference DC for the metafile
	CMetaFileDC MetafileDC;					// the CDC that actually gets rendered to
	RenderRegion *ExportRegion;				// where rendering goes
	DocRect ExportClipRect;					// The Clip rect of the export region
	MetafileView* pMetaView;				// The metafile view to help with the rendering

private:
	Node *pLastInsertedNode;				// where we put the last node in the tree so we can
											// insert new nodes as NEXT using this pointer

	inline void AddNodeToMetaFileGroup(NodeRenderableBounded *pNode);
											// Add a node into the group we are importing the
											// metafile into

	HFILE InFile;
};

/********************************************************************************************

>	class AldusFormatFilter : public MetaFileFilter

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95
	Purpose:	Base class for various Aldus-format filters.
	SeeAlso:	MetaFileFilter

********************************************************************************************/
/*
class AldusFormatFilter : public MetaFileFilter
{
	CC_DECLARE_MEMDUMP(AldusFormatFilter);
	
public:
	static BOOL InitAll(List&);

	AldusFormatFilter();
	~AldusFormatFilter();
	BOOL Init(LPTCHAR, LPTCHAR, LPTCHAR);
	BOOL Init();

	INT32 HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
					   UINT32 FileSize );

protected:

	BOOL OpenAndIterateMetafile( CCLexFile *pDiskFile );

	void UnloadDLL();

	LPTCHAR DLLPath;								// complete path to filter DLL
	HMODULE DLLHandle;								// handle of above DLL, NULL=not loaded
	HANDLE DLLPrefHandle;							// for filters preference info

	static UINT32 AutomaticID;						// dynamic filter ID
	static BOOL DeclareFilter( List&, LPTCHAR , LPTCHAR , LPTCHAR );

};
*/
#endif
