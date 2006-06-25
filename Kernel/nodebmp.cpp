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

// Node representing a bitmap object.

/*
*/

#include "camtypes.h"
#include "nodebmp.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"
//#include "rndrgn.h"
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
//#include "progress.h"
#include "cameleps.h"
#include "ai_epsrr.h"
#include "bitmpinf.h"
#include "nodepath.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "draginfo.h"
//#include "scrcamvw.h"
#include "cliptype.h"
#include "attrmap.h"

#include "colormgr.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "jason.h"
//#include "justin2.h"
//#include "sglfills.h"
//#include "xpehost.h"
#include "grndrgn.h"
//#include "matrix.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodeliveeffect.h"

// These are for CheckGreyscaleBitmap
#ifndef WEBSTER
//these should probably be #ifndef EXCLUDE_BFX rather than WEBSTER Martin-17/03/97
//they aren't used any more anyway 10/04/97
//#include "bfxalu.h"
//#include "bitmapfx.h"
#endif //WEBSTER
//#include "resource.h"
//#include "bmplist.h"
#include "bmpcomp.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "will.h"
#include "qualattr.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "will2.h"
//#include "capturemanager.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fthrattr.h"
#include "ophist.h"
#include "objchge.h"

// headers for the new file format
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxfnbmp.h"

// stuff for CMX rubbish
//#include "cmxrendr.h"
//#include "cmxexdc.h"

// for popup menu
#include "contmenu.h"
//#include "bfxdlg.h"
#include "tracedlg.h"

//This includes the Hot Link class
#include "userattr.h"
//#include "webattr.h"
#include "camview.h"

#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
#include "plugmngr.h"	// OPTOKEN_PLUGINS_MENU
#include "plugop.h"		// PlugInsContextMenu 
#endif

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(NodeBitmap, NodeRect);
#if !defined(EXCLUDE_FROM_RALPH)
CC_IMPLEMENT_DYNCREATE(OpCreateNodeBitmap, UndoableOperation);
#endif
CC_IMPLEMENT_DYNCREATE(ChangeBitmapPtrAction, Action);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	NodeBitmap::NodeBitmap(Node* ContextNode,  
						   AttachNodeDirection Direction,  
					       BOOL Locked, 
						   BOOL Mangled,  
						   BOOL Marked, 
						   BOOL Selected)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/08/94

    Inputs: 	ContextNode: Pointer to a node which this node is to be attached to.     
		
				Direction: 
			
					Specifies the direction in which the node is to be attached to the 
					ContextNode. The values this variable can take are as follows: 
								  
					PREV      : Attach node as a previous sibling of the context node
					NEXT      : Attach node as a next sibling of the context node
					FIRSTCHILD: Attach node as the first child of the context node
					LASTCHILD : Attach node as a last child of the context node                               
					      
				The remaining inputs specify the status of the node: 
			
				Locked:     Is node locked ?
				Mangled:    Is node mangled ?
				Marked:     Is node marked ?
				Selected:   Is node selected ?
			
    Purpose: 	This constructor initialises the nodes flags and links it to ContextNode in 
    			the direction specified by Direction. All neccesary tree links are updated.         
			
	Note:   	SetUpPath() must be called before the NodeBitmap is in a state in which it 
				can be used. 		
                
    SeeAlso: 	SetUpPath   
    
    Errors: 	An assertion error will occur if ContextNode is NULL

********************************************************************************************/

NodeBitmap::NodeBitmap(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ):NodeRect(ContextNode, Direction, Locked, Mangled, Marked, Selected )  
{                         
	Colour = COLOUR_NONE;
	EndColour = COLOUR_NONE;

	ApplyContoneColour = FALSE;
}                        
 

/********************************************************************************************

>	NodeBitmap::NodeBitmap()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/08/94
    Purpose: 	This constructor creates a NodeBitmap linked to no other with all status
		 	  	flags false and an uninitialized bounding rectangle.           
			
				NB. SetUpPath() must be called before the NodeBitmap is in a state in which 
					it can be used. 		
                
    SeeAlso: 	SetUpPath                                                        

********************************************************************************************/

NodeBitmap::NodeBitmap(): NodeRect()
{
	Colour = COLOUR_NONE;
	EndColour = COLOUR_NONE;

	ApplyContoneColour = FALSE;
}



/***********************************************************************************************

>	virtual Node* NodeBitmap::SimpleCopy()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeBitmap::SimpleCopy()
{
	// Make a new NodeBitmap and then copy things into it
	NodeBitmap* NodeCopy = new NodeBitmap();
    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}            


/***********************************************************************************************

>	void NodeBitmap::CopyNodeContents(NodeBitmap* NodeCopy)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		NodeCopy - The node to copy
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRect::CopyNodeContents

***********************************************************************************************/

void NodeBitmap::CopyNodeContents(NodeBitmap* NodeCopy)
{
	NodeRect::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here

	// Copy KernelBitmap reference
	NodeCopy->BitmapRef = BitmapRef;

	// We need to copy the cached bitmaps into the new node
	// But since this cached (option 2) bitmap is only ever used as an intermediate stage
	// in producing another cached bitmap, we probably don't need to copy it
	//CopyCached(pCopyOfNode, GetPixelWidth(), 2);	// Copy cached bitmaps, options 0, 1 and 2 (original and processed)
}




/***********************************************************************************************
>   void NodeBitmap::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBitmap::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBitmap), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBitmap))
		CopyNodeContents((NodeBitmap*)pNodeCopy);
}



/********************************************************************************************

>	virtual String NodeBitmap::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93
	Inputs:		Plural: Flag indicating if the string description should be plural or
				singular. 
	Returns:	Description of the object                                    
	Purpose:	To return a description of the NodeBitmap object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				The description will always begin with a lower case letter.   
	Errors:		A resource exception will be thrown if a problem occurs when loading the 
				string resource. 

********************************************************************************************/
              
String NodeBitmap::Describe(BOOL Plural, BOOL Verbose) 
{     
#if !defined(EXCLUDE_FROM_RALPH)
	if (Plural)
	{
		return(String(_R(IDS_BITMAP_DESCRP)));  
	}
	else
	{
		if (GetBitmap() == NULL || GetBitmap()->ActualBitmap == NULL)
			return(String(_R(IDS_BITMAP_DESCRS))); 

		// If we have a bitmap, then include it's name and actual dpi
		// in the desciption

		String_256 BmpName = GetBitmap()->ActualBitmap->GetName();
		String_64 TrunkName;
		if (BmpName.Length() > 12)
		{
			BmpName.Left(&TrunkName, 9);
			TrunkName += _T("...");
		}
		else
		{
			BmpName.Left(&TrunkName, 12);
		}

		String_256 DpiText;
		UINT32 dpi = GetBitmapDPI();

		if (dpi > 0)
		{
			String_256 jcf(_R(IDS_NODEBMP_DPI_FORMAT));
			camSnprintf(DpiText, 256, jcf, dpi);
		}

		String_64 TrunkDpi;
		DpiText.Left(&TrunkDpi, 11);

		String_256 Desc(_R(IDS_BITMAP_DESCRS));
		String_64 TrunkDesc;
		Desc.Left(&TrunkDesc, 6);

		if (Verbose)
		{
			String_32 ret = TrunkDesc;
			ret += TEXT(" '");
			ret += TrunkName;
			ret += TEXT("'");
			ret += TrunkDpi;
			return ret;
//			return(String(TrunkDesc+TEXT(" '")+TrunkName+TEXT("'")+TrunkDpi));
		}
		else
		{
			String_32 ret = TrunkDesc;
			ret += TEXT(" ");
			ret += TrunkName;
			return ret;
//			return(String(TrunkDesc+TEXT(" ")+TrunkName));
		}
	}
#else
	return(String(_R(IDS_BITMAP_DESCRP)));  
#endif
}

/********************************************************************************************

>	UINT32 NodeBitmap::GetBitmapDPI()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Returns:	The actual DPI of the bitmap.
	Purpose:	Caculates the actual DPI of the bitmap on the page.

********************************************************************************************/

UINT32 NodeBitmap::GetBitmapDPI()
{
	UINT32 Dpi = 0;

	if (GetBitmap() == NULL)
		return 0;

	// Do we have a valid bitmap ?
	OILBitmap *OilBM = GetBitmap()->ActualBitmap;

	if (OilBM != NULL)
	{
		BitmapInfo Info;
		OilBM->GetInfo(&Info);

		// Get the Width of the Bitmap in Pixels
		INT32 PixWidth  = Info.PixelWidth;
		INT32 PixHeight = Info.PixelHeight;

		// Get the Width of the Bitmap in Millipoints
		INT32 Width  = INT32(Parallel[0].Distance(Parallel[1]));
		INT32 Height = INT32(Parallel[1].Distance(Parallel[2]));

		// Use doubles so that we can round up as well as down. This improves
		// the dpi calculated.
		double HDpi = 0;
		double VDpi = 0;

		if (Width > 0)
			HDpi = ((double)PixWidth * 72000.0)/(double)Width;

		if (Height > 0)
			VDpi = ((double)PixHeight * 72000.0)/(double)Height;

		// Use the smaller of the two dpi values
		Dpi = (UINT32)(HDpi + 0.5);
		if (VDpi < Dpi)
			Dpi = (UINT32)(VDpi + 0.5);
	}

	return Dpi;
}

/********************************************************************************************

>	virtual UINT32 NodeBitmap::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Returns:	The size of the node in bytes 
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeBitmap::GetNodeSize() const 
{     
	return (sizeof(NodeBitmap)); 
}  



/********************************************************************************************

>	virtual BOOL NodeBitmap::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to pop up menu clicks on it (rather than its blobs).

********************************************************************************************/
BOOL NodeBitmap::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
#if !defined(EXCLUDE_FROM_RALPH)

	BOOL ok = TRUE;

PORTNOTE("liveeffects", "XPE menu option disabled")
#ifndef EXCLUDE_FROM_XARALX
	ok = ok && pMenu->BuildCommand(OPTOKEN_XPE_EDIT, FALSE);
#endif

	//ok = ok && pMenu->BuildCommand(OPTOKEN_BFXDLG, FALSE);

#ifdef PHOTOSHOPPLUGINS
// Only add in if required - general plug-in removal at present
	// Add a sub-menu tacked onto this which is the main PhotoShopContextMenu
	// we have real OpTokens and so can do it properly
	// Well, in fact we will ask the PhotoShopContextMenu to do it for us!
