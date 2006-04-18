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
// Header for the document record handling classes for the v2 file format

#include "camtypes.h"
#include "rechdoc.h"

//#include <libintl.h>

//#include "selop.h"
#include "spread.h"
#include "page.h"
#include "layer.h"
#include "grid.h"
#include "chapter.h"
#include "cxfrec.h"
#include "cxftags.h"
#include "camfiltr.h"	// BaseCamelotFilter
#include "unitcomp.h"	// UnitListComponent
#include "guides.h"		// NodeGuideline

#include "ngsentry.h"	// NodeSetSentinel & NodeSetProperty
#include "cxfrech.h"	// TemplateAttribute
#include "attrval.h"
#include "userattr.h"
#include "tmpltatr.h"
#include "nodetxts.h"	// For CC_RUNTIME_CLASS(BaseTextClass)
#include "fillattr.h"

//#include "will2.h"
//#include "simon.h"		// _R(IDS_LAYER_DESCRS)
//#include "mario.h"		// for _R(IDE_NOMORE_MEMORY)

// global that is set when a bar property tag is imported to say how many bars there were beforehand
// used by SliceHelper::MeshImportedLayersWithExistingButtonBars()
INT32 g_NoOfBarsBeforeImport;

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(DocumentRecordHandler,CamelotRecordHandler)

// Give this file in memory dumps
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	DocumentRecordHandler::DocumentRecordHandler()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		-
	Returns:	-
	Purpose:	Default constructor
	SeeAlso:	-

********************************************************************************************/

DocumentRecordHandler::DocumentRecordHandler()
{
	InitVars();
}

/********************************************************************************************

>	void DocumentRecordHandler::InitVars()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Returns:	-
	Purpose:	Inits all the member vars to their default settings
	SeeAlso:	-

********************************************************************************************/

void DocumentRecordHandler::InitVars()
{
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::BeginImport()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Default constructor
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::BeginImport()
{
	// Init vars before import
	InitVars();

	return TRUE;
}



/********************************************************************************************
>	virtual UINT32* DocumentRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
********************************************************************************************/

UINT32* DocumentRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_SPREAD,
								TAG_SPREADINFORMATION,
								TAG_DOCUMENT,
								TAG_CHAPTER,
								TAG_LAYER,
								TAG_LAYERDETAILS,
								TAG_GUIDELAYERDETAILS,
								TAG_GUIDELINE,
								TAG_GRIDRULERSETTINGS,
								TAG_GRIDRULERORIGIN,
								TAG_SPREADSCALING_ACTIVE,
								TAG_SPREADSCALING_INACTIVE,
								TAG_SPREAD_ANIMPROPS,
								TAG_LAYER_FRAMEPROPS,
								TAG_SETSENTINEL,
								TAG_SETPROPERTY,
								TAG_BARPROPERTY,
								TAG_CURRENTATTRIBUTES,
								TAG_CURRENTATTRIBUTEBOUNDS,
								TAG_DUPLICATIONOFFSET,
								CXFRH_TAG_LIST_END
							 };
	return TagList;
}



/********************************************************************************************

>	virtual BOOL DocumentRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	INT32 Tag = pCXaraFileRecord->GetTag();
	switch (Tag)
	{
		case TAG_DOCUMENT:
			ok = HandleDocumentRecord(pCXaraFileRecord);
			break;

		case TAG_CHAPTER:
			ok = HandleChapterRecord(pCXaraFileRecord);
			break;

		case TAG_SPREAD:
			ok = HandleSpreadRecord(pCXaraFileRecord);
			break;

		case TAG_SPREADINFORMATION:
			ok = HandleSpreadInformationRecord(pCXaraFileRecord);
			break;
		
		case TAG_GRIDRULERSETTINGS:
			ok = HandleGridAndRulerSettingsRecord(pCXaraFileRecord);
			break;
		case TAG_GRIDRULERORIGIN:
			ok = HandleGridAndRulerOriginRecord(pCXaraFileRecord);
			break;

		case TAG_SPREADSCALING_ACTIVE:
		case TAG_SPREADSCALING_INACTIVE:
			ok = HandleSpreadScalingRecord(pCXaraFileRecord, Tag);
			break;

		case TAG_LAYER:
			ok = HandleLayerRecord(pCXaraFileRecord);
			break;

		case TAG_LAYERDETAILS:
		case TAG_GUIDELAYERDETAILS:
				ok = HandleLayerDetailsRecord(pCXaraFileRecord);
			break;

		case TAG_GUIDELINE:
			ok = HandleGuidelineRecord(pCXaraFileRecord);
			break;

		case TAG_SPREAD_ANIMPROPS:
			ok = HandleSpreadAnimPropertiesRecord(pCXaraFileRecord);
			break;
			
		case TAG_LAYER_FRAMEPROPS:
			ok = HandleFramePropertiesRecord(pCXaraFileRecord);
			break;

		case TAG_SETSENTINEL:
			ok = HandleSetSentinelRecord(pCXaraFileRecord);
			break;

		case TAG_SETPROPERTY:
			ok = HandleSetPropertyRecord(pCXaraFileRecord);
			break;

		case TAG_BARPROPERTY:
			ok = HandleBarPropertyRecord(pCXaraFileRecord);
			break;

		case TAG_CURRENTATTRIBUTES:
			ok = HandleCurrentAttrsRecord(pCXaraFileRecord);
			break;

		case TAG_CURRENTATTRIBUTEBOUNDS:
			ok = HandleCurrentAttrsRecord(pCXaraFileRecord);
			break;

		case TAG_DUPLICATIONOFFSET:
			ok = HandleDuplicationOffsetRecord(pCXaraFileRecord);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}


/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleDocumentRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a document record
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleDocumentRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_DOCUMENT,FALSE,"I don't handle this tag type");

	IncDocumentInsertedCount();

	// If this is not the first doc in the file, ignore the rest of the file
	if (GetDocumentInsertedCount() > 1)
		EndOfFile();

	return TRUE;
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleChapterRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a chapter record
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleChapterRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_CHAPTER,FALSE,"I don't handle this tag type");

	IncChapterInsertedCount();

	// If this is not the first chapter in the file, ignore the rest of the file
	if (GetChapterInsertedCount() > 1)
	{
		EndOfFile();
		return TRUE;
	}

	// Ignore this record if we are importing into an existing file
	if (IsImporting()) return TRUE;

	Node* pContextNode = GetInsertContextNode();
	ERROR2IF(pContextNode == NULL,FALSE, "What? No context node?");
	Node* pChapter = pContextNode->FindParent(CC_RUNTIME_CLASS(Chapter));
	ERROR2IF(pChapter == NULL,FALSE, "Couldn't find the chapter node");

	return TRUE;
/*
	Node* pContextNode = GetInsertContextNode();
	ERROR2IF(pContextNode == NULL,FALSE,"What? No context node?");

	Node* pChapter = pContextNode->FindParent(CC_RUNTIME_CLASS(Chapter));
	if (pChapter!= NULL)
	{
		SetInsertContextNode(pChapter);
		InsertNextNodeAsSibling();
	}

	BOOL ok = FALSE;

	{
		Chapter* pNewChapter = new Chapter;
		if (pNewChapter != NULL)
			ok = InsertNode(pNewChapter);

		SetLastChapterInserted(pNewChapter);
		InsertNextNodeAsChild();
	}

	if (ok && pChapter != NULL)
	{
		pChapter->CascadeDelete();
		delete pChapter;
		pChapter = NULL;
	}
	return ok;
  */
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleSpreadRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a spread record
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleSpreadRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_SPREAD,FALSE,"I don't handle this tag type");

	IncSpreadInsertedCount();

	// If this is not the first spread in the file, ignore the rest of the file
	if (GetSpreadInsertedCount() > 1)
	{
		EndOfFile();
		return TRUE;
	}

	// Ignore this record if we are importing into an existing file
	if (IsImporting())
		return TRUE;

	Node* pContextNode = GetInsertContextNode();
	ERROR2IF(pContextNode == NULL, FALSE, "What? No context node?");

	// JCF: bodge insertion context.
	if (IS_A(pContextNode, NodeSetSentinel))
	{
		SetInsertContextNode(0);
		pContextNode = GetInsertContextNode();
	}

	Spread* pSpread = (Spread*) pContextNode->FindParent(CC_RUNTIME_CLASS(Spread));
	ERROR2IF(pSpread == NULL, FALSE, "Can't find the spread");

	SetLastSpreadInserted(pSpread);
	SetInsertContextNode(pSpread);
	InsertNextNodeAsChild();

	Layer* pLayer = pSpread->FindFirstLayer();
	if (pLayer != NULL)
	{
		SetInsertContextNode(pLayer);
		InsertNextNodeAsChild();
	}

	return TRUE;

