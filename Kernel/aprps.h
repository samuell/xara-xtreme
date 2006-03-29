// $Id$
//#pragma message("Start of aprps.h")
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
#ifndef INC_APRPS
#define INC_APRPS

#include "dialogop.h"
#include "property.h"
#include "layerprp.h"
#include "snap.h"
#include "listitem.h"
#include "list.h"
#include "undoop.h"
#include "unittype.h"
#include "units.h"		
#include "property.h"	
#include "sgframe.h"
#include "msg.h"
#include "cbmpdata.h"

//#pragma message("After includes")

class Layer;
class GIFAnimationPropertyTabs;

/********************************************************************************************

>class GIFAnimationPropertyTabsDlg : public DialogTabOp

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	The Gif Animation Properties dialog box
	
********************************************************************************************/

#define OPTOKEN_GIFANIMPROPERTYTABS	_T("GifAnimationPropertyTabs")

class GIFAnimationPropertyTabsDlg : public DialogTabOp
{
	CC_DECLARE_DYNCREATE(GIFAnimationPropertyTabsDlg)

public:
	GIFAnimationPropertyTabsDlg(CDlgResID DummyDialogResID,CDlgMode Mode);		// Constructor
	GIFAnimationPropertyTabsDlg();												// Default Constructor		
	~GIFAnimationPropertyTabsDlg();												// Destructor

	void Do(OpDescriptor*);														// "Do" function

	static BOOL Init();															// Setup function
	static void Deinit();														// Remove function

	static OpState GetState(String_256*, OpDescriptor*);
																				// Return the state of this operation
	virtual MsgResult Message(Msg* Message);									// Message handler
	virtual BOOL RegisterYourPagesInOrderPlease();								// Add in tab pages
		
	static const CDlgResID IDD;													// Dialog box id
	static CDlgMode Mode;														// Dialog box mode of operation (mode or modeless)

	BOOL CommitDialogValues();

	static GIFAnimationPropertyTabs* GetGIFAnimationPropertiesTabs();			// Returns a pointer to the animation properties handling class.	
	static GIFAnimationPropertyTabsDlg* GetGIFAnimationPropertyTabsDlg()		{ return m_pGIFAnimationPropertyTabsDlg; }	

	static	void SetPageToOpen(INT32 val)		{ m_PageToOpen = val;  }			// The page which the dialog should open with.
	static	BOOL GetPageToOpen()			{ return m_PageToOpen; }

protected:
	String_256 TitleString;													// Dialog box title.	
	static	GIFAnimationPropertyTabs*	m_pGIFAnimationProperties;			// pointer to the list class handling this tab.
	static	INT32	m_PageToOpen;												// The page to open at start up.
	static	GIFAnimationPropertyTabsDlg* m_pGIFAnimationPropertyTabsDlg;	// A static pointer to this dialog.		
};

/*********************************************************************************************************

>	class CCAPI GIFAnimationPropertyTabs : public PropertyTabs

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	These encompass all the tabs available for the Animation properties dialog box to use. 

***********************************************************************************************************/

class CCAPI GIFAnimationPropertyTabs : public PropertyTabs
{
	CC_DECLARE_DYNCREATE(GIFAnimationPropertyTabs);

public:
		
	GIFAnimationPropertyTabs();							// Constructor

	static BOOL InitGIFAnimationPropertyTabs();			// set up all option tabs available
	static DeinitGIFAnimationPropertyTabs();			// remove all option tabs available
	static GIFAnimationPropertyTabs *GetFirst();
	static GIFAnimationPropertyTabs *GetNext(GIFAnimationPropertyTabs*);

	virtual BOOL Init();					
	static	BOOL DeclareGIFAnimationProperty(GIFAnimationPropertyTabs *pGIFAnimationProperty);
			BOOL SameflDelayValues();					// Do all the frame layers have the same delay value.
	virtual BOOL GetApplyNowState();					// Get current state of ApplyNow flag
	virtual BOOL SetApplyNowState(BOOL NewState);		// set new state for ApplyNow 
	virtual BOOL SetInitMessageState(BOOL NewState);	// set new state of InitMessage
	virtual BOOL GetInitMessageState();					// get current state of InitMessage
	virtual CDlgResID GetPageID();						// Return section/page id system
	virtual	BOOL InitSection();							// Called when page first opened
	virtual BOOL CommitSection();						// Called when ok action is required
	virtual BOOL HandleMsg(DialogMsg* Msg);				// Called when messages arrive
		
