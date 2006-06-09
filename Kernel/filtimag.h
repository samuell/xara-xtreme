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
//

#ifndef INC_IMAGEMAPFILTER
#define INC_IMAGEMAPFILTER

#include "filtimop.h"	//For ImagemapFilterOptions
#include "filtirr.h"	//For ImagemapRenderRegion
//#include "filters.h"	//For AuldLangSyne - in camtypes.h [AUTOMATICALLY REMOVED]
#include "htmlexp.h"	//The HTML export filter
//#include "list.h"		//For the list class - in camtypes.h [AUTOMATICALLY REMOVED]

class CCLexFile;
class Operation;

/**************************************************************************************

>	class ImagemapFilter : public HTMLExportFilter

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Purpose:	The imagemap export filter class.

				This code contained in this class is called when

				a. The user selects the Imagemap filter class in the Export...
					dialog box.

				b. The user tells one of the bitmap filters to export an imagemap
					at the same time as it exports the bitmap

**************************************************************************************/
class ImagemapFilter : public HTMLExportFilter
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(ImagemapFilter);

	//Constructor and initialiser
public:
	ImagemapFilter();

	virtual BOOL Init();

	//Destructor
public:
	~ImagemapFilter();


	//Main entry points
public:
	virtual BOOL DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, Document* pDoc,
						  BOOL ShowOptions);
	
	
	BOOL PrepareAndWriteData(Document* pDoc);

	BOOL PrepareData(Document* pDoc);
	BOOL WriteData();

	BOOL GetOptionsFromUser(ImagemapFilterOptions* pifoDefault, PathName* ppthFile, CCLexFile* pFile);
	
	//This function produces the "Do you want to replace file?" message
	BOOL WillAcceptExistingFile(PathName pthToReplace);

	void SearchFileForImagemaps(const PathName& pthSearch, List* plstToAdd);




	//Access functions
public:
	void SetFilterOptions(ImagemapFilterOptions ifoOptions);
	ImagemapFilterOptions GetFilterOptions();

	BOOL ExportSelectionOnly(BOOL MaskedRender = FALSE)
	{
		return (ms_Options.m_stExportArea==SELECTION);
	}

	//Action functions (called from the entry points to do things)
protected:
	void Write(CCLexFile* pfileToWrite);


	//Toolkit functions
protected:
	BOOL PrepareRenderRegion(Document* pDoc);
	void ForceBackgroundRedrawOff(DocView* pView);
	void RestoreBackgroundRedraw(DocView* pView);

	BOOL OpenImagemapFile(CCLexFile* pFile, PathName* pPath);
	BOOL CloseImagemapFile(CCLexFile* pFile);

	INT32 WriteHelper(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);
	INT32 WriteBitmapHTML(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);
	INT32 WriteImagemapHTML(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);
	INT32 WritePreamble(TCHAR* pcBuffer);

	void CopyToClipboard(TCHAR* pcBuffer=NULL);

	BOOL WriteDataToExistingFile(CCLexFile* pFile, PathName* pPath);
	BOOL WriteDataToNewFile(CCLexFile* pFile, PathName* pPath);

	BOOL WriteExistingDataHelper(CCLexFile* pfileTo, CCLexFile* pfileFrom, PathName* pPath);
	BOOL ContainsSameImagemapName(CCLexFile* pfileSearch);

	void BuildMapNamesList(CCLexFile* pfileSearch, PathName* ppthSearch, List* plstToAdd);







	//Member variables
protected:
	//This object should hold all the settings that can be set for the imagemap filter
	static ImagemapFilterOptions ms_Options;

	//The imagemap render region
	ImagemapRenderRegion* m_pRegion;

	//Some member variables to stop background rendering
	DocView* pView;
	BOOL BackgroundRedrawStateSet;
	BOOL BackgroundRedrawState;

	//Static member variables
private:
	//These strings are used to put preferences into the registry
	static TCHAR ms_strApprox[];
	static TCHAR ms_strAllRectangles[];
	static TCHAR ms_strClipboard[];	
	static TCHAR ms_strFile[];	
	static TCHAR ms_strInsert[];	
};

#endif	// INC_IMAGEMAPFILTER
