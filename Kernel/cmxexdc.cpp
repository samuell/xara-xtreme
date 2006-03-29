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
// CMX export device context

#include <math.h>
#include "camtypes.h"
#include "ccfile.h"
#include "cmxexdc.h"
#include "cmxform.h"
#include "riffform.h"
#include "docrect.h"
#include "colormgr.h"
#include "colmodel.h"
#include "camelot.h"
#include "cversion.h"
#include "prvwflt.h"
//#include "extfilts.h"
#include "product.h"
#include "hardwaremanager.h"
using namespace oilHardwareManager;

CC_IMPLEMENT_DYNAMIC(CMXFutureUpdater, CCObject);
CC_IMPLEMENT_DYNAMIC(CMXFutureLength, CMXFutureUpdater);
CC_IMPLEMENT_DYNAMIC(CMXFutureNumber, CMXFutureUpdater);
CC_IMPLEMENT_DYNAMIC(CMXFutureIndex, CMXFutureUpdater);
CC_IMPLEMENT_DYNAMIC(CMXReferListItem, ListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferListItemOffset, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferLayer, CMXReferListItemOffset);
CC_IMPLEMENT_DYNAMIC(CMXReferMasterIndex, CMXReferListItemOffset);
CC_IMPLEMENT_DYNAMIC(CMXReferPage, CMXReferListItemOffset);
CC_IMPLEMENT_DYNAMIC(CMXReferMasterLayer, CMXReferListItemOffset);
CC_IMPLEMENT_DYNAMIC(CMXReferColour, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferDefaultScreen, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferDefaultDotDash, CMXReferDotDash);
CC_IMPLEMENT_DYNAMIC(CMXReferPen, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferArrowheads, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferArrow, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferLens, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferLineStyle, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferOutline, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferBitmap, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferBitmapFill, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXReferDotDash, CMXReferListItem);
CC_IMPLEMENT_DYNAMIC(CMXGroupRecord, ListItem);
CC_IMPLEMENT_DYNAMIC(CMXNestedTagRecord, ListItem);
CC_IMPLEMENT_DYNAMIC(CMXLensBBox, ListItem);

/********************************************************************************************

>	CMXFutureUpdater::CMXFutureUpdater()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	none
	Purpose:	constructor
	SeeAlso:	

********************************************************************************************/

CMXFutureUpdater::CMXFutureUpdater()
{
	AbsPosition = -1;
	DataSize = 0;
}


/********************************************************************************************

>	CMXFutureUpdater::CMXFutureUpdater()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	none
	Purpose:	destructor
	SeeAlso:	

********************************************************************************************/

CMXFutureUpdater::~CMXFutureUpdater()
{
	if(AbsPosition >= 0)
		TRACEUSER( "Ben", _T("AWWOOOOGA! CMXFutureUpdated deleted, but not written\n\n"));
}


/********************************************************************************************

>	BOOL CMXFutureUpdater::Init(CMXExportDC *pDC, INT32 RelativePos, UINT32 DataSize)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		export dc, relative position (+ve = forwards in file), size of data
	Returns:	none
	Purpose:	initialises a updater object
	SeeAlso:	

********************************************************************************************/

BOOL CMXFutureUpdater::Init(CMXExportDC *pDC, INT32 RelativePos, UINT32 tDataSize)
{
	ERROR2IF(AbsPosition >= 0, FALSE, "Updater already initialised");
	ERROR2IF(pDC == 0, FALSE, "No pDC, chaps");
	ERROR2IF(DataSize < 0 || DataSize > 4, FALSE, "Bit of a data size problem really");

	INT32 CurrentPos = pDC->GetFilePosition();
	ERROR2IF(CurrentPos < 0, FALSE, "Whoops -- negative file position here");
	AbsPosition = CurrentPos + RelativePos;
	ERROR2IF(AbsPosition < 0, FALSE, "Serious Bummer -- expected to update a position before the start of the file");
	DataSize = tDataSize;

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXFutureUpdater::WriteValue(CMXExportDC *pDC, INT32 Value)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		export dc, value
	Returns:	none
	Purpose:	writes a value to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXFutureUpdater::WriteValue(CMXExportDC *pDC, INT32 Value)
{
	ERROR2IF(pDC == 0, FALSE, "No pDC, chaps");

	// write the number
	if(!pDC->WriteNumber(AbsPosition, DataSize, Value))
		return FALSE;

	// reset everything for the next run, should we want one.
	AbsPosition = -1;
	DataSize = 0;
	Reset();			// get the derivedness to reset itself too.

	return TRUE;
}


/********************************************************************************************

>	CMXFutureLength::CMXFutureLength()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	none
	Purpose:	constructor
	SeeAlso:	

********************************************************************************************/

CMXFutureLength::CMXFutureLength()
{
	StartPos = -1;
}

/********************************************************************************************

>	BOOL CMXFutureLength::SetLengthStartPos(CMXExportDC *pDC, INT32 RelativePos);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		export dc, relative position from current position (+ve is forwards)
	Returns:	none
	Purpose:	sets the start position of the length which is to be written
	SeeAlso:	

********************************************************************************************/

BOOL CMXFutureLength::SetLengthStartPos(CMXExportDC *pDC, INT32 RelativePos)
{
	ERROR2IF(StartPos >= 0, FALSE, "Start position already set");
	ERROR2IF(pDC == 0, FALSE, "No dc");
	StartPos = pDC->GetFilePosition() + RelativePos;

	return TRUE;
}


/********************************************************************************************

>	void CMXFutureLength::Reset(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		export dc, relative position from current position (+ve is forwards)
	Returns:	none
	Purpose:	resets the future updater object after a write
	SeeAlso:	

********************************************************************************************/

void CMXFutureLength::Reset(void)
{
	StartPos = -1;
}


/********************************************************************************************

>	BOOL CMXFutureLength::Write(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the length to the file. The length is the (+ve) value from the
				start position set earlier to the current position in the file.
	SeeAlso:	

********************************************************************************************/

BOOL CMXFutureLength::Write(CMXExportDC *pDC)
{
	ERROR2IF(pDC == 0, FALSE, "No dc");
	ERROR2IF(StartPos < 0, FALSE, "StartPos is negative... has this length been initialised properly?");

	INT32 len = pDC->GetFilePosition() - StartPos;

	ERROR2IF(len < 0, FALSE, "We're talking negative lengths here? I don't think so");
	
	return WriteValue(pDC, len);
}

/********************************************************************************************

>	BOOL CMXFutureIndex::Write(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the index to the file. The index is the current file position.
	SeeAlso:	

********************************************************************************************/

BOOL CMXFutureIndex::Write(CMXExportDC *pDC)
{
	ERROR2IF(pDC == 0, FALSE, "No dc");

	return WriteValue(pDC, pDC->GetFilePosition());
}


/********************************************************************************************

>	BOOL CMXFutureIndex::Write(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the number to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXFutureNumber::Write(CMXExportDC *pDC)
{
	ERROR2IF(pDC == 0, FALSE, "No dc");

	return WriteValue(pDC, Number);
}

/********************************************************************************************

>	CMXReferListItemOffset::CMXReferListItemOffset(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	constructs the refer list item by getting the offset
	SeeAlso:	

********************************************************************************************/

CMXReferListItemOffset::CMXReferListItemOffset(CMXExportDC *pDC)
	: CMXReferListItem(pDC)
{
	Offset = pDC->GetFilePosition();		// will throw an exception if it goes wrong
}


/********************************************************************************************

>	void CMXReferLayer::SetLayerDetails(WORD Number, String_256 *tName)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		layer number, name
	Returns:	none
	Purpose:	sets the layer details
	SeeAlso:	

********************************************************************************************/

void CMXReferLayer::SetLayerDetails(WORD Number, String_256 *tName)
{
	LayerNumber = Number;
	Name = *tName;
}


/********************************************************************************************

>	BOOL CMXReferLayer::WriteInReferenceList(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		export dc
	Returns:	sucess
	Purpose:	writes the relevant entry in the reference list of the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferLayer::WriteInReferenceList(CMXExportDC *pDC)
{
	ERROR2IF(pDC == 0, FALSE, "no DC");

	// write the entry
	cmxRefListEntryRef en;
	memset(&en, '\0', sizeof(en));

	en.Association = cmxREFLISTEN_DESC_COMPONENT;
	en.Type = cmxREFLISTEN_TYPE_LAYER;
	en.ReferenceNumber = LayerNumber;

	pDC->WriteData(&en, sizeof(en));

	return TRUE;
}


/********************************************************************************************

>	INT32 CMXReferLayer::IndexEntrySize(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		none
	Returns:	length of entry we'll write
	Purpose:	gets the length of the entry we'll write in the index
	SeeAlso:	

********************************************************************************************/

INT32 CMXReferLayer::IndexEntrySize(CMXExportDC *pDC)
{
	ERROR2IF(pDC == NULL, 0, "no dc");
	
	INT32 StrLen = Name.Length();

	return StrLen + sizeof(WORD) + sizeof(DWORD) * 2 + ((pDC->IsThirtyTwoBit())?sizeof(WORD):0);
}


/********************************************************************************************

>	BOOL CMXReferLayer::WriteInIndex(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the layer details in the layer list
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferLayer::WriteInIndex(CMXExportDC *pDC)
{
	// write size in file
	pDC->WriteSizeInFile(Name.Length() + sizeof(DWORD) * 2);

	DWORD tOffset = Offset;
	pDC->WriteData(&tOffset, sizeof(tOffset));

	pDC->WriteString(&Name);

	SDWORD RefList = -1;
	pDC->WriteData(&RefList, sizeof(RefList));

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferPage::WriteInIndex(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the page details in the layer list
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferPage::WriteInIndex(CMXExportDC *pDC)
{

	// write the entry
	cmxPageIndexEntry pe;

	// write size in file
	pDC->WriteSizeInFile(sizeof(pe));

	pe.PageOffset = Offset;
	pe.LayerTableOffset = LayerTableOffset;
	pe.ThumbnailOffset = 0;
	pe.RefListOffset = RefListOffset;

	if(!pDC->WriteData(&pe, sizeof(pe)))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferDefaultScreen::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the default screen list entry -- same as the default one corel writes
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferDefaultScreen::WriteInDesc(CMXExportDC *pDC)
{
	if(pDC->IsThirtyTwoBit())
	{
		cmxScreenDef32 scrdef = {0,0,0,0,0};

		if(!pDC->WriteTag(cmxTAG_DescrSection_Screen_Basic, &scrdef, sizeof(scrdef))
			|| !pDC->WriteMinEndTag())
			return FALSE;
	}
	else
	{
		cmxScreenDef16 scrdef = {0,0,0,0};

		if(!pDC->WriteData(&scrdef, sizeof(scrdef)))
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferDefaultDotDash::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the default dot dash list entry
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferDefaultDotDash::WriteInDesc(CMXExportDC *pDC)
{
	WORD DotCount = 0;

	if(!pDC->WriteTag(cmxTAG_DescrSection_Dash, &DotCount, sizeof(DotCount))
		|| !pDC->WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferColour::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the colour to the DC in a description list
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferColour::WriteInDesc(CMXExportDC *pDC)
{
	ColourModel OutputModel;
	struct {
		BYTE ColType;
		BYTE Palette;
	} ColH;
	ColH.Palette = 5;		// user palette

	// Work out which CMX colour type we will export as, and therefore which internal
	// colour model we should convert the colour to.
	switch (Colour.GetColourModel())
	{
		case COLOURMODEL_HSVT:
			OutputModel = COLOURMODEL_HSVT;
			ColH.ColType = cmxCOLMODEL_HSB;
			break;

		case COLOURMODEL_CMYK:
			OutputModel = COLOURMODEL_CMYK;
			ColH.ColType = cmxCOLMODEL_CMYK255;
			break;

		case COLOURMODEL_GREYT:
			OutputModel = COLOURMODEL_GREYT;
			ColH.ColType = cmxCOLMODEL_GREY;
			break;
		
		case COLOURMODEL_RGBT:
		default:
			// It's RGB, or we don't know the model, so we'll output it as RGB
			OutputModel = COLOURMODEL_RGBT;
			ColH.ColType = cmxCOLMODEL_RGB;
			break;
	}

	// write the header tag
	if(!pDC->WriteTag(cmxTAG_DescrSection_Color_Base, &ColH, sizeof(ColH)))
		return FALSE;

	// Find an appropriate colour context to convert the colour for output
	ColourContext *OutputCC = ColourManager::GetColourContext(OutputModel);
	ERROR3IF(OutputCC == NULL, "Can't find a colour context");

	// Convert the colour into a packed format
	ColourPacked Result;
	OutputCC->ConvertColour(&Colour, &Result);

	// start the tag
	if(!pDC->StartTag(cmxTAG_DescrSection_Color_ColorDescr))
		return FALSE;

	// Pull the converted colour out and export it to the CMX file
	switch (OutputModel)
	{
		case COLOURMODEL_HSVT:
			{
/*TRACEUSER( "Ben", _T("HSV colour: hue %d, sat %d, val %d\n"), Result.HSVT.Hue, Result.HSVT.Saturation, Result.HSVT.Value);
			WORD Hue = Result.HSVT.Hue;
			pDC->WriteData(&Hue, sizeof(Hue));
			pDC->WriteByte(Result.HSVT.Saturation);
			pDC->WriteByte(Result.HSVT.Value); */

				// BODGE because the colour system can't convert HSVT properly yet
			ColourHSVT GResult;
			OutputCC->ConvertColour(&Colour, (ColourGeneric *)&GResult);

			WORD Hue = (WORD)(360.0 * GResult.Hue.MakeDouble());
			pDC->WriteData(&Hue, sizeof(Hue));
			pDC->WriteByte((BYTE)(255.0 * GResult.Saturation.MakeDouble()));
			pDC->WriteByte((BYTE)(255.0 * GResult.Value.MakeDouble()));
			}
			break;

		case COLOURMODEL_CMYK:
