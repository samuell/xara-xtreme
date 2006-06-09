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
// handles CMX commands

#include "camtypes.h"
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "riffform.h"
#include "progress.h"
#include "nodepath.h"
#include "nodebmp.h"
#include "nodershp.h"

#include "cmxifltr.h"
#include "cmxistut.h"
#include "cmxidata.h"
#include "cmxibits.h"
#include "cmxibitm.h"
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "riffform.h"
#include "progress.h"
#include "nodepath.h"
//#include "nev.h"

#include "nodetxts.h"
#include "unicdman.h"
#include "nodetxtl.h"


/********************************************************************************************

>	BOOL CMXImportFilter::Stage4_ConvertPage(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	Goes through the page processing the commands.

********************************************************************************************/

BOOL CMXImportFilter::Stage4_ConvertPage(void)
{
	TRACEUSER( "Ben", _T("Stage4_ConvertPage\n"));
	// plan here -- read the index entries to find the start of the page,
	// wander over to the right position in the file and start processing
	// we simply loop around calling the process command function, and
	// sorting out the progress thingy. Admittedly, this does mean that
	// during the loading of the description sections you'll just get a
	// zero progress count. Oh well, c'est la vie.

	// find the page index, and grab the first page in it
	if(Data->MasterIndexEntries[cmxMASTERIN_PAGE] == 0)
		CMXFORMATERROR(FALSE)

	TRACEUSER( "Ben", _T("index page at %d?\n"), Data->MasterIndexEntries[cmxMASTERIN_PAGE]);
	Seek(Data->MasterIndexEntries[cmxMASTERIN_PAGE]);
	RIFFck chdr;
	pFile->read(&chdr, sizeof(chdr));
	if(chdr.ckID != cmxRIFFCI_PageIndex)
		CMXFORMATERROR(FALSE)

	// get the index header
	CI_READDATA(SimpleIndexHeader, ih)
	if(ih.RecordCount < 1)
		CMXFORMATERROR(FALSE)

	// get the first index entry
	CI_READDATA(PageIndexEntry, ie)
	TRACEUSER( "Ben", _T("Page is at %d\n"), ie.PageOffset);

	// seek to that position, and check the riff header
	Seek(ie.PageOffset);
	pFile->read(&chdr, sizeof(chdr));
	if(chdr.ckID != cmxRIFFCI_Page)
		CMXFORMATERROR(FALSE)				// didn't get one of those page things there. Oh dear...

	// set the end position of the page
	INT32 PageEndFileLoc = Tell() + chdr.ckSize;
	Data->InPage = TRUE;

	// progress system stuff
	INT32 wobble = 0;

	// run through the loop... what follows is a load of instructions
	TRACEUSER( "Ben", _T("processing commands:\n"));
	while(Data->InPage == TRUE)
	{
		// process a command
		if(!ProcessCommand(&wobble))
			return FALSE;					// error

		// check to see if we've over run the page...
		if(Tell() >= PageEndFileLoc)
			Data->InPage = FALSE;

		// progress system
		if(Data->CommandsInPage != -1)
		{
			wobble++;
			if(!Data->pProgress->Update(wobble))
			{ 
TRACEUSER( "claude", _T("User Escaped!\n"));
				Error::SetError(_R(IDN_CMXCANCELIMPORT));
			//TRACEUSER( "claude", _T("after SetError()\n"));
				//InformError(_R(IDN_CMXCANCELIMPORT));//_R(IDN_USER_CANCELLED));//Error::GetErrorNumber());
			TRACEUSER( "claude", _T("after InformError()\n"));
				return FALSE;		// user pressed escape... abort abort!
			}
		}
	}
	TRACEUSER( "Ben", _T("finished processing commands\n"));
TRACEUSER( "claude", _T("finished processing commands\n"));
	// that's it, chaps
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Stage5_ConvertPage(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a CMX command -- mainly a dispatch function

********************************************************************************************/

#define PC_DISPATCH(instr)	case cmxINSTR_##instr: \
							TRACEUSER( "Ben", _T(" * " #instr "\n")); \
							TRACEUSER( "Claude", _T(" * " #instr "\n")); \
							if(!Process_##instr()) return FALSE; break;
#define NEEDLAYER			{if(Data->pCurrentLayer == NULL) {TRACEUSER( "Ben", _T("no layer\n")); \
							CMXFORMATERROR(FALSE)}}

BOOL CMXImportFilter::ProcessCommand(INT32 *ProgressThingy)
{
	// get the current file position
	INT32 CommandEndFilePos = Tell();

	// read the command header
	CI_READDATA(CommandHeader, ch)

	// if the command thing is negative, we may need to fudge it if we're going 16 bitness
	if(ch.Code < 0 && !Is32Bit)
		ch.Code = 0 - ch.Code;

	CommandEndFilePos += ch.Size;			// get the file pos of the end of the instruction

	// dispatch it to the relevant function
	switch(ch.Code)
	{
	PC_DISPATCH(BeginPage)
	PC_DISPATCH(EndPage)
	PC_DISPATCH(BeginLayer)
	PC_DISPATCH(EndLayer)
	PC_DISPATCH(BeginGroup)
	PC_DISPATCH(EndGroup)
	PC_DISPATCH(EndSection)
	PC_DISPATCH(PolyCurve)
	PC_DISPATCH(Rectangle)						// Graeme ( 1-2-00 ).
	PC_DISPATCH(AddGlobalTransform)
	PC_DISPATCH(SetGlobalTransform)
	PC_DISPATCH(RestoreLastGlobalTransform)
	PC_DISPATCH(DrawImage)
	PC_DISPATCH(AddClippingRegion)
	PC_DISPATCH(RemoveLastClippingRegion)
	PC_DISPATCH(ClearClipping)
	PC_DISPATCH(BeginTextStream)
	PC_DISPATCH(TextFrame)
	PC_DISPATCH(Characters)
	PC_DISPATCH(CharInfo)
	PC_DISPATCH(BeginParagraph)
	PC_DISPATCH(EndParagraph)
	PC_DISPATCH(EndTextStream)
	

	case cmxINSTR_JumpAbsolute:
		{
			CI_READDATA(JumpAbsolute, ja)
			TRACEUSER( "Ben", _T(" * JumpAbsolute to %d\n"), ja.Offset);
			TRACEUSER( "Claude", _T(" * JumpAbsolute to %d\n"), ja.Offset);
			// got a jump absolute thingy -- alter the next thing to skip over something
			// silly that Corel's lovely amazingly neat method of storing stuff in a
			// file has stuck in our way
			CommandEndFilePos = ja.Offset;		// make us seek there later
			// JumpAbsolutes aren't counted as instructions, for some reason
			if(ProgressThingy != NULL)
				(*ProgressThingy)--;
		}
		break;

	default:
		// don't understand this command, don't need to do anything special here
		TRACEUSER( "Ben", _T(" * <<unknown command>>\n"));
		TRACEUSER( "Claude", _T(" * <<unknown command>>\n"));
		TRACEUSER( "Graeme", _T(" * <<unknown command>>\n"));
		break;
	}

	// finally, check that we're in the right position in the file for the next command
	if(Tell() != CommandEndFilePos)
		Seek(CommandEndFilePos);

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Process_EndPage(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a EndPage command

********************************************************************************************/

BOOL CMXImportFilter::Process_EndPage(void)
{
	// stop it. Stop it now. I mean it, stop processing that page right now
	// or I set the killer carrot on you, and you wouldn't want that, would you?
	Data->InPage = FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_BeginPage(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a BeginPage command

********************************************************************************************/

BOOL CMXImportFilter::Process_BeginPage(void)
{
	// read the begin page structure from the file
	CI_READDATA(BeginPage, bp)

	// set progress system up (again)
	Data->CommandsInPage = bp.InstructionCount;
	if(Data->pProgress != NULL)
	{
		delete Data->pProgress;
		Data->pProgress = NULL;
	}
	Data->pProgress = new Progress(_R(IDT_CMX_IMPORTMSG), Data->CommandsInPage, FALSE);
	if(Data->pProgress == NULL)
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_EndLayer(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a EndLayer command

********************************************************************************************/

BOOL CMXImportFilter::Process_EndLayer(void)
{
	if(Data->pCurrentLayer == NULL)	// then we weren't doing a layer
		CMXFORMATERROR(FALSE)

	// mark as not doing a layer
	Data->pCurrentLayer = NULL;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_BeginLayer(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a BeginLayer command

********************************************************************************************/

BOOL CMXImportFilter::Process_BeginLayer(void)
{
	if(Data->pCurrentLayer != NULL)	// already doing a layer
		CMXFORMATERROR(FALSE)

	// read the data from the file
	CI_READDATA(BeginLayer, bl)
	TRACEUSER( "Ben", _T("   layer name = %s\n"), (TCHAR *)(*bl.pLayerName));

	// create a new layer object
	Data->pCurrentLayer = new CMXImportLayer;
	if(Data->pCurrentLayer == NULL)
		return FALSE;

	// add the layer to the list
	Data->Layers.AddTail(Data->pCurrentLayer);

	// set the name of the layer
	Data->pCurrentLayer->SetName(bl.pLayerName);

	// clear up
	delete bl.pLayerName;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_BeginGroup(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a BeginGroup command

********************************************************************************************/

BOOL CMXImportFilter::Process_BeginGroup(void)
{
	NEEDLAYER

	if(!Data->pCurrentLayer->StartGroup())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_EndGroup(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a EndGroup command

********************************************************************************************/

BOOL CMXImportFilter::Process_EndGroup(void)
{
	NEEDLAYER

	if(!Data->pCurrentLayer->EndGroup())
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_PolyCurve(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a PolyCurve command

********************************************************************************************/

BOOL CMXImportFilter::Process_PolyCurve(void)
{
	NEEDLAYER

	// read everything I need to know from the file
	CI_READDATA(PolyCurve, pc)

	// check to see if there's a container to think about
	// ONLY polycurves will have containers in a meaningful way -- other objects
	// needn't bother to check the flag
	if((pc.RenderAttributes.Mask & cmxRENDATTRMASK_CONTAIN) != 0)
	{
		if(!Process_PolyCurveContainer(pc))
			return FALSE;
	}
	
	// process it... create a node path with the nice path we've just got
	NodePath *pPNode = new NodePath;
	if(pPNode == NULL || !pPNode->SetUpPath(pc.PointList->GetNumCoords() + 4))
		return FALSE;

	if(!pPNode->InkPath.CloneFrom(*pc.PointList))
		return FALSE;

	pPNode->InkPath.IsFilled = pc.PointList->IsFilled;
	pPNode->InkPath.IsStroked = pc.PointList->IsStroked;

	if(!pPNode->InkPath.EnsureValid())
		return FALSE;

	Node *pNode = pPNode;

	// do all the necessary things on the path
	if(!TransformNode(pNode)
		|| !SetAttributesForNode(pNode, &pc.RenderAttributes)
		|| !ClipNode(&pNode)
		|| !ApplyAttributesToNode(pNode))
		return FALSE;

	// add the node
	Data->pCurrentLayer->AddNode(pNode);

	// clear up after ourselves
	delete pc.PointList;

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Process_PolyCurveContainer(cmxiPolyCurve &pc)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	does all the stuff

********************************************************************************************/

BOOL CMXImportFilter::Process_PolyCurveContainer(cmxiPolyCurve &pc)
{
	// right then, we've got to process one of those there containers
	// we get a reference to a procedure of things inside a page

	// check the procedure reference
	if(pc.RenderAttributes.ContainerSpec.ContainerReference <= 0
			|| pc.RenderAttributes.ContainerSpec.ContainerReference > Data->NumProcedures)
		CMXFORMATERROR(FALSE)

	// see if we can get a position for it
	INT32 Position = Data->Procedures[pc.RenderAttributes.ContainerSpec.ContainerReference-1].Position;
	if(Position <= 0)
		CMXFORMATERROR(FALSE)
	
	// store the position of the file for later
	INT32 StartFilePos = Tell();

	// jump to the position of the vector fill procedure
	Seek(Position);

	// and see if we can find a draw image command in there somewhere -- skip the header
	RIFFck hdr;
	pFile->read(&hdr, sizeof(hdr));
	// I'm not going to check the type of this chunk as corel specify one type, and put
	// another in the file. Stupid documentionness
	BOOL EndProcLoc = Tell() + hdr.ckSize;

	// make a copy of the path of the polycurve, and set a new clipping region to it
	Path *pClipPath = new Path;
	if(pClipPath == NULL || !pClipPath->Initialise())
		return FALSE;		// no pathy thingy for us to use
	pClipPath->CloneFrom(*pc.PointList);
	CMXImportClipper *pClipper = new CMXImportClipper(pClipPath, TRUE);
	if(pClipper == NULL)
		return FALSE;
	Data->ClipperStack.Push(pClipper);

	// run through the commands processing them
	TRACEUSER( "Ben", _T("processing commands in container procedure:\n"));
	BOOL InProc = TRUE;
	Data->EndSectionFound = FALSE;
	while(InProc == TRUE)
	{
		// process a command
		if(!ProcessCommand())
			return FALSE;					// error

		if(Data->EndSectionFound == TRUE)
			InProc = FALSE;

		// check to see if we've over run the page...
		if(Tell() >= EndProcLoc)
			InProc = FALSE;
	}
	TRACEUSER( "Ben", _T("finished processing commands in procedure\n"));

	// go back to where we were in the file
	Seek(StartFilePos);

	// get rid of the clipping region
	delete Data->ClipperStack.Pop();

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Process_Rectangle ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/02/00
	Inputs:		-
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Processes a rectangle command from within the CMX filter.

********************************************************************************************/

BOOL CMXImportFilter::Process_Rectangle ( void )
{
	NodeRegularShape	*pRectangle	= NULL;
	Matrix				RectMatrix;
	DocCoord			MajorAxis	( 0, 0 );
	DocCoord			MinorAxis	( 0, 0 );
	double				RootTwo		= sqrt ( 2 );
	FIXED16				RotateCos;
	FIXED16				RotateSin;

	// One of Ben's macros. It generates an error if there isn't a layer present.
	NEEDLAYER

	// Read the data from the file.
	CI_READDATA ( Rectangle, Rect )

	// Create a new node.
	if ( ( pRectangle = new NodeRegularShape () ) == NULL )
		return FALSE;

	// The major and minor axes of the NodeRegularShape correspond to the height and
	// width of the bounding ellipse. By multiplying half of their values by the
	// square root of two, the ellipse's dimensions are obtained.
	MajorAxis.y = ( INT32 ) ( ( double ) ( Rect.Height / 2 ) * RootTwo );
	MinorAxis.x = ( INT32 ) ( ( double ) ( Rect.Width / 2 ) * RootTwo );

	// Calculate the sine and cosine for the rotation matrix.
	RotateSin = sin ( Rect.Rotation );
	RotateCos = cos ( Rect.Rotation );

	// Compose the transformation matrix for the shape.
	RectMatrix.Compose ( RotateCos, RotateCos, -RotateSin, RotateSin, Rect.Centre );

	pRectangle->SetUpShape ();						// Initialise the shape node.
	pRectangle->SetNumSides ( 4 );					// Always four sides to a rectangle.
	pRectangle->SetTransformMatrix ( &RectMatrix );	// The position of the rectangle.
	pRectangle->SetCentrePoint ( DocCoord (0, 0) );	// Always centred at the origin.
	pRectangle->SetMajorAxes ( MajorAxis );			// The height of the rectangle.
	pRectangle->SetMinorAxes ( MinorAxis );			// The width of the rectangle.

	// Attempt to prepare the node for insertion into the tree.
	if ( !TransformNode ( pRectangle ) ||
		 !SetAttributesForNode ( pRectangle, &Rect.RenderAttributes ) ||
		 !ClipNode ( ( Node ** ) &pRectangle ) ||
		 !ApplyAttributesToNode ( pRectangle ) )
	{
		// A problem has been encountered, so return FALSE.
		return FALSE;
	}

	// Add the node to the tree.
	Data->pCurrentLayer->AddNode( pRectangle );

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Process_AddGlobalTransform(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a AddGlobalTransform command

********************************************************************************************/

BOOL CMXImportFilter::Process_AddGlobalTransform(void)
{
	// get the data from the file
	CI_READDATA(AddGlobalTransform, gt)

	// make a new transform stack object
	CMXImportTransform *pNewTrans = new CMXImportTransform(&gt.TransformMatrix, TRUE);
	if(pNewTrans == NULL)
		return FALSE;

	// add the new object to the stack and everything happy is done for us
	Data->TransformStack.Push(pNewTrans);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_SetGlobalTransfo(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a SetGlobalTransfo command

********************************************************************************************/

BOOL CMXImportFilter::Process_SetGlobalTransform(void)
{
	// get the data from the file
	CI_READDATA(SetGlobalTransform, gt)

	// make a new transform stack object
	CMXImportTransform *pNewTrans = new CMXImportTransform(&gt.TransformMatrix, FALSE);
	if(pNewTrans == NULL)
		return FALSE;

	// add the new object to the stack and everything happy is done for us
	Data->TransformStack.Push(pNewTrans);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_RestoreLastGlobalTransform(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a RestoreLastGlobalTransform command

********************************************************************************************/

BOOL CMXImportFilter::Process_RestoreLastGlobalTransform(void)
{
	// just need to pop the transform off the stack, and delete it
	// everything else just happens. Funny, isn't it?
	delete Data->TransformStack.Pop();

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_DrawImage(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a bitmap command

********************************************************************************************/

BOOL CMXImportFilter::Process_DrawImage(void)
{
	NEEDLAYER

	// right then, load our data from this silly thingy
	CI_READDATA(DrawImage, db)

	// check to see if it's a RImage type thing
	if(db.ImageType != cmxDRAWIMAGE_IMAGETYPE_COLOUR)
		return TRUE;		// can't deal with this

	// first of all, is it a cropped bitmap, 'cos it it is, we're going to have
	// to import it as a path with a bitmap fill on it
	if(db.ImageExtent != db.CroppingRectangle)
		return Process_DrawImage_Cropped(db);	// do a cropped one instead

	// right then, let's make one of these objects...
	INT32 Ref = db.ImageFileReference1 - 1;
	if(!Data->RImages[Ref].IsRImage())
		return TRUE;		// can't do that either without one of the RImages
	KernelBitmap *pBitmap = Data->RImages[Ref].GetBitmap(this);
	if(pBitmap == 0)
		return FALSE;		// wasn't converted, oh dear

	// create a bitmap object
	NodeBitmap *NewBitmap = new NodeBitmap;

	if((NewBitmap == NULL) || !NewBitmap->SetUpPath())
		return FALSE;
	
	NewBitmap->CreateShape(db.ImageExtent);

	// transform it by the enclosed matrix
 	Trans2DMatrix Transform(db.Transformation);
	NewBitmap->Transform(Transform);

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

	// put it on the right place on the page, but don't apply attributes to it
	Node *pNode = NewBitmap;
	if(!TransformNode(pNode)
		|| !ClipNode(&pNode))
		return FALSE;

	// add the node
	Data->pCurrentLayer->AddNode(pNode);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_DrawImage_Cropped(cmxiDrawImage &db)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a bitmap command

********************************************************************************************/

#define BITM_MOVE(px, py) {co.x = (px); co.y = (py); if(!pPath->InkPath.InsertMoveTo(co)) return FALSE;}
#define BITM_LINE(px, py) {co.x = (px); co.y = (py); if(!pPath->InkPath.InsertLineTo(co)) return FALSE;}
#define BITM_CLOSE {if(!pPath->InkPath.CloseSubPath()) return FALSE;}

BOOL CMXImportFilter::Process_DrawImage_Cropped(cmxiDrawImage &db)
{
	// get a nice bitmap to apply as a fill
	INT32 Ref = db.ImageFileReference1 - 1;
	if(!Data->RImages[Ref].IsRImage())
		return TRUE;		// can't do that either without one of the RImages
	KernelBitmap *pBitmap = Data->RImages[Ref].GetBitmap(this);
	if(pBitmap == 0)
		return FALSE;		// wasn't converted, oh dear

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
	BITM_MOVE(db.CroppingRectangle.lo.x, db.CroppingRectangle.lo.y)
	BITM_LINE(db.CroppingRectangle.hi.x, db.CroppingRectangle.lo.y)
	BITM_LINE(db.CroppingRectangle.hi.x, db.CroppingRectangle.hi.y)
	BITM_LINE(db.CroppingRectangle.lo.x, db.CroppingRectangle.hi.y)
	BITM_LINE(db.CroppingRectangle.lo.x, db.CroppingRectangle.lo.y)
	BITM_CLOSE

	// finish off the path
	pPath->InvalidateBoundingRect();
	pPath->InkPath.IsFilled = TRUE;

	// set up an attribute or two
	Data->ObjFilled = TRUE;

	// set up a nice attribute for this thingy
	if(!SetLineColour(DocColour(COLOUR_TRANS)))
		return FALSE;

	// set a nice bitmap fill
	DocCoord Start 	= DocCoord(db.ImageExtent.lo.x, db.ImageExtent.lo.y);
	DocCoord End1 	= DocCoord(db.ImageExtent.hi.x, db.ImageExtent.lo.y);
	DocCoord End2 	= DocCoord(db.ImageExtent.lo.x, db.ImageExtent.hi.y);
	if(!SetBitmapFill(pBitmap, Start, End1, End2))
		return FALSE;
	
	// apply attributes
	SetPathFilled(TRUE);
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;
	if(!AttributeManager::ApplyBasedOnDefaults(pPath, CurrentAttrs))
		return FALSE;
	
	// transform it by the enclosed matrix
 	Trans2DMatrix Transform(db.Transformation);
	pPath->Transform(Transform);

	// put it on the right place on the page, but don't apply attributes to it
	Node *pNode = pPath;
	if(!TransformNode(pNode)
		|| !ClipNode(&pNode))
		return FALSE;

	// add the node
	Data->pCurrentLayer->AddNode(pNode);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_AddClippingRegion(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a AddClippingRegion command

********************************************************************************************/

BOOL CMXImportFilter::Process_AddClippingRegion(void)
{
	CI_READDATA(AddClippingRegion, cr)

	// do we need to do anything?
	switch(cr.RegionType)
	{
	case cmxADDCLIPREGION_TYPE_NONE:
		return TRUE;		// nothing to do
		break;

	case cmxADDCLIPREGION_TYPE_POLYGON:
		break;				// know what to do with these

	default:
		TRACEUSER( "Neville", _T("---- thingy. type of clipping region not supported\n"));
		// The following wasn't here before 5/11/97, causes access violations on 611.cmx
		// as the pPolygon has not been allocated or has been deleted!
		// This is not suprising as the other types do not define it!
		return TRUE;		// nothing to do
		break;
	}

	// OK, we've got a polygon type path to deal with -- do we need to intersect or not
	BOOL Intersect = TRUE;
	if(cr.ClipMode == cmxADDCLIPREGIOM_MODE_COPY)
		Intersect = FALSE;

	// make a new stack item
	CMXImportClipper *pClipper = new CMXImportClipper(cr.Spec.pPolygon, Intersect);
	if(pClipper == NULL)
		return FALSE;		// error

	// and add it to the silly stack
	if(!Data->ClipperStack.Push(pClipper))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_RemoveLastClippingRegion(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a RemoveLastClippingRegion command

********************************************************************************************/

BOOL CMXImportFilter::Process_RemoveLastClippingRegion(void)
{
	// remove the last item from the clipper stack
	if(Data->ClipperStack.GetCount() > 0)
		delete Data->ClipperStack.Pop();

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_ClearClipping(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a ClearClipping command

********************************************************************************************/

BOOL CMXImportFilter::Process_ClearClipping(void)
{
	// go through removing all clipping
	while(Data->ClipperStack.GetCount() != 0)
	{
		delete Data->ClipperStack.Pop();
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Process_EndSection(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a EndSection command

********************************************************************************************/

BOOL CMXImportFilter::Process_EndSection(void)
{
	Data->EndSectionFound = TRUE;

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Process_(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a  command

********************************************************************************************/
/*
BOOL CMXImportFilter::Process_(void)
{
	return TRUE;
}
*/

/********************************************************************************************

>	BOOL CMXImportFilter::Process_BeginTextStream(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	process a text command, create a text story and set up its
				width.

********************************************************************************************/

BOOL CMXImportFilter::Process_BeginTextStream(void)
{
	CI_READDATA(BeginTextStream, text)
TRACEUSER( "Claude", _T("BeginTextStream"));	
	//	Take the top corner of the text's bbox as place to start the text
	
	DocCoord Pos = text.BBox.hi;//HighCorner();
	
	// create the empty text story
	Data->pTextStory = TextStory::CreateTextObject(Pos);


	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Process_TextFrame(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	Put the text in the right place (it was first in a CMX's default
				place) using 

********************************************************************************************/

BOOL CMXImportFilter::Process_TextFrame(void)
{
		TRACEUSER( "Claude", _T("TextFrame"));
	
	CI_READDATA(TextFrame, textf)
	
//	if(!Data->TextFrameRead)
//	{
		Data->TextFrameRead = TRUE;
	
		INT32 TextWidth = 70000;	//	The width of the text story

		for(INT32 i = 0 ; i < textf.columns.count ; i++)
		{
			TextWidth += textf.columns.columns[i];
			TextWidth += textf.columns.gutters[i];
		}


		//	Set the width of the text story. In MILLIPOINT!
		Data->pTextStory->SetImportFormatWidth((INT32)(TextWidth * Data->BaseScaleFactor));

		//	Set the matrix of the text story
		Data->pTextStory->SetStoryMatrix(textf.Mat);

	
		//	Clean up the place
		delete textf.columns.columns;
		delete textf.columns.gutters;
//	}
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_Characters(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	Put the characters in the story

********************************************************************************************/

BOOL CMXImportFilter::Process_Characters(void)
{

	CI_READDATA(Characters, fabbyChars)

	//	Create an empty line
	Data->pTextLine = Data->pTextStory->FindLastLine();
	SetNonTextAttributeIgnore();
	if (!AttributeManager::ApplyBasedOnDefaults(Data->pTextLine, CurrentAttrs))
	{
	 	delete Data->pTextLine;	
		Data->pTextLine = NULL;
	}
	SetNonTextAttributeNotIgnore();

	for(INT32 i = 0 ; i < fabbyChars.ch.count ; i++)
	{
	
		BOOL Ignore = FALSE;
		TRACEUSER( "Claude", _T("Character %d is %c\n"), i, fabbyChars.ch.characters[i]);

		if(fabbyChars.ch.characters[i] < 32)
		{
			if(fabbyChars.ch.characters[i] == 13)
			{	//	End of line to skip and create a new line
	
				//	Create an empty line
				Data->pTextLine = TextLine::CreateEmptyTextLine(Data->pTextStory, LASTCHILD);
				SetNonTextAttributeIgnore();
				if (!AttributeManager::ApplyBasedOnDefaults(Data->pTextLine, CurrentAttrs))
				{
	 				delete Data->pTextLine;
					Data->pTextLine = NULL;
				}
				SetNonTextAttributeNotIgnore();
				Ignore = TRUE;
			}
			else
				if(fabbyChars.ch.characters[i] == 9)
					fabbyChars.ch.characters[i] = 32;
				else
					//	Ignore the bad characters.
					Ignore = TRUE;
		}

		if(!Ignore)	
		{
			//	Use last text line
			VisibleTextNode* pLastNode = Data->pTextLine->FindLastVTN();	
			ERROR2IF(pLastNode == NULL, NULL, "Empty Story");
			//	Create a new TextChar
			VisibleTextNode* pNew = new TextChar(pLastNode, PREV, fabbyChars.ch.characters[i]);
			if (pNew != NULL)
			{
				
				//	UMASK used to determine if this character has any attributes
				//	different from default.
				//	If so set them
				INT32 UMASK	=	0;
				
				if(!CorelDraw7)
				{
					if(Data->NumCharInfo != 0)
						UMASK = Data->CharInfos[fabbyChars.ch.indices[i]].UsageMask;
				}
				else
				{
					if(fabbyChars.ch.indices[i] != cmxMaxValueWORD)
						UMASK = Data->CharInfos[fabbyChars.ch.indices[i]].UsageMask;
				}

				if(((UMASK & cmxUSAGEMASK_UseTypeNum) != 0) ||
					((UMASK & cmxUSAGEMASK_UseTypeStyle) != 0))
					
				{
					INT32 fontRef = Data->CharInfos[fabbyChars.ch.indices[i]].FontRef-1;
					String_64 name((TCHAR*)*Data->Fonts[fontRef].mFont);
		
					SetTextTypeFace(&name);

					SetTextItalic(Data->Fonts[fontRef].mIsItalic); 
					SetTextBold(Data->Fonts[fontRef].mIsBold);
				}

				if((UMASK & cmxUSAGEMASK_UseTypeSize) != 0)
					//	Set up the size of the font
					SetTextSize(Data->CharInfos[fabbyChars.ch.indices[i]].TypeSize);
			
				if(((UMASK & cmxUSAGEMASK_UseCharFill) != 0) ||
					((UMASK & cmxUSAGEMASK_UseCharOutline) != 0))
					//	Apply specific attributes
					SetAttributesForNode(pNew, &Data->CharInfos[fabbyChars.ch.indices[i]].RenderAttributes, FALSE);
				else
					//	Apply default attributes
					SetAttributesForNode(pNew, &Data->textRenderAttributes, FALSE);



				if (!AttributeManager::ApplyBasedOnDefaults(pNew, CurrentAttrs))
				{
			 		delete pNew;
					pNew = NULL;
				}

			
				//	Set the default back to what they were!
				String_64 name((TCHAR*)*Data->Fonts[Data->currentRef].mFont);
	
				SetTextTypeFace(&name);

				SetTextItalic(Data->Fonts[Data->currentRef].mIsItalic); 
				SetTextBold(Data->Fonts[Data->currentRef].mIsBold);
				SetTextSize(Data->currentSize);

			}

		}
	}
	

	//	Clean all up
	delete fabbyChars.ch.characters;
	delete fabbyChars.ch.indices;
	
	return TRUE;
}

/********************************************************************************************

>	BOOL CMXImportFilter::Process_CharInfo(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	Put the characters in the story

********************************************************************************************/

BOOL CMXImportFilter::Process_CharInfo(void)
{
	CI_READDATA(CharInfo, ci)

	//	Get the info from the CharInfo
	Data->NumCharInfo++;
	Data->CharInfos[Data->NumCharInfo-1].UsageMask			= ci.UsageMask;
	Data->CharInfos[Data->NumCharInfo-1].FontRef			= ci.FontRef;
	Data->CharInfos[Data->NumCharInfo-1].TypeSize			= ci.TypeSize;
	Data->CharInfos[Data->NumCharInfo-1].CharAngle			= ci.CharAngle;
	Data->CharInfos[Data->NumCharInfo-1].RenderAttributes	= ci.RenderAttributes;
	Data->CharInfos[Data->NumCharInfo-1].HShift				= ci.HShift;
	Data->CharInfos[Data->NumCharInfo-1].VShift				= ci.VShift;
	
	return TRUE;
}



/********************************************************************************************

>	BOOL CMXImportFilter::Process_BeginParagraph(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	Add the attributes to the text

********************************************************************************************/


BOOL CMXImportFilter::Process_BeginParagraph(void)
{
	SetTextAttributeNotIgnore();
	
	CI_READDATA(BeginParagraph, Paragraph)
	
	//	Get the correct font reference
	Data->currentRef	= Paragraph.FontRef-1;
	//	And font size
	Data->currentSize	= Paragraph.FontSize;

	//	Set the font
	String_64 name((TCHAR*)*Data->Fonts[Paragraph.FontRef-1].mFont);
	SetTextTypeFace(&name);

	//	Set the text attributes to whatever they need to be
	SetTextItalic(Data->Fonts[Paragraph.FontRef-1].mIsItalic); 
	SetTextBold(Data->Fonts[Paragraph.FontRef-1].mIsBold);
	SetTextSize(Paragraph.FontSize);
	
	switch(Paragraph.Justification)
	{

		case cmxNoAlignment:	//	Some Corel sillyness, put it Left Justified
								//	as it's what it looks like anyway.
			Data->textJustification = JLEFT;
			break;
		case cmxLeftJustification:
			Data->textJustification = JLEFT;
			break;
		case cmxCenterJustification:
			Data->textJustification = JCENTRE;
			break;
		case cmxRightJustification:
			Data->textJustification = JRIGHT;
			break;
		case cmxFullJustification:
			Data->textJustification = JFULL;
			break;
		case cmxForceJustification:
			Data->textJustification = JFULL;
			break;
		default: // no worries here.
			break;
	}

	SetTextJustification(Data->textJustification);
	

	
	//	Set the indentation for the text story
	Data->pTextStory->SetLeftIndent((INT32)(Paragraph.LeftMargin * Data->BaseScaleFactor));
	Data->pTextStory->SetRightIndent((INT32)(Paragraph.RightMargin * Data->BaseScaleFactor));
	
		
	//	Save the rendering attributes to use them latter
	Data->textRenderAttributes = Paragraph.RenderAttributes;
		
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_EndParagraph(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	end the paragraph, set the text attributes to be ignored again

********************************************************************************************/

BOOL CMXImportFilter::Process_EndParagraph(void)
{
	//	Set the text attributes back to be ignored
	SetTextAttributeIgnore();

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::Process_EndTextStream(void)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		None
	Returns:	error flag
	Purpose:	end the text thingy, puting things in the right place, and addig the node
				node.
********************************************************************************************/

BOOL CMXImportFilter::Process_EndTextStream(void)
{

	Data->TextFrameRead = FALSE;
	//	Make the text story as a node
	Node *pNode = Data->pTextStory;

	if(	!TransformNode(pNode) ||
		!ClipNode(&pNode))// || 
//		!ApplyAttributesToNode(pNode))	// Don't apply the attributes again because that's bad!
		return FALSE;
	
	// add the node
	Data->pCurrentLayer->AddNode(pNode);

	return TRUE;
}