/*
	Node* pContextNode = GetInsertContextNode();
	ERROR2IF(pContextNode == NULL,FALSE,"What? No context node?");

	Node* pSpread = pContextNode->FindParent(CC_RUNTIME_CLASS(Spread));
	if (pSpread != NULL)
	{
		SetInsertContextNode(pSpread);
		InsertNextNodeAsSibling();
	}

	BOOL ok = FALSE;

	{
		Spread* pNewSpread = new Spread;
		if (pNewSpread != NULL)
			ok = InsertNode(pNewSpread);

		SetLastSpreadInserted(pNewSpread);
		InsertNextNodeAsChild();

		NodeGrid::MakeDefaultGrid(pNewSpread);
		Page* pPage = new Page;
		if (pPage == NULL)
			return FALSE;

		pPage->AttachNode(pNewSpread,LASTCHILD);

		SetInsertContextNode(pPage);
		InsertNextNodeAsSibling();
	}

	if (ok && pSpread != NULL)
	{
		pSpread->CascadeDelete();
		delete pSpread;
		pSpread = NULL;
	}
	return ok;
*/
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleSpreadInformationRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the spread information record. Decodes the information and then
				applies it.
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleSpreadInformationRecord(CXaraFileRecord* pCXaraFileRecord)
{
	// Ignore this record if we are importing into ac existing file
	if (IsImporting())
		return TRUE;

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	Spread* pLastSpread = GetLastSpreadInserted();

	// If there isn't a spread to apply the info to, just ignore it
	if (pLastSpread == NULL)
	{
		ERROR3("I don't have a 'last spread' ptr");
		return TRUE;
	}

	BOOL ok = TRUE;

	// Width, Height of page
	MILLIPOINT Width = 0;
	MILLIPOINT Height = 0;
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Width);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Height);
	
	// <Margin : MILLIPOINT>	The margin to add around all four sides of the pages in the spread to make up the pasteboard. 
	// <Bleed : MILLIPOINT>	Bleed margin to add around all pages in this spread. (0 means none)
	MILLIPOINT Margin = 0;
	MILLIPOINT Bleed = 0;
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Margin);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Bleed);

	// <SpreadFlags : BYTE>	Flags for the current spread.
	// SpreadFlags ::= DoublePageSpread | ShowDropShadow 
	// ShowDropShadow flag to say whether we apply a page shadow behind the page
	// DoublePageSpread flag to say whether one or two pages are present 
	BOOL Dps = TRUE;
	BOOL ShowDropShadow = TRUE;
	BYTE FlagsWord = 0; 
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&FlagsWord);
	if (FlagsWord & 1)
		Dps = TRUE;
	else
		Dps = FALSE; 
	if (FlagsWord & 2)
		ShowDropShadow = TRUE;
	else
		ShowDropShadow = FALSE; 

	// If everything has been read in ok then go and apply those settings to the current
	// spread
//	Document* pDoc = GetDocument();
	if (ok && pLastSpread && Width > 0 && Height > 0)
	{
		// This function also sets the fold linea automatically
		/*BOOL Worked =*/ pLastSpread->SetPageSize(Width, Height, Margin, Bleed, Dps, ShowDropShadow);

		if (!IsImporting())
		{
			// Only change the the import coord origin if are not importing into another document
			// When importing into an existing document, the import origin would have already been set
			// (e.g. to the drag & drop point)

			// We have altered the coordinates origin by this so we must update it
			DocRect PagesRect;
			if (pLastSpread->GetPagesRect(&PagesRect))
				SetCoordOrigin(PagesRect.lo);
		}
	}	

	return ok;
}

/********************************************************************************************

>	Layer* DocumentRecordHandler::FindThisLayer(UINT32 n)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Inputs:		n = number of the layer to find (the first layer is layer 1)
	Returns:	ptr to layer, or NULL if it couldn't find the nth layer
	Purpose:	This hunts for the nth layer in the sibling list of the context node.

				It is used when loading in a file (as opposed to importing).
				If a layer is read in, and there already exists a layer in the tree that corresponds to
				this layer, this func with find it so it can be used, instead of creating a new one
	SeeAlso:	-

********************************************************************************************/

Layer* DocumentRecordHandler::FindThisLayer(UINT32 n)
{
	if (n < 1) n = 1;

	Node* pNode = GetInsertContextNode();
	if (pNode != NULL)
		pNode = pNode->FindParent();
	if (pNode != NULL)
		pNode = pNode->FindFirstChild();

	while (pNode != NULL)
	{
		if (pNode->IsLayer())
		{
			n--;
			if (n == 0)
				return (Layer*)pNode;
		}

		pNode = pNode->FindNext();
	}

	return NULL;
}


/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleLayerRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a layer record
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleLayerRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_LAYER,FALSE,"I don't handle this tag type");

	IncLayerInsertedCount();

	BOOL ok = TRUE;

	Layer* pLayer = NULL;
	
	if (!IsImporting())
		pLayer = FindThisLayer(GetLayerInsertedCount());

	if (pLayer == NULL)
	{
		pLayer = new Layer;

		ok = FALSE;
		if (pLayer != NULL)
		{
			ok = InsertNode(pLayer);

			if (ok)
			{
				// Set the layer name to a default
				String_256 LayerName( _("Imported ") );
				String_256 Suffix( _("layer") );
				LayerName += Suffix;
				pLayer->SetLayerID(LayerName);
				pLayer->EnsureUniqueLayerID();
			}
		}
	}

	SetLastLayerInserted(pLayer);
	
	// If we haven't set it already, then set this as the first imported layer
	Layer * pFirstImportedLayer = GetFirstImportedLayer();
	if (pFirstImportedLayer == NULL)
		SetFirstImportedLayer(pLayer);

	return ok;
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleLayerDetailsRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a layer and guide layer details record
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleLayerDetailsRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_LAYERDETAILS && Tag != TAG_GUIDELAYERDETAILS,FALSE,"I don't handle this tag type");

	Layer* pLastLayer = GetLastLayerInserted();

	// If there isn't a layer to apply the info to, just ignore it
	if (pLastLayer == NULL)
	{
		ERROR3("I don't have a 'last layer' ptr");
		return TRUE;
	}

	BYTE Flags = 0;
	String_256 Name;
	INT32 ColRef = 0;
	DocColour Col;

	BOOL ok = TRUE;

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);
	if (ok) ok = pCXaraFileRecord->ReadUnicode(&Name);//Name, Name.MaxLength());

	if (ok && Tag == TAG_GUIDELAYERDETAILS)
		ok = pCXaraFileRecord->ReadINT32(&ColRef);

