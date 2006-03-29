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

// Implementation of the TipOfTheDay dialog class

#include "camtypes.h"
 
#include "tipsdlg.h"   
//#include "tipsres.h"   
//#include "tips.h"
#include "app.h"
#include "camelot.h"
#include "dialogop.h"
#include "ccstatic.h"   
#include "dragmgr.h"
#include "ensure.h"  
#include "errors.h" 

#include "rndrgn.h"			// For render-into-dialogue support
#include "txtattr.h"
#include "dlgcol.h"
#include "fillval.h"
#include "nodebmp.h"
#include "bitmap.h"
#include "wbitmap.h"

#include "helpuser.h"
//#include "mario.h"			// for _R(IDS_BARSINFO_ONE)
//#include "ollie.h"			// for _R(IDS_TIPOFTHEDAYMENU)


CC_IMPLEMENT_DYNCREATE(TipsDlg, DialogOp)   


// This line MUST go after all CC_IMPLEMENT lines
#define new CAM_DEBUG_NEW


const DocRect g_StaticSize(76,55,138,79);


const UINT32 MIN_TIP_ID = _R(IDS_TIP_1);

const CDlgMode TipsDlg::Mode = MODELESS; 		// The dialog is modeless  

static TCHAR szSection[] = _T("Tip");
static TCHAR szIntNextID[] = _T("NextID");
static TCHAR szIntStartup[] = _T("StartUp");


UINT32 TipsDlg::g_uNextID = MIN_TIP_ID;
BOOL TipsDlg::g_bStartUp = TRUE;


/********************************************************************************************

>	BOOL TipsDlg::Init()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Returns:	FALSE if initialization fails
	Purpose:	Completes the work of the constructor, but returning an error if complete
				construction was not possible.
	Errors:		-
	SeeAlso:	TipsDlg::TipsDlg()

********************************************************************************************/
BOOL TipsDlg::Init()
{  
	if (!RegisterOpDescriptor
	(
 		0,							// toolID
 		_R(IDS_TIPOFTHEDAYMENU),	// Text for Menu
		CC_RUNTIME_CLASS(TipsDlg),
 		OPTOKEN_TIPSDLG,			// Token to Invoke dialog
 		TipsDlg::GetState,			// GetState for menu disabling
 		0,							// HelpID
 		_R(IDS_BBL_TIPDLG),				// BubbleID
		0,							// ResourceID
 		0,							// BitmapID
 		SYSTEMBAR_ILLEGAL,			// Bar ID
		FALSE,						// Recieve system messages
		FALSE,						// Smart duplicate operation
		TRUE,						// Clean operation
		NULL,						// No vertical counterpart
		_R(IDS_BARSINFO_ONE)			// String for one copy only
	))
	{
		return FALSE;
	}

	if (!Camelot.DeclareSection(szSection, 2) ||
		!Camelot.DeclarePref(szSection, szIntNextID, &g_uNextID) ||
		!Camelot.DeclarePref(szSection, szIntStartup, &g_bStartUp)
	)
	{
		return FALSE;
	}
	return TRUE;
}



/********************************************************************************************

>	OpState	TipsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	Provides menu greying. Not that there's much to grey.

********************************************************************************************/
OpState	TipsDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState opState;
	return opState;
}
		 

/********************************************************************************************

>	BOOL TipsDlg::ShowAtStartUp()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Purpose:	Determines whether or not the Tip of the Day dialog should be shown at start
				up.

********************************************************************************************/
BOOL TipsDlg::ShowAtStartUp()
{
	// We need to find out whether to provide the dialog at startup
	// If no startup preference, we assume that the Tips on startup is checked TRUE.
	Camelot.GetPrefValue(szSection, szIntStartup, &g_bStartUp);

	return  g_bStartUp;
}


