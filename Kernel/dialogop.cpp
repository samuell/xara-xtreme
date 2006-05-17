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

// Implementation of the following classes 
// 
//	DialogOp 		   Abstract base class which all dialog classes derive from
//		BlobbyDlg	   Dialog demonstrating DialogOp functions
//		MenuPrefDlg	   Menu preferences demo (This has a very short life!)
//  	DebugTreeDlg   Dialog showing Camelot's debug tree

/*
*/ 

//-----------------------------------------------------------------------------------------
// Include files
                       
                     
#include "camtypes.h" 

#include "document.h"  		// Temp  
//#include "resource.h"   
//#include "simon.h"
//#include "tim.h"
//#include "phil.h"
#include "dialogop.h" 		// DialogOp header 
#include "dlgmgr.h"
#include "ensure.h"  
#include "errors.h" 
#include "camelot.h" 
#include "dlgtypes.h" 
#include "bubbleid.h" 
#include "msg.h" 
#include "docview.h"
//#include "colcontx.h"

#include "ccdc.h"			// For redraw with RenderRegions
#include "grndrgn.h"
#include "grnddib.h"
#include "osrndrgn.h"
#include "dlgview.h"
#include "docmsgs.h"
#include "docvmsg.h"

//#include "reshlpid.h"		// for _R(ID_CC_HELP_BUTTON) etc
#include "helpuser.h"
//#include "oilmods.h"

#include "brushmsg.h"   // for the screen change message
#include "dlgevt.h"
#include "ctrllist.h"

class Node;
struct ReDrawInfoType;

#define MAGIC_CREATE  0xa1ec501dUL
#define MAGIC_DESTROY 0xa1ecdeadUL

#define CheckMagic(f) \
	do \
	{ \
		if (MagicWord == MAGIC_DESTROY) \
		{ \
			ERROR3(f ": DialogOp already destroyed"); \
		} \
		else \
		{ \
			ERROR3IF(MagicWord != MAGIC_CREATE, f ": DialogOp suffered double destroyed or is corrupt"); \
		} \
	} while(0)

//-----------------------------------------------------------------------------------------

DECLARE_SOURCE("$Revision$");

// All dynamic implement macros here before CAM_DEBUG_NEW

CC_IMPLEMENT_DYNCREATE(DialogOp, Operation)
CC_IMPLEMENT_DYNCREATE(DialogTabOp, DialogOp)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW     

/********************************************************************************************

>	DialogOp::DialogOp(CDlgResID DlgResID, CMode Mode, CDlgResID SubDialogID = 0,
					   HINSTANCE MainInst = 0, HINSTANCE SubInst = 0)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/93
	Inputs:		DialogResID: The resource ID of the dialog
				Mode:		 The dialog's mode (modal, medeless)
				SubDialogID: The resource ID of another dialog - this allows you to merge
							 two dialogs to create a new one (if this parameter is omitted
							 no merging occurs and you get a normal dialog).
				MainInst, SubInst - instance handles of the modules containing the dialog
									templates (if 0 the main application instance is used).
				OpeningPage:	For tabbed dialogs allows opening tab to be chosen
	Purpose:	The Constructor of the DialogOp class simply sets the resource ID of the 
				dialog and its modality.

********************************************************************************************/


DialogOp::DialogOp(CDlgResID DialogResID, CDlgMode Mode, 
				   CDlgResID SubDialogID, 
				   /* HINSTANCE MainInst, HINSTANCE SubInst, */
				   CCRuntimeClass* Class,
				   INT32 OpeningPage, 
				   CWindowID ParentWnd) : Operation(Class)
{
	MagicWord = MAGIC_CREATE;			// Save this for later

	DlgResID = DialogResID;     		// Dialog's resource ID
	DlgMode = Mode;             		// Dialog's mode       
	SubDlgID = SubDialogID;				// Dialog to merge with (0 => no merging)
	WindowID = NULL; 					// We don't know this until we create the dialog 
	DlgMgr = CCamApp::GetDlgManager();  // Store a pointer to the Dialog Manager so that 
										// it's easy to get at

	ReadWritePage = 0; 					// The page within the dialog that we are currently
										// reading or writing to. This is NULL for all
										// non tabbed dialogs.
	PageToOpen = OpeningPage;	  		// for tabbed dialogs allows opening tab to be chosen

	pEvtHandler = NULL;

PORTNOTE("dialog","Removed Windows resource'ism")
#ifndef EXCLUDE_FROM_XARALX
	// Make the main resource instance the uk.dll one, or whatever, if it exists
	if(MainInst == 0)
		MainInst = ExtraDLLs[Resources_DLL];

	// Set up instance handle for the main dialog										
	if (MainInst == 0)
		MainDlgInst = AfxGetInstanceHandle();
	else
		MainDlgInst = MainInst;
		
	// Make the sub resource instance the uk.dll one, or whatever, if it exists
	if(SubDlgInst == 0)
		SubDlgInst = ExtraDLLs[Resources_DLL];

	// Set up instance handle for the main dialog										
	if (SubInst == 0)
		SubDlgInst = AfxGetInstanceHandle();
	else
		SubDlgInst = SubInst;
#endif
	
	// User is interacting, so shouldn't be waiting!
	OpFlags.HasOwnTimeIndicator = TRUE;

	ParentDlgWnd = ParentWnd;
}                    
 
// No comment block required
 
// Never ever call this it exists cos DYNCREATE demands it. DYNCREATE has no concept 
// of an abstract class. 
DialogOp::DialogOp()
{                                   
	ENSURE(FALSE, "Invalid constructor called for DialogOp"); 
}
  
  
/********************************************************************************************

>	BOOL DialogOp::Create() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all resources to create the dialog can be successfully allocated. 
				FALSE otherwise
	Purpose:	The create method reserves everything needed to guarantee that the dialog will 
				appear. It returns FALSE if it fails.
				If any dialog merging was specified (by providing a non-zero value for
				SubDialogID in the call to the DialogOp constructor) then it will be carried
				out when this function is called.
	Errors:		If the resources cannot be allocated then ERRORIF is called with a
				Dialog creation failed error message.  
	SeeAlso:	-

********************************************************************************************/


// The Create fn should only need to take a DialogOp param when the Bar system is complete
// and the current infobar merging stuff removed 
BOOL DialogOp::Create() 
{                                      
	// Create an instance of the dialog by calling the dialog managers Create method. The 

	// The WindowID will be set by this call
	BOOL Created = DlgMgr->Create(this, /*MainDlgInst,*/ DlgResID, /*SubDlgInst,*/ SubDlgID, DlgMode, 
																	PageToOpen, ParentDlgWnd);
	
	ERRORIF(!Created, _R(IDT_DIALOG_CREATEFAIL), FALSE);

	// It all worked
	return TRUE;
}

/********************************************************************************************

>	void DialogOp::Open() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The open dialog method displays the dialog box. If Create was successful 
				then this function cannot fail
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogOp::Open() 
{                            
	ENSURE(WindowID != NULL, "Trying to open a dialog box which has not been created !"); 
	
	// we may wish to subclass some of our controls, do so here
	AddControlsToHelper();

	// open the dialog
	DlgMgr->Open(WindowID,this); 
}

/********************************************************************************************

>	void DialogOp::Close() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The close method removes the dialog from the display but keeps all system 	 	
				resources associated with it. It hides the dialog.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogOp::Close() 
{                               
	CheckMagic("DialogOp:Close()");
	ENSURE(WindowID != NULL, "Trying to close a window with a NULL window ID !");        
	
	DlgMgr->Close(WindowID, this); 

	// remove any subclassed controls from the control helper
	RemoveControlsFromHelper();

	// we have changed the appearance of the screen so broadcast a message
	if (Document::GetCurrent() != NULL) // only if we have a current document
		BROADCAST_TO_ALL(ScreenChangeMsg());
}

/********************************************************************************************

>	BOOL DialogOp::IsOpen()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/93
	Returns:	!!!!!!!!!!!!!!!!!!!! Bodge
	Purpose:	Don't use this function any more. It's ambiguous and marked for destruction.
				Call IsWindowVisible or HasWindow instead. This has been a public service announcement !!!!
				(Simon)
				

********************************************************************************************/

BOOL DialogOp::IsOpen()
{
	if( WindowID != NULL )
	{
		return ( (wxWindow *)WindowID )->IsShown();	//		<<< --------------  BODGE, BODGE, BODGE
	}
	else return FALSE; 
}


/********************************************************************************************

>	BOOL DialogOp::HasWindow()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Returns:	TRUE if this DialogOp has an associated window.	
				 
	SeeAlso		DialogOp::IsWindowVisible
	Purpose:	To find out if DialogOp has a window associated with it. 
	
********************************************************************************************/

BOOL DialogOp::HasWindow()
{
	return (WindowID != NULL); 
}

/********************************************************************************************

>	BOOL DialogOp::BringToTop()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if the function failed.
	Purpose:	This function brings an open dialog to the top of the z-order
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL DialogOp::BringToTop()
{
	ERROR2IF(!WindowID, FALSE, "BringToTop called on a dialog without a window"); 
	return (DlgMgr->BringToTop(WindowID, this)); 
}


/********************************************************************************************

>	BOOL DialogOp::AddControlsToHelper()


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/2000
	Inputs:		-
	Outputs:	-
	Returns:	always TRUE
	Purpose:	If you want to subclass some or all of the controls on your dialog then
				override this function.
	Errors:		-
	SeeAlso:	First implementation in BrushDlg.h/cpp, DialogControlHelper class (DlgCtHlp.h/.cpp)

********************************************************************************************/

BOOL DialogOp::AddControlsToHelper()
{
	return TRUE;
}


/********************************************************************************************

>	BOOL DialogOp::RemoveControlsFromHelper()


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	If you added some controls in AddControlsToHelper then you must remove them here
	Errors:		-
	SeeAlso:	First implementation in BrushDlg.h/cpp, DialogControlHelper class (DlgCtHlp.h/.cpp)

********************************************************************************************/

void DialogOp::RemoveControlsFromHelper()
{
	return;
}

/********************************************************************************************

>	BOOL DialogOp::IsGadgetTickable(CGadgetID Gadget) 


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/95
	Inputs:		Gadget: The Gadget to test
	Outputs:	-
	Returns:	-
	Purpose:	To determine if Gadget is of a particular type that can be ticked. 
				eg. if the gadget is a button
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogOp::IsGadgetTickable(CGadgetID Gadget) 
{
	return (DlgMgr->IsGadgetTickable(GetReadWriteWindowID(), Gadget)); 
}

/********************************************************************************************

>	BOOL DialogOp::ColourPickerAbort(CGadgetID Gadget) 


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/2000
	Inputs:		Gadget: The Gadget to call this routine on
	Outputs:	-
	Returns:	-
	Purpose:	Used to abort the dragging of the custom colour picker control from within
				the colour editor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogOp::ColourPickerAbort(CGadgetID Gadget, WPARAM wParam) 
{
	return (DlgMgr->ColourPickerAbort(GetReadWriteWindowID(), Gadget, wParam));
}


/********************************************************************************************

>	BOOL DialogOp::IsWindowVisible()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>, function name changed by Simon
	Created:	9/11/93
	Returns:	TRUE if the window associated with this Op is visible. FALSE if not.
				Note the Window must exist to do this check. A call to HasWindow will verify this
				 
	SeeAlso		DialogOp::HasWindow
	Purpose:	Find out if a dialog is shown or hidden

********************************************************************************************/

BOOL DialogOp::IsWindowVisible()
{
	ERROR2IF (WindowID != NULL, FALSE, "The window does not exist, Should have called HasWindow"); 
	return (DlgMgr->IsWindowVisible(WindowID));
}


/********************************************************************************************

>	BOOL DialogOp::IsCustomComboDropdownVisible(CGadgetID Gadget)


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		Gadget: The Gadget to call this routine on
	Outputs:	-
	Returns:	-
	Purpose:	Used to determin whether the custom controls dropdown is visible.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogOp::IsCustomComboDropdownVisible(CGadgetID Gadget)
{
	return (DlgMgr->IsCustomComboDropdownVisible(GetReadWriteWindowID(), Gadget));
}


/********************************************************************************************

>	BOOL DialogOp::CloseDropdown (CGadgetID Gadget, BOOL CloseVal)


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/2000
	Inputs:		Gadget: The Gadget to call this routine on
	Outputs:	-
	Returns:	-
	Purpose:	Used to close the controls dropdown.  This routine was written because of
				the amount of trouble that I was experiencing with my custom controls
				down to escape key processing (which is viewed as a hotkey within camelot).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogOp::CloseDropdown (CGadgetID Gadget, BOOL CloseVal)
{
	return (DlgMgr->CloseDropdown(GetReadWriteWindowID(), Gadget, CloseVal));
}


/********************************************************************************************

>	DialogOp::~DialogOp() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The DialogOp destructor destroys the instance of the DialogOp and all  	
				associated resources. If the dialog was open then it is closed. 
				
				Note that a dialog box can be destroyed even if it was never created. In this 
				case no message is sent to the dialog manager. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DialogOp::~DialogOp() 
{
	CheckMagic("DialogOp destructor");
	if (WindowID != NULL)
		DlgMgr->Delete(WindowID, this); 

	WindowID = NULL; // ensure we get a NULL pointer if this is used again
	DlgMgr = NULL;	// Again, ensure this is a NULL pointer
	pEvtHandler=NULL;

	MagicWord = MAGIC_DESTROY;
}    

// -----------------------------------------------------------------------------------------
// Methods to set gadget values                                                             

/********************************************************************************************

>	void DialogOp::SetComboListLength( CGadgetID Gadget )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function 
	Errors:		-
	SeeAlso:	DialogManager::SetUnitGadgetValue

********************************************************************************************/