#ifdef WEBSTER
	if (ok)
	{
		// WEBSTER - markn 14/1/97
		// If it's a guide layer, make it invisible
		// otherwise if the layer is visible, make sure it's editable

		if (Tag == TAG_GUIDELAYERDETAILS)
			Flags &= ~TAG_LAYER_FLAG_VISIBLE;	// clear visible flag
		else
		{
			if (Flags & TAG_LAYER_FLAG_VISIBLE)
				Flags &= ~TAG_LAYER_FLAG_LOCKED;	// visible, so clear locked flag

			// Markn 18/7/97
			// Fixed a bug whereby Webster would load in locked page background layers and
			// unlock them because of the two lines above.
			if (Flags & TAG_LAYER_FLAG_PAGEBACKGROUND)
				Flags |= TAG_LAYER_FLAG_LOCKED;		// page background, so make sure it's locked
		}
	}
#endif	// WEBSTER

	pLastLayer->SetVisible(  (Flags & TAG_LAYER_FLAG_VISIBLE)   != 0);
	pLastLayer->SetLocked(   (Flags & TAG_LAYER_FLAG_LOCKED)    != 0);
	pLastLayer->SetPrintable((Flags & TAG_LAYER_FLAG_PRINTABLE) != 0);
	pLastLayer->SetActive(	 (Flags & TAG_LAYER_FLAG_ACTIVE)	!= 0);
	// new 4/4/97
	pLastLayer->SetPageBackground((Flags & TAG_LAYER_FLAG_PAGEBACKGROUND) != 0);

	pLastLayer->SetBackground((Flags & TAG_LAYER_FLAG_BACKGROUND) != 0);

	// handle Guide Layers a little differently:
	if(Tag == TAG_GUIDELAYERDETAILS)
	{
		pLastLayer->SetGuide(TRUE);
	}
	else
	{
		pLastLayer->SetGuide(FALSE);
	}

	pLastLayer->SetLayerID(Name);

#if !defined(EXCLUDE_FROM_RALPH)
	if (ok && ColRef != 0)
	{
		ok = GetDocColour(ColRef,&Col);
		if (ok)
		{
			IndexedColour* pIndexedColour = Col.FindParentIndexedColour();
			pLastLayer->SetGuideColour(pIndexedColour);
			pLastLayer->SetGuide(TRUE);
		}
	}
#endif

	if (ok)
	{
		// Do some extra work on the layer, such as ensuring the layer name is unique
		// Ensure that if we are importing that the layers are placed in as foreground visible
		// and editable layers as otherwise confusion may reign.
		
		// NOTE: If not importing then Webster must not play with the PageBackground layer and
		// its visible and editable status
		
		if (IsImporting())
		{
			// Firstly, make sure all imported layers are printable visible editable foreground layers
			// but only if they are not guide layers
			if (!pLastLayer->IsGuide())
			{
				// Disable the blatent lets make all layers visible code. This would mean
				// that hidden layers actually appear. In cases like pills.xar, this is bad!
				//pLastLayer->SetVisible(TRUE);
				pLastLayer->SetLocked(FALSE);
				pLastLayer->SetPrintable(TRUE);
			}

			// Ensure that all imported layers are NOT background as otherwise we will have a mixture
			//pLastLayer->SetBackground(FALSE);

			// no imported layer is going to be the active layer!
			pLastLayer->SetActive(FALSE);

			// If we have improted a page background layer then we need to do some extra work
			if (pLastLayer->IsPageBackground())
			{
				// If we dont unset this then things may get confusing!
				// Unlocked visible page background layers = trouble
				pLastLayer->SetPageBackground(FALSE);
				// If we set it as a frame layer then it wont get merged with the active layer
				pLastLayer->SetFrame(TRUE);
				// Mark it as hidden and solid so that it is almost as it was as a page background layer
				pLastLayer->SetHiddenFrame(TRUE);
				pLastLayer->SetSolid(TRUE);
			}

			// If importing do something a little more intelligent with the name matching
			// SJK - as a test try to keep the names the same then merge similar names at 
			// the last minute when importing
			INT32 ImportWithLayers = 1;
			Camelot.GetPrefValue(TEXT("Filters"), TEXT("ImportWithLayers"), &ImportWithLayers);

			if (ImportWithLayers == 0) // ask for unique layer names, all on new layers
				pLastLayer->EnsureUniqueLayerID();
			else if (ImportWithLayers == 1) // stick it on the active layer
			{
				Spread* pSpread = GetSpread();

				if (pSpread)
				{
					// name it the same as the active layer
					// and the layers will be meshed together later
					Layer * pActiveLayer = pSpread->FindActiveLayer();
					pLastLayer->SetLayerID(pActiveLayer->GetLayerID());
				}				
			}

			String_256 CurName = pLastLayer->GetLayerID();

			// Has the name changed as a result of the above call?
			if (ImportWithLayers == 0 && CurName != Name)
			{
				// Get a suitable prefix string (in this case "Imported ")
				String_256 Prefix( _("Imported ") );

				// Does the name already have the prefix string embedded in it?
				if (cc_lstristr(Name,Prefix) == NULL)
				{
					// If not, append the prefix to the original name, and try again
					Prefix += Name;
					pLastLayer->SetLayerID(Prefix);
					pLastLayer->EnsureUniqueLayerID();
				}
			}
		}
		else
			pLastLayer->EnsureUniqueLayerID();
	}

	return ok;
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleGuidelineRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a guideline record
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleGuidelineRecord(CXaraFileRecord* pCXaraFileRecord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR3IF(pCXaraFileRecord->GetTag() != TAG_GUIDELINE,"I don't handle this tag type");

	BYTE Type;
	MILLIPOINT Ordinate;

	BOOL ok = pCXaraFileRecord->ReadBYTE(&Type);

	if (ok && Type == GUIDELINE_HORZ)
		ok = pCXaraFileRecord->ReadYOrd(&Ordinate);
	else
		ok = pCXaraFileRecord->ReadXOrd(&Ordinate);

	if (ok)
	{
		NodeGuideline* pGuide = new NodeGuideline;

		if (pGuide != NULL)
		{
			pGuide->SetType(GuidelineType(Type));
			pGuide->SetOrdinate(Ordinate);
			ok = InsertNode(pGuide);
		}
	}

	return ok;
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleGridAndRulerSettingsRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles grid and ruler settings record. Decodes the information and then
				applies it.
	SeeAlso:	Document::ImportGridInfo;

********************************************************************************************/

BOOL DocumentRecordHandler::HandleGridAndRulerSettingsRecord(CXaraFileRecord* pCXaraFileRecord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	// If we are just importing data from this document into an existing one,
	// then don't import this data.
	if (IsImporting())
		return TRUE;
	
	BOOL ok = TRUE;

	double   Divisions    = 0.0;
	UnitType Type         = NOTYPE;
	UINT32    SubDivisions = 0;
	GridType TypeOfGrid   = RECTANGULAR;
	INT32	 UnitsRef	  = 0L;
	BYTE	 TypeOfGridB  = (BYTE)RECTANGULAR;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&UnitsRef);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&Divisions);
	if (ok) ok = pCXaraFileRecord->ReadUINT32(&SubDivisions);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TypeOfGridB);
	TypeOfGrid = (GridType)TypeOfGridB;

	// Convert the reference into unittype
	UnitListComponent * pUnitsComponent = GetUnitDocComponent();
	ERROR2IF(pUnitsComponent == NULL,FALSE,"HandleGridAndRulerSettingsRecord No units doc component present");

	Unit* pUnit = pUnitsComponent->GetReadUnitReference(UnitsRef);
	if (pUnit)
		Type = pUnit->GetUnitType();
	else
		return FALSE;

	Spread* pSpread = GetSpread();
	ERROR2IF(pSpread == NULL,FALSE,"HandleGridAndRulerSettingsRecord No spread");

	NodeGrid* pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	ERROR2IF(pDefaultGrid == NULL,FALSE,"HandleGridAndRulerSettingsRecord No default grid");
	
	// remove it as its bound to be wrong
	pDefaultGrid->CascadeDelete();
	delete pDefaultGrid;				

	// Set all the global variables - What a load of B********
	NodeGrid::SetDefaultDivisions(Divisions);
	NodeGrid::SetDefaultSubdivisions(SubDivisions);
	NodeGrid::SetDefaultUnits(Type);
	NodeGrid::SetDefaultGridType(TypeOfGrid);

	// Make a grid
	// BODGE - old builds (hence docs) save the grid spacing in divisions and units but don't
	// account for unit scaling, so as not to change doc format new docs do the same so we must
	// set the grid with scaling turned off - yuk!
	// But surely it is best saved unscaled!
	BOOL Scale=FALSE;
	NodeGrid::MakeDefaultGrid(pSpread, Scale);

	/* DocRect PageRect;
	pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	if (pSpread->GetPagesRect(&PageRect) && pDefaultGrid)
	{
		pDefaultGrid->SetOrigin(PageRect.lo.x, PageRect.lo.y);
	} */

	return ok;
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleGridAndRulerOriginRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the grid and ruler origin record. Decodes the information and then
				applies it.
	SeeAlso:	Document::ImportOriginInfo;

