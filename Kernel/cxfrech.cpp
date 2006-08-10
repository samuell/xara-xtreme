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
// Implementation of the record handler class of the v2 file format

#include "camtypes.h"
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccolor.h"
#if !defined(EXCLUDE_FROM_XARLIB)
#include "colcomp.h"
#include "bmpcomp.h"
#include "unitcomp.h"
#include "infocomp.h"
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#else
#include "cxfile.h"
#endif
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"

DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(CXaraFileRecordHandler,ListItem);
CC_IMPLEMENT_DYNAMIC(CamelotRecordHandler,CXaraFileRecordHandler);
CC_IMPLEMENT_DYNAMIC(StripSubTreeRecordHandler,CamelotRecordHandler);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------


/********************************************************************************************

>	CXaraFileRecordHandler::CXaraFileRecordHandler()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		-
	Purpose:	Creates a record handler object.
				These are used by CXaraFile for reading the file.
				Handlers handle records as they are read from a file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CXaraFileRecordHandler::CXaraFileRecordHandler()
{
}

/********************************************************************************************

>	CXaraFileRecordHandler::~CXaraFileRecordHandler()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Purpose:	Default destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CXaraFileRecordHandler::~CXaraFileRecordHandler()
{
}


/********************************************************************************************

>	BOOL CXaraFileRecordHandler::IsTagInList(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		Tag	= a tag value
	Returns:	TRUE if tag is in handler's tag list
				FALSE otherwise
	Purpose:	Helper function that searchs the handler's tag list for a given tag
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecordHandler::IsTagInList(UINT32 Tag)
{
	const UINT32* pTagList = GetTagList();

	ERROR2IF(pTagList == NULL,FALSE,"NULL pTagList - is this a default handler?");
	
	while (*pTagList != CXFRH_TAG_LIST_END)
	{
		if (*pTagList == Tag)
			return TRUE;

		pTagList++;
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL CXaraFileRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Inputs:		pCXFile = access to the CXaraFile class
				Tag	 = the tag value
				Size = size of record
				RecordNumber = the record number in the file
	Returns:	TRUE if handled ok
				FALSE otherwise
	Purpose:	This is the baseclass streamed record handler. It should always be overriden.
				We don't want to make it pure virtual and force all handlers to define it
				as most do not require it.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)
{
	ERROR2(FALSE,"CXaraFileRecordHandler::HandleStreamedRecord Baseclass handler called");
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL CXaraFileRecordHandler::Init(BaseCamelotFilter* pThisBaseCamelotFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pThisBaseCamelotFilter = ptr to the filter that will read the file.
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Initialises the handler.
	
				Base class does nowt
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if !defined(EXCLUDE_FROM_XARLIB)
BOOL CXaraFileRecordHandler::Init(BaseCamelotFilter* pThisBaseCamelotFilter)
#else
BOOL CXaraFileRecordHandler::Init(CXaraFile* pCXFile)
#endif
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL CXaraFileRecordHandler::BeginImport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		pThisBaseCamelotFilter = ptr to the filter that will read the file.
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Informs the record handler that importing has begun.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecordHandler::BeginImport()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL CXaraFileRecordHandler::EndImport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Informs the record handler that importing is over

				If you override this func, be sure to call the base class varient
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecordHandler::EndImport()
{
	// Just return TRUE in the base class
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL CXaraFileRecordHandler::BeginSubtree(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96
	Inputs:		Tag = tag value of the tag this handler last handled
	Returns:	TRUE if this func wants exclusive handling of the tag
				FALSE otherwise
	Purpose:	Informs the record handler that a subtree is following a tag of type 'Tag'

				If you override this func and you do not wish other parts of the system to be informed of the
				subtree start, you should return TRUE

				The base class always returns FALSE
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecordHandler::BeginSubtree(UINT32 Tag)
{
	return FALSE;	// FALSE means pass onto other parts of the system
}

/********************************************************************************************

>	virtual BOOL CXaraFileRecordHandler::EndSubtree(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96
	Inputs:		Tag = tag value of the tag this handler last handled
	Returns:	TRUE if this func wants exclusive handling of the tag
				FALSE otherwise
	Purpose:	Informs the record handler that a subtree that followed a tag of type 'Tag' has ended

				If you override this func and you do not wish other parts of the system to be informed of the
				subtree start, you should return TRUE

				The base class always returns FALSE
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecordHandler::EndSubtree(UINT32 Tag)
{
	return FALSE;	// FALSE means pass onto other parts of the system
}

/********************************************************************************************

>	virtual void CXaraFileRecordHandler::GetTagText(UINT32 Tag,String_256& Str)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		Tag	= the tag value
				Str = place to put textual description of the tag
	Returns:	-
	Purpose:	This is the base tag description function.
				Record handler writers may as well update this func for new tags,
				but the func is virtual, so if you want to override it, feel free.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


#ifdef XAR_TREE_DIALOG
void CXaraFileRecordHandler::GetTagText(UINT32 Tag,String_256& Str)
{
	switch (Tag)
	{
		case TAG_UP						: Str = _T("TAG_UP");					break;
		case TAG_DOWN					: Str = _T("TAG_DOWN");					break;
		case TAG_FILEHEADER				: Str = _T("TAG_FILEHEADER");			break;
		case TAG_ENDOFFILE				: Str = _T("TAG_ENDOFFILE");			break;
		case TAG_STARTCOMPRESSION		: Str = _T("TAG_STARTCOMPRESSION");		break;
		case TAG_ENDCOMPRESSION			: Str = _T("TAG_ENDCOMPRESSION");		break;
		case TAG_DOCUMENT				: Str = _T("TAG_DOCUMENT");				break;
		case TAG_CHAPTER				: Str = _T("TAG_CHAPTER");				break;
		case TAG_SPREAD					: Str = _T("TAG_SPREAD");				break;
		case TAG_LAYER					: Str = _T("TAG_LAYER");				break;
		case TAG_PAGE					: Str = _T("TAG_PAGE");					break;
		case TAG_SPREADINFORMATION		: Str = _T("TAG_SPREADINFORMATION");	break;
		case TAG_GRIDRULERSETTINGS		: Str = _T("TAG_GRIDRULERSETTINGS");	break;
		case TAG_GRIDRULERORIGIN		: Str = _T("TAG_GRIDRULERORIGIN");		break;
		case TAG_LAYERDETAILS			: Str = _T("TAG_LAYERDETAILS");			break;
		case TAG_GUIDELAYERDETAILS		: Str = _T("TAG_GUIDELAYERDETAILS");	break;
		case TAG_GUIDELINE				: Str = _T("TAG_GUIDELINE");			break;

		case TAG_ATOMICTAGS				: Str = _T("TAG_ATOMICTAGS");			break;
		case TAG_ESSENTIALTAGS			: Str = _T("TAG_ESSENTIALTAG");			break;
		case TAG_TAGDESCRIPTION			: Str = _T("TAG_TAGDESCRIPTION");		break;
		case TAG_NONRENDERSECTION_START	: Str = _T("TAG_NONRENDERSECTION_START");break;
		case TAG_NONRENDERSECTION_END	: Str = _T("TAG_NONRENDERSECTION_END");	break;
		case TAG_RENDERING_PAUSE		: Str = _T("TAG_RENDERING_PAUSE");		break;
		case TAG_RENDERING_RESUME		: Str = _T("TAG_RENDERING_RESUME");		break;

		case TAG_SPREADSCALING_ACTIVE	: Str = _T("TAG_SPREADSCALING_ACTIVE");		break;
		case TAG_SPREADSCALING_INACTIVE	: Str = _T("TAG_SPREADSCALING_INACTIVE");	break;

		case TAG_DEFINERGBCOLOUR		: Str = _T("TAG_DEFINERGBCOLOUR");		break;
		case TAG_DEFINECOMPLEXCOLOUR	: Str = _T("TAG_DEFINECOMPLEXCOLOUR");	break;

		case TAG_PREVIEWBITMAP_BMP		: Str = _T("TAG_PREVIEWBITMAP_BMP");		break;
		case TAG_PREVIEWBITMAP_GIF		: Str = _T("TAG_PREVIEWBITMAP_GIF");		break;
		case TAG_PREVIEWBITMAP_JPEG		: Str = _T("TAG_PREVIEWBITMAP_JPEG");		break;
		case TAG_PREVIEWBITMAP_PNG		: Str = _T("TAG_PREVIEWBITMAP_PNG");		break;
		case TAG_PREVIEWBITMAP_TIFFLZW	: Str = _T("TAG_PREVIEWBITMAP_TIFFLZW");	break;

		case TAG_DEFINEBITMAP_BMP		: Str = _T("TAG_DEFINEBITMAP_BMP");		break;
		case TAG_DEFINEBITMAP_GIF		: Str = _T("TAG_DEFINEBITMAP_GIF");		break;
		case TAG_DEFINEBITMAP_JPEG		: Str = _T("TAG_DEFINEBITMAP_JPEG");	break;
		case TAG_DEFINEBITMAP_PNG		: Str = _T("TAG_DEFINEBITMAP_PNG");		break;
		case TAG_DEFINEBITMAP_BMPZIP	: Str = _T("TAG_DEFINEBITMAP_BMPZIP");	break;
		case TAG_DEFINESOUND_WAV		: Str = _T("TAG_DEFINESOUND_WAV");		break;
		case TAG_DEFINEBITMAP_JPEG8BPP	: Str = _T("TAG_DEFINEBITMAP_JPEG8BPP");	break;

		case TAG_VIEWPORT				: Str = _T("TAG_VIEWPORT");					break;
		case TAG_VIEWQUALITY			: Str = _T("TAG_VIEWQUALITY");				break;
		case TAG_DOCUMENTVIEW			: Str = _T("TAG_DOCUMENTVIEW");				break;

		case TAG_DEFINE_PREFIXUSERUNIT	: Str = _T("TAG_DEFINE_PREFIXUSERUNIT");	break;
		case TAG_DEFINE_SUFFIXUSERUNIT	: Str = _T("TAG_DEFINE_SUFFIXUSERUNIT");	break;
		case TAG_DEFINE_DEFAULTUNITS	: Str = _T("TAG_DEFINE_DEFAULTUNITS");		break;

		case TAG_DOCUMENTCOMMENT		: Str = _T("TAG_DOCUMENTCOMMENT");			break;
		case TAG_DOCUMENTDATES			: Str = _T("TAG_DOCUMENTDATES");			break;
		case TAG_DOCUMENTUNDOSIZE		: Str = _T("TAG_DOCUMENTUNDOSIZE");			break;
		case TAG_DOCUMENTFLAGS			: Str = _T("TAG_DOCUMENTFLAGS");			break;

		case TAG_PATH							: Str = _T("TAG_PATH");							break;
		case TAG_PATH_FILLED					: Str = _T("TAG_PATH_FILLED");					break;
		case TAG_PATH_STROKED					: Str = _T("TAG_PATH_STROKED");					break;
		case TAG_PATH_FILLED_STROKED			: Str = _T("TAG_PATH_FILLED_STROKED");			break;
		case TAG_PATH_RELATIVE					: Str = _T("TAG_PATH_RELATIVE");				break;
		case TAG_PATH_RELATIVE_FILLED			: Str = _T("TAG_PATH_RELATIVE_FILLED");			break;
		case TAG_PATH_RELATIVE_STROKED			: Str = _T("TAG_PATH_RELATIVE_STROKED");		break;
		case TAG_PATH_RELATIVE_FILLED_STROKED	: Str = _T("TAG_PATH_RELATIVE_FILLED_STROKED"); break;
		case TAG_PATH_FLAGS						: Str = _T("TAG_PATH_FLAGS");					break;
		case TAG_PATHREF_TRANSFORM				: Str = _T("TAG_PATHREF_TRANSFORM");			break;
		case TAG_GROUP							: Str = _T("TAG_GROUP");						break;
		case TAG_GROUPTRANSP					: Str = _T("TAG_GROUPTRANSP");					break;
		case TAG_CACHEDNODESGROUP				: Str = _T("TAG_CACHEDNODESGROUP");				break;
		case TAG_BLEND							: Str = _T("TAG_BLEND");						break;
		case TAG_BLENDER						: Str = _T("TAG_BLENDER");						break;
		case TAG_BLENDER_CURVEPROP				: Str = _T("TAG_BLENDER_CURVEPROP");			break;
		case TAG_BLENDER_CURVEANGLES			: Str = _T("TAG_BLENDER_CURVEANGLES");			break;
		case TAG_BLEND_PATH						: Str = _T("TAG_BLEND_PATH");					break;

		case TAG_MOULD_ENVELOPE					: Str = _T("TAG_MOULD_ENVELOPE");				break;
		case TAG_MOULD_PERSPECTIVE				: Str = _T("TAG_MOULD_PERSPECTIVE");			break;
		case TAG_MOULD_GROUP					: Str = _T("TAG_MOULD_GROUP");					break;
		case TAG_MOULD_PATH						: Str = _T("TAG_MOULD_PATH");					break;

		case TAG_FLATFILL							: Str = _T("TAG_FLATFILL");								break;
		case TAG_FLATFILL_NONE						: Str = _T("TAG_FLATFILL_NONE");						break;
		case TAG_FLATFILL_BLACK						: Str = _T("TAG_FLATFILL_BLACK");						break;
		case TAG_FLATFILL_WHITE						: Str = _T("TAG_FLATFILL_WHITE");						break;
		case TAG_LINECOLOUR							: Str = _T("TAG_LINECOLOUR");							break;
		case TAG_LINECOLOUR_NONE					: Str = _T("TAG_LINECOLOUR_NONE");						break;
		case TAG_LINECOLOUR_BLACK					: Str = _T("TAG_LINECOLOUR_BLACK");						break;
		case TAG_LINECOLOUR_WHITE					: Str = _T("TAG_LINECOLOUR_WHITE");						break;
		case TAG_LINEWIDTH							: Str = _T("TAG_LINEWIDTH");							break;
		case TAG_LINEARFILL							: Str = _T("TAG_LINEARFILL");							break;
		case TAG_LINEARFILL3POINT					: Str = _T("TAG_LINEARFILL3POINT");						break;
		case TAG_LINEARFILLMULTISTAGE				: Str = _T("TAG_LINEARFILLMULTISTAGE");					break;
		case TAG_LINEARFILLMULTISTAGE3POINT			: Str = _T("TAG_LINEARFILL3POINTMULTISTAGE");			break;
		case TAG_CIRCULARFILL						: Str = _T("TAG_CIRCULARFILL");							break;
		case TAG_ELLIPTICALFILL						: Str = _T("TAG_ELLIPTICALFILL");						break;
		case TAG_CONICALFILL						: Str = _T("TAG_CONICALFILL");							break;
		case TAG_SQUAREFILL							: Str = _T("TAG_SQUAREFILL");							break;
		case TAG_THREECOLFILL						: Str = _T("TAG_THREECOLFILL");							break;
		case TAG_FOURCOLFILL						: Str = _T("TAG_FOURCOLFILL");							break;
		case TAG_BITMAPFILL							: Str = _T("TAG_BITMAPFILL");							break;
		case TAG_CONTONEBITMAPFILL					: Str = _T("TAG_CONTONEBITMAPFILL");					break;
		case TAG_FRACTALFILL						: Str = _T("TAG_FRACTALFILL");							break;
		case TAG_NOISEFILL							: Str = _T("TAG_NOISEFILL");							break;
		case TAG_FILLEFFECT_FADE					: Str = _T("TAG_FILLEFFECT_FADE");						break;
		case TAG_FILLEFFECT_RAINBOW					: Str = _T("TAG_FILLEFFECT_RAINBOW");					break;
		case TAG_FILLEFFECT_ALTRAINBOW				: Str = _T("TAG_FILLEFFECT_ALTRAINBOW");				break;
		case TAG_FILL_REPEATING						: Str = _T("TAG_FILL_REPEATING");						break;
		case TAG_FILL_NONREPEATING					: Str = _T("TAG_FILL_NONREPEATING");					break;
		case TAG_FILL_REPEATINGINVERTED				: Str = _T("TAG_FILL_REPEATINGINVERTED");				break;
		case TAG_TRANSPARENTFILL_REPEATING			: Str = _T("TAG_TRANSPARENTFILL_REPEATING");			break;
		case TAG_TRANSPARENTFILL_NONREPEATING		: Str = _T("TAG_TRANSPARENTFILL_NONREPEATING");			break;
		case TAG_TRANSPARENTFILL_REPEATINGINVERTED	: Str = _T("TAG_TRANSPARENTFILL_REPEATINGINVERTED");	break;
		case TAG_FLATTRANSPARENTFILL				: Str = _T("TAG_FLATTRANSPARENTFILL");					break;
		case TAG_LINEARTRANSPARENTFILL				: Str = _T("TAG_LINEARTRANSPARENTFILL");				break;
		case TAG_LINEARTRANSPARENTFILL3POINT		: Str = _T("TAG_LINEARTRANSPARENTFILL3POINT");			break;
		case TAG_CIRCULARTRANSPARENTFILL			: Str = _T("TAG_CIRCULARTRANSPARENTFILL");				break;
		case TAG_ELLIPTICALTRANSPARENTFILL			: Str = _T("TAG_ELLIPTICALTRANSPARENTFILL");			break;
		case TAG_CONICALTRANSPARENTFILL				: Str = _T("TAG_CONICALTRANSPARENTFILLL");				break;
		case TAG_SQUARETRANSPARENTFILL				: Str = _T("TAG_SQUARETRANSPARENTFILLL");				break;
		case TAG_THREECOLTRANSPARENTFILL			: Str = _T("TAG_THREECOLTRANSPARENTFILL");				break;
		case TAG_FOURCOLTRANSPARENTFILL				: Str = _T("TAG_FOURCOLTRANSPARENTFILL");				break;
		case TAG_BITMAPTRANSPARENTFILL				: Str = _T("TAG_BITMAPTRANSPARENTFILL");				break;
		case TAG_FRACTALTRANSPARENTFILL				: Str = _T("TAG_FRACTALTRANSPARENTFILL");				break;
		case TAG_NOISETRANSPARENTFILL				: Str = _T("TAG_NOISETRANSPARENTFILL");					break;
		case TAG_LINETRANSPARENCY					: Str = _T("TAG_LINETRANSPARENCY");						break;
		case TAG_STARTCAP							: Str = _T("TAG_STARTCAP");								break;
		case TAG_ENDCAP								: Str = _T("TAG_ENDCAP");								break;
		case TAG_JOINSTYLE							: Str = _T("TAG_JOINSTYLE");							break;
		case TAG_MITRELIMIT							: Str = _T("TAG_MITRELIMIT");							break;
		case TAG_WINDINGRULE						: Str = _T("TAG_WINDINGRULE");							break;
		case TAG_QUALITY							: Str = _T("TAG_QUALITY");								break;

		case TAG_DASHSTYLE							: Str = _T("TAG_DASHSTYLE");								break;
		case TAG_DEFINEDASH							: Str = _T("TAG_DEFINEDASH");								break;
		case TAG_DEFINEDASH_SCALED					: Str = _T("TAG_DEFINEDASH_SCALED");								break;
		case TAG_ARROWHEAD							: Str = _T("TAG_ARROWHEAD");								break;
		case TAG_ARROWTAIL							: Str = _T("TAG_ARROWTAIL");								break;
		case TAG_DEFINEARROW						: Str = _T("TAG_DEFINEARROW");								break;

		case TAG_USERVALUE							: Str = _T("TAG_USERVALUE");								break;

		case TAG_NODE_BITMAP			: Str = _T("TAG_NODE_BITMAP");			break;
		case TAG_NODE_CONTONEDBITMAP	: Str = _T("TAG_NODE_CONTONEDBITMAP");	break;

		case TAG_ELLIPSE_SIMPLE									: Str = _T("TAG_ELLIPSE_SIMPLE");								break;
		case TAG_ELLIPSE_COMPLEX								: Str = _T("TAG_ELLIPSE_COMPLEX");								break;
		case TAG_RECTANGLE_SIMPLE								: Str = _T("TAG_RECTANGLE_SIMPLE");								break;
		case TAG_RECTANGLE_SIMPLE_REFORMED						: Str = _T("TAG_RECTANGLE_SIMPLE_REFORMED");					break;
		case TAG_RECTANGLE_SIMPLE_STELLATED						: Str = _T("TAG_RECTANGLE_SIMPLE_STELLATED");					break;
		case TAG_RECTANGLE_SIMPLE_STELLATED_REFORMED			: Str = _T("TAG_RECTANGLE_SIMPLE_STELLATED_REFORMED");			break;
		case TAG_RECTANGLE_SIMPLE_ROUNDED						: Str = _T("TAG_RECTANGLE_SIMPLE_ROUNDED");						break;
		case TAG_RECTANGLE_SIMPLE_ROUNDED_REFORMED				: Str = _T("TAG_RECTANGLE_SIMPLE_ROUNDED_REFORMED");			break;
		case TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED				: Str = _T("TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED");			break;
		case TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_REFORMED	: Str = _T("TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_REFORMED");	break;
		case TAG_RECTANGLE_COMPLEX								: Str = _T("TAG_RECTANGLE_COMPLEX");							break;
		case TAG_RECTANGLE_COMPLEX_REFORMED						: Str = _T("TAG_RECTANGLE_COMPLEX_REFORMED");					break;
		case TAG_RECTANGLE_COMPLEX_STELLATED					: Str = _T("TAG_RECTANGLE_COMPLEX_STELLATED");					break;
		case TAG_RECTANGLE_COMPLEX_STELLATED_REFORMED			: Str = _T("TAG_RECTANGLE_COMPLEX_STELLATED_REFORMED");			break;
		case TAG_RECTANGLE_COMPLEX_ROUNDED						: Str = _T("TAG_RECTANGLE_COMPLEX_ROUNDED");					break;
		case TAG_RECTANGLE_COMPLEX_ROUNDED_REFORMED				: Str = _T("TAG_RECTANGLE_COMPLEX_ROUNDED_REFORMED");			break;
		case TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED			: Str = _T("TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED");			break;
		case TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_REFORMED	: Str = _T("TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_REFORMED"); break;
		case TAG_POLYGON_COMPLEX								: Str = _T("TAG_POLYGON_COMPLEX");								break;
		case TAG_POLYGON_COMPLEX_REFORMED						: Str = _T("TAG_POLYGON_COMPLEX_REFORMED");						break;
		case TAG_POLYGON_COMPLEX_STELLATED						: Str = _T("TAG_POLYGON_COMPLEX_STELLATED");					break;
		case TAG_POLYGON_COMPLEX_STELLATED_REFORMED				: Str = _T("TAG_POLYGON_COMPLEX_STELLATED_REFORMED");			break;
		case TAG_POLYGON_COMPLEX_ROUNDED						: Str = _T("TAG_POLYGON_COMPLEX_ROUNDED");						break;
		case TAG_POLYGON_COMPLEX_ROUNDED_REFORMED				: Str = _T("TAG_POLYGON_COMPLEX_ROUNDED_REFORMED");				break;
		case TAG_POLYGON_COMPLEX_ROUNDED_STELLATED				: Str = _T("TAG_POLYGON_COMPLEX_ROUNDED_STELLATED");			break;
		case TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED		: Str = _T("TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED");	break;
		case TAG_REGULAR_SHAPE_PHASE_1					: Str = _T("TAG_REGULAR_SHAPE_GENERAL_PHASE_1");				break;
		case TAG_REGULAR_SHAPE_PHASE_2					: Str = _T("TAG_REGULAR_SHAPE_GENERAL_PHASE_2");				break;

		case TAG_FONT_DEF_TRUETYPE				: Str=_T("TAG_FONT_DEF_TRUETYPE"); break;
		case TAG_FONT_DEF_ATM					: Str=_T("TAG_FONT_DEF_ATM"); break;

		case TAG_TEXT_STORY_SIMPLE				: Str=_T("TAG_TEXT_STORY_SIMPLE"); break;
		case TAG_TEXT_STORY_COMPLEX				: Str=_T("TAG_TEXT_STORY_COMPLEX"); break;
		case TAG_TEXT_STORY_SIMPLE_START_LEFT	: Str=_T("TAG_TEXT_STORY_SIMPLE_START_LEFT"); break;
		case TAG_TEXT_STORY_SIMPLE_START_RIGHT	: Str=_T("TAG_TEXT_STORY_SIMPLE_START_RIGHT"); break;
		case TAG_TEXT_STORY_SIMPLE_END_LEFT		: Str=_T("TAG_TEXT_STORY_SIMPLE_END_LEFT"); break;
		case TAG_TEXT_STORY_SIMPLE_END_RIGHT	: Str=_T("TAG_TEXT_STORY_SIMPLE_END_RIGHT"); break;
		case TAG_TEXT_STORY_COMPLEX_START_LEFT	: Str=_T("TAG_TEXT_STORY_COMPLEX_START_LEFT"); break;
		case TAG_TEXT_STORY_COMPLEX_START_RIGHT	: Str=_T("TAG_TEXT_STORY_COMPLEX_START_RIGHT"); break;
		case TAG_TEXT_STORY_COMPLEX_END_LEFT	: Str=_T("TAG_TEXT_STORY_COMPLEX_END_LEFT"); break;
		case TAG_TEXT_STORY_COMPLEX_END_RIGHT	: Str=_T("TAG_TEXT_STORY_COMPLEX_END_RIGHT"); break;
		case TAG_TEXT_STORY_WORD_WRAP_INFO		: Str=_T("TAG_TEXT_STORY_WORD_WRAP_INFO"); break;
		case TAG_TEXT_STORY_INDENT_INFO			: Str=_T("TAG_TEXT_STORY_INDENT_INFO"); break;

		case TAG_TEXT_LINE						: Str=_T("TAG_TEXT_LINE"); break;
		case TAG_TEXT_STRING					: Str=_T("TAG_TEXT_STRING"); break;
		case TAG_TEXT_CHAR						: Str=_T("TAG_TEXT_CHAR"); break;
		case TAG_TEXT_EOL						: Str=_T("TAG_TEXT_EOL"); break;
		case TAG_TEXT_KERN						: Str=_T("TAG_TEXT_KERN"); break;
		case TAG_TEXT_CARET						: Str=_T("TAG_TEXT_CARET"); break;
		case TAG_TEXT_LINE_INFO					: Str=_T("TAG_TEXT_LINE_INFO"); break;
		case TAG_TEXT_TAB						: Str=_T("TAG_TEXT_TAB"); break;

		case TAG_TEXT_LINESPACE_RATIO			: Str=_T("TAG_TEXT_LINESPACE_RATIO"); break;
		case TAG_TEXT_LINESPACE_ABSOLUTE		: Str=_T("TAG_TEXT_LINESPACE_ABSOLUTE"); break;
		case TAG_TEXT_JUSTIFICATION_LEFT		: Str=_T("TAG_TEXT_JUSTIFICATION_LEFT"); break;
		case TAG_TEXT_JUSTIFICATION_CENTRE		: Str=_T("TAG_TEXT_JUSTIFICATION_CENTRE"); break;
		case TAG_TEXT_JUSTIFICATION_RIGHT		: Str=_T("TAG_TEXT_JUSTIFICATION_RIGHT"); break;
		case TAG_TEXT_JUSTIFICATION_FULL		: Str=_T("TAG_TEXT_JUSTIFICATION_FULL"); break;
		case TAG_TEXT_FONT_SIZE					: Str=_T("TAG_TEXT_FONT_SIZE"); break;
		case TAG_TEXT_FONT_TYPEFACE				: Str=_T("TAG_TEXT_FONT_TYPEFACE"); break;
		case TAG_TEXT_BOLD_ON					: Str=_T("TAG_TEXT_BOLD_ON"); break;
		case TAG_TEXT_BOLD_OFF					: Str=_T("TAG_TEXT_BOLD_OFF"); break;
		case TAG_TEXT_ITALIC_ON					: Str=_T("TAG_TEXT_ITALIC_ON"); break;
		case TAG_TEXT_ITALIC_OFF				: Str=_T("TAG_TEXT_ITALIC_OFF"); break;
		case TAG_TEXT_UNDERLINE_ON				: Str=_T("TAG_TEXT_UNDERLINE_ON"); break;
		case TAG_TEXT_UNDERLINE_OFF				: Str=_T("TAG_TEXT_UNDERLINE_OFF"); break;
		case TAG_TEXT_SCRIPT_ON					: Str=_T("TAG_TEXT_SCRIPT_ON"); break;
		case TAG_TEXT_SCRIPT_OFF				: Str=_T("TAG_TEXT_SCRIPT_OFF"); break;
		case TAG_TEXT_SUPERSCRIPT_ON			: Str=_T("TAG_TEXT_SUPERSCRIPT_ON"); break;
		case TAG_TEXT_SUBSCRIPT_ON				: Str=_T("TAG_TEXT_SUBSCRIPT_ON"); break;
		case TAG_TEXT_TRACKING					: Str=_T("TAG_TEXT_TRACKING"); break;
		case TAG_TEXT_ASPECT_RATIO				: Str=_T("TAG_TEXT_ASPECT_RATIO"); break;
		case TAG_TEXT_BASELINE					: Str=_T("TAG_TEXT_BASELINE"); break;
		case TAG_TEXT_LEFT_INDENT				: Str=_T("TAG_TEXT_LEFT_INDENT"); break;
		case TAG_TEXT_FIRST_INDENT				: Str=_T("TAG_TEXT_FIRST_INDENT"); break;
		case TAG_TEXT_RIGHT_INDENT				: Str=_T("TAG_TEXT_RIGHT_INDENT"); break;
		case TAG_TEXT_RULER						: Str=_T("TAG_TEXT_RULER"); break;

		case TAG_OVERPRINTLINEON				: Str=_T("TAG_OVERPRINTLINEON"); break;
		case TAG_OVERPRINTLINEOFF				: Str=_T("TAG_OVERPRINTLINEOFF"); break;
		case TAG_OVERPRINTFILLON				: Str=_T("TAG_OVERPRINTFILLON"); break;
		case TAG_OVERPRINTFILLOFF				: Str=_T("TAG_OVERPRINTFILLOFF"); break;
		case TAG_PRINTONALLPLATESON				: Str=_T("TAG_PRINTONALLPLATESON"); break;
		case TAG_PRINTONALLPLATESOFF			: Str=_T("TAG_PRINTONALLPLATESOFF"); break;

		case TAG_PRINTERSETTINGS				: Str=_T("TAG_PRINTERSETTINGS"); break;
		case TAG_IMAGESETTING					: Str=_T("TAG_IMAGESETTING"); break;
		case TAG_COLOURPLATE					: Str=_T("TAG_COLOURPLATE"); break;

		case TAG_PRINTMARKDEFAULT				: Str=_T("TAG_PRINTMARKDEFAULT"); break;
		case TAG_PRINTMARKCUSTOM				: Str=_T("TAG_PRINTMARKCUSTOM"); break;

		// Version 2 tags //////////////////////////////////////////////////////////////////
		case TAG_VARIABLEWIDTHFUNC				: Str=_T("TAG_VARIABLEWIDTHFUNC"); break;
		case TAG_VARIABLEWIDTHTABLE				: Str=_T("TAG_VARIABLEWIDTHTABLE"); break;
		case TAG_STROKETYPE						: Str=_T("TAG_STROKETYPE"); break;
		case TAG_STROKEDEFINITION				: Str=_T("TAG_STROKEDEFINITION"); break;
		case TAG_STROKEAIRBRUSH					: Str=_T("TAG_STROKEAIRBRUSH"); break;

		case TAG_PATHREF_IDENTICAL				: Str = _T("TAG_PATHREF_IDENTICAL"); break;
		case TAG_WEBADDRESS						: Str = _T("TAG_WEBADDRESS"); break;
		case TAG_WEBADDRESS_BOUNDINGBOX			: Str = _T("TAG_WEBADDRESS_BOUNDINGBOX"); break;

		case TAG_EXPORT_HINT					: Str = _T("TAG_EXPORT_HINT"); break;

		case TAG_WIZOP							: Str = _T("TAG_WIZOP"); break;
		case TAG_WIZOP_STYLE					: Str = _T("TAG_WIZOP_STYLE"); break;
		case TAG_WIZOP_STYLEREF					: Str = _T("TAG_WIZOP_STYLEREF"); break;

		case TAG_LAYER_FRAMEPROPS				: Str = _T("TAG_LAYER_FRAMEPROPS"); break;
		case TAG_SPREAD_ANIMPROPS				: Str = _T("TAG_SPREAD_ANIMPROPS"); break;

		case TAG_SHADOWCONTROLLER				: Str = _T("TAG_SHADOWCONTROLLER"); break;
		case TAG_SHADOW							: Str = _T("TAG_SHADOW"); break;

		case TAG_SETSENTINEL					: Str = _T("TAG_SETSENTINEL"); break;
		case TAG_SETPROPERTY					: Str = _T("TAG_SETPROPERTY"); break;

		case TAG_NAMEGAL_DOCCOMP				: Str = _T("TAG_NAMEGAL_DOCCOMP"); break;

		case TAG_XPE_BITMAP_PROPERTIES			: Str = _T("TAG_XPE_BITMAP_PROPERTIES"); break;

		case TAG_DEFINEBITMAP_XPE				: Str = _T("TAG_DEFINEBITMAP_XPE"); break;

		case TAG_CURRENTATTRIBUTES				: Str = _T("TAG_CURRENTATTRIBUTES"); break;

		case TAG_CURRENTATTRIBUTEBOUNDS			: Str = _T("TAG_CURRENTATTRIBUTEBOUNDS"); break;

		case TAG_LIVE_EFFECT					: Str = _T("TAG_LIVE_EFFECT"); break;
		case TAG_LOCKED_EFFECT					: Str = _T("TAG_LOCKED_EFFECT"); break;
		case TAG_FEATHER_EFFECT					: Str = _T("TAG_FEATHER_EFFECT"); break;

		case TAG_BRUSHATTR						: Str = _T("TAG_BRUSHATTR"); break;
		case TAG_BRUSHDEFINITION				: Str = _T("TAG_BRUSHDEFINITION"); break;
		case TAG_BRUSHDATA						: Str = _T("TAG_BRUSHDATA"); break;
		case TAG_MOREBRUSHDATA					: Str = _T("TAG_MOREBRUSHDATA"); break;
		case TAG_MOREBRUSHATTR					: Str = _T("TAG_MOREBRUSHATTR"); break;
		case TAG_BITMAP_PROPERTIES				: Str = _T("TAG_BITMAP_PROPERTIES"); break;
		case TAG_EVENMOREBRUSHDATA				: Str = _T("TAG_EVENMOREBRUSHDATA"); break;
		case TAG_EVENMOREBRUSHATTR				: Str = _T("TAG_EVENMOREBRUSHATTR"); break;
		case TAG_TIMESTAMPBRUSHDATA				: Str = _T("TAG_TIMESTAMPBRUSHDATA"); break;
		case TAG_BRUSHPRESSUREINFO				: Str = _T("TAG_BRUSHPRESSUREINFO"); break;
		case TAG_BRUSHPRESSUREDATA				: Str = _T("TAG_BRUSHPRESSUREDATA"); break;
		case TAG_BRUSHATTRPRESSUREINFO			: Str = _T("TAG_BRUSHATTRPRESSUREINFO"); break;
		case TAG_BRUSHCOLOURDATA				: Str = _T("TAG_BRUSHCOLOURDATA"); break;
		case TAG_BRUSHPRESSURESAMPLEDATA		: Str = _T("TAG_BRUSHPRESSURESAMPLEDATA"); break;
		case TAG_BRUSHTIMESAMPLEDATA			: Str = _T("TAG_BRUSHTIMESAMPLEDATA"); break;
		case TAG_BRUSHATTRFILLFLAGS				: Str = _T("TAG_BRUSHATTRFILLFLAGS"); break;
		case TAG_BRUSHTRANSPINFO				: Str = _T("TAG_BRUSHTRANSPINFO"); break;
		case TAG_BRUSHATTRTRANSPINFO			: Str = _T("TAG_BRUSHATTRTRANSPINFO"); break;

		case TAG_DOCUMENTNUDGE					: Str = _T("TAG_DOCUMENTNUDGE"); break;
		case TAG_DOCUMENTBITMAPSMOOTHING		: Str = _T("TAG_DOCUMENTBITMAPSMOOTHING"); break;
		case TAG_DUPLICATIONOFFSET				: Str = _T("TAG_DUPLICATIONOFFSET"); break;

		case TAG_FEATHER						: Str = _T("TAG_FEATHER"); break;
		case TAG_BARPROPERTY					: Str = _T("TAG_BARPROPERTY"); break;

		case TAG_COMPOUNDRENDER					: Str = _T("TAG_COMPOUNDRENDER"); break;
		case TAG_OBJECTBOUNDS					: Str = _T("TAG_OBJECTBOUNDS"); break;

		default:
		{
			TCHAR s[256];
			camSprintf(s, _T("*%d"), Tag);
			Str = s;
		}
		break;
	}
}
#endif

/********************************************************************************************

>	virtual void CXaraFileRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This is the base class record description generator.
				Adds the Tag & Size items to the string, so derived classes should call the base class version
				first.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void CXaraFileRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	String_256 TagText;
	GetTagText(pRecord->GetTag(),TagText);
	TCHAR* pTagText = TagText;

	TCHAR s[256];
	camSprintf(s, _T("Record Number = %d\r\nTag = %d (%s)\r\nSize = %d\r\n\r\n"), pRecord->GetRecordNumber(),pRecord->GetTag(),pTagText,pRecord->GetSize());
	(*pStr) += s;
}
#endif

/********************************************************************************************

>	virtual void CXaraFileRecordHandler::GetRecordInfo(CXaraFileRecord *pRecord, StringBase *pStr)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/07/96
	Inputs:		pRecord	- CXaraFileRecord to read information from
				pStr	- StringBase to write information to
	Returns:	-
	Purpose:	Gathers basic information about the record, tag number, record number and
				record size and writes it to pStr.

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void CXaraFileRecordHandler::GetRecordInfo(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	String_256 TagText;
	GetTagText(pCXaraFileRecord->GetTag(),TagText);
	TCHAR* pTagText = TagText;

	TCHAR s[256];

	camSprintf(s, _T("Record Number = %d\r\n"), pCXaraFileRecord->GetRecordNumber());
	(*pStr) += s;
	camSprintf(s, _T("Tag = %d (%s)\r\n"), pCXaraFileRecord->GetTag(),pTagText);
	(*pStr) += s;
	camSprintf(s, _T("Size = %d\r\n\r\n"), pCXaraFileRecord->GetSize());
	(*pStr) += s;
}
#endif

/********************************************************************************************

>	virtual void CXaraFileRecordHandler::DescribePath(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This reads data from the record, assuming that the record contains a standard path
				definition.

				A description of the path is appended to pStr;
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void CXaraFileRecordHandler::DescribePath(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	TCHAR s[256];

	INT32 NumCoords;
	pRecord->ReadINT32(&NumCoords);
	camSprintf(s,_T("Number of Coords = %d\r\n\r\n"),NumCoords);
	(*pStr) += s;

	if (NumCoords < 1)
		return;

	INT32 i;

	PathVerb* pVerb  = (PathVerb*)CCMalloc(NumCoords*sizeof(PathVerb));
	DocCoord* pCoord = (DocCoord*)CCMalloc(NumCoords*sizeof(DocCoord));

	if (pVerb == NULL || pCoord == NULL)
		return;

	for (i=0;i<NumCoords;i++)
		pRecord->ReadBYTE(pVerb+i);

	for (i=0;i<NumCoords;i++)
		pRecord->ReadCoord(pCoord+i);

	(*pStr) += _T("Num\tVerb\tX Coord\tY Coord\r\n");

	for (i=0;i<NumCoords;i++)
	{
		camSprintf(s, _T("%d\t%d\t%d\t%d\r\n"),i,pVerb[i],pCoord[i].x,pCoord[i].y);
		(*pStr) += s;
	}

	CCFree(pVerb);
	CCFree(pCoord);
}
#endif // _DEBUG

/********************************************************************************************

>	virtual void CXaraFileRecordHandler::DescribePathRelative(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This reads data from the record, assuming that the record contains a standard path
				definition in relative coord form.

				A description of the path is appended to pStr;
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void CXaraFileRecordHandler::DescribePathRelative(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	TCHAR s[256];

	INT32 NumCoords = INT32(pRecord->GetSize())/(sizeof(BYTE)+sizeof(INT32)+sizeof(INT32));
	camSprintf(s, _T("Number of Coords = %d\r\n\r\n"),NumCoords);
	(*pStr) += s;

	if (NumCoords < 1)
		return;

	INT32 i;

	PathVerb* pVerb  = (PathVerb*)CCMalloc(NumCoords*sizeof(PathVerb));
	DocCoord* pCoord = (DocCoord*)CCMalloc(NumCoords*sizeof(DocCoord));

	if (pVerb == NULL || pCoord == NULL)
		return;

	(*pStr) += _T("Num\tVerb\tX Coord\tY Coord\tRel x\tRel y\r\n");

	DocCoord PrevCoord;

	for (i=0;i<NumCoords;i++)
	{
		pRecord->ReadBYTE(pVerb+i);

		if (i == 0)
		{
			pRecord->ReadCoordInterleaved(&PrevCoord);
			camSprintf(s, _T("%d\t%d\t%d\t%d\t%d\t%d\r\n"),i,pVerb[0],PrevCoord.x,PrevCoord.y,0,0);
			(*pStr) += s;
		}
		else
		{
			DocCoord TempCoord;
			pRecord->ReadCoordTransInterleaved(&TempCoord,0,0);

//			INT32 RelX,RelY;
//			pRecord->ReadINT32(&RelX);
//			pRecord->ReadINT32(&RelY);

			PrevCoord.x -= TempCoord.x;
			PrevCoord.y -= TempCoord.y;

			camSprintf(s,_T("%d\t%d\t%d\t%d\t%x\t%x\r\n"),i,pVerb[i],PrevCoord.x,PrevCoord.y,TempCoord.x,TempCoord.y);
			(*pStr) += s;
		}
	}

	CCFree(pVerb);
	CCFree(pCoord);
}
#endif // _DEBUG

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/********************************************************************************************

>	CamelotRecordHandler::CamelotRecordHandler()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		-
	Purpose:	Creates a camelot record handler object.
				These are used by CXaraFile for reading the file.
				Handlers handle records as they are read from a file.

				This is a special varient that contains Camelot-specific functionality
		
	Errors:		-
	SeeAlso:	Init()

********************************************************************************************/

