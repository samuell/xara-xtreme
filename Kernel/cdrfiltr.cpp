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
// *********CDR file filter

/*
*/

// this line disables extrudes as they don't *quite* work yet - corel still need some
// interegation about what's going on. It seems the object is pre-transformed by some
// random values. Weird. #define this to include the stuff.
#undef CDR_DOEXTRUDES

#include "camtypes.h"
#include "cdrfiltr.h"
#include "oilfltrs.h"
//#include "ben.h"
#include "ccfile.h"
#include "progress.h"
#include "cdrcol.h"
#include "rifffile.h"
#include "cdrform.h"
#include "fixmem.h"
#include "nodepath.h"
#include "group.h"
#include "page.h"
#include "spread.h"
#include "nodershp.h"
//#include "resource.h"
#include "fixst256.h"
#include "ndoptmz.h"
#include "collist.h"
#include "layer.h"
#include "sglayer.h"
#include "sprdmsg.h"
#include "nodebmp.h"
#include "nodetxts.h"
//#include "textinfo.h"
#include "zoomops.h"
#include "docview.h"
#include "nodemold.h"
#include "moldshap.h"
#include "ndmldpth.h"
#include "nodemldr.h"
//#include "nev.h"
#include "fontman.h"
#include "app.h"
//#include "cmxres.h"
#include "cmxifltr.h"		// so we can pass stuff to it nicely

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(CDRFilter, VectorFilter)
CC_IMPLEMENT_MEMDUMP(CDRAttributeStore, List)
CC_IMPLEMENT_MEMDUMP(CDRAttributeStoredItem, ListItem)
CC_IMPLEMENT_MEMDUMP(CDRArrowheadStore, List)
CC_IMPLEMENT_MEMDUMP(CDRArrowheadStoredItem, ListItem)
CC_IMPLEMENT_MEMDUMP(CDRBitmapStore, List)
CC_IMPLEMENT_MEMDUMP(CDRBitmapStoredItem, ListItem)
CC_IMPLEMENT_MEMDUMP(CDRVectorStore, List)
CC_IMPLEMENT_MEMDUMP(CDRVectorStoredItem, ListItem)
CC_IMPLEMENT_MEMDUMP(CDRFontnameStore, List)
CC_IMPLEMENT_MEMDUMP(CDRFontnameStoredItem, ListItem)
CC_IMPLEMENT_MEMDUMP(CDRLayerList, List)
CC_IMPLEMENT_MEMDUMP(CDRLayerListItem, ListItem)
CC_IMPLEMENT_MEMDUMP(CDRPageList, List)
CC_IMPLEMENT_MEMDUMP(CDRPageListItem, ListItem)
CC_IMPLEMENT_MEMDUMP(CDRActionList, List)
CC_IMPLEMENT_DYNAMIC(CDRActionListItem, ListItem)
CC_IMPLEMENT_DYNAMIC(CDRActionTransform, CDRActionListItem)
CC_IMPLEMENT_DYNAMIC(CDRActionGroupEnd, CDRActionListItem)
CC_IMPLEMENT_MEMDUMP(CDRTransformConvertor, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(CDRTransformConvertor5, CDRTransformConvertor)
CC_IMPLEMENT_MEMDUMP(CDRTransformConvertor4, CDRTransformConvertor)
CC_IMPLEMENT_MEMDUMP(CDRTransformConvertor3, CDRTransformConvertor)
CC_IMPLEMENT_MEMDUMP(CDRTextOnPathLink, ListItem)


#define new CAM_DEBUG_NEW

#define PROGRESSUPDATE_MASK 0x7f

INT32 CDRFilter::BitmapNumber = 0;
INT32 CDRFilter::PatternNumber = 0;

/********************************************************************************************

>	static void CDRFilter::UpdateProgress(BOOL Now = FALSE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		None
	Returns:	None
	Purpose:	Updating the progress everysooften - called at every significant place
				to get a nice smooth update on that progress bar thingy
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::UpdateProgress(BOOL Now)
{
	BOOL Result = TRUE;
	
	if(Now)
	{
		Result = ContinueSlowJob(RIFF->ProgressNow());
		ProgressCount = 0;
	}
	else
	{
		if((ProgressCount & PROGRESSUPDATE_MASK) == 0) {
			Result = ContinueSlowJob(RIFF->ProgressNow());
		}

		ProgressCount++;
	}

	if(!Result)
	{
		Error::ClearError();		// because this may not be handled the first time around
		Error::SetError(_R(IDN_USER_CANCELLED));
	}
	
	return Result;
}

/********************************************************************************************

>	CDRFilter::CDRFilter(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Constructor
	SeeAlso:	Filter

********************************************************************************************/

CDRFilter::CDRFilter(void)
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_CDRFILTER_FILTERNAME));
	FilterInfo.Load(_R(IDT_CDRFILTER_FILTERINFO));
	FilterID = FILTERID_CDR;

	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;

	Version = CDRVERSION_NULL;
}


/********************************************************************************************

>	BOOL CDRFilter::Init(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Initialisation
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::Init(void)
{
	// Get the OILFilter object
	pOILFilter = new CDROILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// make sure we'll complain about life if HowCompatable isn't called
	Version  = CDRVERSION_NULL;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 CDRFilter::HowCompatible(PathName& Filename, ADDR  HeaderStart,
		UINT32 HeaderSize, UINT32 FileSize)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Examines a file to see how compatable it is with this filter.
	SeeAlso:	Filter

********************************************************************************************/

INT32 CDRFilter::HowCompatible(PathName& Filename, ADDR  HeaderStart,
		UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	BOOL ExtensionMatches = pOILFilter->DoesExtensionOfPathNameMatch(&Filename);

	// check for version 2 files if the extension matches...
	if (ExtensionMatches)
	{
		if(_tcsncmp((char *)HeaderStart, "WL", 2) == 0)
			return 10;			// a version 2.xx CDR file
	}

	// check for version 3 - 5 files...
	FOURCC FormType;

	FormType = RIFFFile::CheckRIFFHeader(HeaderStart, FileSize);

	Version  = CDRVERSION_NULL;
	
	switch (FormType)
	{
		case cdrT_CDR:
			Version = CDRVERSION_3;
			break;
		
		case cdrT_CDR4:
		case cdrT_CDST:			// version 4 template
			Version = CDRVERSION_4;
			break;

		case cdrT_CDR5:
		case cdrT_CDT5:
			Version = CDRVERSION_5;
			break;

		case cdrT_CDR6:
		case cdrT_CDT6:
			Version = CDRVERSION_6;
			break;

		default:
			break;
	}

	INT32 HowMuchWeLikedThisFile = 0;

	if(Version != CDRVERSION_NULL)
	{
		HowMuchWeLikedThisFile = 10;		// loved the file

		// check to see if there is a CMX file embedded in there
		if(HasCDRFileGotCMXFile(&Filename))
		{
			HowMuchWeLikedThisFile = 5;		// didn't like it so much then -- let the CMX filter grab it
		}
	}

	return HowMuchWeLikedThisFile;
}

/********************************************************************************************

>	BOOL CDRFilter::DoImport(SelOperation *Op, CCLexFile *pDiskFile, Document *DestDoc, 
		BOOL AutoChosen, ImportPosition *Pos,
		KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Does the import of a CDR (.cdr or .cdt)
	SeeAlso:	Filter

********************************************************************************************/

#define CDRFILTER_DI_RETURNERROR {Success = FALSE; return ClearUp();}

BOOL CDRFilter::DoImport(SelOperation *Op, CCLexFile *pDiskFile, Document *DestDoc, 
		BOOL AutoChosen, ImportPosition *Pos,
		KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)
{
	// check to see if we've got a CMX file in there, but only if we weren't autochosen
	if(!AutoChosen && HasCDRFileGotCMXFile(NULL, pDiskFile))
	{
		TRACEUSER( "Ben", _T(">>> loading CDR file with new CMXImportFilter\n"));
		// right, somehow we got here -- probably because the user specifically
		// asked to see CorelDRAW files only
		CMXImportFilter *pFilter = new CMXImportFilter;

		if(pFilter == NULL)
			return FALSE;

		BOOL ok = pFilter->DoImport(Op, pDiskFile, DestDoc, AutoChosen, Pos);

		delete pFilter;
		pFilter = NULL;

		return ok;
	}

	// normal CDR import stuff
	BOOL ExtensionMatches = pOILFilter->DoesExtensionOfFileMatch(pDiskFile);

	RIFFFile RIFFo;

TRACEUSER( "Ben", _T("In CDRFilter::DoImport\n"));
	//ERROR2IF(Version == CDRVERSION_NULL, FALSE, "CDRFilter::HowCompatible failed to set recognised version\n");

	// in debug builds, warn about fills hanging around
	ERROR3IF(Fills.IsEmpty() == FALSE, "Fills store not empty");
	ERROR3IF(Outlines.IsEmpty() == FALSE, "Outlines store not empty");
	ERROR3IF(Styles.IsEmpty() == FALSE, "Styles store not empty");
	ERROR3IF(Fonts.IsEmpty() == FALSE, "Fonts store not empty");
	ERROR3IF(Arrowheads.IsEmpty() == FALSE, "Arrowheads store not empty");
	ERROR3IF(Patterns.IsEmpty() == FALSE, "Patterns store not empty");
	ERROR3IF(Bitmaps.IsEmpty() == FALSE, "Bitmap store not empty");
	ERROR3IF(Vectors.IsEmpty() == FALSE, "Vector store not empty");
	ERROR3IF(TextBBoxes.IsEmpty() == FALSE, "TextBBoxes list not empty");
	ERROR3IF(TextV4.IsEmpty() == FALSE, "TextV4 store not empty");
	ERROR3IF(TextOnPathLinks.IsEmpty() == FALSE, "TextOnPathLinks not empty");

	// in retail builds, just vape any contents
	Fills.DeleteAll();
	Outlines.DeleteAll();
	Styles.DeleteAll();
	Fonts.DeleteAll();
	Arrowheads.DeleteAll();
	Patterns.DeleteAll();
	Bitmaps.DeleteAll();
	Vectors.DeleteAll();
	TextV4.DeleteAll();
	Pages.DeleteAll();
	Actions.DeleteAll();
	TextBBoxes.DeleteAll();
	TextOnPathLinks.DeleteAll();

	// set a few flags and initialise some variables
	pDocument = DestDoc;
	Success = TRUE;
	SlowJobStarted = FALSE;
	TranslateByX = 0;
	TranslateByY = 0;
	PageX = 0;
	PageY = 0;
	pLevelNodeList = 0;
	DoneDoc = FALSE;
	DoneMasterPage = FALSE;
	InMasterPage = FALSE;
	ThisPage = 0;
	ThisLayer = 0;
	pMadeNode = 0;
	ObjectsConverted = 0;
	ObjectsFormatWrong = 0;
	ObjectsUnknown = 0;
	EnvelopesApproximated = 0;
	ObjDataBlock = 0;
	ObjDataBlockSize = 0;
	AttrsAlreadyApplied = FALSE;
	TransformChunk = 0;
	FontTableV3 = 0;
	FontStylesV3 = 0;
	LinkTable = 0;
	ThisOp = Op;
	LinkTransformsExist = FALSE;
	TheDocument = DestDoc;

	// if the extension matches, check it for being a 2.xx file
	if(ExtensionMatches)
	{
		// get the first few bytes
		TCHAR FirstBit[8];

		if(pDiskFile->read((void *)FirstBit, sizeof(FirstBit)).bad())
			return FALSE;

		ERROR1IF(_tcsncmp((char *)FirstBit, "WL", 2) == 0, FALSE, _R(IDT_CDRFILTER_ISAV2FILE));
	
		// let's pop the file back to the beginning
		if(pDiskFile->seek(0).bad())
			return FALSE;
	}

	// sort out the RIFF file
	RIFF = &RIFFo;

	FOURCC FormType = RIFF->Init(pDiskFile);

	if(FormType == 1)		// error
	{
		CDRFILTER_DI_RETURNERROR;
	}
	
	// sort out which version we're on
	switch(FormType)
	{
		case cdrT_CDR:
			Version = CDRVERSION_3;
			break;
		
		case cdrT_CDR4:
		case cdrT_CDST:			// version 4 template
			Version = CDRVERSION_4;
			break;

		case cdrT_CDR5:
		case cdrT_CDT5:
			Version = CDRVERSION_5;
			break;

		case cdrT_CDR6:
		case cdrT_CDT6:
			// version 6 file -- we don't support these
			ERROR1(FALSE, _R(IDE_CDRFILTER_CDR6));
			break;

		default:
TRACEUSER( "Ben", _T("Attempted to load something which wasn't a CDR file. I don't think so..."));
			ERROR1(FALSE, _R(IDE_CDRFILTER_NOTCDR));
			break;
	}

	// sort out a colour list
	ColList = DestDoc->GetIndexedColours();
	
	// do we want to import with layers?
	UseLayers = FALSE;
#ifdef WEBSTER
	// In Webster, just use the default preference settings as these should be set to
	// ImportWithLayers to False.
	if (
		(!DestDoc->IsImporting() && Filter::OpenWithLayers) ||
		(DestDoc->IsImporting() && Filter::ImportWithLayers)
		)
		UseLayers = TRUE;
#else
	// In Camelot, we must check to see if the document is being imported and if so if
	// there are any frames present. If there are then don't use layers, if there are
	// then use the preference setting.
	if (DestDoc->IsImporting())
	{
		Spread * pTheSpread = DestDoc->GetSelectedSpread();
		Layer * pFrame = NULL;
		if (pTheSpread != NULL)
			pFrame = pTheSpread->FindFirstFrameLayer();
		if (pFrame != NULL)
			UseLayers = FALSE;						// Frames present so do not import layers
		else
			UseLayers = Filter::ImportWithLayers;	// No frames so use the preference setting
	}
	else
	{
		UseLayers = Filter::OpenWithLayers;
	}
#endif

	// If importing without layers, make sure there is a layer to put things on, and make one if not
	if(UseLayers == FALSE)
	{
		if (!MakeSureLayerExists(DestDoc))
		{
			// clean up and return and error
			CDRFILTER_DI_RETURNERROR;
		}
	}

	// For now, position Draw objects on 1st page of spread 1
	pSpread = DestDoc->GetSelectedSpread();
	pPage = (Page *) pSpread->FindFirstPageInSpread();
	
	// Use bottom left of page as origin
	DocRect PageRect = pPage->GetPageRect();
	Origin = PageRect.lo;

	// OK, let's get into the swing of things!
	
	// Get a default set of attributes for ink objects.
	if (!SetUpCurrentAttrs())
		return FALSE;

	// Let start off by setting the progress indicatorx
	UINT32 id;
	switch(Version)
	{
		case CDRVERSION_4:	id = _R(IDT_IMPORTMSG_CDR4);	break;
		case CDRVERSION_5:	id = _R(IDT_IMPORTMSG_CDR5);	break;
		default:			id = _R(IDT_IMPORTMSG_CDR3);	break;
	}
	if(!DestDoc->IsImporting())
		id += _R(IDT_OPENMSG_CDR3) - _R(IDT_IMPORTMSG_CDR3);
	//String_64 ImportMessage(id);
	String_64 ImportMessage = GetImportProgressString(pDiskFile, id);
	
	// this slow job doesn't have a delay because if the CDR file has a bitmap at the
	// beginning, ContinueSlowJob can't be called for rather a long time (sometimes)
	// and you get 'Nothing Selected' instead of a message telling you what's happening.
	BeginSlowJob(RIFF->ProgressLimit(), FALSE, &ImportMessage);
	SlowJobStarted = TRUE;

	// loop through looking at top level lists, and dispatch them to various handlers
	do
	{
		// get the next object in the file
		if(!RIFF->NextObject())
			CDRFILTER_DI_RETURNERROR;

		if(RIFF->GetObjType() == RIFFOBJECTTYPE_LISTSTART)
		{
			switch(RIFF->GetObjChunkType())
			{
				case cdrT_doc:
					if(!ProcessDoc())
					{
TRACEUSER( "Ben", _T("ProcessDoc returned FALSE\n"));
						CDRFILTER_DI_RETURNERROR;
					}
					break;

				case cdrT_page:
					if(!ProcessPage())
					{
TRACEUSER( "Ben", _T("ProcessPage returned FALSE\n"));
						CDRFILTER_DI_RETURNERROR;
					}
					break;

				default:
					// we're not interested in any other root level list, so skip it
					if(!RIFF->SkipToListEnd(RIFF->GetObjLevel()))
						CDRFILTER_DI_RETURNERROR;
					break;
			}
		}

		if(!UpdateProgress())
			return FALSE;
	} while(RIFF->GetObjType() != RIFFOBJECTTYPE_FILEEND);

	// make sure we're at the end
	ContinueSlowJob(RIFF->ProgressLimit());

	// attach paths to text stories
	if(!AttachTextToPaths())
		return FALSE;

	// only stick objects into the document if some were converted
	if(ObjectsConverted > 0)
	{
	
		// check that there were at least two pages
		if(Pages.GetCount() < 1)
		{
	TRACEUSER( "Ben", _T("not enough pages in file\n"));
			CDRFILTER_DI_RETURNERROR;
		}
	
		// get the pointer to the list of levels on the master pages
		CDRLayerList *MasterLayers;
		CDRPageListItem *En = (CDRPageListItem *)Pages.GetHead();

		if(En == 0)
			CDRFILTER_DI_RETURNERROR;

		MasterLayers = &En->Layers;
	
		// get the list of levels on the actual page
		CDRLayerList *PageLayers;
		En = (CDRPageListItem *)Pages.GetNext(En);

		if(Version != CDRVERSION_3)
		{
			if(En == 0)
				CDRFILTER_DI_RETURNERROR;

			PageLayers = &En->Layers;

			// check that they have the same number of layers in their lists
			if(MasterLayers->GetCount() != PageLayers->GetCount())
			{
		TRACEUSER( "Ben", _T("not the same number of layers on master page and on page\n"));
				CDRFILTER_DI_RETURNERROR;
			}
		} else
		{
			PageLayers = 0;
		}

		if(MasterLayers->GetCount() <= 0)
			CDRFILTER_DI_RETURNERROR;

		// if it's a CDR 3 file set the page size and translation things
		if(Version == CDRVERSION_3)
		{
			// set the page size?
			if(!DestDoc->IsImporting() && PageX > 0 && PageY > 0)
			{
// Make the pasteboard an extra page width either side of the page and an extra
// half page height above and below the page. That should be nicely proportional.

				INT32 ms = (PageX > PageY)?PageX:PageY;
	
				// we're opening the file, so set the page size
				if (pSpread)
				{
					BOOL ok = pSpread->SetPageSize(PageX, PageY, ms, 0, FALSE, TRUE);
					// Must set the origin of the grid to the bottom left corner of the
					// union rectangle of all pages on the spread as this is where the x,y
					// measurements are made from.
					DocRect PagesRect;
					NodeGrid* pGrid = pSpread->FindFirstDefaultGridInSpread();
					ok = ok && pSpread->GetPagesRect(&PagesRect);
					if (pGrid)
						pGrid->SetOrigin(PagesRect.lox, PagesRect.loy);
					
					if (!ok)
						CDRFILTER_DI_RETURNERROR;
				}
			}
		
			DocRect PageRect = pPage->GetPageRect();
			Origin = PageRect.lo;

			TranslateByX = Origin.x + (PageX / 2);
			TranslateByY = Origin.y + (PageY / 2);
		}

		// set up pointers to the layers
		CDRLayerListItem *MasterEn = (CDRLayerListItem *)MasterLayers->GetTail();
		CDRLayerListItem *PageEn = 0;
		if(PageLayers != 0)
			PageEn = (CDRLayerListItem *)PageLayers->GetTail();

		EndSlowJob();
		BeginSlowJob();

		// insert everything into the document
		if(UseLayers)
		{
			// go through inserting every layer which has sometime on it
			while(MasterEn != 0)
			{
				// check to see if we have objects to import
				if(MasterEn->Objects != 0 || (PageEn != 0 && PageEn->Objects != 0))
				{
					// OK, we've got some objects to import, create a layer to put them in
					Layer *pLayer = new Layer();

					if(pLayer == 0)
						CDRFILTER_DI_RETURNERROR;

					// set it's name and attributes
					pLayer->SetLayerID(MasterEn->Name);
					pLayer->SetVisible(MasterEn->IsVisible);
					pLayer->SetLocked(MasterEn->IsLocked);
					pLayer->SetActive(FALSE);
					pLayer->SetPrintable(TRUE);
					pLayer->SetBackground(FALSE);
					pLayer->SetOutline(FALSE);

					// stick objects below it
					Node *Objects = 0;

					if(MasterEn->Objects != 0)
					{
						Objects = MasterEn->Objects;
						MasterEn->LayerInDocument = TRUE;
					}
					else
					{
						Objects = PageEn->Objects;
						PageEn->LayerInDocument = TRUE;
					}

					ERROR3IF(Objects == 0, "Er, tried to make a layer for objects which didn't exist");

					// attach the objects to the layer
					Objects->InsertChainSimple(pLayer, FIRSTCHILD);

					// if it's a CDR 3 file and translate it
					if(Version == CDRVERSION_3)
					{
						Trans2DMatrix Trans(TranslateByX, TranslateByY);
						pLayer->Transform(Trans);
					}
					
					// insert the layer...
					if (!Op->DoInsertNewNode(pLayer, (Node *)pSpread, LASTCHILD, TRUE))
					{
						// It didn't work - delete the sub-tree we just created
						pLayer->CascadeDelete();
						delete pLayer;
						pLayer = NULL;
						CDRFILTER_DI_RETURNERROR;
					}

					// make sure it's name is unique
					pLayer->EnsureUniqueLayerID();
					
					// ... and finally optimise it's attributes
					if(!pLayer->OptimiseAttributes())
						CDRFILTER_DI_RETURNERROR;
				}
	
				// get the next (previous) layer to import
				MasterEn = (CDRLayerListItem *)MasterLayers->GetPrev(MasterEn);
				if(PageEn != 0)
					PageEn = (CDRLayerListItem *)PageLayers->GetPrev(PageEn);
			}		

			// get the insertation node set up
			DestDoc->ResetInsertionPosition();
		}
		else 
		{
			// get a group node to put everything in as we're not using layers here.
			NodeGroup *pGroup = new NodeGroup;
			if(pGroup == 0)
				CDRFILTER_DI_RETURNERROR;
	
			// run through the layers putting in the stuff - remember they're in reverse order
			while(MasterEn != 0)
			{
		TRACEUSER( "Ben", _T("Putting layer '%s' into document\n"), (TCHAR *)MasterEn->Name);		
		
				// put the master pages objects onto this group
				if(MasterEn->Objects != 0)
				{
					MasterEn->Objects->InsertChainSimple(pGroup, LASTCHILD);
		
					MasterEn->LayerInDocument = TRUE;
				}

				// put the page objects into this group
				if(PageEn != 0 && PageEn->Objects != 0)
				{
					PageEn->Objects->InsertChainSimple(pGroup, LASTCHILD);

					PageEn->LayerInDocument = TRUE;
				}

		
				// get the next (previous) layer to import
				MasterEn = (CDRLayerListItem *)MasterLayers->GetPrev(MasterEn);
				if(PageEn != 0)
					PageEn = (CDRLayerListItem *)PageLayers->GetPrev(PageEn);
			}

			// if it's a CDR 3 file translate it
			if(Version == CDRVERSION_3)
			{
				Trans2DMatrix Trans(TranslateByX, TranslateByY);
				pGroup->Transform(Trans);
			}

			// check to see if the group node has more than one child
			// if it doesn't just insert the child and not the group, and then
			// delete the group node afterwards
			NodeRenderableBounded *InsertNode = pGroup;

			NodeRenderableBounded *FirstChild = (NodeRenderableBounded *)pGroup->FindFirstChild();

			if(FirstChild != 0)
			{
				// if the first child hasn't got a sibling, then there's only one
				// child to the group, so just insert the child
				if(FirstChild->FindNext() == 0)
					InsertNode = FirstChild;
			}

			// and slap the group into the document
			if (!Op->DoInsertNewNode(InsertNode, pSpread, TRUE))
			{
				// It didn't work - delete the sub-tree we just created
				pGroup->CascadeDelete();
				delete pGroup;
				pGroup = NULL;
				CDRFILTER_DI_RETURNERROR;
			}

			if(InsertNode != pGroup)
				delete pGroup;

			// optimise the attributes in our nice new group
			if(!InsertNode->OptimiseAttributes())
				CDRFILTER_DI_RETURNERROR;
		}
	}
	else
	{
		// if we were opening the document, then we need make a layer and set the insertation node
		if(!DestDoc->IsImporting())
		{
			if (!MakeSureLayerExists(DestDoc))
				CDRFILTER_DI_RETURNERROR;

			DestDoc->ResetInsertionPosition();
		}
	}

	// add some standard colours
	if(!AddStandardColourSet(pDocument))
		CDRFILTER_DI_RETURNERROR;

	// do a quick post import on it
	DestDoc->PostImport();
	
	// find out how many dodgy objects were encountered along the way
	INT32 ObjNotConv = ObjectsFormatWrong + ObjectsUnknown;

	if(ObjNotConv > 0)
	{
		// tell the user that some objects couldn't be converted
		String_256 WarnMsg;
		WarnMsg.MakeMsg((ObjNotConv == 1)?_R(IDT_CDRFILTER_OBJWARNING1):_R(IDT_CDRFILTER_OBJWARNING), ObjNotConv);
		Error::SetError(0, WarnMsg, 0);
		InformWarning();
		Error::ClearError();
	}

	if(EnvelopesApproximated > 0)
	{
		// tell the user that some envelopes were approximated
		String_256 WarnMsg;
		WarnMsg.MakeMsg((EnvelopesApproximated == 1)?_R(IDT_CDRFILTER_ENVAPPROXWARNING1):_R(IDT_CDRFILTER_ENVAPPROXWARNING), EnvelopesApproximated);
		Error::SetError(0, WarnMsg, 0);
		InformWarning();
		Error::ClearError();
	}

TRACEUSER( "Ben", _T("Do import finished\n"));
	return ClearUp();
}