void DialogOp::SetComboListLength( CGadgetID Gadget)
{
	DlgMgr->SetComboListLength(GetReadWriteWindowID(),Gadget);
}  

/********************************************************************************************

>	BOOL DialogOp::SetUnitGadgetValue( CGadgetID Gadget, 
								 	   UnitType Unit, 
			        		    	   MILLIPOINT Value,
			        		      	   BOOL EndOfList, 
			     			       	   INT32 ListPos 
			        		     	 )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function 
	Errors:		-
	SeeAlso:	DialogManager::SetUnitGadgetValue

********************************************************************************************/

BOOL DialogOp::SetUnitGadgetValue( CGadgetID Gadget, 
							 	   UnitType Unit, 
			        		       MILLIPOINT Value,
			        		       BOOL EndOfList, 
			     			       INT32 ListPos 
			        		     )
{
	return(DlgMgr->SetUnitGadgetValue(GetReadWriteWindowID(), Gadget, Unit, Value, EndOfList, ListPos)); 
}  

/********************************************************************************************
>	BOOL DialogOp::SetDimensionUnitGadgetValue(	CGadgetID Gadget, 
											 	UnitType  units,
			        		    				double    Value,
												Node*     pNode,
												BOOL      IncludeUnitSpecifier = TRUE,
				        		      			BOOL      EndOfList = FALSE, 
					     						INT32       ListPos   = -1
				        		     	 		)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/95
	Purpose:	See DialogManager for a description of this function 
********************************************************************************************/

BOOL DialogOp::SetDimensionUnitGadgetValue( CGadgetID Gadget, 
										 	UnitType  units, 
			        		       			double    Value,
								   			Node*     pNode,
											BOOL      IncludeUnitSpecifier,
				        		       		BOOL      EndOfList, 
				     			       		INT32       ListPos 
					        		     	)
{
	return DlgMgr->SetDimensionUnitGadgetValue(GetReadWriteWindowID(), Gadget, units, Value, pNode,
											   IncludeUnitSpecifier, EndOfList, ListPos); 
}  


/********************************************************************************************

>	BOOL DialogOp::SetLongGadgetValue(CGadgetID Gadget, 
								  	  INT32 value, 
	  							  	  BOOL EndOfList, 
			     			 	  	  INT32 ListPos) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetLongGadgetValue

********************************************************************************************/

BOOL DialogOp::SetLongGadgetValue(CGadgetID Gadget, 
								  INT32 value, 
	  							  BOOL EndOfList, 
			     			 	  INT32 ListPos) 
{                                                
	return(DlgMgr->SetLongGadgetValue(GetReadWriteWindowID(), Gadget, value, EndOfList, ListPos));  
}    

/********************************************************************************************

>	BOOL DialogOp::SetDoubleGadgetValue(CGadgetID Gadget, 
								  	  double value, 
	  							  	  BOOL EndOfList, 
			     			 	  	  INT32 ListPos) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetLongGadgetValue

********************************************************************************************/

BOOL DialogOp::SetDoubleGadgetValue(CGadgetID Gadget, 
								  double value, 
	  							  BOOL EndOfList, 
			     			 	  INT32 ListPos) 
{                                                
	return(DlgMgr->SetDoubleGadgetValue(GetReadWriteWindowID(), Gadget, value, EndOfList, ListPos));  
}    

/********************************************************************************************

>	BOOL DialogOp::SetBoolGadgetSelected(CGadgetID Gadget,
										  BOOL IsSelected,
				     			 	  	  INT32 ListPos = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/7/94
	Inputs:		Gadget - The Gadget to be set
				IsSelected - TRUE to select, FALSE to deselect the gadget
				ListPos - The item (in list fields) to be affected
	Outputs:	-
	Returns:	TRUE if it succeeded
	Purpose:	An interface for turning option/radio buttons and list item selection states
				on or off. See DialogManager::SetBoolGadgetValue for details.
	Errors:		-
	SeeAlso:	DialogManager::SetBoolGadgetValue

********************************************************************************************/

BOOL DialogOp::SetBoolGadgetSelected(CGadgetID Gadget, BOOL IsSelected, INT32 ListPos) 
{                                                
	return(DlgMgr->SetBoolGadgetSelected(GetReadWriteWindowID(), Gadget, IsSelected, ListPos));

//	return(DlgMgr->SetLongGadgetValue(WindowID, Gadget,
// 									(IsSelected) ? TRUE : FALSE, EndOfList, ListPos));
}    



/********************************************************************************************

>	BOOL DialogOp::SetRadioGroupSelected(CGadgetID *GroupGadgets, CGadgetID SelectedGadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		GroupGadgets - A NULL-terminated list of gadgets in the radio group
				SelectedGadget - The gadget from this list which you wish to be selected
	Outputs:	-
	Returns:	TRUE if it succeeded
	Purpose:	Allows you to select one of a group of gadgets, while ensuring that all
				other gadgets are disabled. Each item in the NULL_terminated list will
				be deselected, except the item 'SelectedGadget', which will be selected

	Errors:		ERROR3 will occur if the list is empty, or if the SelectedGadget does not
				appear in the list.

	SeeAlso:	DialogManager::SetBoolGadgetValue;
				DialogOp::GetRadioGroupSelected

********************************************************************************************/

BOOL DialogOp::SetRadioGroupSelected(CGadgetID *GroupGadgets, CGadgetID SelectedGadget)
{
	INT32 i = 0;
	INT32 returnval = 0;		// Accumulator for BOOL return values
#ifdef _DEBUG
	BOOL WasInList = FALSE;
#endif

	while (GroupGadgets[i] != 0)
	{
#ifdef _DEBUG
		if (GroupGadgets[i] == SelectedGadget)
			WasInList = TRUE;
#endif
		returnval |= (INT32) SetBoolGadgetSelected(GroupGadgets[i], (GroupGadgets[i] == SelectedGadget));
		i++;
	}

#ifdef _DEBUG
	if (!WasInList)
		ERROR3("DialogOp::SetRadioGroupSelected - SelectedGadget is not in the list of radio gadgets");
#endif

	return(returnval != 0);
}


/********************************************************************************************

>	BOOL DialogOp::SetStringGadgetValue(CGadgetID Gadget, 
										UINT32 IDStr,  
										BOOL EndOfList, 
			     			 	  		INT32 ListPos 
										)          
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetStringGadgetValue

********************************************************************************************/

BOOL DialogOp::SetStringGadgetValue(CGadgetID Gadget, 
									UINT32 IDStr,  
									BOOL EndOfList, 
			     			 	  	INT32 ListPos 
									)           
{       
	return(DlgMgr->SetStringGadgetValue(GetReadWriteWindowID(), Gadget, IDStr, EndOfList, ListPos));  
}
    
/********************************************************************************************

>	BOOL DialogOp::SetStringGadgetValue(CGadgetID Gadget, 
									StringBase* StrValue, 
									BOOL EndOfList, 
			     			 	  	INT32 ListPos)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetStringGadgetValue

********************************************************************************************/
    
BOOL DialogOp::SetStringGadgetValue(CGadgetID Gadget, 
									const StringBase& StrValue, 
									BOOL EndOfList, 
			     			 	  	INT32 ListPos)
{      
	return (DlgMgr->SetStringGadgetValue(GetReadWriteWindowID(), Gadget, StrValue, EndOfList, ListPos)); 
}

/********************************************************************************************

>	BOOL DialogOp::SetCustomComboGadgetValue (CGadgetID Gadget, 
							  CustomComboBoxControlDataItem* TheItem,   
							  BOOL EndOfList = TRUE, 
			     			  INT32 ListPos = 0)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> S
	Created:	12/8/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetCustomComboGadgetValue

********************************************************************************************/

