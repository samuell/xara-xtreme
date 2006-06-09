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
// CMX import Filter


#include "camtypes.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "filtrres.h"
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepath.h"
#include "riffform.h"
#include "progress.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cdrfiltr.h"
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "combshps.h"
#include "nodetxts.h"
#include "attrmap.h"

#include "cmxform.h"
#include "cmxifltr.h"
#include "cmxistut.h"
#include "cmxibits.h"
//#include "cmxres.h"
#include "cmxidata.h"
#include "cmxibitm.h"

CC_IMPLEMENT_DYNAMIC(CMXImportFilter, VectorFilter)
CC_IMPLEMENT_DYNAMIC(CMXImportFilterDataSet, CCObject)

#define new CAM_DEBUG_NEW

#ifdef _DEBUG
void CMXImportFilter::FormatErrorBreakingFunction(void)
{
	TRACEUSER( "Ben", _T(">>> format error in cmx file\n"));
}
#endif

/********************************************************************************************

>	CMXImportFilter::CMXImportFilter()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/08/96
	Inputs:		None
	Returns:	None
	Purpose:	Constructor for class CMXImportFilter
	SeeAlso:	class VectorFilter

********************************************************************************************/

CMXImportFilter::CMXImportFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_CMXIMPORTFILTER_FILTERNAME));
	FilterInfo.Load(_R(IDT_CMXIMPORTFILTER_FILTERINFO));
	FilterID = FILTERID_CMXIMPORT;

	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;

	ImportMsgID = _R(IDT_IMPORTMSG_CMX);

	ExportMsgID = _R(IDT_EXPORTMSG_CMX);

	// set up the info
	CMXIinfo::SetUpInfo();

	// mark the data set as not being used
	Data = NULL;

	//	By default we're not dealing with a file from CorelDraw 7
	CorelDraw7	=	FALSE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Init()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/08/96
	Inputs:		None
	Returns:	BOOL, propably saying whether we succeeded or not, buts it not documented
	Purpose:	Initialiser for class CMXImportFilter
	SeeAlso:	Filter

********************************************************************************************/

BOOL CMXImportFilter::Init()
{
 	// Get the OILFilter object
	pOILFilter = new CMXImportOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	
	return TRUE;
}

/********************************************************************************************

>	INT32 CMXImportFilter::HowCOmpatible(PathName& Filename,
							ADDR HeaderStart,
							UINT32 HeaderSize,
							UINT32 FileSize);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/08/96
	Inputs:		Filename - filename of file to check
				HeaderStart, HeaderSize, - infomation on header of file loaded into memory
				FileSize - length of file (bytes)
	Returns:	0 to 10. 0 means I know nothing about this file, 10 means its definitly mine
	Purpose:	Looks at the start of the file to see if we recognise it as one of ours
	SeeAlso:	class Filters, class CDRFilter

********************************************************************************************/

INT32 CMXImportFilter::HowCompatible(PathName & FileName, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
	// see if we're a nice CMX file...
PORTNOTE("byteorder", "TODO: Check byte ordering")
	RIFFFile_Header *pHdr = (RIFFFile_Header *)HeaderStart;

	if(pHdr->CK.ckID == RIFFTYPE_RIFF)
	{
		switch(pHdr->FormType)
		{
		case cmxRIFFFORMTYPE_CMX:
			// yep, it's one of ours
			return 10;
			break;

		case cmxRIFFFORMTYPE_CDR5:
		case cmxRIFFFORMTYPE_CDT5:
		case cmxRIFFFORMTYPE_CDR6:
		case cmxRIFFFORMTYPE_CDT6:
			// Tootle through the file if it's a CDR file to see if we
			// can't load that nice CMX file it might have inside it.
			if(CDRFilter::HasCDRFileGotCMXFile(&FileName))
			{
				return 10;			// we'll have that too, then
			}
			break;

		default:
			break;
		}
	}

	return 0;
}

/********************************************************************************************

>	BOOL CMXImportFilter::DoImport(SelOperation * Op, CCLexFile * pDiskFile,
							Document * DestDoc, BOOL AutoChoosen , ImportPosition * Pos,
							KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/08/96
	Inputs:		Op - pointer to the operatioj that this input process is associated with,
				pDiskFile - the CMXImport file to be loaded
				DestDoc - the document object whcih should hold the data read in from this file
				URL - original URL of the imported file
	Returns:	TRUE is the input operation worked, FALSE if not
	Purpose:	Read an CMXImport format file and convert it into Camelot tree data structures. When
				the file is loaded we force a redraw for the area affected by the new data
				At the moment the data is just put on the first layer of the first spread
				of the first chapter in the document, and this is a bodge.
				The operation is terminated (i.e. its End() function is called) if the
				operation completed successfully.
	Errors:		Fails (returns FALSE) if the document structure is incorrect, if the CMXImport file
				contains a syntax error, of the CMXImport file cannot be found/opened or if its
				not actually an CMXImport file.
	SeeAlso:	CMXImportFilter::HowCompatible

********************************************************************************************/

BOOL CMXImportFilter::DoImport(SelOperation * Op, CCLexFile * pDiskFile,
							Document * DestDoc, BOOL AutoChoosen , ImportPosition * Pos,
							KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)
{
	// set variables
	pFile = pDiskFile;
	Is32Bit = FALSE;			// for now... Stage1_ReadHeader sets it properly
	StartPosition = 0;
	TheDocument = DestDoc;

	// create a dataset
	ERROR3IF(Data != NULL, "CMX data set already created");
	Data = new CMXImportFilterDataSet(this);
	if(Data == NULL)
		return FALSE;

	// set up various filter type things
	Data->pProgress = new Progress(_R(IDT_CMX_IMPORTMSG), 1, FALSE);
	if(Data->pProgress == NULL)
		return FALSE;
	if(!SetUpCurrentAttrs())
		return FALSE;

	//	We haven't read the text frame yet
	Data->TextFrameRead = FALSE;
	
	SetTextAttributeIgnore();

	// get the spread we're using
	Document *pTheDocument = GetDocument();
	ERROR2IF(pTheDocument == NULL, FALSE, "no document for filter");
	Spread *pSpread = NULL; //pTheDocument->GetSelectedSpread();

	if (Pos == NULL)
	{		
		// For now, position objects on 1st page of spread 1
		pSpread = GetFirstSpread(DestDoc);
		Page *pPage = (Page *) pSpread->FindFirstPageInSpread();
		ENSURE(pPage->IsKindOf(CC_RUNTIME_CLASS(Page)),
			   "MetaFileFilter::DoImport(): Could not find first Page");
	}
	else
	{
		pSpread = Pos->pSpread;
	}


	// do various bits and pieces to convert that silly file
	BOOL EverythingWentSwimmingly = TRUE;
	TRY
	{
		if(		!Stage1_ReadHeader(pSpread)
			||	!Stage2_ReadMasterIndex()
			||	!Stage3_ConvertDescriptionSections()
			||	!Stage4_ConvertPage()
			||	!Stage5_PlaceObjectsInTree(pSpread, Op, Pos) )
			EverythingWentSwimmingly = FALSE;
	}
	CATCH(CFileException, e)
	{
		// something went wrong -- clean up
		CleanUpAfterImport();

		return FALSE;
	}
	END_CATCH

	CleanUpAfterImport();

	return EverythingWentSwimmingly;
}


/********************************************************************************************

>	void CMXImportFilter::CleanUpAfterImport(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		None
	Returns:	nothin' mate
	Purpose:	Cleans everything up after a sucessful or non-sucessful import

********************************************************************************************/

void CMXImportFilter::CleanUpAfterImport(void)
{
	// delete the data set -- that class will take care of itself and delete everything
	// necessary to ensure we don't leak anything
	if(Data != NULL)
		delete Data;

	Data = NULL;

	// vape attributes
	DeleteCurrentAttrs();
}


/********************************************************************************************

>	BOOL CMXImportFilter::LocateRootLevelRIFFSection(DWORD Section)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		None
	Returns:	TRUE if the section was fount
	Purpose:	Sets the file pointer to the start of the data in the specified
				RIFF section. Doesn't go into lists.

********************************************************************************************/

