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

// kernel class to encompass bitmaps lists
// This covers the bitmap document component related items
// The global bitmap lists are in bmplist.h 
// This file was broken off from bmplist 11/6/96

#ifndef	INC_BMPCOMP
#define	INC_BMPCOMP

//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccomp.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class OILBitmap;
class BitmapInfo;
class RenderRegion;
class Filter;
class FillGeometryAttribute;
class Document;
class EPSExportDC;
class KernelBitmap;
class KernelBitmapRef;

class ExportedBitmaps;
class ImportedBitmaps;
class CXaraFileRecord;
class CXaraFile;
class CXaraFileRecordHandler;



/***********************************************************************************************

>	class SaveBitmapsPool : public List

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Purpose:	A list of Kernel Bitmaps to save out
	SeeAlso:	-

***********************************************************************************************/

class BitmapSavePool : public List
{
	CC_DECLARE_MEMDUMP(BitmapSavePool)

public:
	BOOL AddBitmap(KernelBitmap* pBitmap);
	INT32 GetBitmapIndex(KernelBitmap* pBitmap);
};

/***********************************************************************************************

>	class BitmapList : public List

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Purpose:	A list of Kernel Bitmaps used by a document
	SeeAlso:	-

***********************************************************************************************/

class BitmapList : public List
{
CC_DECLARE_MEMDUMP(BitmapList)

	friend class GlobalBitmapList;

public:
	BitmapList();
	~BitmapList();

	void Init(BaseDocument *ParentDocument);

public:		// Access to special BitmapList data members
	BOOL AddItem(KernelBitmap *Item);
	BOOL RemoveItem(KernelBitmap *Item);

	BOOL MoveAfter(KernelBitmap *InsertPoint, KernelBitmap *ItemToMove);
	BOOL MoveBefore(KernelBitmap *InsertPoint, KernelBitmap *ItemToMove);

	BaseDocument *GetParentDocument(void);

	// Check to see if the bitmap exists already
	KernelBitmap* FindDuplicateBitmap(OILBitmap* pBitmap);

	// Find the default bitmap
	KernelBitmap* FindDefaultBitmap();

private:
	BaseDocument *ParentDoc;
};

/********************************************************************************************

>	class BitmapListComponentClass : public DocComponentClass

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Provide a class that will add Bitmap components to new documents.
	SeeAlso:	DocComponentClass

********************************************************************************************/

class BitmapListComponentClass : public DocComponentClass
{
	CC_DECLARE_DYNAMIC(BitmapListComponentClass)

public:
	static BOOL Init();

	BOOL AddComponent(BaseDocument *);
};

/********************************************************************************************

>	class BitmapListComponent : public DocComponent

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Provide a component that contains a list of kernel bitmaps in use by
				a document.
	SeeAlso:	DocComponent

********************************************************************************************/

class BitmapListComponent : public DocComponent
{
	CC_DECLARE_DYNAMIC(BitmapListComponent)

public:
	BitmapListComponent();
	BitmapListComponent(BitmapList *);
	virtual ~BitmapListComponent();
	BitmapList* GetBitmapList() { return pBitmaps; }

public:
	// Usual document component functions...
	virtual BOOL EPSStartImport(EPSFilter*);
	virtual void EPSEndImport(EPSFilter*, BOOL);
	virtual BOOL WriteEPSComments(EPSFilter*);
	virtual BOOL WriteScript(EPSFilter*);
	virtual ProcessEPSResult ProcessEPSComment(EPSFilter*, const char*);
	virtual void EndOfEPSComment(EPSFilter*);
	virtual INT32 GetSizeOfExport(Filter*);

	virtual BOOL EPSStartExport(EPSFilter*);
	virtual void EPSEndExport(EPSFilter*);

	ExportedBitmaps* GetBitmapExportPool() { return pBitmapsToExport; }
	ImportedBitmaps* GetImportList() { return pImportedBitmaps; }

protected:
	// Function to help with the exporting of the bitmap pool
	BOOL ExportBitmapPoolItem(INT32 BitmapNum, EPSFilter* pFilter, EPSExportDC* pDC, KernelBitmap* pBitmap);

protected:
	// This is the list of bitmaps that we need to export (used for EPS export only) ...
	ExportedBitmaps* pBitmapsToExport;

private:
	BitmapList* pBitmaps;

public:
	virtual BOOL StartComponentCopy();
	virtual BOOL EndComponentCopy();     
	virtual void AbortComponentCopy();

public:
	// These are the Camelot Version 2 Web and Native file related functions
	virtual BOOL StartImport(BaseCamelotFilter* pFilter);
	virtual BOOL EndImport(BaseCamelotFilter* pFilter, BOOL Success);
	virtual BOOL StartExport(BaseCamelotFilter* pFilter);
	virtual BOOL EndExport(BaseCamelotFilter* pFilter, BOOL Success);

	// The public way of importing and exporting bitmaps for the new native/web filters
	KernelBitmap* GetReadBitmapReference(INT32 BitmapRef);
	INT32 GetWriteBitmapReference(KernelBitmap* pBitmap, BaseCamelotFilter* pFilter);
	// Code to save out a bitmap definition
	INT32 SaveBitmapDefinition(KernelBitmap* pBitmap, BaseCamelotFilter* pFilter);
	BOOL LoadBitmapDefinition(CXaraFileRecordHandler* pXFileRecHandler, CXaraFile* pCXFile,
							  INT32 Tag, UINT32 Size, UINT32 RecordNumber);

	// Helper function for SaveBitmapDefinition: writes a bitmap properties record
	BOOL WriteBitmapPropertiesRecord(KernelBitmap* pBitmap, BaseCamelotFilter* pFilter, INT32 BmpRef);

protected:
	// Our list of bitmaps that we have currently exported from this document 
	ExportedBitmaps* pExportedBitmaps;

	// Our list of bitmaps that we have currently imported into this document 
	ImportedBitmaps* pImportedBitmaps;

private:	// Private link used during Importing to provide warnings to the user
	BaseCamelotFilter* pCamFilter;
	// Private variable which means that during Importing we only warn the user
	// once about problem bitmaps
	BOOL WarnedBefore;
};

#endif // INC_BMPCOMP