BOOL DialogOp::SetCustomComboGadgetValue( CGadgetID Gadget, 
										  CustomComboBoxControlDataItem* TheItem,
										  BOOL EndOfList, 
			     						  INT32 ListPos )
{
#ifndef EXCLUDE_FROM_XARALX
	return( DlgMgr->SetCustomComboGadgetValue( GetReadWriteWindowID(), Gadget, TheItem, EndOfList, ListPos ) ); 
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL DialogOp::SelectCustomComboGadgetValueOnString (CGadgetID Gadget, StringBase* StrVal)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> S
	Created:	9/9/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SelectCustomComboGadgetValueOnString

********************************************************************************************/

BOOL DialogOp::SelectCustomComboGadgetValueOnString (CGadgetID Gadget, StringBase* StrVal)
{
	return (DlgMgr->SelectCustomComboGadgetValueOnString (GetReadWriteWindowID(), Gadget, StrVal));
}




/******************************************************************************
>	void		DialogOp::BuildResDropList(	const CGadgetID DropListID,
												const BOOL		bIncludeAuto = TRUE,
												const BOOL		bVerbose = FALSE,
												const INT32 		Default=0
												)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10-06-2005
	Purpose:	Builds a drop down list for a combo box 
******************************************************************************/

void DialogOp::BuildResDropList(const CGadgetID DropListID,
									const BOOL		bIncludeAuto,
									const BOOL		bVerbose,
									const INT32 		Default
									)
{
	DeleteAllValues(DropListID);
	SetComboListLength(DropListID);

	// TODO:
	// Get XML document from alongside exe
	// For each resolution
	//	Add item to dropdown
	// Next
	if (bVerbose)
	{
		if (bIncludeAuto)
			SetStringGadgetValue(DropListID, _R(IDS_RES_AUTOVARIABLE),	TRUE);

//		SetStringGadgetValue(DropListID, _R(IDS_RES_72),	TRUE);
		SetStringGadgetValue(DropListID, _R(IDS_RES_SCREEN),	TRUE);
		SetStringGadgetValue(DropListID, _R(IDS_RES_INTERMEDIATE),	TRUE);
		SetStringGadgetValue(DropListID, _R(IDS_RES_PRINT),	TRUE);
		SetStringGadgetValue(DropListID, _R(IDS_RES_PRINT300),	TRUE);
	}
	else
	{
		if (bIncludeAuto)
			SetStringGadgetValue(DropListID, _R(IDS_RES_AUTO),	TRUE);

//		SetStringGadgetValue(DropListID, _R(IDS_RES_72),	TRUE);
		SetStringGadgetValue(DropListID, _R(IDS_RES_96),	TRUE);
		SetStringGadgetValue(DropListID, _R(IDS_RES_150),	TRUE);
		SetStringGadgetValue(DropListID, _R(IDS_RES_200),	TRUE);
		SetStringGadgetValue(DropListID, _R(IDS_RES_300),	TRUE);
	}

	SetComboListLength(DropListID);
	SetSelectedValueIndex(DropListID, Default);
}




/******************************************************************************
>	void		DialogOp::SetResDropListValue(	const CGadgetID DropListID,
												const BOOL		bIncludeAuto = TRUE,
												const INT32 		iValue = 0
												)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10-06-2005
	Purpose:	Builds a drop down list for a combo box 
******************************************************************************/

void DialogOp::SetResDropListValue(const CGadgetID DropListID,
									const BOOL		bIncludeAuto,
									const INT32 		iValue
									)
{
	// TODO:
	// Get XML document from alongside exe
	// Discover index by comparing value against values in XML
	INT32 iAutoAdjust = bIncludeAuto ? 1 : 0;

	switch (iValue)
	{
	case 0:
		if (bIncludeAuto)
		{
			SetSelectedValueIndex(DropListID, 0);
		}
		break;
//	case 72:
//		SetSelectedValueIndex(DropListID, iAutoAdjust + 0);
//		break;
	case 96:
		SetSelectedValueIndex(DropListID, iAutoAdjust + 0);
		break;
	case 150:
		SetSelectedValueIndex(DropListID, iAutoAdjust + 1);
		break;
	case 200:
		SetSelectedValueIndex(DropListID, iAutoAdjust + 2);
		break;
	case 300:
		SetSelectedValueIndex(DropListID, iAutoAdjust + 3);
		break;
	default:
		{
			// Remove selected item from list first
			// otherwise is screws up the value we are about to put into the field
			SetSelectedValueIndex(DropListID, -1);

			TCHAR str[32];
			String_32 strTemp(_R(IDS_RES_PPI_FORMAT));
			camSnprintf( str, 32, strTemp, iValue );
			String_32 strPPI(str);
			SetStringGadgetValue(DropListID, strPPI, FALSE, -1);
		}
		break;
	}
}

			
			
			
/******************************************************************************
>	INT32		DialogOp::GetResDropListValue(	const CGadgetID DropListID,
											const BOOL		bIncludeAuto = TRUE,
											BOOL*			bValid = NULL
											)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10-06-2005
	Purpose:	Gets value from Resolution dropdown list
******************************************************************************/

INT32	DialogOp::GetResDropListValue(	const CGadgetID DropListID,
									const BOOL		bIncludeAuto,
									BOOL*			pbValid
									)
{
	BOOL Valid;
	INT32 iValue = 0;
	if (pbValid) *pbValid = FALSE;

	// First check whether it's "none" or not
	String_256 str;
	str = GetStringGadgetValue(DropListID, &Valid, -1);

//	if (str.IsIdentical(String_32(_R(IDS_LE_NONE))) ||
//		str.IsIdentical(String_32(_R(IDS_LE_MANY))) ||
//		!Valid)
//	{
//		return;
//	}
//
	if (bIncludeAuto && str.IsIdentical(String_32(_R(IDS_RES_AUTO))))
	{
		iValue = 0;
		Valid = TRUE;
	}
	else if (str.IsIdentical(String_32(_R(IDS_RES_AUTOVARIABLE))))
	{
		iValue = 0;
		Valid = TRUE;
	}
	else if (str.IsIdentical(String_32(_R(IDS_RES_SCREEN))))
	{
		iValue = 96;
		Valid = TRUE;
	}
	else if (str.IsIdentical(String_32(_R(IDS_RES_INTERMEDIATE))))
	{
		iValue = 150;
		Valid = TRUE;
	}
	else if (str.IsIdentical(String_32(_R(IDS_RES_PRINT))))
	{
		iValue = 200;
		Valid = TRUE;
	}
	else if (str.IsIdentical(String_32(_R(IDS_RES_PRINT300))))
	{
		iValue = 300;
		Valid = TRUE;
	}
	else
		iValue = GetLongGadgetValue(DropListID, 1, 1000, 0, &Valid);

	if (pbValid) *pbValid = Valid;
	return iValue;
}

			
			
			
/********************************************************************************************

>	BOOL DialogOp::SetGadgetRange(CGadgetID GadgetID, INT32 Min, INT32 Max, INT32 PageInc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetGadgetValue

********************************************************************************************/

BOOL DialogOp::SetGadgetRange(CGadgetID GadgetID, INT32 Min, INT32 Max, INT32 PageInc)
{                                                       
	return (DlgMgr->SetGadgetRange(GetReadWriteWindowID(), GadgetID, Min, Max, PageInc)); 	
} 

/********************************************************************************************

>	BOOL DialogOp::SetSelectedValueIndex(CGadgetID Gadget, 
							   			  INT32 Index)	   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetSelectedValueIndex

********************************************************************************************/

BOOL DialogOp::SetSelectedValueIndex(CGadgetID Gadget, 
						   			  INT32 Index)	   
{ 
	return (DlgMgr->SetSelectedValueIndex(GetReadWriteWindowID(), Gadget, Index)); 	
}                                              

/********************************************************************************************

>	BOOL DialogOp::SetSelectedValueRange(CGadgetID Gadget, 
							   			  WORD StartIndex,
							   			  WORD EndIndex,
							   			  BOOL Select)	   

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetSelectedValueIndex

********************************************************************************************/

BOOL DialogOp::SetSelectedValueRange(CGadgetID Gadget,
						   			  WORD StartIndex,
						   			  WORD EndIndex,
						   			  BOOL Select)
{
	return (DlgMgr->SetSelectedValueRange(GetReadWriteWindowID(), Gadget, StartIndex, EndIndex, Select)); 	
}

/********************************************************************************************

>	BOOL DialogOp::SetDimensionGadgetValue(	CGadgetID Gadget, 
			        		    			MILLIPOINT Value,
											Node* pNode,
											BOOL IncludeUnitSpecifier = TRUE,
			        		      			BOOL EndOfList = FALSE, 
				     						INT32 ListPos = -1
			        		     	 		)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function 
	Errors:		-
	SeeAlso:	DialogManager::SetDimensionGadgetValue

********************************************************************************************/

BOOL DialogOp::SetDimensionGadgetValue( CGadgetID Gadget, 
			        		       		MILLIPOINT Value,
								   		Node* pNode,
										BOOL IncludeUnitSpecifier,
			        		       		BOOL EndOfList, 
			     			       		INT32 ListPos 
			        		     	)
{
	return(DlgMgr->SetDimensionGadgetValue(GetReadWriteWindowID(), Gadget, Value, pNode, IncludeUnitSpecifier, EndOfList, ListPos)); 
}  

/********************************************************************************************

>	BOOL DialogOp::SetMemoryGadgetValue( CGadgetID Gadget, 
			        		    		  UINT32 Value,
			        					  BOOL EndOfList, 
			     						  INT32 ListPos 
			        		     	 	)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function 
	Errors:		-
	SeeAlso:	DialogManager::SetMemoryGadgetValue; DialogManager::SetMemoryGadgetValue;

********************************************************************************************/

BOOL DialogOp::SetMemoryGadgetValue( CGadgetID Gadget, 
			        				 UINT32 Value,
			  						 BOOL EndOfList, 
			   						 INT32 ListPos 
			        				)
{
	return(DlgMgr->SetMemoryGadgetValue(GetReadWriteWindowID(), Gadget, Value, EndOfList, ListPos)); 
}  

/********************************************************************************************

>	BOOL DialogOp::SetGadgetHelp( CGadgetID Gadget, 
						    	  UINT32 BubbleID, 
						    	  UINT32 StatusID, 
						    	  UINT32 ModuleID = 0)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function 
	Errors:		-
	SeeAlso:	DialogManager::SetGadgetHelp

********************************************************************************************/

BOOL DialogOp::SetGadgetHelp( CGadgetID Gadget, 
					    	  UINT32 BubbleID, 
					    	  UINT32 StatusID, 
					    	  UINT32 ModuleID)
{
	return(DlgMgr->SetGadgetHelp(GetReadWriteWindowID(), Gadget, BubbleID, StatusID, ModuleID));  
}   


// -----------------------------------------------------------------------------------------
// Methods to get gadget values

/********************************************************************************************

>	MILLIPOINT DialogOp::GetUnitGadgetValue(CGadgetID Gadget,  
			       				  UnitType DefaultType,
		        	    		  MILLIPOINT StartRange, 
		                 		  MILLIPOINT EndRange,
		              		      UINT32 IDSInvalidMsg,  
		              			  BOOL* Valid)        

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetUnitGadgetValue

********************************************************************************************/
	
MILLIPOINT DialogOp::GetUnitGadgetValue(CGadgetID Gadget,  
			       				  UnitType DefaultType,
		        	    		  MILLIPOINT StartRange, 
		                 		  MILLIPOINT EndRange,
		              		      UINT32 IDSInvalidMsg,  
		              			  BOOL* Valid)        
{    
	return(DlgMgr->GetUnitGadgetValue(GetReadWriteWindowID(), Gadget, DefaultType, StartRange, EndRange,
		                 		  IDSInvalidMsg, Valid));         
}   

/********************************************************************************************

>	INT32 DialogOp::GetLongGadgetValue(CGadgetID Gadget, 
		       			      		  INT32 StartRange, 
		  			          		  INT32 EndRange, 
	           	  			  		  UINT32 IDSInvalidMsg, 
	           			      		  BOOL* Valid,
	           			      		  DialogManager::PFNSTRINGTOINT32 = ::StringToLong)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetLongGadgetValue

********************************************************************************************/

INT32 DialogOp::GetLongGadgetValue(CGadgetID Gadget, 
		       			      	  INT32 StartRange, 
		  			          	  INT32 EndRange, 
	           	  			  	  UINT32 IDSInvalidMsg, 
	           			      	  BOOL* Valid,
	           			      	  DialogManager::PFNSTRINGTOINT32 pfnParser)
{  
	return DlgMgr->GetLongGadgetValue(GetReadWriteWindowID(),
									  Gadget,
									  StartRange,
									  EndRange,
									  IDSInvalidMsg, 
	           	    			   	  Valid,
	           	    			   	  pfnParser);
}  

/********************************************************************************************

>	double DialogOp::GetDoubleGadgetValue(CGadgetID Gadget, 
		       			      		  double StartRange, 
		  			          		  double EndRange, 
	           	  			  		  UINT32 IDSInvalidMsg, 
	           			      		  BOOL* Valid,
	           			      		  DialogManager::PFNSTRINGTODOUBLE = ::StringToDouble)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetDoubleGadgetValue

********************************************************************************************/

double DialogOp::GetDoubleGadgetValue(CGadgetID Gadget, 
		       			      	  double StartRange, 
		  			          	  double EndRange, 
	           	  			  	  UINT32 IDSInvalidMsg, 
	           			      	  BOOL* Valid,
	           			      	  DialogManager::PFNSTRINGTODOUBLE pfnParser)
{  
	return DlgMgr->GetDoubleGadgetValue(GetReadWriteWindowID(),
									  Gadget,
									  StartRange,
									  EndRange,
									  IDSInvalidMsg, 
	           	    			   	  Valid,
	           	    			   	  pfnParser);
}  


/********************************************************************************************
BOOL DialogOp::GetDoubleAndUnitGadgetValue( double*   pMPValue,
											double*   pUnitValue,
											UnitType* pUnitType,
								 			CGadgetID GadgetID,
								 			Node*     pNode)
	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Inputs:		GadgetID   -
				pNode      - node from which to determine default units (ie page units)
	Outputs:	pMPValue   - value of gadget converted to millipoints (accounting for scaling)
				pUnitValue - value of gadget in terms of units output in pUnitType
				pUnitType  - type of unit the value was specified in (or page units if none)
	Returns:	FALSE if fails
	Purpose:	Read the value from a gadget as a double millipont values accounting for unit scaling
				ALSO read the type of unit specified and the gadget value in terms of these units
				ie 0.5m would return 36000.0, 0.5 and METERS (assuming scaling 1m->1in)
********************************************************************************************/

BOOL DialogOp::GetDoubleAndUnitGadgetValue( double*   pMPValue,
											double*   pUnitValue,
											UnitType* pUnitType,
							 				CGadgetID GadgetID,
							 				Node*     pNode)
{
	return DlgMgr->GetDoubleAndUnitGadgetValue( pMPValue, pUnitValue, pUnitType,
												GetReadWriteWindowID(), GadgetID, pNode);
}


/********************************************************************************************

>	BOOL DialogOp::GetBoolGadgetSelected(CGadgetID Gadget,
	           	  			  			UINT32 IDSInvalidMsg = 0,
	           	  			  			BOOL* Valid = NULL,
	           			   				INT32 ListPos = -1)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetLongGadgetValue;
				DialogOp::GetRadioGroupSelected

********************************************************************************************/

BOOL DialogOp::GetBoolGadgetSelected(CGadgetID Gadget,
	           	  			  		UINT32 IDSInvalidMsg,
	           			      		BOOL* Valid,
	           			      		INT32 ListPos)
{  
	return (DlgMgr->GetBoolGadgetSelected(GetReadWriteWindowID(),
										Gadget,
										IDSInvalidMsg,
	           	    			   		Valid,
	           	    			   		ListPos));
}  



/********************************************************************************************

>	CGadgetID DialogOp::GetRadioGroupSelected(CGadgetID *GroupGadgets)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		GroupGadgets - Points to a NULL-terminated list of Gadget IDs
	Outputs:	-
	Returns:	The gadget ID of the gadget in the input list which was selected
				or NULL if the list was empty or no items in it were selected

	Purpose:	To determine which control in a group is selected.
				Returns the first control found to be selected, or if no controls were
				found to be selected, returns NULL.

	Errors:		ERROR3 occurs in debug builds if the list was empty

	SeelAlso:	DialogOp::SetRadioGroupSelected;
				DialogOp::SetBoolGadgetSelected

********************************************************************************************/

CGadgetID DialogOp::GetRadioGroupSelected(CGadgetID *GroupGadgets)
{
	INT32  i = 0;

	ERROR3IF(GroupGadgets[0] == 0,
				"DialogOp::GetRadioGroupSelected called with blank ID list");

	while (GroupGadgets[i] != 0)
	{
		if (GetBoolGadgetSelected(GroupGadgets[i]))
			return(GroupGadgets[i]);

		i++;
	}

	return( 0 );
}



/********************************************************************************************

>	String_256 DialogOp::GetStringGadgetValue(CGadgetID Gadget, BOOL* Valid, INT32 ListPos=1)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetStringGadgetValue

********************************************************************************************/

String_256 DialogOp::GetStringGadgetValue(CGadgetID Gadget, BOOL* Valid, INT32 ListPos)
{   
	return (DlgMgr->GetStringGadgetValue(GetReadWriteWindowID(), Gadget, Valid, ListPos));
}

/********************************************************************************************

>	BOOL DialogOp::GetGadgetRange(CGadgetID GadgetID, INT32* Min, INT32* Max) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetGadgetRange

********************************************************************************************/

BOOL DialogOp::GetGadgetRange(CGadgetID GadgetID, INT32* Min, INT32* Max) 
{                                                       
	return (DlgMgr->GetGadgetRange(GetReadWriteWindowID(), GadgetID, Min, Max)); 	
}	 



/********************************************************************************************

>	MILLIPOINT DialogOp::GetDimensionGadgetValue(CGadgetID Gadget, Node* pNode,BOOL* Valid,INT32 ListPos=-1)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetDimensionGadgetValue

********************************************************************************************/

MILLIPOINT DialogOp::GetDimensionGadgetValue(CGadgetID Gadget, Node* pNode, BOOL* Valid,INT32 ListPos)
{   
	return (DlgMgr->GetDimensionGadgetValue(GetReadWriteWindowID(), Gadget, pNode, Valid, ListPos));
}

/********************************************************************************************

>	UINT32 DialogOp::GetMemoryGadgetValue(CGadgetID Gadget,  
		        	    		  		 UINT32 StartRange, 
		                 		  		 UINT32 EndRange,
		              		      		 UINT32 IDSInvalidMsg,  
		              			  		 BOOL* Valid)        

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetMemoryGadgetValue;	DialogManager::SetMemoryGadgetValue;

********************************************************************************************/
	
UINT32 DialogOp::GetMemoryGadgetValue(CGadgetID Gadget,  
		        	    			 UINT32 StartRange, 
		                 		  	 UINT32 EndRange,
		              		 		 UINT32 IDSInvalidMsg,  
		              				 BOOL* Valid)        
{    
	return(DlgMgr->GetMemoryGadgetValue(GetReadWriteWindowID(), Gadget, StartRange, EndRange,
		                 				IDSInvalidMsg, Valid));         
}   



/********************************************************************************************

>	BOOL DialogOp::DeleteAllValues(CGadgetID GadgetID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes ALL values in list-gadgets
				See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::DeleteAllValues

********************************************************************************************/

BOOL DialogOp::DeleteAllValues(CGadgetID GadgetID)
{
	return(DlgMgr->DeleteAllValues(GetReadWriteWindowID(), GadgetID));
}



/********************************************************************************************

>	BOOL DialogOp::DeleteValue(CGadgetID GadgetID, 
				 BOOL EndOfList, 
				 INT32 ListPos)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::DeleteValue

********************************************************************************************/
  
BOOL DialogOp::DeleteValue(CGadgetID GadgetID, 
				 BOOL EndOfList, 
				 INT32 ListPos)
{    
	return (DlgMgr->DeleteValue(GetReadWriteWindowID(), GadgetID, EndOfList, ListPos)); 
}      

/********************************************************************************************

>	BOOL DialogOp::GetValueCount(CGadgetID GadgetID, INT32* Count)            

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetValueCount

********************************************************************************************/

BOOL DialogOp::GetValueCount(CGadgetID GadgetID, INT32* Count)            
{                                                            
	return (DlgMgr->GetValueCount(GetReadWriteWindowID(), GadgetID, Count)); 
}	      


/********************************************************************************************

>	INT32 DialogOp::GetSelectedValueIndex(CGadgetID GadgetID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Inputs:		GadgetID - the 'listy' gadget you wish to interrogate
	Returns:	-1 if no items are selected in the gadget
				Otherwise, the index of the selected item [0 .. numitems-1]

	Purpose:	A *SENSIBLE* interface to read which item of a combo/list box type of
				gadget is selected.

	SeeAlso:	DialogOp::GetValueIndex; DialogOp::SetSelectedValueIndex

********************************************************************************************/

INT32 DialogOp::GetSelectedValueIndex(CGadgetID GadgetID)
{
	INT32 Index;

	BOOL Valid = DlgMgr->GetValueIndex(GetReadWriteWindowID(), GadgetID, &Index);

	if (Valid)
		return(Index);

	return(-1);		// No selection, or some error
}


/********************************************************************************************

>	INT32 DialogOp::GetSelectedCount(CGadgetID GadgetID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		GadgetID = ID of list-type gadget
	Returns:	The number of selected items in the list-type gadget, or -1 if it fails
	Purpose:	Returns the number of selected items in a list-type gadget
	SeeAlso:	-

********************************************************************************************/

INT32 DialogOp::GetSelectedCount(CGadgetID GadgetID)
{
	return (DlgMgr->GetSelectedCount(GetReadWriteWindowID(), GadgetID));
}

/********************************************************************************************

>	INT32 DialogOp::GetFirstSelectedItem(CGadgetID GadgetID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		GadgetID = ID of list-type gadget
	Returns:	The index of the firsted selected items in the list-type gadget, or -1 if it fails
	Purpose:	Returns the index of the first selected item in a list-type gadget
	SeeAlso:	-

********************************************************************************************/

INT32 DialogOp::GetFirstSelectedItem(CGadgetID GadgetID)
{
	return (DlgMgr->GetFirstSelectedItem(GetReadWriteWindowID(),GadgetID));
}

/********************************************************************************************

>	INT32* DialogOp::GetSelectedItems(CGadgetID GadgetID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		GadgetID = ID of list-type gadget
	Returns:	ptr to the INT32 array, or NULL if it fails
	Purpose:	Returns a ptr to an INT32 array that holds the list of selected indexes.
				The last array entry contains -1.

				The caller is responsible for deleting the array that's returned.
				e.g
					INT32* pArray = GetSelectedItems(WindowID,GadgetID);
					if (pArray != NULL)
					{
						..... // Use the array
						delete [] pArray;
					}
	SeeAlso:	-

********************************************************************************************/

INT32* DialogOp::GetSelectedItems(CGadgetID GadgetID)
{
	return (DlgMgr->GetSelectedItems(GetReadWriteWindowID(),GadgetID));
}


/********************************************************************************************

>	BOOL DialogOp::GetValueIndex(CGadgetID GadgetID, WORD* Index)    

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetValueIndex

********************************************************************************************/

BOOL DialogOp::GetValueIndex(CGadgetID GadgetID, WORD* Index)    
{
	return (DlgMgr->GetValueIndex(GetReadWriteWindowID(), GadgetID, Index));
}   

/********************************************************************************************

>	BOOL DialogOp::GetValueIndex(CGadgetID GadgetID, INT32* Index)    

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GetValueIndex

********************************************************************************************/

BOOL DialogOp::GetValueIndex(CGadgetID GadgetID, INT32* Index)    
{
	return (DlgMgr->GetValueIndex(GetReadWriteWindowID(), GadgetID, Index));
}   

/********************************************************************************************

>	BOOL DialogOp::EnableGadget(CGadgetID Gadget, BOOL Enabled) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::EnableGadget

********************************************************************************************/

BOOL DialogOp::EnableGadget(CGadgetID Gadget, BOOL Enabled) 
{   
	return (DlgMgr->EnableGadget(GetReadWriteWindowID(), Gadget, Enabled)); 	
} 

/********************************************************************************************

>	BOOL DialogOp::HideGadget(CGadgetID GadgetID, BOOL Hide) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::HideGadget

********************************************************************************************/

BOOL DialogOp::HideGadget(CGadgetID GadgetID, BOOL Hide) 
{                                                                                
	return (DlgMgr->HideGadget(GetReadWriteWindowID(), GadgetID, Hide)); 		
}

/********************************************************************************************

>	void DialogOp::Layout(BOOL CanYield = FALSE)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	10/05/2006
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Relayout dialog - for sizer changes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogOp::Layout(BOOL CanYield /*=FALSE*/)
{
	DlgMgr->Layout(GetReadWriteWindowID(), CanYield);
}
    
