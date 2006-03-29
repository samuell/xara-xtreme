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
// The Camelot Native File format for Studio (version 1)

#ifndef INC_XPFILTER
#define INC_XPFILTER

#include "native.h"	// 	CamelotNativeFilter

class WebPrefsDlgParam;
class CapabilityTree;

/********************************************************************************************

>	class PluginNativeFilter : public CamelotNativeFilter

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the plugin import/export filter that passes Xar format data to an 
				external object for further processing.

********************************************************************************************/

class PluginNativeFilter : public CamelotNativeFilter
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(PluginNativeFilter);

public:
	// Construction and initialisation
	PluginNativeFilter();
	~PluginNativeFilter();
	BOOL Init(const CLSID& rCLSID);

	// HowCompatible() returns value in range 0 to 10:
	//   0 => "Definitely Not"
	//  10 => "Yes Sir, that's my file format"
	virtual INT32 HowCompatible( PathName& Filename, ADDR  HeaderStart, UINT32 HeaderSize,
							   UINT32 FileSize );

	// We need to override DoImport to handle the translation
	virtual BOOL DoImport(SelOperation*, CCLexFile*, Document*, 
						  BOOL AutoChosen = FALSE, ImportPosition *Pos = NULL,
						  KernelBitmap** ppImportedBitmap = NULL,
						  DocCoord* pPosTranslate = NULL, String_256* = NULL);

	// We need to override DoExport to handle the translation
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

	// Override these so that the output files aren't compressed
	virtual BOOL StartCompression() { return(TRUE); }
	virtual BOOL StopCompression() { return(TRUE); }

	virtual BOOL GetExportOptions( WebPrefsDlgParam *pPrefs );

	virtual BOOL IsWebFilter() { return FALSE; }
	virtual BOOL IsCompactNativeFilter() { return FALSE; }

	// Find out how much compression is required for bitmaps
	virtual INT32 GetBitmapCompression(); 

	// Find out whether the user has requested a preview bitmap
	virtual BOOL GetPreviewBitmapExport(); 

	// Find out whether the user has requested convert text to outlines
	virtual BOOL GetConvertTextToOutlines(); 
	
	// Find out whether the user has requested remove invisible layers
	virtual BOOL GetRemoveInvisibleLayers(); 

	// Find out whether the user has requested to remove unused colours
	virtual BOOL GetRemoveUnusedColours(); 

	// Find out whether the user has requested to save XPE bitmaps
	virtual BOOL GetSaveXPEBitmaps(); 
	virtual BOOL SetSaveXPEBitmaps(BOOL NewSaveXPEBitmaps);

	virtual INT32 HowCompatibleIsFileType(char* pFileType);

	virtual	double GetSimilarPathTolerance(NodePath* pPath);

	virtual INT32 GetMaxPathLookUp();

	virtual WebPrefsDlgParam* GetCachedExportOptions() { return NULL; }

	virtual BOOL WriteSpecificRegularShapes() { return(TRUE); }

	// This function allows derived filters to cause objects to output
	// bounding box records
	virtual BoundsWriteLevel GetBoundsWriteLevel() { return(m_BoundsLevel); }

	BOOL SetProgressBarCount(UINT32 n);

	void AddBitmapToList(KernelBitmap* pBmp)
	{
		KernelBmpListItem* pItem = new KernelBmpListItem(pBmp);
		if (pItem)
			m_BitmapList.AddTail(pItem);
	}

	String_256 GetNewBitmapName();

protected:
//	virtual BaseCamelotFilter* CreateNULLFilter(Document* pDocument);
	
	virtual char* GetExportFileType();	// Returns a three char long ASCII string

	BOOL GenerateExportData(CapabilityTree* pPlugCaps);
	BOOL DoConversionPassN(CapabilityTree* pPlugCaps, INT32 ConvertPass);
	virtual Node* GetExportNode();
	virtual BOOL BeginDocumentExport();
	virtual BOOL EndDocumentExport();
	virtual BOOL WriteCurrentAttributes();

	// Resource IDs of the strings that describe this filter.
	UINT32 FilterNameID;
	UINT32 FilterInfoID;

	// The string to display when actually exporting rather than rendering.
	// 0 if we're not an export filter.
	// ExportMsgID is used in ExportRender when we are rendering the bitmap
	UINT32 ExportingMsgID;

	Node* m_pNewTree;

	UINT32 m_ProgressOffset;
	BOOL m_bSaveXPEBitmaps;

	BoundsWriteLevel m_BoundsLevel;

	List m_BitmapList;
	INT32 m_BitmapCount;
};


#endif  // INC_XPFILTER