//	ok = ok && pMenu->BuildCommand(OPTOKEN_PLUGINS_UNDO_MENU, FALSE);
//	MenuItem* pMainRoot = pMenu->GetLastItem();
	MenuItem* pMainRoot = NULL;
	// Add a submenu which is the list of bitmap based plug-ins.
	// We have no need for specifying a document and a bitmap as this will be read
	// by the undoable op.
	// There should not be a separator and we should use the undoable versions
	// of the operations (hence FALSE, TRUE).
	ok = ok && PlugInsContextMenu::BuildMenu(NULL, NULL, pMenu, pMainRoot, FALSE, TRUE);
#endif	// PHOTOSHOPPLUGINS

	ok = ok && pMenu->BuildCommand(OPTOKEN_TRACEDLG, TRUE);

	return ok;

#else	// !defined(EXCLUDE_FROM_RALPH)

	return FALSE;

#endif	// !defined(EXCLUDE_FROM_RALPH)
}



/********************************************************************************************

>	void NodeBitmap::Render(RenderRegion* pRender)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Inputs:		Pointer to a render region
	Purpose:	Will render the bitmap contained within the object to the given render 
				region.

********************************************************************************************/

void NodeBitmap::Render(RenderRegion* pRender)
{
	// No need to render if we are going to supply directly to a capture
	Capture* pCapture = pRender->GetTopCapture();
	if (!(pCapture && pCapture->ConsumeDirectBitmap(this)))
	{
		// Render the transformed bitmap
		pRender->DrawTransformedBitmap(this);
	}
}  


/********************************************************************************************

>	void NodeBitmap::PreExportRender(RenderRegion*)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		Ignored
	Purpose:	Prevent the base class NodeRect::PreExportRender() function from exporting 
				anything to mark this as a rectangle object - because it's not; it's a 
				bitmap object.

********************************************************************************************/

void NodeBitmap::PreExportRender(RenderRegion*)
{
	// No action, and we want to stop the base class from exporting any 'rectangly' 
	// information that might confuse us.
}


/********************************************************************************************

>	BOOL NodeBitmap::ExportRender(RenderRegion* pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/09/94
	Inputs:		pRegion - the export render region to export to.
	Returns:	TRUE if the bitmap was exported ok;
				FALSE if not.
	Purpose:	Export a bitmap object to a file.
	Errors:		Usual disk/file errors.
	SeeAlso:	Filter::ExportBitmap

********************************************************************************************/

BOOL NodeBitmap::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSRenderRegion)))
	{
		// Get valid transparency fill for this object
		CamelotEPSRenderRegion *pCamelotRegion = (CamelotEPSRenderRegion *) pRegion;
		pCamelotRegion->GetValidPathAttributes();

		// Output bitmap object position and keyword...
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		DocCoord *Coords = InkPath.GetCoordArray();

// Bodge Code

// So this is some Bodge code to make Bitmaps in moulds, save themselves
// correctly if a Line colour is applied to the Mould.

// This line colour makes the Bitmap think it has been contoned, so we save out
// a contoned version, which is looks incorrect when re-loaded.

// To get around this problem, this code checks specifically for the situation,
// and forces the bitmap to be saved as  a non-contoned version.

		BOOL CanSaveAsContoned = TRUE;
		BOOL LineColourIsAppliedToMould = FALSE;

		if (GetBitmap() && GetBitmap()->GetBPP() <= 8)
		{
			// We only need to worry about this for < 8BPP
			NodeAttribute* pLineAttr;
	
			// So we find the applied Stroke Colour ...
			if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pLineAttr))
			{
				Node* pParent = pLineAttr->FindParent();
				
				// And if it has been applied to a Mould ...
				if (pParent && IS_A(pParent, NodeMould))
				{
					LineColourIsAppliedToMould = TRUE;

					// and there is no Flat Colour Applied to the Bitmap,
					// then this flag will be FALSE, stopping the bitmap
					// from being saved as contone.
					CanSaveAsContoned = (GetEndColour() != NULL);
				}
			}
		}

		if (CanSaveAsContoned && (GetStartColour() || GetEndColour()))
		{
			// Must be a contone bitmap

			// If either the start or end colour is NULL, we need
			// to output a dummy colour, because 'No_Colour' doesn't
			// get saved
			if (LineColourIsAppliedToMould || GetStartColour() == NULL)
			{
				DocColour StartCol;
				AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
													_R(IDS_BLACKNAME), &StartCol);

				if (*GetEndColour() == StartCol)
				{
					AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
														_R(IDS_WHITENAME), &StartCol);
				}

				// Output dummy start colour here
				if (StartCol.FindParentIndexedColour() == NULL)
				{
					// Unnamed colour
					PColourCMYK CMYK;
					StartCol.GetCMYKValue(&CMYK);
					pDC->OutputColour(&CMYK);
					pDC->OutputToken(_T("K"));
				}
				else
				{
					// Named colour
					pDC->OutputNamedColour(&StartCol);
					pDC->OutputToken(_T("X"));
				}

				pDC->OutputNewLine();
			}
			else if (GetEndColour() == NULL)
			{
				DocColour EndCol;
				AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
													_R(IDS_WHITENAME), &EndCol);

				if (*GetStartColour() == EndCol)
				{
					AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
														_R(IDS_BLACKNAME), &EndCol);
				}

				// Output dummy end colour here
				if (EndCol.FindParentIndexedColour() == NULL)
				{
					// Unnamed colour
					PColourCMYK CMYK;
					EndCol.GetCMYKValue(&CMYK);
					pDC->OutputColour(&CMYK);
					pDC->OutputToken(_T("k"));
				}
				else
				{
					// Named colour
					pDC->OutputNamedColour(&EndCol);
					pDC->OutputToken(_T("x"));
				}

				pDC->OutputNewLine();
			}
		
			// Co-ords first,
			for (INT32 i = 0; i <= 3; i++)
				pDC->OutputCoord(Coords[i]);

			// ...then the bitmap object token.
			pDC->OutputToken(_T("cbm"));
			pDC->OutputNewLine();

			// ...and then the bitmap data itself.
			pDC->GetParentFilter()->ExportBitmap(*GetBitmap());
		}
		else
		{
			// It's not a contone bitmap.
			// Oh bum. We've made a bit of a bo-bo with these.
			// Since we don't save 'no-colour' we don't know that
			// the bitmap has no-colour when re-loaded, and so it
			// uses the 'current' fill colour, so sometimes the bitmaps
			// come out coloured, when they shouldn't be.

			// Solution .... We'll save this bitmap as a bitmap fill
			// (which already has a non-contone format), and save out
			// some extra stuff to tell the import routines that it's
			// not a bitmap fill, but really a bitmap object.
			// This means it will look correct in the Viewer (they'll
			// be loaded as bitmap filled paths), but will appear as 
			// bitmaps when loaded into the main program.

			// So first we'll output a new eps object (which will be Ignored by
			// the viewer), telling the main program that the following
			// object is really a bitmap object
						
			pDC->OutputValue((INT32) EOTAG_BITMAPFLAGS);
			pDC->OutputToken(_T("cso"));	// Start of new object type

			pDC->OutputToken(_T("cbot"));	// Flag the next bitmap fill as an object

			pDC->OutputToken(_T("ceo"));	// End of new object type
			pDC->OutputNewLine();

			// Now output the Bitmap fill
			pDC->OutputCoord(Coords[3]);
			pDC->OutputCoord(Coords[2]);
			pDC->OutputCoord(Coords[0]);

			pDC->OutputValue((INT32) CAMEPS_FILL_BITMAP);
			pDC->OutputToken(_T("cax"));
			pDC->OutputNewLine();

			// ...and then the bitmap data itself.
			pDC->GetParentFilter()->ExportBitmap(*GetBitmap());

			// Now output a filled path for the Viewer to render
			// This will be skipped over by the new import code
			pDC->OutputCoord(Coords[0]);
			pDC->OutputToken(_T("m"));
			pDC->OutputNewLine();
			pDC->OutputCoord(Coords[1]);
			pDC->OutputToken(_T("l"));
			pDC->OutputNewLine();
			pDC->OutputCoord(Coords[2]);
			pDC->OutputToken(_T("l"));
			pDC->OutputNewLine();
			pDC->OutputCoord(Coords[3]);
			pDC->OutputToken(_T("l"));
			pDC->OutputNewLine();
			pDC->OutputCoord(Coords[0]);
			pDC->OutputToken(_T("l"));
			pDC->OutputNewLine();
			pDC->OutputToken(_T("F"));	// Filled path
			pDC->OutputNewLine();
		}
		
		// Now force the Fill Colour, Line Colour, and Line Width
		// attribute to output themselves again, 'cus old versions
		// (including the Viewer) will corrupt these attributes for 
		// any subsequent objects

		// Get rid of the current copies of the last output attrs
		delete pCamelotRegion->LastOutputAttrs[ATTR_FILLGEOMETRY].pAttr;
		delete pCamelotRegion->LastOutputAttrs[ATTR_STROKECOLOUR].pAttr;
		delete pCamelotRegion->LastOutputAttrs[ATTR_LINEWIDTH].pAttr;
		
		// NULL the pointers
		pCamelotRegion->LastOutputAttrs[ATTR_FILLGEOMETRY].pAttr = NULL;
		pCamelotRegion->LastOutputAttrs[ATTR_STROKECOLOUR].pAttr = NULL;
		pCamelotRegion->LastOutputAttrs[ATTR_LINEWIDTH].pAttr = NULL;

		// and clear the Temp flags so we don't try and delete them on exit
		pCamelotRegion->LastOutputAttrs[ATTR_FILLGEOMETRY].Temp = FALSE;
		pCamelotRegion->LastOutputAttrs[ATTR_STROKECOLOUR].Temp = FALSE;
		pCamelotRegion->LastOutputAttrs[ATTR_LINEWIDTH].Temp = FALSE;

		// Re-output the attributes we've just reset
		pCamelotRegion->GetValidPathAttributes();

		// Tell caller we rendered ourselves ok
		return TRUE;
	}
PORTNOTE("cmx", "Disabled CMXRenderRegion")
#ifndef EXCLUDE_FROM_XARALX
	else if (pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *)pRegion->GetRenderDC();
		ERROR2IF(pDC == NULL, FALSE, "no dc in CMXRenderRegion");
		KernelBitmap *pBittyThingy = BitmapRef.GetBitmap();
		ERROR2IF(pBittyThingy == NULL, FALSE, "no bitmap in the node");

		pDC->WriteBitmap ( pBittyThingy, ( DocCoord * ) ( &Parallel ),
						   CMXExportDC::CMXBITMAPCOLOURSOURCE_LINEANDFILL );

		return TRUE;
	}
