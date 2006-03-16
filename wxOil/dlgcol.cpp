// $Id: dlgcol.cpp 662 2006-03-14 21:31:49Z alex $
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
// Support for kernel use of host Operating System dialogue rendering colours

/*
*/


#include "camtypes.h"

#include "dlgcol.h"


CC_IMPLEMENT_MEMDUMP(DialogColourInfo, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW


DocColour DialogColourInfo::mDialogBack;
DocColour DialogColourInfo::mTextBack;
DocColour DialogColourInfo::mTextFore;
DocColour DialogColourInfo::mDisabledTextFore;
DocColour DialogColourInfo::mHighlightedTextBack;
DocColour DialogColourInfo::mHighlightedTextFore;
DocColour DialogColourInfo::mButtonFace;
DocColour DialogColourInfo::mButtonHighlight;
DocColour DialogColourInfo::mButtonShadow;



/********************************************************************************************

>	DialogColourInfo::DialogColourInfo()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94

	Purpose:	Constructor. Caches the Host Operatig System's dialogue redraw colours,
				ready for use by kernel-rendered dialogues

********************************************************************************************/

DialogColourInfo::DialogColourInfo()
{
	RecacheColours();
}




/********************************************************************************************

	static void GetOSColour(DocColour *Col, INT32 ColID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94

	Inputs:		Col - DocColour to be set
				ColID - Windows ID of the system colour to be read (cf ::GetSysColor)

	Outputs:	Col is filled in with an appropriate definition for the colour

	Purpose:	Given a Windows COLORREF colour word, creates an equivalent DocColour

	Notes:		This method may check for specific values and tweak them to get better
				results from GRenderRegions (e.g. GDraw gives a less noticable dither
				for RGB(191,191,191) than for (192,192,192), as it cannot exactly match
				that RGB value- this is the default WIndows background grey, so it is
				important to match it as closely as possible

	Scope:		private (to dlgcol.cpp)

********************************************************************************************/

static void GetOSColour(DocColour *Col, wxSystemColour ColID)
{
	wxColour ColDef = wxSystemSettings::GetColour(ColID);
	
	if (ColDef == wxColour(192, 192, 192))
	{
		*Col = DocColour(191, 191, 191);

		Col->SetSeparable(FALSE);	// UI colours should not colour-separate
		return;
	}
	
	*Col = DocColour((INT32) ColDef.Red(),
					 (INT32) ColDef.Green(),
					 (INT32) ColDef.Blue());

	Col->SetSeparable(FALSE);		// UI colours should not colour-separate
}



/********************************************************************************************

>	void DialogColourInfo::RecacheColours(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94

	Purpose:	Re-caches the Host Operatig System's dialogue redraw colours, ready for
				use by kernel-rendered dialogues

********************************************************************************************/

void DialogColourInfo::RecacheColours(void)
{
	GetOSColour(&mDialogBack, wxSYS_COLOUR_BTNFACE);

	GetOSColour(&mTextBack, wxSYS_COLOUR_WINDOW);
	GetOSColour(&mTextFore, wxSYS_COLOUR_WINDOWTEXT);

	GetOSColour(&mDisabledTextFore, wxSYS_COLOUR_GRAYTEXT);

	GetOSColour(&mHighlightedTextBack, wxSYS_COLOUR_HIGHLIGHT);
	GetOSColour(&mHighlightedTextFore, wxSYS_COLOUR_HIGHLIGHTTEXT);

	GetOSColour(&mButtonFace, wxSYS_COLOUR_BTNFACE);
	GetOSColour(&mButtonHighlight, wxSYS_COLOUR_BTNHIGHLIGHT);
	GetOSColour(&mButtonShadow, wxSYS_COLOUR_BTNSHADOW);
}




