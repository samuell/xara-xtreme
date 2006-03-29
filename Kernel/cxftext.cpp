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

/********************************************************************************************

>	cxftext.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Purpose:	Exporting functions for text related objects

********************************************************************************************/

#include "camtypes.h"

#ifdef DO_EXPORT

#include "fontman.h"
#include "nodetext.h"
#include "nodetxtl.h"
#include "nodetxts.h"
#include "cxftags.h"
#include "cxfdefs.h"
#include "camfiltr.h"
#include "cxfrec.h"
#include "cxftext.h"
#include "keypress.h"
#include "vkextra.h"
#include "becomea.h"
#include "group.h"
#include "attrmap.h"
//#include "filtrres.h"
#include "app.h"


/***********************************************************************************************

>	class TextToOutlinesBecomeA: public BecomeA

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Purpose:	The class that handles the convertion of text to outlines.
				It receives all the paths passed back from the text story, and appends them 
				to a group object.

***********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
class TextToOutlinesBecomeA : public BecomeA
{
CC_DECLARE_MEMDUMP(TextToOutlinesBecomeA);
public:
	TextToOutlinesBecomeA(NodeGroup* pThisGroup) :	pGroup(pThisGroup), BecomeA(BECOMEA_PASSBACK,CC_RUNTIME_CLASS(NodePath),NULL) {};
	~TextToOutlinesBecomeA();

	// This function should be called when Reason == BECOMEA_PASSBACK 
	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap);

private:
	NodeGroup* pGroup;
};

CC_IMPLEMENT_MEMDUMP(TextToOutlinesBecomeA,BecomeA);
*/

//--------------------------------------------------------------------------------------

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	TextToOutlinesBecomeA::~TextToOutlinesBecomeA()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	The default destructor.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
TextToOutlinesBecomeA::~TextToOutlinesBecomeA()
{
}
*/
/********************************************************************************************

>	BOOL TextToOutlinesBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		pNewNode = the node created by the object being made into shapes
				pCreatedByNode = ptr to the node that created the new node
				pAttrMap = ptr to attrs (can be NULL)
	Outputs:	-
	Returns:	TRUE if the new path was used successfully, FALSE if op should be aborted
	Purpose:	This receives the paths from the selected objects and joins them to pJoinedPath.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
BOOL TextToOutlinesBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)
{
#ifdef DO_EXPORT
	ERROR2IF(pGroup == NULL,FALSE,"NULL group");
	ERROR2IF(pNewNode == NULL,FALSE,"NULL new path");

	// We only expect to get paths, so do a check
	if (!pNewNode->IsNodePath())
	{
		ERROR3("Received a node that's not a NodePath");
		return TRUE;
	}

	BOOL ok = TRUE;

	// Attach the next path as the last child of the group node
	pNewNode->AttachNode(pGroup,LASTCHILD);

	// Apply the supplied attrs, if there are any
	if (pAttrMap != NULL)
		ok = pNewNode->ApplyAttributes(pAttrMap,TRUE);
	else
	{
		// No supplied attrs, so we need to apply the applied attrs of the node that created the
		// path to the path itself, so that it will have the same appearence

		ok = FALSE;	// Assume failure (as we do when promised a bonus)

		// Create an attr map with the applied attrs of the "created by" node
		pAttrMap = CCAttrMap::MakeAppliedAttrMap(pCreatedByNode);
		if (pAttrMap != NULL)
		{
			ok = pNewNode->ApplyAttributes(pAttrMap,TRUE);		// Apply these attrs to the path
			// Delete the attr map
			delete pAttrMap;
			pAttrMap = NULL;
		}
	}

	return ok;
#else
	return FALSE;
#endif
}
*/
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

/********************************************************************************************

>	static BOOL CXaraFileTxtStory::WriteTextStoryAsOutlines(BaseCamelotFilter *pFilter, TextStory *pStory)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		pFilter = filter to write records to
				pStory  = the text story to convert to paths
	Returns:	TRUE if one or more records were written, FALSE otherwise
	Purpose:	Converts the text story into a group of paths, and then outputs that group
				to the filter
	Errors:		Errors via pFilter->GotError() if if fails to create the group of paths properly.

********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
BOOL CXaraFileTxtStory::WriteTextStoryAsOutlines(BaseCamelotFilter *pFilter, TextStory *pStory)
{
	ERROR3("Shouldn't call this,  Use BaseCamelotFilter::WriteNodeAsOutlines() instead");

#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok = TRUE;

	NodeGroup* pGroup = pFilter->FindTextStoryGroupRefGroup(pStory);

	if (pGroup == NULL)
	{
		// Create a group to hold all the outlines
		pGroup = new NodeGroup;
		ok = ok && (pGroup != NULL);

		// Create a BecomeA object that will receive all the outlines and attach them as children
		// of the group
		TextToOutlinesBecomeA* pBecomeA = new TextToOutlinesBecomeA(pGroup);
		ok = ok && (pBecomeA != NULL);

		// Collect all the outlines
		if (ok) ok = pStory->DoBecomeA(pBecomeA);

		// This code factors out the unnecessary attributes by placing the group in the tree, normalising
		// the attrs for all the paths in the group, then removing the group from the tree again
		if (ok)
		{
			// Attach it next to the text story
			pGroup->AttachNode(pStory,NEXT);

			// Ask all the child objects in the group to normalise the attributes
			Node* pNode = pGroup->FindFirstChild();
			while (pNode != NULL)
			{
				if (pNode->IsAnObject())
					((NodeRenderableInk*)pNode)->NormaliseAttributes();
				pNode = pNode->FindNext();
			}

			// Unlink the group from the tree
			pGroup->UnlinkNodeFromTree();
		}

		// Add it to the list of textstory/groups ref list items
		if (ok) pFilter->AddTextStoryGroupRefToList(pStory,pGroup);

		// Tidy up
		if (pBecomeA != NULL)
		{
			delete pBecomeA;
			pBecomeA = NULL;
		}
	}

	// Now write out the group to the file
	if (ok) ok = pFilter->WriteNodes(pGroup);

	// Report a general error if necessary
	if (!ok)
		pFilter->GotError(_R(IDS_EXPORT_ERROR_TEXTTOOUTLINE));

	return ok;
#else
	return FALSE;
#endif
}
*/
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