/********************************************************************************************

>	BOOL CDRFilter::ProcessDoc(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	Processes the root level doc chunk of a CDR file
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::ProcessDoc(void)
{
	BOOL InFillTable = FALSE;
	BOOL InOutlineTable = FALSE;
	BOOL InStyleTable = FALSE;
	BOOL InFontTable = FALSE;
	BOOL InArrowheadTable = FALSE;
	BOOL InPatternTable = FALSE;
	BOOL InBitmapTable = FALSE;
	BOOL GotPageInfo = FALSE;

	ADDR VectorListContents = 0;
	INT32 VectorListContentsSize;

TRACEUSER( "Ben", _T("In ProcessDoc\n"));

	// store the level we're working on
	UINT32 StartLevel = RIFF->GetObjLevel();

	do
	{
		switch(RIFF->GetObjType())
		{
			case RIFFOBJECTTYPE_LISTSTART:
				// list start - attributes are stored in lists
				switch(RIFF->GetObjChunkType())
				{
					case cdrT_filt:		InFillTable = TRUE;			break;
					case cdrT_otlt:		InOutlineTable = TRUE;		break;
					case cdrT_stlt:		InStyleTable = TRUE;		break;
					case cdrT_fntt:		InFontTable = TRUE;			break;
					case cdrT_arrt:		InArrowheadTable = TRUE;	break;
					case cdrT_bpft:		InPatternTable = TRUE;		break;
					case cdrT_bmpt:		InBitmapTable = TRUE;		break;
					default:										break;

					case cdrT_vect:
						// get the contents of this list for later
						if(!RIFF->GetListContents(&VectorListContents, &VectorListContentsSize))
							return FALSE;

						// and skip to the end of it
						if(!RIFF->SkipToListEnd(RIFF->GetObjLevel()))
							return FALSE;
						break;
					
					case cdrT_btxt:
						if(Version == CDRVERSION_4)
						{
							// if we're in a version 4 file, this contains text for the paragraph text objects
							if(!ProcessTextList4())
								return FALSE;
						}
						break;
				}
				break;

			case RIFFOBJECTTYPE_LISTEND:
				switch(RIFF->GetObjChunkType())
				{
					case cdrT_filt:		InFillTable = FALSE;		break;
					case cdrT_otlt:		InOutlineTable = FALSE;		break;
					case cdrT_stlt:		InStyleTable = FALSE;		break;
					case cdrT_fntt:		InFontTable = FALSE;		break;
					case cdrT_arrt:		InArrowheadTable = FALSE;	break;
					case cdrT_bpft:		InPatternTable = FALSE;		break;
					case cdrT_bmpt:		InBitmapTable = FALSE;		break;
					default:										break;
				}
				break;

			case RIFFOBJECTTYPE_CHUNK:
				// got a chunk to be interested about!
				switch(RIFF->GetObjChunkType())
				{
					case cdrT_fill:
						if(InFillTable)
							Fills.AddChunkToStore(RIFF);
						break;

					case cdrT_outl:
						if(InOutlineTable)
							Outlines.AddChunkToStore(RIFF);
						break;

					case cdrT_styd:
						if(InStyleTable)
							Styles.AddChunkToStore(RIFF);
						break;

					case cdrT_font:
						if(InFontTable)
							Fonts.AddChunkToStore(RIFF, Version);
						break;

					case cdrT_arrw:
						if(InArrowheadTable)
							Arrowheads.AddChunkToStore(RIFF);
						break;

					case cdrT_bmpf:
						if(InPatternTable)
							Patterns.AddChunkToStore(RIFF, TRUE, this);

						UpdateProgress(TRUE);
						break;

					case cdrT_bmp:
						// a couple of forced progress updates here to ensure a
						// resonably often updated progress bar
						UpdateProgress(TRUE);

						if(InBitmapTable)
							Bitmaps.AddChunkToStore(RIFF, FALSE, this);

						UpdateProgress(TRUE);
						break;

					case cdrT_mcfg:
						{	// get the chunk and read the page size
							if(GotPageInfo)		// if we've already got it, then there's a problem with the file
							{
TRACEUSER( "Ben", _T("mcfg chunk encountered twice in doc list\n"));								
								ERROR1(FALSE, _R(IDE_CDRFILTER_FORMATERROR));
							}

							if(!RIFF->AquireChunkData())
								return FALSE;

							cdrfPageInfo *i = (cdrfPageInfo *)RIFF->GetAquiredData();

							ERROR3IF(i == 0, "No aquired data returned from RIFF chunk");

							PageX = CDRDATA_WORD(i->PageX) * CDRCOORDS_TO_MILLIPOINTS;
							PageY = CDRDATA_WORD(i->PageY) * CDRCOORDS_TO_MILLIPOINTS;
							Landscape = (CDRDATA_WORD(i->Orientation) == cdrf_ORIENTATION_LANDSCAPE)?TRUE:FALSE;

							GotPageInfo = TRUE;
						}
						break;

					case cdrT_tspc:		// font style table for version 3
						if(Version == CDRVERSION_3)
						{
							// store the font styles
							if((FontStylesV3 = (ADDR)CCMalloc(RIFF->GetObjSize())) == 0)
								return FALSE;

							if(!RIFF->GetChunkData(FontStylesV3, RIFF->GetObjSize()))
								return FALSE;
						}
						break;

					case cdrT_ftbl:		// font name table for version 3
						if(Version == CDRVERSION_3)
						{
							// store the font names
							if((FontTableV3 = (ADDR)CCMalloc(RIFF->GetObjSize())) == 0)
								return FALSE;

							if(!RIFF->GetChunkData(FontTableV3, RIFF->GetObjSize()))
								return FALSE;

							// store number of entries
							FontTableEntriesV3 = RIFF->GetObjSize() / sizeof(cdrfFontTableEntryV3);
							
							// cache the font names
							cdrfFontTableEntryV3 *FT = (cdrfFontTableEntryV3 *)FontTableV3;
							for(INT32 l = 0; l < FontTableEntriesV3; l++)
							{
								if(FT[l].Name[0] != '\0' && FT[l].Name[0] != ' ')
								{
									//GetApplication()->GetFontManager()->CacheNamedFont(&String_64((TCHAR *)&FT[l].Name));
								
									Fonts.AddFontToStore(FT[l].Reference, (TCHAR *)&FT[l].Name);
								}
							}

						}
						break;

					case cdrT_lnkt:
						// store the link table - it's a pretty useful piece of kit
						if((LinkTable = (ADDR)CCMalloc(RIFF->GetObjSize())) == 0)
							return FALSE;

						if(!RIFF->GetChunkData(LinkTable, RIFF->GetObjSize()))
							return FALSE;

						// generate the links from paths to text text from it
						if(!MakeTextOnPathLinkList())
							return FALSE;
						break;

					default:
						break;
				}
				break;

			default:
				break;
		}
		UpdateProgress();
		
		if(!RIFF->NextObject())
			return FALSE;
		
		ERROR3IF(RIFF->GetObjType() == RIFFOBJECTTYPE_FILEEND, "Unexpected end of RIFF file in ProcessDoc");
	} while(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > StartLevel);

	// process the vector list, if we got one
	if(VectorListContents != 0)
	{
		if(!ProcessVectorList(VectorListContents, VectorListContentsSize))
		{
			CCFree(VectorListContents);
			return FALSE;
		}

		CCFree(VectorListContents);
	}

	// if this isn't a version 3 file, check that we got some page info
	if(Version != CDRVERSION_3)
	{
		if(GotPageInfo == FALSE)
		{
TRACEUSER( "Ben", _T("mcfg chunk not found in doc list\n"));
			ERROR1(FALSE, _R(IDE_CDRFILTER_FORMATERROR));
		
		}

		// set the page size?
		if(!pDocument->IsImporting() && PageX > 0 && PageY > 0)
		{
// Make the pasteboard an extra page width either side of the page and an extra
// half page height above and below the page. That should be nicely proportional.

			INT32 ms = (PageX > PageY)?PageX:PageY;

			// we're opening the file, so set the page size
			if (pSpread)
			{
				BOOL ok = pSpread->SetPageSize(PageX, PageY, ms, 0, FALSE, TRUE);

				// Must set the origin of the grid to the bottom left corner of the
				// union rectangle of all pages on the spread as this is where the x,y
				// measurements are made from.
				DocRect PagesRect;
				NodeGrid* pGrid = pSpread->FindFirstDefaultGridInSpread();
				ok = ok && pSpread->GetPagesRect(&PagesRect);
				if (pGrid)
					pGrid->SetOrigin(PagesRect.lox, PagesRect.loy);

				if (!ok)
					return FALSE;
			}
		}
	
		// Use bottom left of page as origin
		DocRect PageRect = pPage->GetPageRect();
		Origin = PageRect.lo;

		// set the translation variables to position the bottom left hand corner of the page
		// at the bottom left hand corner of the page we're importing onto. The origin of
		// a CDR file is in the centre of the page
		TranslateByX = Origin.x + (PageX / 2);
		TranslateByY = Origin.y + (PageY / 2);
	}

TRACEUSER( "Ben", _T("PageX = %d, PageY = %d\n"), PageX, PageY);

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ProcessPage(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	Processes a page list of a CDR file
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::ProcessPage(void)
{
TRACEUSER( "Ben", _T("In ProcessPage\n"));
	// first of all, create a new page item in the page list
	ERROR3IF(ThisPage != 0, "ThisPage is not null - ProcessPage called in wrong context");
	ThisPage = new CDRPageListItem;
	
	if(ThisPage == 0)
		return FALSE;
	
	// link it into the page list
	Pages.AddTail(ThisPage);

	// set the master page flag
	if(DoneMasterPage == FALSE)
		InMasterPage = TRUE;
	
	// go through the page list processing the layers
		// store the level we're working on
	UINT32 StartLevel = RIFF->GetObjLevel();

	do
	{
		if(RIFF->GetObjType() == RIFFOBJECTTYPE_LISTSTART &&
					RIFF->GetObjChunkType() == cdrT_layr)
		{
			// it's a layer, process it
			if(!ProcessLayer())
			{
TRACEUSER( "Ben", _T("ProcessLayer returned FALSE\n"));
				return FALSE;
			}
		}

		// if we're doing a version 3 file, have we got a page
		// size chunk to play with?
		if(Version == CDRVERSION_3 && RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK &&
					RIFF->GetObjChunkType() == cdrT_mcfg)
		{
			if(!RIFF->AquireChunkData())
				return FALSE;
			
			cdrfPageInfoV3 *i = (cdrfPageInfoV3 *)RIFF->GetAquiredData();

			// in a CDR 3 file, the page size is half what it should be	
			PageX = CDRDATA_WORD(i->PageX) * CDRCOORDS_TO_MILLIPOINTS * 2;
			PageY = CDRDATA_WORD(i->PageY) * CDRCOORDS_TO_MILLIPOINTS * 2;
			Landscape = FALSE;
		}

		UpdateProgress();
		
		if(!RIFF->NextObject())
			return FALSE;
		
		ERROR3IF(RIFF->GetObjType() == RIFFOBJECTTYPE_FILEEND, "Unexpected end of RIFF file in ProcessDoc");
	} while(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > StartLevel);


	// finally make ThisPage as not used
	ThisPage = 0;

	// sort out master page flags
	if(InMasterPage)
	{
		InMasterPage = FALSE;
		DoneMasterPage = TRUE;
	}

TRACEUSER( "Ben", _T("End ProcessPage\n"));

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ProcessLayer(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	Processes a layer list of a CDR file. This is the function which actually
				grabs objects to convert.
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ProcessLayer(void)
{
TRACEUSER( "Ben", _T("In ProcessLayer\n"));	
	// first of all, create a new layer item in the page list
	ERROR3IF(ThisLayer != 0, "ThisLayer is not null - ProcessLayer called in wrong context");
	ThisLayer = new CDRLayerListItem;
	
	if(ThisLayer == 0)
		return FALSE;
	
	// link it into the layer list on the page we're working on
	ThisPage->Layers.AddTail(ThisLayer);

	// store the level we're working on
	UINT32 StartLevel = RIFF->GetObjLevel();

	ThisLayer->Objects = 0;
	ThisLayer->IsVisible = TRUE;
	ThisLayer->IsLocked = FALSE;

	// set up the pointer to the node list to add to
	ERROR3IF(pLevelNodeList != 0, "Level node list is not empty");
	pLevelNodeList = 0;

	// if we're on the master page, find the layer name
	if(InMasterPage)
	{
		BOOL Done = FALSE;
		FOURCC LookingFor = (Version == CDRVERSION_3)?cdrT_lobj:cdrT_loda;

		do
		{
			if(RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK && RIFF->GetObjChunkType() == LookingFor)
			{
				// found the interesting chunk of data
				if(!RIFF->AquireChunkData())
					return FALSE;

				cdrfMasterLayerInfo *i = (cdrfMasterLayerInfo *)RIFF->GetAquiredData();

				if(i == 0)		// data couldn't be got
					return FALSE;

				if(i->Size != RIFF->GetObjSize()) {
TRACEUSER( "Ben", _T("level data in master page has wrong size\n"));
					ERROR1(FALSE, _R(IDE_CDRFILTER_FORMATERROR));
				}

				// different format of layer info for version 3
				if(Version == CDRVERSION_3)
				{
					cdrfMasterLayerInfoV3 *ii = (cdrfMasterLayerInfoV3 *)RIFF->GetAquiredData();
TRACEUSER( "Ben", _T("name of layer = %s\n"), ii->Name);
					ThisLayer->Name = (char *)ii->Name;
				} else
				{
TRACEUSER( "Ben", _T("name of layer = %s\n"), i->Name);
					ThisLayer->Name = (char *)i->Name;
				}

				Done = TRUE;
			}
			else if(RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK && RIFF->GetObjChunkType() == cdrT_flgs)
			{
				// look at the flags of the layer...
				if(!RIFF->AquireChunkData())
					return FALSE;

				WORD *pFlags = (WORD *)RIFF->GetAquiredData();

				ERROR3IF(pFlags == 0, "RIFF class shouldn't have done that");

				INT32 Flags = CDRDATA_WORD(*pFlags);

				if((Flags & cdrfLAYERFLAGS_INVISIBLE) != 0)
					ThisLayer->IsVisible = FALSE;

				if((Flags & cdrfLAYERFLAGS_LOCKED) != 0)
					ThisLayer->IsLocked = TRUE;
			}

			if(!RIFF->NextObject())
				return FALSE;
		
		} while(Done == FALSE && (RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > StartLevel));
	
		if(Done == FALSE)
		{
TRACEUSER( "Ben", _T("Reached level end in master page layer without finding layer attributes\n"));		
			ERROR1(FALSE, _R(IDE_CDRFILTER_FORMATERROR));
		}
	} else {
		ThisLayer->Name = "";
	}
	
	// go through the layer list processing the objects
	if(!ProcessObjectTree(StartLevel))
		return FALSE;

	// put the nodes we got onto the layer we're processing
	ThisLayer->Objects = pLevelNodeList;
	pLevelNodeList = 0;

	// finally mark ThisLayer as not used
	ThisLayer = 0;

TRACEUSER( "Ben", _T("End ProcessLayer\n"));
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ProcessObjectTree(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		the RIFF level the list we're processing starts at
	Returns:	error flag
	Purpose:	process objects from the CDR file - they end up linked to pLevelNodeList.
	SeeAlso:	Filter, CDRFilter::ProcessLayer, CDRFilter::ProcessVectorList

********************************************************************************************/

