// $Id: slice.h 662 2006-03-14 21:31:49Z alex $
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
// The operation for the "Image Slice" menu item

#ifndef INC_IMAGESLICE
#define INC_IMAGESLICE

#include "impexpop.h"
//#include <afxtempl.h> // required for CList MFC stuff

class Layer;

class RolloverState : public CObject
{
public:

	Layer * pLayer;
	BOOL Exists;
	BOOL WasVisibleToStartWith;
	String_16	Name;
};


// defines for show layer, etc..
#define DEFAULT		0
#define MOUSE		1
#define CLICKED		2
#define SELECTED	3
#define ALL_LAYERS	4
#define	BACKBAR		4
#define ORIGINAL_LAYERS	5

/********************************************************************************************

>	class CSlice : public CObject

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/99
	Purpose:	Simple class, stores the data required for each slice
	SeeAlso:	OpSlice

********************************************************************************************/
class CSlice : public CObject
{
public:
	CSlice();
	CSlice(INT32 Ax, INT32 Ay, INT32 Cx, INT32 Cy, String_256 ThisName = "", String_16 FileExt = "gif", BOOL Named = FALSE);
	

	void SwapSpreadAndSliceCoords();

	// defines the boundary of the slice in spread co-ords
	// or in slice co-ords which are the same as spreads co-ords
	// only the y axis has been reversed.
	// Top and left are always less than bottom and right
	INT32 top;	
	INT32 bottom;	
	INT32 left;	
	INT32 right;

	// Used to define the HTML table to reform the slices
	INT32 row;	// the row that this slice is in
	INT32 colm;	// the column that this slice is in
	INT32 rowspan;// no. of rows this slice covers
	INT32 colmspan;// no. of columns this slice covers
	INT32 rowheight; // the minimuim height of an item in this row
	INT32 colmwidth; // the minimuim width of an item in this (or previous?) colm

	BOOL deleteme; // this slice has been marked for deletion

	String_256 name;// name of slice
	String_16 FileTypeExtStr; // file type used when exporting

	BOOL IsNamedSlice; // is it a user added slice or a computer generated section?
	INT32 ButtonNumber;
	BOOL IsEmpty;		// if empty the graphic doesn't really need to be exported
	BOOL HasURL;	// Set to false all the time unless ScanURL finds a URL tag on this slice
	TCHAR *pFrameText; // ptr to the frame text of the HTML frame that the URL links to only valid if HasURL

	BYTE ExistsOnLayerState[4];

};


#define OPTOKEN_IMAGESLICE _T("ImageSlice")

class BitmapExportOptions;

/********************************************************************************************

>	class OpSlice : public OpMenuImport

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/99
	Purpose:	The "Image Slice" menu item
	SeeAlso:	

********************************************************************************************/
class OpSlice : public OpMenuImport
{
	CC_DECLARE_DYNCREATE(OpSlice)

public:
	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
	static DocRect ScanLargeSliceBounds(const String_256 & Name);

protected:
	BOOL AddSlice (INT32 Ax, INT32 Ay, INT32 Cx, INT32 Cy, CList <CSlice *, CSlice *> * pMosaicList, String_256 SliceName);
	BOOL InRect(CSlice * pRect, INT32 x, INT32 y);
	BOOL Slice (CSlice * pRect, INT32 Ax, INT32 Ay, INT32 Bx, INT32 By, CList <CSlice *, CSlice *> * pMosaicList);
	BOOL CutsRect(CSlice * pRect, INT32 Ax, INT32 Ay, INT32 Bx, INT32 By);
	BOOL SortSlices (CList <CSlice *, CSlice *> * pMosaicList, const DocRect & Bounds);
	BOOL ExportImageSliceHTML(CList <CSlice *, CSlice *> * pMosaicList, const String_256 & HTMLFile, INT32 FileNumber = 1);
	BOOL ExportSliceGraphic(CSlice * pSlice, BitmapExportOptions * pExportOptions, const String_256 & GraphicFileName);
	BOOL SaveFileDlg();
	INT32 TidyMosaicList(CList <CSlice *, CSlice *> * pMosaicList);
	void UpdateSliceFileExts(CList <CSlice *, CSlice *> * pMosaicList);
	void NudgeSliceIfClose(INT32 &Ax, INT32 &Ay, INT32 &Cx, INT32 &Cy, CList <CSlice *, CSlice *> * pMosaicList);
	void RemoveIlligalFileAndJavaChars(String_256 & Str);
	String_256 TurnUnderScoreIntoSpace(const String_256 & StartStr);
	void PixelAlignedInflate(DocRect * r);
	void MarkEmptySlices(CList <CSlice *, CSlice *> * pMosaicList, DocRect &SelRect, BitmapExportOptions * pExportOptions);
	void ScanTextStorySliceBounds(const String_256 & Name, DocRect & Bounds);
	CSlice * GetButtonNumber (CList <CSlice *, CSlice *> * pMosaicList, INT32 buttonno);

	PathName m_PathName;
	String_256 m_HTMLext;
	INT32 m_lRed, m_lGreen, m_lBlue;
	String_256 m_ErrorStr;

	BOOL m_UsedShimGraphic;

	BOOL m_UsesDesignNotes;

	// for rollover stuff
	BOOL ScanForRolloverStates(CList <CSlice *, CSlice *> * pMosaicList);
	BOOL ExportRollOverSlice(CSlice * pSlice, BitmapExportOptions * pExportOptions);
	void ShowRolloverLayer(INT32 ShowLayer);

	String_256 * m_pLinkName;
	BOOL URLScan(String_256 * pLinkName, CList <CSlice *, CSlice *> * pMosaicList);


	// store data on each rollover state
	RolloverState m_RolloverState[4];
	BOOL m_FoundRolloverStates;
	INT32 m_NumberOfButtons;

	// for the HTML injection code
	static BYTE * LoadFileIntoMemory(String_256 FileName, DWORD & Size);
	static BYTE * FindNextOccuranceOf(BYTE * pSearch, BYTE * pFrom);
	static BYTE * FindPreviousChar(char Search, BYTE * pFrom, BYTE * pLimit);
	static void WriteText(ofstream & f, BYTE * pData, INT32 NoOfBytes);

	void FailledToExportHTML (const String_256 & file);

	BOOL m_ExportedHTMLOK;
	BOOL m_InjectHTML; // did the user ask to inject or overwrite?

};


#include "filedlgs.h"

class SliceSaveFileDlg : public SaveFileDialog
{
public:
	SliceSaveFileDlg(LPCTSTR FilterString, String_256& DefPath, String_256& DefName, String_256& DocumentName) :
		SaveFileDialog(FilterString, DefPath, DefName, DocumentName) {m_Inject = FALSE; }

	// this is a version of OnFileNameOK that helps work around the win 95 bug
	// but asks
	virtual BOOL IsValidFilename();

	BOOL m_Inject;
};


#endif // !INC_IMAGESLICE