/********************************************************************************************

>	static BOOL CXaraFileTxtStory::IsGuaranteedFont(BaseCamelotFilter *pFilter,String_64* pFontName)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		pFilter = ptr to filter
				pFontName = ptr to a string containing the font name
	Returns:	TRUE if the given font name is in the list of guaranteed font names
				FALSE otherwise
	Purpose:	This scans the list of guaranteed font names.
				If the font name given matches one in the list, then TRUE is returned

	Errors:		-

********************************************************************************************/

BOOL CXaraFileTxtStory::IsGuaranteedFont(BaseCamelotFilter *pFilter,String_64* pFontName)
{
#ifdef DO_EXPORT
	ERROR2IF(pFontName == NULL,FALSE,"NULL font name ptr");

	String_256 GuaranteedFonts = pFilter->GetDontConvertTheseFontsToOutlinesString();
	String_64  GuaranteedFontName;
	String_64  Delimiters = _T(";");
	String_64  Str;

	TCHAR* pTchar = cc_lstrtok(GuaranteedFonts,Delimiters);
	while (pTchar != NULL)
	{
		Str = pTchar;
		if (*pFontName == Str)
			return TRUE;

		pTchar = cc_lstrtok(NULL,Delimiters);
	}

	return FALSE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	static BOOL CXaraFileTxtStory::SiblingAndChildTypefaceAttrsAllGuaranteedFonts(BaseCamelotFilter *pFilter,Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		pFilter = ptr to filter
				pNode = ptr to a node tree
	Returns:	TRUE if all sibling & child typeface attrs define guaranteed fonts
				FALSE otherwise
	Purpose:	This func does a sibling scan and depth-first traversal from the point of the
				given node, hunting for typeface attrs.

				If it finds one attr that does *not* define a guaranteed font, then FALSE is returned.

				Otherwise TRUE is returned

	Errors:		-

********************************************************************************************/

BOOL CXaraFileTxtStory::SiblingAndChildTypefaceAttrsAllGuaranteedFonts(BaseCamelotFilter *pFilter,Node* pNode)
{
#ifdef DO_EXPORT
	if (pNode == NULL)
		return TRUE;

	String_64 FontName;

	while (pNode != NULL)
	{
		// Only check the child nodes if the node is not a Caret or EOL node.
		if (!IS_A(pNode,CaretNode) && !IS_A(pNode,EOLNode))
		{
			if (!SiblingAndChildTypefaceAttrsAllGuaranteedFonts(pFilter,pNode->FindFirstChild()))
				return FALSE;
		}

		if (IS_A(pNode,AttrTxtFontTypeface))
		{
			AttrTxtFontTypeface* pTypefaceAttr = (AttrTxtFontTypeface*)pNode;

			if (FONTMANAGER->GetFontName(pTypefaceAttr->Value.HTypeface, FontName))
			{
				if (!IsGuaranteedFont(pFilter,&FontName))
					return FALSE;
			}
		}

		pNode = pNode->FindNext();
	}

	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	static BOOL CXaraFileTxtStory::MustConvertToOutlines(BaseCamelotFilter *pFilter, TextStory *pStory)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		pFilter = filter to write records to
				pStory  = the text story to convert to paths
	Returns:	TRUE if it has to be written out as paths, FALSE otherwise
	Purpose:	Checks to see if the given text story has to be written out as paths.

				Some text stories don't have to be written out as outlines, because they only
				contain fonts that are deemed to be guaranteed to be available.

				The default guaranteed font list is:
					Times New Roman
					Arial
					Courier New

	Errors:		-

********************************************************************************************/

BOOL CXaraFileTxtStory::MustConvertToOutlines(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	// Do a fast check - if there are no font names in the list, then don't bother doing any checking
	// because all fonts must have to be converted to outlines
	String_256 GuaranteedFonts = pFilter->GetDontConvertTheseFontsToOutlinesString();
	if (GuaranteedFonts.IsEmpty())
		return TRUE;

	BOOL Must = FALSE;

	String_64 FontName;

	CCAttrMap* pAttrMap = CCAttrMap::MakeAppliedAttrMap(pStory);

	if (pAttrMap != NULL)
	{
		void* pValue;
		pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrTxtFontTypeface),pValue);
		if (pValue != NULL)
		{
			AttrTxtFontTypeface* pTypefaceAttr = (AttrTxtFontTypeface*)pValue;

			if (IS_A(pTypefaceAttr,AttrTxtFontTypeface))
			{
				if (FONTMANAGER->GetFontName(pTypefaceAttr->Value.HTypeface, FontName))
				{
					if (!IsGuaranteedFont(pFilter,&FontName))
						Must = TRUE;
				}
			}
		}

		// Delete the attr map
		delete pAttrMap;
		pAttrMap = NULL;
	}

	if (!Must)
		Must = !SiblingAndChildTypefaceAttrsAllGuaranteedFonts(pFilter,pStory->FindFirstChild());

	return Must;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL CXaraFileTxtStory::WritePreChildrenWeb(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WritePreChildrenNative(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStory(BaseCamelotFilter *pFilter, TextStory *pStory);

	BOOL CXaraFileTxtStory::WriteTextStorySimple(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStoryComplex(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStorySimpleStartLeft(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStorySimpleStartRight(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStorySimpleEndLeft(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStorySimpleEndRight(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStoryComplexStartLeft(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStoryComplexStartRight(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStoryComplexEndLeft(BaseCamelotFilter *pFilter, TextStory *pStory);
	BOOL CXaraFileTxtStory::WriteTextStoryComplexEndRight(BaseCamelotFilter *pFilter, TextStory *pStory);
	
	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter		- new file format to save record to
				pStory		- TextStory node to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Exports text story records to the new file format

********************************************************************************************/

BOOL CXaraFileTxtStory::WritePreChildrenWeb(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	// If this story should not be exported for what ever reason (e.g. it contains no characters)
	// then just return FALSE (see also CanWriteChildren...)
	if (!pStory->OKToExport())
		return FALSE;

	// WEBSTER - markn 29/1/97
	// Part of the general form of the system used to convert text to outlines in v1.5
	if (pFilter->GetConvertTextToOutlines() && MustConvertToOutlines(pFilter,pStory))
		return pFilter->WriteNodeAsOutlines(pStory);

//	if (pFilter->GetConvertTextToOutlines() && MustConvertToOutlines(pFilter,pStory))
//		return WriteTextStoryAsOutlines(pFilter,pStory);

	return WriteTextStory(pFilter, pStory);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WritePreChildrenNative(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	// If this story should not be exported for what ever reason (e.g. it contains no characters)
	// then just return FALSE (see also CanWriteChildren...)
	if (!pStory->OKToExport())
		return FALSE;

	if (pFilter->IsCompactNativeFilter())
	{
		// WEBSTER - markn 29/1/97
		// Part of the general form of the system used to convert text to outlines in v1.5
		if (pFilter->GetConvertTextToOutlines() && MustConvertToOutlines(pFilter,pStory))
			return pFilter->WriteNodeAsOutlines(pStory);

		//	if (pFilter->GetConvertTextToOutlines() && MustConvertToOutlines(pFilter,pStory))
		//		return WriteTextStoryAsOutlines(pFilter,pStory);
	}

	return WriteTextStory(pFilter, pStory);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::CanWriteChildrenWeb(BaseCamelotFilter* pFilter,TextStory *pStory)
{
#ifdef DO_EXPORT
	if (!pStory->OKToExport())
		return FALSE;

	if (pFilter->GetConvertTextToOutlines() && MustConvertToOutlines(pFilter,pStory))
		return FALSE;

	return TRUE;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::CanWriteChildrenNative(BaseCamelotFilter* pFilter,TextStory *pStory)
{
#ifdef DO_EXPORT
	if (!pStory->OKToExport())
		return FALSE;

	if (pFilter->IsCompactNativeFilter())
	{
		if (pFilter->GetConvertTextToOutlines() && MustConvertToOutlines(pFilter,pStory))
			return FALSE;
	}

	return TRUE;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStory(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	// we break text stories into 10 different types, depending on how the text has been
	// transformed and how it is fitted to a path. using this information we jump to one
	// of the following functions to write the information out for us.

	if (pStory->GetTextPath() == NULL)
	{
		// this text story is _not_ fitted to a path, which makes a few things easier.
		// find out whether the associated matrix is the identity or not.
		Matrix MyMatrix = pStory->GetStoryMatrix();

		if (MyMatrix.IsTranslation(mEpsilon))
		{
			ok = WriteTextStorySimple(pFilter, pStory);
		}
		else
		{
			ok = WriteTextStoryComplex(pFilter, pStory);
		}
	}
	else
	{
		const INT32 COMPLEX = 0x1;
		const INT32 REVERSED = 0x2;
		const INT32 Y_REFLECTED = 0x4;

		INT32 TextStoryType = 0;

		{
			Matrix MyMatrix = pStory->GetStoryMatrix();
			ANGLE MyRotation = pStory->GetCharsRotation();
			ANGLE MyShear = pStory->GetCharsShear();

			if (MyMatrix.IsTranslation(mEpsilon)==FALSE
				|| MyRotation!=0
				|| MyShear!=0) TextStoryType |= COMPLEX;
		}

		if (pStory->IsTextOnPathReversed()) TextStoryType |= REVERSED;

		if (pStory->GetCharsScale() < 0) TextStoryType |= Y_REFLECTED;

		switch (TextStoryType)
		{
			case 0								: ok = WriteTextStorySimpleStartLeft(pFilter, pStory); break;
			case REVERSED						: ok = WriteTextStorySimpleEndRight(pFilter, pStory); break;
			case Y_REFLECTED					: ok = WriteTextStorySimpleStartRight(pFilter, pStory); break;
			case REVERSED|Y_REFLECTED			: ok = WriteTextStorySimpleEndLeft(pFilter, pStory); break;
			case COMPLEX						: ok = WriteTextStoryComplexStartLeft(pFilter, pStory); break;
			case COMPLEX|REVERSED				: ok = WriteTextStoryComplexEndRight(pFilter, pStory); break;
			case COMPLEX|Y_REFLECTED			: ok = WriteTextStoryComplexStartRight(pFilter, pStory); break;
			case COMPLEX|REVERSED|Y_REFLECTED	: ok = WriteTextStoryComplexEndLeft(pFilter, pStory); break;
			default								: ok = WriteTextStoryInvalid(pFilter, pStory); break;
		}
	}

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStorySimple(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	Matrix MyMatrix;
	DocCoord MyTranslation;

	MyMatrix = pStory->GetStoryMatrix();
	MyMatrix.GetTranslation(MyTranslation);

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_SIMPLE, TAG_TEXT_STORY_SIMPLE_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(MyTranslation);
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStoryComplex(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_COMPLEX, TAG_TEXT_STORY_COMPLEX_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteMatrix(pStory->GetStoryMatrix());
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStorySimpleStartLeft(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	Matrix MyMatrix;
	DocCoord MyTranslation;

	MyMatrix = pStory->GetStoryMatrix();
	MyMatrix.GetTranslation(MyTranslation);

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_SIMPLE_START_LEFT, TAG_TEXT_STORY_SIMPLE_START_LEFT_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteCoord(MyTranslation);
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStorySimpleStartRight(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	Matrix MyMatrix;
	DocCoord MyTranslation;

	MyMatrix = pStory->GetStoryMatrix();
	MyMatrix.GetTranslation(MyTranslation);

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_SIMPLE_START_RIGHT, TAG_TEXT_STORY_SIMPLE_START_RIGHT_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteCoord(MyTranslation);
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStorySimpleEndLeft(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	Matrix MyMatrix;
	DocCoord MyTranslation;

	MyMatrix = pStory->GetStoryMatrix();
	MyMatrix.GetTranslation(MyTranslation);

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_SIMPLE_END_LEFT, TAG_TEXT_STORY_SIMPLE_END_LEFT_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteCoord(MyTranslation);
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStorySimpleEndRight(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	Matrix MyMatrix;
	DocCoord MyTranslation;

	MyMatrix = pStory->GetStoryMatrix();
	MyMatrix.GetTranslation(MyTranslation);

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_SIMPLE_END_RIGHT, TAG_TEXT_STORY_SIMPLE_END_RIGHT_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteCoord(MyTranslation);
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStoryComplexStartLeft(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_COMPLEX_START_LEFT, TAG_TEXT_STORY_COMPLEX_START_LEFT_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteMatrix(pStory->GetStoryMatrix());
	if (ok) ok = Rec.WriteANGLE(pStory->GetCharsRotation());
	if (ok) ok = Rec.WriteANGLE(pStory->GetCharsShear());
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());

	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStoryComplexStartRight(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_COMPLEX_START_RIGHT, TAG_TEXT_STORY_COMPLEX_START_RIGHT_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteMatrix(pStory->GetStoryMatrix());
	if (ok) ok = Rec.WriteANGLE(pStory->GetCharsRotation());
	if (ok) ok = Rec.WriteANGLE(pStory->GetCharsShear());
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());

	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStoryComplexEndLeft(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_COMPLEX_END_LEFT, TAG_TEXT_STORY_COMPLEX_END_LEFT_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteMatrix(pStory->GetStoryMatrix());
	if (ok) ok = Rec.WriteANGLE(pStory->GetCharsRotation());
	if (ok) ok = Rec.WriteANGLE(pStory->GetCharsShear());
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());

	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStoryComplexEndRight(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_COMPLEX_END_RIGHT, TAG_TEXT_STORY_COMPLEX_END_RIGHT_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteMatrix(pStory->GetStoryMatrix());
	if (ok) ok = Rec.WriteANGLE(pStory->GetCharsRotation());
	if (ok) ok = Rec.WriteANGLE(pStory->GetCharsShear());
	if (ok) ok = Rec.WriteINT32 ((INT32) pStory->IsAutoKerning ());

	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStoryWordWrapInfo(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_WORD_WRAP_INFO, TAG_TEXT_STORY_WORD_WRAP_INFO_SIZE);

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(pStory->GetStoryWidth());
	if (ok) ok = Rec.WriteBYTE(BYTE(pStory->IsWordWrapping() != 0));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStoryIndentInfo(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	CamelotFileRecord Rec(pFilter, TAG_TEXT_STORY_INDENT_INFO, TAG_TEXT_STORY_INDENT_INFO_SIZE);

	MILLIPOINT LeftIndent  = pStory->GetLeftIndent();
	MILLIPOINT RightIndent = pStory->GetRightIndent();

	BOOL ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(LeftIndent);
	if (ok) ok = Rec.WriteINT32(RightIndent);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteTextStoryInvalid(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	ERROR3("Unable to determine type of text story to write out - help!");

	return FALSE;
#else
	return FALSE;
#endif
}

//--------------------------------

BOOL CXaraFileTxtStory::WriteBeginChildRecordsWeb(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	return WriteBeginChildRecordsNative(pFilter, pStory);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteBeginChildRecordsNative(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pStory==NULL, FALSE, "Parameter pFilter == NULL.");

	// First thing to do is write out the Down record
	BOOL ok = (pFilter->WriteZeroSizedRecord(TAG_DOWN));
	if (ok) ok = WriteTextStoryWordWrapInfo(pFilter,pStory);
	if (ok) ok = WriteTextStoryIndentInfo(pFilter,pStory);

	// Write out the bounds record
	if (ok && pFilter->GetBoundsWriteLevel() >= BWL_COMPOUND)
	{
		DocRect Rect = pStory->GetBoundingRect();

		CamelotFileRecord Rec(pFilter, TAG_OBJECTBOUNDS, TAG_OBJECTBOUNDS_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteCoord(Rect.lo);
		if (ok) ok = Rec.WriteCoord(Rect.hi);
		if (ok) ok = pFilter->Write(&Rec);
	}

	// Write out the bounds record
	if (ok && pFilter->GetBoundsWriteLevel() >= BWL_COMPOUND)
	{
		DocRect Rect = pStory->GetBoundingRect();

		CamelotFileRecord Rec(pFilter, TAG_OBJECTBOUNDS, TAG_OBJECTBOUNDS_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteCoord(Rect.lo);
		if (ok) ok = Rec.WriteCoord(Rect.hi);
		if (ok) ok = pFilter->Write(&Rec);
	}

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteEndChildRecordsWeb(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	return WriteEndChildRecordsNative(pFilter, pStory);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtStory::WriteEndChildRecordsNative(BaseCamelotFilter *pFilter, TextStory *pStory)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	return pFilter->WriteZeroSizedRecord(TAG_UP);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	const double CXaraFileTextStory::mEpsilon

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	15/07/96
	Purpose:	Epsilon value used for checking matrix type

********************************************************************************************/

const double CXaraFileTxtStory::mEpsilon = 0.000016;

/********************************************************************************************

>	BOOL CXaraFileTxtLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter, TextLine *pLine)
	BOOL CXaraFileTxtLine::WritePreChildrenNative(BaseCamelotFilter *pFilter, TextLine *pLine)
	BOOL CXaraFileTxtLine::WriteTextLine(BaseCamelotFilter *pFilter, TextLine *pLine)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter	- new file format filter to write record to
				pLine	- text line object to export
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Exports a text line object record to the new file format

********************************************************************************************/

BOOL CXaraFileTxtLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter, TextLine *pLine)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pLine==NULL, FALSE, "Parameter pLine==NULL");

	return WriteTextLine(pFilter, pLine);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtLine::WritePreChildrenNative(BaseCamelotFilter *pFilter, TextLine *pLine)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pLine==NULL, FALSE, "Parameter pLine==NULL");

	return WriteTextLine(pFilter, pLine);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtLine::WriteTextLine(BaseCamelotFilter *pFilter, TextLine *pLine)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pLine==NULL, FALSE, "Parameter pLine==NULL");

	// using our current plans, we don't actually need to write out text line objects, they're
	// explicit because of a newline in the character string

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_LINE, TAG_TEXT_LINE_SIZE);
	ok = Rec.Init();
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

//-----------------------------------------------------------------

BOOL CXaraFileTxtLine::WriteBeginChildRecordsWeb(BaseCamelotFilter *pFilter, TextLine *pLine)
{
#ifdef DO_EXPORT
	return WriteBeginChildRecordsNative(pFilter,pLine);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtLine::WriteBeginChildRecordsNative(BaseCamelotFilter *pFilter, TextLine *pLine)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pLine==NULL, FALSE, "Parameter pLine==NULL");

	// First thing to do is write out the Down record
	BOOL ok = (pFilter->WriteZeroSizedRecord(TAG_DOWN));

	// Calc dist between this line & the previous line
	MILLIPOINT DistFromPreviousLine = pLine->GetPosInStory();
	TextLine* pPrevLine = pLine->FindPrevLine();
	if (pPrevLine != NULL)
		DistFromPreviousLine -= pPrevLine->GetPosInStory();

	DocRect Rect = pLine->GetBoundingRect();

	CamelotFileRecord Rec(pFilter,TAG_TEXT_LINE_INFO,TAG_TEXT_LINE_INFO_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(Rect.Width());
	if (ok) ok = Rec.WriteINT32(Rect.Height());
	if (ok) ok = Rec.WriteINT32(DistFromPreviousLine);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtLine::WriteEndChildRecordsWeb(BaseCamelotFilter *pFilter, TextLine *pLine)
{
#ifdef DO_EXPORT
	return WriteEndChildRecordsNative(pFilter,pLine);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtLine::WriteEndChildRecordsNative(BaseCamelotFilter *pFilter, TextLine *pLine)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	return (pFilter->WriteZeroSizedRecord(TAG_UP));
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtChar::WritePreChildrenWeb(BaseCamelotFilter *pFilter, TextChar *pChar);
	BOOL CXaraFileTxtChar::WritePreChildrenNative(BaseCamelotFilter *pFilter, TextChar *pChar);
	BOOL CXaraFileTxtChar::WriteTextChar(BaseCamelotFilter *pFilter, TextChar *pChar);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter	- new file format filter to write the record to.
				pLine	- TextLine object to export
	Returns:	TRUE if a record has been written , FALSE otherwise
	Purpose:	Writes a text char record to the filter
	See Also:	<???>

********************************************************************************************/

BOOL CXaraFileTxtChar::WritePreChildrenWeb(BaseCamelotFilter *pFilter, TextChar *pChar)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pChar==NULL, FALSE, "Parameter pChar==NULL");

	return WriteTextChar(pFilter, pChar);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtChar::WritePreChildrenNative(BaseCamelotFilter *pFilter, TextChar *pChar)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pChar==NULL, FALSE, "Parameter pChar==NULL");

	return WriteTextChar(pFilter, pChar);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtChar::WriteTextChar(BaseCamelotFilter *pFilter, TextChar *pChar)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pChar==NULL, FALSE, "Parameter pChar==NULL");

	BOOL ok = TRUE;
	BOOL TryAString = TRUE;

	// This function will work out whether it has to save this char as an atomic TAG_TEXT_CHAR record,
	// or whether it can be saved as part of a TAG_TEXT_STRING record along with it's immediate
	// neighbours.

	// Already been written to the file? (I.e. was it included in a previous string?)
	// If so return now (this flag is reset in TextChar::WritePostChildenX )
	if (pChar->AlreadyWritten())
		return FALSE;

	// Start a string record up, and make the first char in the string this char
	CamelotFileRecord Rec(pFilter, TAG_TEXT_STRING, TAG_TEXT_STRING_SIZE);
	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteWCHAR(pChar->GetUnicodeValue());


	// In debug builds you can force text to be saved out as single chars by holding down the 'S' key
	#ifdef _DEBUG
	if (KeyPress::IsKeyPressed(CAMKEY(S)))
		TryAString = FALSE;
	#endif
	

	if (ok && TryAString)
	{
		// See how many child nodes the char's got, and also how many of these are children
		UINT32 ChildCount, ChildAttrCount;
		pChar->CountChildNodes(&ChildCount,&ChildAttrCount);

		// Only proceed if all the child nodes are attributes
		// (if not all the child nodes are attrs, then this is an unexpected state, so best not to proceed)
		if (ChildCount == ChildAttrCount)
		{
			BOOL CharWritten = TRUE;				// We have successfully written the previous char
			Node* pNextNode = pChar->FindNext();	// Find the next node

			// The While loop will continue if all the following are true:
			//		The current char has been written successfully
			//		There is a node next to the current char (i.e. not the last not in sibling list)
			//		The next node is a TextChar node.

			while (ok && CharWritten && pNextNode != NULL && pNextNode->IsATextChar())
			{
				CharWritten = FALSE;							// The next char has not been written out yet
				TextChar* pNextChar = (TextChar*)pNextNode;		// The next node is a TextChar, so safe to cast

				// Count the number of child nodes, and num child attrs
				UINT32 NextChildCount, NextChildAttrCount;
				pNextChar->CountChildNodes(&NextChildCount,&NextChildAttrCount);

				// If the next node only has child attrs  (NextChildCount == NextChildAttrCount)
				// and has the same number of child atrrs as pChar (NextChildAttrCount == ChildAttrCount)
				// then proceed
				if (NextChildCount == NextChildAttrCount && NextChildAttrCount == ChildAttrCount)
				{
					// are the child attrs of the next char identical to the first char in the string?
					// I.e. do both of these characters look the same?
					if (pChar->AreChildAttrsIdentical(pNextChar))
					{
						// If so, add the next char's code to the string record
						CharWritten = ok = Rec.WriteWCHAR(pNextChar->GetUnicodeValue());

						// Make sure the newly added char doesn't get written out later
						pNextChar->SetAlreadyWritten(CharWritten);
					}
				}

				// Find the next node
				pNextNode = pNextChar->FindNext();				
			}
		}
	}

	// If only one character ended up in the string, write out a TAG_TEXT_CHAR record instead of a string rec.
	// There's no important reason for this.  I've done it so it's clear when looking at the file
	// what has happened to text when it's been exported
	// Also, text char records are fixed-sized records, allowing a precompressor to drop the size field if it wants.
	if (ok && Rec.GetSize() == 2)
	{
				ok = Rec.Reinit(TAG_TEXT_CHAR,TAG_TEXT_CHAR_SIZE);
		if (ok) ok = Rec.WriteWCHAR(pChar->GetUnicodeValue());
	}

	if (ok) ok = (pFilter->Write(&Rec) != 0);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtKern::WritePreChildrenWeb(BaseCamelotFilter *pFilter, KernCode *pKern);
	BOOL CXaraFileTxtKern::WritePreChildrenNative(BaseCamelotFilter *pFilter, KernCode *pKern);
	BOOL CXaraFileTxtKern::WriteTextKern(BaseCamelotFilter *pFilter, KernCode *pKern);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter	- New file format filter to write the record to
				pKern	- kern code node to export
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes a kern code record to the filter

********************************************************************************************/

BOOL CXaraFileTxtKern::WritePreChildrenWeb(BaseCamelotFilter *pFilter, KernCode *pKern)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pKern==NULL, FALSE, "Parameter pKern==NULL");

	return WriteTextKern(pFilter, pKern);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtKern::WritePreChildrenNative(BaseCamelotFilter *pFilter, KernCode *pKern)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pKern==NULL, FALSE, "Parameter pKern==NULL");

	return WriteTextKern(pFilter, pKern);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtKern::WriteTextKern(BaseCamelotFilter *pFilter, KernCode *pKern)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pKern==NULL, FALSE, "Parameter pChar==NULL");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_KERN, TAG_TEXT_KERN_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoordTrans(pKern->GetValue(),0,0);		// Kerns are relative, so ensure they don't get translated when output
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtCaret::WritePreChildrenWeb(BaseCamelotFilter *pFilter, CaretNode *pCaret);
	BOOL CXaraFileTxtCaret::WritePreChildrenNative(BaseCamelotFilter *pFilter, CaretNode *pCaret);
	BOOL CXaraFileTxtCaret::WriteTextCaret(BaseCamelotFilter *pFilter, CaretNode *pCaret);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter	- New file format filter to write record to
				pCaret	- Caret node to write out
	Returns:	TRUE if record written, FALSE otherwise
	Purpose:	Writes a caret record to the filter

********************************************************************************************/

BOOL CXaraFileTxtCaret::WritePreChildrenWeb(BaseCamelotFilter *pFilter, CaretNode *pCaret)
{
/*
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pCaret==NULL, FALSE, "Parameter pCaret==NULL");

	return WriteTextCaret(pFilter, pCaret);
*/
	return FALSE;
}

BOOL CXaraFileTxtCaret::WritePreChildrenNative(BaseCamelotFilter *pFilter, CaretNode *pCaret)
{
/*
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pCaret==NULL, FALSE, "Parameter pCaret==NULL");

	return WriteTextCaret(pFilter, pCaret);
*/
	return FALSE;
}

BOOL CXaraFileTxtCaret::WriteTextCaret(BaseCamelotFilter *pFilter, CaretNode *pCaret)
{
/*
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pCaret==NULL, FALSE, "Parameter pCaret==NULL");

	BOOL ok;
	
	CamelotFileRecord Rec(pFilter, TAG_TEXT_CARET, TAG_TEXT_CARET_SIZE);
	ok = Rec.Init();
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
*/
	return FALSE;
}

/********************************************************************************************

>	BOOL CXaraFileTxtEOL::WritePreChildrenWeb(BaseCamelotFilter *pFilter, EOLNode *pEOL)
	BOOL CXaraFileTxtEOL::WritePreChildrenNative(BaseCamelotFilter *pFilter, EOLNode *pEOL)
	BOOL CXaraFileTxtEOL::WriteTextEOL(BaseCamelotFilter *pFilter, EOLNode *pEOL)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter	- New file format filter to use
				pEOL	- EOL node to write to filter
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes an EOL Node record to the filter

********************************************************************************************/

BOOL CXaraFileTxtEOL::WritePreChildrenWeb(BaseCamelotFilter *pFilter, EOLNode *pEOL)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pEOL==NULL, FALSE, "Parameter pEOL==NULL");

	return WriteTextEOL(pFilter, pEOL);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtEOL::WritePreChildrenNative(BaseCamelotFilter *pFilter, EOLNode *pEOL)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pEOL==NULL, FALSE, "Parameter pEOL==NULL");

	return WriteTextEOL(pFilter, pEOL);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtEOL::WriteTextEOL(BaseCamelotFilter *pFilter, EOLNode *pEOL)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter==NULL");
	ERROR2IF(pEOL==NULL, FALSE, "Parameter pEOL==NULL");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_EOL, TAG_TEXT_EOL_SIZE);
	ok = Rec.Init();
	if (ok) ok = pFilter->Write(&Rec);

//	WCHAR MyWChar = '\n';
//	pFilter->StartRecord(TAG_TEXT_STRING, TAG_TEXT_STRING_SIZE);
//	if (ok) ok = pFilter->WriteWCHAR(MyWChar);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtBold::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtBold *pAttr);
	BOOL CXaraFileTxtBold::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtBold *pAttr);
	BOOL CXaraFileTxtBold::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtBold *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text bold attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtBold::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtBold *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtBold::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtBold *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtBold::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtBold *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	if (pAttr->Value.BoldOn != FALSE)
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_BOLD_ON, TAG_TEXT_BOLD_ON_SIZE);
		ok = Rec.Init();
		if (ok) ok = pFilter->Write(&Rec);
	}
	else
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_BOLD_OFF, TAG_TEXT_BOLD_OFF_SIZE);
		ok = Rec.Init();
		if (ok) ok = pFilter->Write(&Rec);
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtFontTypeface::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtFontTypeface *pAttr);
	BOOL CXaraFileTxtFontTypeface::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtFontTypeface *pAttr);
	BOOL CXaraFileTxtFontTypeface::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtFontTypeface *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text font typeface attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtFontTypeface::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtFontTypeface *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtFontTypeface::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtFontTypeface *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtFontTypeface::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtFontTypeface *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	INT32 FontRecordRef = pFilter->WriteFontDefinition(pAttr->Value.HTypeface, pAttr->Value.IsBold, pAttr->Value.IsItalic);

	if (FontRecordRef == 0)
	{
		ok = FALSE;
	}
	else
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_FONT_TYPEFACE, TAG_TEXT_FONT_TYPEFACE_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteReference(FontRecordRef);
		if (ok) ok = pFilter->Write(&Rec);
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtItalic::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtItalic *pAttr);
	BOOL CXaraFileTxtItalic::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtItalic *pAttr);
	BOOL CXaraFileTxtItalic::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtItalic *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text italic attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtItalic::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtItalic *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtItalic::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtItalic *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtItalic::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtItalic *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	if (pAttr->Value.ItalicOn != FALSE)
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_ITALIC_ON, TAG_TEXT_ITALIC_ON_SIZE);
		ok = Rec.Init();
		if (ok) ok = pFilter->Write(&Rec);
	}
	else
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_ITALIC_OFF, TAG_TEXT_ITALIC_OFF_SIZE);
		ok = Rec.Init();
		if (ok) ok = pFilter->Write(&Rec);
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtUnderline::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtUnderline *pAttr);
	BOOL CXaraFileTxtUnderline::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtUnderline *pAttr);
	BOOL CXaraFileTxtUnderline::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtUnderline *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text underline attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtUnderline::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtUnderline *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtUnderline::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtUnderline *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtUnderline::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtUnderline *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

//	BOOL ok;
//
//	if (pAttr->Value.Underlined != FALSE)
//	{
//		CamelotFileRecord Rec(pFilter, TAG_TEXT_UNDERLINE_ON, TAG_TEXT_UNDERLINE_ON_SIZE);
//		ok = Rec.Init();
//		if (ok) ok = pFilter->Write(&Rec);
//	}
//	else
//	{
//		CamelotFileRecord Rec(pFilter, TAG_TEXT_UNDERLINE_OFF, TAG_TEXT_UNDERLINE_OFF_SIZE);
//		ok = Rec.Init();
//		if (ok) ok = pFilter->Write(&Rec);
//	}
//
//	return ok;

	// we don't support underline attributes yet; they can exist in the tree but
	// aren't used by anybody. hence we don't save anything out.

	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtAspectRatio::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtAspectRatio *pAttr);
	BOOL CXaraFileTxtAspectRatio::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtAspectRatio *pAttr);
	BOOL CXaraFileTxtAspectRatio::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtAspectRatio *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text aspect ratio attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtAspectRatio::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtAspectRatio *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtAspectRatio::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtAspectRatio *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtAspectRatio::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtAspectRatio *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_ASPECT_RATIO, TAG_TEXT_ASPECT_RATIO_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteFIXED16(pAttr->Value.AspectRatio);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtJustification::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtJustification *pAttr);
	BOOL CXaraFileTxtJustification::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtJustification *pAttr);
	BOOL CXaraFileTxtJustification::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtJustification *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text justification attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtJustification::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtJustification *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtJustification::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtJustification *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtJustification::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtJustification *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	switch (pAttr->Value.justification)
	{
		case JLEFT :
			{
				CamelotFileRecord Rec(pFilter, TAG_TEXT_JUSTIFICATION_LEFT, TAG_TEXT_JUSTIFICATION_LEFT_SIZE);
				ok = Rec.Init();
				if (ok) ok = pFilter->Write(&Rec);
			}
			break;
		case JRIGHT :
			{
				CamelotFileRecord Rec(pFilter, TAG_TEXT_JUSTIFICATION_RIGHT, TAG_TEXT_JUSTIFICATION_RIGHT_SIZE);
				ok = Rec.Init();
				if (ok) ok = pFilter->Write(&Rec);
			}
			break;
		case JCENTRE :
			{
				CamelotFileRecord Rec(pFilter, TAG_TEXT_JUSTIFICATION_CENTRE, TAG_TEXT_JUSTIFICATION_CENTRE_SIZE);
				ok = Rec.Init();
				if (ok) ok = pFilter->Write(&Rec);
			}
			break;
		case JFULL :
			{
				CamelotFileRecord Rec(pFilter, TAG_TEXT_JUSTIFICATION_FULL, TAG_TEXT_JUSTIFICATION_FULL_SIZE);
				ok = Rec.Init();
				if (ok) ok = pFilter->Write(&Rec);
			}
			break;
		default :
			ERROR3("Unknown justification attribute.");
			ok = FALSE;
			break;
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtTracking::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtTracking *pAttr);
	BOOL CXaraFileTxtTracking::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtTracking *pAttr);
	BOOL CXaraFileTxtTracking::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtTracking *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text tracking attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtTracking::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtTracking *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtTracking::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtTracking *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtTracking::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtTracking *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_TRACKING, TAG_TEXT_TRACKING_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(pAttr->Value.Tracking);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtFontSize::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtFontSize *pAttr);
	BOOL CXaraFileTxtFontSize::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtFontSize *pAttr);
	BOOL CXaraFileTxtFontSize::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtFontSize *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text font size attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtFontSize::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtFontSize *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtFontSize::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtFontSize *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtFontSize::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtFontSize *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_FONT_SIZE, TAG_TEXT_FONT_SIZE_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(pAttr->Value.FontSize);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtScript::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtScript *pAttr);
	BOOL CXaraFileTxtScript::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtScript *pAttr);
	BOOL CXaraFileTxtScript::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtScript *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text script attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtScript::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtScript *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtScript::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtScript *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtScript::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtScript *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	if (pAttr->Value.Offset == 0 && pAttr->Value.Size == 1)
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_SCRIPT_OFF, TAG_TEXT_SCRIPT_OFF_SIZE);
		ok = Rec.Init();
		if (ok) ok = pFilter->Write(&Rec);
	}
	else if (pAttr->Value.Offset == FIXED16(Text_SuperScriptOffset)
				&& pAttr->Value.Size == FIXED16(Text_SuperScriptSize))
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_SUPERSCRIPT_ON, TAG_TEXT_SUPERSCRIPT_ON_SIZE);
		ok = Rec.Init();
		if (ok) ok = pFilter->Write(&Rec);
	}
	else if (pAttr->Value.Offset == FIXED16(Text_SubScriptOffset)
				&& pAttr->Value.Size == FIXED16(Text_SubScriptSize))
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_SUBSCRIPT_ON, TAG_TEXT_SUBSCRIPT_ON_SIZE);
		ok = Rec.Init();
		if (ok) ok = pFilter->Write(&Rec);
	}
	else
	{
		CamelotFileRecord Rec(pFilter, TAG_TEXT_SCRIPT_ON, TAG_TEXT_SCRIPT_ON_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteFIXED16(pAttr->Value.Offset);
		if (ok) ok = Rec.WriteFIXED16(pAttr->Value.Size);
		if (ok) ok = pFilter->Write(&Rec);
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtBaseLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtBaseLine *pAttr);
	BOOL CXaraFileTxtBaseLine::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtBaseLine *pAttr);
	BOOL CXaraFileTxtBaseLine::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtBaseLine *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text base line attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtBaseLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtBaseLine *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtBaseLine::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtBaseLine *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtBaseLine::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtBaseLine *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_TEXT_BASELINE, TAG_TEXT_BASELINE_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteINT32(pAttr->Value.Value);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileTxtLineSpace::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtLineSpace *pAttr);
	BOOL CXaraFileTxtLineSpace::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtLineSpace *pAttr);
	BOOL CXaraFileTxtLineSpace::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtLineSpace *pAttr);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter	- camelot filter to use
				pAttr	- attribute to save
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text line space attribute to the filter