BOOL CDRFilter::ProcessObjectTree(UINT32 Level)
{
	do
	{
		switch(RIFF->GetObjType())
		{
			case RIFFOBJECTTYPE_LISTSTART:
				// see if it's anything we're interested in
				switch(RIFF->GetObjChunkType())
				{
					case cdrT_obj:		// found an object
						if(!ProcessObject())
							return FALSE;
						break;

					
					case cdrT_grp:
						// found a group type objects
						if(!ProcessGroup(RIFF->GetObjLevel()))
							return FALSE;
						break;

					case cdrT_lnkg:
						// found a link object - treated as the same as a group, but searched for a
						// transformation matrix
						if(!ProcessLink())
							return FALSE;
						break;

					default:
						break;
				}
				break;

			case RIFFOBJECTTYPE_LISTEND:
				// check to see if any actions are associated with this list end
				if(!Actions.IsEmpty())
				{
					CDRActionListItem *Act, *Previous;

					Act = (CDRActionListItem *)Actions.GetTail();

					// perform the actions which end on this level
					while(Act != 0)
					{
						if(Act->GetLevel() >= RIFF->GetObjLevel())
						{
							// the last action ended on this level - perform the action
							if(!Act->DoAction(this))
								return FALSE;

							// get the previous item
							Previous = (CDRActionListItem *)Actions.GetPrev(Act);

							// remove and delete the action we just performed
							delete Actions.RemoveTail();

							// and set up the pointer to the next action to look at
							Act = Previous;
						} else {
							// the action shouldn't be performed yet - stop looking
							break;
						}
					}
				}
				break;

			default:
				break;
		}
		
		if(!UpdateProgress())
			return FALSE;
		
		if(!RIFF->NextObject())
			return FALSE;
		
		ERROR3IF(RIFF->GetObjType() == RIFFOBJECTTYPE_FILEEND, "Unexpected end of RIFF file in ProcessLayer");

	} while(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > Level);

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ProcessGroup(UINT32 Level, BOOL FromLink = FALSE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	Sets up a group end action and alters the level node list to be a new list
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ProcessGroup(UINT32 Level, BOOL FromLink)
{
	// if it's a CDR3 file we need a transform action
	if(Version == CDRVERSION_3)
	{
		// see if there's a transform object around
		BOOL Found = FALSE;
		do {
			if(!RIFF->NextObject())
				return FALSE;

			FOURCC id = RIFF->GetObjChunkType();

			// the group need not have a transform chunk, so we've got to be a little careful
			if(id == cdrT_grp || id == cdrT_lnkg || id == cdrT_obj)
			{
				// oh dear, it didn't - make sure this object we've just run onto is seen again
				RIFF->RepeatCurrent();
		
				break;
			}

			if(id == cdrT_lobj && RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK)
			{
				// found a transform chunk
				Found = TRUE;
			}
		} while(Found == FALSE);

		// if there is, set up a post transform action
		if(Found)
		{
TRACEUSER( "Ben", _T("Setting transform for CDR3 group at level %d\n"), Level);
			CDRActionTransform *TAct = new CDRActionTransform;

			if(TAct == 0)
				return FALSE;

			// initialise my new action, giving it the RIFFFile we're using
			// it takes the current chunk as it's data.
			if(!TAct->Init(RIFF, Version))
				return FALSE;

			// set it to act on the same level ending as the group
			TAct->SetLevel(Level);
		
			// add my new action to the action list
			Actions.AddTail(TAct);
		}
	}
	else if(!FromLink)
	{
		// see if there's a transform chunk, and if there is, whether it's got a non matrix transform in it or not
		BOOL Found = FALSE;
		do {
			if(!RIFF->NextObject())
				return FALSE;

			FOURCC id = RIFF->GetObjChunkType();

			// the group need not have a transform chunk, so we've got to be a little careful
			if(id == cdrT_grp || id == cdrT_lnkg || id == cdrT_obj)
			{
				// oh dear, it didn't - make sure this object we've just run onto is seen again
				RIFF->RepeatCurrent();
		
				break;
			}

			if(id == cdrT_trfl && RIFF->GetObjType() == RIFFOBJECTTYPE_LISTSTART)
			{
				// found a transform list
				Found = TRUE;
			}
		} while(Found == FALSE);

		// if there is, set up a post transform action
		if(Found)
		{
			CDRActionTransform *Act = new CDRActionTransform;

			if(Act == 0)
				return FALSE;

			// initialise my new action, giving it the RIFFFile we're using
			// it takes the current chunk as it's data.
			if(!Act->Init(RIFF, Version))
				return FALSE;

			// check to see if the transform contains a non-matrix thingy
			if(IsTransformJustMatrices(Act->Chunk, Act->ChunkSize))
			{
				// just matrices - don't do this action
				delete Act;
			}
			else
			{
				// set it to act on the same level ending as the group
				Act->SetLevel(Level);
		
				// set it to ignore matrices
				Act->SetIsAGroupTransform(TRUE);

				// add my new action to the action list
				Actions.AddTail(Act);
			}
		}
	}
	
	CDRActionGroupEnd *Act = new CDRActionGroupEnd;

	if(Act == 0)
		return FALSE;

	// store this levels node list
	Act->pOldLevelNodeList = pLevelNodeList;

	// set up the level it's interested in
	Act->SetLevel(Level);

	// and start a new level node list for this group
	pLevelNodeList = 0;

	Actions.AddTail(Act);

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRActionGroupEnd::DoAction(CDRFilter *C)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	does some funcky stuff to put objects in a group
	SeeAlso:	

********************************************************************************************/

BOOL CDRActionGroupEnd::DoAction(CDRFilter *C)
{
	// make sure we have some objects to put in a group
	if(C->pLevelNodeList == 0)
	{
		// restore the pointer
		C->pLevelNodeList = pOldLevelNodeList;
		// and exit
		return TRUE;
	}
	
	// make a new group node
	NodeGroup *pGroup = new NodeGroup;

	if(pGroup == 0)
		return FALSE;

	// OK, pop everything on the level node list onto this nice new group
	C->pLevelNodeList->InsertChainSimple(pGroup, LASTCHILD);

	// restore the old pointer
	C->pLevelNodeList = pOldLevelNodeList;

	// and attach!
	if(C->pLevelNodeList == 0)
	{
		C->pLevelNodeList = pGroup;
	} else {
		pGroup->AttachNode(C->pLevelNodeList, PREV);
		C->pLevelNodeList = pGroup;	
	}

	// and that's it
	return TRUE;
}


/********************************************************************************************

>	void CDRActionGroupEnd::ImportFailure(CDRFilter *C)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	ensures no memory leaks if the import fails
	SeeAlso:	

********************************************************************************************/

void CDRActionGroupEnd::ImportFailure(CDRFilter *C)
{
	if(pOldLevelNodeList != 0)
	{
		// delete all the entries stored in this action
		C->DeleteNodeList(pOldLevelNodeList);
	}
}

/********************************************************************************************

>	BOOL CDRFilter::ProcessLink(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	Sets up a group end action and a post transform action and alters the level
				node list to be a new list
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ProcessLink(void)
{
	// store the level we're on
	UINT32 GroupLevel = RIFF->GetObjLevel();

	// only search for a transform block if this isn't a CDR3 file - ProcessGroup
	// takes care of it in that case
	if(Version != CDRVERSION_3)
	{
		// we want to set up the transform object first so that it's done *after* the group end
		// object as the transform object acts on the last object converted
	
		// see if there's a transform object around
		BOOL Found = FALSE;
		do {
			if(!RIFF->NextObject())
				return FALSE;

			FOURCC id = RIFF->GetObjChunkType();

			// the group need not have a transform chunk, so we've got to be a little careful
			if(id == cdrT_grp || id == cdrT_lnkg || id == cdrT_obj)
			{
				// oh dear, it didn't - make sure this object we've just run onto is seen again
				RIFF->RepeatCurrent();
		
				break;
			}

			if(id == cdrT_trfl && RIFF->GetObjType() == RIFFOBJECTTYPE_LISTSTART)
			{
				// found a transform list
				Found = TRUE;
			}
		} while(Found == FALSE);

		// if there is, set up a post transform action
		if(Found)
		{
			CDRActionTransform *Act = new CDRActionTransform;

			if(Act == 0)
				return FALSE;

			// initialise my new action, giving it the RIFFFile we're using
			// it takes the current chunk as it's data.
			if(!Act->Init(RIFF, Version))
				return FALSE;

			// set it to act on the same level ending as the group
			Act->SetLevel(GroupLevel);
			
			// set up link translation things
			Act->LinkTrans = TRUE;
			LinkTransformsExist = TRUE;
		
			// add my new action to the action list
			Actions.AddTail(Act);
		}
	}
	
	// set up a group end action
	if(!ProcessGroup(GroupLevel, TRUE))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRActionTransform::Init(RIFFFile *RIFF, CDRVersion Version)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	sets up a transform action - stored given chunk
	SeeAlso:	

********************************************************************************************/

BOOL CDRActionTransform::Init(RIFFFile *RIFF, CDRVersion Version)
{
	if(Version == CDRVERSION_3)
	{
		// it's a version 3 file, so get the chunk
		ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK, "CDRActionTransform::Init called with RIFFFile in wrong state");

		ERROR3IF(Chunk != 0, "CDRActionTransform::Init called more than once");

		// claim some memory to put the transform in
		if((Chunk = (ADDR)CCMalloc(RIFF->GetObjSize())) == 0)
			return FALSE;

		// and get the chunk data into this nice block
		if(!RIFF->GetChunkData(Chunk, RIFF->GetObjSize()))
			return FALSE;
	}
	else
	{
		// it's a version 5 file, so get the list contents
		ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTSTART, "CDRActionTransform::Init called with RIFFFile in wrong state");

		ERROR3IF(Chunk != 0, "CDRActionTransform::Init called more than once");
	
		// get the list contents
		if(!RIFF->GetListContents(&Chunk, &ChunkSize))
			return FALSE;
	}

	// that's it for now
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRActionTransform::DoAction(CDRFilter *C)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	transforms the last object converted
	SeeAlso:	

********************************************************************************************/

BOOL CDRActionTransform::DoAction(CDRFilter *C)
{
	ERROR3IF(Chunk == 0, "CDRActionTransform not initialised");
	
	if(LinkTrans)
	{
		return TRUE;
	}
	
	ADDR Transform = Chunk;

	// if this is a version 3 file we need to find the transform in the data - it's in
	// a standard offset header
	if(C->Version == CDRVERSION_3)
	{
		cdrfOffsetHeader *Header = (cdrfOffsetHeader *)Chunk;

		// if there isn't a transform, then we're not worried
		if((Transform = C->FindDataInObject(Header, cdrfOBJOFFSETTYPE_TRANSFORM_V3)) == 0)
			return TRUE;
	}

	// transform the last node created

	// unlink it from the tree first...
	Node *pNext = C->pLevelNodeList->FindNext();
 	C->pLevelNodeList->UnlinkNodeFromTree();

	// we don't want to scale it to translate the origin because it's already been done
	if(!LinkTrans && C->pLevelNodeList != 0)
		C->TransformConvertedObject(&C->pLevelNodeList, Transform, ChunkSize, FALSE, IsAGroupTransform, TRUE);

	// relink it in
	if(pNext != 0)
		C->pLevelNodeList->AttachNode(pNext, PREV);
	
	return TRUE;
}


/********************************************************************************************

>	void CDRActionTransform::ImportFailure(CDRFilter *C)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	don't actually need to do anything - the chunk will be deleted on object
				destruction
	SeeAlso:	

********************************************************************************************/

void CDRActionTransform::ImportFailure(CDRFilter *C)
{
}


/********************************************************************************************

>	CDRActionTransform::~CDRActionTransform

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	destructor
	SeeAlso:	

********************************************************************************************/

CDRActionTransform::~CDRActionTransform()
{
	CCFree(Chunk);
}


/********************************************************************************************

>	BOOL CDRFiler::ConvertColour(cdrfColour *Col, DocColour *Out)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28 03 95
	Inputs:		cdr colour and a DocColour to put it into
	Returns:	error flag
	Purpose:	converts a colour from the CDR definition taking into account the version
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::ConvertColour(cdrfColour *Col, DocColour *Out)
{
	ERROR3IF(Col == 0 || Out == 0, "Null pointers passed to ConvertColour");
	
	ImportedNewColour ICol;
	
	switch(Version)
	{
		case CDRVERSION_5:	CDRColour::Convert5(Col, &ICol);	break;
		case CDRVERSION_4:	CDRColour::Convert4(Col, &ICol);	break;
		default:			CDRColour::Convert3(Col, &ICol);	break;
	}
	
	IndexedColour *NewCol = new IndexedColour(ICol.Model, &ICol.Colour);

	if(NewCol == 0)
		return FALSE;

	ColList->GetUnnamedColours()->AddTail(NewCol);

	Out->MakeRefToIndexedColour(NewCol);

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ProcessObject(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	Converts an object. If there's a problem with the object, then it's skipped
				rather than reporting an error, and the count of problem objects incremented
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ProcessObject(void)
{
	UINT32 StartLevel = RIFF->GetObjLevel();

	// set the made node to none
	pMadeNode = 0;

	// set the already applied flag
	AttrsAlreadyApplied = FALSE;
	
	// set the is text things to make sure bodges don't happen
	IsText = FALSE;
	IsTextStory = FALSE;

	// find the loda (4 or 5) or lobj (3) chunk... contains the object header
	BOOL Found = FALSE;
	FOURCC LookingFor = (Version == CDRVERSION_3)?cdrT_lobj:cdrT_loda;
	do
	{
		// is the serial number?
		if(RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK && RIFF->GetObjChunkType() == cdrT_spnd) {
			// got the serial number block - get it...
			if(!RIFF->AquireChunkData())
				return FALSE;
		
			WORD *Data = (WORD *)RIFF->GetAquiredData();
		
			if(Data == 0)
				return FALSE;

			// store it...
			SerialNumber = CDRDATA_WORD(*Data);
		}

		if(RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK && RIFF->GetObjChunkType() == LookingFor) {
			Found = TRUE;
			break;
		}

		if(!UpdateProgress())
			return FALSE;
		
		if(!RIFF->NextObject())
			return FALSE;
		
	} while(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > StartLevel);

	// if we didn't find anything, then the object has a wrong format
	if(Found == FALSE)
	{
TRACEUSER( "Ben", _T("Could not find loda (or lobj) chunk in object\n"));		
		ObjectsFormatWrong++;
		return TRUE;
	}

	// get the header - but get it into our block, not the one in RIFFFile as we need
	// to refer to it later. It shouldn't be too big to make this inefficient...
	cdrfOffsetHeader *Header;
	
	// ensure we have a block big enough
	if(ObjDataBlock == 0 || ObjDataBlockSize < RIFF->GetObjSize())
	{
		// free the old one
		CCFree(ObjDataBlock);

		// size?
		INT32 Size = RIFF->GetObjSize();

		if((ObjDataBlock = (ADDR)CCMalloc(Size)) == 0)
			return FALSE;

		ObjDataBlockSize = Size;
	}
	
	if(!RIFF->GetChunkData(ObjDataBlock, ObjDataBlockSize))
		return FALSE;

	Header = (cdrfOffsetHeader *)ObjDataBlock;

	// look at the type and dispatch to appropriate processing function
	FormatError = FALSE;
	
	INT32 ObjectType = CDRDATA_WORD(Header->ObjectType);

	// if this is a version 3 object, alter it's type
	if(Version == CDRVERSION_3)
		ObjectType += cdrfOBJTYPE_V3ADD;

	switch(ObjectType) {
		case cdrfOBJTYPE_RECTANGLE:
			if(!ConvertRectangle(Header))
				return FALSE;
			break;
		
		case cdrfOBJTYPE_ELLIPSE:
			if(!ConvertEllipse(Header))
				return FALSE;
			break;
		
		case cdrfOBJTYPE_PATH:
			if(!ConvertPath(Header))
				return FALSE;
			break;

		case cdrfOBJTYPE_TEXT:
		case cdrfOBJTYPE_TEXTSTORY:
			if(!ConvertText(Header))
				return FALSE;
			break;

		case cdrfOBJTYPE_BITMAP:
			if(!ConvertBitmap(Header))
				return FALSE;
			break;

		default:
			// unknown object type
			ObjectsUnknown++;
			RIFF->SkipToListEnd(StartLevel);
			return TRUE;
			break;
	}
	
	// check to see if the format was OK
	if(FormatError)
	{
TRACEUSER( "Ben", _T("A convert object function returned a format error\n"));
		ObjectsFormatWrong++;

		if(TransformChunk != 0)
		{
			CCFree(TransformChunk);
			TransformChunk = 0;
		}

		return TRUE;
	}

	ERROR3IF(pMadeNode == 0, "No node made, but still nothing errored\n");
	
	// think about lenses
	BOOL UnsetTransparency = FALSE;
	if(Version == CDRVERSION_5)
	{
		cdrfLensDefn *Lens = (cdrfLensDefn *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_LENS);

		if(Lens != 0)
		{
			// we have a lens to worry about... what type is it?
			if(Lens->Type == cdrfLENSTYPE_TRANSPARENT)
			{
				// check the percentage
				UINT32 Percent = CDRDATA_WORD(Lens->Percent) / 10;
				if(Percent <= 100)
				{
					// munge the percentage a bit to get it to a Camelot definition
					Percent = 256 - ((Percent * 256) / 100);

					// what subtype is it?
					switch(Lens->SubType)
					{
						case cdrfTRANSPARENTTYPE_MIX:
							SetFlatTranspFill(T_REFLECTIVE, Percent);
							UnsetTransparency = TRUE;
							break;

						case cdrfTRANSPARENTTYPE_STAINED:
							SetFlatTranspFill(T_SUBTRACTIVE, Percent);
							UnsetTransparency = TRUE;
							break;

						case cdrfTRANSPARENTTYPE_BLEACH:
							SetFlatTranspFill(T_ADDITIVE, Percent);
							UnsetTransparency = TRUE;
							break;

						default:
							break;
					}
				}
			}
		}
	}
	
	// find the transform chunk - it's in a seperate chunk for version 4 or 5 file,
	// and in the data chunk for version 3 files
	ADDR TransformData = 0;
	INT32 TransformDataSize = 0;
	if(TransformChunk != 0)
	{
		// if something had to move forward in the file (for example, text objects)
		// past the transform chunk, it will have stored it here.
		TransformData = TransformChunk;
		TransformDataSize = TransformChunkSize;
	} 
	else if(Version == CDRVERSION_3)
	{
		TransformData = FindDataInObject(Header, cdrfOBJOFFSETTYPE_TRANSFORM_V3);
	}
	else 
	{
		Found = FALSE;
		do
		{
			if(RIFF->GetObjType() == RIFFOBJECTTYPE_LISTSTART && RIFF->GetObjChunkType() == cdrT_trfl) {
				Found = TRUE;
				break;
			}

			UpdateProgress();
		
			if(!RIFF->NextObject())
				return FALSE;
		
		} while(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > StartLevel);

		// if we didn't find anything, then the object has a wrong format
		if(Found == FALSE)
		{
	TRACEUSER( "Ben", _T("Could not find trfd chunk in object\n"));		
			ObjectsFormatWrong++;
			// delete the current node
			delete pMadeNode;
			return TRUE;
		}

		// aquire the chunk data
		if(!RIFF->AquireListContents(&TransformDataSize))
		{
			delete pMadeNode;
			return FALSE;
		}
		TransformData = RIFF->GetAquiredData();
	}

	if(TransformData == 0)
	{
		ObjectsFormatWrong++;
		// delete the current node
		delete pMadeNode;
		return TRUE;
	}

	// go through any link transforms which may exist
	if(Version != CDRVERSION_3)
	{
		if(LinkTransformsExist)
		{
			BOOL FoundSome = FALSE;
		
			CDRActionListItem *Act = (CDRActionListItem *)Actions.GetTail();

			while(Act != 0)
			{
				if(IS_A(Act, CDRActionTransform))
				{
					CDRActionTransform *Tran = (CDRActionTransform *)Act;
				
					if(Tran->LinkTrans)
					{
						// set the flag to say we found a link transform
						FoundSome = TRUE;

						// transform the converte object
						if(!TransformConvertedObject(&pMadeNode, Tran->Chunk, Tran->ChunkSize, FALSE))
						{
							delete pMadeNode;
							return FALSE;
						}
					}
				}

				Act = (CDRActionListItem *)Actions.GetPrev(Act);
			}

			LinkTransformsExist = FoundSome;
		}
	}

	// apply the transform to the converted node
	if(!TransformConvertedObject(&pMadeNode, TransformData, TransformDataSize))
	{
		delete pMadeNode;
		return FALSE;
	}

	// apply attributes
	if(AttrsAlreadyApplied == FALSE)
	{
		// look at fill attributes
		if(!SetFillAttr(Header))
			return FALSE;

		// look at line attributes
		if(!SetLineAttr(Header))
			return FALSE;
	}

	BOOL Result;
	if(AttrsAlreadyApplied == FALSE)
	{
		// apply the attributes to the object
		// If not filled, then set the ignore bit on the fill attribute.
		if (ObjFilled == FALSE)
			CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;

		// Add attributes to the path, if they are different from the default...
		Result = AttributeManager::ApplyBasedOnDefaults(pMadeNode, CurrentAttrs);

		// Enable the fill attribute again
		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;
	} else {
		Result = TRUE;
	}

	if(!Result)
		return FALSE;

	if(UnsetTransparency)
	{
		SetNoTranspFill();
	}

	// attach the node to the level list
	if(pLevelNodeList != 0)
		pMadeNode->AttachNode(pLevelNodeList, PREV);

	pLevelNodeList = pMadeNode;	
	
	// set the made node pointer to zero to prevent accidents
	pMadeNode = 0;

	// increment the count of converted objects
	ObjectsConverted++;

	// if necessary, get rid of any transform chunk
	if(TransformChunk != 0)
	{
		CCFree(TransformChunk);
		TransformChunk = 0;
	}
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ConvertPath(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	actually converts a path object to a node. If the object has an unexpected
				format, then FormatError is set and it return *TRUE* - the return value only
				indicated environmental errors such as lack of memory
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ConvertPath(cdrfOffsetHeader *Header)
{
	// find the address of the coordinates in the header
	cdrfPathCoordHeader *CHdr = (cdrfPathCoordHeader *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_COORDS);

	// it has got some coords, hasn't it?
	if(CHdr == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}

	// find the address of the types and the coordinates
	cdrfCoord *Coords = (cdrfCoord *)(CHdr + 1);

	// and the address of the types
	UINT32 NCoords = CDRDATA_WORD(CHdr->NCoords);
	BYTE *Types = (BYTE *)(Coords + NCoords);

	// check the path a little
	if((NCoords * (sizeof(cdrfCoord) + sizeof(BYTE))) > CDRDATA_WORD(Header->Size) || NCoords < 2)
	{
		FormatError = TRUE;
		return TRUE;
	}
	
	// knock up a path node
	NodePath *pPath = new NodePath;
	if(pPath == 0 || (!pPath->SetUpPath()))
	{
		delete pPath;
		return FALSE;			// no room to create it
	}

	// position the new elements at the beginning of the path
	pPath->InkPath.FindStartOfPath();

	// flag the initial status of the fill/stokedness of the path
	ObjFilled = FALSE;
	ObjStroked = TRUE;

	if(!AddElementsToPath(&pPath->InkPath, NCoords, Coords, Types, &ObjFilled))
		goto NoMemory;

	// check that the path is OK
	if(!pPath->InkPath.EnsureValid())
	{
		// no, it's completely knackered
		delete pPath;
		FormatError = TRUE;
		return TRUE;
	}

	// finish off the path
	pPath->InvalidateBoundingRect();				
	pPath->InkPath.IsFilled = ObjFilled;

	// save node pointer
	pMadeNode = pPath;

	// check to see if text goes along this...
	{
		CDRTextOnPathLink *Item = (CDRTextOnPathLink *)TextOnPathLinks.GetHead();

		while(Item != 0)
		{
			// does this link refer to this text story?
			if(SerialNumber == Item->PathSerialNumber)
			{
				// yep - store a pointer to this path
				Item->pPath = pPath;
			}
		
			// next!
			Item = (CDRTextOnPathLink *)TextOnPathLinks.GetNext(Item);
		}
	}
	
	// everything was nice
	return TRUE;

NoMemory:
	// get rid of any path which is in the process of being created
	if(pPath != 0)
		pPath->CascadeDelete();

	delete pPath;

	return FALSE;
}


/********************************************************************************************

>	BOOL CDRFilter::AddElementsToPath(Path *pPath, INT32 NCoords, cdrfCoord *Coords, BYTE *Types, BOOL *IsClosed);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/05/95
	Inputs:		path to add too, number of coords, pointer to coord data, pointer to types
	Returns:	error flag
	Purpose:	Adds elements to a path from a CDR path data
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::AddElementsToPath(Path *pPath, INT32 NCoords, cdrfCoord *Coords, BYTE *Types, BOOL *IsClosed)
{
	// go through the coordinates in the CDR path adding them to the Camelot path
	INT32 c;					// coord counter
	UINT32 CoordType;			// type of this coordinate
   	UINT32 Control1 = 0;		// number of first control point
	UINT32 Control2 = 0;		// of second
	DocCoord co, cn1, cn2;	// coordinates
	PathFlags Flags;
	BOOL NeedMoveTo = TRUE;

	if(IsClosed != 0)
		(*IsClosed) = FALSE;
	
	// go though converting all the coords
	for(c = 0; c < NCoords; c++)
	{
		CoordType = Types[c] & cdrfPATHCOORDTYPE_MASK;

		co.x = CDRDATA_SWORD(Coords[c].X) * CDRCOORDS_TO_MILLIPOINTS;
		co.y = CDRDATA_SWORD(Coords[c].Y) * CDRCOORDS_TO_MILLIPOINTS;

		// add in a move to if necessary
		if(NeedMoveTo && CoordType != cdrfPATHCOORDTYPE_MOVE)
		{
			if(!pPath->InsertMoveTo(co))
				return FALSE;
		}

		NeedMoveTo = FALSE;
		
		// process the coordinate		
		switch(CoordType)
		{
			case cdrfPATHCOORDTYPE_MOVE:
				// add a move to this path
				if(!pPath->InsertMoveTo(co))
					return FALSE;
				break;
			
			case cdrfPATHCOORDTYPE_LINETO:
				// add a line to this coord to the path
				if(!pPath->InsertLineTo(co))
					return FALSE;
				break;
			
			case cdrfPATHCOORDTYPE_CURVE:
				// check we have some control points for this curve
				// a control point cannot be the first coord, so it's OK to check against 0
				if(Control1 == 0 || Control2 == 0)
				{
TRACEUSER( "Ben", _T("No control points for curve element\n"));
					break;
				}

				// convert the control points
				cn1.x = CDRDATA_SWORD(Coords[Control1].X) * CDRCOORDS_TO_MILLIPOINTS;
				cn1.y = CDRDATA_SWORD(Coords[Control1].Y) * CDRCOORDS_TO_MILLIPOINTS;
				cn2.x = CDRDATA_SWORD(Coords[Control2].X) * CDRCOORDS_TO_MILLIPOINTS;
				cn2.y = CDRDATA_SWORD(Coords[Control2].Y) * CDRCOORDS_TO_MILLIPOINTS;
				
				// create the curve
				Flags.IsSelected = FALSE;
				Flags.IsSmooth = Flags.IsRotate = ((Types[c] & cdrfPATHCOORDATTR_SMOOTH) != 0)?TRUE:FALSE;
				Flags.IsEndPoint;
				
				// insert it into the path
				if(!pPath->InsertCurveTo(cn1, cn2, co, &Flags))
					return FALSE;
				break;

			case cdrfPATHCOORDTYPE_CONTROL:
				// shuffle the control points we've already got and add the new one
				Control1 = Control2;
				Control2 = c;
				break;
			
			default:
				// can't get here as mask won't allow any other value than the ones above.
				break;
		}

		// is this a close subpath situtation?
		if((Types[c] & cdrfPATHCOORDATTR_CLOSE) != 0)
		{
			if(IsClosed != 0)
				(*IsClosed) = TRUE;		// path has at least one closed element and should be filled
			
			// close the sub path
			if(CoordType != cdrfPATHCOORDTYPE_MOVE)
			{
				if(!pPath->CloseSubPath())
					return FALSE;

				// ensure that the next coord is a moveto
				NeedMoveTo = TRUE;
			}
		}
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ConvertRectangle(cdrfOffsetHeader *Header)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		pointer to header
	Returns:	error flag
	Purpose:	generates a rectangle object
	SeeAlso:	

********************************************************************************************/

#define RECTANGLE_CORNER_MAGIC (1.0 - 0.552)

#define RECT_MOVE(px, py) {co.x = (px); co.y = (py); if(!pPath->InkPath.InsertMoveTo(co)) goto NoMemory;}
#define RECT_LINE(px, py) {co.x = (px); co.y = (py); if(!pPath->InkPath.InsertLineTo(co)) goto NoMemory;}
#define RECT_CURVE(cx1, cy1, cx2, cy2, px, py) {cn1.x = (cx1); cn1.y = (cy1); cn2.x = (cx2); cn2.y = (cy2); \
			co.x = (px); co.y = (py); if(!pPath->InkPath.InsertCurveTo(cn1, cn2, co)) goto NoMemory;}
#define RECT_CLOSE {if(!pPath->InkPath.CloseSubPath()) goto NoMemory;}


BOOL CDRFilter::ConvertRectangle(cdrfOffsetHeader *Header)
{
	// find the address of the coordinates in the header
	cdrfRectangleData *Size = (cdrfRectangleData *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_COORDS);

	// it has got some coords, hasn't it?
	if(Size == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}

	// get the dimensions from the object
	INT32 Width = CDRDATA_SWORD(Size->Width) * CDRCOORDS_TO_MILLIPOINTS;
	INT32 Height = CDRDATA_SWORD(Size->Height) * CDRCOORDS_TO_MILLIPOINTS;
	INT32 CornerRadius = CDRDATA_SWORD(Size->CornerRadius) * CDRCOORDS_TO_MILLIPOINTS;
	
	if(CornerRadius < 0)
		CornerRadius = 0 - CornerRadius;

	// make a rectangle quickshape...
	NodeRegularShape *NewShape = new (NodeRegularShape);

	if ((NewShape == NULL) || !NewShape->SetUpShape() )
	{
		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
	}

	INT32 AWidth = Width;
	INT32 AHeight = Height;
	if(AWidth < 0) AWidth = 0 - AWidth;
	if(AHeight < 0) AHeight = 0 - AHeight;
	
	if(!NewShape->MakeRectangle(AWidth, AHeight, CornerRadius))
		return FALSE;

	// translate it a bit so the corner is on the origin, not the thingy
	Trans2DMatrix Trans(Width / 2, Height / 2);

	NewShape->Transform(Trans);

	// finish off the shape
	NewShape->InvalidateBoundingRect();

	// set up an attribute or two
	ObjFilled = TRUE;
	ObjStroked = TRUE;				

	// set the made node
	pMadeNode = NewShape;

	return TRUE;

}


/********************************************************************************************

>	BOOL CDRFilter::ConvertEllipse(cdrfOffsetHeader *Header)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		pointer to header
	Returns:	error flag
	Purpose:	generates an ellipse object
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ConvertEllipse(cdrfOffsetHeader *Header)
{
	// find the address of the coordinates in the header
	cdrfEllipseData *Size = (cdrfEllipseData *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_COORDS);

	// it has got some coords, hasn't it?
	if(Size == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}

	// Create the ellipse
	NodeRegularShape *NewShape = new (NodeRegularShape);

	if ((NewShape == NULL) || !NewShape->SetUpShape() )
	{
		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
	}

	// fill it all in for an ellipse
	NewShape->SetNumSides(4);

	INT32 Width = CDRDATA_SWORD(Size->Width) * CDRCOORDS_TO_MILLIPOINTS;
	INT32 Height = CDRDATA_SWORD(Size->Height) * CDRCOORDS_TO_MILLIPOINTS;

	DocCoord co;
	co.x = Width / 2;		// centre - it all starts at (0,0)
	co.y = Height / 2;
	NewShape->SetCentrePoint(co);
	co.x = Width;
	NewShape->SetMajorAxes(co);
	co.x /= 2;
	co.y = Height;
	NewShape->SetMinorAxes(co);

	NewShape->SetCircular(TRUE);
	NewShape->SetStellated(FALSE);
	NewShape->SetPrimaryCurvature(TRUE);
	NewShape->SetStellationCurvature(FALSE);

	// finish off the path
	NewShape->InvalidateBoundingRect();
	
	// set up an attribute or two
	ObjFilled = TRUE;
	ObjStroked = TRUE;				

	// set the made node
	pMadeNode = NewShape;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ConvertBitmap(cdrfOffsetHeader *Header)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/95
	Inputs:		pointer to header
	Returns:	error flag
	Purpose:	generates a bitmap object
	SeeAlso:	

********************************************************************************************/

#define BITM_MOVE(px, py) {co.x = (px); co.y = (py); if(!pPath->InkPath.InsertMoveTo(co)) return FALSE;}
#define BITM_LINE(px, py) {co.x = (px); co.y = (py); if(!pPath->InkPath.InsertLineTo(co)) return FALSE;}
#define BITM_CLOSE {if(!pPath->InkPath.CloseSubPath()) return FALSE;}

BOOL CDRFilter::ConvertBitmap(cdrfOffsetHeader *Header)
{
	// find the address of the coordinates in the header
	cdrfBitmapData *BInfo = (cdrfBitmapData *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_COORDS);

	// it has got some object data, hasn't it?
	if(BInfo == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}

	// OK, let's get the information in a nice structure to start off with
	struct {
		DWORD Reference;
		INT32 SizeX;
		INT32 SizeY;
		INT32 Left;			// cropping coordinates
		INT32 Right;
		INT32 Bottom;
		INT32 Top;
	} Info;

	// choose the right reference - it's in a slightly different place in CDR 3 files
	if(Version != CDRVERSION_3)
	{
		Info.Reference = CDRDATA_WORD(BInfo->BitmapReference);
		Info.SizeX = CDRDATA_SWORD(BInfo->SizeX) * CDRCOORDS_TO_MILLIPOINTS;
		Info.SizeY = CDRDATA_SWORD(BInfo->SizeY) * CDRCOORDS_TO_MILLIPOINTS;
		Info.Left = CDRDATA_SWORD(BInfo->Left) * CDRCOORDS_TO_MILLIPOINTS;
		Info.Right = CDRDATA_SWORD(BInfo->Right) * CDRCOORDS_TO_MILLIPOINTS;
		Info.Bottom = CDRDATA_SWORD(BInfo->Bottom) * CDRCOORDS_TO_MILLIPOINTS;
		Info.Top = CDRDATA_SWORD(BInfo->Top) * CDRCOORDS_TO_MILLIPOINTS;
	}
	else
	{
		cdrfBitmapData3 *B3Info = (cdrfBitmapData3 *)BInfo;
		
		Info.Reference = CDRDATA_WORD(B3Info->BitmapReference);
		Info.SizeX = CDRDATA_SWORD(B3Info->SizeX) * CDRCOORDS_TO_MILLIPOINTS;
		Info.SizeY = CDRDATA_SWORD(B3Info->SizeY) * CDRCOORDS_TO_MILLIPOINTS;
		Info.Left = CDRDATA_SWORD(B3Info->Left) * CDRCOORDS_TO_MILLIPOINTS;
		Info.Right = CDRDATA_SWORD(B3Info->Right) * CDRCOORDS_TO_MILLIPOINTS;
		Info.Bottom = CDRDATA_SWORD(B3Info->Bottom) * CDRCOORDS_TO_MILLIPOINTS;
		Info.Top = CDRDATA_SWORD(B3Info->Top) * CDRCOORDS_TO_MILLIPOINTS;
	}

	// find that there bitmap
	KernelBitmap *pBitmap = Bitmaps.FindBitmap(Info.Reference);

	if(pBitmap == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}

	// find a bounding box for the bitmap on the page
	DocRect BoundsRect;

	if(Info.SizeX > 0)
	{
		BoundsRect.lo.x = 0;
		BoundsRect.hi.x = Info.SizeX;
	} else {
		BoundsRect.lo.x = Info.SizeX;
		BoundsRect.hi.x = 0;
	}

	if(Info.SizeY > 0)
	{
		BoundsRect.lo.y = 0;
		BoundsRect.hi.y = Info.SizeY;
	} else {
		BoundsRect.lo.y = Info.SizeY;
		BoundsRect.hi.y = 0;
	}


	// OK, if the bitmap isn't cropped, create a bitmap object, otherwise
	// create a rectangle which is the right size and pop a bitmap fill on it
	if(Info.Left == 0 && Info.Right == Info.SizeX && Info.Top == 0 && Info.Bottom == Info.SizeY)
	{
		// create a bitmap object
		NodeBitmap *NewBitmap = new NodeBitmap;

		if((NewBitmap == NULL) || !NewBitmap->SetUpPath())
			return FALSE;
		
		NewBitmap->CreateShape(BoundsRect);

		// set the reference to the bitmap
		NewBitmap->GetBitmapRef()->Attach(pBitmap, GetDocument());
		if (NewBitmap->GetBitmap() != pBitmap)
		{
			// It didn't use the bitmap we gave it, so we can delete it
			delete pBitmap;
		}
	
		// apply a few nice attributes to it
		NewBitmap->ApplyDefaultBitmapAttrs(0);

		// invalidate the bounding rectangle
		NewBitmap->InvalidateBoundingRect();

		// cool.
		ObjFilled = FALSE;
		ObjStroked = FALSE;	

		// we don't want any of those fancy attributes things on this object.
		AttrsAlreadyApplied = TRUE;

		// set the made node
		pMadeNode = NewBitmap;
	}
	else
	{
		// create a rectangular path object...
		// generate a path
		NodePath *pPath = new NodePath;
		if(pPath == 0 || (!pPath->SetUpPath()))
		{
			delete pPath;
			return FALSE;			// no room to create it
		}

		// position the new elements at the beginning of the path
		pPath->InkPath.FindStartOfPath();

		DocCoord co, cn1, cn2;
	
		// make the rectangle shape
		BITM_MOVE(Info.Left,	Info.Bottom)
		BITM_LINE(Info.Right,	Info.Bottom)
		BITM_LINE(Info.Right,	Info.Top)
		BITM_LINE(Info.Left,	Info.Top)
		BITM_LINE(Info.Left,	Info.Bottom)
		BITM_CLOSE

		// finish off the path
		pPath->InvalidateBoundingRect();
		pPath->InkPath.IsFilled = TRUE;
	
		// set up an attribute or two
		ObjFilled = TRUE;
		ObjStroked = FALSE;

		// set up a nice attribute for this thingy
		if(!SetLineColour(DocColour(COLOUR_TRANS)))
			return FALSE;

		// set a nice bitmap fill
		DocCoord Start 	= DocCoord(BoundsRect.lo.x, BoundsRect.lo.y);
		DocCoord End1 	= DocCoord(BoundsRect.hi.x, BoundsRect.lo.y);
		DocCoord End2 	= DocCoord(BoundsRect.lo.x, BoundsRect.hi.y);
		if(!SetBitmapFill(pBitmap, Start, End1, End2))
			return FALSE;
		
		// apply attributes
		SetPathFilled(TRUE);
		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;
		if(!AttributeManager::ApplyBasedOnDefaults(pPath, CurrentAttrs))
			return FALSE;
		
		// don't apply attributes later
		AttrsAlreadyApplied = TRUE;

		// set the made node
		pMadeNode = pPath;
	}

	return TRUE;
}


/********************************************************************************************

>	ADDR CDRFilter::FindDataInObject(cdrfOffsetHeader *Header, WORD Type)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	returns the address of data of the given type in an objects with a standard
				offset header
	SeeAlso:	Filter

********************************************************************************************/

ADDR CDRFilter::FindDataInObject(cdrfOffsetHeader *Header, WORD Type)
{
	WORD *Types;
	WORD *Offsets;

	Types = (WORD *)(((BYTE *)Header) + CDRDATA_WORD(Header->TypesOffset));
	Offsets = (WORD *)(((BYTE *)Header) + CDRDATA_WORD(Header->OffsetsOffset));

	UINT32 l;

	for(l = 0; l < CDRDATA_WORD(Header->NOffsets); l++)
	{
		if(CDRDATA_WORD(Types[l]) == Type)
		{
			UINT32 Off = CDRDATA_WORD(Offsets[Header->NOffsets - l - 1]);
			if(Off > CDRDATA_WORD(Header->Size))
				return 0;
			
			return (ADDR)(((BYTE *)Header) + Off);
		}
	}

	return 0;
}


/********************************************************************************************

>	BOOL CDRFilter::GetMatrixFromTransform(Matrix *M, ADDR Trans, INT32 Size, BOOL TranslateForPage)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25 03 95
	Inputs:		the node to transform, and the trfd chunk to transform it by
	Returns:	error flag
	Purpose:	given some transform data, works out a matrix. This ignores any 3D transforms,
				TransformConvertedObject should be used in preference.
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::GetMatrixFromTransform(Matrix *M, ADDR Trans, INT32 Size, BOOL TranslateForPage)
{
	// set up a matrix conversion object
	CDRTransformConvertor *Conv;
	
	// putting the convertors here is probably more efficicent than allocating
	// a new one in the heap as they're not very big at all
	CDRTransformConvertor5 Conv5;
	CDRTransformConvertor4 Conv4;
	CDRTransformConvertor3 Conv3;
	
	switch(Version)
	{
		case CDRVERSION_5:	Conv = &Conv5;	break;
		case CDRVERSION_4:	Conv = &Conv4;	break;
		case CDRVERSION_3:	Conv = &Conv3;	break;
		default:			return FALSE;	break;
	}

	// initialise the convertor object
	Conv->Init(Trans, Size);
	
	// set up the initial matrix which just converts to millipoints
	// note that these elements are in Corel order
	cdrMatrix Ma, Mb, Mt;
	BOOL DoneFirst = FALSE;

	// apply all the transformations in the chunk
	INT32 t;
	for(t = Conv->GetNTransforms() - 1; t >= 0; t--)
	{
		// check that the transform is a matrix - for now we can only do matrices
		if(Conv->GetTransformType(t) != CDRTRANSFORMTYPE_MATRIX)
			continue;

		if(DoneFirst)
		{
			// done the first matrix, so we need get the matrix into another one and then combine.
			
			// get the matrix
			if(!Conv->GetMatrix(&Mb, t))
				return FALSE;

			// and combine it with the matrix we've already got
			Mt.a = (Mb.a * Ma.a) + (Mb.d * Ma.b);
			Mt.b = (Mb.b * Ma.a) + (Mb.e * Ma.b);
			Mt.c = (Mb.c * Ma.a) + (Mb.f * Ma.b) + Ma.c;
			Mt.d = (Mb.a * Ma.d) + (Mb.d * Ma.e);
			Mt.e = (Mb.b * Ma.d) + (Mb.e * Ma.e);
			Mt.f = (Mb.c * Ma.d) + (Mb.f * Ma.e) + Ma.f;

			Ma = Mt;
		} else {
			// just get the matrix
			if(!Conv->GetMatrix(&Ma, t))
				return FALSE;

			DoneFirst = TRUE;
		}
	}

	// if we didn't get a matrix at all, knock up a quick dummy one
	if(!DoneFirst)
	{
		cdrMatrix Mx = {1, 0, 0, 0, 1, 0};
	
		Ma = Mx;
	}
	
	// add in the translation bit
	if(TranslateForPage)
	{
		Ma.c += TranslateByX;
		Ma.f += TranslateByY;
	}

	// convert the final matrix into a Camelot matrix
	*M = Matrix(Ma.a, Ma.d, Ma.b, Ma.e, (INT32)Ma.c, (INT32)Ma.f);
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::IsTransformJustMatrices(ADDR Trans)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/05/95
	Inputs:		trfd chunk to examine
	Returns:	error flag
	Purpose:	scans a transform chunk. Returns TRUE if the transform was just composed of
				matrices and unknown transforms.
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::IsTransformJustMatrices(ADDR Trans, INT32 Size)
{
	// set up a matrix conversion object
	CDRTransformConvertor *Conv;
	
	// putting the convertors here is probably more efficicent than allocating
	// a new one in the heap as they're not very big at all
	CDRTransformConvertor5 Conv5;
	CDRTransformConvertor4 Conv4;
	CDRTransformConvertor3 Conv3;
	
	switch(Version)
	{
		case CDRVERSION_5:	Conv = &Conv5;	break;
		case CDRVERSION_4:	Conv = &Conv4;	break;
		case CDRVERSION_3:	Conv = &Conv3;	break;
		default:			return FALSE;	break;
	}

	// initialise the convertor object
	Conv->Init(Trans, Size);
	
	// check all the transformations in the chunk
	for(INT32 t = Conv->GetNTransforms() - 1; t >= 0; t--)
	{
		switch(Conv->GetTransformType(t))
		{
			case CDRTRANSFORMTYPE_MATRIX:
			case CDRTRANSFORMTYPE_UNKNOWN:
				break;

			default:
				return FALSE;
				break;
		}

	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::TransformConvertedObject(NodeRenderable **N, cdrfTransform *Trans, BOOL TranslateForPage = TRUE,
			BOOL IsAGroupTransform = FALSE, BOOL WorkAtOrigin = FALSE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25 03 95
	Inputs:		the node to transform, and the trfd chunk to transform it by
	Returns:	error flag
	Purpose:	transforms an object we converted, given a trfd chunk. If TranslateForPage
				is true (defaults to TRUE) then the object will also be translated to get
				the origin in the right place. The pointer may be modified to be a pointer
				to a different object.
				WorkAtOrigin causes the object to be translated back to the origin
				before being hacked apon, except if it's in a CDR3 file when it's ignored
				as the file is mass-translated at the end of the conversion..
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::TransformConvertedObject(NodeRenderable **N, ADDR Trans, INT32 Size, BOOL TranslateForPage,
						BOOL IsAGroupTransform, BOOL WorkAtOrigin)
{
	if(IsText)
	{
		(*N)->OptimiseAttributes();
	}

	// make sure the WorkAtOrigin flag is set correctly - we mustn't use it if we're fiddling with a v3 file
	if(Version == CDRVERSION_3) WorkAtOrigin = FALSE;

	// transform it back to the origin if necessary
	if(WorkAtOrigin)
	{
		Trans2DMatrix Trans(-TranslateByX, -TranslateByY);
		(*N)->Transform(Trans);
	}

	// set up a transform conversion object
	CDRTransformConvertor *Conv;

	// putting the convertors here is probably more efficicent than allocating
	// a new one in the heap as they're not very big at all
	CDRTransformConvertor5 Conv5;
	CDRTransformConvertor4 Conv4;
	CDRTransformConvertor3 Conv3;
	
	switch(Version)
	{
		case CDRVERSION_5:	Conv = &Conv5;	break;
		case CDRVERSION_4:	Conv = &Conv4;	break;
		case CDRVERSION_3:	Conv = &Conv3;	break;
		default:			return FALSE;	break;
	}

	// initialise the convertor object
	Conv->Init(Trans, Size);
	
	// note that these elements are in Corel order
	cdrMatrix Ma, Mb, Mt;
	BOOL HaveMatrix = FALSE;

	BOOL ApplyMatrices = IsAGroupTransform?FALSE:TRUE;

#ifdef CDR_DOEXTRUDE
	for(UINT32 g = 0; g < Conv->GetNTransforms(); g++)
	{
		CDRTransformType Type = Conv->GetTransformType(t);

		if(Type == CDRTRANSFORMTYPE_EXTRUDE)
			ApplyMatrices = FALSE;
	}
#endif


	// apply all the transformations in the chunk
	for(UINT32 t = 0; t < Conv->GetNTransforms(); t++)
	{
		CDRTransformType Type = Conv->GetTransformType(t);
		
		// check that the transform is something we know about
		if(Type == CDRTRANSFORMTYPE_UNKNOWN)
			continue;

		// if it's a matrix, then append it to the previous one, if any
		if(Type == CDRTRANSFORMTYPE_MATRIX)
		{
			if(ApplyMatrices)
			{
				if(HaveMatrix)
				{
					// done the first matrix, so we need get the matrix into another one and then combine.
			
					// get the matrix
					if(!Conv->GetMatrix(&Mb, t))
						return FALSE;

					// and combine it with the matrix we've already got
					Mt.a = (Ma.a * Mb.a) + (Ma.d * Mb.b);
					Mt.b = (Ma.b * Mb.a) + (Ma.e * Mb.b);
					Mt.c = (Ma.c * Mb.a) + (Ma.f * Mb.b) + Mb.c;
					Mt.d = (Ma.a * Mb.d) + (Ma.d * Mb.e);
					Mt.e = (Ma.b * Mb.d) + (Ma.e * Mb.e);
					Mt.f = (Ma.c * Mb.d) + (Ma.f * Mb.e) + Mb.f;

					Ma = Mt;
				} else {
					// just get the matrix
					if(!Conv->GetMatrix(&Ma, t))
						return FALSE;

					HaveMatrix = TRUE;
				}
			}
		}
		else
		{
			// wasn't a matrix - apply a matrix if we've got one
			if(HaveMatrix)
			{
				// apply it
				if(!ApplyMatrices)
				{
					if(!TransformConvertedObjectDoMatrix(N, &Ma, FALSE))
						return FALSE;
				}

			}
		
			// unset the flag
			HaveMatrix = FALSE;

			// deal with the transformation
			ERROR3IF(Type == CDRTRANSFORMTYPE_MATRIX, "Matrix got through in TransformConvertedObject")

			switch(Type)
			{
				case CDRTRANSFORMTYPE_PERSPECTIVE:
					{
						// convert the perpective
						cdrPerspective P;

						if(!Conv->GetPerspective(&P, t))
							return FALSE;

						// apply it to the object
						if(!TransformConvertedObjectDoPerspective(N, &P, IsAGroupTransform))
							return FALSE;
					}
					break;

				case CDRTRANSFORMTYPE_ENVELOPE:
					if(!(Version != CDRVERSION_3 && IsTextStory))		// text stories don't get enveloped - this is used as the frame shape
					{
						// convert the envelope into a path
						DocRect OBBox;
						Path *Shape = 0;
						INT32 Corners[4];

						if(Conv->GetEnvelope(&Shape, &OBBox, (INT32 *)&Corners, t))
						{
							// apply to object
							if(!TransformConvertedObjectDoEnvelope(N, Shape, &OBBox, (INT32 *)&Corners, IsAGroupTransform))
								return FALSE;

							// delete the shape
							delete Shape;
						}
					}
					break;

				case CDRTRANSFORMTYPE_EXTRUDE:
					{
						cdrExtrudeInfo Info;

						if(Conv->GetExtrude(&Info, t))
						{
							// apply to object
							if(!TransformConvertedObjectDoExtrude(N, &Info, IsAGroupTransform))
								return FALSE;
						}
						ApplyMatrices = TRUE;		// start applying the matrices...
					}
					break;
 
				default:
					break;
			}
			
			if(Version == CDRVERSION_5)
			{
				// we want to apply matrices from now on
				ApplyMatrices = TRUE;
			}
		}
	}

	// if we've got a matrix still to do...
	if(HaveMatrix)
	{
		// apply it
		if(ApplyMatrices)
		{
			if(!TransformConvertedObjectDoMatrix(N, &Ma, TranslateForPage))
				return FALSE;
		}
	}
	else if(!HaveMatrix && TranslateForPage)
	{
		// we need to apply an identity matrix which then gets translated, but only if we need to
		cdrMatrix Mi = {1, 0, 0, 0, 1, 0};
		
		// apply it
		if(!TransformConvertedObjectDoMatrix(N, &Mi, TranslateForPage))
			return FALSE;
	}

	// reverse the effect of the previous translation
	if(WorkAtOrigin)
	{
		Trans2DMatrix Trans(TranslateByX, TranslateByY);
		(*N)->Transform(Trans);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CDRFilter::TransformConvertedObjectDoMatrix(NodeRenderable **N, cdrMatrix *Ma, BOOL TranslateForPage)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		the node to transform, and the trfd chunk to transform it by
	Returns:	error flag
	Purpose:	transforms an object given a CDR matrix
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::TransformConvertedObjectDoMatrix(NodeRenderable **N, cdrMatrix *Ma, BOOL TranslateForPage)
{
	// make a camelot matrix
	Matrix M(Ma->a, Ma->d, Ma->b, Ma->e, (INT32)Ma->c + (TranslateForPage?TranslateByX:0),
			(INT32)Ma->f + (TranslateForPage?TranslateByY:0));

	// if it's a text story node, then it needs to be dealt with differently, and it's got
	// an associated bounding box which need transforming
	if(IS_A(*N, TextStory))
	{		
		// find it's bounding box
		DocRect *BBox = TextBBoxes.Find(*N);

		// transform it with the unmodifed matrix
	 	Trans2DMatrix TTransform(M);
		if(BBox != 0)
			TTransform.GetMatrix().TransformBounds(BBox);

		// if it's a text story, not just an artisitic text then don't scale it, but scale the import width
		if(IsTextStory)
		{
			// work out the scale factors along the axes
			double ScaleX = sqrt((Ma->a * Ma->a) + (Ma->b * Ma->b));
			double ScaleY = sqrt((Ma->d * Ma->d) + (Ma->e * Ma->e));
			
			// knock up a new matrix which isn't scaled along these axes
			M = Matrix(Ma->a / ScaleX, Ma->d / ScaleY, Ma->b / ScaleX, Ma->e / ScaleY,
					(INT32)Ma->c + (TranslateForPage?TranslateByX:0),
					(INT32)Ma->f + (TranslateForPage?TranslateByY:0));
		
			// get the import format width scaled nicely
			TextStory *TSNode = (TextStory *)(*N);
			double Width = ((double)TSNode->GetImportFormatWidth()) * ScaleX;
			TSNode->SetImportFormatWidth((INT32)Width);
		}
	}
	
	// transform the node
 	Trans2DMatrix Transform(M);
	(*N)->Transform(Transform);

	return TRUE;
}


/********************************************************************************************

>	CDRFilter::TransformConvertedObjectDoPerspective(NodeRenderable *N, cdrPerspective *P, BOOL TranslateShapeForPage = FALSE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		the node to transform, and the trfd chunk to transform it by
	Returns:	error flag
	Purpose:	transforms an object given a CDR perspective
	SeeAlso:	Filter

********************************************************************************************/

#define PERS_MOVE(co) {if(!pShape->InkPath.InsertMoveTo(co)) return FALSE;}
#define PERS_LINE(co) {if(!pShape->InkPath.InsertLineTo(co)) return FALSE;}
#define PERS_CLOSE {if(!pShape->InkPath.CloseSubPath()) return FALSE;}

BOOL CDRFilter::TransformConvertedObjectDoPerspective(NodeRenderable **N, cdrPerspective *P, BOOL TranslateShapeForPage)
{
TRACEUSER( "Ben", _T("applying perspective\n"));

	// create a mould parent and insert it in the tree
	NodeMould* pMouldParent = new NodeMould;
	if (pMouldParent == 0)
		return FALSE;

	// give the parent mould object a shape and mould space to work with and stick it
	// in the tree
	if(!pMouldParent->CreateGeometry(MOULDSPACE_PERSPECTIVE))
	{	
		delete pMouldParent;
		return FALSE;
	}

	// create a nice shape to mould into
	NodePath *pShape = new NodePath;
	if(pShape == 0 || (!pShape->SetUpPath()))
	{	
		delete pMouldParent;
		return FALSE;
	}

	// position the new elements at the beginning of the path
	pShape->InkPath.FindStartOfPath();

	// make a nice shape out of the corners
	DocCoord co;
	PERS_MOVE(P->NewCorners[0])
	PERS_LINE(P->NewCorners[1])
	PERS_LINE(P->NewCorners[2])
	PERS_LINE(P->NewCorners[3])
	PERS_LINE(P->NewCorners[0])
	PERS_CLOSE

	// maybe translate it a bit
	DocRect OriginalBBox = P->OriginalBBox;
	if(TranslateShapeForPage)
	{
		Trans2DMatrix Transform(Matrix(TranslateByX, TranslateByY));
		pShape->Transform(Transform);
	
		OriginalBBox.Translate(TranslateByX, TranslateByY);
	}

	// Create and add a mould shape object
	NodeMouldPath* pNodeMPath = pMouldParent->AddNewMouldShape(&pShape->InkPath, 0, NULL);
	if (!pNodeMPath)
		return FALSE;

	// set the geometry using this new mould shape
	if(!pMouldParent->GetGeometry()->Define(&(pNodeMPath->InkPath), &OriginalBBox))
		return FALSE;

	// now create the mould group object
	NodeMouldGroup* pMouldGroup = pMouldParent->AddNewMouldGroup(NULL);
	if (!pMouldGroup)
		return FALSE;

	// and attach the node to the molder group
	(*N)->AttachNode((Node *) pMouldGroup, FIRSTCHILD, FALSE);
		
	// set the node to the converted one
	*N = pMouldParent;

	// delete the shape
	delete pShape;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::TransformConvertedObjectDoEnvelope(NodeRenderable **N, Path *P, DocRect *OriginalBBox, INT32 *Corners, BOOL TranslateShapeForPage = FALSE);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/95
	Inputs:		the node to transform, and the trfd chunk to transform it by
	Returns:	error flag
	Purpose:	transforms an object given a path to envelope into
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::TransformConvertedObjectDoEnvelope(NodeRenderable **N, Path *P, DocRect *pOriginalBBox, INT32 *Corners, BOOL TranslateShapeForPage)
{
TRACEUSER( "Ben", _T("applying envelope\n"));

	// create a mould parent and insert it in the tree
	NodeMould* pMouldParent = new NodeMould;
	if (pMouldParent == 0)
		return FALSE;

	// give the parent mould object a shape and mould space to work with and stick it
	// in the tree
	if(!pMouldParent->CreateGeometry(MOULDSPACE_ENVELOPE))
	{	
		delete pMouldParent;
		return FALSE;
	}

	// validate the path a bit, and get a good one if this is duff
	Path *Shape = P;
	UINT32 errID;
	if(pMouldParent->GetGeometry()->Validate(Shape,errID) == FALSE)
	{
		// OK, it's not suitiable so try to create an approximation which is
		Path *NewShape;
		if(!pMouldParent->GetGeometry()->MakeValidFrom(&NewShape, Shape, Corners))
		{
			// couldn't make a proper one, so return TRUE to ignore the moulding
			return TRUE;
		}

		Shape = NewShape;

		// inc count of approximated envelopes so the user can be warned later
		EnvelopesApproximated++;
	}
	
	// maybe translate it a bit
	DocRect OriginalBBox = *pOriginalBBox;
	if(TranslateShapeForPage)
	{
		Shape->Translate(TranslateByX, TranslateByY);
	
		OriginalBBox.Translate(TranslateByX, TranslateByY);
	}

	// Create and add a mould shape object
	NodeMouldPath* pNodeMPath = pMouldParent->AddNewMouldShape(Shape, 0, NULL);
	if (!pNodeMPath)
		return FALSE;

	// set the geometry using this new mould shape
	if(!pMouldParent->GetGeometry()->Define(&(pNodeMPath->InkPath), &OriginalBBox))
		return FALSE;

	// now create the mould group object
	NodeMouldGroup* pMouldGroup = pMouldParent->AddNewMouldGroup(NULL);
	if (!pMouldGroup)
		return FALSE;

	// and attach the node to the molder group
	(*N)->AttachNode((Node *) pMouldGroup, FIRSTCHILD, FALSE);
		
	// set the node to the converted one
	*N = pMouldParent;

	// delete any created paths
	if(Shape != P)
		delete Shape;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::TransformConvertedObjectDoExtrude(NodeRenderable **N, cdrExtrudeInfo *Info,
			BOOL TranslateShapeForPage = FALSE);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/01/96
	Inputs:		node to transform, extrude info
	Returns:	error flag
	Purpose:	performs a extrude transform
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFilter::TransformConvertedObjectDoExtrude(NodeRenderable **N, cdrExtrudeInfo *Info, BOOL TranslateShapeForPage)
{
	// the plan here is to create a new group, and slap the node onto it (it could possibly be
	// a group with lots of other stuff under it, but we won't worry about that for the moment)
	// Then, wander down the sub-tree attempting to convert all NodeRenderableInk object into
	// NodePaths. This will hopefully result in a load of object which can then be transformed

	// first, get one of them there group thingynesseses.
	NodeGroup *pTempGroup = new NodeGroup;
	if(pTempGroup == 0)
		return FALSE;		// no room, oh dear.

	// now attach the thing to it. It isn't attached to anything, so it's easy.
	(*N)->AttachNode(pTempGroup, FIRSTCHILD);

	// scan the tree converting non-NodePaths to NodePaths where possible
	BecomeA ParamBecomeA(BECOMEA_REPLACE, 
							 CC_RUNTIME_CLASS(NodePath),
							 NULL);

	// find the first node to think about
	Node* Current = (*N); 
	while (Current->FindFirstChild() != NULL && !(Current->FindFirstChild())->IsKindOf(CC_RUNTIME_CLASS(TextStory))) 
	{
		Current = Current->FindFirstChild(); 
	}  

	// check each renderable node for not being a path
	while(Current != 0)
	{
		if(Current->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)) && !Current->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
		{
			if (Current->CanBecomeA(&ParamBecomeA))
			{
				// is it a text story?
				if (Current->IsKindOf(CC_RUNTIME_CLASS(TextStory)))
				{
					// give formatting it a go
					((TextStory *)Current)->FormatAndChildren();
				}
				
				if (!(Current)->DoBecomeA(&ParamBecomeA))
				{
					return FALSE;
				}
			}
		}

		// get the next one
		Current = Current->FindNextDepthFirst(*N);
	}

	// now wander through the tree doing the funky transform - find the first one again
	Current = pTempGroup; 
	while (Current->FindFirstChild() != NULL) 
	{
		Current = Current->FindFirstChild(); 
	}  

	// transform each NodePath
	while(Current != 0)
	{
		if(IS_A(Current, NodePath))
		{
			if(!TransformConvertedObjectDoExtrudeTransPath((NodePath *)Current, Info))
				return FALSE;		// something very wrong happened
		}

		// get the next one
		Current = Current->FindNextDepthFirst(pTempGroup);
	}

	// update the *N pointer to the node
	(*N) = (NodeRenderable *)pTempGroup->FindFirstChild();
	ERROR2IF((*N) == 0, FALSE, "Oi! there that node just vanished");
	ERROR2IF(!(*N)->IsKindOf(CC_RUNTIME_CLASS(NodeRenderable)), FALSE, "Oh! Mr. Node changing into something non-renderable");

	// remove it from the group
	(*N)->UnlinkNodeFromTree();

	// finally, vape that temperary group
	delete pTempGroup;
	
	return TRUE;
}

/********************************************************************************************

>	BOOL CDRFilter::TransformConvertedObjectDoExtrudeTransPath(NodePath *pPath, cdrExtrudeInfo *Info)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/96
	Inputs:		path to transform, info about the transform
	Returns:	error flag
	Purpose:	does the extrude transform on a path
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFilter::TransformConvertedObjectDoExtrudeTransPath(NodePath *pPath, cdrExtrudeInfo *Info)
{
	// check the inputs
	ERROR2IF(pPath == 0 || Info == 0, FALSE, "Null something supplied where a raspberry is required");
	ERROR2IF(!IS_A(pPath, NodePath), FALSE, "Oi! I asked for a NodePath you silly person");

	// get some useful info about that there path
	DocCoord *Coords = pPath->InkPath.GetCoordArray();
	ERROR2IF(Coords == 0, FALSE, "Hmmm. I think that path is a bit of a dodgy one really");
	INT32 NCoords = pPath->InkPath.GetNumCoords();

	// mild sanity check...
	if(Info->Camera.z == 0)
		return TRUE;			// don't try this one at home, kiddies

	// prepare a few handy values
	FLOAT8 projA = - Info->Camera.x / Info->Camera.z;
	FLOAT8 projB = - Info->Camera.y / Info->Camera.z;
	FLOAT8 projC = - 1 / Info->Camera.z;

	// run through that path transforming all those lovely cheeky little coordinates
	for(INT32 c = 0; c < NCoords; c++)
	{
		FLOAT8 x = (FLOAT8)Coords[c].x, y = (FLOAT8)Coords[c].y;

		FLOAT8 alpha =	x * Info->Trans.cf11 + y * Info->Trans.cf21 + Info->Trans.cf41;
		FLOAT8 beta =	x * Info->Trans.cf12 + y * Info->Trans.cf22 + Info->Trans.cf42;
		FLOAT8 gamma = 	x * Info->Trans.cf13 + y * Info->Trans.cf23 + Info->Trans.cf43;

		FLOAT8 norm =	gamma * projC + 1;

		Coords[c].x = (INT32)((alpha + gamma * projA) / norm);
		Coords[c].y = (INT32)((beta  + gamma * projB) / norm);
	}

	// well, wasn't that squishy?
	return TRUE;
}

/********************************************************************************************

>	BOOL CDRTransformConvertor5::Init(ADDR Data, INT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/95
	Inputs:		pointer to trfl list and it's size
	Returns:	whether the contents are OK
	Purpose:	intialises this convertor
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor5::Init(ADDR Data, INT32 Size)
{
	List = Data;
	ListSize = Size;
 
 	ADDR TransformData;

	if(!RIFFFile::FindChunkWithinListContents(List, ListSize, cdrT_trfd, 0, &TransformData, 0))
		return 0;

 	Trans = (cdrfTransform *)TransformData;
 
 	return TRUE;
 }


/********************************************************************************************

>	BOOL CDRTransformConvertor4::Init(ADDR Data, INT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/95
	Inputs:		pointer to trfl list and it's size
	Returns:	whether the contents are OK
	Purpose:	intialises this convertor
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor4::Init(ADDR Data, INT32 Size)
{
	List = Data;
	ListSize = Size;
 
 	ADDR TransformData;

	if(!RIFFFile::FindChunkWithinListContents(List, ListSize, cdrT_trfd, 0, &TransformData, 0))
		return 0;

 	Trans = (cdrfTransformV4 *)TransformData;
 
 	return TRUE;
 }


/********************************************************************************************

>	CDRTransformType CDRTransformConvertor5::GetTransformType(UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 03 95
	Inputs:		transform number
	Returns:	a transform type
	Purpose:	gets a transform type from an trfd chunk
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRTransformType CDRTransformConvertor5::GetTransformType(UINT32 n)
{
	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return CDRTRANSFORMTYPE_UNKNOWN;

	cdrfTransformHeader *Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + CDRDATA_WORD(Trans->TransformOffsets[n]));

	switch(CDRDATA_WORD(Hdr->Type))
	{
		case cdrfTRANSFORMTYPE_MATRIX:
			return CDRTRANSFORMTYPE_MATRIX;
			break;
		
		case cdrfTRANSFORMTYPE_PERSPECTIVE:
			return CDRTRANSFORMTYPE_PERSPECTIVE;
			break;
		
#ifdef CDR_DOEXTRUDE
		case cdrfTRANSFORMTYPE_EXTRUDE:
			return CDRTRANSFORMTYPE_EXTRUDE;
			break;
#endif

		case cdrfTRANSFORMTYPE_ENVELOPE:
			return CDRTRANSFORMTYPE_ENVELOPE;
			break;
		
		default:
			TRACEUSER( "Ben", _T("[] Unknown v5 transform type\n"));
			break;
	}

	return CDRTRANSFORMTYPE_UNKNOWN;
}


/********************************************************************************************

>	CDRTransformType CDRTransformConvertor4::GetTransformType(UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 03 95
	Inputs:		transform number
	Returns:	a transform type
	Purpose:	gets a transform type from an trfd chunk
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRTransformType CDRTransformConvertor4::GetTransformType(UINT32 n)
{
	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return CDRTRANSFORMTYPE_UNKNOWN;

	WORD *Offsets = (WORD *)(((ADDR)Trans) + CDRDATA_WORD(Trans->OffsetsStart));
	cdrfTransformHeader *Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + CDRDATA_WORD(Offsets[n]));

	switch(CDRDATA_WORD(Hdr->Type))
	{
		case cdrfTRANSFORMTYPE_MATRIX:
			return CDRTRANSFORMTYPE_MATRIX;
			break;
		
		case cdrfTRANSFORMTYPE_PERSPECTIVE:
			return CDRTRANSFORMTYPE_PERSPECTIVE;
			break;
		
#ifdef CDR_DOEXTRUDE
		case cdrfTRANSFORMTYPE_EXTRUDE:
			return CDRTRANSFORMTYPE_EXTRUDE;
			break;
#endif

		case cdrfTRANSFORMTYPE_ENVELOPE:
			return CDRTRANSFORMTYPE_ENVELOPE;
			break;
					
		default:
			TRACEUSER( "Ben", _T("[] Unknown v4 transform type\n"));
			break;
	}

	return CDRTRANSFORMTYPE_UNKNOWN;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor5::GetMatrix(cdrMatrix *M, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 03 95
	Inputs:		matrix to convert into, transform number
	Returns:	a transform type
	Purpose:	converts a matrix from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor5::GetMatrix(cdrMatrix *M, UINT32 n)
{
	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

	cdrfTransformHeader *Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + CDRDATA_WORD(Trans->TransformOffsets[n]));

	ERROR3IF(CDRDATA_WORD(Hdr->Type) != cdrfTRANSFORMTYPE_MATRIX, "GetMatrix called for a non matrix trfd entry");
	ERROR3IF((sizeof(FLOAT8) / 2) != sizeof(DWORD) || sizeof(DWORD) != 4, "Non valid sizeof assumptions");

	// OK, so apparently either PCs or Acorns store them the wrong way round. Oh well.
	FLOAT8 *TData = (FLOAT8 *)(Hdr + 1);
	FLOAT8 *Out = (FLOAT8 *)M;

	INT32 l;
	for(l = 0; l < 6; l++)
	{
		Out[l] = CDRDATA_FLOAT8(TData[l]);
	}
	
	// change the translation vector to be in millipoints
	M->c *= CDRCOORDS_TO_MILLIPOINTS;
	M->f *= CDRCOORDS_TO_MILLIPOINTS;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor::ConvertPerspective(cdrPerspective *P, cdrfPerspectiveTransform *Trans)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		perspective to convert into, transform number
	Returns:	a transform type
	Purpose:	converts corel perspective - used by all the convertors
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor::ConvertPerspective(cdrPerspective *P, cdrfPerspectiveTransform *Tran)
{
	// knock up the bbox
	INT32 x0, y0, y1, x1;
	x0 = CDRDATA_SWORD(Tran->Ox0) * CDRCOORDS_TO_MILLIPOINTS;
	y0 = CDRDATA_SWORD(Tran->Oy0) * CDRCOORDS_TO_MILLIPOINTS;
	x1 = CDRDATA_SWORD(Tran->Ox1) * CDRCOORDS_TO_MILLIPOINTS;
	y1 = CDRDATA_SWORD(Tran->Oy1) * CDRCOORDS_TO_MILLIPOINTS;

	if(x0 > x1)
	{
		INT32 t = x1;
		x1 = x0;
		x0 = t;
	}
	
	if(y0 > y1)
	{
		INT32 t = y1;
		y1 = y0;
		y0 = t;
	}

	P->OriginalBBox.lo.x = x0;
	P->OriginalBBox.lo.y = y0;
	P->OriginalBBox.hi.x = x1;
	P->OriginalBBox.hi.y = y1;

	// and the new corners
	for(INT32 l = 0; l < 4; l++)
	{
		P->NewCorners[l].x = CDRDATA_SWORD(Tran->NewCorners[l].X) * CDRCOORDS_TO_MILLIPOINTS;
		P->NewCorners[l].y = CDRDATA_SWORD(Tran->NewCorners[l].Y) * CDRCOORDS_TO_MILLIPOINTS;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor5::GetPerspective(cdrPerspective *P, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		perspective to convert into, transform number
	Returns:	a transform type
	Purpose:	converts a perspective from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor5::GetPerspective(cdrPerspective *P, UINT32 n)
{
	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

	cdrfTransformHeader *Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + CDRDATA_WORD(Trans->TransformOffsets[n]));

	ERROR3IF(CDRDATA_WORD(Hdr->Type) != cdrfTRANSFORMTYPE_PERSPECTIVE, "GetPerspective called for a non matrix trfd entry");

	cdrfPerspectiveEntry *Per = (cdrfPerspectiveEntry *)Hdr;

	return ConvertPerspective(P, &Per->Trans);
}


/********************************************************************************************

>	BOOL CDRTransformConvertor4::GetPerspective(cdrPerspective *P, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		perspective to convert into, transform number
	Returns:	a transform type
	Purpose:	converts a perspective from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor4::GetPerspective(cdrPerspective *P, UINT32 n)
{
	if(n >= Trans->NTransforms)
		return FALSE;

	WORD *Offsets = (WORD *)(((ADDR)Trans) + Trans->OffsetsStart);
	cdrfTransformHeader *Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + Offsets[n]);

	ERROR3IF(Hdr->Type != cdrfTRANSFORMTYPE_PERSPECTIVE, "GetPerspective called for a non matrix trfd entry");

	cdrfPerspectiveEntry *Per = (cdrfPerspectiveEntry *)Hdr;

	return ConvertPerspective(P, &	Per->Trans);
}


/********************************************************************************************

>	CDRTransformType CDRTransformConvertor4::GetMatrix(cdrMatrix *M, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 03 95
	Inputs:		matrix to convert into, transform number
	Returns:	a transform type
	Purpose:	converts a matrix from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor4::GetMatrix(cdrMatrix *M, UINT32 n)
{
	if(n >= Trans->NTransforms)
		return FALSE;

	WORD *Offsets = (WORD *)(((ADDR)Trans) + Trans->OffsetsStart);
	cdrfTransformHeader *Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + Offsets[n]);

	ERROR3IF(Hdr->Type != cdrfTRANSFORMTYPE_MATRIX, "GetMatrix called for a non matrix trfd entry");

 	// unsigned word pointer
 	WORD *TDataUS = (WORD *)(Hdr + 1);
	// signed word pointer
	SWORD *TDataS = (SWORD *)(Hdr + 1);
	// signed dword pointer
	SDWORD *TDDataS = (SDWORD *)(Hdr + 1);

	// convert the matrix - an interesting mix of 16.16 fixed point values and a
	// signed double word translation vector
	M->a = (FLOAT8)CDRDATA_SWORD(TDataS[1]) + ((FLOAT8)CDRDATA_WORD(TDataUS[0]) / 0xffff);
	M->b = (FLOAT8)CDRDATA_SWORD(TDataS[3]) + ((FLOAT8)CDRDATA_WORD(TDataUS[2]) / 0xffff);
	M->c = (FLOAT8)(CDRDATA_DSWORD(TDDataS[2]) * CDRCOORDS_TO_MILLIPOINTS);
	M->d = (FLOAT8)CDRDATA_SWORD(TDataS[7]) + ((FLOAT8)CDRDATA_WORD(TDataUS[6]) / 0xffff);
	M->e = (FLOAT8)CDRDATA_SWORD(TDataS[9]) + ((FLOAT8)CDRDATA_WORD(TDataUS[8]) / 0xffff);
	M->f = (FLOAT8)(CDRDATA_DSWORD(TDDataS[5]) * CDRCOORDS_TO_MILLIPOINTS);

	return TRUE;
}


/********************************************************************************************

>	CDRTransformType CDRTransformConvertor3::GetTransformType(UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/95
	Inputs:		transform number
	Returns:	a transform type
	Purpose:	gets a transform type from an trfd chunk
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRTransformType CDRTransformConvertor3::GetTransformType(UINT32 n)
{
	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return CDRTRANSFORMTYPE_UNKNOWN;

	cdrfTransformEntryV3 *En = (cdrfTransformEntryV3 *)(Trans + 1);

	switch(CDRDATA_WORD(En[n].Type))
	{
		case cdrfTRANSFORMTYPE_MATRIX_V3:
			return CDRTRANSFORMTYPE_MATRIX;
			break;
		
		case cdrfTRANSFORMTYPE_PERSPECTIVE_V3:
			return CDRTRANSFORMTYPE_PERSPECTIVE;
			break;
		
		case cdrfTRANSFORMTYPE_ENVELOPE_V3:
			return CDRTRANSFORMTYPE_ENVELOPE;
			break;
		
#ifdef CDR_DOEXTRUDE
		case cdrfTRANSFORMTYPE_EXTRUDE_V3:
			return CDRTRANSFORMTYPE_EXTRUDE;
			break;
#endif

		default:
			TRACEUSER( "Ben", _T("[] Unknown v3 transform type %x\n"), En[n].Type);
			break;
	}

	return CDRTRANSFORMTYPE_UNKNOWN;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor3::GetExtrude(cdrExtrudeInfo *Info, UINT32 n);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/01/96
	Inputs:		extrude info to convert to, transform number
	Returns:	a transform type
	Purpose:	converts a matrix from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor3::GetExtrude(cdrExtrudeInfo *Info, UINT32 n)
{
 	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

 	cdrfTransformEntryV3 *En = (cdrfTransformEntryV3 *)(Trans + 1);
	cdrfTransformExtrudeV3 *Data = (cdrfTransformExtrudeV3 *)(((BYTE *)Trans) + En[n].Offset);

	Info->Trans.cf11 = (FLOAT8)CDRDATA_SWORD(Data->cf11b) + ((FLOAT8)CDRDATA_WORD(Data->cf11a) / 0xffff);
	Info->Trans.cf12 = (FLOAT8)CDRDATA_SWORD(Data->cf12b) + ((FLOAT8)CDRDATA_WORD(Data->cf12a) / 0xffff);
	Info->Trans.cf13 = (FLOAT8)CDRDATA_SWORD(Data->cf13b) + ((FLOAT8)CDRDATA_WORD(Data->cf13a) / 0xffff);
	Info->Trans.cf21 = (FLOAT8)CDRDATA_SWORD(Data->cf21b) + ((FLOAT8)CDRDATA_WORD(Data->cf21a) / 0xffff);
	Info->Trans.cf22 = (FLOAT8)CDRDATA_SWORD(Data->cf22b) + ((FLOAT8)CDRDATA_WORD(Data->cf22a) / 0xffff);
	Info->Trans.cf23 = (FLOAT8)CDRDATA_SWORD(Data->cf23b) + ((FLOAT8)CDRDATA_WORD(Data->cf23a) / 0xffff);
	Info->Trans.cf31 = (FLOAT8)CDRDATA_SWORD(Data->cf31b) + ((FLOAT8)CDRDATA_WORD(Data->cf31a) / 0xffff);
	Info->Trans.cf32 = (FLOAT8)CDRDATA_SWORD(Data->cf32b) + ((FLOAT8)CDRDATA_WORD(Data->cf32a) / 0xffff);
	Info->Trans.cf33 = (FLOAT8)CDRDATA_SWORD(Data->cf33b) + ((FLOAT8)CDRDATA_WORD(Data->cf33a) / 0xffff);
	Info->Trans.cf41 = (FLOAT8)CDRDATA_FLOAT4(Data->f41);
	Info->Trans.cf42 = (FLOAT8)CDRDATA_FLOAT4(Data->f42);
	Info->Trans.cf43 = (FLOAT8)CDRDATA_FLOAT4(Data->f43);

	Info->Camera.x = CDRDATA_SWORD(Data->Cx) * CDRCOORDS_TO_MILLIPOINTS;
	Info->Camera.y = CDRDATA_SWORD(Data->Cy) * CDRCOORDS_TO_MILLIPOINTS;
	Info->Camera.z = CDRDATA_DWORD(Data->Cz) * CDRCOORDS_TO_MILLIPOINTS;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor::ConvertExtrudeInfo(cdrfTransformExtrude *pTransform, cdrExtrudeInfo *pInfo)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/96
	Inputs:		pointer to transform from file, extrude info to convert to
	Returns:	error flag
	Purpose:	converts an extrude thingy from a v4,5,6 file
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor::ConvertExtrudeInfo(cdrfTransformExtrude *pTransform, cdrExtrudeInfo *Info)
{
	Info->Trans.cf11 = (FLOAT8)CDRDATA_SWORD(pTransform->cf11b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf11a) / 0xffff);
	Info->Trans.cf12 = (FLOAT8)CDRDATA_SWORD(pTransform->cf12b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf12a) / 0xffff);
	Info->Trans.cf13 = (FLOAT8)CDRDATA_SWORD(pTransform->cf13b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf13a) / 0xffff);
	Info->Trans.cf21 = (FLOAT8)CDRDATA_SWORD(pTransform->cf21b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf21a) / 0xffff);
	Info->Trans.cf22 = (FLOAT8)CDRDATA_SWORD(pTransform->cf22b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf22a) / 0xffff);
	Info->Trans.cf23 = (FLOAT8)CDRDATA_SWORD(pTransform->cf23b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf23a) / 0xffff);
	Info->Trans.cf31 = (FLOAT8)CDRDATA_SWORD(pTransform->cf31b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf31a) / 0xffff);
	Info->Trans.cf32 = (FLOAT8)CDRDATA_SWORD(pTransform->cf32b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf32a) / 0xffff);
	Info->Trans.cf33 = (FLOAT8)CDRDATA_SWORD(pTransform->cf33b) + ((FLOAT8)CDRDATA_WORD(pTransform->cf33a) / 0xffff);
	Info->Trans.cf41 = (FLOAT8)CDRDATA_DWORD(pTransform->f41);
	Info->Trans.cf42 = (FLOAT8)CDRDATA_DWORD(pTransform->f42);
	Info->Trans.cf43 = (FLOAT8)CDRDATA_DWORD(pTransform->f43);

	Info->Camera.x = CDRDATA_SWORD(pTransform->Cx) * CDRCOORDS_TO_MILLIPOINTS;
	Info->Camera.y = CDRDATA_SWORD(pTransform->Cy) * CDRCOORDS_TO_MILLIPOINTS;
	Info->Camera.z = CDRDATA_DWORD(pTransform->Cz) * CDRCOORDS_TO_MILLIPOINTS;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor4::GetExtrude(cdrExtrudeInfo *Info, UINT32 n);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/01/96
	Inputs:		extrude info to convert to, transform number
	Returns:	a transform type
	Purpose:	converts a matrix from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor4::GetExtrude(cdrExtrudeInfo *Info, UINT32 n)
{
	if(n >= Trans->NTransforms)
		return FALSE;

	WORD *Offsets = (WORD *)(((ADDR)Trans) + Trans->OffsetsStart);
	cdrfTransformHeader *Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + Offsets[n]);

	return ConvertExtrudeInfo((cdrfTransformExtrude *)Hdr, Info);
}


/********************************************************************************************

>	BOOL CDRTransformConvertor5::GetExtrude(cdrExtrudeInfo *Info, UINT32 n);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/01/96
	Inputs:		extrude info to convert to, transform number
	Returns:	a transform type
	Purpose:	converts a matrix from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor5::GetExtrude(cdrExtrudeInfo *Info, UINT32 n)
{
	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

	cdrfTransformHeader *Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + CDRDATA_WORD(Trans->TransformOffsets[n]));

	return ConvertExtrudeInfo((cdrfTransformExtrude *)Hdr, Info);
}


/********************************************************************************************

>	CDRTransformType CDRTransformConvertor3::GetMatrix(cdrMatrix *M, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/95
	Inputs:		matrix to convert into, transform number
	Returns:	a transform type
	Purpose:	converts a matrix from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor3::GetMatrix(cdrMatrix *M, UINT32 n)
{
 	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

 	cdrfTransformEntryV3 *En = (cdrfTransformEntryV3 *)(Trans + 1);
	BYTE *Data = ((BYTE *)Trans) + En[n].Offset;

	// unsigned word pointer
 	WORD *TDataUS = (WORD *)Data;
	// signed word pointer
	SWORD *TDataS = (SWORD *)Data;
	// signed dword pointer
	SDWORD *TDDataS = (SDWORD *)Data;

	// convert the matrix - an interesting mix of 16.16 fixed point values and a
	// signed double word translation vector
	M->a = (FLOAT8)CDRDATA_SWORD(TDataS[1]) + ((FLOAT8)CDRDATA_WORD(TDataUS[0]) / 0xffff);
	M->b = (FLOAT8)CDRDATA_SWORD(TDataS[3]) + ((FLOAT8)CDRDATA_WORD(TDataUS[2]) / 0xffff);
	M->c = (FLOAT8)(CDRDATA_DSWORD(TDDataS[2]) * CDRCOORDS_TO_MILLIPOINTS);
	M->d = (FLOAT8)CDRDATA_SWORD(TDataS[7]) + ((FLOAT8)CDRDATA_WORD(TDataUS[6]) / 0xffff);
	M->e = (FLOAT8)CDRDATA_SWORD(TDataS[9]) + ((FLOAT8)CDRDATA_WORD(TDataUS[8]) / 0xffff);
	M->f = (FLOAT8)(CDRDATA_DSWORD(TDDataS[5]) * CDRCOORDS_TO_MILLIPOINTS);

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor3::GetPerspective(cdrPerspective *P, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		perspective to convert into, transform number
	Returns:	a transform type
	Purpose:	converts a perspective from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor3::GetPerspective(cdrPerspective *P, UINT32 n)
{
 	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

 	cdrfTransformEntryV3 *En = (cdrfTransformEntryV3 *)(Trans + 1);
	BYTE *Data = ((BYTE *)Trans) + En[n].Offset;

	return ConvertPerspective(P, (cdrfPerspectiveTransform *)Data);
}



/********************************************************************************************

>	BOOL CDRTransformConvertor::ConvertEnvelopeBBox(ADDR Entry, DocRect *BBox, INT32 *Corners)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/95
	Inputs:		Pointer to entry in transform chunk and an output DocRect
	Returns:	sucess
	Purpose:	converts a envelope original bounding box
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor::ConvertEnvelopeBBox(ADDR Entry, DocRect *BBox, INT32 *Corners)
{
	// get the bounding box;
	cdrfTransformEnvelope *En = (cdrfTransformEnvelope *)Entry;

	INT32 x0, y0, y1, x1;
	x0 = CDRDATA_SWORD(En->Ox0) * CDRCOORDS_TO_MILLIPOINTS;
	y0 = CDRDATA_SWORD(En->Oy0) * CDRCOORDS_TO_MILLIPOINTS;
	x1 = CDRDATA_SWORD(En->Ox1) * CDRCOORDS_TO_MILLIPOINTS;
	y1 = CDRDATA_SWORD(En->Oy1) * CDRCOORDS_TO_MILLIPOINTS;

	if(x0 > x1)
	{
		INT32 t = x1;
		x1 = x0;
		x0 = t;
	}
	
	if(y0 > y1)
	{
		INT32 t = y1;
		y1 = y0;
		y0 = t;
	}

	BBox->lo.x = x0;
	BBox->lo.y = y0;
	BBox->hi.x = x1;
	BBox->hi.y = y1;

	// fill in the corners
	for(INT32 l = 0; l < 4; l++)
	{
		Corners[l] = CDRDATA_WORD(En->Corners[l]);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor::ConvertEnvelopePath(ADDR List, INT32 ListSize, INT32 Entry, Path **ppPath);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/95
	Inputs:		pointer to list, list size, entry number to get and a pointer to a path to fill in
	Returns:	sucess
	Purpose:	converts a envelope path
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor::ConvertEnvelopePath(ADDR List, INT32 ListSize, INT32 Entry, Path **ppPath)
{
	// find the envd chunk we're interested in
	ADDR Chunk;
	INT32 ChunkSize;
	if(!RIFFFile::FindChunkWithinListContents(List, ListSize, cdrT_envd, Entry, &Chunk, &ChunkSize))
		return FALSE;

	// right, got the thingy...
	cdrfOffsetHeader *Hdr = (cdrfOffsetHeader *)Chunk;
	INT32 NCoords = CDRDATA_WORD(Hdr->ObjectType);	// hold the number of coords...

	// find the things within the chunk
	ADDR Coords = CDRFilter::FindDataInObject(Hdr, cdrfENVELOPEOFFSETTYPE_COORDS);
	ADDR Types = CDRFilter::FindDataInObject(Hdr, cdrfENVELOPEOFFSETTYPE_TYPES);

	if(Coords == 0 || Types == 0)
		return FALSE;

	// make a path from this all...
	// knock up a path node
	Path *pPath = new Path;
	if(pPath == 0 || (!pPath->Initialise()))
	{
		delete pPath;
		return FALSE;			// no room to create it
	}

	// position the new elements at the beginning of the path
	pPath->FindStartOfPath();

	if(!CDRFilter::AddElementsToPath(pPath, NCoords, (cdrfCoord *)Coords, Types, 0))
		return FALSE;

	// check that the path is OK
	if(!pPath->EnsureValid())
	{
		// no, it's completely knackered
		delete pPath;
		return FALSE;
	}

	// return it...
	(*ppPath) = pPath;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor5::GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/95
	Inputs:		pointer to a pointer to a nodepath to fill in, a docrect for the orignial BBox
				and the position
	Returns:	a transform type
	Purpose:	converts a envelope from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor5::GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n)
{
	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

	cdrfTransformHeader *Hdr;

	// run through to this entry counting the number of entries before this one
	INT32 EnvelopesSoFar = 0;
	for(UINT32 l = 0; l <= n; l++)
	{
		Hdr = (cdrfTransformHeader *)(((ADDR)Trans) + CDRDATA_WORD(Trans->TransformOffsets[l]));

		// increment the number of envelopes so far...
		if(CDRDATA_WORD(Hdr->Type) == cdrfTRANSFORMTYPE_ENVELOPE)
			EnvelopesSoFar++;
	}

	ERROR2IF(EnvelopesSoFar <= 0, FALSE, "No envelopes found!");
	
	// OK, convert this entry...
	if(!ConvertEnvelopeBBox((ADDR)Hdr, OriginalBBox, Corners))
		return FALSE;

	if(!ConvertEnvelopePath(List, ListSize, EnvelopesSoFar - 1, P))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor4::GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/95
	Inputs:		pointer to a pointer to a nodepath to fill in, a docrect for the orignial BBox
				and the position
	Returns:	a transform type
	Purpose:	converts a envelope from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRTransformConvertor4::GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n)
{
	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

	cdrfTransformHeader *Hdr;
	WORD *Offsets = (WORD *)(((ADDR)Trans) + Trans->OffsetsStart);

	// run through to this entry counting the number of entries before this one
	INT32 EnvelopesSoFar = 0;
	for(UINT32 l = 0; l <= n; l++)
	{
		Hdr = (cdrfTransformHeader *)(cdrfTransformHeader *)(((ADDR)Trans) + Offsets[l]);

		// increment the number of envelopes so far...
		if(CDRDATA_WORD(Hdr->Type) == cdrfTRANSFORMTYPE_ENVELOPE)
			EnvelopesSoFar++;
	}

	ERROR2IF(EnvelopesSoFar <= 0, FALSE, "No envelopes found!");
	
	// OK, convert this entry...
	if(!ConvertEnvelopeBBox((ADDR)Hdr, OriginalBBox, Corners))
		return FALSE;

	if(!ConvertEnvelopePath(List, ListSize, EnvelopesSoFar - 1, P))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRTransformConvertor3::GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/95
	Inputs:		pointer to a pointer to a nodepath to fill in, a docrect for the orignial BBox
				and the position
	Returns:	a transform type
	Purpose:	converts a envelope from position N
	SeeAlso:	CDRFilter

********************************************************************************************/

#define ENVV3_NCOORDS	25

BOOL CDRTransformConvertor3::GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n)
{
 	if(n >= CDRDATA_WORD(Trans->NTransforms))
		return FALSE;

 	cdrfTransformEntryV3 *En = (cdrfTransformEntryV3 *)(Trans + 1);
	BYTE *Data = ((BYTE *)Trans) + En[n].Offset;

	// make a pointer to the interesting data
	cdrfTransformEnvelopeV3 *Env = (cdrfTransformEnvelopeV3 *)Data;

	// convert the original bbox
	INT32 x0, y0, y1, x1;
	x0 = CDRDATA_SWORD(Env->Ox0) * CDRCOORDS_TO_MILLIPOINTS;
	y0 = CDRDATA_SWORD(Env->Oy0) * CDRCOORDS_TO_MILLIPOINTS;
	x1 = CDRDATA_SWORD(Env->Ox1) * CDRCOORDS_TO_MILLIPOINTS;
	y1 = CDRDATA_SWORD(Env->Oy1) * CDRCOORDS_TO_MILLIPOINTS;

	if(x0 > x1)
	{
		INT32 t = x1;
		x1 = x0;
		x0 = t;
	}
	
	if(y0 > y1)
	{
		INT32 t = y1;
		y1 = y0;
		y0 = t;
	}

	OriginalBBox->lo.x = x0;
	OriginalBBox->lo.y = y0;
	OriginalBBox->hi.x = x1;
	OriginalBBox->hi.y = y1;

	// get the coords
	DocCoord Co[ENVV3_NCOORDS];
	INT32 l;
	for(l = 0; l < ENVV3_NCOORDS; l++)
	{
		Co[l].x = CDRDATA_SWORD(Env->Coords[l].X) * CDRCOORDS_TO_MILLIPOINTS;
		Co[l].y = CDRDATA_SWORD(Env->Coords[l].Y) * CDRCOORDS_TO_MILLIPOINTS;
	}

	// create the path
	Path *pShape = new Path;
	if(pShape == 0 || (!pShape->Initialise()))
	{	
		delete pShape;
		return FALSE;
	}

	// position the new elements at the beginning of the path
	pShape->FindStartOfPath();

	// make a nice shape out of the corners
	if(!pShape->InsertMoveTo(Co[0]))
		return FALSE;
	for(INT32 c = 1; c < ENVV3_NCOORDS; c += 3)
	{                     
		if(!pShape->InsertCurveTo(Co[c], Co[c + 1], Co[c + 2]))
			return FALSE;
	}
	if(!pShape->CloseSubPath())
		return FALSE;
	
	*P = pShape;

	// fill in the corners
	Corners[0] = 0;
	Corners[1] = 6;
	Corners[2] = 12;
	Corners[3] = 18;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ClearUp(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		
	Returns:	
	Purpose:	Clears up after an CDR file import
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::ClearUp(void)
{
	Version = CDRVERSION_NULL;

	if(SlowJobStarted == TRUE)
		EndSlowJob();

	if(Success == FALSE)
	{
		// get rid of the actions list
		while(!Actions.IsEmpty())
		{
			CDRActionListItem *Act = (CDRActionListItem *)Actions.GetTail();

			ERROR3IF(Act == 0, "Returned tail in a non-empty Action list is 0");

			Act->ImportFailure(this);

			delete Actions.RemoveItem(Act);
		}

		// get rid of the level's list of nodes
		DeleteNodeList(pLevelNodeList);

		// tell everyone about some new layers, if we were doing them
		if(UseLayers)
		{
			LayerSGallery::MakeTopLayerActive(pSpread);
			BROADCAST_TO_ALL(SpreadMsg(pSpread, SpreadMsg::LAYERCHANGES));
		}
	}
	
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Fills has %d entries\n"), Fills.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Outlines has %d entries\n"), Outlines.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Styles has %d entries\n"), Styles.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Fonts has %d entries\n"), Fonts.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Arrowheads has %d entries\n"), Arrowheads.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Patterns has %d entries\n"), Patterns.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Bitmaps has %d entries\n"), Bitmaps.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Vectors has %d entries\n"), Vectors.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), TextV4 has %d entries\n"), TextV4.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Pages has %d entries\n"), Pages.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), Actions has %d entries\n"), Actions.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), TextBBoxes has %d entries\n"), TextBBoxes.GetCount());
TRACEUSER( "Ben", _T("At CDRFilter::ClearUp(), TextOnPathLinks has %d entries\n"), TextOnPathLinks.GetCount());
TRACEUSER( "Ben", _T("ObjectsConverted = %d\nObjectsFormatWrong = %d\nObjectsUnknown = %d\nEnvelopesApproximated = %d\n"), ObjectsConverted, ObjectsFormatWrong, ObjectsUnknown, EnvelopesApproximated);

	ERROR3IF(Success == TRUE && Actions.GetCount() != 0, "Not all actions used at the end of a successful CDR convert!");
	
	Fills.DeleteAll();
	Outlines.DeleteAll();
	Styles.DeleteAll();
	Fonts.DeleteAll();
	Arrowheads.DeleteAll();
	Patterns.DeleteAll();
	Bitmaps.DeleteAll();
	Vectors.DeleteAll();
	TextV4.DeleteAll();
	Pages.DeleteAll();
	Actions.DeleteAll();
	TextBBoxes.DeleteAll();
	TextOnPathLinks.DeleteAll();

	// avoid indexedcolour in uses
	ColourCMYK CNullCol;
	CNullCol.Cyan = 1;
	CNullCol.Magenta = 1;
	CNullCol.Yellow = 1;
	CNullCol.Key = 1;
	DocColour NullCol = DocColour(COLOURMODEL_CMYK, (ColourGeneric *)&CNullCol);
	SetLineColour(NullCol);
	SetFillColour(NullCol);
	
	// delete the current attributes
	DeleteCurrentAttrs();

	// free a few data blocks
	CCFree(ObjDataBlock);
	CCFree(FontTableV3);
	CCFree(FontStylesV3);
	CCFree(LinkTable);