********************************************************************************************/

BOOL DocumentRecordHandler::HandleGridAndRulerOriginRecord(CXaraFileRecord* pCXaraFileRecord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	// If we are just importing data from this document into an existing one,
	// then don't import this data.
	if (IsImporting())
		return TRUE;
	
	BOOL ok = TRUE;

	DocCoord Origin(0,0);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&Origin);

	Spread* pSpread = GetSpread();
	ERROR2IF(pSpread == NULL,FALSE,"HandleGridAndRulerOriginRecord No spread");

	NodeGrid* pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	ERROR2IF(pDefaultGrid == NULL,FALSE,"HandleGridAndRulerOriginRecord No default grid");

	pDefaultGrid->SetOrigin(Origin.x, Origin.y);	

	return ok;
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleSpreadScalingRecord(CXaraFileRecord* pCXaraFileRecord,
														  INT32 Tag)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				Tag	= tag for this record
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the spread scaling records. Decodes the information and then
				applies it.
	SeeAlso:	Document::ImportViewInfo;

********************************************************************************************/

BOOL DocumentRecordHandler::HandleSpreadScalingRecord(CXaraFileRecord* pCXaraFileRecord, INT32 Tag)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	// If we are just importing data from this document into an existing one,
	// then don't import this data.
	if (IsImporting())
		return TRUE;
	
	BOOL ok = TRUE;

	BOOL Active = FALSE;
	if (Tag == TAG_SPREADSCALING_ACTIVE)
		Active= TRUE;
	
	double 		DrawingScaleValue	= 1.0;
	INT32		DrawingUnits		= 0L;
	double 		RealScaleValue		= 1.0;
	INT32		RealUnits			= 0L;
//	UnitType	DrawingType			= NOTYPE;
//	UnitType	RealType			= NOTYPE;
	BOOL		DrawingPrefix		= FALSE;
	BOOL		RealPrefix			= FALSE;
	String_32	DrawingAbbr;
	String_32	RealAbbr;

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&DrawingScaleValue);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&DrawingUnits);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&RealScaleValue);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&RealUnits);

	// Any problems then return immediately
	if (!ok)
		return FALSE;

	// Convert the reference into unittype
	UnitListComponent * pUnitsComponent = GetUnitDocComponent();
	ERROR2IF(pUnitsComponent == NULL,FALSE,"HandleSpreadScalingRecord No units doc component present");

	Unit* pDrawingUnit = pUnitsComponent->GetReadUnitReference(DrawingUnits);
	if (pDrawingUnit)
	{
		DrawingAbbr = pDrawingUnit->GetSpecifier();
		DrawingPrefix = pDrawingUnit->IsPrefix();
	}
	else
		return FALSE;

	Unit* pRealUnit = pUnitsComponent->GetReadUnitReference(RealUnits);
	if (pRealUnit)
	{
		RealAbbr = pRealUnit->GetSpecifier();
		RealPrefix = pRealUnit->IsPrefix();
	}
	else
		return FALSE;

	//if (RealType == NOTYPE || DrawingType == NOTYPE)
	//	ERROR2(FALSE,"HandleSpreadScalingRecord bad real and/or drawing unit types");

	Spread* pSpread = GetSpread();
	ERROR2IF(pSpread == NULL,FALSE,"HandleSpreadScalingRecord No spread");

	DimScale* pDimScale = pSpread->GetPtrDimScale();
	ERROR2IF(pDimScale == NULL,FALSE,"HandleSpreadScalingRecord No pDimScale");

	// Set up the scale factor strings with this new info
	// Could do with functions to set the scale factor and units directly but if that's the
	// way DimScale wants it then thats the way it is going to get it!
	String_32 DrawingScale;
	String_32 RealScale;
	INT32 DecimalPlaces  = 6;
	if (DrawingPrefix)
	{
		DrawingScale += DrawingAbbr;
	}
	if (ok) ok = Convert::DoubleToString( DrawingScaleValue, &DrawingScale, DecimalPlaces);
	if (!DrawingPrefix)
	{
		DrawingScale += DrawingAbbr;
	}

	if (RealPrefix)
		RealScale += RealAbbr;
	if (ok) ok = Convert::DoubleToString( RealScaleValue, &RealScale, DecimalPlaces);
	if (!RealPrefix)
		RealScale += RealAbbr;
	
	if (ok)
	{
		// Set up a possibly new active state
		pDimScale->SetActiveState(Active);
		// Now set up all the other aspects
		ok = pDimScale->SetDrawingScaleStr(DrawingScale);
		if (ok) ok = pDimScale->SetRealScaleStr(RealScale);
		if (ok) ok = pDimScale->SetScaleFactor();
	}

#ifdef __WXMAC__
PORTNOTE("MacPort", "Horrendous bodge - One of the above functions returns ok = FALSE. Another ReadDOUBLE problem?")
return TRUE;
#endif

	return ok;