********************************************************************************************/

BOOL CXaraFileTxtLineSpace::WritePreChildrenWeb(BaseCamelotFilter *pFilter, AttrTxtLineSpace *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtLineSpace::WritePreChildrenNative(BaseCamelotFilter *pFilter, AttrTxtLineSpace *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	return WritePreChildrenAux(pFilter, pAttr);
#else
	return FALSE;
#endif
}

BOOL CXaraFileTxtLineSpace::WritePreChildrenAux(BaseCamelotFilter *pFilter, AttrTxtLineSpace *pAttr)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");
	ERROR2IF(pAttr==NULL, FALSE, "Parameter pAttr == NULL.");

	BOOL ok;

	if (pAttr->Value.IsARatio())
	{
		// its a ratio linespace node
		CamelotFileRecord Rec(pFilter, TAG_TEXT_LINESPACE_RATIO, TAG_TEXT_LINESPACE_RATIO_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteFIXED16(pAttr->Value.Ratio);
		if (ok) ok = pFilter->Write(&Rec);
	}
	else
	{
		// its an absolute linespace node
		CamelotFileRecord Rec(pFilter, TAG_TEXT_LINESPACE_ABSOLUTE, TAG_TEXT_LINESPACE_ABSOLUTE_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteINT32(pAttr->Value.Value);
		if (ok) ok = pFilter->Write(&Rec);
	}

	return ok;
#else
	return FALSE;
#endif
}

#endif	// DO_EXPORT