	static UINT32 FramePropertiesTabNumber;				// Tab number assigned to the frame properties tab.
	static UINT32 AnimationColoursTabNumber;				// Tab number assigned to the animation colours tab.	
	static UINT32 AnimationPropertiesTabNumber;			// Tab number assigned to the animation properties tab.
	static UINT32 PreviewInBrowserTabNumber;				// Tab number assigned to the Preview In Browser tab.
	
	virtual DialogTabOp * GetTabbedDlg(); 				 	// return pointer to the main dialog box
	static	BOOL		SetTabbedDlg(DialogTabOp * pDlg);		// return pointer to the main dialog box
	static	void		IncCount(UINT32 Count)					{ m_Count += Count; }
	static	void		DecCount(UINT32 Count)					{ m_Count -= Count; } 
	static	UINT32		GetCount()								{ return m_Count;	}

	static	void	SetIncludeFramePropertiesTab(BOOL Val)		{ m_IncludeFramePropertiesTab = Val;  }
	static	BOOL	GetIncludeFramePropertiesTab()				{ return m_IncludeFramePropertiesTab; }
	static	void	SetBrowserPreviewTab(BOOL Val)				{ m_BrowserPreviewTab = Val;  }
	static	BOOL	GetBrowserPreviewTab()						{ return m_BrowserPreviewTab; }
	virtual BOOL	UpdateApplyState()							{ return TRUE; }
	static	void	SetCurrentApplyNow(BOOL Val)				{ m_CurrentApplyNow = Val;  }
	static	BOOL	GetCurrentApplyNow()						{ return m_CurrentApplyNow; }
	static	BOOL	GetApplyNow()								{ return m_ApplyNow;		}
	static	void	SetFrameGalleryOpen(BOOL Val)				{ m_FrameGalleryOpen = Val;			} 
	static	void	SetChangeLayerState(BOOL Val)				{ m_ChangeLayerState = Val;			}
	static	void	SetFrameDelayValueChanged(BOOL Val)			{ m_FrameDelayValueChanged = Val;	}
	static	BOOL	GetFrameDelayValueChanged()					{ return m_FrameDelayValueChanged;	}
			void 	SetIgnoreTextChangeMsg(BOOL Val)			{ m_IgnoreTextChangeMsg = Val;		}
			BOOL	GetIgnoreTextChangeMsg()					{ return m_IgnoreTextChangeMsg;		} 

protected:
	static	DialogTabOp *pPropertiesDlg;				// pointer to the main dialog box
	static	UINT32		m_Count;						// The number of instances of this dialog.
	static	BOOL		m_IsManyDisplayed;				// Is many displayed in the Delay edit field.
			BOOL		m_NoFrameLayers;				// Does the spread have any frame layers present.
	static	BOOL		m_IncludeFramePropertiesTab;	// Is the frame properties tab required.
	static	BOOL		m_BrowserPreviewTab;			// This flag allows us to know when to exclude all other tabs.
	static	BOOL		m_InitMessage;					// True if handling an init message
	static	BOOL		m_ApplyNow; 					// True if Apply now should be ungreyed	
	static	BOOL		m_CurrentApplyNow;
	static	List		GIFAnimationPropertyTabsList;	// List of animation tabs.
	static  BOOL		m_FrameGalleryOpen;				// Is the frame gallery open
	static	BOOL		m_ChangeLayerState;				// The layer state is being changed from within the frame gallery,
			BOOL		m_IgnoreTextChangeMsg;			// Flag to indicate when we wish to ignore all text change messages.
	static	BOOL		m_FrameDelayValueChanged;		// The delay value for a frame layer has changed.
};