TRACEUSER( "Ben", _T("ClearUp finished - import of CDR over\n"));
	return Success;
}


/********************************************************************************************

>	static void CDRFilter::DeleteNodeList(Node *Start)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27 03 95
	Inputs:		
	Returns:	
	Purpose:	Deletes a linked list of nodes (forward direction only)
	SeeAlso:	Filter

********************************************************************************************/

void CDRFilter::DeleteNodeList(Node *Start)
{
	Node *This, *Next;

	This = Start;

	while(This != 0)
	{
		Next = This->FindNext();

		This->CascadeDelete();

		delete This;

		This = Next;
	}
}


/********************************************************************************************

>	BOOL CDRFilter::DoExport(Operation*, CCLexFile*, PathName*, Document*);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Refuses to export a Corel Palette file
	SeeAlso:	Filter

********************************************************************************************/

BOOL CDRFilter::DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL)
{
	// No exporting here
	return FALSE;
}


/********************************************************************************************

>	BOOL CDRAttributeStore::AddChunkToStore(RIFFFile *RIFF)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		A RIFFFile object
	Returns:	error flag
	Purpose:	Stores attibute chunks for the CDRFilter class
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRAttributeStore::AddChunkToStore(RIFFFile *RIFF)
{
	if(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK)
	{
		ERROR2(FALSE, "CDRAttributeStore::AddChunkToStore called without a chunk in the RIFFFile");
	}

	// get a new item obect
	CDRAttributeStoredItem *Item = new CDRAttributeStoredItem;

	if(Item == 0)
		return FALSE;

	// get the data of the RIFF chunk
	if(!Item->Aquire(RIFF))
	{
		delete Item;
		return FALSE;
	}

	Item->Size = RIFF->GetObjSize();

	// and add the new item to the list
	AddTail(Item);

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRAttributeStore::Find(DWORD Reference, INT32 *Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		reference to the object (DWORD)
	Returns:	error flag
	Purpose:	Finds a chunk in the store of objects using the reference given
	SeeAlso:	CDRFilter

********************************************************************************************/