#else
	return TRUE;
#endif
}


/********************************************************************************************

>	virtual void DocumentRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,
																 StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the document records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void DocumentRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	// This outputs the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	TCHAR s[256];
	BOOL ok = TRUE;

	UINT32 Tag = pRecord->GetTag();
	switch (Tag)
	{
		case TAG_DOCUMENT:
			break;

		case TAG_SPREAD:
			break;

		case TAG_CHAPTER:
			break;

		case TAG_SETSENTINEL:
			break;

		case TAG_SETPROPERTY:
			break;

		case TAG_SPREADINFORMATION:
		{
			(*pStr) += _T("Spread Information record\r\n\r\n");
			
			// Width, Height of page
			MILLIPOINT Width = 0;
			MILLIPOINT Height = 0;
			if (ok) ok = pRecord->ReadINT32(&Width);
			if (ok) ok = pRecord->ReadINT32(&Height);
			camSprintf(s,_T("Page width \t: %d\r\n"),Width);
			(*pStr) += s;
			camSprintf(s,_T("Page height \t: %d\r\n"),Height);
			(*pStr) += s;
			
			// <Margin : MILLIPOINT>	The margin to add around all four sides of the pages in the spread to make up the pasteboard. 
			// <Bleed : MILLIPOINT>	Bleed margin to add around all pages in this spread. (0 means none)
			MILLIPOINT Margin = 0;
			MILLIPOINT Bleed = 0;
			if (ok) ok = pRecord->ReadINT32(&Margin);
			if (ok) ok = pRecord->ReadINT32(&Bleed);
			camSprintf(s,_T("Margin around spread : %d\r\n"),Margin);
			(*pStr) += s;
			camSprintf(s,_T("Bleed around spread : %d\r\n"),Bleed);
			(*pStr) += s;

			// <SpreadFlags : BYTE>	Flags for the current spread.
			// SpreadFlags ::= DoublePageSpread | ShowDropShadow | ShowFoldLine
			// ShowDropShadow flag to say whether we apply a page shadow behind the page
			// ShowFoldLine flag to say whether a fold line is required.
			// DoublePageSpread flag to say whether one or two pages are present 
			BOOL Dps = TRUE;
			BOOL ShowDropShadow = TRUE;
			BYTE FlagsWord = 0; 
			if (ok) ok = pRecord->ReadBYTE(&FlagsWord);
			if (FlagsWord & 4)
				Dps = TRUE;
			else
				Dps = FALSE; 
			if (FlagsWord & 2)
				ShowDropShadow = TRUE;
			else
			camSprintf(s,_T("Double page spread : %d\r\n"),Dps);
			(*pStr) += s;
			camSprintf(s,_T("Show drop shadow : %d\r\n"),ShowDropShadow);
			(*pStr) += s;

			break;
		}
		case TAG_PAGE:
			break;

		case TAG_LAYERDETAILS:
		case TAG_GUIDELAYERDETAILS:
		{
			BYTE Flags;
			String_256 Name;
			INT32 ColRef = 0;

			pRecord->ReadBYTE(&Flags);
			pRecord->ReadUnicode(&Name);//Name, Name.MaxLength());
			if (Tag == TAG_GUIDELAYERDETAILS)
			{
				(*pStr) += _T("Guide layer details\r\n\r\n");
				pRecord->ReadINT32(&ColRef);
			}
			else
			{
				(*pStr) += _T("Standard layer details\r\n\r\n");
			}

			(*pStr) += _T("Flags\r\n");
			sprintf(s,"Visible:\t%d\r\n",	(Flags & TAG_LAYER_FLAG_VISIBLE)	!= 0); (*pStr) += s;
			sprintf(s,"Locked:\t%d\r\n",	(Flags & TAG_LAYER_FLAG_LOCKED)		!= 0); (*pStr) += s;
			sprintf(s,"Printable:\t%d\r\n",	(Flags & TAG_LAYER_FLAG_PRINTABLE)	!= 0); (*pStr) += s;
			sprintf(s,"Active:\t%d\r\n",	(Flags & TAG_LAYER_FLAG_ACTIVE)		!= 0); (*pStr) += s;
			sprintf(s,"PageBackground:\t%d\r\n",(Flags & TAG_LAYER_FLAG_PAGEBACKGROUND)	!= 0); (*pStr) += s;
			(*pStr) += _T("\r\n");

			sprintf(s,"Name: %s\r\n\r\n",(TCHAR*)Name); (*pStr) += s;
			sprintf(s,"Colour reference : %d\r\n",ColRef); (*pStr) += s;
		}
		break;

		case TAG_GRIDRULERSETTINGS:
		{
			double   Divisions    = 0.0;
			UnitType Unit         = NOTYPE;
			UINT32    SubDivisions = 0;
			GridType TypeOfGrid   = RECTANGULAR;
			BYTE	 TypeOfGridB  = 0;
			INT32	 UnitsRef	  = 0L;

			if (ok) ok = pRecord->ReadINT32(&UnitsRef);
			_stprintf(s,"Grid units : %d\r\n",UnitsRef);
			(*pStr) += s;
			if (ok) ok = pRecord->ReadDOUBLE(&Divisions);
			_stprintf(s,"Grid divisions : %f\r\n",Divisions);
			if (ok) ok = pRecord->ReadUINT32(&SubDivisions);
			_stprintf(s,"Grid sub divisions : %d\r\n",SubDivisions);
			if (ok) ok = pRecord->ReadBYTE(&TypeOfGridB);
			TypeOfGrid = (GridType)TypeOfGridB;
			if (TypeOfGrid == RECTANGULAR)
				(*pStr) += "Rectangular";
			else if (TypeOfGrid == ISOMETRIC)
				(*pStr) += "Isometric";
			else
				(*pStr) += "Unknown";
			break;
		}

		case TAG_GRIDRULERORIGIN:
		{
			DocCoord Origin(0,0);
			if (ok) ok = pRecord->ReadCoord(&Origin);
			_stprintf(s,"Grid and Ruler origin : %d, %d\r\n",Origin.x,Origin.y);
			(*pStr) += s;
			break;
		}

		case TAG_SPREADSCALING_ACTIVE:
		case TAG_SPREADSCALING_INACTIVE:
		{
			if (Tag == TAG_SPREADSCALING_ACTIVE)
				(*pStr) += "Spread scaling active \r\n\r\n";
			else
				(*pStr) += "Spread scaling inactive \r\n\r\n";
			
			double 	DrawingScaleValue	= 1.0;
			INT32	DrawingUnits		= 0L;
			double 	RealScaleValue		= 1.0;
			INT32	RealUnits			= 0L;
			if (ok) ok = pRecord->ReadDOUBLE(&DrawingScaleValue);
			if (ok) ok = pRecord->ReadINT32(&DrawingUnits);
			if (ok) ok = pRecord->ReadDOUBLE(&RealScaleValue);
			if (ok) ok = pRecord->ReadINT32(&RealUnits);
			_stprintf(s,"Drawing scale factor \t: %f\r\n",DrawingScaleValue);
			(*pStr) += s;
			_stprintf(s,"Drawing units ref \t: %d\r\n",DrawingUnits);
			(*pStr) += s;
			_stprintf(s,"Real scale factor \t\t: %f\r\n",RealScaleValue);
			(*pStr) += s;
			_stprintf(s,"Real units ref \t: %d\r\n",RealUnits);
			(*pStr) += s;
			break;
		}

		case TAG_GUIDELINE:
		{
			BYTE Type;
			MILLIPOINT Ordinate;

			pRecord->ReadBYTE(&Type);
			pRecord->ReadINT32(&Ordinate);

			_stprintf(s,"Type\t: %d\r\n",INT32(Type));	(*pStr) += s;
			_stprintf(s,"Type\t: %d\r\n",Ordinate);		(*pStr) += s;
		}
		break;

		case TAG_SPREAD_ANIMPROPS:
		{
			DWORD	Loop				=0;				
			DWORD	GlobalDelay			=0;		
			DWORD	Dither				=0;			
			DWORD	WebPalette			=0;		
			DWORD	ColoursPalette		=0;	
			DWORD	NumColsInPalette	=0;	
			DWORD	FlagsWord			=0;

			if (ok)
				ok = pRecord->ReadUINT32(&Loop);	
			if (ok)
				ok = pRecord->ReadUINT32(&GlobalDelay);
			if (ok)
				ok = pRecord->ReadUINT32(&Dither);
			if (ok)
				ok = pRecord->ReadUINT32(&WebPalette);
			if (ok)
				ok = pRecord->ReadUINT32(&ColoursPalette);
			if (ok)
				ok = pRecord->ReadUINT32(&NumColsInPalette);
			if (ok)
				ok = pRecord->ReadUINT32(&FlagsWord);

			// Decode the flags word into its component parts 
			BOOL UseSystemColours = FALSE;
			// <SpreadAnimationFlags : WORD>	Flags for the current animation properties.
			// SpreadAnimationFlags ::= UseSystemColours
			// UseSystemColours flag to say whether we add system colours to the exported palettes
			UseSystemColours = ((FlagsWord & 1) ? TRUE : FALSE);
	
			// Display the info..

			_stprintf(s,"Animation Loop \t: %d\r\n",Loop);
				(*pStr) += s;
			_stprintf(s,"GlobalDelay \t: %d\r\n",GlobalDelay);
				(*pStr) += s;
			_stprintf(s,"Animation Dither \t: %d\r\n",Dither);
				(*pStr) += s;
			_stprintf(s,"Palette	 \t: %d\r\n",WebPalette);
				(*pStr) += s;
			_stprintf(s,"Palette Colours \t: %d\r\n",ColoursPalette);
				(*pStr) += s;
			_stprintf(s,"Number of Colours In Palette \t: %d\r\n",NumColsInPalette);
				(*pStr) += s;
			_stprintf(s,"Add System Colours \t: %d\r\n",UseSystemColours);
				(*pStr) += s;

		};
		break;

		case TAG_LAYER_FRAMEPROPS:
		{
			DWORD Delay=0;
			BYTE Flags = 0;
			
			if (ok) ok = pRecord->ReadUINT32(&Delay);
			if (ok)	ok = pRecord->ReadBYTE(&Flags);
			_stprintf(s,"Frame Layer Delay \t: %d\r\n",Delay);						(*pStr) += s;
			sprintf(s,"Solid:\t%d\r\n",  (Flags & TAG_LAYER_FLAG_SOLID)		!= 0);	(*pStr) += s;
			sprintf(s,"Overlay:\t%d\r\n",(Flags & TAG_LAYER_FLAG_OVERLAY)	!= 0);	(*pStr) += s;
			sprintf(s,"Hidden:\t%d\r\n", (Flags & TAG_LAYER_FLAG_HIDDEN)	!= 0);	(*pStr) += s;
		}
		break;

		case TAG_CURRENTATTRIBUTES:
		{
			BYTE GroupID;
			pRecord->ReadBYTE(&GroupID);
			switch(GroupID)
			{
			case ATTRIBUTEGROUP_INK:
				_stprintf(s,"Attribute Group ID = INK\r\n"); (*pStr) += s;
				break;
			case ATTRIBUTEGROUP_TEXT:
				_stprintf(s,"Attribute Group ID = TEXT\r\n"); (*pStr) += s;
				break;
			default:
				_stprintf(s,"Attribute Group ID = UNKNOWN!\r\n"); (*pStr) += s;
				break;
			}
		}
		break;

		case TAG_CURRENTATTRIBUTEBOUNDS:
		{
			DocRect attrBounds;
			pRecord->ReadCoord(&attrBounds.lo);
			pRecord->ReadCoord(&attrBounds.hi);

			_stprintf(s,"lox \t: %d\r\n", attrBounds.lox);
				(*pStr) += s;
			_stprintf(s,"loy \t: %d\r\n", attrBounds.loy);
				(*pStr) += s;
			_stprintf(s,"hix \t: %d\r\n", attrBounds.hix);
				(*pStr) += s;
			_stprintf(s,"hiy \t: %d\r\n", attrBounds.hiy);
				(*pStr) += s;

		}
		break;

	}
}
#endif // _DEBUG

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleSpreadAnimPropertiesRecord(CXaraFileRecord* pRec);

 	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/05/97
	Inputs:		pCXaraFileRecord,  ptr to the record to handle.
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the spread information record. Decodes the information and then
				applies it.
	Notes:		Karim 07/12/2000 - modified to read in transparent GIF information.

********************************************************************************************/
BOOL DocumentRecordHandler::HandleSpreadAnimPropertiesRecord(CXaraFileRecord* pRec)
{
	// Ignore this record if we are importing into an existing file
	if (IsImporting())
		return TRUE;

	UINT32 Tag = pRec->GetTag();
	ERROR2IF(pRec == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_SPREAD_ANIMPROPS,FALSE,"I do not handle this tag type!");

	// Get a ptr to the last spread and test for NULL.
	Spread* pLastSpread = GetLastSpreadInserted();
	if (pLastSpread == NULL)
	{
		ERROR3("I don't have a 'last spread' ptr");
		return TRUE;
	}

	BOOL ok = TRUE;

	// Read the Animation information.
	UINT32	Loop				=0;				
	UINT32	GlobalDelay			=0;		
	UINT32	Dither				=0;			
	UINT32	WebPalette			=0;		
	UINT32	ColoursPalette		=0;	
	UINT32	NumColsInPalette	=0;	
	UINT32	FlagsWord			=0;

	if (ok)
		ok = pRec->ReadUINT32(&Loop);	
	if (ok)
		ok = pRec->ReadUINT32(&GlobalDelay);
	if (ok)
		ok = pRec->ReadUINT32(&Dither);
	if (ok)
		ok = pRec->ReadUINT32(&WebPalette);
	if (ok)
		ok = pRec->ReadUINT32(&ColoursPalette);
	if (ok)
		ok = pRec->ReadUINT32(&NumColsInPalette);
	if (ok)
		ok = pRec->ReadUINT32(&FlagsWord);

	// Decode the flags word into its component parts 
	BOOL UseSystemColours	= FALSE;
	BOOL BgTransparent		= FALSE;

	// <SpreadAnimationFlags : WORD>	Flags for the current animation properties.
	// SpreadAnimationFlags ::= UseSystemColours
	// UseSystemColours flag to say whether we add system colours to the exported palettes
	UseSystemColours	= ((FlagsWord & 1) ? TRUE : FALSE);

	// BgTransparent flag says whether or not we're creating a transparent GIF or not.
	// To be compatible with CX2, BgTransparent is set if (FlagsWord & 2) is zero,
	// as CX2 always does background transparent GIFs and didn't have this new bit.
	BgTransparent		= ((FlagsWord & 2) ? FALSE : TRUE);

	// Apply these details to the spread.
	if (ok && pLastSpread)
	{
		// Set the details in the layer.
		/*BOOL Worked =*/ pLastSpread->SetSpreadAnimPropertiesParam(Loop, GlobalDelay,(DITHER)Dither,(WEB_PALETTE)WebPalette,
																(PALETTE_COLOURS)ColoursPalette, NumColsInPalette,
																UseSystemColours, BgTransparent);
	}
	return ok;
};