/********************************************************************************************

>	class FramePropertiesTab : public GIFAnimationPropertyTabs

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	Allows the user to set various Frame properties.

********************************************************************************************/

class FramePropertiesTab : public GIFAnimationPropertyTabs
{         
	CC_DECLARE_DYNCREATE( FramePropertiesTab )  
public:

	FramePropertiesTab();
	~FramePropertiesTab();

	virtual BOOL Init();                        
	virtual BOOL HandleMsg(DialogMsg* Msg);				// Function to handle the messages for this tab/pane window
	virtual BOOL InitSection();							// Function to set up the values on this tab when being opened
	virtual BOOL CommitSection();						// Function to commit the values on this tab on ok
	virtual BOOL UpdateLayerSection();					// Called when layer-related UI needs updating
	virtual BOOL GreySection();							// Called when the user has closed all documents
	virtual BOOL UngreySection();						// Called when the user has opened a document
	virtual BOOL UpdateSection();						// Called when the user has switched to a new document
	virtual CDlgResID GetPageID();						// Return section/page id system
	virtual BOOL IsPropertyRequired();					// Return True if required
			BOOL ChangeControlStatus(const BOOL Status, BOOL IgnoreOverlay = FALSE );	// used to grey/ungrey all controls
			BOOL ChangeLayerState();
	

protected:
	BOOL ShowDetails();
	virtual BOOL UpdateApplyState();
	BOOL HavePropertiesChanged(Layer* pLayer, BOOL ValidateDelayValue = FALSE );	
	BOOL m_GreyStatus;									// Remember the state that we are currently in	
}; 



/********************************************************************************************

>	class AnimationPropertiesTab : public GIFAnimationPropertyTabs

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	Allows the user to set various Animation properties.
	
********************************************************************************************/

class AnimationPropertiesTab : public GIFAnimationPropertyTabs
{         
	CC_DECLARE_DYNCREATE( AnimationPropertiesTab )  
public:

	AnimationPropertiesTab();								// Constructor
	~AnimationPropertiesTab();								// Destructor

	virtual BOOL Init();                        
	virtual BOOL HandleMsg(DialogMsg* Msg);					// Function to handle the messages for this tab/pane window	
	virtual BOOL InitSection();								// Function to set up the values on this tab when being opened
	virtual BOOL CommitSection();							// Function to commit the values on this tab on ok
	virtual BOOL UpdateLayerSection();
	virtual BOOL GreySection();								// Called when the user has closed all documents
	virtual BOOL UngreySection();							// Called when the user has opened a document
	virtual BOOL UpdateSection();							// Called when the user has switched to a new document
	virtual CDlgResID GetPageID();							// Return section/page id system
	virtual BOOL IsPropertyRequired();						// Return True if required

	BOOL	ChangeControlStatus(const BOOL Status);			// used to grey/ungrey all controls
	BOOL	SetGlobalDelay(Spread *pSpread, DWORD Delay);	// Set this global delay value in the frame layer/OILBitmap.

protected:
	BOOL ShowDetails();
	virtual BOOL UpdateApplyState();
	BOOL HavePropertiesChanged(BOOL ValidateDelayValue = FALSE);
	BOOL m_GreyStatus;										// Remember the state that we are currently in				
}; 



/********************************************************************************************

>	class AnimationColoursTab : public GIFAnimationPropertyTabs

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	Allows the user to set various Animation Colours.
	
********************************************************************************************/

class AnimationColoursTab : public GIFAnimationPropertyTabs
{         
	CC_DECLARE_DYNCREATE( AnimationColoursTab )  
public:

	AnimationColoursTab();
	~AnimationColoursTab();