#endif
	else if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( AIEPSRenderRegion ) ) )
	{
		// Grab a pointer to the AIEPS_RenderRegion.
		AIEPSRenderRegion	*pAIEPSRR	= static_cast<AIEPSRenderRegion*> ( pRegion );

		// Write the bitmap using the render region's custom code.
		return pAIEPSRR->ExportBitmap ( this );
	}

#endif
	// Render this node in the normal way
	return FALSE;
}


/********************************************************************************************

>	INT32 NodeBitmap::GetSizeOfExport(Filter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pFilter - the filter that will be used to export this node.
	Returns:	The number of 'nodes' this bitmap is equivalent to (in size).
	Purpose:	Determine how many 'nodes' this bitmap is equivalent to.  We use the
				rough approximation that each scanline taks the same amount of space as
				a typical node.  This gives us enough granularity for a reasonably
				accurate and smooth progress display update.
	SeeAlso:	-

********************************************************************************************/

INT32 NodeBitmap::GetSizeOfExport(Filter *pFilter)
{
#ifdef DO_EXPORT
	// Are we going to save the bitmap out as part of this object?
	if (pFilter->GetBitmapSupportLevel() != SimpleBitmapSupport)
		// No - just one node
		return 1;

	// Find out how many scanlines there are, and return as the result.
	KernelBitmap *pBitmap = GetBitmap();
	BitmapInfo Info;
	if (!pBitmap->ActualBitmap->GetInfo(&Info))
		// something has gone wrong!
		return 1;

	// Return number of scanlines.
	return Info.PixelHeight;
#else
	return 0;
#endif
}

/********************************************************************************************

>	BOOL NodeBitmap::RequiresAttrib(NodeAttribute* pAttrib, 
									BOOL Search = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95
	Inputs:		pAttrib - specifies the attribute being enquired about.
				Search - See NodeRenderableInk::RequiresAttrib (Has no effect in this fn) 
	Returns:	TRUE => object supports this kind of attribute.
				FALSE => object does not support this kind of attribute, and so it
						 should not be applied.
	Purpose:	Determine which attributes a bitmap object supports a particular type of
				attribute.   A bitmap only supports transparent fill geometries, 
				mappings, and effects - all other attributes are rejected.
	SeeAlso:	NodeRenderableInk::RequiresAttrib

********************************************************************************************/

BOOL NodeBitmap::RequiresAttrib(NodeAttribute* pAttrib, BOOL Search /*= FALSE*/)
{
	// Sanity checks
	ERROR2IF(pAttrib == NULL, FALSE, "Pointer is null in NodeBitmap::RequiresAttrib");

	if ((pAttrib->IS_KIND_OF(AttrColourChange) ||
		 pAttrib->IS_KIND_OF(AttrStrokeColourChange)))
	{
		// Someone's trying to apply a contone colour
		// Remember this, so we can do a conversion in the 
		// AllowOp routine.
		ApplyContoneColour = TRUE;

	 	// Just say yes
	 	return TRUE;
	}

	ApplyContoneColour = FALSE;

	// Is it one we want?
	return ((pAttrib->IsAColourFill() && pAttrib->IsAFlatFill()) ||
			 pAttrib->IsAStrokeColour() ||
			 pAttrib->IsATranspFill() ||
			 pAttrib->IS_KIND_OF(AttrValueChange) ||
			 pAttrib->IS_KIND_OF(AttrColourDrop) ||
			 pAttrib->IS_KIND_OF(AttrTranspFillMapping) ||
			 pAttrib->IS_KIND_OF(AttrFillEffect) ||
			 pAttrib->IS_KIND_OF(AttrFillMapping) ||
PORTNOTE("other","Removed AttrWebAddress")
#ifndef EXCLUDE_FROM_XARALX
			 pAttrib->IS_KIND_OF(AttrWebAddress) ||				//Added by Graham 1/9/96, so bitmaps can have HotLinks
#endif
			 pAttrib->IS_KIND_OF(AttrUser)
			);
}

/********************************************************************************************

>	BOOL NodeBitmap::ApplyDefaultBitmapAttrs(UndoableOperation* pOp,
											 AttrBitmapTranspFill* pTranspBitmap = NULL)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		pOp - the undoable operation in which this Node has been created.
				pTranspBitmap - optional bitmap to apply as transparency. (Default = NULL)
	Returns:	TRUE => Default attribute where applied ok.
				FALSE => Apply failed.
	Purpose:	Apply the default attributes that a NodeBitmap needs.
				This is specific to Bitmap Nodes, which need the Start and End colours
				to be NONE initially, and also need to ensure the line width is 0.
				There is now an optional pointer to a transparent bitmap fill to be applied
				over the current bitmap node. This is used by the transparent GIF import code.
				

********************************************************************************************/

/* Note by Will 12/1/96

	Removed Line transparency attr (AttrStrokeTransp), because this is no longer needed,
	and caused a bug where outlines disappeared when transparency is turned on (eg. after 
	slicing a bitmap and applying a line colour). It is no longer required because the 
	rendering code now looks at the line colour, rather than relying on the Line
	transparency attr being present.

*/