/********************************************************************************************

>	TipsDlg::TipsDlg()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	This default constructor provides the only way to instantiate TipsDlg objects

********************************************************************************************/
TipsDlg::TipsDlg() : DialogOp(TipsDlg::IDD(), TipsDlg::Mode)
{
	m_bSentInvalid	= FALSE;

	m_bTipOK		= FALSE;
	m_iHashPosition	= 0;

	m_pCurrentMsg	= NULL;
	m_pRenderer		= NULL;

	m_pTipTextGadget	= NULL;

	Camelot.GetPrefValue(szSection, szIntNextID, &g_uNextID);
	// Now try to get the next tip
	GetNextTipString();
}        


/********************************************************************************************

>	TipsDlg::~TipsDlg()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	This destructor is executed whether the user had pressed the escape key
				or clicked on the close button. If the user had pressed the escape key,
				we still need to update the NextID in the ini file with the  latest position
				so that we don't repeat the tips! 

********************************************************************************************/
TipsDlg::~TipsDlg()
{
	Camelot.SetPrefValue(szSection, szIntNextID, &g_uNextID);
}


/********************************************************************************************

>	BOOL TipsDlg::Create()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Returns:	TRUE if creation successful
				FALSE if it fails
	Purpose:	Overrides the base class's Create() member to set up additional class
				specific data.
	SeeAlso:	DialogOp::Create()

********************************************************************************************/
BOOL TipsDlg::Create()
{                            
	if (!DialogOp::Create())
	{
		return FALSE;
	}

	// Create the static control to contain the tip text
	if (!CreateTipGadget())
	{
		return FALSE;
	}

	OnInitDialog();		// Set the initial control values
	return TRUE; 
}           


