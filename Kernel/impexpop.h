// $Id: impexpop.h 662 2006-03-14 21:31:49Z alex $
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

// The operations for importing and exporting using the filter system.

#ifndef INC_IMPORTEXPORTOP
#define INC_IMPORTEXPORTOP

#include "filters.h"
#include "selop.h"
//#include "bmpsdlg.h"

// These used to be are OILy as they used to invoke common dialogs etc
// and hence used to be in OilFltrs.h/cpp
// Moved to new kernel based impexpop.h/cpp 4/1/96 as there is now a class, BaseFileDialog
// and derived classes which handle all of the OILy based common dialog functionality

class BmpDlgParam;
class PathName;


/********************************************************************************************

>	class OpMenuImport : public SelOperation

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	Pops up a common dialog to allow the user to import a file into Camelot
				using any of the available import filters.
	SeeAlso:	OpMenuExport; OpDroppedFile

********************************************************************************************/

// Operation token for the OpMenuImport class
#define	OPTOKEN_MENUIMPORT	_T("Import")

class OpMenuImport : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpMenuImport)

public:
	OpMenuImport();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
	void DoImport(PathName Path, Filter* pFilter, String_256* URL, ImportPosition* pImportPosition=NULL);
	void EndImport();

	virtual BOOL OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }

private:
	char *FilterString;
	static UINT32 SelectedFilterID;
	static INT32 SelectedFilter;


public:
	// The preference for the import path
	static String_256 DefaultImportFilterPath;
};



/********************************************************************************************
>	class BitmapExportParam : public OpParam

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Passes infomation about bitmap(s) to export
	SeeAlso:	OpParam
********************************************************************************************/

class BitmapExportParam : public OpParam
{
	CC_DECLARE_DYNAMIC(BitmapExportParam);

public:
	BitmapExportParam();
	BitmapExportParam(UINT32 Count, KernelBitmap** pBitmap, BmpDlgParam* pParam);
	BmpDlgParam* GetBmpDlgParam()	{ return m_pBmpDlgParam; }
	BOOL SetBmpDlgParam (BmpDlgParam* pParam);

	// Access functions with helpful checking
	virtual UINT32 GetBitmapCount();
	virtual KernelBitmap* GetBitmap(UINT32 Index);

	// We cannot store layers but this removes IS_KIND_OFs when using GIFAnimationExportParam
	virtual Layer * GetLayer(UINT32 Index) { return NULL; }
	virtual UINT32 GetLayerCount() { return 0; }

protected:
	BmpDlgParam* m_pBmpDlgParam;
};



/********************************************************************************************

>	class OpMenuExport : public Operation

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	Pops up a common dialog to allow theuser to export a file from Camelot
				using any of the available import filters.
	SeeAlso:	OpMenuImport; OpDroppedFile

********************************************************************************************/

// Operation token for the OpMenuExport class
#define	OPTOKEN_MENUEXPORT	_T("Export")

class OpMenuExport : public SelOperation
{
public:
	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor*, OpParam*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);

	static void SaveSelection();
	static void NormaliseSelection();
	static void RestoreSelection();

protected:
	void CreateNamedSet(Filter* pFilter, const PathName& pthTarget);
	BOOL SetBitmapName(KernelBitmap* pTheBitmap, String_256* DefaultName, 	
					   BitmapExportParam* pExportParam = 0);

public:
	// The preference for the export path
	static String_256 DefaultExportFilterPath;

private:
	char* FilterString;

	static UINT32 SelectedFilterID;
	static INT32 SelectedFilter;
	static UINT32 DefaultBitmapFilterID;
	static INT32 SelectedBitmapFilter;
	static List* m_pSelNodeList;

	CC_DECLARE_DYNCREATE(OpMenuExport);
};



/********************************************************************************************

>	class OpDroppedFile : public Operation

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	Responds to the user dragging and dropping a file onto Camelot - it 
				attempts to deduce which import filter should be used to load the file.
				If for any reason it cannot work out which filter to use, a dialog is
				popped up to allow the user to select the filter they want to use.
	SeeAlso:	OpMenuImport; OpMenuExport

********************************************************************************************/

// Operation token for the OpDroppedFile class
#define	OPTOKEN_DROPPEDFILE	_T("DroppedFile")

class OpDroppedFile : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpDroppedFile)

public:
	void DoWithParam(OpDescriptor*, OpParam*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const	{ return FALSE; }
};

#endif // !INC_IMPORTEXPORTOP