BOOL NodeBitmap::ApplyDefaultBitmapAttrs(UndoableOperation* pOp,
										 AttrBitmapTranspFill* pTranspBitmap)
{
	if (pTranspBitmap != NULL)
	{
		pTranspBitmap->AttachNode(this, FIRSTCHILD);

		// Apply it as a single tiled fill. Otherwise in the GIF transparency case
		// the repeating can give ghost pixels along the edges.
		AttrTranspFillMappingLinear *pNoRepeatAttr = new AttrTranspFillMappingLinear;
		if (pNoRepeatAttr == NULL)
			return FALSE;

		pNoRepeatAttr->Value.Repeat = 1;

		pNoRepeatAttr->AttachNode(this, FIRSTCHILD);
	}

	Node* pLineColAttr = new AttrStrokeColour();
	if (pLineColAttr == NULL)
		return FALSE;

/*
	Node* pLineTranspAttr = new AttrStrokeTransp();
	if (pLineTranspAttr == NULL)
	{
		delete pLineColAttr;
		return FALSE;
	}
 */

	Node* pFillColAttr = new AttrFlatColourFill();
	if (pFillColAttr == NULL)
	{
		delete pLineColAttr;
//		delete pLineTranspAttr;
		return FALSE;
	}

	Node* pLineWidthAttr = new AttrLineWidth();
	if (pFillColAttr == NULL)
	{
		delete pLineColAttr;
//		delete pLineTranspAttr;
		delete pFillColAttr;
		return FALSE;
	}

	DocColour			colorNull(COLOUR_NONE);
	((AttrFillGeometry*)pLineColAttr)->SetStartColour( &colorNull );
	((AttrFillGeometry*)pFillColAttr)->SetStartColour( &colorNull );
	((AttrLineWidth*)pLineWidthAttr)->Value.LineWidth = 0;

//	UINT32 LineTransp = 255;
//	((AttrFillGeometry*)pLineTranspAttr)->SetStartTransp(&LineTransp);
//	((AttrFillGeometry*)pLineTranspAttr)->SetTranspType(1);

	pLineColAttr->AttachNode(this, FIRSTCHILD);
//	pLineTranspAttr->AttachNode(this, FIRSTCHILD);
	pFillColAttr->AttachNode(this, FIRSTCHILD);
	pLineWidthAttr->AttachNode(this, FIRSTCHILD);

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL NodeBitmap::CanBecomeA(BecomeA* pBecomeA)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/95
	Inputs:		pClass: The class of object
				pNumObjects = ptr to place number of objects of type pClass that will be created (Note: can be NULL).
							  *pNumObects in undefined on entry
	Returns:	TRUE if the node, or any of its children can transmogrify themselves to become 
				an InkClass object
	Purpose:	This function is used by the convert to shapes operation. It determines if 
				the node or any of its children can convert themselves into an InkClass object.
				In the case of a node bitmap we must stop it from it having the make shapes
				applied as otherwise we will loose the bitmap from view.
				NodeSimpleShape overrides the baseclass function to return True.

				The number you put into pNumObjects (if it's not NULL) should exactly equal the total number
				of pClass objects you create.  It should NOT contain any additional objects you may produce
				such as group objects for containing the pClass object, or attributes.

				Also, the entry value of *pNumObjects cannot be assumed to be 0.

********************************************************************************************/

BOOL NodeBitmap::CanBecomeA(BecomeA* pBecomeA)
{
	// The NodeBitmap can become a NodePath	(with a bitmap fill)
	if (pBecomeA->BAPath())
	{
		pBecomeA->AddCount(1);
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL NodeBitmap::DoBecomeA(CCRuntimeClass* InkClass, UndoableOperation* pOp) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory

	Purpose:	Transforms the object into another type of object. 
				Note: changed 7/10/94 by MarkN to take the pBecomeA param, so that more data
				could be passed	to these functions in the future without causing massive
				rebuilds due to the editing of node.h
	SeeAlso:	NodeSimpleShape::CanBecomeA

********************************************************************************************/

BOOL NodeBitmap::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("pBecomeA is NULL"));

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	// pBecomeA->Reason is one that we understand.

	UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();
	BOOL 		Success = TRUE;			// Our success flag (Important that this defaults to TRUE)
	NodePath* 	pNewNodePath = NULL;	// Ptr to a new NodePath, if we get to make one.

	if (pBecomeA->BAPath())
	{
		// We need to create a new NodePath, no matter what the reason.
		
		// Allocate a new NodePath node
		ALLOC_WITH_FAIL(pNewNodePath, (new NodePath), pBecomeA->GetUndoOp()); 
		Success = (pNewNodePath != NULL);

		// Initialise the path
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.Initialise(InkPath.GetNumCoords(),12), pBecomeA->GetUndoOp(), Success);
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.CopyPathDataFrom(&InkPath), pBecomeA->GetUndoOp(), Success);
		
		if (Success) pNewNodePath->InkPath.IsFilled = TRUE;

		// If Success is TRUE, then we now have a new NodePath object that contains this shape's path

		if (Success)
		{
			// Remember the contone colours before we go and hide the applied attributes
			DocColour* ContoneStart = GetStartColour();
			DocColour* ContoneEnd   = GetEndColour();
		
		 	switch (pBecomeA->GetReason())
			{
		 		case BECOMEA_REPLACE :
				{
					// It's a BECOMEA_REPLACE, so replace this node with the new NodePath in an undoable way

					// Can't do it in an undoable way without an Undo Op
//					ERROR2IF_PF(pBecomeA->GetUndoOp() == NULL,FALSE,("GetUndoOp() returned NULL"));

					// The NodeBitmap uses the Fill and line colours to set it's
					// contone colours.
					// But the bitmap fill doesn't need them, so we will Hide
					// any of these attributes that we find	applied
					Node* pChild = FindFirstChild();
					while (pChild != NULL)
					{
						Node* pThisChild = pChild;
						pChild = pChild->FindNext();	// Find next before we hide this child
												
						if (pThisChild->IsAnAttribute())
						{
							NodeAttribute* pAttr = (NodeAttribute*)pThisChild;

							if (pAttr->IsAColourFill() ||
								pAttr->IsAStrokeColour() )
							{
								if (pUndoOp)
								{
									// Hide and Fill or Line colour attributes
									NodeHidden* pNodeHidden; 
									Success = pBecomeA->GetUndoOp()->DoHideNode(pAttr, TRUE, &pNodeHidden);
								}
								else
								{
									pAttr->CascadeDelete();
									delete pAttr;
								}
							}
						}
					}

					// Now make a Bitmap Fill that is equivalent to this NodeBitmap
					AttrFillGeometry* pBitmapFill;
					ALLOC_WITH_FAIL(pBitmapFill , new AttrBitmapColourFill(), pBecomeA->GetUndoOp());
					
					if (pBitmapFill)
					{
						pBitmapFill->AttachBitmap(GetBitmap());

						// Set any contone colours that we have
						pBitmapFill->SetStartColour(ContoneStart);
						pBitmapFill->SetEndColour(ContoneEnd);

						// Set the control points from the NodeBitmap Parallelogram
						pBitmapFill->SetStartPoint(&Parallel[3]);
						pBitmapFill->SetEndPoint(&Parallel[2]);
						pBitmapFill->SetEndPoint2(&Parallel[0]);
						
						pBitmapFill->AttachNode(this, FIRSTCHILD);

						if (pUndoOp)
						{
							// Create a hide node action to hide the node when we undo 
							HideNodeAction* UndoHideNodeAction;     
							Success = (HideNodeAction::Init(pBecomeA->GetUndoOp(),
										  			 pBecomeA->GetUndoOp()->GetUndoActionList(),
								 					 pBitmapFill, 
								 					 TRUE, 		 // Include subtree size 
						  		 					 ( Action**)(&UndoHideNodeAction))
						  		  					 != AC_FAIL);
						}
					}

					// We also need to make sure it has no line colour
					AttrFillGeometry* pStrokeColour;
					ALLOC_WITH_FAIL(pStrokeColour , new AttrStrokeColour(), pBecomeA->GetUndoOp());
					
					if (pStrokeColour)
					{
						// Ensure the line colour is set to NONE
						DocColour NoCol = COLOUR_NONE;
						pStrokeColour->SetStartColour(&NoCol);
						
						pStrokeColour->AttachNode(this, FIRSTCHILD);

						if (pUndoOp)
						{
							// Create a hide node action to hide the node when we undo 
							HideNodeAction* UndoHideNodeAction;     
							Success = (HideNodeAction::Init(pBecomeA->GetUndoOp(),
										  			 pBecomeA->GetUndoOp()->GetUndoActionList(),
								 					 pStrokeColour, 
								 					 TRUE, 		 // Include subtree size 
						  		 					 ( Action**)(&UndoHideNodeAction))
						  		  					 != AC_FAIL);
						}
					}

					// Copy the node's attributes
					CALL_WITH_FAIL(CopyChildrenTo(pNewNodePath), pBecomeA->GetUndoOp(), Success); 

					// Insert the new NodePath into the tree, next to this node
					pNewNodePath->AttachNode(this,NEXT);

					if (Success)
					{
						// Set the bounds  
						pNewNodePath->InvalidateBoundingRect();
						pNewNodePath->SetSelected(IsSelected());

						if (pUndoOp)
						{
							// Create a hide node action to hide the node when we undo 
							HideNodeAction* UndoHideNodeAction;     
							Success = (HideNodeAction::Init(pBecomeA->GetUndoOp(),
										  			 pBecomeA->GetUndoOp()->GetUndoActionList(),
								 					 pNewNodePath, 
								 					 TRUE, 		 // Include subtree size 
						  		 					 ( Action**)(&UndoHideNodeAction))
						  		  					 != AC_FAIL);
						}
					}

					if (Success)
					{
						CCAttrMap AttrMap;
						CCAttrMap* pAttrMap = NULL;
						BOOL bFoundAttrs = pNewNodePath->FindAppliedAttributes(&AttrMap);
						if (bFoundAttrs)
							pAttrMap = AttrMap.Copy();
						pBecomeA->PassBack(pNewNodePath, this, pAttrMap);
					}

					// Now hide this NodeBitmap
					if (pUndoOp)
					{
						NodeHidden* pNodeHidden; 
						Success = pBecomeA->GetUndoOp()->DoHideNode(this, TRUE, &pNodeHidden);
					}
					else
					{
						CascadeDelete();
						delete this;		// Scary!
					}
				}
				break;

				case BECOMEA_PASSBACK :
				{
					// First find all attributes applied to this node...
					// 30 is a default value - this will grow if it needs more space
					CCAttrMap* pAttribMap = new CCAttrMap(30);
					CCAttrMap* pCopyOfAttrMap = NULL;

					// Now find any attributes that are applied to this node.
					/*BOOL FoundAttrs =*/ FindAppliedAttributes(pAttribMap);

//					size_t AttrCount = FoundAttrs ? pAttribMap->GetCount() : 30;

					// Now make a copy of the applied attributes map
					pCopyOfAttrMap = pAttribMap->Copy ();//new CCAttrMap(AttrCount);
					if (pCopyOfAttrMap != NULL)
					{			
						// We've made a copy of the Attr map now, so we don't need
						// the old one anymore
						delete pAttribMap;
					 
					 	// Replace the Fill and Line Colour attributes
						// in the Attr Map copy

						AttrFillGeometry* pBitmapFill = new AttrBitmapColourFill();
						if (pBitmapFill)
						{
							// Make a Bitmap Fill that is equivalent to this
							// node bitmap
							pBitmapFill->AttachBitmap(GetBitmap());

							// Set any contone colours that we have
							pBitmapFill->SetStartColour(ContoneStart);
							pBitmapFill->SetEndColour(ContoneEnd);

							// Set the control points from the NodeBitmap Parallelogram
							pBitmapFill->SetStartPoint(&Parallel[3]);
							pBitmapFill->SetEndPoint(&Parallel[2]);
							pBitmapFill->SetEndPoint2(&Parallel[0]);

							void* pOldFill;
							if( pCopyOfAttrMap->Lookup( CC_RUNTIME_CLASS(AttrFillGeometry), pOldFill ) )
							{
								// We need to Remove and Delete the existing Fill Attr
								pCopyOfAttrMap->RemoveKey( CC_RUNTIME_CLASS(AttrFillGeometry) );
								delete (AttrFillGeometry*)pOldFill;
							}

							// Add the Bitmap fill into the Attr Map
							pCopyOfAttrMap->SetAt( CC_RUNTIME_CLASS(AttrFillGeometry), (void*)pBitmapFill );
						}

						AttrFillGeometry* pStrokeColour = new AttrStrokeColour();
						if (pStrokeColour)
						{
							// Ensure the line colour is set to NONE
							DocColour NoCol = COLOUR_NONE;
							pStrokeColour->SetStartColour(&NoCol);

							void* pOldStroke;
							if( pCopyOfAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), pOldStroke ) )
							{
								// We need to Remove and Delete the existing Stroke Colour Attr
								pCopyOfAttrMap->RemoveKey( CC_RUNTIME_CLASS(AttrStrokeColour) );
								delete (AttrFillGeometry*)pOldStroke;
							}

							// Add the new Stroke Colour into the Attr Map
							pCopyOfAttrMap->SetAt( CC_RUNTIME_CLASS(AttrStrokeColour), (void*)pStrokeColour );
						}
					}

					Success = pBecomeA->PassBack(pNewNodePath, this, pCopyOfAttrMap);
				}
				break;

				case BECOMEA_TEST:
					// Do nothing
				break;

				default:
				{
					ERROR3("Unhandled BecomeA type in NodeBitmap::DoBecomeA");
				}
			}
		}
	}

	if (!Success)
	{
		if (pNewNodePath != NULL)
		{
			// Delete all the NodePath's children (if it has any) and unlink it from the tree (if it's linked)
			// This is all done by CascadeDelete()
			pNewNodePath->CascadeDelete(); 
			delete pNewNodePath;
			pNewNodePath = NULL;
		}
	}

	return Success;
}

/********************************************************************************************

> 	virtual BOOL NodeBitmap::AllowOp(ObjChangeParam* pParam,BOOL SetOpPermissionState = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/02/95
	Inputs:		pParam				 = describes the way an op wants to change the node
				SetOpPermissionState = if TRUE the Op permission state of this node will be set according to
										the outcome of the call
	Outputs:	-
	Returns:	TRUE means the node and all its parents are happy with this op, FALSE means don't do it
	Purpose:	This is the way to ask a node if you can do an op to it.

				The ObjChangeParam class contains flags that describe how it will change the node

				For example, the op could change the node's appearence (e.g. attr application, editing a path), 
				replace the node with another node (e.g. because it uses hidden nodes to hide the original and put another
				node in its place, or "make shapes"), delete the node (e.g. the delete and cut ops), etc.

				This function gives the node a chance to say NO.  It also gives the parents a chance to say no too.
				E.g. a blend node will allow itself to be deleted, but it will NOT allow a child of itself to be deleted).

				This call should only be made on selected, or parents of selected, nodes.  It makes a decision as a 
				straight node if it is selected.  It makes a decision as a parent if it has selected children.

				E.g. NodeBlend::AllowOp(...op delete...)
					if the node is selected, then it will return TRUE (parents permitting), i.e. I can be deleted
					if the node is a parent of selected it will return FALSE (i.e. can't delete children of blends).

				So when the node is selected, you are asking the node if you can do the op to it.  When the node is
				a parent of a selected node, you are asking if you can do the op to one of its children.

				********

				If the 'SetOpPermissionState' param is TRUE, the following indented lines applies:
					The node's op permission state is set according to the result of this function.

					If TRUE  is returned, then the node's op permission state will be left unchanged.
						AND the parent's op permission state will be set to PERMISSION_ALLOWED

					if FALSE is returned, then the node's op permission state will be PERMISSION_DENIED,
						AND all it's parents (up to the layer) will be set to PERMISSION_DENIED

					Also, all parents of this node are called via their AllowOp() func with the same state 
					as this node.  This means that after this call, you can guarantee that all of its parents will
					have either a PERMISSION_DENIED or PERMISSION_ALLOWED state.

					Note: Even if this node tries to set all it's parents to have a PERMISSION_DENIED state, if any
					of its parents have previously been set to PERMISSION_ALLOWED they will remain in that state (see
					SetOpPermission()). Why? Well, it is possible for a parent node to have one child with a 
					PERMISSION_DENIED and another child with a PERMISSION_ALLOWED.  It this state the parent MUST be
					in state PERMISSION_ALLOWED, because at least one of its children will allow the op to happen to it.

					So, after this call:
						 	The op permission state for this node will be either left unchanged (and
							therefore remain PERMISSION_UNDEFINED), or PERMISSION_DENIED.

							The parent's op permission state will be either PERMISSION_ALLOWED, or PERMISSION_DENIED.

					This is so UndoableOperation::UpdateChangedNodes() will only call OnChildChange() on parent nodes,
					because it only calls that func for nodes that have an op permission state of PERMISSION_ALLOWED.

				********

	SeeAlso:	GetOpPermission(),SetOpPermission();

********************************************************************************************/