BOOL CMXImportFilter::LocateRootLevelRIFFSection(DWORD Section)
{
	// go to the beginning of the file
	Seek(0);

	// read in the file header, and check it's a riff file
	RIFFFile_Header Hdr;
	pFile->read(&Hdr, sizeof(Hdr));

	if(Hdr.CK.ckID != RIFFTYPE_RIFF)
		return FALSE;			// file is not a riff form

	UINT32 loc = sizeof(Hdr);

	// run through the thingys
	while(loc < Hdr.CK.ckSize)
	{
		RIFFck ck;
		pFile->read(&ck, sizeof(ck));
		loc += sizeof(ck);
		if(ck.ckID == Section)
			return TRUE;			// found it!

		// skip to start of next chunk
		loc += ck.ckSize;
		Seek(loc);
	}

	return FALSE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Stage1_ReadHeader(Spread *pSpread)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	Reads the header from the file, and sets various internal variables to
				reflect what it found there. It creates the base matrix for transforming
				stuff.

********************************************************************************************/

BOOL CMXImportFilter::Stage1_ReadHeader(Spread *pSpread)
{
	// find the header of the file
	if(!LocateRootLevelRIFFSection(cmxRIFFCI_Header))
	{
		// OK, didn't find one of those... see if we can get a CMX one instead
		if(LocateRootLevelRIFFSection(cmxRIFFFORMTYPE_cmx1))
		{
			// fine... alter the thingy to point to the beginning of this section
			StartPosition = pFile->tellIn();
			// and locate the header section
			if(!LocateRootLevelRIFFSection(cmxRIFFCI_Header))
			{
				CMXFORMATERROR(FALSE)
			}
		}
		else
		{
			CMXFORMATERROR(FALSE)
		}
	}

	// read it in
	CI_READDATA(FileHeader, Hdr)

	// check that the id is correct
	if(strcmp(Hdr.ID, cmxID) != 0)
		CMXFORMATERROR(FALSE)		// wasn't a CMX file

	// read the handy bits from it
	switch(Hdr.CoordSize & 0xf)
	{
	case cmxCOORDSIZE_32:
		TRACEUSER( "Ben", _T("\n\n********************** 32 bit CMX file\n"));
		Is32Bit = TRUE;
		break;

	case cmxCOORDSIZE_16:
		TRACEUSER( "Ben", _T("\n\n********************** 16 bit CMX file\n"));
		Is32Bit = FALSE;
		break;

	default:
		// don't understand this data size
		CMXFORMATERROR(FALSE);
		break;
	}

	// set the location of the index
	Data->IndexFilePosition = Hdr.IndexSection;

	// set up the base matrix for transformation of the objects on the page
	Page *pPage = (Page *)pSpread->FindFirstPageInSpread();

	// Use bottom left of page as origin as in CMX in (0,0) is the centre of the page
	DocRect PageRect = pPage->GetPageRect();
	DocCoord Origin = DocCoord(PageRect.lo.x + (PageRect.Width() / 2),
			PageRect.lo.y + (PageRect.Height() / 2));

	// set a matrix which transforms things appropriately
	double Scale;
	if(Is32Bit)
	{
		Scale = 1 / CAMCOORD_SCALEFACTOR32;
	}
	else
	{
		Scale = 72;
	}
	Matrix TheMat = Matrix((FIXED16)Scale, (FIXED16)Scale);

	// transform the coordinates
	TheMat *= Matrix(Origin);
	
	// set the matrix
	Data->BaseMatrix = TheMat;
	Data->BaseScaleFactor = Scale;

	// and set the current matrix pointer to it
	Data->pCurrentMatrix = &Data->BaseMatrix;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Stage5_PlaceObjectsInTree(Spread *pSpread, SelOperation *pOp, ImportPosition *pPos)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	sticks the objects in the document tree

********************************************************************************************/

BOOL CMXImportFilter::Stage5_PlaceObjectsInTree(Spread *pSpread, SelOperation *pOp, ImportPosition *pPos)
{
	Trans2DMatrix Xlate;
	BOOL Translate = FALSE;
	Coord Offset;

	// do we need to do a drag and drop translation?
	if(GetDragAndDropTranslation(pPos, Data->AllObjectsBoundsRect, &Offset))
	{
		Translate = TRUE;
		Xlate = Trans2DMatrix(Offset.x, Offset.y);
	}

	BOOL ImportingWithLayers = FALSE;

	// do we want to import with layers?
#ifdef WEBSTER
	// In Webster, just use the default preference settings as these should be set to
	// ImportWithLayers to False.
	if (
		(!TheDocument->IsImporting() && Filter::OpenWithLayers) ||
		(TheDocument->IsImporting() && Filter::ImportWithLayers)
		)
		ImportingWithLayers = TRUE;
#else
	// In Camelot, we must check to see if the document is being imported and if so if
	// there are any frames present. If there are then don't use layers, if there are
	// then use the preference setting.
	if (TheDocument->IsImporting())
	{
		Spread * pTheSpread = TheDocument->GetSelectedSpread();
		Layer * pFrame = NULL;
		if (pTheSpread != NULL)
			pFrame = pTheSpread->FindFirstFrameLayer();
		if (pFrame != NULL)
			ImportingWithLayers = FALSE;					// Frames present so do not import layers
		else
			ImportingWithLayers = Filter::ImportWithLayers;	// No frames so use the preference setting
	}
	else
	{
		ImportingWithLayers = Filter::OpenWithLayers;
	}
#endif

	NodeGroup *pGroup = NULL;
	if(!ImportingWithLayers)
	{
		if((pGroup = new NodeGroup) == NULL)
			return FALSE;
	}

	// go through the layer list adding stuff
	CMXImportLayer *pSarah = (CMXImportLayer *)Data->Layers.GetHead();
	while(pSarah != NULL)
	{
		ERROR2IF(!IS_A(pSarah, CMXImportLayer), FALSE, "thingy not a CMX import layer");

		if(ImportingWithLayers)
		{
			// get the layer, and mark the import layer as used
			Layer *pNewLayer = pSarah->UseSubTreeAsLayer();
			if(pNewLayer == NULL)
				return FALSE;

			// and maybe translate it a bit
			if(Translate)
				pNewLayer->Transform(Xlate);

			// insert the layer into the document
			if(!pOp->DoInsertNewNode(pNewLayer, (Node *)pSpread, LASTCHILD, TRUE))
			{
				// It didn't work - delete the sub-tree we just created
				pNewLayer->CascadeDelete();
				delete pNewLayer;
				return FALSE;
			}

			// make sure it's name is unique
			pNewLayer->EnsureUniqueLayerID();					
			
			// ... and finally optimise it's attributes
			if(!pNewLayer->OptimiseAttributes())
				return FALSE;
		}
		else
		{
			// do stuff without layers
			ERROR3IF(pGroup == NULL, "no group node created");

			// get the subtree of the layer, and add it
			if(pSarah->GetSubTree() != NULL)	// some may be empty
			{
				Node *pSubTree = pSarah->GetSubTree();
				// add it to the group node
				pSubTree->InsertChainSimple(pGroup, LASTCHILD);
			}

			// mark the layer as being used
			pSarah->UseSubTree();
		}

		pSarah = (CMXImportLayer *)Data->Layers.GetNext(pSarah);
	}

	if(ImportingWithLayers)
	{
		// get the insertation node set up
		TheDocument->ResetInsertionPosition();
	}
	else
	{
		// Make sure that there is a layer to put the bitmap onto
		if (!MakeSureLayerExists(TheDocument))
			// There is no layer and one could not be made, so we will have to fail
			return FALSE;

	//	TheDocument->ResetInsertionPosition();
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

		// bit of translation for sir?
		if(Translate)
			InsertNode->Transform(Xlate);

		// if there isn't a group, detach it
		if(InsertNode != pGroup)
			InsertNode->UnlinkNodeFromTree(NULL);

		// invalidate the insert node's bounds
		InsertNode->InvalidateBoundingRect();

		// put the group into the document
		if (!pOp->DoInsertNewNode(InsertNode, pSpread, TRUE))
		{
			// It didn't work - delete the sub-tree we just created
			pGroup->CascadeDelete();
			delete pGroup;
			return FALSE;
		}

		// optimise the groups attributes
		if(!InsertNode->OptimiseAttributes())
			return FALSE;

		// delete the group if it wasn't used
		if(InsertNode != pGroup)
			delete pGroup;
	}

	// add colours
	if(!AddColoursToDocument())
		return FALSE;

	// do a quick post import on it
	TheDocument->PostImport();
	
	return TRUE;
}


/********************************************************************************************

>	CMXImportFilterDataSet::CMXImportFilterDataSet(CMXImportFilter *pFilter)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		none
	Returns:	none
	Purpose:	constructor for the CMX import filter's data set

********************************************************************************************/

CMXImportFilterDataSet::CMXImportFilterDataSet(CMXImportFilter *pFilter)
	: TransformStack(cmxiSTACKTYPE_TRANSFORM, pFilter)
	, ClipperStack(cmxiSTACKTYPE_CLIPPER, pFilter)
{
	// mark all index entry positions as not present
	for(INT32 l = 0; l < cmxMASTERIN_MAXENTRY; l++)
		MasterIndexEntries[l] = 0;

	// set things to null
	pProgress = NULL;
	CommandsInPage = -1;
	pCurrentLayer = NULL;
	pCurrentMatrix = NULL;
	pTextStory = NULL;
	pTextLine = NULL;
	NumColours = 0;
	Colours = NULL;
	NumOutlines = 0;
	Outlines = NULL;
	NumPens = 0;
	Pens = NULL;
	BaseScaleFactor = 0;
	NumLineStyles = 0;
	LineStyles = NULL;
	NumRImages = 0;
	RImages = NULL;
	NumProcedures = 0;
	Procedures = NULL;
	NumDotDashes = 0;
	DotDashes = NULL;
	pClippingPath = NULL;
	NumArrowShapes = 0;
	ArrowShapes = NULL;
	NumArrowheads = 0;
	Arrowheads = NULL;
	NumBitmaps = 0;
	Bitmaps = NULL;
	NumFonts	=	0;
	Fonts	=	NULL;
	//	No way to know how many CharInfo are going to be there before actually read them
	//	Use a number big enough. NumCharInfo will be the actual number.
	NumCharInfo	=	0;
	CharInfos = new cmxiCharInfo[100];
	for(INT32 i = 0 ; i < 100 ; i++)
		CharInfos[i].UsageMask	=	0;



	// flags
	InPage = FALSE;
	RemoveLens = FALSE;
	EndSectionFound = FALSE;
	HaveFirstObjInBoundsRect = FALSE;

	// counts of approxed objects
	AttributesApproximated = 0;
	ObjectsApproximated = 0;
	ClippingsApproximated = 0;
}


/********************************************************************************************

>	CMXImportFilterDataSet::~CMXImportFilterDataSet()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		none
	Returns:	none
	Purpose:	destructor for the CMX import filter's data set -- it will delete all
				objects which aren't needed after import.

********************************************************************************************/

CMXImportFilterDataSet::~CMXImportFilterDataSet()
{
	// delete list entries -- they will delete their contents if they haven't been used
	Layers.DeleteAll();
	
	// close down the progress system
	if(pProgress != NULL)
	{
		delete pProgress;
		pProgress = NULL;
	}

	// delete lists of stuff
	if(Colours != 0)
		delete [] Colours;
	if(Outlines != 0)
		delete [] Outlines;
	if(Pens != 0)
		delete [] Pens;
	if(LineStyles != 0)
		delete [] LineStyles;
	if(RImages != 0)
		delete [] RImages;
	if(Procedures != 0)
		delete [] Procedures;
	if(DotDashes != 0)
		delete [] DotDashes;
	if(ArrowShapes != 0)
		delete [] ArrowShapes;
	if(Arrowheads != 0)
		delete [] Arrowheads;
	if(Bitmaps != 0)
		delete [] Bitmaps;
	if(Fonts != 0)
		delete [] Fonts;
	if(CharInfos != 0)
		delete [] CharInfos;
}


// access functions to avoid adding cmxidata.h to cmxifltr.h
void CMXImportFilter::AttributeApproximated(void) {Data->AttributesApproximated++;}
void CMXImportFilter::ClippingApproximated(void) {Data->ClippingsApproximated++;}
void CMXImportFilter::ObjectApproximated(void) {Data->ObjectsApproximated++;}
Matrix *CMXImportFilter::GetBaseMatrix(void) {return &Data->BaseMatrix;};
Matrix *CMXImportFilter::GetCurrentMatrix(void) {return Data->pCurrentMatrix;};
void CMXImportFilter::SetMatrix(Matrix *pNewMatrix) {Data->pCurrentMatrix = pNewMatrix;};
void CMXImportFilter::SetClippingPath(Path *pClipper) {Data->pClippingPath = pClipper;};


/********************************************************************************************

>	BOOL CMXImportFiler::TransformNode(Node *pNode)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pointer to node to fiddle with
	Returns:	error flag
	Purpose:	transforms the node to the correct place on the page. Only give it
				nodes which are kinds of NodeRenderableBounded.

********************************************************************************************/

BOOL CMXImportFilter::TransformNode(Node *pNode)
{
	ERROR2IF(Data->pCurrentMatrix == NULL, FALSE, "No matrix to transform by");

	// transform the node
 	Trans2DMatrix Transform(*Data->pCurrentMatrix);
	NodeRenderableBounded *pBN = (NodeRenderableBounded *)pNode;
	ERROR3IF(!pBN->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)), "not a transformable node passed to TransformNode");
	pBN->Transform(Transform);

	// do some funky stuff on it
	DocRect BBox = pBN->GetBoundingRect(TRUE, FALSE);
	if(Data->HaveFirstObjInBoundsRect == FALSE)
	{
		// set the bbox
		Data->HaveFirstObjInBoundsRect = TRUE;
		Data->AllObjectsBoundsRect = BBox;
	}
	else
	{
		Data->AllObjectsBoundsRect = Data->AllObjectsBoundsRect.Union(BBox);
	}


	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFiler::SetAttributesForNode(Node *pNode, cmxiRenderAttr *pAttr)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pointer to node to fiddle with, pointer to attributes from CMX file
	Returns:	error flag
	Purpose:	sets the attributes for the node. The bits and pieces from the
				render attributes thingy are deleted, so you can't use them twice

********************************************************************************************/

#define MAXLINEWIDTH	(720000 * 2)		// sounds reasonable

BOOL CMXImportFilter::SetAttributesForNode(Node *pNode, void *pvAttr, BOOL DeleteStuff)
{
	cmxiRenderAttr *pAttr = (cmxiRenderAttr *)pvAttr;

	// remove lens attributes?
	if(Data->RemoveLens)
	{
		Data->RemoveLens = FALSE;
		if(!SetNoTranspFill())
			return FALSE;
	}

	BOOL DoFillStuff = TRUE;

	// -----------------------------------------  fills
	if((pAttr->Mask & cmxRENDATTRMASK_LENS) != 0)
	{
		// can we handle the lens?
		if(pAttr->LensSpec.Type == cmxLENSTYPE_GLASS)
		{
			UINT32 Type;

			// yep, it's one of those glass ones
			switch(pAttr->LensSpec.Glass.TintMethod)
			{
			case cmxLENSTINTMETH_SUBTRACT:
				Type = TT_StainGlass;
				break;

			case cmxLENSTINTMETH_ADD:
				Type = TT_Bleach;
				break;

			case cmxLENSTINTMETH_AVERAGE:
				Type = TT_Mix;
				break;

			default:
				Type = TT_Mix;
				AttributeApproximated();
				break;
			}

			// munge to our range
			UINT32 Percent = 255 - ((pAttr->LensSpec.Glass.UniformRate * 255) / 1000);

			// set the attribute
			if(!SetFlatTranspFill(Type, Percent))
				return FALSE;

			// set the colour of the object
			DocColour *pColour = GetColourFromReference(pAttr->LensSpec.Glass.ColourRef);
			if(pColour == NULL)
				return FALSE;
			// set the attribute
			if(!SetFillColour(*pColour))
				return FALSE;

			Data->RemoveLens = TRUE;	// something to remove next time around
			DoFillStuff = FALSE;		// we've handled all fill type stuff
		}
		else
		{
			AttributeApproximated();	// mark the fact we've ignored something
		}
	}

	// -----------------------------------------  fills
	if(DoFillStuff)
	{
		if((pAttr->Mask & cmxRENDATTRMASK_FILL) != 0)
		{
			// got a fill -- set filledness
			Data->ObjFilled = TRUE;
			SetPathFilled(TRUE);

			// set attributes for it
			BOOL UnknownFill = FALSE;

			TRACEUSER( "Ben", _T("   fill type is %d\n"), pAttr->FillSpec.FillID);
			// here we convert the stuff, and delete anything which is created
			// for us
			switch(pAttr->FillSpec.FillID)
			{
			case cmxFILLID_UNIFORM:
				{
					// get a colour for the fill
					DocColour *pColour = GetColourFromReference(pAttr->FillSpec.Spec.Uniform.ColourReference);
					if(pColour == NULL)
						return FALSE;
					// set the attribute
					if(!SetFillColour(*pColour))
						return FALSE;
				}
				break;

			case cmxFILLID_FOUNTAIN:
				if(!SetAttributesFountain(pNode, &pAttr->FillSpec))
				{
					if(DeleteStuff) delete pAttr->FillSpec.Spec.Fountain.pColours;
					return FALSE;
				}

				if(DeleteStuff) delete pAttr->FillSpec.Spec.Fountain.pColours;
				break;

			case cmxFILLID_PS:
				if(DeleteStuff) delete pAttr->FillSpec.Spec.Postscript.UserFunction;
				break;

			case cmxFILLID_COLPATTERN:
				if(!SetAttributesTwoColourBitmapFill(pNode, &pAttr->FillSpec))
					return FALSE;
				break;

			case cmxFILLID_BITMAPFILL:
			case cmxFILLID_BITMAPFILL16:	// also vector fills in v6
				// we've got one of those possible bitmap fill type things
				if(!SetAttributesBitmapFill(pNode, pAttr->FillSpec.Spec.Bitmap.VectorFillReference, &pAttr->FillSpec.Spec.Bitmap.Tile))
					return FALSE;
				break;

			case cmxFILLID_TEXTURE:
				if(!SetAttributesBitmapFill(pNode, pAttr->FillSpec.Spec.Texture.VectorFillReference, &pAttr->FillSpec.Spec.Texture.Tile))
					return FALSE;
				break;

			default:
				// dunno nothing about this kind of fill ID -- set to no colour
				UnknownFill = TRUE;
				break;
			}

			if(UnknownFill)
			{
				// give a fill no colour if we don't know how to do it
				AttributeApproximated();
				if(!SetNoFill(pNode))
					return FALSE;
			}
		}
		else
		{
			Data->ObjFilled = FALSE;

			// no fill here
			if(!SetNoFill(pNode))
				return FALSE;
		}
	}

	// -----------------------------------------  outline
	if((pAttr->Mask & cmxRENDATTRMASK_OUTLINE) != 0)
	{
		// right then, get the outline defn
		CMXImportOutline *pOut;
		if(pAttr->OutlineReference <= 0 || pAttr->OutlineReference > Data->NumOutlines)
			CMXFORMATERROR(FALSE)		// outline ref out of range

		pOut = &Data->Outlines[pAttr->OutlineReference - 1];

		if(pOut->NoStroke)
		{
			if(!SetNoStroke(pNode))
				return FALSE;
		}
		else
		{
			// have an outline to fiddle with...

			// get the colour
			DocColour *pCol = GetColourFromReference(pOut->ColourReference);
			if(pCol == NULL)
				return FALSE;

			// set it...
			if(!SetLineColour(*pCol))
				return FALSE;

			// do the line width
			double LineWidth = pOut->Width * Data->BaseScaleFactor;
			if(LineWidth > MAXLINEWIDTH) LineWidth = MAXLINEWIDTH;
			if(!SetLineWidth((MILLIPOINT)LineWidth))
				return FALSE;

			// do that funky dot dash pattern thingy
			DashRec Dash;
			if(pOut->pDots != NULL && pOut->pDots->NDots != 0)
			{
				// set a dash pattern
				Dash.Elements = pOut->pDots->NDots;
				Dash.DashStart = 0;
				Dash.ElementData = pOut->pDots->pElements;
				Dash.LineWidth = 1; //(MILLIPOINT)LineWidth;
				Dash.ScaleWithLineWidth = TRUE;
				Dash.DashID = -1;
			}
			else
			{
				// no dash pattern
				Dash = SD_SOLID;
			}

			// do the cap and join bits
			if(!SetLineCap(pOut->Cap)
				|| !SetJoinType(pOut->Join)
				|| !SetDashPattern(Dash))
				return FALSE;

			// do arrowheads
			if(pOut->pArrowheads != NULL)
			{
				// got some arrowheads...
				// -- <> -- START
				if(pOut->pArrowheads->pStart != NULL)
				{
					if(!SetStartArrow(pOut->pArrowheads->pStart->Arrow))
						return FALSE;
				}
				else
				{
					ArrowRec NullArrow;
					if(!SetStartArrow(NullArrow))
						return FALSE;
				}

				// -- <> -- END
				if(pOut->pArrowheads->pEnd != NULL)
				{
					if(!SetEndArrow(pOut->pArrowheads->pEnd->Arrow))
						return FALSE;
				}
				else
				{
					ArrowRec NullArrow;
					if(!SetEndArrow(NullArrow))
						return FALSE;
				}
			}
			else
			{
				// no arrowheads -- set a null arrow rec
				ArrowRec NullArrow;
				if(!SetStartArrow(NullArrow)
					|| !SetEndArrow(NullArrow))
					return FALSE;
			}
		}
	}
	else
	{
		if(!SetNoStroke(pNode))
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::SetNoFill(Node *pNode)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		node
	Returns:	error flag
	Purpose:	sets attributes for this node to have no fill -- also fiddles it if it's
				a path

********************************************************************************************/

BOOL CMXImportFilter::SetNoFill(Node *pNode)
{
	if(pNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
	{
		NodePath *pPath = (NodePath *)pNode;

//		pPath->InkPath.IsFilled = FALSE;
	}

	Data->ObjFilled = FALSE;

	SetFillColour(DocColour(COLOUR_TRANS));

	return SetPathFilled(FALSE);
}


/********************************************************************************************

>	BOOL CMXImportFilter::SetNoStroke(Node *pNode)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		node
	Returns:	error flag
	Purpose:	sets attributes for this node to have no stroke colour -- also fiddles
				it if it's a path

********************************************************************************************/

BOOL CMXImportFilter::SetNoStroke(Node *pNode)
{

// Jim 13/9/96 - Setting Transparent stroke colour is enough - don't set IsStroked to FALSE
// otherwise it stops them rendering at zero quality.
//	if(pNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
//	{
//		NodePath *pPath = (NodePath *)pNode;
//
//		pPath->InkPath.IsStroked = FALSE;
//	}

	SetLineColour(DocColour(COLOUR_TRANS));

	return TRUE;
}


class CMXBecomeA : public BecomeA
{
public:
	CMXBecomeA(BecomeAReason ThisReason, CCRuntimeClass* pClass, UndoableOperation* pOp, BOOL sel=TRUE)
		: BecomeA(ThisReason, pClass, pOp, sel) {};

	BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap=NULL);

	NodePath *pPath;
};

/********************************************************************************************

>	BOOL CMXBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap=NULL)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/08/96
	Inputs:		random stuff
	Returns:	error flag
	Purpose:	stores the pointer to the copy of my thingy, and bungs the attributes
				on it.

********************************************************************************************/

BOOL CMXBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)
{
	pPath = (NodePath *)pNewNode;

	if(pAttrMap != NULL)
	{
		// iterating all (key, value) pairs
		for (POSITION Pos = pAttrMap->GetStartPosition(); Pos != NULL;)
		{
			void *pType,*pVal;
			pAttrMap->GetNextAssoc(Pos,pType,pVal);

			// Get the attribute value of this attribute
			NodeAttribute* pAttr = (NodeAttribute*)pVal;

			// bung the attribute onto the node thingy
			pAttr->AttachNode(pNewNode, LASTCHILD);
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFiler::ClipNode(Node **pNode)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pointer to pointer to node to clip
	Returns:	error flag
	Purpose:	clips the node to the current clipping path. Note that it may delete this
				one and change your pointer to a completely new node, just for the fun
				of it.

********************************************************************************************/

BOOL CMXImportFilter::ClipNode(Node **pNode)
{
	if(Data->pClippingPath == NULL)
		return TRUE;		// nothin' to do

	NodePath *pPath = NULL;

	// check to see what kind of node we've got
	if(!IS_A(*pNode, NodePath))
	{
		CMXBecomeA ParamBecomeA(BECOMEA_PASSBACK, 
							 CC_RUNTIME_CLASS(NodePath),
							 NULL, FALSE);

		if((*pNode)->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
		{
			if ((*pNode)->CanBecomeA(&ParamBecomeA))
			{
				// is it a text story?
				if((*pNode)->IsKindOf(CC_RUNTIME_CLASS(TextStory)))
				{
					// give formatting it a go
					((TextStory *)*pNode)->FormatAndChildren();
				}
				
				if (!(*pNode)->DoBecomeA(&ParamBecomeA))
				{
					return FALSE;
				}

				// quick check...
				if(!IS_A(ParamBecomeA.pPath, NodePath))
				{
					// didn't get a path!
					delete ParamBecomeA.pPath;
					return TRUE;		// don't complain, I suppose
				}

				// right then, replace the node with this one
				(*pNode)->CascadeDelete();
				delete *pNode;
				*pNode = ParamBecomeA.pPath;
				pPath = ParamBecomeA.pPath;
			}
		}
	}
	else
	{
		pPath = (NodePath *)*pNode;
	}
	ERROR2IF(pPath == NULL, FALSE, "no path, even though we tried really really hard");

	// right then, we've got to do something fabby. Get a new path to stuff stuff in
	Path *pOutputPath = new Path;
	if(pOutputPath == NULL || !pOutputPath->Initialise())
		return FALSE;		// something went wrong

	// clip the thingy to the thingy
	if(Data->pClippingPath->ClipPathToPath(pPath->InkPath, pOutputPath, 2 /*CLIP_STYLE_INTERSECT*/) > 0)
	{
		// clipping produced something...
		// now replace the path with the output path
		if(!pPath->InkPath.CloneFrom(*pOutputPath))
			return FALSE;
	}
	else
	{
		// clipping didn't produce anything -- delete the thingy
		pPath->CascadeDelete();
		delete pPath;

		// nothing doing, chaps.
		(*pNode) = NULL;
	}

	// delete the output path, don't need it any more
	delete pOutputPath;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::GetCorelBBox(NodeRenderable *pNode, DocRect *BBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		node and a pointer to bbox to fill in
	Returns:	error flag
	Purpose:	gets the Corel simple union bbox of an object

********************************************************************************************/

BOOL CMXImportFilter::GetCorelBBox(NodeRenderableBounded *pNode, DocRect *BBox)
{
	if(pNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
	{
		// ok, so it's a path. That means we can run through it to find out the simple
		// union of all points. 

		NodePath *pNodePath = (NodePath *)pNode;

		// get a pointer to the array of coords in the nice path
		INT32 NCoords = pNodePath->InkPath.GetNumCoords();
		DocCoord *Coords = pNodePath->InkPath.GetCoordArray();

		ERROR3IF(NCoords < 1, "Awooga! Trying to find a corel bbox of a path with less than one coord.");

		INT32 c;

		INT32 x0, y0, x1, y1;

		// initialise my bbox variables
		x0 = x1 = Coords[0].x;
		y0 = y1 = Coords[0].y;
		
		for(c = 1; c < NCoords; c++)
		{
			// update my bbox
			if(Coords[c].x < x0) x0 = Coords[c].x;
			if(Coords[c].y < y0) y0 = Coords[c].y;
			if(Coords[c].x > x1) x1 = Coords[c].x;
			if(Coords[c].y > y1) y1 = Coords[c].y;
		}

		// fill in *BBox
		BBox->lo.x = x0;
		BBox->lo.y = y0;
		BBox->hi.x = x1;
		BBox->hi.y = y1;
	} else {
		// if it's not a path object, ask the node what it's bounding box is - we'll just
		// have to make do with a *correct* bounding box instead of simply corel union.
		*BBox = pNode->GetBoundingRect();
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFiler::ApplyAttributesToNode(Node *pNode)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pointer to node to fiddle with, pointer to attributes from CMX file
	Returns:	error flag
	Purpose:	adds the attributes to the node.

********************************************************************************************/

BOOL CMXImportFilter::ApplyAttributesToNode(Node *pNode)
{
	// not having a node is a valid option here, and shouldn't be complained at
	// as the previous clipping operation may have decided that there wasn't anything
	// there after the intersection
	if(pNode == NULL)
		return TRUE;

	// apply the attributes to the object
	// If not filled, then set the ignore bit on the fill attribute.
	if(Data->ObjFilled == FALSE)
		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;

	// Add attributes to the path, if they are different from the default...
	BOOL Result = AttributeManager::ApplyBasedOnDefaults(pNode, CurrentAttrs);

	// Enable the fill attribute again
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;

	return Result;
}


/********************************************************************************************

>	BOOL CMXImportFilter::GotoSectionFromIndex(INT32 MasterIndexEntry, DWORD PredictedRIFFType,
			BOOL NonExistanceIsFormatError)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		MasterIndexEntry - entry to read position from
				PredictedRIFFType - the type the RIFF section should have
				NonExistanceIsFormatError - whether it's a format error for the section
						not to exist
				Found - pointer to flag to say whether we got it or not (can be null)
	Returns:	error flag
	Purpose:	positions the file pointer after the riff header for a section defined
				in the master index

********************************************************************************************/

BOOL CMXImportFilter::GotoSectionFromIndex(INT32 MasterIndexEntry, DWORD PredictedRIFFType,
		BOOL NonExistanceIsFormatError, BOOL *Found, INT32 *Size)
{
	if(Found != 0)
		(*Found) = FALSE;

	if(Data->MasterIndexEntries[MasterIndexEntry] == 0)
	{
		// if non-existance is a problem, do an error
		if(NonExistanceIsFormatError)
			CMXFORMATERROR(FALSE)
	
		// otherwise return true, but the found flag marks the problem
		return TRUE;
	}

	if(Found != 0)
		(*Found) = TRUE;

	TRACEUSER( "Ben", _T("index entry %d at %d\n"), MasterIndexEntry, Data->MasterIndexEntries[MasterIndexEntry]);
	Seek(Data->MasterIndexEntries[MasterIndexEntry]);
	RIFFck chdr;
	pFile->read(&chdr, sizeof(chdr));
	if(chdr.ckID != PredictedRIFFType)
		CMXFORMATERROR(FALSE)

	if(Size != 0)
		(*Size) = chdr.ckSize;

	return TRUE;
}




/********************************************************************************************

>	BOOL CMXImportFilter::SetAttributesFountain(Node *pNode, cmxiFillSpec *Fill)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		pointer to fill header
	Returns:	error flag
	Purpose:	sets graduated fill attributes

********************************************************************************************/

BOOL CMXImportFilter::SetAttributesFountain(Node *pNode, cmxiFillSpec *Fill)
{
	// find the bounding box of this lovely node
	DocRect BBox;
	if(!GetCorelBBox((NodeRenderableBounded *)pNode, &BBox))
		return FALSE;

	// find the minimum and maximim percentages along the grad fill line.
	DocColour *pStartColour = GetColourFromReference(Fill->Spec.Fountain.pColours->GetColourRefClosestToPosition(0));
	DocColour *pEndColour = GetColourFromReference(Fill->Spec.Fountain.pColours->GetColourRefClosestToPosition(100));

	// if it's a rainbow effect, work out whether an anti-clockwise thingy will be a
	// a normal or alt rainbow fill
	BOOL NormalRainbow = TRUE;
	if(Fill->Spec.Fountain.FillMode == cmxFILLMODE_HSB_CW ||
			 Fill->Spec.Fountain.FillMode == cmxFILLMODE_HSB_CCW)
	{
		// convert both colours to HSV to find their hue
		ColourContext *Conv = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);

		ColourHSVT StartC;
		ColourHSVT EndC;

		Conv->ConvertColour(pStartColour, (ColourGeneric *)&StartC);
		Conv->ConvertColour(pEndColour, (ColourGeneric *)&EndC);

		// compare the hues and work out a difference
		ColourValue Difference;

		if(EndC.Hue > StartC.Hue)
		{
			Difference = EndC.Hue - StartC.Hue;
		} else {
			Difference = 1.0 - (StartC.Hue - EndC.Hue);
		}

		// if the difference is more than 0.5 then it's a alt rainbow thingy
		if(Difference > fixed24(0.5))
			NormalRainbow = FALSE;
	}

	// set the fade effect
	switch(Fill->Spec.Fountain.FillMode)
	{
		case cmxFILLMODE_RGB:
		case cmxFILLMODE_CUSTOM:
		default:
			SetFadeFillEffect();
			break;

		case cmxFILLMODE_HSB_CCW:
			if(NormalRainbow)
				SetRainbowFillEffect();
			else
				SetAltRainbowFillEffect();
			break;

		case cmxFILLMODE_HSB_CW:
			if(NormalRainbow)
				SetAltRainbowFillEffect();
			else
				SetRainbowFillEffect();
			break;
	}

	// and finally, sort out the start and end points of the thingy
	switch(Fill->Spec.Fountain.Type)
	{
		case cmxFOUNTAINTYPE_LINEAR:
			if(!SetAttributesLinearGrad(pNode, Fill, BBox, pStartColour, pEndColour))
				return FALSE;
			break;

		case cmxFOUNTAINTYPE_RADIAL:
			if(!SetAttributesRadialGrad(pNode, Fill, BBox, pStartColour, pEndColour))
				return FALSE;
			break;

		case cmxFOUNTAINTYPE_SQUARE:
			if(!SetAttributesRadialGrad(pNode, Fill, BBox, pStartColour, pEndColour, TRUE))
				return FALSE;
			break;

		case cmxFOUNTAINTYPE_CONICAL:
			if(!SetAttributesConicalGrad(pNode, Fill, BBox, pStartColour, pEndColour))
				return FALSE;
			break;

		default:			
			return SetNoFill(pNode);
			break;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::SetAttributesLinearGrad(Node *pNode, cmxiFillSpec *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		grad fill defn and converted colours for the fill
	Returns:	error flag
	Purpose:	sets a linear graduated fill from fill definition in the file
				Copied from coreleps.cpp and modified
	SeeAlso:	Filter

********************************************************************************************/

BOOL CMXImportFilter::SetAttributesLinearGrad(Node *pNode, cmxiFillSpec *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)
{
	// NB. this function is (hopefully!) over-complex and will be simplified.
	// However, it's like this atm so I can get my head around the weird maths Corel 
	// forces us to use.

	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End;

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// bodge just in case of zero height or widths
	if(Width < 16)
		Width = 16;

	if(Height < 16)
		Height = 16;

	// Find centre of box
	DocCoord Centre;
	Centre.x = BBox.lo.x + (Width / 2);
	Centre.y = BBox.lo.y + (Height / 2);

	// Find total area of BBox
	double TotalArea = (double) Width * (double) Height;

	// Cope with angles > 180
	BOOL Mirror = FALSE;
	double Angle = GFill->Spec.Fountain.Angle;
	
	if (Angle >= PI)
	{
		Angle -= PI;
		Mirror = TRUE;
	}
	else if (Angle < 0)
	{
		Angle += PI;
		Mirror = TRUE;
	}

	Angle += (PI/2);

	if (Angle >= PI)
	{
		Angle -= PI;
	}

	// get edge pad value
	INT32 EdgePad = GFill->Spec.Fountain.Padding;
	
	// Calculate tan of the angle - convert angle to radians first.
	double TanTheta;
	if (Angle == (PI/2))
	{
		// Special case for horizontal grad fill arrow.

		// Make 0% padding first
		Start.x = BBox.lo.x;
		Start.y = Centre.y;
		End.x = BBox.hi.x;
		End.y = Centre.y;

		// Find out width of padding
		INT32 Padding = (Width * EdgePad) / 100;
		Start.x += Padding;
		End.x -= Padding;
	}
	else if (Angle == 0)
	{
		// Special case for vertical grad fill arrow.

		// Make 0% padding first
		Start.x = Centre.x;
		Start.y = BBox.lo.y;
		End.x = Centre.x;
		End.y = BBox.hi.y;

		// Find out width of padding
		INT32 Padding = (Height * EdgePad) / 100;
		Start.y += Padding;
		End.y -= Padding;
	}
	else
	{
		TanTheta = tan(Angle);

		// Find out what the maximum padding is that we can achieve using just triangles:

		// Find the maximum triangle width
		MILLIPOINT TriWidth = (MILLIPOINT) ((double) Height / TanTheta);

		// Limit it to sensible value
		if (TriWidth < 0)
			TriWidth = -TriWidth;
		if (TriWidth > Width)
			TriWidth = Width;

		// Find the maximum triangle width
		MILLIPOINT TriHeight = (MILLIPOINT) ((double) Width * TanTheta);

		// Limit it to sensible value
		if (TriHeight < 0)
			TriHeight = -TriHeight;
		if (TriHeight > Height)
			TriHeight = Height;

		// The 'c' values of the y = mx+c equation.
		MILLIPOINT StartC, EndC;

		// Work out the maximum percentage/edge padding this gives us
		// (50 because it's 100 / 2 because we want area of triangle, not rectangle).
		double Percentage = (50.0 * (double) TriWidth * (double) TriHeight) / TotalArea;

		INT32 Diff = 0;

		// Is this enough?
		if (((INT32) Percentage) >= EdgePad)
		{
			// Yes - calculate exactly how big the triangle needs to be.
			TriHeight = (MILLIPOINT) sqrt(ABS(((double) EdgePad * TotalArea * TanTheta) / 100.0));

			TriWidth = (MILLIPOINT) ((double) TriHeight / TanTheta);
			if (TriWidth < 0)
				TriWidth = -TriWidth;

			ENSURE(TriWidth < Width, "Error in Corel Grad fill decoding logic");
		}
		else
		{
			// How much percentage do we need to add with each rectangle?
			Percentage = (EdgePad - Percentage) / 2;

			// Handle the rectangle stuff.
			if (TriWidth == Width)
			{
				// Need to add rectangles to the top and bottom.
				Diff = (MILLIPOINT) ((Percentage * Height) / 100.0);
			}
			else
			{
				// Need to add rectangles to left and right
				Diff = (MILLIPOINT) ((Percentage * Width) / 100.0);
				Diff = (MILLIPOINT) (Diff / tan(PI - Angle));
				Diff = ABS(Diff);
			}
		}

		// Work out the C value for the start line (c = y - mx)
		// (m = tan(angle) )
		if (Angle == (PI/2))
		{
			//ENSURE(FALSE, "90 degree angle found!");
		}
		else if (Angle < (PI/2))
		{
			StartC = (MILLIPOINT) (BBox.lo.y - ((BBox.hi.x - TriWidth) * TanTheta));
			EndC = (MILLIPOINT) (BBox.hi.y - ((BBox.lo.x + TriWidth) * TanTheta));
		}
		else
		{
			StartC = (MILLIPOINT) (BBox.lo.y - ((BBox.lo.x + TriWidth) * TanTheta));
			EndC = (MILLIPOINT) (BBox.hi.y - ((BBox.hi.x - TriWidth) * TanTheta));
		}

		// Add on difference for rectangles, if any.
		StartC += Diff;
		EndC -= Diff;


		// Work out m and c for the grad fill line.
		// We know m is -1/m of the triangle's hypotenuse.
		// c = roy - (rox/m)
		double FillM = -1.00 / TanTheta;
		MILLIPOINT FillC = (MILLIPOINT) (Centre.y - (Centre.x * FillM));

		// Work out intersections:  x = (c2 - c1) / (2m)

		Start.x = (MILLIPOINT) ( (FillC - StartC) / (TanTheta + (1.00 / TanTheta)) );
		Start.y = (MILLIPOINT) ((FillM * Start.x) + FillC);

		End.x = (MILLIPOINT) ( (FillC - EndC) / (TanTheta + (1.00 / TanTheta)) );
		End.y = (MILLIPOINT) ((FillM * End.x) + FillC);
	}

	if (Mirror)
	{
		// Swap the grid fill end-points over.
		DocCoord Tmp = Start;
		Start = End;
		End = Tmp;
	}

	// Set the fill type according to these calculations.
	return SetLinearFill(*StartColour, *EndColour, Start, End);	
}


/********************************************************************************************

>	BOOL CMXImportFilter::SetAttributesConicalGrad(Node *pNode, cmxiFillSpec *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		grad fill defn and converted colours for the fill
	Returns:	error flag
	Purpose:	sets a conical graduated fill from fill definition in the file

********************************************************************************************/

BOOL CMXImportFilter::SetAttributesConicalGrad(Node *pNode, cmxiFillSpec *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)
{
	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End;

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// Start point is the centre given by Corel.
	// This centre is percentage offsets from the centre of the object, i.e. (0,0) is
	// the centre of the bounding box.
	Start.x = BBox.lo.x + (Width  / 2);
	Start.y = BBox.lo.y + (Height / 2);
 	Start.x += ((GFill->Spec.Fountain.Offset.x * Width) / 100);
	Start.y += ((GFill->Spec.Fountain.Offset.y * Height) / 100);

	// End point is start point + radius but takes into account the angle
	double Radius = Width / 2;
	double Theta = GFill->Spec.Fountain.Angle;
	
	// make the angle go anti-clockwise
	Theta = 0 - Theta;

	// rotate by PI / 2
	Theta -= PI / 2;
	
	// angle can be negative, ensure it's positive
	while(Theta < 0)
		Theta += (2 * PI);

	// calculate the triangle
	double dx, dy;

	dx = Radius * sin(Theta);
	dy = Radius * cos(Theta);

	End.x = Start.x + (INT32)dx;
	End.y = Start.y + (INT32)dy;

	// Seems that we need to swap start and end colours...

	// Set the fill type according to these calculations.
	return SetConicalFill(*EndColour, *StartColour, Start, End);	
}


/********************************************************************************************

>	BOOL CMXImportFilter::SetAttributesRadialGrad(Node *pNode, cmxiFillSpec *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		grad fill defn and converted colours for the fill
	Returns:	error flag
	Purpose:	sets a radial graduated fill from fill definition in the file

********************************************************************************************/

BOOL CMXImportFilter::SetAttributesRadialGrad(Node *pNode, cmxiFillSpec *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour, BOOL IsSquare)
{
	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End, End2;

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// caluculate the source area
	// first, what's the diagonal length
	double dWidth = Width;
	double dHeight = Height;
	INT32 Diagonal = (INT32)sqrt(dWidth*dWidth + dHeight*dHeight);

	// and from that calculate area of the box containing the bit of the
	// bit of the circle in the bbox
	INT32 Edge = (Diagonal * (100 - (GFill->Spec.Fountain.Padding * 2))) / 100;

	// Start point is the centre given by Corel.
	// This centre is percentage offsets from the centre of the object, i.e. (0,0) is
	// the centre of the bounding box.
	DocCoord Centre = DocCoord(BBox.lo.x + (Width / 2), BBox.lo.y + (Height / 2));
	INT32 OffX = (GFill->Spec.Fountain.Offset.x * Width) / 100;
	INT32 OffY = (GFill->Spec.Fountain.Offset.y * Height) / 100;
	Start.x = Centre.x + OffX;
	Start.y = Centre.y + OffY;

	// Find required radius of circle.
	double Radius = Edge / 2;

	// how far away is the centre of the fill from the centre of the bbox?
	double dOffX = OffX;
	double dOffY = OffY;
	double Dist = (INT32)sqrt(dOffX*dOffX + dOffY*dOffY);

	// and increase the radius by a bodge factor
	double BodgeFactor = 1 + (Dist / (double)(Diagonal / 2));
	Radius *= BodgeFactor;
	
	// End point is start point + radius
	if(IsSquare)
	{
		// angled thingy for square fills
		double Theta = GFill->Spec.Fountain.Angle;
		End.x = Start.x + ((MILLIPOINT)(Radius * cos(Theta)));
		End.y = Start.y + ((MILLIPOINT)(Radius * sin(Theta)));
		End2.x = Start.x + ((MILLIPOINT)(Radius * cos(Theta + PI/2)));
		End2.y = Start.y + ((MILLIPOINT)(Radius * sin(Theta + PI/2)));
	}
	else
	{
		// simple thingy for radial fills
		End.x = Start.x + ((MILLIPOINT) Radius);
		End.y = Start.y;
	}

	// Seems that we need to swap start and end colours...

	// Set the fill type according to these calculations.
	BOOL ok;
	if(IsSquare)
		ok = SetSquareFill(*EndColour, *StartColour, Start, End, End2);

	else
		ok = SetRadialFill(*EndColour, *StartColour, Start, End);

	return ok;	
}


/********************************************************************************************

>	BOOL CMXImportFilter::SetAttributesBitmapFill(INT32 VectorReference, cmxiTiling *Tile)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		vector reference, tiling specification
	Returns:	error flag
	Purpose:	sets a bitmap fill from possible fillness in the CMX file...

********************************************************************************************/

BOOL CMXImportFilter::SetAttributesBitmapFill(Node *pNode, INT32 VectorReference, cmxiTiling *Tile)
{
	// check the vector fill reference
	if(VectorReference <= 0 || VectorReference > Data->NumProcedures)
		return SetNoFill(pNode);

	// see if we can get a position for it
	INT32 Position = Data->Procedures[VectorReference-1].Position;
	if(Position <= 0)
		return SetNoFill(pNode);
	
	// store the position of the file for later
	INT32 StartFilePos = Tell();

	// jump to the position of the vector fill procedure
	Seek(Position);

	// variable for getting the bitmap reference
	INT32 Jenny = -1;

	// and see if we can find a draw image command in there somewhere -- skip the header
	RIFFck hdr;
	pFile->read(&hdr, sizeof(hdr));
	// I'm not going to check the type of this chunk as corel specify one type, and put
	// another in the file. Stupid documentionness
	BOOL EndProcLoc = Tell() + hdr.ckSize;

	// run through the loop... what follows is a load of instructions
	TRACEUSER( "Ben", _T("searching commands in vector fill procedure:\n"));
	BOOL Done = FALSE;
	while(Done == FALSE)
	{
		// look at a command
		// get the current file position
		INT32 CommandEndFilePos = Tell();

		// read the command header
		CI_READDATA(CommandHeader, ch)

		// if the command thing is negative, we may need to fudge it if we're going 16 bitness
		if(ch.Code < 0 && !Is32Bit)
			ch.Code = 0 - ch.Code;

		CommandEndFilePos += ch.Size;			// get the file pos of the end of the instruction

		// is this a bit of a DrawImage command?
		if(ch.Code == cmxINSTR_DrawImage)
		{
			// yep -- read in it's data
			CI_READDATA(DrawImage, di)

			if(di.ImageFileReference1 > 0 && di.ImageFileReference1 <= Data->NumRImages)
			{
				// found our bitmap image -- all done here
				Jenny = di.ImageFileReference1;
				break;		// don't bother looking at anything else
			}
		}
		else if(ch.Code == cmxINSTR_EndSection)
		{
			// the section finished -- stop looking now...
			break;
		}

		// finally, check that we're in the right position in the file for the next command
		if(Tell() != CommandEndFilePos)
			Seek(CommandEndFilePos);

		if(CommandEndFilePos >= EndProcLoc)
			break;
	}
	TRACEUSER( "Ben", _T("finished searching commands, ref = %d\n"), Jenny);

	// go back to where we were in the file
	Seek(StartFilePos);

	// now, did we get a bitmap reference? if not, give up now
	if(Jenny <= 0)
		return SetNoFill(pNode);

	// right then, get the bounding box for the object as we'll need this later
	DocRect BBox;
	if(!GetCorelBBox((NodeRenderableBounded *)pNode, &BBox))
		return FALSE;

	// get the coord things from the tiling info
	DocCoord StartPoint, EndPoint, EndPoint2;
	if(!GetTilingPoints(&BBox, Tile, &StartPoint, &EndPoint, &EndPoint2))
		return FALSE;

	// locate the bitmap thingy -- we know that the reference we have is OK
	if(!Data->RImages[Jenny-1].IsRImage())
		return SetNoFill(pNode);		// can't do that either without one of the RImages
	KernelBitmap *pBitmap = Data->RImages[Jenny-1].GetBitmap(this);
	if(pBitmap == 0)
		return SetNoFill(pNode);		// wasn't converted, oh dear

	// and set the fill (wow)
	return SetBitmapFill(pBitmap, StartPoint, EndPoint, EndPoint2);
}


/********************************************************************************************

>	BOOL CMXImportFilter::SetAttributesTwoColourBitmapFill(Node *pNode, cmxiFillSpec *Fill)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		node, fill spec
	Returns:	error flag
	Purpose:	sets a two colour contone fill

********************************************************************************************/

BOOL CMXImportFilter::SetAttributesTwoColourBitmapFill(Node *pNode, cmxiFillSpec *pFill)
{
	// right then, get the bounding box for the object as we'll need this later
	DocRect BBox;
	if(!GetCorelBBox((NodeRenderableBounded *)pNode, &BBox))
		return FALSE;

	// get the coord things from the tiling info
	DocCoord StartPoint, EndPoint, EndPoint2;
	if(!GetTilingPoints(&BBox, &pFill->Spec.TwoColourBitmap.Tile, &StartPoint, &EndPoint, &EndPoint2))
		return FALSE;

	// reverse the tiling thingys
	INT32 y0 = StartPoint.y;
	INT32 y1 = EndPoint2.y;
	StartPoint.y = y1;
	EndPoint2.y = y0;
	EndPoint.y = y1;

	// check the reference
	if(pFill->Spec.TwoColourBitmap.BitmapReference <= 0 || pFill->Spec.TwoColourBitmap.BitmapReference > Data->NumBitmaps)
		return SetNoFill(pNode);		// couldn't find the bitmap, go away, your mother was a hamster and your father smelled of elderberries

	// get the colours
	DocColour *pStartColour = GetColourFromReference(pFill->Spec.TwoColourBitmap.ForeColourReference);
	DocColour *pEndColour = GetColourFromReference(pFill->Spec.TwoColourBitmap.BackColourReference);

	// locate the bitmap thingy -- we know that the reference we have is OK
	KernelBitmap *pBitmap = Data->Bitmaps[pFill->Spec.TwoColourBitmap.BitmapReference - 1].CreateColouredCopy(this, pStartColour, pEndColour);
	if(pBitmap == 0)
		return SetNoFill(pNode);		// wasn't converted, oh dear

	// and set the fill and sod off back to the function we came from
	return SetBitmapFill(pBitmap, StartPoint, EndPoint, EndPoint2);
}


/********************************************************************************************

>	BOOL CMXImportFilter::GetTilingPoints(DocRect *BBox, cmxiTiling *Tile, DocCoord *StartPoint, DocCoord *EndPoint, DocCoord *EndPoint2)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		bbox of object, tiling struct, points to fill in
	Returns:	error flag
	Purpose:	works out points for the fill of a tiled thingy

********************************************************************************************/

BOOL CMXImportFilter::GetTilingPoints(DocRect *BBox, cmxiTiling *Tile, DocCoord *StartPoint, DocCoord *EndPoint, DocCoord *EndPoint2)
{
	if(Tile != 0)
	{
		// find the size of the tile
		INT32 SizeX = (INT32)(Tile->Width * Data->BaseScaleFactor);
		INT32 SizeY = (INT32)(Tile->Height * Data->BaseScaleFactor);

		// set to the top left + the first tile offset
		INT32 XPos = BBox->lo.x + ((SizeX * Tile->XOffset) / 100);
		INT32 YPos = BBox->hi.y - SizeY - ((SizeY * Tile->YOffset) / 100);

		// set the points
		*StartPoint = DocCoord(XPos, 			YPos);
		*EndPoint	= DocCoord(XPos + SizeX, 	YPos);
		*EndPoint2 	= DocCoord(XPos, 			YPos + SizeY);
	}
	else
	{
		// don't have a tile -- set things to fill the entire object
		*StartPoint = DocCoord(BBox->lo.x, BBox->lo.y);
		*EndPoint	= DocCoord(BBox->hi.x, BBox->lo.y);
		*EndPoint2 	= DocCoord(BBox->lo.x, BBox->hi.y);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::SkipToPreviewBitmap(CCLexFile * pFile)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/96
	Inputs:		file
	Returns:	True if it is a RIFF format file and found the preview bitmap.
				False if any problems are found or the file format is wrong.
	Purpose:	sees if there is a preview bitmap to fiddle with -- this will actually
				work for any RIFF file thingy

********************************************************************************************/

BOOL CMXImportFilter::SkipToPreviewBitmap(CCLexFile * pFile)
{
	// beginning
	pFile->seekIn(0);

	// get header
	RIFFFile_Header hdr;
	pFile->read(&hdr, sizeof(hdr));

	// is it a form?
	if(hdr.CK.ckID != RIFFTYPE_RIFF)
		return FALSE;		// no thingy

	INT32 FileEnd = hdr.CK.ckSize;
	INT32 where = sizeof(hdr);

	// OK, look at the first few chunks
	for(INT32 togo = 4; togo > 0 && where < FileEnd; togo--)
	{
		// read the chunk header
		RIFFck ck;
		pFile->read(&ck, sizeof(ck));

		where += sizeof(ck) + ck.ckSize;

		// does this have the required type?
		if(ck.ckID == cmxRIFFCI_Thumbnail)
		{
			// yep, found it -- skip four bytes
			DWORD clipboardformat;
			pFile->read(&clipboardformat, sizeof(clipboardformat));

			return TRUE;		// found it
		}

		// next thingy
		pFile->seekIn(where);
	}

	// nothing found, mate
	return FALSE;
}





/********************************************************************************************

>	void CMXImportFilter::SetNonTextAttributeIgnore()

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		none
	
	Purpose:	Set the non-text attribute to be ignored

********************************************************************************************/

void CMXImportFilter::SetNonTextAttributeIgnore()
{
	CurrentAttrs[ATTR_BAD_ID].Ignore = TRUE;		
	CurrentAttrs[ATTR_STROKECOLOUR].Ignore = TRUE;
	CurrentAttrs[ATTR_STROKETRANSP].Ignore = TRUE;
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;
	CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Ignore = TRUE;
	CurrentAttrs[ATTR_FILLMAPPING].Ignore = TRUE;
	CurrentAttrs[ATTR_TRANSPFILLMAPPING].Ignore = TRUE;
	CurrentAttrs[ATTR_FILLEFFECT].Ignore = TRUE;
	CurrentAttrs[ATTR_LINEWIDTH].Ignore = TRUE;
	CurrentAttrs[ATTR_WINDINGRULE].Ignore = TRUE;
	CurrentAttrs[ATTR_JOINTYPE].Ignore = TRUE;
	CurrentAttrs[ATTR_QUALITY].Ignore = TRUE;
	CurrentAttrs[ATTR_DASHPATTERN].Ignore = TRUE;
	CurrentAttrs[ATTR_STARTCAP].Ignore = TRUE;
	CurrentAttrs[ATTR_STARTARROW].Ignore = TRUE;
	CurrentAttrs[ATTR_ENDARROW].Ignore = TRUE;
	CurrentAttrs[ATTR_MITRELIMIT].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTFONTTYPEFACE].Ignore = TRUE;
	CurrentAttrs[ATTR_OVERPRINTLINE].Ignore = TRUE;
	CurrentAttrs[ATTR_OVERPRINTFILL].Ignore = TRUE;
	CurrentAttrs[ATTR_PRINTONALLPLATES].Ignore = TRUE;
	CurrentAttrs[ATTR_FIRST_FREE_ID].Ignore = TRUE;
	CurrentAttrs[ATTR_MOULD].Ignore = TRUE;
	CurrentAttrs[ATTR_ENDCAP].Ignore = TRUE;
}
	
/********************************************************************************************

>	void CMXImportFilter::SetNonTextAttributeNotIgnore()

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		none
	
	Purpose:	Set the non-text attribute not to be ignored

********************************************************************************************/

void CMXImportFilter::SetNonTextAttributeNotIgnore()
{
	CurrentAttrs[ATTR_BAD_ID].Ignore = FALSE;		
	CurrentAttrs[ATTR_STROKECOLOUR].Ignore = FALSE;
	CurrentAttrs[ATTR_STROKETRANSP].Ignore = FALSE;
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;
	CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Ignore = FALSE;
	CurrentAttrs[ATTR_FILLMAPPING].Ignore = FALSE;
	CurrentAttrs[ATTR_TRANSPFILLMAPPING].Ignore = FALSE;
	CurrentAttrs[ATTR_FILLEFFECT].Ignore = FALSE;
	CurrentAttrs[ATTR_LINEWIDTH].Ignore = FALSE;
	CurrentAttrs[ATTR_WINDINGRULE].Ignore = FALSE;
	CurrentAttrs[ATTR_JOINTYPE].Ignore = FALSE;
	CurrentAttrs[ATTR_QUALITY].Ignore = FALSE;
	CurrentAttrs[ATTR_DASHPATTERN].Ignore = FALSE;
	CurrentAttrs[ATTR_STARTCAP].Ignore = FALSE;
	CurrentAttrs[ATTR_STARTARROW].Ignore = FALSE;
	CurrentAttrs[ATTR_ENDARROW].Ignore = FALSE;
	CurrentAttrs[ATTR_MITRELIMIT].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTFONTTYPEFACE].Ignore = FALSE;
	CurrentAttrs[ATTR_OVERPRINTLINE].Ignore = FALSE;
	CurrentAttrs[ATTR_OVERPRINTFILL].Ignore = FALSE;
	CurrentAttrs[ATTR_PRINTONALLPLATES].Ignore = FALSE;
	CurrentAttrs[ATTR_FIRST_FREE_ID].Ignore = FALSE;
	CurrentAttrs[ATTR_MOULD].Ignore = FALSE;
	CurrentAttrs[ATTR_ENDCAP].Ignore = FALSE;
}
/********************************************************************************************

>	void CMXImportFilter::SetTextAttributeIgnore()

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		none
	
	Purpose:	Set the text attributes to be ignored

********************************************************************************************/

void CMXImportFilter::SetTextAttributeIgnore()
{
	CurrentAttrs[ATTR_TXTFONTTYPEFACE].Ignore = TRUE;		
	CurrentAttrs[ATTR_TXTBOLD].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTITALIC].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTASPECTRATIO].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTJUSTIFICATION].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTTRACKING].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTUNDERLINE].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTFONTSIZE].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTSCRIPT].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTBASELINE].Ignore = TRUE;
	CurrentAttrs[ATTR_TXTLINESPACE].Ignore = TRUE;

}
	
/********************************************************************************************

>	void CMXImportFilter::SetTextAttributeNotIgnore()

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		none
	
	Purpose:	Set the text attribute not to be ignored

********************************************************************************************/

void CMXImportFilter::SetTextAttributeNotIgnore()
{
	CurrentAttrs[ATTR_TXTFONTTYPEFACE].Ignore = FALSE;		
	CurrentAttrs[ATTR_TXTBOLD].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTITALIC].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTASPECTRATIO].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTJUSTIFICATION].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTTRACKING].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTUNDERLINE].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTFONTSIZE].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTSCRIPT].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTBASELINE].Ignore = FALSE;
	CurrentAttrs[ATTR_TXTLINESPACE].Ignore = FALSE;
	
}