/********************************************************************************************

>	BOOL DialogOp::GadgetRedraw(CGadgetID GadgetID, BOOL Redraw) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::GadgetRedraw

********************************************************************************************/
    
BOOL DialogOp::GadgetRedraw(CGadgetID GadgetID, BOOL Redraw) 
{                                                           
	return (DlgMgr->GadgetRedraw(GetReadWriteWindowID(), GadgetID, Redraw)); 
}



/********************************************************************************************
>	BOOL DialogOp::SetKeyboardFocus(CGadgetID gadD)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		gadID			ID of the "gadget" (control) which is to receive the
								input focus
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Sets the keyboard input focus to the given control.
	Errors:		-
	SeeAlso:	DialogManager::SetKeyboardFocus; DialogManager::DefaultKeyboardFocus
********************************************************************************************/

BOOL DialogOp::SetKeyboardFocus(CGadgetID gadID)
{
	return DialogManager::SetKeyboardFocus(GetReadWriteWindowID(), gadID);
}


/********************************************************************************************
>	BOOL DialogOp::CaptureMouse(CGadgetID gadD)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/97
	Inputs:		gadID			ID of the "gadget" (control) which is to receive the
								input focus
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Allows the given control to Capture the mouse.
	Errors:		-
	SeeAlso:	DialogManager::CaptureMouse; DialogManager::ReleaseMouse
********************************************************************************************/

BOOL DialogOp::CaptureMouse(CGadgetID gadID)
{
	return DialogManager::CaptureMouse(GetReadWriteWindowID(), gadID);
}



/********************************************************************************************
>	BOOL DialogOp::ReleaseMouse(CGadgetID gadD)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/97
	Inputs:		gadID			ID of the "gadget" (control) which is to receive the
								input focus
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Releases the mouse captured by the given control.
	Errors:		-
	SeeAlso:	DialogManager::CaptureMouse; DialogManager::ReleaseMouse
********************************************************************************************/

BOOL DialogOp::ReleaseMouse(CGadgetID gadID)
{
	return DialogManager::ReleaseMouse(GetReadWriteWindowID(), gadID);
}



/********************************************************************************************
>	BOOL DialogOp::HighlightText(CGadgetID gadID, INT32 nStart = 0, INT32 nEnd = -1)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		gadID			control ID within the window/dialogue box
				nStart			first letter to highlight (by default the very first)
				nEnd			last letter to highlight (by default the very last)
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	Highlights the given range of text (by default all of it) within a
				control that holds editable text, eg. an edit field.
	Errors:		ERROR3 if you try to highlight text in a control without any, eg. a
				button or a scroller.
	SeeAlso:	DialogManager::HighlightText
********************************************************************************************/

BOOL DialogOp::HighlightText(CGadgetID gadID, INT32 nStart, INT32 nEnd)
{
	return DialogManager::HighlightText(GetReadWriteWindowID(), gadID, nStart, nEnd);
}



/********************************************************************************************
>	void DialogOp::PaintGadgetNow(CGadgetID gid)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/94
	Inputs:		gid				the gadget ID of a control
	Outputs:	-
	Returns:	-
	Purpose:	Immediately paints any invalid areas the control may have (like the 
				"UpdateWindow" function in Windows, really).
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void DialogOp::PaintGadgetNow(CGadgetID gid)
{
	DialogManager::PaintGadgetNow(GetReadWriteWindowID(), gid);
}



/********************************************************************************************

>	void DialogOp::InvalidateGadget(CGadgetID Gadget, BOOL EraseBackground=TRUE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		Gadget - The ID of the gadget that you want to be redrawn
				EraseBackground	TRUE if the background should be erased
	Purpose:	Causes the Dialog Manager to tell the host os to get the gadget to be
				redrawn. Calling this on a cc_DialogDraw gadget will cause you to receive
				a DIM_REDRAW message in the not too distant future.

	SeeAlso:	DialogManager::InvalidateGadget

********************************************************************************************/

void DialogOp::InvalidateGadget(CGadgetID Gadget, BOOL EraseBackground/*=TRUE*/)
{
	// Call the Dialog Manager
	DialogManager::InvalidateGadget(GetReadWriteWindowID(), Gadget, EraseBackground);
}



/********************************************************************************************

>	void DialogOp::InvalidateGadget(CGadgetID Gadget,
									RedrawInfoType *ExtraInfo,
									DocRect *InvalidRect);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		Gadget - The ID of the gadget that you want to be redrawn
				ExtraInfo - The information passed to your DIM_ mouse event handler
				InvalidRect - The MILLIPOINT rectangle to invalidate, in the 
				(0,0)->(dx,dy) coordinate space used in ReDrawInfoType.

	Purpose:	Causes the Dialog Manager to tell the host os to get the cc_DialogDraw
				gadget to be redrawn, over the specfied rectangle. You should recieve
				a DIM_REDRAW message in the not too distant future.

	SeeAlso:	DialogManager::InvalidateGadget

********************************************************************************************/