CamelotRecordHandler::CamelotRecordHandler()
{
#if !defined(EXCLUDE_FROM_XARLIB)
	pBaseCamelotFilter = NULL;
#else
	m_pCXFile = NULL;
#endif
}

/********************************************************************************************

>	CamelotRecordHandler::~CamelotRecordHandler()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Purpose:	Default destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamelotRecordHandler::~CamelotRecordHandler()
{
}


/********************************************************************************************

>	virtual BOOL CamelotRecordHandler::Init(BaseCamelotFilter* pThisBaseCamelotFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pThisBaseCamelotFilter = ptr to the filter that will read the file.
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Initialises the handler.
	
				The base class forms a link between the handler and the filter class

				If you override this func, be sure to call the base class varient
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if !defined(EXCLUDE_FROM_XARLIB)
BOOL CamelotRecordHandler::Init(BaseCamelotFilter* pThisBaseCamelotFilter)
{
	ERROR2IF(pThisBaseCamelotFilter == NULL,FALSE,"pThisBaseCamelotFilter is NULL");

	pBaseCamelotFilter = pThisBaseCamelotFilter;

	return TRUE;
}
#else
BOOL CamelotRecordHandler::Init(CXaraFile* pCXFile)
{
	ERROR2IF(pCXFile == NULL,FALSE,"pCXFile is NULL");

	m_pCXFile = pCXFile;

	return TRUE;
}
#endif

/********************************************************************************************

>	virtual BOOL CamelotRecordHandler::IsStreamed(UINT32 Tag)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Inputs:		The tag of the record
	Returns:	TRUE if this is a streamed record
				FALSE otherwise
	Purpose:	Function to find out if the record is streamed or not.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotRecordHandler::IsStreamed(UINT32 Tag)
{
	// Baseclass returns False, i.e. not streamed
	return FALSE;
}

/********************************************************************************************

>	BOOL CamelotRecordHandler::SetCompression(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		The new compression state required, True for on, False for off
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression on or off on the underlying CCFile.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotRecordHandler::SetCompression(BOOL NewState)
{
#if !defined(EXCLUDE_FROM_XARLIB)
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"CamelotRecordHandler::SetCompression pBaseCamelotFilter is NULL");
	
	return pBaseCamelotFilter->SetCompression(NewState);
#else
	ERROR2IF(m_pCXFile == NULL,FALSE,"CamelotRecordHandler::SetCompression m_pCXFile is NULL");
	
	return m_pCXFile->SetCompression(NewState);
#endif
}

#if !defined(EXCLUDE_FROM_XARLIB)
/********************************************************************************************

>	virtual BOOL CamelotRecordHandler::GetDocColour(INT32 ColourRef,DocColour* pDocColour)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		ColourRef  = a reference to a colour defined earlier in the file
				pDocColour = ptr to a DocColour object
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	This dereferences a colour ref value, and creates a DocColour from it.
				The resultant DocColour is put into pDocColour.

				The ColourRef value is typically found in records that define a colour or colours.
				It references a colour defined earlier in the file.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotRecordHandler::GetDocColour(INT32 ColourRef,DocColour* pDocColour)
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	ERROR2IF(pDocColour == NULL,FALSE,"pDocColour param is NULL");

	ColourListComponent* pColComp = pBaseCamelotFilter->GetColourDocComponent();

	ERROR2IF(pColComp == NULL,FALSE,"No colour component when getting a colour");

	return (pColComp->GetDocColour(ColourRef,pDocColour));
}

/********************************************************************************************

>	KernelBitmap* CamelotRecordHandler::GetReadBitmapReference(INT32 BitmapRef)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		BitmapRef	= a reference to a colour defined earlier in the file
	Outputs:	-
	Returns:	pBitmap		= ptr to a KernelBitmap object
	Purpose:	This dereferences a bitmap ref value, and returns a pointer to a KernelBitmap
				from it.
				The resultant KernelBitmap can then be used by the caller e.g. in fills.

				The BitmapRef value is typically found in records that define a bitmap.
				It references a bitmap defined earlier in the file.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

KernelBitmap* CamelotRecordHandler::GetReadBitmapReference(INT32 BitmapRef)
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");

	BitmapListComponent* pBmpComp = pBaseCamelotFilter->GetBitmapDocComponent();

	ERROR2IF(pBmpComp == NULL,FALSE,"No bitmap component when getting a bitmap");

	return (pBmpComp->GetReadBitmapReference(BitmapRef));
}

//------------------------------------------------------------------------------------------
// The following functions map onto functions provided by BaseCamelotFilter
//
// For more details, see the headers of the BaseCamelotFilter func equivalents

ColourListComponent* CamelotRecordHandler::GetColourDocComponent()
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->GetColourDocComponent());
}

BitmapListComponent* CamelotRecordHandler::GetBitmapDocComponent()
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->GetBitmapDocComponent());
}

UnitListComponent* CamelotRecordHandler::GetUnitDocComponent()
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->GetUnitDocComponent());
}

DocInfoComponent* CamelotRecordHandler::GetDocInfoComponent()
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->GetDocInfoComponent());
}

PrintComponent* CamelotRecordHandler::GetPrintComponent()
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->GetPrintComponent());
}

FontComponent* CamelotRecordHandler::GetFontComponent()
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->GetFontComponent());
}

BOOL CamelotRecordHandler::IsImportingAtPosition()
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->IsImportingAtPosition());
}

BOOL CamelotRecordHandler::IsImporting()
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->IsImporting());
}

BOOL CamelotRecordHandler::InsertNode(Node* pNode)
{
	ERROR2IF(pBaseCamelotFilter == NULL,FALSE,"pBaseCamelotFilter is NULL");
	return (pBaseCamelotFilter->InsertNode(pNode));
}

void CamelotRecordHandler::EndOfFile()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->EndOfFile();
}

void CamelotRecordHandler::SetTotalNumBytesToRead(UINT32 s)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetTotalNumBytesToRead(s);
}

void CamelotRecordHandler::IncProgressBarCount(UINT32 n)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->IncProgressBarCount(n);
}

void CamelotRecordHandler::SetTotalProgressBarCount(UINT32 n)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetTotalProgressBarCount(n);
}

BOOL CamelotRecordHandler::IncInsertLevel()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->IncInsertLevel();

	return FALSE;
}

BOOL CamelotRecordHandler::DecInsertLevel()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->DecInsertLevel();

	return FALSE;
}

Node* CamelotRecordHandler::GetInsertContextNode()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetInsertContextNode();

	return NULL;
}

void CamelotRecordHandler::SetInsertContextNode(Node* pNode)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetInsertContextNode(pNode);
}

void CamelotRecordHandler::InsertNextNodeAsChild()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->InsertNextNodeAsChild();
}

void CamelotRecordHandler::InsertNextNodeAsSibling()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->InsertNextNodeAsSibling();
}


InsertTreeContext *CamelotRecordHandler::GetInsertContext(void)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return(pBaseCamelotFilter->GetInsertContext());

	return(NULL);
}

void CamelotRecordHandler::RestoreInsertContext(InsertTreeContext *pOldState)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->RestoreInsertContext(pOldState);
}


Document* CamelotRecordHandler::GetDocument()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetDocument();

	return NULL;
}

Spread* CamelotRecordHandler::GetSpread()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetSpread();

	return NULL;
}

Document* CamelotRecordHandler::GetLastDocumentInserted()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetLastDocumentInserted();

	return NULL;
}

Chapter* CamelotRecordHandler::GetLastChapterInserted()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetLastChapterInserted();

	return NULL;
}

Spread* CamelotRecordHandler::GetLastSpreadInserted()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetLastSpreadInserted();

	return NULL;
}

Layer* CamelotRecordHandler::GetLastLayerInserted()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetLastLayerInserted();

	return NULL;
}

NodePath* CamelotRecordHandler::GetLastNodePathInserted()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetLastNodePathInserted();

	return NULL;
}

void CamelotRecordHandler::SetLastDocumentInserted(Document* pNode)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetLastDocumentInserted(pNode);
}

void CamelotRecordHandler::SetLastChapterInserted(Chapter* pNode)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetLastChapterInserted(pNode);
}

void CamelotRecordHandler::SetLastSpreadInserted(Spread* pNode)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetLastSpreadInserted(pNode);
}

void CamelotRecordHandler::SetLastLayerInserted(Layer* pNode)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetLastLayerInserted(pNode);
}

void CamelotRecordHandler::SetLastNodePathInserted(NodePath* pNode)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetLastNodePathInserted(pNode);
}

Layer * CamelotRecordHandler::GetFirstImportedLayer()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetFirstImportedLayer();
	return NULL;
}

void CamelotRecordHandler::SetFirstImportedLayer(Layer* pNode)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetFirstImportedLayer(pNode);
}

UINT32 CamelotRecordHandler::GetDocumentInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetDocumentInsertedCount();

	return 0;
}

UINT32 CamelotRecordHandler::GetChapterInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetChapterInsertedCount();

	return 0;
}

UINT32 CamelotRecordHandler::GetSpreadInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetSpreadInsertedCount();

	return 0;
}

UINT32 CamelotRecordHandler::GetLayerInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetLayerInsertedCount();

	return 0;
}

UINT32 CamelotRecordHandler::GetSetSentinelInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetSetSentinelInsertedCount();

	return 0;
}


void CamelotRecordHandler::IncDocumentInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->IncDocumentInsertedCount();
}

void CamelotRecordHandler::IncChapterInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->IncChapterInsertedCount();
}

void CamelotRecordHandler::IncSpreadInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->IncSpreadInsertedCount();
}

void CamelotRecordHandler::IncLayerInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->IncLayerInsertedCount();
}

void CamelotRecordHandler::IncSetSentinelInsertedCount()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->IncSetSentinelInsertedCount();
}

void CamelotRecordHandler::SetCoordOrigin(const DocCoord& Origin)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetCoordOrigin(Origin);
}

DocCoord CamelotRecordHandler::GetCoordOrigin()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetCoordOrigin();

	return DocCoord(0,0);
}

BOOL CamelotRecordHandler::AddTagDescription(TagDescriptionListItem* pItem)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->AddTagDescription(pItem);

	return FALSE;
}

BOOL CamelotRecordHandler::SetDocumentNudgeSize(UINT32 newVal)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->SetDocumentNudgeSize(newVal);

	return FALSE;
}

#endif


void CamelotRecordHandler::AddAtomicTag(AtomicTagListItem* pItem)
{
#if !defined(EXCLUDE_FROM_XARLIB)
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->AddAtomicTag(pItem);
#else
	ERROR3IF(m_pCXFile == NULL,"m_pCXFile is NULL");
	if (m_pCXFile != NULL)
		m_pCXFile->AddAtomicTag(pItem);
#endif
}

void CamelotRecordHandler::AddEssentialTag(EssentialTagListItem* pItem)
{
#if !defined(EXCLUDE_FROM_XARLIB)
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->AddEssentialTag(pItem);
#else
	ERROR3IF(m_pCXFile == NULL,"m_pCXFile is NULL");
	if (m_pCXFile != NULL)
		m_pCXFile->AddEssentialTag(pItem);
#endif
}

BOOL CamelotRecordHandler::IsTagInAtomicList(UINT32 Tag)
{
#if !defined(EXCLUDE_FROM_XARLIB)
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->IsTagInAtomicList(Tag);
#else
	ERROR3IF(m_pCXFile == NULL,"m_pCXFile is NULL");
	if (m_pCXFile != NULL)
		m_pCXFile->IsTagInAtomicList(Tag);
#endif

	return FALSE;
}

BOOL CamelotRecordHandler::IsTagInEssentialList(UINT32 Tag)
{
#if !defined(EXCLUDE_FROM_XARLIB)
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->IsTagInEssentialList(Tag);
#else
	ERROR3IF(m_pCXFile == NULL,"m_pCXFile is NULL");
	if (m_pCXFile != NULL)
		return m_pCXFile->IsTagInEssentialList(Tag);
#endif
	return FALSE;
}


#if !defined(EXCLUDE_FROM_XARLIB)
BOOL CamelotRecordHandler::UnrecognisedTag(UINT32 Tag)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->UnrecognisedTag(Tag);

	return FALSE;
}

TagDescriptionListItem* CamelotRecordHandler::GetTagDescription(UINT32 Tag)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetTagDescription(Tag);

	return NULL;
}

BOOL CamelotRecordHandler::SetPreCompression(UINT32 Flags)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->SetPreCompression(Flags);

	return FALSE;
}

UINT32 CamelotRecordHandler::GetPreCompression()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->GetPreCompression();

	return 0;
}

void CamelotRecordHandler::AddPathRecordRefToList(NodePath* pNodePath, UINT32 RecordNumber)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->AddPathRecordRefToList(pNodePath,RecordNumber);
}

UINT32 CamelotRecordHandler::FindPathRecordRefRecordNumber(NodePath* pNodePath)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->FindPathRecordRefRecordNumber(pNodePath);

	return 0;
}

NodePath* CamelotRecordHandler::FindPathRecordRefPath(UINT32 RecordNumber)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->FindPathRecordRefPath(RecordNumber);

	return NULL;
}

void CamelotRecordHandler::SetImportFileType(char* pFileType)
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		pBaseCamelotFilter->SetImportFileType(pFileType);
}

BOOL CamelotRecordHandler::IsOpeningMinimalWebFormat()
{
	ERROR3IF(pBaseCamelotFilter == NULL,"pBaseCamelotFilter is NULL");
	if (pBaseCamelotFilter != NULL)
		return pBaseCamelotFilter->IsOpeningMinimalWebFormat();

	return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------


/********************************************************************************************

>	StripSubTreeRecordHandler::StripSubTreeRecordHandler()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

StripSubTreeRecordHandler::StripSubTreeRecordHandler()
{
	StripSubTreeOff();
	LevelCounter = 0;
}


/********************************************************************************************

>	virtual BOOL StripSubTreeRecordHandler::BeginImport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Informs the record handler that importing is about to begin
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL StripSubTreeRecordHandler::BeginImport()
{
	StripSubTreeOff();
	LevelCounter = 0;

	return TRUE;
}

/********************************************************************************************

>	virtual UINT32* StripSubTreeRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* StripSubTreeRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL StripSubTreeRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		pCXaraFileRecord = ptr to the record to handle
	Returns:	TRUE if handled ok, FALSE otherwise
	Purpose:	The main record handling function
	SeeAlso:	-

********************************************************************************************/