ADDR CDRAttributeStore::Find(DWORD Reference, INT32 *Size)
{
	CDRAttributeStoredItem *Item;

	if(IsEmpty())
		return 0;		// no items in the list
	
	Item = (CDRAttributeStoredItem *)GetHead();

	// scan though the list looking for the reference
	while(Item != 0)
	{
		if(CDRDATA_DWORD(*((DWORD *)(Item->Block))) == Reference)
		{
			*Size = Item->Size;
			return Item->Block;
		}

		Item = (CDRAttributeStoredItem *)GetNext(Item);
	}

	return 0;
}


/********************************************************************************************

>	BOOL CDRAttributeStore::Find(WORD Reference, INT32 *Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		reference to the object (WORD)
	Returns:	error flag
	Purpose:	Finds a chunk in the store of objects using the reference given
	SeeAlso:	CDRFilter

********************************************************************************************/

ADDR CDRAttributeStore::Find(WORD Reference, INT32 *Size)
{
	CDRAttributeStoredItem *Item;

	if(IsEmpty())
		return 0;		// no items in the list
	
	Item = (CDRAttributeStoredItem *)GetHead();

	// scan though the list looking for the reference
	while(Item != 0)
	{
		if(CDRDATA_WORD(*((WORD *)(Item->Block))) == Reference)
		{
			*Size = Item->Size;
			return Item->Block;
		}

		Item = (CDRAttributeStoredItem *)GetNext(Item);
	}

	return 0;
}