void DialogOp::InvalidateGadget(CGadgetID Gadget,
								ReDrawInfoType *ExtraInfo,
								DocRect *InvalidRect)
{
	// Call the Dialog Manager
	DialogManager::InvalidateGadget(GetReadWriteWindowID(), Gadget, ExtraInfo, InvalidRect);
}

/********************************************************************************************

>	void DialogOp::ScrollKernelRenderedGadget(CGadgetID Gadget,
										DocRect *RectToScroll, DocCoord *ScrollBy);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95
	Inputs:		wid - The Window identifier
				Gadget - The ID of the gadget that you want to be redrawn

				RectToScroll - The MILLIPOINT rectangle to scroll, in the (0,0)->(dx,dy)
				coordinate space used in ReDrawInfoType structures. Think of this rectangle
				as the visible portion of the window which will be copied (some of which
				will be scrolled out of view), and let this method worry about invalidating
				the 'hole' left behind to finish off the display.
				This parameter may be NULL, in which case the entire gadget will be scrolled

				ScrollBy - Gives the X and Y amounts in millipoints to scroll the given
				rectangle. I don't know what will happen if neither of these values is zero!

	Purpose:	Causes the Dialog Manager to tell the host os to get the cc_DialogDraw
				gadget to be scrolled, over the specfied rectangle. You should recieve
				a DIM_REDRAW message in the not too distant future, to update any portions
				that 'scroll into view'.
				
	Notes:		If you are using a Virtual coordinate space which differs from the
				(0,0)->(dx,dy) space that this requires, then you'll need to call
				some conversion methods which do not yet exist!

				See the dlgmgr version for fuller details

	SeeAlso:	DialogManager::ScrollKernelRenderedGadget

********************************************************************************************/

void DialogOp::ScrollKernelRenderedGadget(CGadgetID Gadget,
										DocRect *RectToScroll, DocCoord *ScrollBy)
{
	// Call the Dialog Manager
	DialogManager::ScrollKernelRenderedGadget(GetReadWriteWindowID(), Gadget, RectToScroll, ScrollBy);
}



/********************************************************************************************

>	BOOL DialogOp::GetKernelRenderedGadgetInfo(CGadgetID Gadget, ReDrawInfoType *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		Gadget - The ID of the gadget that you want info for
	
	Returns:	FALSE if there was a catastrophic error (it will report an ERROR2), in
				which case the returned data is invalid (well, it defaults to 96 Dpi and
				an area of 72000x72000 millipoints, so is 'safe' to use, but probably wrong)
				TRUE under normal conditions

	Outputs:	Result - will be returned filled in with appropriate kernel-rendered-
				dialogue information (dx, dy, and Dpi will be filled in with the appropriate
				values; pMousePOs, pDC, and pClipRect will all be NULL)

	Purpose:	Allows the user access to the same information which is passed in to
				DIM_REDRAW and the kernel-rendered-dialogue mouse-handling messages. This
				is just for convenience so they can calculate stuff at a time other than
				handling those two types of dialogue event (e.g. if a document message
				causes you to have to redraw a small portion of your gadget, you need this
				information to calculate the invalidation rectangle from)

	SeeAlso:	DialogManager::GetKernelRenderedGadgetInfo

********************************************************************************************/

BOOL DialogOp::GetKernelRenderedGadgetInfo(CGadgetID Gadget, ReDrawInfoType *Result)
{
	// Call the Dialog Manager
	return(DialogManager::GetKernelRenderedGadgetInfo(GetReadWriteWindowID(), Gadget, Result));
}



/********************************************************************************************

>	void DialogOp::SetGadgetBitmaps(GadgetID GadgetID, UINT32 Bitmap1, UINT32 Bitmap2)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		GadgetID - The gadget to set the bitmaps of
				Bitmap1 - The first Bitmaps ID
				Bitmap2 - The Second bitmaps ID
	Purpose:	Calls the dialog manager to set the bitmaps of the gadget (only for
				cc_Slider controls at the moment)

********************************************************************************************/

void DialogOp::SetGadgetBitmaps(CGadgetID GadgetID, UINT32 Bitmap1, UINT32 Bitmap2)
{
	// Call the dialog Manager
	DialogManager::SetGadgetBitmaps(GetReadWriteWindowID(), GadgetID, Bitmap1, Bitmap2);
}


/********************************************************************************************

>	void DialogOp::SetGadgetBitmaps(GadgetID GadgetID, const CGadgetImageList& imagelist)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		GadgetID - The gadget to set the bitmaps of
				imagelist - list of images for this gadget
	Purpose:	Calls the dialog manager to set the bitmaps of the gadget

********************************************************************************************/

void DialogOp::SetGadgetBitmaps(CGadgetID GadgetID, const CGadgetImageList& imagelist)
{
	// Call the dialog Manager
	DialogManager::SetGadgetBitmaps(GetReadWriteWindowID(), GadgetID, imagelist);
}

/********************************************************************************************

>	void DialogOp::SetGadgetBitmap(CGadgetID Gadget, ResourceID Bitmap)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	07/05/2006
	Inputs:		GadgetID - The gadget to set the bitmaps of
				Bitmap - The ID of the bitmap
	Purpose:	Calls the dialog manager to set the bitmaps of the gadget

********************************************************************************************/

void DialogOp::SetGadgetBitmap(CGadgetID Gadget, ResourceID Bitmap)
{
	// Call the dialog Manager
	DialogManager::SetGadgetBitmap(GetReadWriteWindowID(), Gadget, Bitmap);
}

/********************************************************************************************

>	ResourceID DialogOp::GetGadgetBitmap(CGadgetID Gadget)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	07/05/2006
	Inputs:		GadgetID - The gadget to set the bitmaps of
				Bitmap - The ID of the bitmap
	Purpose:	Calls the dialog manager to set the bitmaps of the gadget

********************************************************************************************/

ResourceID DialogOp::GetGadgetBitmap(CGadgetID Gadget)
{
	// Call the dialog Manager
	return DialogManager::GetGadgetBitmap(GetReadWriteWindowID(), Gadget);
}


/********************************************************************************************

>	UINT32 DialogOp::GetGadgetImageCount(CGadgetID GadgetID)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/Feb/2006
	Inputs:		GadgetID - The gadget to set the bitmaps of
				imagelist - list of images for this gadget
	Purpose:	Calls the dialog manager to set the bitmaps of the gadget

********************************************************************************************/

UINT32 DialogOp::GetGadgetImageCount(CGadgetID GadgetID)
{
	// Call the dialog Manager
	return DialogManager::GetGadgetImageCount(GetReadWriteWindowID(), GadgetID);
}


/********************************************************************************************

>	void DialogOp::SetBitmapButtonIndexes(GadgetID GadgetID, UINT32 SelectedIndex, UINT32 UnselectedIndex)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		GadgetID - The gadget to set the bitmaps of
				SelectedIndex - index for the selected state glyph
				UnselectedIndex - index for the unselected state glyph
	Purpose:	Calls the dialog manager to set the bitmaps of the cc_bitmapbutton control

********************************************************************************************/
void DialogOp::SetBitmapButtonIndexes(CGadgetID GadgetID, UINT32 UnselectedIndex, UINT32 SelectedIndex)
{
	// call the dialog manager function
	DialogManager::SetBitmapButtonIndexes(GetReadWriteWindowID(), GadgetID, SelectedIndex, UnselectedIndex);
}


/********************************************************************************************

>	BOOL DialogOp::AddDialogControlToHelper(CGadgetID GadgetID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		GadgetID
	Purpose:	Calls the dialog manager add this control to the dialog control helper

********************************************************************************************/

BOOL DialogOp::AddDialogControlToHelper(CGadgetID GadgetID)
{
	return DialogManager::AddDialogControlToHelper(GetReadWriteWindowID(), GadgetID);
}


/********************************************************************************************

>	BOOL DialogOp::AddDialogControlToHelper(CGadgetID GadgetID, CDlgResID PageID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		GadgetID
				PageID - the ID of the tabbed dialog page that this control belongs to 
	Purpose:	Overridden version of above for use with tabbed dialogs, allows you to 
				add controls that are not on the currently active page.

********************************************************************************************/

BOOL DialogOp::AddDialogControlToHelper(CGadgetID GadgetID, CDlgResID PageID)
{
	// get the handle of the page first
	CWindowID PageWindow = DialogManager::GetPageWindow(WindowID, PageID);

	if (PageWindow == NULL)
		return FALSE;

	return DialogManager::AddDialogControlToHelper(PageWindow, GadgetID);
}

/********************************************************************************************

>	BOOL DialogOp::RemoveDialogControlFromHelper(CGadgetID GadgetID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		GadgetID
	Purpose:	Calls the dialog manager remove this control from the dialog control helper

********************************************************************************************/

BOOL DialogOp::RemoveDialogControlFromHelper(CGadgetID GadgetID)
{
	return DialogManager::RemoveDialogControlFromHelper(GetReadWriteWindowID(), GadgetID);
}


/********************************************************************************************

>	BOOL DialogOp::RemoveDialogControlFromHelper(CGadgetID GadgetID, CDlgResID PageID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		GadgetID
				PageID - ID of the tabbed dialog page this control is on
	Purpose:	Calls the dialog manager remove this control from the dialog control helper
				Overridden version.
********************************************************************************************/

BOOL DialogOp::RemoveDialogControlFromHelper(CGadgetID GadgetID, CDlgResID PageID)
{
	// get the handle of the page first
	CWindowID PageWindow = DialogManager::GetPageWindow(WindowID, PageID);

	if (PageWindow == NULL)
		return FALSE;

	return DialogManager::RemoveDialogControlFromHelper(PageWindow, GadgetID);

}

/********************************************************************************************

>	void SetEditGadgetType(CGadgetID Gadget, EditGadgetType Type)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		- 
	SeeAlso:	DialogManager::SetEditGadgetType

********************************************************************************************/

void DialogOp::SetEditGadgetType(CGadgetID Gadget, EditGadgetType Type)
{
	DlgMgr->SetEditGadgetType(GetReadWriteWindowID(), Gadget, Type);
}                                                     

/********************************************************************************************

>	void SetEditGadgetType(CGadgetID Gadget, UINT32 IDSValidChar) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::SetEditGadgetType

********************************************************************************************/

void DialogOp::SetEditGadgetType(CGadgetID Gadget, UINT32 IDSValidChar) 
{                                                          
	DlgMgr->SetEditGadgetType(GetReadWriteWindowID(), Gadget, IDSValidChar); 
}  

/********************************************************************************************

>	void DialogOp::DualFunctionButton(CGadgetID ButtonGadget)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/93
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	See DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::DualFunctionButton

********************************************************************************************/

void DialogOp::DualFunctionButton(CGadgetID ButtonGadget)
{
	DlgMgr->DualFunctionButton(GetReadWriteWindowID(), ButtonGadget); 
}    

/********************************************************************************************

>	BOOL DialogOp::MakeListBoxDragable(CGadgetID ListGadget)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	See	DialogManager for a description of this function
	Errors:		-
	SeeAlso:	DialogManager::MakeListBoxDragable

********************************************************************************************/


BOOL DialogOp::MakeListBoxDragable(CGadgetID ListGadget)
{
	return (DlgMgr->MakeListBoxDragable(GetReadWriteWindowID(), ListGadget)); 
}



/********************************************************************************************

>	virtual MsgResult DialogOp::Message( Msg* Message )
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/93
	Inputs:		Message: The message
	Outputs:	-
	Returns:	-
	Purpose:	The Message	function for a DialogOp should look something like this 

				if (IS_OUR_DIALOG_MSG(Message))
				{
					DialogMsg* Msg = (DialogMsg*)Message; 
					// Handle Msg here
				}
				
				// Pass message on to base class for rest of handling.
				return <BaseClass>::Message(Message);

				This Pure virtual function will ENSURE 


	Errors:		-
	SeeAlso:	MessageHandler
	SeeAlso:	IS_OUR_DIALOG_MSG

********************************************************************************************/