BOOL NodeBitmap::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState)
{
	ERROR2IF(pParam==NULL,FALSE,"Node::AllowOp() - pParam==NULL");

	// if not called by a child AllowOp(), ensure AllowOp() called for all nodes in compound nodes,
	if (pParam->GetDirection()!=OBJCHANGE_CALLEDBYCHILD)
	{
		BOOL AnyAllowed=AllowOp_AccountForCompound(pParam, SetOpPermissionState);
		// if called by a parent, just pass this result back
		if (pParam->GetDirection()==OBJCHANGE_CALLEDBYPARENT)
			return AnyAllowed;
	}
	else
	{
		// clean out the calling-child ptr, so it doesn't get passed around unintentionally.
		pParam->SetCallingChild(NULL);
	}

	// at this point we must have been called directly by the op or via a child AllowOp()

	// decide if we allow it ... 
	BOOL allowed=TRUE;
	ObjChangeFlags Flags = pParam->GetChangeFlags();

	if (Flags.Attribute)
	{
		// Someone is applying an attribute .....
		// We really need to check the actual attribute, so for
		// now we'll rely on the fact that the 'RequiresAttrib()'
		// function will have been called before this one.

		// If we are applying a contone colour, then make sure we
		// are a Grey level bitmap, and give the user the option
		// of making one if not.
		if (ApplyContoneColour)
		{
			// 'ApplyContoneColour' is set in the 'RequiresAttrib()'
			// function if a contone colour change attr is applied.
			allowed = MakeContoneBitmap(pParam->GetOpPointer());		
		}		
	}

	// if we allowed it, see if our parents do ...
	if (allowed && Parent!=NULL)
	{
		ObjChangeDirection OldDirection=pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed=Parent->AllowOp(pParam,SetOpPermissionState);
		pParam->SetDirection(OldDirection);
	}

	// if setting permisions ...
	if (SetOpPermissionState)
	{
		// if allowed, mark parent accordingly, else mark child as denied and update parents
		if (allowed)
			Parent->SetOpPermission(PERMISSION_ALLOWED);
		else
			SetOpPermission(PERMISSION_DENIED,TRUE);
	}

	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}

/********************************************************************************************

>	BOOL NodeBitmap::MakeContoneBitmap(UndoableOperation* pOperation)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Purpose:	Make this NodeBitmap into a grey level version of itself.

********************************************************************************************/

BOOL NodeBitmap::MakeContoneBitmap(UndoableOperation* pOperation)
{
	KernelBitmap* pBitmap = GetBitmap();
	KernelBitmap* pGreyBmp = CheckGreyscaleBitmap(pBitmap, _R(IDS_MAKEGREY), _R(IDS_DOGREY));

	if (pGreyBmp == NULL)
		return FALSE;	// Failed in some way (or the user cancelled)

	return TRUE;
}

/********************************************************************************************

>	static KernelBitmap* NodeBitmap::CheckGreyscaleBitmap(	KernelBitmap* pBitmap, 
															UINT32 PromptID, UINT32 OkID)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Purpose:	Checks a bitmap to see if can be applied as a texture,
				and gives the option to create a grey level version if not.

********************************************************************************************/

KernelBitmap* NodeBitmap::CheckGreyscaleBitmap(KernelBitmap* pBitmap, UINT32 PromptID, UINT32 OkID)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pBitmap == NULL, NULL, "Null bitmap passed to CheckGreyscaleBitmap");

	// Don't need to do anything here now, as the greyscale bitmap is generated on the
	// fly when rendering
	
	return pBitmap;
#else
	ERROR3("NodeBitmap::CheckGreyscaleBitmap");
	return NULL;
#endif
}
		   
/********************************************************************************************

>	KernelBitmap* NodeBitmap::GetBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Purpose:	Get the bitmap referenced by this node. 

********************************************************************************************/

KernelBitmap* NodeBitmap::GetBitmap()
{
	KernelBitmap* pBitmap = BitmapRef.GetBitmap();
	
	// Check for a deleted bitmap
	if (pBitmap && pBitmap->HasBeenDeleted())
	{
		ERROR2IF(pBitmap->GetParentBitmapList() == NULL, NULL, "Deleted bitmap has no parent list");
		
		// Use the default bitmap instead
		pBitmap = pBitmap->GetParentBitmapList()->FindDefaultBitmap();

		// There should always be a default bitmap in the list
		ERROR2IF(pBitmap == NULL, 0L, "Couldn't find the default bitmap");
	}

	return pBitmap;
}

/***********************************************************************************************

>    virtual BOOL NodeBitmap::HidingNode()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   11/1/94
	 Returns:   TRUE if all was ok. FALSE if an error occured.
     Purpose:   This virtual function is called whenever the node is hidden.
	 			It allows the node do things like 'optimise' itself to use less memory or
	 			send a message to let others know it is being hidden etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL NodeBitmap::HidingNode()
{
	// Call the base class first
	if (!NodeRect::HidingNode())
		return FALSE;

	GetBitmapRef()->RemoveFromTree();

	return TRUE;
}

/***********************************************************************************************

>    virtual BOOL NodeBitmap::ShowingNode()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   11/1/94
	 Returns:   TRUE if all was ok. FALSE if an error occured (eg Out of memory).
     Purpose:   This virtual function is called whenever the node is re-shown after being 
     			Hidden.
	 			It allows the node to reconstruct itself if it was optimised or
	 			send a message to let others know it is back etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL NodeBitmap::ShowingNode()
{
	// Call the base class first
	if (!NodeRect::ShowingNode())
		return FALSE;

	GetBitmapRef()->AddtoTree();

	return TRUE;
}

/********************************************************************************************

>	void NodeBitmap::SetStartColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Purpose:	Set the start colour of a bitmap palette

********************************************************************************************/

void NodeBitmap::SetStartColour(DocColour* NewCol)
{
	if (NewCol == NULL)
		Colour = COLOUR_NONE;
	else
		Colour = *NewCol;
}

/********************************************************************************************

>	void NodeBitmap::SetEndColour(DocColour* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Purpose:	Set the end colour of a bitmap palette

********************************************************************************************/

void NodeBitmap::SetEndColour(DocColour* NewCol)
{
	if (NewCol == NULL)
		EndColour = COLOUR_NONE;
	else
		EndColour = *NewCol;
}

/********************************************************************************************

>	DocColour* NodeBitmap::GetStartColour()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Returns:	NULL if there is no colour (> 8bpp), or a pointer to the Start Colour.
	Purpose:	Gets the start colour of the bitmap palette

********************************************************************************************/

DocColour* NodeBitmap::GetStartColour()
{
/*
	if (Colour == COLOUR_NONE)
		return NULL;
	else
		return &Colour;
*/

//	if (GetBitmap()->GetBPP() <= 8)
//	{
		NodeAttribute* pLineAttr;
		
		if (!FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pLineAttr))
			return NULL;

		DocColour* Col = ((AttrStrokeColour*)pLineAttr)->GetStartColour();
		if (Col && *Col != COLOUR_NONE)
			return Col;
//	}

	return NULL;
}

/********************************************************************************************

>	DocColour* NodeBitmap::GetEndColour()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Returns:	NULL if there is no colour (> 8bpp), or a pointer to the End Colour.
	Purpose:	Gets the end colour of the bitmap palette

********************************************************************************************/

DocColour* NodeBitmap::GetEndColour()
{
/*
	if (EndColour == COLOUR_NONE)
		return NULL;
	else
		return &EndColour;
*/

//	if (GetBitmap()->GetBPP() <= 8)
//	{
		NodeAttribute* pFillAttr;
		
		if (!FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry), &pFillAttr))
			return NULL;

		DocColour* Col = ((AttrFillGeometry*)pFillAttr)->GetStartColour();
		if (Col && *Col != COLOUR_NONE)
			return Col;
//	}

	return NULL;
}


/********************************************************************************************

>	BOOL NodeBitmap::HasSimpleOrientation(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/95
	Inputs:		pRegion - the region that the bitmap orientation should be tested in.
	Returns:	TRUE if the bitmap orientation is a simple rectangular stretch, perpendicular
				to the co-ordinate system;
				FALSE if not (e.g. rotation or shearing applied to bitmap).
	Purpose:	Determine if a bitmap is classified as a 'simple' bitmap, for use during the
				complex rendering of a view - if the render region can support simple bitmaps
				and this function returns TRUE, then we render the bitmap directly, otherwise
				it goes into the masked GDraw bitmap and is rendered that way.
	SeeAlso:	DrawTransformedBitmap

********************************************************************************************/