/********************************************************************************************

>	BOOL TipsDlg::CreateTipGadget()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Returns:	TRUE if creation successful
				FALSE if it fails
	Purpose:	Creates a CCStatic gadget for the Tip text
	Notes:		Unfortunately we can't get the redraw messages for statics out of the
				dialog manager.	So we have to let CCStatic take care of it.
				There's a nasty bit of hard-coded dialog in here, re.the size

********************************************************************************************/
BOOL TipsDlg::CreateTipGadget()
{
	m_pTipTextGadget = new CCStatic;
	if (m_pTipTextGadget == NULL)
	{
		return FALSE;
	}

//	GetGadget()->Size();	// Ha ha ha

	// Create the window for it
	if (!m_pTipTextGadget->Create(this, g_StaticSize, _R(IDC_TIPSTRING), m_strTip.MaxLength()))
	{
		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	void TipsDlg::Do(OpDescriptor*)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	Overrides the base class Op::Do function to perform the operation of
				displaying the Tip of the Day dialog.

********************************************************************************************/
void TipsDlg::Do(OpDescriptor*)
{
	if (!Create())
	{
		return;
	}
	Open();
}


/********************************************************************************************

>	BOOL TipsDlg::OnInitDialog()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Returns:	FALSE if initialisation failed
	Purpose:	Initializes this TipsDlg ready for display

********************************************************************************************/
BOOL TipsDlg::OnInitDialog()
{
	// If Tips file does not exist then disable NextTip
	if (!m_bTipOK)
	{
		EnableGadget(_R(IDC_NEXTTIP), FALSE);

	}
	// Check there's more help & disable the more button if there isn't
	HelpID helpID = GetTipHelpID();
	if (helpID == 0)
	{
		EnableGadget(_R(IDC_MORE), FALSE);
	}
	else
	{
		EnableGadget(_R(IDC_MORE), TRUE);
	}		

	// Tip should be some error message if not OK
	DrawTipString();

	SetBoolGadgetSelected(_R(IDC_STARTUP), g_bStartUp);

	return TRUE;
}

        

/********************************************************************************************

>	MsgResult TipsDlg::Message(Msg* pMessage)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	Dispatches messages to various other member functions beginning On...

********************************************************************************************/
MsgResult TipsDlg::Message(Msg* pMessage)
{
	if (IS_OUR_DIALOG_MSG(pMessage))
	{
		m_pCurrentMsg = (DialogMsg*)pMessage;
		
		switch (m_pCurrentMsg->DlgMsg)
		{
			case DIM_CREATE:
			{
				break;
			}

			case DIM_REDRAW:
			{	
				// Get the redraw information from the message
				ReDrawInfoType* pReDrawInfo = (ReDrawInfoType*) m_pCurrentMsg->DlgMsgParam;
				OnRedraw(pReDrawInfo);
				break;
			}

			case DIM_COMMIT:
				OnCommit();		// and fall through...

			case DIM_CANCEL:
				Close();		// Close and destroy the dialog 
				delete m_pTipTextGadget;
				End();			// WARNING: Destroys this!!!
				return (DLG_EAT_IF_HUNGRY((DialogMsg*)pMessage));	// End() destroyed m_...

			case DIM_LFT_BN_CLICKED:
				// A control on the dialog box has been clicked...
				switch (m_pCurrentMsg->GadgetID)
				{
					case _R(IDC_MORE):		// clicked on "Tell me more..."
						OnMoreHelp();
						return (DLG_EAT_IF_HUNGRY(m_pCurrentMsg));

					case _R(IDC_NEXTTIP):
						OnNextTip();
						break;
				}
				break; // DIM_LFT_BN_CLICKED		
		}
	}
	return OK;  
}  


/********************************************************************************************

>	void TipsDlg::OnNextTip()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	Responds to a user's request for the Next Tip by displaying it.

********************************************************************************************/
void TipsDlg::OnNextTip()
{
	GetNextTipString();

	// Check there's more help & disable the more button if there isn't
	HelpID helpID = GetTipHelpID();
	if (helpID == 0)
	{
		EnableGadget(_R(IDC_MORE), FALSE);
	}
	else
	{
		EnableGadget(_R(IDC_MORE), TRUE);
	}		

	DrawTipString();
}


/********************************************************************************************

>	void TipsDlg::OnCommit()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	Performs any operations after the user has pressed the close button

********************************************************************************************/
void TipsDlg::OnCommit()
{
    // Update the startup information stored in the INI file
	g_bStartUp = GetBoolGadgetSelected(_R(IDC_STARTUP));
	Camelot.SetPrefValue(szSection, szIntStartup, &g_bStartUp);
}


/********************************************************************************************

>	void TipsDlg::OnRedraw(ReDrawInfoType* pRedrawInfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	Redraws the dialog in response to a DIM_REDRAW message

********************************************************************************************/
void TipsDlg::OnRedraw(ReDrawInfoType* pRedrawInfo)
{
	// Use the actual size of the window as our scale
	DocRect RedrawRect(0, 0, pRedrawInfo->dx, pRedrawInfo->dy);

	m_pRenderer = CreateGRenderRegion(&RedrawRect, pRedrawInfo);
	if (m_pRenderer != NULL)
	{
		// Interlock redraw with the drag manager to ensure we don't redraw over any drag
		// blobs
		DragManagerOp::RedrawStarting(WindowID, m_pCurrentMsg->GadgetID);

		DialogColourInfo defaultColours;			// Object supplying Host OS redraw colours

		m_pRenderer->SaveContext();

		// Paint the background
		m_pRenderer->SetLineWidth(0);
		m_pRenderer->SetLineColour(COLOUR_WHITE);
		m_pRenderer->SetFillColour(COLOUR_WHITE);
		m_pRenderer->DrawRect(&RedrawRect);
		switch (m_pCurrentMsg->GadgetID)
		{
			case _R(IDC_BITMAP):
			{
				// Draw bitmap in top corner and validate only top portion of window
				DrawBitmap(_R(IDB_LIGHTBULB), RedrawRect);
				break;
			}
			case _R(IDC_DIDYOUKNOW):
			{
				
				DrawBitmap(_R(IDB_DIDYOU), RedrawRect);
/*
				m_pRenderer->SetFixedSystemTextColours(	&defaultColours.TextFore(), 
														&defaultColours.TextBack());
				// Draw out "Did you know..." message next to the bitmap
				String_256 strMessage;
				strMessage.Load(_R(IDS_DIDYOUKNOW));
				DrawText(&strMessage, RedrawRect, ALN_CENTRE);
*/
				break;
				// Just draw a white rectangle with the text on it
			}
	
			default:
			{
				TRACEUSER( "Colin", _T("Invalidated - m_bSentInvalid %d\n"), m_bSentInvalid);
				if (!m_bSentInvalid)
				{
					m_bSentInvalid = TRUE;
					m_pTipTextGadget->Invalidate(pRedrawInfo->pClipRect);
				}
				else
				{
					m_bSentInvalid = FALSE;
				}
				break;
			}
		}


		m_pRenderer->RestoreContext();

		DestroyGRenderRegion(m_pRenderer);
		// And turn off the drag redraw interlock
		DragManagerOp::RedrawFinished();

	}
}


/********************************************************************************************

>	void TipsDlg::OnMoreHelp()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Returns:	A MsgResult determining how the message handler should continue processing
	Purpose:	Responds to a user's request for more help for a tip by invoking the on-line
				help.

********************************************************************************************/
void TipsDlg::OnMoreHelp()
{
	HelpID helpID = GetTipHelpID();
	if (helpID > 0)				// Invoke the help system
	{
		HelpUserTopic(helpID);
	}
	else
	{
		GetTipString(_R(IDS_SORRY_NO_HELP));
		DrawTipString();
	}
}



/********************************************************************************************

>	void TipsDlg::GetNextTipString()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	Stores the next tip string in m_strTip
				and the position of the '#' in that string in m_iHashPosition
				A TIP should be of the form "tip string#nnnn" where nnnn is the help topic
				ID.

********************************************************************************************/
void TipsDlg::GetNextTipString()
{
	// Check the next tip id is in the designated range
	if (g_uNextID < MIN_TIP_ID || g_uNextID > MAX_TIP_ID)
	{
		GetTipString(_R(IDS_INVALID_TIP_ID));
		g_uNextID = MIN_TIP_ID;
		return;
	} 

	// Attempt to get the tip string resource
	GetTipString(g_uNextID);

	// Get ready to read the next one
	++g_uNextID;
	// If we've run out of tips, start all over...
	if (g_uNextID > MAX_TIP_ID)
	{
		g_uNextID = MIN_TIP_ID;
	}
}



/********************************************************************************************

>	void TipsDlg::GetTipString(ResourceID StringID)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Inputs:		StringID : the ID of the tip resource string to get
	Purpose:	Support function that gets the Tip string defined by the given resource id
				The resource should have been declared using the TIP() macro declared in
				tipsres.h

********************************************************************************************/
void TipsDlg::GetTipString(ResourceID StringID)
{
	// Be pessimistic and assume we won't be able to get the string
	m_bTipOK = FALSE;

	// Attempt to get the tip string resource
	if (!m_strTip.Load(StringID))
	{
		if (!m_strTip.Load(_R(IDS_UNABLE_TO_LOAD)))
		{
			ERROR3("Can't even load unable to load message");
			return;
		}
	} 

	// The resource should be of the form tip#nnn
	// Find the '#' in the resource string and remember its position
	m_iHashPosition = m_strTip.FindNextChar(TEXT('#'), 0);
	if (m_iHashPosition <= 0)
	{
		ERROR3("m_strTip doesn't contain a help id");
		return;
	}
	
	// We managed to get a tip, so signal that everyone can use it
	m_bTipOK = TRUE;
}


/********************************************************************************************

>	HelpID TipsDlg::GetTipHelpID()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Returns:	The HelpID associated with the current tip
	Purpose:	Allows the user to be given further information regarding the current tip.
	See Also:	OnMoreHelp()

********************************************************************************************/
HelpID TipsDlg::GetTipHelpID()
{
	INT32 iHashPosition = m_iHashPosition + 2;	// +1 for an annoying quote marks
	// Convert the right hand side to a HelpID
	HelpID helpID = HelpID(m_strTip.ConvertToInteger(iHashPosition));
	return helpID;
}


/********************************************************************************************

>	void TipsDlg::DrawTipString()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/96
	Purpose:	Renders the current (m_strTip) tip

********************************************************************************************/
void TipsDlg::DrawTipString()
{
	String_256 actualTip;

	m_strTip.Left(&actualTip, m_iHashPosition - 1);
	m_pTipTextGadget->SetString(&actualTip);
	m_bSentInvalid = TRUE;
	m_pTipTextGadget->Invalidate();
}


/********************************************************************************************

>	BOOL TipsDlg::DrawBitmap(ResourceID BitmapID, DocRect& RedrawRect)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		BitmapID : the resource id of the bitmap to draw
				RedrawRect : the rectangle in which to draw it
	Returns:	TRUE if drawn OK
				FALSE otherwise
	Purpose:	Renders the given bitmap in the given rectangle

********************************************************************************************/
BOOL TipsDlg::DrawBitmap(ResourceID BitmapID, DocRect& RedrawRect)
{
	OILBitmap *pOILBitmap = OILBitmap::Create();
	if (pOILBitmap == NULL)
	{
		return FALSE;
	}

	if (!pOILBitmap->LoadBitmap(BitmapID))
	{
		return FALSE;
	}

	// Centre the bitmap within the RedrawRect
	UINT32 bitmapWidth = pOILBitmap->GetRecommendedWidth();
	UINT32 bitmapHeight = pOILBitmap->GetRecommendedHeight();

	UINT32 xOffset = RedrawRect.lo.x + (RedrawRect.Width() - bitmapWidth) / 2;
	UINT32 yOffset = RedrawRect.lo.y + (RedrawRect.Height() - bitmapHeight) / 2;

	DocCoord lowCorner(xOffset, yOffset);
	DocRect bitmapRect(lowCorner, bitmapWidth, bitmapHeight);

	KernelBitmap bitmap(pOILBitmap, TRUE);
	
	// Now we need to create a temporary NodeBitmap, which we will
	// use to render the bitmap preview.
	NodeBitmap* dummyNode = new NodeBitmap();
	if (dummyNode == NULL)
	{
		return FALSE;
	}

	// Set the NodeBitmap path to be our RedrawRect and attach the Bitmap to it.
	dummyNode->SetUpPath();
	dummyNode->CreateShape(bitmapRect);
	dummyNode->GetBitmapRef()->SetBitmap(&bitmap);

	// Now render the bitmap preview
  	dummyNode->Render(m_pRenderer);
	delete dummyNode;

	// All OK
	return TRUE;
}


/********************************************************************************************

>	void TipsDlg::DrawText(StringBase* pString, DocRect& RedrawRect, Justification justification)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		pString : a pointer to the text to be drawn
				RedrawRect: a reference to the rectangle into which the text is to be drawn
				justification : how the text is to be positioned in the RedrawRect
	Purpose:	Draw the given text in the given rectangle, aligned as specified.
	Notes:		Only JLEFT implemented: centred horizontally & vertically in RedrawRect
				NOT USED - Will be eventually, hopefully

********************************************************************************************/
/*	
void TipsDlg::DrawText(StringBase* pString, DocRect& RedrawRect, Justification justification)
{
	DocRect stringBounds;

	// Find the text size...
	m_pRenderer->GetFixedSystemTextSize(pString, &stringBounds);

	// And then move it depending on the required alignment
	switch (justification)
	{
		case JLEFT:
		{
			INT32 yCentre = RedrawRect.lo.y + (RedrawRect.Height() - stringBounds.Height()) / 2;
			INT32 xOffset = 0;//RedrawRect.lo.x + bitmap.GetRecommendedWidth();
			stringBounds.Translate(xOffset, yCentre);
		}
			
	}
	m_pRenderer->DrawFixedSystemText(pString, stringBounds);
}
*/