MsgResult DialogOp::Message( Msg* Message )       
{
	CheckMagic("DialogOp::Message");;

	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		// Get a pointer the correct type of message.
		DocChangingMsg *pDocMsg = (DocChangingMsg *) Message;

		if ( static_cast<OpMsg*> ( Message )->MsgType  == (OpMsg::OpMsgType)DocViewMsg::SELCHANGED )
		{
			// Selected document has changed:

			// Make sure we were actually up to date before..
			if (pDocMsg->pOldDoc)
			{
				ERROR3IF(pOurDoc != pDocMsg->pOldDoc,
						 "pOurDoc ptr is out of date in DialogOp::Message()");
			}

			// Update our document pointer (might be NULL).
			pOurDoc = pDocMsg->pNewDoc;
		}
	}
	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		// Get a pointer the correct type of message.
		DocViewMsg *pDocViewMsg = (DocViewMsg *) Message;

		if ( static_cast<OpMsg*> ( Message )->MsgType  == (OpMsg::OpMsgType)DocViewMsg::SELCHANGED )
		{
			// Selected document has changed:
			// Make sure we were actually up to date before..
			ERROR3IF(pOurView != (View *) pDocViewMsg->pOldDocView,
					 "pOurView ptr is out of date in DialogOp::Message()");

			// Update our view pointer.
			pOurView = (View *) pDocViewMsg->pNewDocView;
		}
	}
	else if (IS_OUR_DIALOG_MSG(Message))
	{
		// We have an unhandled dialog message.  If it is for a button with the appropriate
		// "help" resource/gadget ID then run some help for this dialog.
		DialogMsg* pDlgMsg = (DialogMsg*) Message;

		// Messages for individial controls with OpDescriptors need to be sent to that control
		if (pDlgMsg->GadgetID)
		{
			OpDescriptor *OpDesc = GetGadgetOpDescriptor(pDlgMsg->GadgetID);
			if (OpDesc)
			{
				// These tend to change bar state
				ControlList::Get()->Changed();
				// Its a message for the current OpDescriptor
				// Convert the dialog message into a OpDescriptor control message
				BROADCAST_TO_CLASS(
					OpDescControlMsg(OpDesc, 
										pDlgMsg->DlgMsg, 
										OpDesc->GetBarControlInfo()->ControlID,
										OpDesc->GetBarControlInfo()->ResourceID,
										this), OpDescriptor);
			}
		}

		switch (pDlgMsg->DlgMsg)
		{
			case DIM_LFT_BN_CLICKED:

			case DIM_RGT_BN_CLICKED:
				// If they clicked on a "help" button then invoke the help system for this dialog.
				if (pDlgMsg->GadgetID == _R(ID_CC_HELP_BUTTON))
				{
					// Invoke the help system and mark this message as completely processed.  If we
					// don't and instead fall through to the base-class handler, the base class might
					// conceivably fail the message	(as it doesn't know anything about it).
PORTNOTE("other","DialogOp::Message - Help system disabled")
#ifndef EXCLUDE_FROM_XARALX
					HelpUser(*this);
#endif
					return DLG_EAT_IF_HUNGRY(pDlgMsg);
				}
				break;
			
			case DIM_TITLEFOCUSWARN:
				{
					// this indicates that the dialog has been moved, we need to send a message saying that the
					// screen has changed
					BROADCAST_TO_ALL(ScreenChangeMsg());
				}
				break;

			case DIM_DLG_MOVED:
				{
					BROADCAST_TO_ALL(ScreenChangeMsg());
				}
				break;

			case DIM_COMMIT:
			case DIM_CANCEL:
			case DIM_CREATE:
			case DIM_BAR_DEATH:
				{
					// These messages need to be propagated to all controls
					BOOL Destroy = ControlList::Get()->SendMessageToAllControls(this, pDlgMsg);	

					// These messages need to be broadcast to all controls within the dialog
					// temporarily ALWAYS destroy if we get as far as here on a DIM_CANCEL
					if ((pDlgMsg->DlgMsg==DIM_CANCEL) || (pDlgMsg->DlgMsg==DIM_COMMIT) ||Destroy )
					{
						Close();
						End();
						// the Op has now been deleted. Exit fast!
						return OK;
						//DlgMgr->Delete(WindowID, this);
						//WindowID = NULL;
					}
					break;
				}

			default:
				break;
		}

		// Pass to the base class for possibly more handling.  If there isn't any then we
		// eat the message, as we know it's for this dialogue.
		MsgResult msgrslt = Operation::Message(Message);
		return (msgrslt == OK) ? DLG_EAT_IF_HUNGRY(pDlgMsg) : msgrslt;
	} 

	// Not interested or possibly more to do - pass to base class.
	return Operation::Message(Message);
}      


/********************************************************************************************

>	static BOOL DialogOp::IsADialogWindow(CWindowID WndID)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Inputs:		WndID:	The CWindowID to try and find
	Outputs:	-
	Returns:	TRUE if WndID is the window id of a dialog
	Purpose:	To find out if WndID is the window ID of a dialog 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL DialogOp::IsADialogWindow(CWindowID WndID)
{
	// Traverse the dialog list to see if WndID is a dialog window
	
	// Obtain the first live dialogOp 
	List* DlgList = GetClassList(CC_RUNTIME_CLASS(DialogOp)); 
	ListItem* CurrentOp = DlgList->GetHead(); 
	

	while (CurrentOp != NULL)	 
	{        
                
		if (((DialogOp*)CurrentOp)->WindowID == WndID)    
	    													  
		{	
			return TRUE;
		}       
		CurrentOp = DlgList->GetNext(CurrentOp); // Get next operation in the live list
	}        
	return FALSE; 
}

/********************************************************************************************

>	BOOL DialogOp::SetTitlebarName(String_256* Name)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		The title bar name
	Outputs:	-
	Returns:	FALSE if the function fails, otherwise TRUE
	Purpose:	Sets the titlebar text
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogOp::SetTitlebarName(String_256* Name)
{																 
	return(DlgMgr->SetTitlebarName(GetReadWriteWindowID(), Name)); 
}

/********************************************************************************************

>	RenderRegion* DialogOp::CreateGRenderRegion(DocRect* pRequiredSize,
												ReDrawInfoType* ExtraInfo,
												BOOL UseSelViewColContext = FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		pRequiredSize - The size that you want the effective area of the window to
				be. Measured in Millipoints. eg. If you want your control to appear to be
				1 inch square (whatever the actual size of it is), pass in a DocRect that
				looks a bit like this ... DocRect(0,0,72000,72000)
				ExtraInfo - The info about the region, as supplied by the dialog manager
	Returns:	Render Region. This returns a pointer to a render region, or NULL if it
				failed to create one.
	Purpose:	Simplifies the creation of a render region as used in a dialog box. It
				will create a render region, scaled to the correct size etc and deal with
				all the Creation, initialisation etc of the region. StartRender is also
				called, so if this function does not return NULL you are able to start
				drawing into it. You MUST call DestroyGRenderRegion when you have draw
				everything that you need to as this calls StopRender (which does the blit
				to the screen) as well as deleting the render region for you

	Notes:		Use GRenderRegions when you want fast redraw, grad fills, and transparency.
				If only simple rendering is required, think about perhaps using an
				OSRenderRegion instead.

	SeeAlso:	DialogOp::DestroyGRenderRegion
	SeeAlso:	DialogOp::CreateOSRenderRegion

********************************************************************************************/

RenderRegion* DialogOp::CreateGRenderRegion(DocRect* pRequiredSize, ReDrawInfoType* ExtraInfo,
												BOOL UseSelViewColContext)
{
	// Make a new dialog view
	DialogView *pDialogView = new DialogView;
	if (pDialogView == NULL)
		// Error - return failure.
		return NULL;

	// Initialise the DialogView
	if (!pDialogView->Init())
	{
		// Error - return failure.
		delete pDialogView;
		return NULL;
	}

	// Pixelise the rectangle
	pRequiredSize->lo.Pixelise(pDialogView);
	pRequiredSize->hi.Pixelise(pDialogView);
	DocRect ClipRect = *pRequiredSize;

	// Get some default params for the render region
	FIXED16 Scale(1);

	// Ok, go and scale things
	INT32 ReqDx = ClipRect.Width();
	INT32 ReqDy = ClipRect.Height();

	// Work out the scale factors
	FIXED16 XScale = FIXED16(double(ExtraInfo->dx) / double(ReqDx));
	FIXED16 YScale = FIXED16(double(ExtraInfo->dy) / double(ReqDy));

	// Build the matricies
	// One to shift everything to the correct side of the X axis
	Matrix Translate(0, -ExtraInfo->dy);

	// One to scale everything into the window
	Matrix ScaleIt(XScale, YScale);

	// One to translate everything to the origin
	Matrix ToOrigin(-ClipRect.lo.x, -ClipRect.lo.y);

	// Combine them all
	ToOrigin *= ScaleIt;
	ToOrigin *= Translate;

	// If the caller wants to use the same colour separation options (ColourPlate) as the
	// currently selected view, then copy the ColourPlate across to the new DialogView
	if (UseSelViewColContext && DocView::GetSelected() != NULL)
		pDialogView->SetColourPlate(DocView::GetSelected()->GetColourPlate());

	// make a render region
	GRenderRegion* pRender = new GRenderDIB(ClipRect, ToOrigin, Scale, 32, (double)ExtraInfo->Dpi);
	if (pRender!=NULL)
	{
		// Try and create the bitmap etc
		if (pRender->AttachDevice(pDialogView, ExtraInfo->pDC, NULL))
		{
			// Try and start the render region
			if (pRender->StartRender())
			{
				return pRender;
			}
		}

		// Failed to attach and start the render region so free up the region
		delete pRender;
	}

	// Something went wrong, fail
	delete pDialogView;
	return NULL;
}




/********************************************************************************************

>	BOOL DialogOp::DestroyGRenderRegion()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		A pointer to a render region return by CreateGRenderRegion
	Returns:	TRUE if it worked, FALSE if the render region passed in was bad
	Purpose:	Deinitialises the render region and deletes it.
	Errors:		ERROR2 error if pRender is NULL
	SeeAlso:	DialogOp::CreateGRenderRegion

********************************************************************************************/

BOOL DialogOp::DestroyGRenderRegion(RenderRegion* pRender)
{
	// Test for preconditions
	ERROR2IF(pRender==NULL, FALSE, "DestroyGRenderRegion was passed a NULL render region");

	// Blit to the screen
	pRender->StopRender();

	// and delete the render region and its (dialog) view
	// these are deleted in this order because the RenderRegion destructor may
	// call functions on its RenderView
	View* pView = pRender->GetRenderView();
	delete pRender;
	delete pView;

	// It worked
	return TRUE;
}


/********************************************************************************************

>	RenderRegion* DialogOp::CreateOSRenderRegion(DocRect* pRequiredSize,
												 ReDrawInfoType* ExtraInfo,
												 BOOL UseSelViewColContext = FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (Jason copied CreateGRenderRegion)
	Created:	17/10/94 (11/1/95)
	
	Inputs:		pRequiredSize - The size that you want the effective area of the window to
				be. Measured in Millipoints. eg. If you want your control to appear to be
				1 inch square (whatever the actual size of it is), pass in a DocRect that
				looks a bit like this ... DocRect(0,0,72000,72000)
				ExtraInfo - The info about the region, as supplied by the dialog manager
	Returns:	Render Region. This returns a pointer to a render region, or NULL if it
				failed to create one.

	Purpose:	Simplifies the creation of a render region as used in a dialog box. It
				will create a render region, scaled to the correct size etc and deal with
				all the Creation, initialisation etc of the region. StartRender is also
				called, so if this function does not return NULL you are able to start
				drawing into it. You MUST call DestroyOSRenderRegion when you have draw
				everything that you need to as this calls StopRender (which does the blit
				to the screen) as well as deleting the render region for you

	Notes:		Use GRenderRegions when you want fast redraw, grad fills, and transparency.
				If only simple rendering is required, think about perhaps using an
				OSRenderRegion instead.

	SeeAlso:	DialogOp::DestroyOSRenderRegion
	SeeAlso:	DialogOp::CreateGRenderRegion

********************************************************************************************/

RenderRegion* DialogOp::CreateOSRenderRegion(DocRect* pRequiredSize, ReDrawInfoType* ExtraInfo,
												BOOL UseSelViewColContext)
{
	// Make a new dialog view
	DialogView *pDialogView = new DialogView;
	if (pDialogView == NULL)
		// Error - return failure.
		return NULL;

	// Initialise the DialogView
	if (!pDialogView->Init())
	{
		// Error - return failure.
		delete pDialogView;
		return NULL;
	}

	// Pixelise the rectangle
	pRequiredSize->lo.Pixelise(pDialogView);
	pRequiredSize->hi.Pixelise(pDialogView);
	DocRect ClipRect = *pRequiredSize;

	// Get some default params for the render region
	FIXED16 Scale(1);

	// Ok, go and scale things
	INT32 ReqDx = ClipRect.Width();
	INT32 ReqDy = ClipRect.Height();

	// Work out the scale factors
	FIXED16 XScale = FIXED16(double(ExtraInfo->dx) / double(ReqDx));
	FIXED16 YScale = FIXED16(double(ExtraInfo->dy) / double(ReqDy));

	// Build the matricies
	// One to shift everything to the correct side of the X axis
	Matrix Translate(0, -ExtraInfo->dy);

	// One to scale everything into the window
	Matrix ScaleIt(XScale, YScale);

	// One to translate everything to the origin
	Matrix ToOrigin(-ClipRect.lo.x, -ClipRect.lo.y);

	// Combine them all
	ToOrigin *= ScaleIt;
	ToOrigin *= Translate;

	// If the caller wants to use the same colour separation options (ColourPlate) as the
	// currently selected view, then copy the ColourPlate across to the new DialogView
	if (UseSelViewColContext && DocView::GetSelected() != NULL)
		pDialogView->SetColourPlate(DocView::GetSelected()->GetColourPlate());

	// Make a render region for the screen, asking specifically for an OSRenderRegion,
	// and none of this sneaky & cunning diverting it back into being a GRenderRegion!
	RenderRegion* pRender = OSRenderRegion::Create(ClipRect, ToOrigin, Scale,
													RENDERTYPE_SCREEN, NULL, TRUE);

	if (pRender!=NULL)
	{
		// Try and create the bitmap etc
		if (pRender->AttachDevice(pDialogView, ExtraInfo->pDC, NULL))
		{
			// Try and start the render region
			if (pRender->StartRender())
			{
				return pRender;
			}
		}

		// Failed to attach and start the render region so free up the region
		delete pRender;
	}

	// Something went wrong, fail
	delete pDialogView;
	return NULL;
}