/********************************************************************************************

>	CDRAttributeStoredItem::CDRAttributeStoredItem(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		
	Returns:	
	Purpose:	Constructor
	SeeAlso:	CDRAttributeStore

********************************************************************************************/

CDRAttributeStoredItem::CDRAttributeStoredItem(void)
{
	Block = 0;
}


/********************************************************************************************

>	CDRAttributeStoredItem::~CDRAttributeStoredItem(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		
	Returns:	
	Purpose:	Destructor
	SeeAlso:	CDRAttributeStore

********************************************************************************************/

CDRAttributeStoredItem::~CDRAttributeStoredItem(void)
{
	CCFree(Block);
}


/********************************************************************************************

>	BOOL CDRAttributeStoredItem::~CDRAttributeStoredItem(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		
	Returns:	
	Purpose:	Destructor
	SeeAlso:	CDRAttributeStore

********************************************************************************************/

BOOL CDRAttributeStoredItem::Aquire(RIFFFile *RIFF)
{
	ERROR3IF(Block != 0, "CDRAttributeStoredItem already has data");

	Size = RIFF->GetObjSize();
	
	// get a block of memory
	if((Block = (ADDR)CCMalloc(Size)) == 0)
		return FALSE;

	// get the data of the object into it
	if(!RIFF->GetChunkData(Block, Size))
		return FALSE;

	// bye
	return TRUE;
}