	virtual BOOL Init();                        
	virtual BOOL HandleMsg(DialogMsg* Msg);					// Function to handle the messages for this tab/pane window
	virtual BOOL InitSection();								// Function to set up the values on this tab when being opened
	virtual BOOL CommitSection();							// Function to commit the values on this tab on ok
	virtual BOOL UpdateLayerSection();						// Called when layer-related UI needs updating
	virtual BOOL GreySection();								// Called when the user has closed all documents
	virtual BOOL UngreySection();							// Called when the user has opened a document
	virtual BOOL UpdateSection();							// Called when the user has switched to a new document
	virtual CDlgResID GetPageID();							// Return section/page id system
	virtual BOOL IsPropertyRequired();						// Return True if required
	BOOL	ChangeControlStatus(const BOOL Status);

private:
	BOOL ShowDetails();
	virtual BOOL UpdateApplyState();
	BOOL HavePropertiesChanged();
	BOOL m_GreyStatus;										// Remember the state that we are currently in
}; 


/********************************************************************************************

>	class PreviewInBrowserTab : public GIFAnimationPropertyTabs

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	Allows the user to set various Animation Colours.
	
********************************************************************************************/

class PreviewInBrowserTab : public GIFAnimationPropertyTabs
{         
	CC_DECLARE_DYNCREATE( PreviewInBrowserTab )  
public:

	static Declare();

	PreviewInBrowserTab();
	~PreviewInBrowserTab();

	virtual BOOL Init();                        
	virtual BOOL HandleMsg(DialogMsg* Msg);					// Function to handle the messages for this tab/pane window
	virtual BOOL InitSection();								// Function to set up the values on this tab when being opened
	virtual BOOL CommitSection();							// Function to commit the values on this tab on ok
	virtual BOOL UpdateLayerSection();						// Called when layer-related UI needs updating
	virtual BOOL GreySection();								// Called when the user has closed all documents
	virtual BOOL UngreySection();							// Called when the user has opened a document
	virtual BOOL UpdateSection();							// Called when the user has switched to a new document
	virtual CDlgResID GetPageID();							// Return section/page id system
	virtual BOOL IsPropertyRequired();						// Return True if required
			BOOL ChangeControlStatus(const BOOL Status);
			BOOL GetBrowserValues(BrowserPreviewOptions  *BrowserOptions);

public:
	static BrowserBackground g_Background;
	static BOOL g_InfoInHtmlStub;
	static BOOL g_Imagemap;

private:
	BOOL ShowDetails();
	virtual BOOL UpdateApplyState();
	BOOL HavePropertiesChanged();
	BOOL m_GreyStatus;										// Remember the state that we are currently in
}; 



/********************************************************************************************

>	class AnimExOptns : public GIFAnimationPropertyTabsDlg

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	Invokes the Animation Export Options dialog. 
				This is a two tab form of the GIFAnimationPropertyTabsDlg.
				The two tabs present are:- Animation colours and the animation properties.
					
********************************************************************************************/

class AnimExOptns: public GIFAnimationPropertyTabsDlg
{
	#define OPTOKEN_EXPORTGIFANIMTABSDLG		_T("OpenGifAnimTabsDlg")

	CC_DECLARE_DYNCREATE(AnimExOptns)

public:
	AnimExOptns();									// Default constructor
	~AnimExOptns();									// Destructor
	static	BOOL Init();							// Register the OpDescriptor.
	void	Do(OpDescriptor * pOpDesc);				// 'Do' function
	virtual MsgResult Message(Msg* Message);		// Message handler
	static	OpState	GetState(String_256*, OpDescriptor*) { return TRUE; } 		// GetState function

protected:
	static	CDlgMode Mode;			// Dialog box mode of operation (mode or modeless)
	static const CDlgResID IDD;		// Dialog box id
};


/*********************************************************************************************
>	class BrowserPrvwChgdMsg : public Msg

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	Messages detailing specific changes to the Browsser preview properties.

*********************************************************************************************/

class BrowserPrvwChgdMsg : public Msg
{
	CC_DECLARE_DYNAMIC(BrowserPrvwChgdMsg)

	public:
	
	enum BrowserPrvwPropState
	{
		PROPERTIES_CHANGED
	};

	BrowserPrvwPropState State;

	BrowserPrvwChgdMsg(BrowserPrvwPropState ThisState): State(ThisState) { /* empty */} 
};

/*	
#else
#pragma message("Skipping")
*/
#endif // INC_PROPERTIESLAYERS

//#pragma message("End of aprps.h")