/********************************************************************************************

>	BOOL DialogOp::DestroyOSRenderRegion(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (Jason copied CreateGRenderRegion)
	Created:	17/10/94 (11/1/95)

	Inputs:		A pointer to a render region return by CreateOSRenderRegion
	Returns:	TRUE if it worked, FALSE if the render region passed in was bad

	Purpose:	Deinitialises the render region and deletes it.

	Errors:		ERROR2 error if pRender is NULL

	SeeAlso:	DialogOp::CreateGRenderRegion
	SeeAlso:	DialogOp::CreateOSRenderRegion

********************************************************************************************/

BOOL DialogOp::DestroyOSRenderRegion(RenderRegion* pRender)
{
	// Test for preconditions
	ERROR2IF(pRender==NULL, FALSE, "DestroyOSRenderRegion was passed a NULL render region");

	// Blit to the screen
	pRender->StopRender();

	// and delete the render region and its (dialog) view
	// these are deleted in this order because the RenderRegion destructor may
	// call functions on its RenderView
	View* pView = pRender->GetRenderView();
	delete pRender;
	delete pView;

	// It worked
	return TRUE;
}


/********************************************************************************************

>	CWindowID DialogOp::GetReadWriteWindowID()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Inputs:		-
	Outputs:	-
	Returns:	the WindowID of the window that we are currently reading or writing
				to.	NULL if the window does not exist.

	Purpose:	For a non tabbed dialog this function simply returns the DialogOp's WindowID.
				For a tabbed dialog the WindowID of the dialog page specified by the last call
				to TalkToPage is returned.
	Errors:		-
	Scope:		protected
	SeeAlso:	DialogTabOp::TalkToPage

********************************************************************************************/


CWindowID DialogOp::GetReadWriteWindowID()
{
	CheckMagic("DialogOp::GetReadWriteWindowID");
	return (DlgMgr->GetPageWindow(WindowID, ReadWritePage)); 		
}

/********************************************************************************************

>	CDlgResID DialogOp::GetCurrentPageID()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		-
	Outputs:	-
	Returns:	the PageID of the tab currently being used.	NULL if its the main page.

	Purpose:	For a non tabbed dialog this function simply returns NULL.
				For a tabbed dialog it returns the PageID of the dialog page specified by
				the last call to TalkToPage.
	Errors:		-
	Scope:		public
	SeeAlso:	DialogTabOp::TalkToPage; DialogOp::GetReadWriteWindowID();

********************************************************************************************/

CDlgResID DialogOp::GetCurrentPageID()
{
	// Just return the current id of the tab page in use.
	return ReadWritePage;
}

/********************************************************************************************

>	static DialogOp* DialogOp::FindDialogOp(CDlgResID ResID)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	17/4/2006
	Inputs:		ResID - The resource ID of the dialog to find
	Outputs:	-
	Returns:	ptr to DialogOp
				NULL is returned if not found
	Purpose:	Looks for a given DialogOp by using its ResourceID
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DialogOp* DialogOp::FindDialogOp(CDlgResID ResID)
{
	List*		pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogOp));
	DialogOp*	pDialogOp = (DialogOp*)pList->GetHead();
	
	while (pDialogOp != NULL)
	{
		if (pDialogOp->DlgResID == ResID)
			return (pDialogOp);

		pDialogOp = (DialogOp*)pList->GetNext(pDialogOp);
	}

	return NULL;
}

/********************************************************************************************

>	BOOL DialogOp::IsVisible()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this DialogOp is on screen
	Purpose:	Used by the tool bar customize system to find out if a tool bar is
				visible or not. 
	Errors:		-
	SeeAlso:	-

This function is a hangover from old bar days. You probably don't want to use it.
Use Show() etc. which are orthogonal to this. This merely tells you whether there
is a window ID there... FOR COMPATIBILITY ONLY

********************************************************************************************/

BOOL DialogOp::IsVisible()
{
	return (WindowID != NULL);
}

/********************************************************************************************

>	virtual void DialogOp::SetVisibility(BOOL Visible)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		Open - 	if TRUE then open DialogOp in its last pos if it is
						currently closed.
						if FALSE, it is closed
	Outputs:	-
	Returns:	-
	Purpose:	Used by the tool bar customize system to open and close a tool bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogOp::SetVisibility(BOOL Visible)
{
	if (Visible)
	{
		if (!WindowID)
		{
			Create();
		}
		Open();
	}
	else
	{
		Close();
	}
}


// -----------------------------------------------------------------------------------------
// DialogTabOp functions 
 
/********************************************************************************************
>			DialogTabOp(CDlgResID DialogResID, CDlgMode Mode
		 				CCRuntimeClass* Class = CC_RUNTIME_CLASS(DialogOp));


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/93
				DummyDialogResID: 	An ID which is unique to this dialog. Even though 
									a Dialog resource is not specified for the tab dialog
									itself (only it's pages) we need to uniquly identify it.
									(so we can record it's position, etc.)  
				Mode:		 		The dialog's mode (modal, medeless)
	Purpose:	The Constructor of the DialogTabOp class simply sets the resource ID of the 
				dialog and its modality.

********************************************************************************************/



// distinguish a tab dialog message from a message for any other type of dialog
DialogTabOp::DialogTabOp(CDlgResID DummyDialogResID,  
						 CDlgMode Mode,
		 				 CCRuntimeClass* Class,
		 				 INT32 OpeningPage):DialogOp(DummyDialogResID, Mode, 0, /*NULL, NULL,*/
		 				 						   Class, OpeningPage)
{
	// The base class does most of the work as you can see
};

/********************************************************************************************

>	BOOL DialogTabOp::AddAPage(CDlgResID DialogResID, CGadgetID Gadget=0)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		DialogResID: Dialog resource ID of the page you want to add to the Tabbed 
							 dialog. 
				GadgetID:	 The gadget ID of the book control to add it to, or zero
							 for the default one (either specified using GetDefaultBookGadget
							 or just the first one it finds). You only need specify this if
							 the dialog contains multiple book controls (if there are
							 multiple book controls and only one is kernel managed, the
							 best thing to do is to override GetDefaultBookGadget)
	Outputs:	-
	Returns:	-
	Purpose:	This function adds a dialog page to the tabbed dialog. 
	
				Note: 
				
				This function can be called many times before the dialog is made 
				visible. For a modal dialog this function should be called in response to a 
				dialog DIM_CREATE message.

				To keep things simple all Dialog pages should be the same size please.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogTabOp::AddAPage(CDlgResID DialogResID, CGadgetID Gadget/*=0*/)
{	  
	// Ask the Dialog Manager to add the page 
	return (DlgMgr->AddAPage(this, DialogResID, Gadget)); 
}; 

/********************************************************************************************

>	virtual BOOL DialogTabOp::RegisterYourPagesInOrderPlease()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		-
	Outputs:	-
	Returns:	return TRUE if all calls to AddAPage returned TRUE
	Purpose:	This virtual function will get called to give you a chance to add pages to your
				DialogTabOp. This function will usually make multiple calls to AddAPage to 
				register the initial set of pages to be contained in the tab dialog. The pages
				should all be the same size and be registered in the left to right order in which
				you wish them to appear. 

				This function get's called directly from the Create method, Don't call it 
				explicitly yourself. If you return FALSE from this method then the Create 
				method will fail. 

	Errors:		-
	SeeAlso:	DialogTabOp::AddPage
	SeeAlso:	DialogOp::Create

********************************************************************************************/

BOOL DialogTabOp::RegisterYourPagesInOrderPlease()
{
	ERROR2(FALSE, "The DialogTabOp has not had any pages added during creation"); 
} 


/********************************************************************************************

>	void DialogTabOp::SetName(String_256* pName) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		pName: The Dialogs name
	Outputs:	-
	Returns:	-
	Purpose:	This function should be called BEFORE calling create. It sets the title bar
				name of the dialog
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DialogTabOp::SetName(String_256* pName)
{
	MyName = *pName; 
	return; // done
} 


/********************************************************************************************

>	String_256 DialogTabOp::GetName(void) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The dialog's name
	Purpose:	Returns the name of the dialog
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_256* DialogTabOp::GetName(void)
{
	return &MyName;
} 



/********************************************************************************************

>	void DialogTabOp::SetPropertyPageModified(BOOL Modified)


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		the modified value to set
	Returns:	-
	Purpose:	Sets the modified property of the currently active property page of the current 
				tabbed dialog (if it exists)

********************************************************************************************/

void DialogTabOp::SetPropertyPageModified(BOOL Modified)
{
	if (DlgMgr != NULL)
		DlgMgr->SetPropertyPageModified(Modified);
	else
		ERROR3("Dialog Manager is NULL in DialogTabOp::SetPropertyPageModified");
}

/********************************************************************************************

>	DialogTabOp::~DialogTabOp()			  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The DialogTabOp destructor destroys the instance of the DialogTabOp and all  
				associated resources. If the dialog was open then it is closed. 

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DialogTabOp::~DialogTabOp()
{
	// We must do this before we become a DialogOp (due changes to vtable as
	// destructors are called)
	if (WindowID != NULL)
	{
		DlgMgr->Delete(WindowID, this);
		WindowID = NULL;
	}

	//  Let's get the base class destructor to do all the work shall we

};         



/********************************************************************************************

>	BOOL DialogTabOp::TalkToPage(CDlgResID PageID)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Inputs:		PageID: Resource ID of the page you wish to communicate with
	Outputs:	-
	Returns:	FALSE if the page has not yet been created. 
				This function is guaranteed to return TRUE if it is called in responce to
				a message from the page.  

	Purpose:	Set's the Page that you wish to communicate with. Initially this page
				is set to NULL which means that all communication will be directed at the
				tabbed dialog itself. After this call all subsequent Get and Set functions will 
				be directed at the specified page within the dialog.
				
				eg. To set the string value of a gadget _R(IDC_BUTTON1) on page _R(IDD_PAGE1)
				
				TalkToPage(_R(IDD_PAGE1));
				SetStringGadgetValue(_R(IDC_BUTTON1), 
			  			   		 	 &Wibble);  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DialogTabOp::TalkToPage(CDlgResID PageID)
{
	ReadWritePage = PageID;

	// Determine if the page we are trying to talk to exists
   	return (GetReadWriteWindowID() != NULL);
}

/********************************************************************************************

>	virtual MsgResult DialogOp::Message( Msg* Message )
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/96
	Inputs:		Message: The message to be processed
	Purpose:	Performs message handling specific to DialogTabOps

********************************************************************************************/

MsgResult DialogTabOp::Message( Msg* Message )       
{
PORTNOTE("dialog","DialogTabOp::Message - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* pDlgMsg = (DialogMsg*) Message;
		if (pDlgMsg->PageID == 0)
		{
			// It's a message from the Property sheet itself
			if (pDlgMsg->GadgetID == _R(ID_HELP))
			{
				// Map to our standard help ID. This is no longer equal to _R(ID_HELP)
			   pDlgMsg->GadgetID = _R(ID_CC_HELP_BUTTON);
			}
		}
	} 
#endif
	return DialogOp::Message(Message); // Let the base class do the rest
}


// Never ever call this it exists cos DYNCREATE demands it. DYNCREATE has no concept 
// of an abstract class. 
DialogTabOp::DialogTabOp()
{                                   
	ENSURE(FALSE, "Invalid constructor called for DialogTabOp"); 
}
  
/********************************************************************************************

>	UINT32 DialogOp::SetTimer(UINT32 nIDEvent, UINT32 nElapse,
							void (* lpfnTimer)(void *) = NULL,
							void * param = NULL,
							BOOL OneShot = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		nIDEvent	Specifies a nonzero timer identifier.
				nElapse		Specifies the time-out value, in milliseconds.
				param		An opaque parameter sent to the timer function (along with nIDEvent)
	Returns:	The timer identifier of the new timer if Nonzero, Zero means a problem.
	Purpose:	Allows the user access to setting a timer caller back or event for a dialog box.
				The caller can either specify a call back procedure to be called when the timer
				goes off or if null is specified, a DIM_TIMER message will be sent.
				The return value is effectively the handle onto the timer system. It must be
				passed to the KillTimer member function to kill the timer. A Nonzero value
				indicates successful allocation of the timer; non-zero implies a problem.

	SeeAlso:	DialogOp::KillTimer; DialogManager::SetTimer;

********************************************************************************************/