/********************************************************************************************

>	BOOL HandleFramePropertiesRecord(CXaraFileRecord* pRec);

 	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/05/97
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the Frame Layer Delay record. Decodes the information and then
				applies it.
	SeeAlso:	-

***********************************************************************************************/

BOOL DocumentRecordHandler::HandleFramePropertiesRecord(CXaraFileRecord* pRec)
{
	ERROR2IF(pRec == NULL,FALSE,"pCXaraFileRecord is NULL");
	UINT32 Tag = pRec->GetTag();
	ERROR2IF(Tag != TAG_LAYER_FRAMEPROPS,FALSE,"I do not handle this tag type");

	// Get a ptr to the last inserted layer and check for NULL.
	Layer* pLastLayer = GetLastLayerInserted();
	if (pLastLayer == NULL)
	{
		ERROR3("I don't have a 'last layer' ptr!");
		return TRUE;
	}

	UINT32 Delay=0;
	BYTE Flags = 0;
	BOOL ok =TRUE;

	// Read the record and pass the details onto the layer.
	if (ok) ok = pRec->ReadUINT32(&Delay);
	if (ok) ok = pRec->ReadBYTE(&Flags);

	if (ok)	pLastLayer->SetFrameDelay(Delay);

	pLastLayer->SetSolid((Flags	& TAG_LAYER_FLAG_SOLID)	  != 0);
	pLastLayer->SetOverlay((Flags & TAG_LAYER_FLAG_OVERLAY) != 0);
	pLastLayer->SetHiddenFrame((Flags & TAG_LAYER_FLAG_HIDDEN) != 0);

	pLastLayer->SetFrame(TRUE); 
	// Frame layers must always be foreground layers!
	pLastLayer->SetBackground(FALSE); 
	
	return ok;
};