BOOL NodeBitmap::HasSimpleOrientation(RenderRegion *pRegion)
{
	PORTNOTETRACE("other","NodeBitmap::HasSimpleOrientation - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Get information from the render region
	View *RenderView = pRegion->GetRenderView();
	Matrix RenderMatrix = pRegion->GetMatrix();

	if (RenderView == NULL)
		// We can't tell - assume the worst
		return FALSE;

	// Check for sideways printing - if the render matrix has rotation, then we are
	// printing at 270 degrees rotation, so we can't do the blit.
	FIXED16 RenderABCD[4];
	INT32	RenderEF[2];
	RenderMatrix.GetComponents(RenderABCD, RenderEF);
	if ((RenderABCD[1] != FIXED16(0)) || (RenderABCD[2] != FIXED16(0)))
		// Rotated by 270 degrees - not simple orientation
		return FALSE;

	// Check the co-ordinates
	DocCoord *Coords = InkPath.GetCoordArray();

	// Use tolerance of 1 pixel when checking for perpendicular nature of the
	// bitmap, as undo/redo can cause small millipoint errors that we should
	// cope with.
	FIXED16 fxPixelSize;
	RenderView->GetScaledPixelSize(&fxPixelSize, &fxPixelSize);
	MILLIPOINT Tolerance = fxPixelSize.MakeLong();

	MILLIPOINT Diff1 = abs(Coords[0].x - Coords[3].x);
	MILLIPOINT Diff2 = abs(Coords[1].x - Coords[2].x);
	MILLIPOINT Diff3 = abs(Coords[0].y - Coords[1].y);
	MILLIPOINT Diff4 = abs(Coords[3].y - Coords[2].y);

	if ((Diff1 < Tolerance) && (Diff2 < Tolerance) &&
		(Diff3 < Tolerance) && (Diff4 < Tolerance))
	{
		// Yes - simple rectangular orientation.
		return TRUE;
	} 
#endif
	// Complex bitmap orientation
	return FALSE;
}




/********************************************************************************************

>	Matrix NodeBitmap::GetInstanceTransform()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/07/2005
	Inputs:		-
	Returns:	Matrix containing the combined transformations that are required to
				get from the original bitmap size and position to that represented by the
				current NodeSimpleShape parallelogram.
	Purpose:	Find the instance transform matrix
	Notes:		Parallelogram format includes 4 coords and is stored in this format:
					0-------1
					|		|
					3-------2
				(Why?)

********************************************************************************************/

Matrix NodeBitmap::GetInstanceTransform()
{
	// First construct matrix to get from OriginalBitmapRect to unit square
	// Then construct matrix to get from unit square to current parallelogram
	// Then combine the two matrices
	// Do the maths in doubles for speed and accuracy!

	DocRect oRect = GetOriginalBitmapRect();

	// Construct the original-to-unit matrix
	double oa = 1.0/oRect.Width();
	double ob = 0;
	double oc = 0;
	double od = 1.0/oRect.Height();
	double oe = -oRect.lo.x;
	double of = -oRect.lo.y;

	// Construct the matrix for unit-to-instance-on-page matrix
	double ia = Parallel[2].x-Parallel[3].x;
	double ib = Parallel[2].y-Parallel[3].y;
	double ic = Parallel[0].x-Parallel[3].x;
	double id = Parallel[0].y-Parallel[3].y;
	double ie = Parallel[3].x;
	double i_f = Parallel[3].y;

	// Multiply the two matrices
	double ra = oa * ia + ob * ic + 0 * ie;
	double rb = oa * ib + ob * id + 0 * i_f;
	double rc = oc * ia + od * ic + 0 * ie;
	double rd = oc * ib + od * id + 0 * i_f;
	double re = oe * ia + of * ic + 1 * ie;
	double rf = oe * ib + of * id + 1 * i_f;

	// Return a crappy old FIXED16-based Matrix object
	return Matrix(ra, rb, rc, rd, INT32(re), INT32(rf) );
}




/********************************************************************************************

>	DocRect NodeBitmap::GetOriginalBitmapRect()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/07/2005
	Inputs:		-
	Returns:	DocRect containing the size of the bitmap if it were placed on the 
				page untransformed.
	Purpose:	Find the starting point for the instance transform

********************************************************************************************/

DocRect NodeBitmap::GetOriginalBitmapRect()
{
	DocRect oRect;
	KernelBitmap* pkBitmap = GetBitmap();
	if (pkBitmap==NULL)
		return oRect;

	oRect.lo.x = 0;
	oRect.lo.y = 0;
//	oRect.hi.x = 750.0 * pkBitmap->GetWidth();	// Note: Pixel width, ignoring bitmap's stated DPI
//	oRect.hi.y = 750.0 * pkBitmap->GetHeight();	// Note: Pixel height, ignoring bitmap's stated DPI
	oRect.hi.x = pkBitmap->GetRecommendedWidth();
	oRect.hi.y = pkBitmap->GetRecommendedHeight();

	return oRect;
}




/********************************************************************************************

>	BOOL NodeBitmap::GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/2005
	Inputs:		pMat	- pointer to matrix awaiting update
	Outputs:	plpInfo - LPBITMAPINFO pointer describing bitmap
				plpBits - LPBYTE pointer to bitmap data
				pRect	- rect of Original bitmap data
				pMat	- matrix describing transform from pRect to this instance on the page
				pdRes	- resolution of this bitmap (dpi)
	Returns:	TRUE if this node can supply direct bitmap data to the caller
				FALSE otherwise
	Purpose:	Return details of direct bitmap to caller (caller is usually a NodeBitmapEffect)

********************************************************************************************/

BOOL NodeBitmap::GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes)
{
	KernelBitmap* pkBitmap = GetBitmap();

	// Initial response is to return our raw bitmap info...
	if (plpInfo)	*plpInfo	= pkBitmap->GetBitmapInfo();
	if (plpBits)	*plpBits	= pkBitmap->GetBitmapBits();
	if (pRect)		*pRect		= GetOriginalBitmapRect();
	if (pMat)		*pMat		= GetInstanceTransform();
	if (pdRes)		*pdRes		= pkBitmap->GetHorizontalDPI();

	// Now test whether the user is asking for a bitmap and whether our raw bitmap needs to
	// be specially processed...
	if (plpInfo && plpBits && HasBitmapAttrs())
	{
		// Lookup processed bitmap in cache 
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache==NULL)
			return FALSE;

		CBitmapCacheKey inky(this, 72000.0/pkBitmap->GetHorizontalDPI(), 2);	// Option 2 is processed/rendered bitmap (see below)
		CCachedBitmap cbmp;
		BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
		if (bFound)
		{
			if (plpInfo)	*plpInfo = cbmp.pbmpInfo;
			if (plpBits)	*plpBits = cbmp.pbmpBits;
			return TRUE;
		}

		// We need to create a new bitmap here that captures the effects of the attributes
		//
		// If we don't have a RenderRegion, we can't build it, so return NULLs to indicate
		// that we need to be called again with a RenderRegion (allows quick test)
		// (See EnsureLiveEffectOriginalBitmaps)
		if (pRender == NULL)
		{
			if (plpInfo)	*plpInfo	= NULL;
			if (plpBits)	*plpBits	= NULL;
			return TRUE;
		}

		// We must "render" this bitmap to capture the attributes that effect its appearance
		// By default, we can't do arbitrarily transformed bitmaps - use a bitmap fill.
		AttrFillGeometry* pTranspAttr = NULL;
		AttrFeather* pFeatherAttr = NULL;
		BitmapTranspFillAttribute* pFeatherTransp = NULL;
		KernelBitmap* pFeatherBitmap = NULL;
		MILLIPOINT mpScaledFeatherSize = 0;
		CProfileBiasGain profileFeather;
		DocRect CaptureRect = GetOriginalBitmapRect();
		CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfFULLCOVERAGE);
		pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, pkBitmap->GetHorizontalDPI());
		pRender->SaveContext();

		{	// ------------------------------------------------------
			// Setup coords for rendering in DirectBitmap domain
			DocCoord coords[3];
			coords[0] = CaptureRect.lo;
			coords[1] = DocCoord(CaptureRect.hi.x, CaptureRect.lo.y);
			coords[2] = DocCoord(CaptureRect.lo.x, CaptureRect.hi.y);

			// Create the path to be rendered in the DirectBitmap domain
			Path InkPath;
			InkPath.Initialise();
			InkPath.AddMoveTo(coords[0]);
			InkPath.AddLineTo(coords[1]);
			InkPath.AddLineTo(DocCoord(coords[2].x+coords[1].x-coords[0].x, coords[2].y+coords[1].y-coords[0].y));
			InkPath.AddLineTo(coords[2]);
			InkPath.AddLineTo(coords[0]);
			InkPath.CloseSubPath();
			InkPath.IsFilled = TRUE;

			// Compute the inverse transform matrix - a matrix to get from "Screen space" to "Direct space"
			Matrix matInverse = GetInstanceTransform().Inverse();

			// No lines on the rectangle
			pRender->SetLineColour(COLOUR_TRANS);
			pRender->SetLineWidth(0);

			// Render the transparency geometry using inverse Direct-Screen transform
			// (This bitmap is being rendered in "Direct space" whereas the transparency attribute
			// was applied in "Screen space")
			NodeAttribute* pAttr;
			if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), &pAttr))
			{
				if (pAttr && !pAttr->IsADefaultAttr() && !pAttr->HasEquivalentDefaultValue(TRUE))
				{
					pTranspAttr = (AttrFillGeometry*) ((AttrFillGeometry*)pAttr)->SimpleCopy();
					if (pTranspAttr)
					{
						DocCoord tcoords[4];
						DocCoord* pCoord = NULL;
						pCoord = pTranspAttr->GetStartPoint();
						tcoords[0] = pCoord ? *pCoord : DocCoord(0,0);
						pCoord = pTranspAttr->GetEndPoint();
						tcoords[1] = pCoord ? *pCoord : DocCoord(0,0);
						pCoord = pTranspAttr->GetEndPoint2();
						tcoords[2] = pCoord ? *pCoord : DocCoord(0,0);
						pCoord = pTranspAttr->GetEndPoint3();
						tcoords[3] = pCoord ? *pCoord : DocCoord(0,0);

						matInverse.transform(tcoords, 4);

						pTranspAttr->SetStartPoint(&tcoords[0]);
						pTranspAttr->SetEndPoint(&tcoords[1]);
						pTranspAttr->SetEndPoint2(&tcoords[2]);
						pTranspAttr->SetEndPoint3(&tcoords[3]);

						pTranspAttr->Render(pRender);
					}
				}
			}

			// Render the feather attribute with inverse scaled feather width
			// (This bitmap is being rendered in "Direct space" whereas the attribute
			// was applied in "Screen space")
			if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFeather), &pAttr))
			{
				// currently, we only generate an offscreen bitmap for GRenderRegions, although we are
				// 'rendered' into all RR's. therefore we must quit quietly if pRender is not a GRR.
				if (pAttr && !pAttr->IsADefaultAttr() && !pAttr->HasEquivalentDefaultValue(TRUE) && pRender->IS_KIND_OF(GRenderRegion))
				{
					// We can't render the feather attribute or a copy of it because the crappy
					// thing relies intimately on its connection to the tree to get geometric
					// information.
					GRenderRegion* pGRR = (GRenderRegion*)pRender;
					pFeatherAttr = (AttrFeather*) ((AttrFeather*)pAttr)->SimpleCopy();
					if (pFeatherAttr)
					{
						mpScaledFeatherSize = pFeatherAttr->Value.GetFeatherSize();
						FIXED16 f16Scale;
						matInverse.Decompose(&f16Scale);	// Should take aspect into account as well???
						mpScaledFeatherSize = (MILLIPOINT)((double)mpScaledFeatherSize * f16Scale.MakeDouble() + 0.5);
						pFeatherAttr->Value.SetFeatherSize(mpScaledFeatherSize);

						profileFeather = pFeatherAttr->Value.GetProfile();

						// Don't let the feather render itself, but call it's core functions with
						// our special information to get it to create the crucial transparency bitmap fill
						pFeatherAttr->Value.CreateFeatherTransp(pGRR, &InkPath, 72000.0/((double)(pkBitmap->GetHorizontalDPI())), &pFeatherTransp, &pFeatherBitmap);

						if (pFeatherTransp)
						{
							TranspFillMappingLinearAttribute* pFeatherTranspMapping = new TranspFillMappingLinearAttribute;
							pFeatherTranspMapping->Repeat = 0;
							pRender->SetTranspFillMapping(pFeatherTranspMapping, TRUE);

							pRender->SetTranspFillGeometry(pFeatherTransp, TRUE);
						}
					}
				}
			}

			// Simple bitmap fill which fills the whole shape
			BitmapFillAttribute *pBitmapAttr = new BitmapFillAttribute;
			pBitmapAttr->GetBitmapRef()->SetBitmap(pkBitmap);

			if (GetStartColour())
				pBitmapAttr->SetStartColour(GetStartColour());
		
			if (GetEndColour())
				pBitmapAttr->SetEndColour(GetEndColour());

			// Set fill coords
			pBitmapAttr->StartPoint = coords[0];
			pBitmapAttr->EndPoint   = coords[1];
			pBitmapAttr->EndPoint2 	= coords[2];

			// Set bitmap attribute, and get the render region to throw it away when it's finished
			// with (hence the TRUE parameter).
			pRender->SetFillGeometry(pBitmapAttr, TRUE);

			// Set the mapping to have no repeat, otherwise we get artifacts at the edges when
			// anti-aliasing is enabled (see bug 1391).
			FillMappingLinearAttribute *pNoRepeatAttr = new FillMappingLinearAttribute;

			// Prevent tesselation to get rid of possible edge effects
			pNoRepeatAttr->Repeat = 0;

			// Set mapping attribute, and get the render region to throw it away when it's finished
			// with (hence the TRUE parameter).
			pRender->SetFillMapping(pNoRepeatAttr, TRUE);

			// Draw the bitmap by rendering a bitmap filled path.
			pRender->DrawPath(&InkPath);
		}	// ------------------------------------------------------

		pRender->RestoreContext();
		if (pTranspAttr)
		{
			delete pTranspAttr;
			pTranspAttr = NULL;
		}

		if (pFeatherAttr)
		{
			delete pFeatherAttr;
			pFeatherAttr = NULL;
		}