/********************************************************************************************
             
>	CDRLayerListItem::CDRLayerListItem()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		
	Returns:	
	Purpose:	Constructor
	SeeAlso:	CDRLayerList

********************************************************************************************/

CDRLayerListItem::CDRLayerListItem()
{
	Objects = 0;
	LayerInDocument = FALSE;
	IsVisible = TRUE;
	IsLocked = FALSE;
}


/********************************************************************************************
             
>	CDRLayerListItem::~CDRLayerListItem()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		
	Returns:	
	Purpose:	Destructor
	SeeAlso:	CDRLayerList

********************************************************************************************/

CDRLayerListItem::~CDRLayerListItem()
{
	Node *Previous;
	Node *This;

	This = Objects;

	if(LayerInDocument == FALSE)
	{	
		// delete the tree if it hasn't been used in the document
		while(This != 0)
		{
			// check that the node we're about to delete hasn't got a next pointer
			// as it jolly well shouldn't have one as we create the tree backwards
			ERROR3IF(This->FindNext() != 0, "Node in layer has next pointer during deletion");
			
			// find the previous node
			Previous = This->FindPrevious();
			
			// delete the current one's children
			This->CascadeDelete();

			// and delete the current one
			delete This;

			// shuffle backwards
			This = Previous;
		}
		Objects = 0;
	}
}