BOOL StripSubTreeRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR3IF(!IsStripSubTreeOn(),"This should never be called if it's not stripping a subtree");
	if (!IsStripSubTreeOn())
		return TRUE;

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_UP:
			if (LevelCounter > 0)
				LevelCounter--;

			if (LevelCounter == 0)
				StripSubTreeOff();
			break;

		case TAG_DOWN:
			LevelCounter++;
			break;
	}

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL StripSubTreeRecordHandler::IsTagInList(UINT32 Tag)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		Tag = tag value of a record
	Returns:	TRUE if it's to be handled by this handler
				FALSE otherwise
	Purpose:	This decides whether the given Tag should be handled by the sub tree stripper or not.
	SeeAlso:	-

********************************************************************************************/

BOOL StripSubTreeRecordHandler::IsTagInList(UINT32 Tag)
{
	// If the stripper is not turned on, there's no point in trying
	if (!IsStripSubTreeOn())
		return FALSE;

	// If the next tag is not a down node (i.e. it's not the start of a sub-tree)
	// and the level counter is 0 (i.e. we're not in the depths of a sub-tree either)
	// then we should return FALSE & turn the stripper off
	if (Tag != TAG_DOWN && LevelCounter == 0)
	{
		StripSubTreeOff();
		return FALSE;
	}

	return TRUE;
}

