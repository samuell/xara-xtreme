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
// sginit.cpp - Centralised initialisation for galleries

#include "camtypes.h"
#include "sginit.h"

#include "app.h"		// For GetApplication()
#include "layergal.h"	// For OpDisplayLayerGallery et al
#include "newcol.h"		// For NewColourDlg
#include "sgallery.h"	// For SGalleryOptionsDlg and SGallerySearchDlg
#include "sgbitmap.h"	// For OpDisplayBitmapGallery
#include "sgcolour.h"	// For OpDisplayColourGallery && ColourSGallery::Init
#include "sgfonts.h"	// For OpDisplayFontsGallery
#include "sglcart.h"	// For OpDisplayLibClipartGallery et al
#include "sglfills.h"	// For OpDisplayLibFillsGallery
#include "sgline.h"		// For OpDisplayLineGallery
#include "ngdialog.h"	// For OpDisplayNameGallery & OpNGPropertyIndexDesc
#include "sgscan.h"		// For Library::InitLibPrefs()
#include "layerprp.h"	// For LayerPropertiesDlg::Init()
#include "aprps.h"
#include "frameops.h"	// OpGrabFrame::Init()

// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
#include "sglinepr.h"	// For SGalleryLinePropertiesDlg::Init()
#endif

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(SGInit, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW
													   

#ifndef EXCLUDE_GALS

/********************************************************************************************

>	static BOOL SGInit::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Purpose:	Initialises the supergallery system
				(Registers Ops for displaying galleries, reads preferences, etc)

********************************************************************************************/

BOOL SGInit::Init(void)
{
	// Declare the global supergallery preferences...
	Camelot.DeclareSection(TEXT("Displays"), 8);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("FixedGalleryColours"),
							&SuperGallery::UseFixedColourSet);

//////////////////////////////////////////////////////////////////////////////////////////
	// I'm putting this in here - cause I can't find a better place - and seeing as these
	// are mostly display preferences anyway, it makes sense
	static INT32 defaultCompoundToEditableShapesBitmapDPI = 96;
	Camelot.DeclarePref(TEXT("Displays"), TEXT("CompCToEDPI"), &defaultCompoundToEditableShapesBitmapDPI, 10, 1000);
//////////////////////////////////////////////////////////////////////////////////////////

	// And initialise each of the derived SuperGallery systems...
	BOOL ok = (	SGalleryOptionsDlg::Init()			&&
				SGallerySearchDlg::Init()			&&
				SGallerySortDlg::Init()				&&

#ifndef STANDALONE
				OpDisplayFrameGallery::Init()		&&
				OpLayerGalChange::Init()			&&
				OpGrabFrame::Init()					&&
// WEBSTER - markn 15/1/97
// Removed some init functions that no longer exist
#ifndef WEBSTER
				OpDisplayLayerGallery::Init()		&&
				LayerNameDlg::Init()				&&
				LayerPropertyTabsDlg::Init()		&&				

				OpDisplayLineGallery::Init()		&&
				OpDisplayNameGallery::Init()		&&
#endif // WEBSTER				
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
				SGalleryLinePropertiesDlg::Init()	&&
#endif // VECTOR_STROKING				
				ColourSGallery::Init()				&&
				OpDisplayColourGallery::Init()		&&
				ColourNameDlg::Init()				&&
				NewColourDlg::Init()				&&

				OpDisplayBitmapGallery::Init()		&&

				OpDisplayFontsGallery::Init()		&&
#endif
				LibClipartSGallery::Init()			&&
				OpDisplayLibClipartGallery::Init()	&&

#ifndef STANDALONE
				LibFillsSGallery::Init()			&&
				OpDisplayLibFillsGallery::Init()	&&
#endif

				Library::InitLibPrefs()
			);
	
	return ok;
}


/********************************************************************************************

>	static void SGInit::DeInit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Purpose:	De-Initialises the supergallery system

********************************************************************************************/

void SGInit::DeInit(void)
{
// WEBSTER - markn 15/1/97
// Removed some deinit functions that no longer exist

	// Nothing much to deinit as yet
#ifndef STANDALONE
#ifndef WEBSTER
	LayerPropertyTabsDlg::Deinit();	
#endif	// WEBSTER	
	GIFAnimationPropertyTabsDlg::Deinit();
	OpGrabFrame::DeInit();
#endif
}

#endif

/********************************************************************************************

>	static void SGInit::UpdateGalleryButton(char *OpToken, BOOL IsVisible)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Purpose:	Helper function for gallery display Ops - whenever the gallery visibility
				changes, this should be called to update the button for it

********************************************************************************************/

void SGInit::UpdateGalleryButton(char *OpToken, BOOL IsVisible)
{
	// Update all button controls that invoke this Op
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OpToken);
	if (pOpDesc!=NULL)
	{
		// Found the opdescriptor. Now find all the gadgets associated with it
		List Gadgets;
		if (pOpDesc->BuildGadgetList(&Gadgets))
		{
			// Found some. Set the controls accordingly
			GadgetListItem* pGadgetItem = (GadgetListItem*) Gadgets.GetHead();

			while (pGadgetItem != NULL)
			{
				// Set the gadget
				pGadgetItem->pDialogBarOp->SetBoolGadgetSelected(pGadgetItem->gidGadgetID,
																	IsVisible);
				// Find the next gadget
				pGadgetItem = (GadgetListItem*) Gadgets.GetNext(pGadgetItem);
			}
	
			// Clean out the list
			Gadgets.DeleteAll();
		}
	}
}