/********************************************************************************************
>	BOOL DocumentRecordHandler::HandleSetSentinelRecord(CXaraFileRecord* pRec)

 	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/99
	Inputs:		pRec	---		the (TAG_SETSENTINEL) record to process
	Returns:	TRUE if handled successfully.
	Purpose:	Handles insertion for a NodeSetSentinel record.
	SeeAlso:	DocumentRecordHandler::HandleRecord; DocumentRecordHandler::GetTagList;
				NodeSetSentinel
********************************************************************************************/

BOOL DocumentRecordHandler::HandleSetSentinelRecord(CXaraFileRecord* pRec)
{
	ERROR2IF(pRec == 0, FALSE,
				"DocumentRecordHandler::HandleSetSentinelRecord: no record");
	ERROR2IF_PF(pRec->GetTag() != TAG_SETSENTINEL, FALSE,
				("SetSentinel: I don't handle records with the tag (%d)", pRec->GetTag()));

	// If this is not the first SetSentinel handled then ignore the rest of the file.
	IncSetSentinelInsertedCount();
	if (GetSetSentinelInsertedCount() > 1) EndOfFile();

	// Tell the handler to insert the following records as children of the sentinel.
	SetInsertContextNode(GetDocument()->GetSetSentinel());
	InsertNextNodeAsChild();

	// Success.
	return TRUE;
}



/********************************************************************************************
>	BOOL DocumentRecordHandler::HandleSetPropertyRecord(CXaraFileRecord* pRec)

 	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/99
	Inputs:		pRec	---		the (TAG_SETPROPERTY) record to process
	Returns:	TRUE if handled successfully.
	Purpose:	Handles insertion for a NodeSetProperty record.
	SeeAlso:	DocumentRecordHandler::HandleRecord; DocumentRecordHandler::GetTagList;
				NodeSetProperty
********************************************************************************************/

BOOL DocumentRecordHandler::HandleSetPropertyRecord(CXaraFileRecord* pRec)
{
	ERROR2IF(pRec == 0, FALSE,
				"DocumentRecordHandler::HandleSetPropertyRecord: no record");
	ERROR2IF_PF(pRec->GetTag() != TAG_SETPROPERTY, FALSE,
				("SetProperty: I don't handle records with the tag (%d)", pRec->GetTag()));

	// Read in the property's set name and number of associated properties.
	String_256 strName;
	short n;
	if (!pRec->ReadUnicode(&strName) ||
		!pRec->ReadINT16(&n))
			return FALSE;

	// If the set already has a property then don't input.  This will reject
	// duplicate properties on imports. (justin)
// no longer reject duplicate nodes. We need this data! Instead duplicates are removed later (sjk 21/9/00)
//	if (GetDocument()->GetSetSentinel()->FindPropertyNode(strName) != 0)
//		return TRUE;

	// Create a new set property with a full complement of default properties.
	NodeSetProperty* pProp = new NodeSetProperty(strName);
	ERRORIF( pProp == 0 || !pProp->CreateDefaults(), _R(IDE_NOMORE_MEMORY), FALSE );

	BOOL ok = TRUE;

	// Read in each property in ascending index order.  Stop reading if we reach a
	// property index that this build doesn't know about.
	while (n--)
	{
		short nType;
		if (!pRec->ReadINT16(&nType))
		{
			ok = FALSE;
			break;
		}
		if (nType >= SGNameProp::nPropertyCount)
			break;
		if (!pProp->GetProperty(nType)->Read(pRec))
		{
			ok = FALSE;
			break;
		}
	}

	// the data in this property node is not correct
	// the default data for a property node is used instead
	// which the user shouldn't notice much of a change
	if (!ok)
	{
		delete pProp;
		pProp = NULL;
		// Create a new set property with a full complement of default properties.
		pProp = new NodeSetProperty(strName);
		ERRORIF(pProp == 0 || !pProp->CreateDefaults(), _R(IDE_NOMORE_MEMORY), FALSE );
	}

	pProp->m_Imported = TRUE;

	// Insert the property into the tree.
	if (!InsertNode(pProp))
		return FALSE;

	// If importing into a document, rather than loading, then try to create undo
	// actions for the new attachments.  In an extremely bodgy way we also create
	// undo records for the TemplateAttributes we've imported.
	if (IsImporting())
	{
		SelOperation* pSelOp = GetBaseCamelotFilter()->GetImportSelOp();
		if (pSelOp != 0)
		{
			HideNodeAction* pHideAct;
			if (AC_FAIL == HideNodeAction::Init(pSelOp, pSelOp->GetUndoActions(),
												pProp, TRUE, (Action**) &pHideAct) )
					return FALSE;

			TemplateAttribute* pAttr = GetDocument()->GetSetSentinel()->GetNameAttr(strName);
			//ERROR3IF(pAttr == 0, "DocumentRecordHandler::HandleSetPropertyRecord: no name");

			if (pAttr && AC_FAIL == HideNodeAction::Init(pSelOp, pSelOp->GetUndoActions(),
												pAttr, TRUE, (Action**) &pHideAct) )
					return FALSE;

		}
	}

	// Success.
	return TRUE;
}