/********************************************************************************************
             
>	CDRLayerList::CDRLayerList()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		
	Returns:	
	Purpose:	Constructor
	SeeAlso:	CDRLayerList

********************************************************************************************/

CDRLayerList::CDRLayerList()
{
}


/********************************************************************************************
             
>	CDRLayerList::~CDRLayerList()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		
	Returns:	
	Purpose:	Destructor so we can delete all the pages with one DeleteAll call
	SeeAlso:	CDRLayerList

********************************************************************************************/

CDRLayerList::~CDRLayerList()
{
	DeleteAll();
}


/********************************************************************************************
             
>	CDRPageList::CDRPageList()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		
	Returns:	
	Purpose:	Constructor
	SeeAlso:	CDRLayerList

********************************************************************************************/

CDRPageList::CDRPageList()
{
}


/********************************************************************************************
             
>	CDRPageList::~CDRPageList()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		
	Returns:	
	Purpose:	Destructor
	SeeAlso:	CDRLayerList

********************************************************************************************/

CDRPageList::~CDRPageList()
{
}


/********************************************************************************************
             
>	CDRPageListItem::CDRPageListItem()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		
	Returns:	
	Purpose:	Constructor
	SeeAlso:	CDRLayerList

********************************************************************************************/

CDRPageListItem::CDRPageListItem()
{
}


/********************************************************************************************
             
>	CDRPageListItem::~CDRPageListItem()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		
	Returns:	
	Purpose:	Destructor
	SeeAlso:	CDRLayerList

********************************************************************************************/

CDRPageListItem::~CDRPageListItem()
{
}





/********************************************************************************************
             
>	static BOOL CDRFilter::HasCDRFileGotCMXFile(PathName *FileName, CCDiskFile *pFile = NULL)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Inputs:		pathname of file to check, or pointer to disk file
	Returns:	whether it has got a cmx file embedded in it
	Purpose:	to find out whether a CDR file has got a CMX file in it, and therefore
				whether the CMX filter should grab it instead of this one

********************************************************************************************/

BOOL CDRFilter::HasCDRFileGotCMXFile(PathName *FileName, CCLexFile *pFile)
{
	CCDiskFile oTheFile;
	CCLexFile *pTheFile;
	INT32 pos;

	if(pFile == NULL)
	{
		if (!(oTheFile.open(*FileName, ios::in | ios::binary)))
		{
			TRACEUSER( "Ben", _T("couldn't open file to check it for CMX\n"));
			return FALSE;
		}
		pTheFile = &oTheFile;
	}
	else
	{
		pTheFile = pFile;
		pos = pFile->tellIn();
	}

	// read in the file header, and check it's a riff file
	RIFFFile_Header Hdr;
	pTheFile->read(&Hdr, sizeof(Hdr));

	if(Hdr.CK.ckID != RIFFTYPE_RIFF)
		return FALSE;			// file is not a riff form

	UINT32 loc = sizeof(Hdr);

	// run through the thingys
	BOOL Found = FALSE;
	while(loc < Hdr.CK.ckSize)
	{
		RIFFck ck;
		pTheFile->read(&ck, sizeof(ck));
		loc += sizeof(ck);
		if(ck.ckID == cdrT_cmx1)
		{
			// found the cmx section
			Found = TRUE;
			break;	// we can stop now
		}

		// skip to start of next chunk
		loc += ck.ckSize;
		pTheFile->seekIn(loc);
	}

	// close it or clear up...
	if(pFile == NULL)
	{
		oTheFile.close();
	}
	else
	{
		pFile->seekIn(pos);
	}
	
	return Found;
}