// Don't need this because pFeatherTransp is rendered with "Temp=TRUE" so that the RenderRegion will delete it
//		if (pFeatherTransp)
//		{
//			delete pFeatherTransp;
//			pFeatherTransp = NULL;
//		}

		if (pFeatherBitmap != NULL)
		{
			delete pFeatherBitmap;
			pFeatherBitmap = NULL;
		}

		LPBITMAPINFO lpInfo = NULL;
		LPBYTE lpBits = NULL;
		pRender->StopCapture(this, FALSE, FALSE, &lpInfo, &lpBits, &CaptureRect);

		// We should now have a bitmap containing an upright version of the NodeBitmap
		// with transparency and contoning applied as per the attributes in the tree
		if (lpInfo && lpBits)
		{
			*plpInfo = lpInfo;
			*plpBits = lpBits;

			// Cache the PROCESSED/RENDERED bitmap as Option 2
			CBitmapCacheKey inky(this, 72000.0/pkBitmap->GetHorizontalDPI(), 2);
			CCachedBitmap cbmp;
			cbmp.pbmpBits = lpBits;
			cbmp.pbmpInfo = lpInfo;
			cbmp.SetCachedRect(CaptureRect);
			cbmp.nPriority = CACHEPRIORITY_TEMPBITMAP_HIGH;
			if (cbmp.IsValid())
				pBitmapCache->StoreBitmap(inky, cbmp);
		}
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL NodeBitmap::HasBitmapAttrs()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has attributes applied which affect the way it renders
				FALSE otherwise
	Purpose:	Test whether we need to render this bitmap before we can return it from
				GetDirectBitmap

********************************************************************************************/

BOOL NodeBitmap::HasBitmapAttrs()
{
	if (GetStartColour() || GetEndColour())
		return TRUE;

	NodeAttribute* pAttr;
	if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), &pAttr))
	{
		if (pAttr && !pAttr->IsADefaultAttr() && !pAttr->HasEquivalentDefaultValue(TRUE))
			return TRUE;
	}

	if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFeather), &pAttr))
	{
		if (pAttr && !pAttr->IsADefaultAttr() && !pAttr->HasEquivalentDefaultValue(TRUE))
			return TRUE;
	}

	return FALSE;
}




/********************************************************************************************

>	virtual BOOL NodeBitmap::ReleaseCached(BOOL bAndParents = TRUE,
													  BOOL bAndChildren = TRUE,
													  BOOL bSelf = TRUE,
													  BOOL bAndDerived = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/07/2005
	Inputs:		bAndParents - TRUE if should release parent objects
				bAndChildren - TRUE if should release child objects
				bSelf - TRUE if should release own cached data
				bAndDerived - TRUE if should release cached data derived from this object
	Outputs:	-
	Returns:	TRUE
	Purpose:	Get rid of cached data held in the tree

********************************************************************************************/

BOOL NodeBitmap::ReleaseCached(BOOL bAndParents, BOOL bAndChildren, BOOL bSelf, BOOL bAndDerived)
{
	BOOL bRemoveOpaqueBitmapsOnly = IsDragged();				// If we're being dragged, only remove opaque bmps
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache!=NULL && bSelf)
	{
		CBitmapCacheKey inky(this, 42);
		pBitmapCache->RemoveAllOwnedBitmaps(inky, bRemoveOpaqueBitmapsOnly);
	}

	// If we should release our children's caches as well...
	if (bAndChildren)
	{
		Node* pChild = FindFirstChild();
		while (pChild)
		{
			if (pChild->IsBounded())
				((NodeRenderableBounded*)pChild)->ReleaseCached(FALSE, TRUE, TRUE, TRUE);

			pChild = pChild->FindNext();
		}
	}

	// If I can't be cached, neither can my parent...
	if (bAndParents && FindParent() && FindParent()->IsBounded())
		((NodeRenderableBounded*)FindParent())->ReleaseCached(TRUE, FALSE, TRUE, TRUE);

	return TRUE;
}




/********************************************************************************************

>	virtual BOOL NodeBitmap::SupportsClipboardFormat(InternalClipboardFormat *Format) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95

	Returns:	TRUE  = This node supports the given format
				FALSE = This node does not support the format (or the format is unknown)

	Purpose:	Determine if a node supports a given internal data type. This is used
				by the clipboard when exporting to other applications in order to
				determine if certain data types can be supplied.

				e.g. The basic formats include:
					Vector		- this is ALWAYS assumed to be available (Vector format
								  includes every Node, e.g. export in Camelot .art format)

					Text		- As well as paths, some objects can provide text chars

					Bitmap		- Bitmap fills can render a filled object or supply the
								  bitmap used for filling with.

				See InternalClipboardFormat (kernel\cliptype.h) for more details

	Notes:		TextChars can be exported as either "vector" or "text" data

	SeeAlso:	InternalClipboardFormat; Node::SupportsClipboardFormat

********************************************************************************************/

BOOL NodeBitmap::SupportsClipboardFormat(InternalClipboardFormat *Format) const
{
#if !defined(EXCLUDE_FROM_RALPH)
	// bitmaps can be exported as either "vector" or "bitmap" data
	InternalClipboardFormat temp(CLIPTYPE_BITMAP);
	return(Format->IsSameFormat(temp));
//	return(Format->IsSameFormat(InternalClipboardFormat(CLIPTYPE_BITMAP)));
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL NodeBitmap::NeedsTransparency() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode

********************************************************************************************/

BOOL NodeBitmap::NeedsTransparency() const
{
	NodeBitmap* pNonConst = (NodeBitmap*) this;

//	BOOL NeedsTransparency = FALSE;

	return(pNonConst->GetBitmap()->IsTransparent());

/*	if (pNonConst->GetBitmap() != NULL)
	{
		// If the bitmap is 8 bpp or less then check for a transparent colour
		if(pNonConst->GetBitmap()->GetBPP() <= 8)
		{
	 		INT32 TranspIndex;

			// If the bitmap has a transparency index then we'll need to force transparency on
			if (pNonConst->GetBitmap()->GetTransparencyIndex(&TranspIndex))
				return TRUE;
		}
		
		if(pNonConst->GetBitmap()->GetBPP() == 32)
		{
			// Mark H - If we`ve got a 32 bit bitmap then we must make sure we render into a 32bit
			// render region to get the correct output!
			return TRUE;
		}
	}

	return FALSE;
*/
}

/********************************************************************************************

>	BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	
	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/96
	Inputs:		pFilter - file filter to save to
	Returns:	TRUE if successful, false otherwise
	Purpose:	saves a NodeBitmap to the filter

********************************************************************************************/

BOOL NodeBitmap::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL");

	return CXaraFileNodeBitmap::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL NodeBitmap::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL");

	return CXaraFileNodeBitmap::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	KernelBitmap *NodeBitmap::EnumerateBitmaps(UINT32 Count)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/17/95
	Inputs:		Count - the bitmap to get (see Purpose).
	Returns:	The KernelBitmap in use by the node, or NULL if no more are used.
	Purpose:	Find out what bitmaps, if any, are used by this node.

				The base class returns NULL always, so you over-ride this in any node classes
				that use bitmaps.

				This function supports nodes that use more than one bitmap - you call this
				function repeatedly and keep incrementing the Count parameter that you pass
				in each time by 1.  You should stop calling it when it returns NULL, as this
				indicates that no more bitmaps are used by this node.
				Count should start off as 0 for the first call.  Note that this function
				can (and often will) return NULL for the first call, as many nodes don't
				use bitmaps, obviously.

	SeeAlso:	KernelBitmap

********************************************************************************************/

KernelBitmap *NodeBitmap::EnumerateBitmaps(UINT32 Count)
{
	if (Count == 0) return GetBitmap();

	return NULL;
}



/********************************************************************************************
>	BOOL NodeBitmap::IsABitmap() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/17/95
	Returns:	TRUE
	SeeAlso:	Node::IsABitmap
********************************************************************************************/

BOOL NodeBitmap::IsABitmap() const
{
	return TRUE;
}



/********************************************************************************************

>	BOOL NodeBitmap::OnClick(DocCoord PointerPos,
							 ClickType Click,
							 ClickModifiers ClickMods,
							 Spread* pSpread)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/2004
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to clicks by selecting its blobs or starting
				drags etc.
				This functions should be overridden in the all the NodeRenderableInk classes
				so that this version never gets called. Eg the NodePath class might claim
				the click if it happened over one of its unselected blobs.

********************************************************************************************/

BOOL NodeBitmap::OnClick(DocCoord PointerPos,
						 ClickType Click,
						 ClickModifiers ClickMods,
						 Spread* pSpread)
{
PORTNOTE("other","OnClick - Remove XPE edit")
#ifndef EXCLUDE_FROM_XARALX
	if (Click == CLICKTYPE_DOUBLE)
	{
		// We're going to start up a new dialog so we want to clear out any click
		// processing logic that may be in process...
		DocView* pDocView = DocView::GetCurrent();
		if (pDocView)
			pDocView->ClearClickState();

		// Invoke and XPE Edit operation
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_XPE_EDIT);
		if (pOpDesc)
		{
			pOpDesc->Invoke();
			return TRUE;
		}
	}
#endif
	// did not use the click
	return FALSE;
}