/********************************************************************************************
>	BOOL DocumentRecordHandler::HandleBarPropertyRecord(CXaraFileRecord* pRec)

 	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/99
	Inputs:		pRec	---		the (TAG_BARPROPERTY) record to process
	Returns:	TRUE if handled successfully.
	Purpose:	Handles insertion for a NodeBarProperty record.
	SeeAlso:	DocumentRecordHandler::HandleRecord; DocumentRecordHandler::GetTagList;
				NodeSetProperty
********************************************************************************************/

BOOL DocumentRecordHandler::HandleBarPropertyRecord(CXaraFileRecord* pRec)
{
PORTNOTE("dialog","DocumentRecordHandler::HandleBarPropertyRecord - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pRec == 0, FALSE,
				"DocumentRecordHandler::HandleBarPropertyRecord: no record");
	ERROR2IF_PF(pRec->GetTag() != TAG_BARPROPERTY, FALSE,
				("BarProperty: I don't handle records with the tag (%d)", pRec->GetTag()));

	// Find the existing NodeBarProperty (there is always one).
	NodeSetSentinel* pSentry = Document::GetCurrent()->GetSetSentinel();
	ERROR3IF(pSentry == 0, "DocumentRecordHandler::HandleBarPropertyRecord: no sentry");
	NodeBarProperty* pBarProp = pSentry->FindBarProperty();
	ERROR3IF(pBarProp == 0, "DocumentRecordHandler::HandleBarPropertyRecord: no properties");

	g_NoOfBarsBeforeImport = pBarProp->HowMany();

	// Read in the how many array entries were previously written out, if any.
	INT32 nBars;
	if (!pRec->ReadINT32(&nBars)) return FALSE;
	if (nBars == 0) return TRUE;

	// If importing then duplicate and hide the old NodeBarProperty, otherwise 
	// clear out all existing entries.
	if (IsImporting())
	{
		SelOperation* pSelOp = GetBaseCamelotFilter()->GetImportSelOp();
		if (pSelOp != 0)
		{
			// Try to duplicate the old bar properties.
			NodeBarProperty* pCopy;
			ALLOC_WITH_FAIL(pCopy, ((NodeBarProperty*) pBarProp->SimpleCopy()), pSelOp);
			if (pCopy == 0) return FALSE;

			// Try to attach the duplicate bar properties in the right place.
			pCopy->AttachNode(pSentry, LASTCHILD);
			HideNodeAction* pHideAct;
			if (AC_FAIL == HideNodeAction::Init(pSelOp, pSelOp->GetUndoActions(),
												pCopy, TRUE, (Action**) &pHideAct))
				return FALSE;

			// Try to hide the old bar properties, and substitute the new for the old.
			if (!pSelOp->DoHideNode(pBarProp, TRUE, 0, TRUE)) return FALSE;
			pBarProp = pCopy;
		}
	}
	else
		pBarProp->Clear();

	// Read in each property in ascending index order.
	for (INT32 i = 0; i < nBars; i++)
	{
		BarDataType bdt;
		BYTE nCode;	
		if (!pRec->ReadINT32(&bdt.Spacing) || !pRec->ReadBYTE(&nCode) || !pRec->ReadBYTE(&bdt.SameSize))
			return FALSE;

		bdt.IsLive = nCode & 1;		
		bdt.IsHorizontal = (nCode >> 1) & 1;
		bdt.RequiresShuffle = (nCode >> 2) & 1;
		bdt.ButtonsExtend = (nCode >> 3) & 1;
		bdt.ButtonsScale = (nCode >> 4) & 1;
		bdt.GroupsStretch = (nCode >> 5) & 1;

		if (pBarProp->HowMany() < MAX_BAR_PROPERTIES) pBarProp->Add(bdt);
	} 
#endif	
	// Success.
	return TRUE;
}

/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleCurrentAttrsRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/2004
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a document record
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleCurrentAttrsRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(!(pCXaraFileRecord->GetTag() == TAG_CURRENTATTRIBUTES || pCXaraFileRecord->GetTag() == TAG_CURRENTATTRIBUTEBOUNDS), FALSE, "I don't handle this tag type");

	BOOL bOK = TRUE;

	if (pCXaraFileRecord->GetTag() == TAG_CURRENTATTRIBUTES)
	{
		// Something clever needs to be done here to allow the child attribute TAGs
		// of this TAG to behave differently, and make themselves current rather than
		// adding themselves to the document tree.
		//
		// Set the special flag that makes InsertNode behave differently!
		// Keep on going until we get a TAG_UP
		// Then set the flag back again...
		// (Bodgy but quicker and simpler than deriving a special filter class to behave that way)
		CCRuntimeClass* pGroupType = NULL;

		BYTE groupid = 0;
		bOK = pCXaraFileRecord->ReadBYTE(&groupid);

		if (groupid == ATTRIBUTEGROUP_INK)
			pGroupType = CC_RUNTIME_CLASS(NodeRenderableInk);

		else if (groupid == ATTRIBUTEGROUP_TEXT)
			pGroupType = CC_RUNTIME_CLASS(BaseTextClass);

		else
		{
			ERROR3("Unknown Attribute Group ID");
//			bOK = FALSE;
			pGroupType = NULL;	// Skip this current attr group
		}

		if (bOK && pGroupType!=NULL)
		{
			pBaseCamelotFilter->SetInsertMode(INSERTMODE_SETCURRENTATTRIBUTE, pGroupType);
			bOK = pBaseCamelotFilter->ReadFileUntil(TAG_UP);
			pBaseCamelotFilter->SetInsertMode(INSERTMODE_ATTACHTOTREE);
		}
	}
	else if (pCXaraFileRecord->GetTag() == TAG_CURRENTATTRIBUTEBOUNDS)
	{
		NodeAttribute* pLastCurrentAttr = pBaseCamelotFilter->GetLastCurrentAttrInserted();
		if (pLastCurrentAttr && pLastCurrentAttr->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)))
		{
			DocRect newBounds;
			if (bOK) bOK = pCXaraFileRecord->ReadCoord(&newBounds.lo);
			if (bOK) bOK = pCXaraFileRecord->ReadCoord(&newBounds.hi);
			((AttrFillGeometry*)pLastCurrentAttr)->SetBoundingRect(newBounds);
		}
	}

	return bOK;
}




/********************************************************************************************

>	BOOL DocumentRecordHandler::HandleDuplicationOffsetRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a TAG_DUPLICATIONOFFSET record
	SeeAlso:	-

********************************************************************************************/

BOOL DocumentRecordHandler::HandleDuplicationOffsetRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_DUPLICATIONOFFSET,FALSE,"I don't handle this tag type");

	BOOL bOK = TRUE;
	Document* pDoc = Document::GetCurrent();
	if (pDoc)
	{
		DocCoord newOffset;
		if (bOK) bOK = pCXaraFileRecord->ReadINT32(&(newOffset.x));
		if (bOK) bOK = pCXaraFileRecord->ReadINT32(&(newOffset.y));
		if (bOK) pDoc->SetDuplicationOffset(newOffset);
	}

	return bOK;
}