TRACEUSER( "Ben", _T("CMYK colour: cyan %d mag %d yel %d key %d\n"), Result.CMYK.Cyan, Result.CMYK.Magenta, Result.CMYK.Yellow, Result.CMYK.Key);
			pDC->WriteByte(Result.CMYK.Cyan);
			pDC->WriteByte(Result.CMYK.Magenta);
			pDC->WriteByte(Result.CMYK.Yellow);
			pDC->WriteByte(Result.CMYK.Key);
			break;

		case COLOURMODEL_GREYT:
TRACEUSER( "Ben", _T("grey colour: grey %d\n"), Result.GreyT.Intensity);
			pDC->WriteByte(Result.GreyT.Intensity);
			break;

		case COLOURMODEL_RGBT:
		default:
TRACEUSER( "Ben", _T("RGB colour: red %d green %d blue %d\n"), Result.RGBT.Red, Result.RGBT.Green, Result.RGBT.Blue);
			pDC->WriteByte(Result.RGBT.Red);
			pDC->WriteByte(Result.RGBT.Green);
			pDC->WriteByte(Result.RGBT.Blue);
			break;
	}

	// end the tag
	if(!pDC->EndTag())
		return FALSE;

	// write the end tag
	if(!pDC->WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferMasterIndex::WriteInIndex(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the index details in the master index
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferMasterIndex::WriteInIndex(CMXExportDC *pDC)
{
	// write the entry
	cmxMasterIndexEntry me;

	me.Type = Type;
	me.Offset = Offset;

TRACEUSER( "Ben", _T("Writing entry in master index, type = %d, offset = %x = %d, size = %d\n"), Type, Offset, Offset, sizeof(me));

	if(!pDC->WriteData(&me, sizeof(me)))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferMasterIndex::WriteInIndex(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		export dc
	Returns:	none
	Purpose:	writes the index details in the master index
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferMasterLayer::WriteInIndex(CMXExportDC *pDC)
{
	// write the entry
	if(pDC->IsThirtyTwoBit())
	{
		cmxMasterLayerEntry32 le;		// note that this is the entire table, not an entry

		le.TableType = cmxMASTERIN_MASTERLAYER;		// quite why we need this is another matter
		le.SizeInFile = sizeof(DWORD);
		le.ID = 1;					// first page, I presume
		le.Offset = Offset;			// offset...

		if(!pDC->WriteData(&le, sizeof(le)))
			return FALSE;
	}
	else
	{
		cmxMasterLayerEntry16 le;		// note that this is the entire table, not an entry

		le.TableType = cmxMASTERIN_MASTERLAYER;		// quite why we need this is another matter
		le.ID = 1;					// first page, I presume
		le.Offset = Offset;			// offset...

		if(!pDC->WriteData(&le, sizeof(le)))
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	CMXExportDC::CMXExportDC(Filter *Parent)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		The filter object associated with this export DC.
	Purpose:	Initialise a DC for exporting EPS.

********************************************************************************************/

CMXExportDC::CMXExportDC(Filter *Parent) : ExportDC(Parent)
{
}



/********************************************************************************************

>	BOOL CMXExportDC::Init(CCLexFile* pFile, CMXRenderRegion *pReg, BOOL bThirtyTwoBit)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		pFile - The file to attach to the DC
	Returns:	TRUE if all went well, FALSE if not
	Purpose:	Calls the base class Init function and then sets up the DC to throw exceptions
				when it gets errors as that is how the EPS stuff expects errors to be reported.
	SeeAlso:	ExportDC::Init

********************************************************************************************/

BOOL CMXExportDC::Init(CCLexFile* pFile, CMXRenderRegion *pReg, BOOL bThirtyTwoBit, BOOL ClipboardExport)
{
	// First get the base class to do its thing
	if (!ExportDC::Init(pFile)) return FALSE;

	// now do what I want done.
	// We want export files to throw exceptions, and not report errors.
	ExportFile->SetThrowExceptions(TRUE);
	ExportFile->SetReportErrors(FALSE);

	// set the bitness of the file
	ThirtyTwoBit = bThirtyTwoBit;

	// mark the fact that we've not actually doing a section at all
	CurrentSection = CMXSECTION_NONE;

	// set up a few more variables
	DoingCommand = FALSE;
	DoingPage = FALSE;
	DoingLayer = FALSE;
	PageRefer = NULL;
	DoingTag = FALSE;
	pMatrix = NULL;
	NestedTagLevel = 0;
	CurrentInstrFilePosition = -1;
	pRenderRegion = pReg;
	DoingRIFFList = FALSE;
	NumberOfEntriesUsedInCache = 0;

	// overlapped lenses flag
	if(!ClipboardExport)
	{
		// normal ask the user mode
		AreIgnoreingOverlappedLenses = FALSE;
		OverlappedLensesHaveBeenIgnored = FALSE;
		HaveAskedUserAboutLenses = FALSE;
	}
	else
	{
		// don't ask the user
		AreIgnoreingOverlappedLenses = TRUE;
		OverlappedLensesHaveBeenIgnored = FALSE;
		HaveAskedUserAboutLenses = TRUE;
	}

	for(INT32 i = 0 ; i < CMXFUTUREVALUECACHESIZE ; i++)
	{
		//	init the write-cache 
		FutureDataCache[i].Location	= INT32_MAX;
		FutureDataCache[i].Data		= 0;
		FutureDataCache[i].DataSize	= 0;
	}

	// pop in the default screen thingy
	CMXReferDefaultScreen *pDefScreen = new CMXReferDefaultScreen(this);
	if(pDefScreen == NULL) return FALSE;

	// and add it to the list
	ReferList.AddTail(pDefScreen);

	// pop in the default dot dash thingy
	CMXReferDefaultDotDash *pDefdd = new CMXReferDefaultDotDash(this);
	if(pDefdd == NULL) return FALSE;

	// and add it to the list
	ReferList.AddTail(pDefdd);
	return TRUE;
}

/********************************************************************************************

>	void CMXExportDC::CleanUp(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		none
	Returns:	none
	Purpose:	Cleans up after writing to a file
	SeeAlso:	

********************************************************************************************/

void CMXExportDC::CleanUp(void)
{
	WriteFutureDataCache();
	ReferList.DeleteAll();
	LensBBoxes.DeleteAll();
}


/********************************************************************************************

>	BOOL CMXExportDC::StartSection(CMXSectionType Type)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		The type of section to start
	Returns:	success
	Purpose:	Start writing a section to a CMX file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::StartSection(CMXSectionType Type)
{
	ERROR2IF(CurrentSection != CMXSECTION_NONE, FALSE, "Tried to start a section without ending the previous one");

	// write the riff file header
	// first, decide what riff chunck id we want
	DWORD id;
	BOOL IsAList = FALSE;			// whether in fact it's a list
	switch(Type)
	{
	case CMXSECTION_NONE:
		ERROR2(FALSE, "Tried to start a section of type none. Oh dear");
		break;
	case CMXSECTION_HEADER:		id = cmxRIFFCI_Header;			break;
	case CMXSECTION_PAGE:		id = cmxRIFFCI_Page;			break;
	case CMXSECTION_REFLIST:	id = cmxRIFFCI_ReferenceList;	break;
	case CMXSECTION_INDEX:		id = cmxRIFFLIST_Index;	IsAList=TRUE;	break;
	case CMXSECTION_TRAILER:	id = cmxRIFFLIST_Trail;	IsAList=TRUE;	break;
	case CMXSECTION_ARROW:		id = cmxRIFFCI_Desc_Arrow;		break;
	case CMXSECTION_ARROWHEADS:	id = cmxRIFFCI_Desc_Arrowheads;	break;
	case CMXSECTION_COLOUR:		id = cmxRIFFCI_Desc_Colour;		break;
	case CMXSECTION_DOTDASH:	id = cmxRIFFCI_Desc_DotDash;	break;
	case CMXSECTION_FONT:		id = cmxRIFFCI_Desc_Font;		break;
	case CMXSECTION_OUTLINE:	id = cmxRIFFCI_Desc_Outline;	break;
	case CMXSECTION_LINESTYLE:	id = cmxRIFFCI_Desc_LineStyle;	break;
	case CMXSECTION_PEN:		id = cmxRIFFCI_Desc_Pen;		break;
	case CMXSECTION_LENS:		id = cmxRIFFCI_Desc_Lens;		break;
	case CMXSECTION_SCREEN:		id = cmxRIFFCI_Desc_Screen;		break;
	case CMXSECTION_IMAGINFO:	id = cmxRIFFCI_ImageInfo;		break;
	case CMXSECTION_IMAGDATA:	id = cmxRIFFCI_ImageData;		break;
	case CMXSECTION_PROCBITFILL:id = cmxRIFFCI_Procedure;		break;
	case CMXSECTION_PREVIEWBITMAP: id = cmxRIFFCI_Thumbnail;	break;
	default:
		ERROR2(FALSE, "Unknown section type");
		break;
	}

	// OK, write the RIFF header
	RIFFck chdr;
	chdr.ckID = IsAList?RIFFTYPE_LIST:id;
	chdr.ckSize = 0;
	ExportFile->write(&chdr, sizeof(chdr));

	// set the future length
	SectionLengthFuture.Init(this, FUTURE_BACKLOC(&chdr, ckSize), sizeof(chdr.ckSize));
	SectionLengthFuture.SetLengthStartPos(this, 0 /* - (INT32)sizeof(chdr)*/);

	// write the list type, if we're doing a list
	if(IsAList)
		ExportFile->write(&id, sizeof(id));

	// note which section type we just did
	CurrentSection = Type;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::EndSection(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/06/96
	Inputs:		none
	Returns:	success
	Purpose:	Finish off writing a section to a CMX file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::EndSection(void)
{
	ERROR2IF(CurrentSection == CMXSECTION_NONE, FALSE, "Tried to end a section which didn't exist");

	// write the length
	SectionLengthFuture.Write(this);

	// set the section type to none
	CurrentSection = CMXSECTION_NONE;

	// word align the file
	if((GetFilePosition() & 1) != 0)
	{
		// put an extra byte in the file
		ExportFile->put('\0');
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::StartRIFFList(INT32 Type)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/07/96
	Inputs:		The type of section to start
	Returns:	success
	Purpose:	Start writing a section to a CMX file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::StartRIFFList(INT32 Type)
{
	ERROR2IF(DoingRIFFList, FALSE, "Tried to start a riff list without ending the previous one");

	// write the riff list header
	RIFFck chdr;
	chdr.ckID = RIFFTYPE_LIST;
	chdr.ckSize = 0;
	ExportFile->write(&chdr, sizeof(chdr));

	// set the future length
	RIFFListLengthFuture.Init(this, FUTURE_BACKLOC(&chdr, ckSize), sizeof(chdr.ckSize));
	RIFFListLengthFuture.SetLengthStartPos(this, 0);

	// write the list type, if we're doing a list
	DWORD id = Type;
	ExportFile->write(&id, sizeof(id));

	// flag...
	DoingRIFFList = TRUE;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::EndRIFFList(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/07/96
	Inputs:		none
	Returns:	success
	Purpose:	Finish off writing a section to a CMX file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::EndRIFFList(void)
{
	ERROR2IF(!DoingRIFFList, FALSE, "Tried to end a riff list which didn't exist");

	// write the length
	RIFFListLengthFuture.Write(this);

	// word align the file
	if((GetFilePosition() & 1) != 0)
	{
		// put an extra byte in the file
		ExportFile->put('\0');
	}

	// flag
	DoingRIFFList = FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::StartTag(INT32 Tag)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		Tag for the command
	Returns:	success
	Purpose:	starts writing a tag to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::StartTag(INT32 Tag)
{
	if(!ThirtyTwoBit)
		return TRUE;		// 16 bit files don't have tags

	ERROR2IF(DoingTag == TRUE, FALSE, "already doing a tag");

	// write the tag header
	cmxTagHdr hdr;
	hdr.ID = (BYTE)Tag;
	hdr.Size = 0;
	ExportFile->write(&hdr, sizeof(hdr));

	// initialise the future value
	TagLengthFuture.Init(this, FUTURE_BACKLOC(&hdr, Size), sizeof(hdr.Size));
	TagLengthFuture.SetLengthStartPos(this, 0-(INT32)sizeof(hdr));

	// flag
	DoingTag = TRUE;

	return TRUE;
}

	
/********************************************************************************************

>	BOOL CMXExportDC::EndTag(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		none
	Returns:	success
	Purpose:	ends a tag
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::EndTag(void)
{
	if(!ThirtyTwoBit)
		return TRUE;		// 16 bit files don't have tags

	ERROR2IF(DoingTag == FALSE, FALSE, "not doing a tag");
	ERROR2IF(NestedTagLevel != 0, FALSE, "tried to end a main tag when nested tags are active");

	// write the length
	TagLengthFuture.Write(this);

	// flag
	DoingTag = FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::StartNestedTag(INT32 Tag)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		Tag for the command
	Returns:	success
	Purpose:	starts writing a nested tag to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::StartNestedTag(INT32 Tag)
{
	if(!ThirtyTwoBit)
		return TRUE;		// 16 bit files don't have tags

	ERROR2IF(DoingTag == FALSE, FALSE, "tried to start a nested tag outside a tag");

	// write the tag header
	cmxTagHdr hdr;
	hdr.ID = (BYTE)Tag;
	hdr.Size = 0;
	ExportFile->write(&hdr, sizeof(hdr));

	// get one of those nested tag record things
	CMXNestedTagRecord *pRec = new CMXNestedTagRecord;
	if(pRec == NULL)
		return FALSE;

	// initialise the future value
	pRec->LengthFuture.Init(this, FUTURE_BACKLOC(&hdr, Size), sizeof(hdr.Size));
	pRec->LengthFuture.SetLengthStartPos(this, 0-(INT32)sizeof(hdr));

	// add the record to the list
	NestedTagList.AddTail(pRec);

	// flag
	NestedTagLevel++;

	return TRUE;
}

	
/********************************************************************************************

>	BOOL CMXExportDC::EndNestedTag(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/96
	Inputs:		none
	Returns:	success
	Purpose:	ends a nested tag
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::EndNestedTag(void)
{
	if(!ThirtyTwoBit)
		return TRUE;		// 16 bit files don't have tags

	ERROR2IF(DoingTag == FALSE, FALSE, "not doing a tag");
	ERROR2IF(NestedTagLevel <= 0, FALSE, "not doing a nested tag");

	// get the last tag record
	CMXNestedTagRecord *pRec = (CMXNestedTagRecord *)NestedTagList.GetTail();

	// check it's OK
	ERROR2IF(!IS_A(pRec, CMXNestedTagRecord), FALSE, "tag record not a tag record");

	// write the length
	pRec->LengthFuture.Write(this);

	// delete the record
	delete NestedTagList.RemoveTail();

	// flag
	NestedTagLevel--;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::DoWriteTag(INT32 Tag, void *Data = 0, UINT32 Size = 0, BOOL Nested = FALSE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		none
	Returns:	success
	Purpose:	writes a tag in one go
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::DoWriteTag(INT32 Tag, void *Data, UINT32 Size, BOOL Nested)
{
	if(!Nested)
	{
		ERROR2IF(DoingTag == TRUE, FALSE, "already doing a tag");
	}

	// if we're not doing a 32 bit file, we just want to export the data
	if(ThirtyTwoBit)
	{
		// write the tag header
		cmxTagHdr hdr;
		hdr.ID = (BYTE)Tag;
		hdr.Size = sizeof(hdr)+Size;
		ExportFile->write(&hdr, sizeof(hdr));
	}

	// write the data, if required
	if(Size != 0)
		ExportFile->write(Data, Size);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteCommand(UINT32 Tag, void *Data, UINT32 Size)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		Tag for the command, the data, and the size
	Returns:	success
	Purpose:	Write a command to the file -- one off operation, writes command
				and data in one go. Use default params for a no-data command.
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteCommand(UINT32 Tag, void *Data, UINT32 Size)
{
	ERROR2IF(DoingCommand != FALSE, FALSE, "Already writing a command");
	
	// write the header
	cmxInstrHdr hdr = {Size + sizeof(hdr), ThirtyTwoBit?Tag:(0 - (SWORD)Tag)};

	// word align the size
	if((hdr.Size & 1) != 0)
		hdr.Size++;

	ExportFile->write(&hdr, sizeof(hdr));

	if(Size != 0)
	{
		ERROR2IF(Data == 0, FALSE, "didn't get some data");

		// write the command
		ExportFile->write(Data, Size);
	}

	Tally++;
	PageInstructionCount++;
	LayerInstructionCount++;

	// word align the file
	if((/*GetFilePosition()*/ Size & 1) != 0)
	{
		// put an extra byte in the file
		ExportFile->put('\0');
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::StartCommand(UINT32 Tag)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	success
	Purpose:	starts writing a command for a command where the data is of indetermant
				size. Use EndCommand to finish it off
	SeeAlso:	WriteCommand -- use for one off commands

********************************************************************************************/

BOOL CMXExportDC::StartCommand(UINT32 Tag)
{
	ERROR2IF(DoingCommand != FALSE, FALSE, "Already writing a command");

	// record the start of the file
	CurrentInstrFilePosition = GetFilePosition();

	// write the header
	cmxInstrHdr hdr = {0, ThirtyTwoBit?Tag:(0 - (SWORD)Tag)};
	ExportFile->write(&hdr, sizeof(hdr));

	Tally++;
	PageInstructionCount++;
	LayerInstructionCount++;

	// set the future length
	CommandLengthFuture.Init(this, FUTURE_BACKLOC(&hdr, Size), sizeof(hdr.Size));
	CommandLengthFuture.SetLengthStartPos(this, 0 - (INT32)sizeof(hdr));

	// set whether we're doing a command
	DoingCommand = TRUE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::EndCommand(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	success
	Purpose:	ends a command
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::EndCommand(void)
{
	ERROR2IF(DoingCommand != TRUE, FALSE, "Already writing a command");

	// word align the file
	if((GetFilePosition() & 1) != 0)
	{
		// put an extra byte in the file
		ExportFile->put('\0');
	}

	CommandLengthFuture.Write(this);

	DoingCommand = FALSE;

	CurrentInstrFilePosition = -1;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::CreateHeader(DocRect *pFileBBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		bounding box of file
	Returns:	success
	Purpose:	Write the file's header, and name notes of the location of things
				like the relevant sizes and indices and other stuff which can only
				be written after the export.

				This fn is called Create rather than Write because it's not finished
				off properly until the end of the file.
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::CreateHeader(DocRect *pFileBBox)
{
	ERROR2IF(pMatrix == NULL, FALSE, "No matrix yet");

	// first of all, think about the riff file header
	RIFFFile_Header rhdr;
	rhdr.CK.ckID = RIFFTYPE_RIFF;
	rhdr.CK.ckSize = 0;
	rhdr.FormType = cmxRIFFFORMTYPE_CMX;

	ExportFile->write(&rhdr, sizeof(rhdr));

	// set up the future length object
	FileLengthFuture.Init(this, FUTURE_BACKLOC(&rhdr, CK.ckSize), sizeof(rhdr.CK.ckSize));
	FileLengthFuture.SetLengthStartPos(this, /*-((INT32)sizeof(rhdr))*/ -(INT32)sizeof(DWORD));

	// start the CMX header section
	if(!StartSection(CMXSECTION_HEADER))
		return FALSE;

	// write the CMX header
	cmxHeader fhdr;
	memset(&fhdr, 0, sizeof(fhdr));			// set everything to zero
	_tcscpy((char *)fhdr.Id, cmxID);
	_tcscpy((char *)fhdr.OS, cmxOS);
	fhdr.ByteOrder = cmxBYTEORDER_INTEL | 0x30;
	if(ThirtyTwoBit)
	{
		fhdr.CoordSize = cmxCOORDSIZE_32 | 0x30;
		fhdr.Major = cmxMAJOR_32 | 0x30;
		fhdr.Factor = cmxFACTOR_32;
		fhdr.Unit = cmxUNIT_MILLIMETER;
	}
	else
	{
		fhdr.CoordSize = cmxCOORDSIZE_16 | 0x30;
		fhdr.Major = cmxMAJOR_16 | 0x30;
		fhdr.Factor = cmxFACTOR_16;
		fhdr.Unit = cmxUNIT_INCH;
	}
	fhdr.Minor = cmxMINOR | 0x30;

	// bounding box
	DocRect bb = *pFileBBox;
	pMatrix->TransformBounds(&bb);

	fhdr.IBBLeft	= bb.lo.x;
	fhdr.IBBTop		= bb.hi.y;
	fhdr.IBBRight	= bb.hi.x;
	fhdr.IBBBottom	= bb.lo.y;
	fhdr.IThumbnail	= -1;

	ExportFile->write(&fhdr, sizeof(fhdr));

	// set up it's future value objects
	Tally.Init(this, FUTURE_BACKLOC(&fhdr, ITally), sizeof(fhdr.ITally));
	Tally = 0;
	IndexSectionFuture.Init(this, FUTURE_BACKLOC(&fhdr, IIndexSection), sizeof(fhdr.IIndexSection));
	TrailerIndexFuture.Init(this, FUTURE_BACKLOC(&fhdr, IInfoSection), sizeof(fhdr.IInfoSection));
	ThumbnailIndexFuture.Init(this, FUTURE_BACKLOC(&fhdr, IThumbnail), sizeof(fhdr.IThumbnail));

	// end the CMX header section
	if(!EndSection())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::PolishOffThatFile()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	success
	Purpose:	Finish off the file, update indexes and all that
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::PolishOffThatFile()
{
	ERROR2IF(CurrentSection != CMXSECTION_NONE, FALSE, "Didn't end the section before ending the file");


	// write some future values
	if(!Tally.Write(this))
		return FALSE;

	// write the info section
	if(!TrailerIndexFuture.Write(this))
		return FALSE;
	if(!StartSection(CMXSECTION_TRAILER))
		return FALSE;
	if(!EndSection())
		return FALSE;

	// and pop the length in
	if(!FileLengthFuture.Write(this))
		return FALSE;
	

	WriteFutureDataCache();	


	return TRUE;
}

/********************************************************************************************

>	INT32 CMXExportDC::GetFilePosition();

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	success
	Purpose:	returns the position of the current file pointer in the export file
	SeeAlso:	

********************************************************************************************/

INT32 CMXExportDC::GetFilePosition()
{
	return ExportFile->tell();
}

/********************************************************************************************

>	BOOL CMXExportDC::WriteBBox(DocRect *BBox, BOOL Transform = TRUE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	success
	Purpose:	Write a bounding box to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteBBox(DocRect *BBox, BOOL Transform)
{
	ERROR2IF(pMatrix == NULL, FALSE, "No matrix");

	if(ThirtyTwoBit)
	{
		DocRect bb = *BBox;
		if(Transform)
			pMatrix->TransformBounds(&bb);
		
		cmxBBox32 b = {bb.lo.x, bb.hi.y, bb.hi.x, bb.lo.y};

		ExportFile->write(&b, sizeof(b));
	}
	else
	{
		DocRect bb = *BBox;
		if(Transform)
			pMatrix->TransformBounds(&bb);
		
		cmxBBox16 b = {(SWORD)bb.lo.x, (SWORD)bb.hi.y, (SWORD)bb.hi.x, (SWORD)bb.lo.y};

		ExportFile->write(&b, sizeof(b));
	}

	return TRUE;
}


/********************************************************************************************

>	void CMXExportDC::WriteAngle(double theta)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		angle, in radians
	Returns:	-
	Purpose:	writes an angle to the file
	SeeAlso:	

********************************************************************************************/

void CMXExportDC::WriteAngle ( double theta )
{
	// Convert theta to be measured in degrees.
	theta = ( theta * 360.0 ) / ( 2.0 * PI );

	// 32-bit export uses a larger scale factor.
	if(ThirtyTwoBit)
	{
		INT32	Angle32 = ( INT32 ) ( theta * 1000000.0 );
		ExportFile->write ( &Angle32, sizeof ( Angle32 ) );
	}
	else
	{
		short	Angle16 = ( short ) ( theta * 10.0);
		ExportFile->write ( &Angle16, sizeof ( Angle16 ) );
	}
}

/********************************************************************************************

>	BOOL CMXExportDC::WriteString(StringBase *Str)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	Write a bounding box to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteString(StringBase *Str)
{
	INT32 StrLen = Str->Length();

	cmxString h;
	h.Length = (WORD)StrLen;
	ExportFile->write(&h, sizeof(h));

	ExportFile->write((TCHAR *)(*Str), StrLen);

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXExportDC::WriteMatrix(Matrix *pMatrix = NULL)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		matrix to write, NULL for identity matrix
	Returns:	success
	Purpose:	writes a matrix to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteMatrix(Matrix *pMatrix)
{
	if(pMatrix == NULL)
	{
		// write an identity matrix
		WORD MatrixType = cmxMATRIXTYPE_IDENTITY;
		ExportFile->write(&MatrixType, sizeof(MatrixType));
	}
	else
	{
		// write the matrix
		cmxMatrix Thingy;
		FIXED16 abcd[4];
		INT32 ef[2];
		pMatrix->GetComponents((FIXED16 *)(&abcd), (INT32 *)(&ef));
		Thingy.Type = cmxMATRIXTYPE_GENERAL;
		Thingy.a = abcd[0].MakeDouble();
		Thingy.b = abcd[1].MakeDouble();
		Thingy.c = abcd[2].MakeDouble();
		Thingy.d = abcd[3].MakeDouble();
		Thingy.e = (DOUBLE)ef[0];
		Thingy.f = (DOUBLE)ef[1];

		ExportFile->write(&Thingy, sizeof(Thingy));
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXExportDC::WriteNumber(INT32 Position, UINT32 DataSize, INT32 Value);

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/96
	Inputs:		none
	Returns:	success
	Purpose:	Finish off the file, update indexes and all that

				Fixed by Ben so it doesn't write over the end of the array and
				screw up the other variables in the class and cause nasty AVs in
				retail builds which annoy Charles and take me ages to find.

********************************************************************************************/

BOOL CMXExportDC::WriteNumber(INT32 Position, UINT32 DataSize, INT32 Value)
{
TRACEUSER( "Claude", _T("Writing number %x at %d, size %d\n"), Value, Position, DataSize);

	//	When cache full => Flush it to the file.
	if(NumberOfEntriesUsedInCache >= (CMXFUTUREVALUECACHESIZE - 1))
		WriteFutureDataCache();

	//	Variable used to find out what entry is to be used
	INT32 theEntryInCache = 0;
	
	//	Search for the right place in the cache, i.e. sort the cache as
	//	it's generated
	while((theEntryInCache < NumberOfEntriesUsedInCache) && (Position > FutureDataCache[theEntryInCache].Location))
		theEntryInCache++;
	
	TRACEUSER( "Claude", _T("Writing entry %d"), theEntryInCache);
	
	if(theEntryInCache < NumberOfEntriesUsedInCache)
	{	//	If the entry to use is before the last entry already used in the cache
		//	Push all entries used from the one we want onward.
		
		//	Variable used to determine which entry is to be pushed
		//	Starting from the end.
		
		for(INT32 entrySorting = NumberOfEntriesUsedInCache;
			entrySorting >= theEntryInCache; entrySorting--)
		{
			//	Copy the entry onto the next one
			FutureDataCache[entrySorting+1] = FutureDataCache[entrySorting];
		}
	}
	
	//	Copy the information into the cache
	FutureDataCache[theEntryInCache].Location	=	Position;
	FutureDataCache[theEntryInCache].Data		=	Value;
	FutureDataCache[theEntryInCache].DataSize	=	DataSize;
	
	TRACEUSER( "Claude", _T("Writing number %x at %d, size %d\n"),
					FutureDataCache[theEntryInCache].Data,
					FutureDataCache[theEntryInCache].Location, 
					FutureDataCache[theEntryInCache].DataSize);
	//	Put the entry to use on the next one.
	NumberOfEntriesUsedInCache++;
	
	return TRUE;
}

/*******************************************************************************************

>	BOOL CMXExportDC::WriteFutureDataCache()

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96
	Inputs:		none
	Returns:	Success
	Purpose:	Write the Numbers from the cache to the file
	See also:

*******************************************************************************************/

BOOL CMXExportDC::WriteFutureDataCache()
{
	//	Find out the current position in the file
	INT32 here = ExportFile->tell();

	for(INT32 i = 0 ; i < NumberOfEntriesUsedInCache ; i++)
	{	// write the numbers to file
		
		TRACEUSER( "Claude", _T("Writing cache %x at %d, size %d\n"),
						FutureDataCache[i].Data,
						FutureDataCache[i].Location, 
						FutureDataCache[i].DataSize);

		//	Get the location where to put the info in the file
		ExportFile->seek(FutureDataCache[i].Location);
		
		if(FutureDataCache[i].DataSize != 0)
		{	//	If there is something flush it (there should always be something)
			
			switch(FutureDataCache[i].DataSize)
			{
				case sizeof(BYTE):
				{	//	If it's a BYTE, write a BYTE
					BYTE Num = (BYTE)FutureDataCache[i].Data;
					ExportFile->write(&Num, sizeof(Num));
				}
				break;
		
				case sizeof(WORD):
				{	//	If it's a WORD, write a WORD
					WORD Num = (WORD)FutureDataCache[i].Data;
					ExportFile->write(&Num, sizeof(Num));
				}
				break;

				case sizeof(INT32):
				{	//	If it's a INT32, write a INT32
					INT32 Num = FutureDataCache[i].Data;
					ExportFile->write(&Num, sizeof(Num));
				}
				break;
	
				default:
					//	If it's anything else, something went wrong
					ERROR3("OOOKK! Non supported data size");
				break;
			}
			//	Empty that entry of the cache, putting the Location to the maximum possible
			FutureDataCache[i].Location	=	INT32_MAX;
			FutureDataCache[i].Data		=	0;
			FutureDataCache[i].DataSize	=	0;
		}
	}	
	//	Put the file back to wherever it was before we used it
	ExportFile->seek(here);

	// reset the cache count thingy
	NumberOfEntriesUsedInCache = 0;

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXExportDC::StartPage(Rect *BBox, BOOL IsProcedure = FALSE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	starts a Page
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::StartPage(DocRect *BBox, BOOL IsProcedure)
{
	ERROR2IF(DoingPage == TRUE, FALSE, "Already doing a page");

	if(!IsProcedure)
	{
		////////////////// page version

		// make the page reference object
		PageRefer = new CMXReferPage(this);
		if(PageRefer == NULL)
			return FALSE;
		ReferList.AddTail(PageRefer);

		// start the section
		StartSection(CMXSECTION_PAGE);

		// write the page start
		StartCommand(cmxINSTR_BeginPage);

		// tag this bit
		if(!StartTag(cmxTAG_BeginPage_PageSpecification))
			return FALSE;

		cmxInstrPage1 phdr1;
		phdr1.PageNumber = 1;
		phdr1.Flags = 0;
		ExportFile->write(&phdr1, sizeof(phdr1));

		// set the future flags value
		PageFlags.Init(this, FUTURE_BACKLOC(&phdr1, Flags), sizeof(phdr1.Flags));
		PageFlags = 0;

		// write the bbox, transforming the thing of course
		WriteBBox(BBox);

		// write the rest of the page command
		cmxInstrPage2 phdr2;
		phdr2.EndPageOffset = 0;
		phdr2.GroupCount = 0;
		phdr2.InstructionCount = 0;
		ExportFile->write(&phdr2, sizeof(phdr2));

		// set up some future values
		PageEndIndexFuture.Init(this, FUTURE_BACKLOC(&phdr2, EndPageOffset), sizeof(phdr2.EndPageOffset));
		PageGroupCount.Init(this, FUTURE_BACKLOC(&phdr2, GroupCount), sizeof(phdr2.GroupCount));
		PageGroupCount = 0;
		PageInstructionCount.Init(this, FUTURE_BACKLOC(&phdr2, InstructionCount), sizeof(phdr2.InstructionCount));
		PageInstructionCount = 1; // to include this instr

		// end the tag
		if(!EndTag())
			return FALSE;

		// matrix tag...
		if(!StartTag(cmxTAG_BeginPage_Matrix))
			return FALSE;

		// write the matrix
		if(!WriteMatrix())
			return FALSE;

		// end the tag
		if(!EndTag())
			return FALSE;

		// mapping mode...
		BYTE MapMode = 0;
		if(!WriteTag(cmxTAG_BeginPage_MappingMode, &MapMode, sizeof(MapMode)))
			return FALSE;

		// end tag
		if(!WriteMinEndTag())
			return FALSE;

		// end the command
		EndCommand();

		// flag
		PageIsProc = FALSE;
	}
	else
	{
		///////////////////// procedure version

		// write the procedure start
		StartCommand(cmxINSTR_BeginProcedure);

		// tag this bit
		if(!StartTag(cmxTAG_BeginProcedure_ProcedureSpecification))
			return FALSE;

		cmxInstrProc1 phdr1;
		phdr1.Flags = 0;
		ExportFile->write(&phdr1, sizeof(phdr1));

		// set the future flags value
		PageFlags.Init(this, FUTURE_BACKLOC(&phdr1, Flags), sizeof(phdr1.Flags));
		PageFlags = 0;

		// write the bbox, not transforming it as we've already done that
		WriteBBox(BBox, FALSE);

		// write the rest of the page command
		cmxInstrProc2 phdr2;
		phdr2.EndPageOffset = 0;
		phdr2.GroupCount = 0;
		phdr2.InstructionCount = 0;
		ExportFile->write(&phdr2, sizeof(phdr2));

		// set up some future values
		PageEndIndexFuture.Init(this, FUTURE_BACKLOC(&phdr2, EndPageOffset), sizeof(phdr2.EndPageOffset));
		PageGroupCount.Init(this, FUTURE_BACKLOC(&phdr2, GroupCount), sizeof(phdr2.GroupCount));
		PageGroupCount = 0;
		PageInstructionCount.Init(this, FUTURE_BACKLOC(&phdr2, InstructionCount), sizeof(phdr2.InstructionCount));
		PageInstructionCount = 1; // to include this instr

		// end the tag
		if(!EndTag())
			return FALSE;

		// end tag
		if(!WriteMinEndTag())
			return FALSE;

		// end the command
		EndCommand();

		// flag
		PageIsProc = TRUE;
	}

	DoingPage = TRUE;
	LayerNumber = 0;

	// record the location of the first instruction in the file, which will follow immediately
	if(!IsProcedure)
		FirstInstrFilePosition = GetFilePosition();

	// write a comment to say where this file came from
	// return WriteCamComment();
	return TRUE;				// Graeme (10-2-00) replaced above return with return TRUE.
}


/********************************************************************************************

>	BOOL CMXExportDC::EndPage(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	ends a Page
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::EndPage(void)
{
	ERROR2IF(DoingPage == FALSE, FALSE, "Not doing a page");

	// write the index of the end page command
	PageEndIndexFuture.Write(this);

	// write the end page command
	if(!PageIsProc)
		WriteCommand(cmxINSTR_EndPage);
	else
		WriteCommand(cmxINSTR_EndSection);

	// write future values
	PageFlags.Write(this);
	PageInstructionCount.Write(this);
	PageGroupCount.Write(this);

	// finish the section
	if(!PageIsProc)
		EndSection();

	DoingPage = FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::StartLayer(String_256 &Name)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		layer name
	Returns:	success
	Purpose:	starts a layer
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::StartLayer(String_256 &Name)
{
	ERROR2IF(DoingPage == FALSE, FALSE, "Layers should be in pages");
	ERROR2IF(DoingLayer == TRUE, FALSE, "Already doing a layer");

	LayerNumber++;

	// get a entry for this thingy in the reference list
	CMXReferLayer *pRL = new CMXReferLayer(this);
	if(pRL == NULL)
		return FALSE;
	pRL->SetLayerDetails(LayerNumber, &Name);
	ReferList.AddTail(pRL);				// add it to the list

	// start the command
	StartCommand(cmxINSTR_BeginLayer);

	// tag
	if(!StartTag(cmxTAG_BeginLayer_LayerSpecification))
		return FALSE;

	// write the first bit of the instruction
	cmxInstrLayer1 chdr1;
	chdr1.PageNumber = 1;
	chdr1.LayerNumber = (WORD)LayerNumber;
	chdr1.LayerFlags = 0;
	chdr1.InstructionCount = 0;
	ExportFile->write(&chdr1, sizeof(chdr1));

	// set up the instruciton count future
	LayerInstructionCount.Init(this, FUTURE_BACKLOC(&chdr1, InstructionCount), sizeof(chdr1.InstructionCount));
	LayerFlags.Init(this, FUTURE_BACKLOC(&chdr1, LayerFlags), sizeof(chdr1.LayerFlags));
	LayerFlags = 0;

	// write it's name out
	WriteString(&Name);

	// end the tag
	if(!EndTag())
		return FALSE;

	// write the second bit of the thingy
	if(!StartTag(cmxTAG_BeginLayer_Matrix))
		return FALSE;
	if(!WriteMatrix())
		return FALSE;
	if(!EndTag())
		return FALSE;

	// mapping mode...
	BYTE MapMode = 0;
	if(!WriteTag(cmxTAG_BeginLayer_MappingMode, &MapMode, sizeof(MapMode)))
		return FALSE;
	
	// end tag
	if(!WriteMinEndTag())
		return FALSE;

	// end the command
	EndCommand();

	LayerInstructionCount = 1;		// to include this one

	DoingLayer = TRUE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::EndLayer(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	ends a layer
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::EndLayer(void)
{
	ERROR2IF(DoingLayer == FALSE, FALSE, "Already doing a layer");

	// write the end layer command
	WriteCommand(cmxINSTR_EndLayer);

	// write the count of instructions in the layer
	LayerInstructionCount.Write(this);
	LayerFlags.Write(this);

	DoingLayer = FALSE;

	return TRUE;
}



/********************************************************************************************

>	BOOL CMXExportDC::StartGroup(DocRect *BBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	starts a group
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::StartGroup(DocRect *BBox, BOOL TransformBBox)
{
	ERROR2IF(DoingLayer == FALSE && !PageIsProc, FALSE, "Groups should be in layers");
	
	// increment the group count
	PageGroupCount++;

	// start the command
	if(!StartCommand(cmxINSTR_BeginGroup))
		return FALSE;

	// start the tag
	if(!StartTag(cmxTAG_BeginGroup_GroupSpecification))
		return FALSE;

	// write the bounding box
	if(!WriteBBox(BBox, TransformBBox))
		return FALSE;

	// get a group record object
	CMXGroupRecord *pGRec = new CMXGroupRecord(this);
	if(pGRec == NULL)
		return FALSE;

	// add the group record object to the list
	GroupList.AddTail(pGRec);

	// write a place holder data chunk
	cmxGroupInfo gi = {0,0,0};
	ExportFile->write(&gi, sizeof(gi));

	if(!EndTag())
		return FALSE;

	if(!WriteMinEndTag())
		return FALSE;

	// end the command
	if(!EndCommand())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::EndGroup(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	ends a group
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::EndGroup(void)
{
	// get the last entry off the group list
	CMXGroupRecord *pGRec = (CMXGroupRecord *)GroupList.GetTail();

	// get it to write itself
	if(!pGRec->Write(this))
		return FALSE;

	// delete the group record
	delete GroupList.RemoveTail();

	// write the end group command
	if(!WriteCommand(cmxINSTR_EndGroup))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	CMXGroupRecord::CMXGroupRecord(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		a CMXExportDC
	Returns:	none
	Purpose:	initialises the group record
	SeeAlso:	

********************************************************************************************/

CMXGroupRecord::CMXGroupRecord(CMXExportDC *pDC)
{
	Offset = pDC->GetFilePosition();
	TallyAtStart = pDC->GetInstructionTally();
	GroupCountAtStart = pDC->GetGroupCount();
}


/********************************************************************************************

>	CMXGroupRecord::CMXGroupRecord(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		a CMXExportDC
	Returns:	sucess
	Purpose:	writes the info in the start of the group command
	SeeAlso:	

********************************************************************************************/

BOOL CMXGroupRecord::Write(CMXExportDC *pDC)
{
	// write the group count
	if(!pDC->WriteNumber(Offset, sizeof(WORD), pDC->GetGroupCount() - GroupCountAtStart))
		return FALSE;
	// write the instruction count
	if(!pDC->WriteNumber(Offset + sizeof(WORD), sizeof(DWORD), pDC->GetInstructionTally() - TallyAtStart))
		return FALSE;
	// write the group end location
	if(!pDC->WriteNumber(Offset + sizeof(WORD) + sizeof(DWORD), sizeof(DWORD), pDC->GetFilePosition()))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteReferenceList(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	writes the reference list for the page we've just done
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteReferenceList(void)
{
	// first, find the number of entries in this list
	WORD RefListEntries = 0;
	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "Entry in the refer list isn't a correct list item");

		if(pEn->IsInReferenceList())
			RefListEntries++;

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// set the location in the page reference item
	ERROR2IF(PageRefer == NULL, FALSE, "no page reference item");
	PageRefer->SetRefListOffset(GetFilePosition());
	RefListOffset = GetFilePosition();

	// start the section
	if(!StartSection(CMXSECTION_REFLIST))
		return FALSE;

	// write the number of entries in the list
	ExportFile->write(&RefListEntries, sizeof(RefListEntries));

	// write the entries
	pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "Entry in the refer list isn't a correct list item");

		if(!pEn->WriteInReferenceList(this))
			return FALSE;

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// end the section
	if(!EndSection())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteIndex(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	writes the index for the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteIndex(void)
{
	// start the section
	if(!StartSection(CMXSECTION_INDEX))
		return FALSE;

	// write the layer index
	if(!WriteAnIndex(cmxINDEX_LAYER))
		return FALSE;

	// write the master layer index
	if(!WriteAnIndex(cmxINDEX_MASTERLAYER))
		return FALSE;

	// write the page index
	if(!WriteAnIndex(cmxINDEX_PAGE))
		return FALSE;

	// write the procedure index
	if(!WriteAnIndex(cmxINDEX_PROCEDURE))
		return FALSE;

	// write the embedded file index
	if(!WriteAnIndex(cmxINDEX_EMBEDFILE))
		return FALSE;

	// write the embedded file index
	if(!WriteAnIndex(cmxINDEX_ARROW))
		return FALSE;

	// write the master index
	if(!WriteAnIndex(cmxINDEX_MASTERINDEX))
		return FALSE;

	// end the section
	if(!EndSection())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteCamComment(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/96
	Inputs:		none
	Returns:	success
	Purpose:	writes a helpful comment so we can see where dodgy cam cmx files come from
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteCamComment(void)
{
	if(PageIsProc)
		return TRUE;			// only write comments in pages

	CCamApp*	Application;
	String_64	Text;
	String_256	TheText = String_256("saved from " PRODUCT_NAME " "); // WEBSTER - markn 26/11/96

	// munge an info string together
	Application = (CCamApp*) AfxGetApp();			// Find our main application object

	Application->GetCamelotVersion(&Text);
	Text += TEXT("(v");
	Text += TEXT(PRODUCT_VERSION_NO);
	Text += TEXT(")");
	TheText += Text;

//	Text.MakeMsg(_R(IDS_CAMVERSION), &Text1);

	Text._MakeMsg(TEXT(" (#1%d) "), (INT32) CAMELOT_VERSION_MINOR);
	TheText += Text;

	Application->GetCamelotType(&Text);
	TheText += Text;
	
	TheText += " on ";
	Application->GetHostOSName(&Text);				// ... plonk info from it into the dlog
	TheText += Text;

	TRACEUSER( "Ben", _T("setting comment = %s\n"), (TCHAR *)TheText);

	// now start a command and a tag
	if(!StartCommand(cmxINSTR_Comment) || !StartTag(cmxTAG_Comment_CommentSpecification))
		return FALSE;

	WORD Size = TheText.Length();

	ExportFile->write(&Size, sizeof(Size));

	ExportFile->write((TCHAR *)TheText, Size);

	// end the command
	if(!EndTag() || !WriteMinEndTag() || !EndCommand())
		return FALSE;

  	return TRUE; 	
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteReferSections(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/96
	Inputs:		none
	Returns:	success
	Purpose:	gets all refer list items which are sections to write themselves
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteReferSections(void)
{
	// wander through the list
	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsASection())
		{
			if(!pEn->WriteSection(this))
				return FALSE;
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteAnIndex(INT32 Which)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	writes the a particular index to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteAnIndex(INT32 Which)
{
	// size of the table -- special actions may add some stuff to it
	INT32 TableSize = sizeof(WORD);	// for record count

	// get some types and sort out the master index entry for this table
	// and do any special actions for certain index types
	INT32 id;
	CMXReferMasterIndex *pMIE = new CMXReferMasterIndex(this);
	if(pMIE == NULL)
		return FALSE;
	BOOL NoMIE = FALSE;
	switch(Which)
	{
	case cmxINDEX_LAYER:
		{
			ERROR2IF(PageRefer == NULL, FALSE, "no page refer set up");
			PageRefer->SetLayerTableOffset(GetFilePosition());

			id = cmxRIFFCI_LayerIndex;
			NoMIE = TRUE;
			TableSize += sizeof(WORD);			// for page number

			// add an entry for the master layer table
			CMXReferMasterLayer *pML = new CMXReferMasterLayer(this);
			if(pML == NULL)
				return FALSE;
			ReferList.AddTail(pML);				// add it -- nothing else to set
		}
		break;

	case cmxINDEX_PAGE:
		id = cmxRIFFCI_PageIndex;
		pMIE->SetType(cmxMASTERIN_PAGE);
		break;

	case cmxINDEX_MASTERINDEX:
		// note the start of the index section in the file header
		IndexSectionFuture.Write(this);

		TableSize += sizeof(cmxMasterIndexHeader);
		id = cmxRIFFCI_MasterIndex;
		pMIE->SetType(cmxMASTERIN_MASTERIN);
		break;

	case cmxINDEX_MASTERLAYER:				// remember this is single entry table and is fudged
		id = cmxRIFFCI_MasterLayerIndex;	// in the refer list object
		pMIE->SetType(cmxMASTERIN_MASTERLAYER);
		break;

	case cmxINDEX_PROCEDURE:
		id = cmxRIFFCI_ProcIndex;
		pMIE->SetType(cmxMASTERIN_PROCEDURE);
		break;

	case cmxINDEX_EMBEDFILE:
		id = cmxRIFFCI_EmbeddedFileIndex;
		pMIE->SetType(cmxMASTERIN_EMBEDFILE);
		break;

	case cmxINDEX_ARROW:
		id = cmxRIFFCI_ArrowIndex;
		pMIE->SetType(cmxMASTERIN_ARROW);
		TableSize += sizeof(cmxArrowIndexHeader) + (ThirtyTwoBit?sizeof(WORD):0);
		break;

	default:
		ERROR3("Don't know what type of index to write!");
	}

	if(NoMIE)
	{
		delete pMIE;		// don't need this
	}
	else
	{
		ReferList.AddTail(pMIE);
	}

	// scan the refer list and see how many entries we've got, and how long they are
	WORD Entries = 0;
	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsInWhichIndex() == Which)
		{
			Entries++;
			TableSize += pEn->IndexEntrySize(this);
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	if(Entries == 0)
	{
		// nothing to write -- go away now after removing the entry from the master index
		delete ReferList.RemoveItem(pMIE);
		return TRUE;
	}

	// write the header
	RIFFck ck;
	ck.ckID = id;
	ck.ckSize = TableSize;
	ExportFile->write(&ck, sizeof(ck));

	if(Which == cmxINDEX_MASTERINDEX)
	{
		// write it's special header
		cmxMasterIndexHeader mih = {cmxMASTERIN_MASTERIN, ThirtyTwoBit?0xfff:0x18};
		ExportFile->write(&mih, sizeof(mih));
	}

	ExportFile->write(&Entries, sizeof(Entries));		// write record count

	// do specials
	switch(Which)
	{
	case cmxINDEX_LAYER:
		{
			WORD PageNumber = 1;
			ExportFile->write(&PageNumber, sizeof(PageNumber));
		}
		break;

	case cmxINDEX_ARROW:
		{
			cmxArrowIndexHeader aih;
			WriteSizeInFile(sizeof(cmxArrowIndexEntry));
			aih.Type = cmxARROWINDEXTYPE;
			ExportFile->write(&aih, sizeof(aih));
		}
		break;

	default:
		break;
	}

	// write the entries
	pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsInWhichIndex() == Which)
		{
			if(!pEn->WriteInIndex(this))
				return FALSE;
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// if necessary, word align the thingy
	if((GetFilePosition() & 1) != 0)
	{
		// put an extra byte in the file
		ExportFile->put('\0');
	}

	// done
	return TRUE;
}

/********************************************************************************************

>	void CMXExportDC::SetCMXFlag(INT32 flag)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		none
	Returns:	none
	Purpose:	sets a flag for pages, layers
	SeeAlso:	

********************************************************************************************/

void CMXExportDC::SetCMXFlag(INT32 flag)
{
	PageFlags = ((INT32)PageFlags) | flag;
	LayerFlags = ((INT32)LayerFlags) | flag;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteDescriptionSections(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		none
	Returns:	success
	Purpose:	writes the descption sections
	SeeAlso:	

********************************************************************************************/

#define WRITEDESC(h) if(!WriteADesc(h)) return FALSE;

BOOL CMXExportDC::WriteDescriptionSections(void)
{
	WRITEDESC(cmxDESC_ARROW)
	WRITEDESC(cmxDESC_ARROWHEADS)
	WRITEDESC(cmxDESC_DOTDASH)
	WRITEDESC(cmxDESC_FONT)
	WRITEDESC(cmxDESC_OUTLINE)
	WRITEDESC(cmxDESC_LINESTYLE)
	WRITEDESC(cmxDESC_PEN)
	WRITEDESC(cmxDESC_SCREEN)
	WRITEDESC(cmxDESC_COLOUR)

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXExportDC::WriteADesc(INT32 Which)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Inputs:		none
	Returns:	success
	Purpose:	writes the a particular index to the file
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WriteADesc(INT32 Which)
{
	// get some types and sort out the master index entry for this table
	// and do any special actions for certain index types
	INT32 Section;
	CMXReferMasterIndex *pMIE = new CMXReferMasterIndex(this);
	if(pMIE == NULL)
		return FALSE;
	BOOL NoMIE = FALSE;
	BOOL HasRecordCount = TRUE;
	BOOL CanOmit = FALSE;
	switch(Which)
	{
	case cmxDESC_ARROW:
		Section = CMXSECTION_ARROW;
		CanOmit = TRUE;
		pMIE->SetType(cmxMASTERIN_ARROW);
		HasRecordCount = FALSE;
		break;

	case cmxDESC_ARROWHEADS:
		Section = CMXSECTION_ARROWHEADS;
		pMIE->SetType(cmxMASTERIN_ARROWHEADS);
		break;

	case cmxDESC_COLOUR:
		Section = CMXSECTION_COLOUR;
		pMIE->SetType(cmxMASTERIN_COLOUR);
		break;

	case cmxDESC_DOTDASH:
		Section = CMXSECTION_DOTDASH;
		pMIE->SetType(cmxMASTERIN_DOTDASH);
		break;

	case cmxDESC_FONT:
		Section = CMXSECTION_FONT;
		pMIE->SetType(cmxMASTERIN_FONT);
		CanOmit = TRUE;
		HasRecordCount = FALSE;
		break;

/*	case cmxDESC_IMAGE:
		Section = CMXSECTION_IMAGE;
		pMIE->SetType(cmxMASTERIN_IMAGE);
		CanOmit = TRUE;
		HasRecordCount = FALSE;
		break;
*/
	case cmxDESC_OUTLINE:
		Section = CMXSECTION_OUTLINE;
		pMIE->SetType(cmxMASTERIN_OUTLINE);
		break;

	case cmxDESC_LINESTYLE:
		Section = CMXSECTION_LINESTYLE;
		pMIE->SetType(cmxMASTERIN_LINESTYLE);
		break;

	case cmxDESC_PEN:
		Section = CMXSECTION_PEN;
		pMIE->SetType(cmxMASTERIN_PEN);
		break;

	case cmxDESC_SCREEN:
		Section = CMXSECTION_SCREEN;
		pMIE->SetType(cmxMASTERIN_SCREEN);
		break;
	
	default:
		ERROR3("Don't know what type of desc to write!");
	}

	// scan the refer list and see how many entries we've got, and how long they are
	WORD Entries = 0;
	CMXReferListItem *pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsInWhichDesc() == Which)
		{
			Entries++;
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// do we need to save this section?
	if(Entries == 0 && CanOmit)
	{
		delete pMIE;		// delete the record
		return TRUE;		// don't do anything
	}

	// add this section to the master entry list
	if(NoMIE)
	{
		delete pMIE;		// don't need this
	}
	else
	{
		ReferList.AddTail(pMIE);
	}

	// start the section, and maybe write a record count
	if(!StartSection((CMXExportDC::CMXSectionType)Section))
		return FALSE;
	if(HasRecordCount)
		ExportFile->write(&Entries, sizeof(Entries));		// write record count

	// write the entries
	pEn = (CMXReferListItem *)ReferList.GetHead();
	while(pEn != 0)
	{
		ERROR3IF(!pEn->IsKindOf(CC_RUNTIME_CLASS(CMXReferListItem)), "unexpected type of entry in refer list");

		if(pEn->IsInWhichDesc() == Which)
		{
			if(!pEn->WriteInDesc(this))
				return FALSE;
		}

		pEn = (CMXReferListItem *)ReferList.GetNext(pEn);
	}

	// end the section
	if(!EndSection())
		return FALSE;

	// done
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferDotDash::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		none
	Returns:	success
	Purpose:	writes a dot dash pattern to the description section
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferDotDash::WriteInDesc(CMXExportDC *pDC)
{
	pDash->CheckAndFix();			// just in case

	// start the tag
	if(!pDC->StartTag(cmxTAG_DescrSection_Dash))
		return FALSE;

	// dot count
	WORD DotCount = (WORD)pDash->Elements;
	if(!pDC->WriteData(&DotCount, sizeof(DotCount)))
		return FALSE;

	// write the elements
	for(INT32 l = 0; l < pDash->Elements; l++)
	{
		WORD Element = (WORD)((pDash->ElementData[l] + 4) / pDash->LineWidth);
		if(Element <= 0) Element = 1;
		Element *= 4;
		if(!pDC->WriteData(&Element, sizeof(Element)))
			return FALSE;
	}

	// end the tag and write the end tag
	if(!pDC->EndTag() || !pDC->WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferArrow::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		none
	Returns:	success
	Purpose:	writes a arrowhead definition to the description section
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferArrow::WriteInDesc(CMXExportDC *pDC)
{
	// store the position of this thingy
	ArrowFilePosition = pDC->GetFilePosition();

	// start a tag
	if(!pDC->StartTag(cmxTAG_DescrSection_Arrow))
		return FALSE;

	// get some details about the path of the arrowhead
	Path *pPath = pArrow->GetArrowPath();
	ERROR2IF(pPath == NULL, FALSE, "no arrow path");
	INT32 NumCoords = pPath->GetNumCoords();
	DocCoord *Coords = pPath->GetCoordArray();
	PathVerb *Verbs = pPath->GetVerbArray();
	DocRect ArrowBBox;
	pDC->CalcCorelBBox(Coords, NumCoords, &ArrowBBox);

	// work out a matrix for transforming the arrowhead path
	DocCoord ArrowCentre = pArrow->GetArrowCentre();
	// this bounding box needs to be scaled so that it's in the required range
	INT32 dl = abs(ArrowCentre.x - ArrowBBox.lo.x);
	INT32 dr = abs(ArrowBBox.hi.x - ArrowCentre.x);
	INT32 db = abs(ArrowCentre.y - ArrowBBox.lo.y);
	INT32 dt = abs(ArrowBBox.hi.y - ArrowCentre.y);
	INT32 maxdist = (dl > dr)?dl:dr;
	maxdist = (maxdist > db)?maxdist:db;
	maxdist = (maxdist > dt)?maxdist:dt;
	// maxdist is the greatest distance from the centre point -- this is
	// the distance which determines the scaling factor
	double ScaleFactor =
		((double)(pDC->IsThirtyTwoBit()?cmxARROWDEF_MAXCOORD32:cmxARROWDEF_MAXCOORD16))
		/ ((double)maxdist);
	Matrix Mat((FIXED16)ScaleFactor, 0, 0, (FIXED16)ScaleFactor, - ArrowCentre.x, - ArrowCentre.y);

	// export the path, transforming each coordinate as we go along

	// do we need to add an extra point?
	BOOL BungInExtraPoint = FALSE;
	if(Coords[0] != Coords[NumCoords - 1])
		BungInExtraPoint = TRUE;

	// write the point count
	WORD Count = (WORD)NumCoords + (BungInExtraPoint?1:0);
	pDC->WriteData(&Count, sizeof(Count));

	// run through the coords writing and transforming
	INT32 l;
	for(l = 0; l < NumCoords; l++)
	{
		DocCoord Coord = Coords[l];
		Mat.transform(&Coord);

		// write it...
		if(pDC->IsThirtyTwoBit())
		{
			cmxPoint32 p = {Coord.x, Coord.y};
			pDC->WriteData(&p, sizeof(p));
		}
		else
		{
			cmxPoint16 p = {(SWORD)Coord.x, (SWORD)Coord.y};
			pDC->WriteData(&p, sizeof(p));
		}
	}

	if(BungInExtraPoint)
	{
		DocCoord Coord = Coords[0];
		Mat.transform(&Coord);

		// write it...
		if(pDC->IsThirtyTwoBit())
		{
			cmxPoint32 p = {Coord.x, Coord.y};
			pDC->WriteData(&p, sizeof(p));
		}
		else
		{
			cmxPoint16 p = {(SWORD)Coord.x, (SWORD)Coord.y};
			pDC->WriteData(&p, sizeof(p));
		}
	}

	// run through the verbs writing point types
	INT32 BezCount = 0;		// for counting Bez
	for(l = 0; l < NumCoords; l++)
	{
		BYTE Type;

		switch(Verbs[l] & (PT_LINETO | PT_MOVETO | PT_BEZIERTO))
		{
		case PT_MOVETO:
			{
				Type = CMXNODETYPE_TYPE_MOVE | CMXNODETYPE_USER;
			
				// wibble forwards through the verbs to see if the correspondingness is closed
				for(INT32 lp = l + 1; lp < NumCoords; lp++)
				{
					// OK, is this a move?
					if((Verbs[lp] & (PT_LINETO | PT_MOVETO | PT_BEZIERTO)) == PT_MOVETO)
						break;

					// is closefigure set?
					if((Verbs[lp] & PT_CLOSEFIGURE) != 0)
					{
						Type |= CMXNODETYPE_CLOSED;
						break;
					}
				}
			}
			BezCount = 0;
			break;

		case PT_LINETO:
			Type = CMXNODETYPE_TYPE_LINE | CMXNODETYPE_USER;
			BezCount = 0;
			break;

		case PT_BEZIERTO:
			// in CMX, the control points have type ARC, and the end points
			// have type CURVE. This code does the trickery.
			if(BezCount == 2)
			{
				Type = CMXNODETYPE_TYPE_CURVE | CMXNODETYPE_USER;
			}
			else
			{
				Type = CMXNODETYPE_TYPE_ARC;
			}
			BezCount++;
			if(BezCount > 2)
				BezCount = 0;		// so that only the end points of beziers get user flag
			break;

		default:
			ERROR3("Unknown node type");
			break;
		}

		if((Verbs[l] & PT_CLOSEFIGURE) != 0)
		{
			if(!BungInExtraPoint || l != (NumCoords - 1))
				Type |= CMXNODETYPE_CLOSED;
		}

		if(l == (NumCoords - 1) && !BungInExtraPoint)
			Type |= CMXNODETYPE_CLOSED;

		pDC->WriteData(&Type, sizeof(Type));
	}

	if(BungInExtraPoint)
	{
		BYTE Type = CMXNODETYPE_TYPE_LINE | CMXNODETYPE_USER | CMXNODETYPE_CLOSED;
		pDC->WriteData(&Type, sizeof(Type));
	}

	// write the line offset
	DWORD LineOffset = 0;
	if(!pDC->WriteData(&LineOffset, sizeof(LineOffset)))
		return FALSE;

	// end the tag
	if(!pDC->EndTag() || !pDC->WriteMinEndTag())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXReferArrow::WriteInDesc(CMXExportDC *pDC)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		none
	Returns:	success
	Purpose:	writes the index entry for the arrowhead
	SeeAlso:	

********************************************************************************************/

BOOL CMXReferArrow::WriteInIndex(CMXExportDC *pDC)
{
	// make the entry
	cmxArrowIndexEntry En;
	En.FilePosition = ArrowFilePosition;

	// write it...
	return pDC->WriteData(&En, sizeof(En));
}


/********************************************************************************************

>	BOOL CMXExportDC::WritePreviewBitmap(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/96
	Inputs:		none
	Returns:	success
	Purpose:	writes a preview bitmap type thing
	SeeAlso:	

********************************************************************************************/

BOOL CMXExportDC::WritePreviewBitmap(void)
{
	Filter* pFilter = NULL;
	pFilter = Filter::GetFirst();
	while ((pFilter != NULL) && (pFilter->FilterID != FILTERID_PREVIEW_BMP))
	{
		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	// Make sure that we found the preview bitmap filter
	if ((pFilter != NULL) && (pFilter->IS_KIND_OF(BaseBitmapFilter)))
	{
		// write the index of this thumbnail section
		ThumbnailIndexFuture.Write(this);

		// start the section
		if(!StartSection(CMXSECTION_PREVIEWBITMAP))
			return FALSE;

		// write a number
		DWORD Eight = 8;
		Eight = NativetoLE(Eight);
		ExportFile->write(&Eight, sizeof(Eight));

		// fudge things a bit
		INT32 Start = ExportFile->tell();
		BITMAPFILEHEADER hdr;
		ExportFile->seek(Start - sizeof(hdr));
		ExportFile->read(&hdr, sizeof(hdr));
		ExportFile->seek(Start - sizeof(hdr));

		//WEBSTER-Martin-03/01/97
		// force the Preview to a fixed size
		INT32 OldPreviewSize = PreviewFilter::PreviewBitmapSize;
		// Set the Preview to be just over an inch accross
		PreviewFilter::PreviewBitmapSize = 96000;

		// Get as Bitmap Filter and set it up
		BaseBitmapFilter* pBitmapFilter = (BaseBitmapFilter*) pFilter;
		pBitmapFilter->SetPreviewBitmap(TRUE);

		// Export the Preview to the file
		BOOL ok = pFilter->DoExport(pOp, ExportFile, pPath, TheDocument);
		// If the Preview bitmap failed to export then we still need to clean up

		// Set it back
		pBitmapFilter->SetPreviewBitmap(FALSE);
		// Set the Preview back to its default setting
		PreviewFilter::PreviewBitmapSize = OldPreviewSize;

		// replace the data which was overwritten
		INT32 Here = ExportFile->tell();
		ExportFile->seek(Start - sizeof(hdr));
		ExportFile->write(&hdr, sizeof(hdr));
		ExportFile->seek(Here);

		// finish the section
		if(!EndSection())
			return FALSE;

		return ok;
	}

	return TRUE;
}

/********************************************************************************************

>	static double CMXExportDC::Round ( double ToRound )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Inputs:		ToRound	- The value to be rounded up.
	Returns:	double	- The rounded value.
	Purpose:	Rounds a value up or down to the nearest integer.

********************************************************************************************/

double CMXExportDC::Round ( double ToRound )
{
	// Add 0.5 to the value, and floor it. This will return the nearest integer.
	return floor ( ToRound + 0.5 );
}