/********************************************************************************************

>	virtual void NodeBitmap::SetAspectRatio(double dExWidth, double dExHeight, BOOL bPathAndFill)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/2004
	Inputs:		dExWidth - Sample width (numerator)
				dExHeight - Sample height (denominator)
				bPathAndFill - Apply this logic to both the path and the bitmap fill???
	Returns:	-
	Purpose:	To reset the stored path and Parallelogram to retain their shape and scale
				but to take on a new aspect ratio.

********************************************************************************************/

void NodeBitmap::SetAspectRatio(double dExWidth, double dExHeight, BOOL bPathAndFill)
{
/*
	Gavin says:
	If the parallelogram is defined by the three points P0, P1, P2
	where P1-P0 is the x axis of the bitmap and P2-P0 is the y axis
	and the bitmap had old size W,H and new size NW, NH
	then the new parallelogram NP0,NP1,NP2 will be
		NP0 = P0
		NP1x = P0x+(P1x-P0x)*NW/W
		NP1y = P0y+(P1y-P0y)*NW/W
		NP2x = P0x+(P2x-P0x)*NH/H
		NP2y = P0y+(P2y-P0y)*NH/H
	The fourth points of the parallelogram can be found from the other three:
		P3 = P2+P1-P0

	If you dont want the centre to move then:
		C = (P1+P2)*0.5
		Dxx = (P1x-P0x)*NW/W
		Dxy = (P1y-P0y)*NW/W
		Dyx = (P2x-P0x)*NH/H
		Dyy = (P2y-P0y)*NH/H
		NP0x = (P1x+P2x-Dxx -Dyx)*0.5
		NP0y = (P1y+P2y-Dxy -Dyy)*0.5
		NP1x = NP0x+Dxx
		NP1y = NP0y+Dxy
		NP2x = NP0x+Dyx
		NP2y = NP0y+Dyy
		NP3x = NP0x+Dxx+Dyx
		NP3y = NP0y+Dxy+Dyy

	Phil says:
	Parallelogram points are stored in this order:
			3	<-	2
			v		^
			0	->	1
*/
	DocCoord p0 = Parallel[0];
	DocCoord p1 = Parallel[1];
	DocCoord p2 = Parallel[3];

	double				w = double(GetBitmap()->GetWidth());
	double				h = double(GetBitmap()->GetHeight());

	DocCoord C = DocCoord::OneHalf( p1, p2 );
	INT32 dxx = INT32(( p1.x - p0.x ) * dExWidth / w);
	INT32 dxy = INT32(( p1.y - p0.y ) * dExWidth / w);
	INT32 dyx = INT32(( p2.x - p0.x ) * dExHeight / h);
	INT32 dyy = INT32(( p2.y - p0.y ) * dExHeight / h);

	INT32 ox = INT32(( p1.x + p2.x - dxx - dyx ) * 0.5);
	INT32 oy = INT32(( p1.y + p2.y - dxy - dyy ) * 0.5);
	Parallel[0].x = ox;
	Parallel[0].y = oy;
	Parallel[1].x = ox+dxx;
	Parallel[1].y = oy+dxy;
	Parallel[3].x = ox+dyx;
	Parallel[3].y = oy+dyy;
	Parallel[2].x = ox+dxx+dyx;
	Parallel[2].y = oy+dxy+dyy;

	UpdateShape();
	InvalidateBoundingRect();
}




#if !defined(EXCLUDE_FROM_RALPH)
//-----------------------------------------------------------------------------------------------

// OpCreateNodeBitmap - the operation that is used to create a new NodeBitmap

/********************************************************************************************

>	BOOL OpCreateNodeBitmap::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpCreateNodeBitmap initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpCreateNodeBitmap::Init()
{
	return (RegisterOpDescriptor(
	 							0,
	 							0,
								CC_RUNTIME_CLASS(OpCreateNodeBitmap),
	 							OPTOKEN_CREATENODEBITMAP,
	 							OpCreateNodeBitmap::GetState,
	 							0,	/* help ID */
	 							0,
	 							0	/* bitmap ID */));
}               
    
/********************************************************************************************

>	OpState	OpCreateNodeBitmap::GetState(String_256*, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpCreateNodeBitmap operation
	Purpose:	For finding the OpCreateNodeBitmap's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpCreateNodeBitmap::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	// If there are no open documents, you can't create a node bitmap
	OpSt.Greyed = (Document::GetSelected() == NULL);

 	return(OpSt);
}

/********************************************************************************************

>	void OpCreateNodeBitmap::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a new bitmap object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpCreateNodeBitmap::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)
{
	BOOL ok = FALSE;

	ERROR3IF(pOpParam == NULL, "NULL OpParam passed to OpCreateNodeBitmap"); 

	KernelBitmap* KernelBmp = (KernelBitmap*)(void *)pOpParam->Param1;
	PageDropInfo* pDropInfo = (PageDropInfo*)(void *)pOpParam->Param2;

//	Document* pDoc 		= pDropInfo->pDoc;
	Spread* pSpread		= pDropInfo->pSpread;
	DocCoord DropPos 	= pDropInfo->DropPos;

	DocRect BoundsRect;
	BitmapInfo Info;

	NodeBitmap* pNodeBitmap = new NodeBitmap();
	if ((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12)))
		goto EndOp;

	// Attach the Dropped Bitmap to our Node
	pNodeBitmap->GetBitmapRef()->Attach(KernelBmp);
	if (pNodeBitmap->GetBitmap() != KernelBmp)
	{
		// It didn't use the bitmap we gave it, so we can delete it
		delete KernelBmp;
	}

	
	pNodeBitmap->GetBitmap()->ActualBitmap->GetInfo(&Info);

	BoundsRect.lo.x = DropPos.x - (Info.RecommendedWidth/2);
	BoundsRect.lo.y = DropPos.y - (Info.RecommendedHeight/2);
	BoundsRect.hi.x = DropPos.x + (Info.RecommendedWidth/2);
	BoundsRect.hi.y = DropPos.y + (Info.RecommendedHeight/2);

	// And set this in our bitmap node
	pNodeBitmap->CreateShape(BoundsRect);

	// Set the default attrs
	// This Must be done before the NodeBitmap is inserted into the tree
	if (!pNodeBitmap->ApplyDefaultBitmapAttrs(this))
		goto EndOp;
		
	// Insert the node
	if (!DoInsertNewNode(pNodeBitmap, pSpread, TRUE))
	{
		// It didn't work - delete the sub-tree we just created.
		delete pNodeBitmap;
		goto EndOp;
	} 
	ok = TRUE;

EndOp:
	if (!ok)
		FailAndExecute();

	End();
}

void OpCreateNodeBitmap::GetOpName(String_256* OpName)
{
	OpName->Load(_R(IDS_K_NODEBMP_CREATEBITMAP));
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////
//
// 				ChangeBitmapPtrAction class																 //
//
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ChangeBitmapPtrAction::ChangeBitmapPtrAction()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo fill modification
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeBitmapPtrAction::ChangeBitmapPtrAction()
{
	pChangedBmpNode = NULL;
}

/********************************************************************************************

>	ActionCode ChangeBitmapPtrAction::Init( Operation* pOp,
											ActionList* pActionList,
											Action** NewAction)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeBitmapPtrAction::Init( Operation* pOp,
										ActionList* pActionList,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(ChangeBitmapPtrAction);
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ChangeBitmapPtrAction), NewAction);

	TRACEUSER( "Will", _T("Creating ChangeBitmapPtrAction") );

	return Ac;
}

/********************************************************************************************

>	void ChangeBitmapPtrAction::StoreChanges(NodeBitmap* pBmpNode)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		pBmpNode, is the NodeBitmap that is being changed.
	Outputs:	-
	Returns:	-
	Purpose:	This function initialises the array pointers in this action. Note that the 
				variable NumElements is initialised in the Init function
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ChangeBitmapPtrAction::StoreChanges(NodeBitmap* pBmpNode)
{
	ERROR3IF(pBmpNode == NULL, "NodeBitmap is NULL in ChangeBitmapPtrAction::StoreChanges()");
	if (pBmpNode == NULL)
		return;

	pChangedBmpNode	= pBmpNode;
	BitmapRef.Attach(pBmpNode->GetBitmap(), pOperation->GetWorkingDoc());
	BitmapRef.RemoveFromTree();
}

/********************************************************************************************

>	ActionCode ChangeBitmapPtrAction::Execute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		-
	Outputs:	-
	Returns:	Action code, one of AC_OK, AC_NORECORD or AC_FAIL.
	Purpose:	This is the virtual function that is called when the action is executed
				by the Undo/Redo system. This is the function that actually undoes the 
				ChangeBitmapPtr action by changing the attribute values, and
				records redo information from the current values.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode ChangeBitmapPtrAction::Execute()
{
	ChangeBitmapPtrAction* ModAction;
	
	ActionCode Act;
	Act = ChangeBitmapPtrAction::Init(pOperation, pOppositeActLst, (Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Store away the current bitmap attached to the node
	ModAction->StoreChanges(pChangedBmpNode);

	// Now restore the old one
	pChangedBmpNode->GetBitmapRef()->Attach(BitmapRef.GetBitmap(), pOperation->GetWorkingDoc());

	((UndoableOperation*)pOperation)->
			DoInvalidateNodeRegion((NodeRenderableInk*)pChangedBmpNode, TRUE);

		return Act;
}