UINT32 DialogOp::SetTimer(UINT32 nIDEvent, UINT32 nElapse, void (* lpfnTimer)(void *)/* = NULL*/, void * param/*=NULL*/, BOOL OneShot /*=FALSE*/)
{
	// Call the Dialog Manager
	return DialogManager::SetTimer( this, GetReadWriteWindowID(), nIDEvent, nElapse, lpfnTimer, param, OneShot );
}

/********************************************************************************************

>	BOOL DialogOp::KillTimer( INT32 nIDEvent )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		nIDEvent	Specifies a nonzero timer identifier.
	Returns:	True if the event was killed, FALSE if the specified timer event could not be found.
	Purpose:	Allows the user access to killing a timer caller back or event that has been
				set up for a dialog box.
				Kills the timer event identified by nIDEvent from the earlier call to SetTimer.
				Any pending WM_TIMER messages associated with the timer are removed from the
				message queue.
	SeeAlso:	DialogOp::SetTimer; DialogManager::SetTimer;

********************************************************************************************/

BOOL DialogOp::KillTimer(INT32 nIDEvent)
{
	// Call the Dialog Manager
	return DialogManager::KillTimer( this, GetReadWriteWindowID(), nIDEvent );
}

/********************************************************************************************

>	BOOL DialogOp::GetWindowPosition(RECT * pRect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Outputs:	Rect		pointer to the rectangle to fill in
	Returns:	True if the call was successful, FALSE otherwise.
	Purpose:	Allows the user to get the current window position.
	SeeAlso:	DialogOp::SetWindowPosition; DialogManager::GetWindowPosition;

********************************************************************************************/
// Technical Note:
// Don't even think of using GetWindowRect or GetClientRect as there are not set equivalents.
// MoveWindow is completely spappy and does not use the same coordinates on client items!!!!
// GetWindowPlacement at least has a SetWindowPlacement form which
// uses the same coordinate system.

BOOL DialogOp::GetWindowPosition( wxRect *pRect )
{
	// Call the Dialog Manager
	return DialogManager::GetWindowPosition(GetReadWriteWindowID(), pRect);
}

BOOL DialogOp::GetWindowPosition( RECT *pRect )
{
	// Call the Dialog Manager
	return DialogManager::GetWindowPosition(GetReadWriteWindowID(), pRect);
}

/********************************************************************************************

>	BOOL DialogOp::GetGadgetPosition(CGadgetID Gadget, RECT * pRect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Outputs:	Rect		pointer to the rectangle to fill in
	Returns:	True if the call was successful, FALSE otherwise.
	Purpose:	Allows the user to get the current position of the specified icon or control.
	SeeAlso:	DialogOp::SetGadgetPosition; DialogManager::GetGadgetPosition;

********************************************************************************************/

BOOL DialogOp::GetGadgetPosition( CGadgetID Gadget, wxRect *pRect )
{
	// Call the Dialog Manager
	return DialogManager::GetGadgetPosition(GetReadWriteWindowID(), Gadget, pRect);
}

BOOL DialogOp::GetGadgetPosition( CGadgetID Gadget, RECT *pRect )
{
	// Call the Dialog Manager
	return DialogManager::GetGadgetPosition(GetReadWriteWindowID(), Gadget, pRect);
}

/********************************************************************************************

>	BOOL DialogOp::SetWindowPosition(const RECT& Rect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Inputs:		Rect		pointer to the rectangle to fill in
	Returns:	True if the call was successful, FALSE otherwise.
	Purpose:	Allows the user to set the current window position.
	SeeAlso:	DialogOp::SetWindowPosition; DialogManager::GetWindowPosition;

********************************************************************************************/

BOOL DialogOp::SetWindowPosition( const wxRect &Rect )
{
	// Call the Dialog Manager
	return DialogManager::SetWindowPosition(GetReadWriteWindowID(), Rect);
}

BOOL DialogOp::SetWindowPosition( const RECT &Rect )
{
	// Call the Dialog Manager
	return DialogManager::SetWindowPosition(GetReadWriteWindowID(), Rect);
}

/********************************************************************************************

>	BOOL DialogOp::SetGadgetPosition(CGadgetID Gadget, const RECT& Rect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Inputs:		Rect		pointer to the rectangle to fill in
	Returns:	True if the call was successful, FALSE otherwise.
	Purpose:	Allows the user to set the current position of the specified icon or control.
	SeeAlso:	DialogOp::SetGadgetPosition; DialogManager::GetGadgetPosition;

********************************************************************************************/

BOOL DialogOp::SetGadgetPosition( CGadgetID Gadget, const wxRect &Rect )
{
	// Call the Dialog Manager
	return DialogManager::SetGadgetPosition(GetReadWriteWindowID(), Gadget, Rect);
}

BOOL DialogOp::SetGadgetPosition( CGadgetID Gadget, const RECT &Rect )
{
	// Call the Dialog Manager
	return DialogManager::SetGadgetPosition(GetReadWriteWindowID(), Gadget, Rect);
}

/********************************************************************************************

>	INT32 DialogOp::GetScreenDpi()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Returns:	0 or the dpi of the screen.
	Purpose:	Allows the user to find out the screen dpi.
	SeeAlso:	DialogManager::GetScreenDpi;

********************************************************************************************/

INT32 DialogOp::GetScreenDpi()
{
	return DialogManager::GetScreenDpi();
}

/********************************************************************************************

>	BOOL DialogOp::GetScreenSize(INT32 * pWidth, INT32 * pHeight)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Outputs:	pWidth	the screen width in pixels
				pHeight	the screen height in pixels
	Returns:	True if worked ok, False otherwise.
	Purpose:	Allows the user to find out the screen size.
	SeeAlso:	DialogManager::GetScreenSize;

********************************************************************************************/

BOOL DialogOp::GetScreenSize(INT32 * pWidth, INT32 * pHeight)
{
	ERROR2IF(pWidth == NULL || pHeight == NULL,FALSE,"GetScreenSize Bad params!");
	return DialogManager::GetScreenSize(pWidth, pHeight);
}

/*******************************************************************************************
>	BOOL UpdateStringGadgetValue(CGadgetID Gadget, StringBase* NewString);	
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/09/94
	Inputs:		Dialog - pointer to the DialogOp containing the gadget.
				Gadget - the gadget ID to update.
				NewString - the new contents of the gadget.
	Returns:	TRUE if the dialogue was updated.
				FALSE if it wasn't.
	Purpose:	Similar to SetStringGadgetValue but compares the contents of the gadget in
				the dialogue before the update and dosen't bother if the new value is the
				same as the current one.  This reduces dialogue flicker.
	See also:	DialogOp::SetStringGadgetValue


*******************************************************************************************/
BOOL DialogOp::UpdateStringGadgetValue(CGadgetID Gadget, StringBase* NewString)
{
	String_256 OldString = GetStringGadgetValue(Gadget, NULL, -1);

	if (OldString != *NewString)
	{
		SetStringGadgetValue(Gadget, *NewString);
		return TRUE;
	}
	else
	{				
		return FALSE;
	}
}


/*******************************************************************************************
>	BOOL UpdateStringGadgetValue(DialogOp* Dialog, UINT32 Gadget, StringBase* NewString);	
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/09/94
	Inputs:		Dialog - pointer to the DialogOp containing the gadget.
				Gadget - the gadget ID to update.
				NewString - the new contents of the gadget.
	Returns:	TRUE if the dialogue was updated.
				FALSE if it wasn't.
	Purpose:	Similar to SetStringGadgetValue but compares the contents of the gadget in
				the dialogue before the update and dosen't bother if the new value is the
				same as the current one.  This reduces dialogue flicker.
	See also:	DialogOp::SetStringGadgetValue

This is a complete bodge and should be made part of the DialogOp class - here for
compatibility.

*******************************************************************************************/
BOOL UpdateStringGadgetValue(DialogOp* Dialog, UINT32 Gadget, StringBase* NewString)
{
	return Dialog->UpdateStringGadgetValue(Gadget, NewString);
}




CTreeItemID DialogOp::SetTreeGadgetItem(CGadgetID Gadget, CTreeItemID hParent, const StringBase& str, CTreeItemID hInsAfter, INT32 iImage, CCObject* pObj)
{
	return (DlgMgr->SetTreeGadgetItem(GetReadWriteWindowID(), Gadget, hParent, str, hInsAfter, iImage, pObj));
}

CCObject* DialogOp::GetTreeGadgetItemData(CGadgetID Gadget, CTreeItemID hItem)
{
	return (DlgMgr->GetTreeGadgetItemData(GetReadWriteWindowID(), Gadget, hItem));
}

BOOL DialogOp::SelectTreeGadgetItem(CGadgetID Gadget, CTreeItemID hItem, BOOL bNewState)
{
	return (DlgMgr->SelectTreeGadgetItem(GetReadWriteWindowID(), Gadget, hItem, bNewState));
}

CTreeItemID	DialogOp::GetTreeGadgetRootItem(CGadgetID Gadget)
{
	return (DlgMgr->GetTreeGadgetRootItem(GetReadWriteWindowID(), Gadget));
}

CTreeItemID	DialogOp::GetTreeGadgetFirstSelectedItem(CGadgetID Gadget)
{
	return (DlgMgr->GetTreeGadgetFirstSelectedItem(GetReadWriteWindowID(), Gadget));
}

BOOL DialogOp::TreeGadgetExpandItem(CGadgetID Gadget, CTreeItemID hItem)
{
	return (DlgMgr->TreeGadgetExpandItem(GetReadWriteWindowID(), Gadget, hItem));
}

CTreeItemID	DialogOp::GetTreeGadgetNextVisItem(CGadgetID Gadget, CTreeItemID hItem)
{
	return (DlgMgr->GetTreeGadgetNextVisItem(GetReadWriteWindowID(), Gadget, hItem));
}

CTreeItemID	DialogOp::GetTreeGadgetFirstChildItem(CGadgetID Gadget, CTreeItemID hItem)
{
	return (DlgMgr->GetTreeGadgetFirstChildItem(GetReadWriteWindowID(), Gadget, hItem));
}

size_t DialogOp::GetTreeGadgetChildrenCount(CGadgetID Gadget, CTreeItemID hItem, BOOL bRecursive)
{
	return (DlgMgr->GetTreeGadgetChildrenCount(GetReadWriteWindowID(), Gadget, hItem));
}

/********************************************************************************************

>	OpDescriptor * DialogOp::GetGadgetOpDescriptor(CGadgetID Gadget)

	Author:		Alex Bligh
	Created:	03-Mar-2005
	Inputs:		Gadget - the Gadget to get the OpDescriptor for
	Outputs:	-				   
	Returns:	Pointer the gadget's associated OpDescriptor
	Purpose:	Finds the OpDescriptor associated with a gadget
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpDescriptor * DialogOp::GetGadgetOpDescriptor(CGadgetID Gadget)
{
	return (DlgMgr->GetGadgetOpDescriptor(GetReadWriteWindowID(), Gadget));
}


/********************************************************************************************

>	BOOL DialogOp::SendMessageToControl(OpDescriptor * OpDesc, DialogMsg* Msg, BOOL Processed=FALSE)

	Author:		Alex Bligh
	Created:	03-Mar-2005
	Inputs:		Msg: The message to handle
	Outputs:	-				   
	Returns:	TRUE to destroy the window, else FALSE
	Purpose:	Conditionally sends a message to a particular control pointed to by an opdescriptor,
	Errors:		-
	SeeAlso:	-

The message is only sent if it is destined for that OpDescriptor - that's a broadcast,
or if the dialog message has a gadget ID equal to that of the opdescriptor

********************************************************************************************/

BOOL DialogOp::SendMessageToControl(OpDescriptor * OpDesc, DialogMsg* DlgMsg, BOOL Processed)
{
	BOOL DestroyWindow = FALSE;
	if (OpDesc)
	{
		// If the message is DIM_CANCEL then we must inform all OpDescriptors on the 
		// bar that their controls are about to be destroyed. 
		if (DlgMsg->DlgMsg == DIM_CANCEL||DlgMsg->DlgMsg == DIM_BAR_DEATH)
		{
			// Tell the OpDescriptor its controls are about to be destroyed
			OpDescControlDestroyMsg op(OpDesc, OpDesc->GetBarControlInfo()->ControlID, OpDesc->GetBarControlInfo()->ResourceID, this);
			OpDesc->Message(&op);
			DestroyWindow = TRUE; // Cos we are cancelling
		}
		else if  (DlgMsg->DlgMsg == DIM_CREATE)
		{
			// Tell the OpDescriptor its controls have been created 
			OpDescControlCreateMsg op(OpDesc, OpDesc->GetBarControlInfo()->ControlID, OpDesc->GetBarControlInfo()->ResourceID, this);
			OpDesc->Message(&op);
		}		
	}
	return DestroyWindow;
}
