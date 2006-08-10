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

// This file defines the DialogManager class

/*

*/

#ifndef INC_DLGMGR
#define INC_DLGMGR         

//------------------------------------------------------------------------------------------
// Include files
 
//#include "ccobject.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dlgtypes.h"  // Dialog types         - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "basestr.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "convert.h"   - in camtypes.h [AUTOMATICALLY REMOVED]
#include "stack.h"
//#include "xaraprop.h"

class Node;
class DialogOp;
class DialogTabOp;
class DialogColourInfo;
class CustomComboBoxControlDataItem;
class DialogEventHandler;

#define WM_CTL_COMMIT 	WM_USER + 211 	// Message sent by subclassed edit/combo boxed when enter or 
										// control is pressed.

#define WM_CTL_SETFOCUS WM_USER + 212 	// Message sent by subclassed edit/combo boxed to parent tool bar
										// to indicate  gaining or losing the focus.

  
//------------------------------------------------------------------------------------------

////////////////////////////////////////////////
//     SPECIAL TYPES FOR MERGING DIALOGS      // 
////////////////////////////////////////////////

// Special typedefs for WIN16 as these only exist in header files under NT.
// (See Win3.1 SDK Help "Dialog Box resource")

#if WIN16

// These *MUST* be byte-packed
#pragma pack(1)

typedef struct
{
    DWORD style;
    BYTE  cdit;
    WORD  x;
    WORD  y;
    WORD  cx;
    WORD  cy;
} DLGTEMPLATE;

typedef struct 
{
    WORD  x;
    WORD  y;
    WORD  cx;
    WORD  cy;
    WORD  id;
    DWORD style;
} DLGITEMTEMPLATE;

//Back to normal packing
#pragma pack()

#endif

WX_DECLARE_STRING_HASH_MAP( String_256, IdToSerializedPaneInfo );

/********************************************************************************************

>	class DialogPosition: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/93
	Purpose:	DialogPosition ListItem's are stored in the DialogManager's DialogPositionList
				they record the position of a dialog box before it is destroyed. They are 
				used to restore the position of the dialog when it is created the next time. 
				 
	SeeAlso:	DialogManager

********************************************************************************************/


class DialogPosition: public ListItem
{
public:                       
	CDlgResID DlgResourceID; // Resource ID of the dialog   
	List DlgWinList;         // A list of all live dialogs created from the DlgResourceID
	INT32 LastX; 				 // Last X position  
	INT32 LastY; 				 // Last Y position
	CDlgResID ActivePage;	 // We remember the active page in a tabbed dialog.
	INT32 ActivePageIndex;	 // The active page's index
};

/********************************************************************************************

>	class ActiveDlgStateItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/95
	Purpose:	Stored on  an ActiveDlgStateStack. See this class for a definition of its
				purpose.
	SeeAlso:	ActiveDlgStateStack

********************************************************************************************/

class ActiveDlgStateItem: public ListItem
{
public:
	wxWindow		   *pActiveWindow;		// The active window
	BOOL				fIsAModalDialog;	// Is the active window a modal dialog
};


/********************************************************************************************

>	class ActiveDlgStateStack: public Stack

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/95
	Purpose:	An ActiveDlgStateStack holds ActiveDlgStateItems. Whenever a MODAL dialog is
				closed we will need to restore the previously active window. What's more if
				the previously active window is a MODAL dialog , then we must disable
				the mainframe and other popups owned by the mainframe; so that the dialog 
				continues to behave modally. This stack stores this information. 

	SeeAlso:	ActiveDlgStateItem
	SeeAlso:	DialogManager

********************************************************************************************/

class ActiveDlgStateStack: public Stack
{
	
};




/********************************************************************************************

>	class CGadgetImageList : public CCObject

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2006
	Purpose: 	Kernel-usable class represeting a list of images

********************************************************************************************/

class CGadgetImageList : public CCObject
{
	CC_DECLARE_DYNAMIC(CGadgetImageList);

	class ResIDListItem : public ListItem
	{
	public:
		ResIDListItem(ResourceID newresID = 0) {m_ResID = newresID;}
		ResourceID m_ResID;
	};

public:
	CGadgetImageList(UINT32 width=0, UINT32 height=0) {m_width = width; m_height = height;}
	~CGadgetImageList();

	UINT32 Add(CDlgResID resID);
	ListItem* FindFirstBitmap(ResourceID* presID) const;
	ListItem* FindNextBitmap(ListItem* pListItem, ResourceID* presID) const;
	UINT32 GetWidth() const {return m_width;}
	UINT32 GetHeight() const {return m_height;}

private:
	List	m_BitmapIDList;
	UINT32	m_width;
	UINT32	m_height;
};




/********************************************************************************************

>	class CamelotTreeItemData : public wxTreeItemData

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2006
	Purpose: 	Hold pointer to CCObject associated with a wxTreeCtrl item

********************************************************************************************/

class CamelotTreeItemData : public wxTreeItemData
{
public:
	CamelotTreeItemData(CCObject* pObj = NULL) {m_pObject = pObj;}

	CCObject* GetObject() {return m_pObject;}

private:
	CCObject*	m_pObject;
};




/********************************************************************************************

>	class DialogManager : public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	There will only ever be one instance of the DialogManager. It resides in the 
				oil layer and is used to perform all platform specific dialog box operations. 
				It calls and is called by DialogOp. The interface to this object should be the
				same no matter what platform it is ported to. That is with the exception of the
				SendDialogMessage function which requires system specific types as its arguments. 
				
	SeeAlso:	-

********************************************************************************************/

class wxPropertySheetDialog;

class DialogBarOp;
               
class DialogManager: public CCObject  
{
	friend class DialogEventHandler;
	friend class wxPropertySheetDialog; // Is a friend because when a property sheet is created 
							   // it needs to call PostCreate.
public: 

	DialogManager();	 // Constructor
   
	// The DialogManager's create function creates a DialogOp window
   	static BOOL Create(DialogOp* DlgOp, /* HINSTANCE MainInstance, */ CDlgResID MainDlgID, 
					 /*HINSTANCE SubInstance, */ CDlgResID SubDlgID, 
						CDlgMode Mode = MODELESS, INT32 OpeningPage = -1, CWindowID ParentWnd = NULL);
private:

	static void CreateRecursor(wxWindow * pwxWindow);

	// Creates a DialogBarOp
	static BOOL CreateBar(DialogBarOp* DlgOp);
	static BOOL CreateTabbedDialog(DialogTabOp* pTabDlgOp, CDlgMode Mode, INT32 OpeningPage, CDlgResID MainDlgID );


	// Post create gets called after a dialog window has been created.
	static BOOL PostCreate(DialogOp * pDialogOp, INT32 OpeningPage);

public:
	// This should eventually be the only Create method that we require. If the DialogOp is 
	// a BarDialogOp then it will create a window of the correct type and attach it to the 
	// relevant docking bar. Otherwise it will create a regular modeless dialog. 
	CWindowID Create(DialogOp* DialogOp); 

	// The open method displays the dialog box on the display. 
	static void Open(CWindowID WindowID, DialogOp* DlgOp); 

	// The close method removes the dialog from the display but keeps all system 	 
	// resources associated with it. It hides the dialog.  
	static void Close(CWindowID WindowID, DialogOp* DlgOp); 

	// Place dialog above or below exist controls in dialog
	static BOOL MergeDialogs( CWindowID Dialog, CWindowID Mergee, bool fAbove );

	// Brings a dialog to the top-most z-order position
	static BOOL BringToTop(CWindowID WindowID, DialogOp* pDlgOp);

	// Get the book control
	static wxBookCtrlBase * GetBookControl(CWindowID WindowID, CGadgetID Gadget =0 );

	// Function to determine if a gadget is of a type that can be ticked 
	static BOOL IsGadgetTickable(CWindowID WindowID, 
						 		 CGadgetID Gadget);

	// Used to abort the dragging of the custom colour picker control
	// from within the colour editor
	static BOOL ColourPickerAbort(CWindowID WindowID, CGadgetID Gadget, WPARAM wParam = 0);

	// The delete method will delete all system resources associated with the Dialog 
	// box and any information which is being kept about the dialog.  
	static void Delete(CWindowID WindowID, DialogOp* DlgOp);   

	// Called by SendDialogMessage to send messages for mouse events - for cc_DialogDraw
	// controls, this then sends an extra chunk of useful information to the owning Dlg,
	// so that it can handle the click intelligently.
	static void ProcessMouseEvent(CDlgMessage DialogMessageType,
									wxWindow *pDlg, UINT32 wParam, INT32 lParam);

	// The following functions are called by same named functions in the DialogOp 	
	// class. The DialogOp functions are provided for ease of use and do not have a 
	// CWindowID argument. 


	static wxWindow * GetGadget(CWindowID WindowID, CGadgetID Gadget);

	static OpDescriptor * GetGadgetOpDescriptor(CWindowID WindowID, CGadgetID Gadget);

    // -------------------------------------------------------------------------------------         
	// Methods to set gadget values                                                        
	
	static BOOL SetUnitGadgetValue( CWindowID WindowID,
						 CGadgetID Gadget, 
					     UnitType Unit, 
			             MILLIPOINT value, 
			             BOOL EndOfList = TRUE, 
			     		 INT32 ListPos = 0
			             ); 

	static BOOL SetDimensionUnitGadgetValue(CWindowID WindowID, 
											CGadgetID Gadget, 
											UnitType  units, 
											double    value,
											Node*     pNode,
											BOOL      IncludeUnitSpecifier = TRUE,
											BOOL      EndOfList = FALSE, 
											INT32       ListPos   = 0); 

	static BOOL SetLongGadgetValue(CWindowID WindowID, 
							CGadgetID Gadget, 
							INT32 value, 
							BOOL EndOfList = TRUE, 
			     			INT32 ListPos = 0);   
			     			
	static BOOL SetDoubleGadgetValue(CWindowID WindowID, 
							CGadgetID Gadget, 
							double value, 
							BOOL EndOfList = TRUE, 
			     			INT32 ListPos = 0);   
			     			
	static BOOL SetStringGadgetValue(CWindowID WindowID,
							  CGadgetID Gadget, 
							  UINT32 IDStrID, 
							  BOOL EndOfList = TRUE, 
			     			  INT32 ListPos = 0);   
			     			    
	static BOOL SetStringGadgetValue(CWindowID WindowID,
							  CGadgetID Gadget, 
							  const StringBase& StrVal, 
							  BOOL EndOfList = TRUE, 
			     			  INT32 ListPos = 0);    

	static BOOL SetCustomComboGadgetValue(CWindowID WindowID,
									 CGadgetID Gadget, 
									 CustomComboBoxControlDataItem* TheItem,   
									 BOOL EndOfList = TRUE, 
			     					 INT32 ListPos = 0);

	static BOOL SelectCustomComboGadgetValueOnString (CWindowID WindowID,
													  CGadgetID Gadget, 
													  StringBase* StrVal);
			     			  
	static BOOL SetGadgetRange(CWindowID WindowID, 
					    CGadgetID Gadget, 
					    INT32 Min, 
					    INT32 Max, 
					    INT32 PageInc = 1);   

	static BOOL SetListBoxSelection( CWindowID WindowID, CGadgetID Gadget, INT32 Index, BOOL SelectIt, BOOL SingleSelection );

	static BOOL SetBoolGadgetSelected(CWindowID WindowID,
								CGadgetID Gadget,
								BOOL IsSelected,
								INT32 ListPos = 0);
					    
	static BOOL SetSelectedValueIndex(CWindowID WindowID, 
			    			   CGadgetID Gadget, 
						  	   INT32 Index);

	static BOOL SetSelectedValueRange(CWindowID WindowID,
								CGadgetID Gadget,
								WORD StartIndex,
								WORD EndIndex,
								BOOL Selected = TRUE);
	
	static BOOL SetDimensionGadgetValue( CWindowID WindowID, 
						 CGadgetID Gadget, 
			             MILLIPOINT value,
						 Node* pNode,
						 BOOL IncludeUnitSpecifier = TRUE,
			             BOOL EndOfList = FALSE, 
			     		 INT32 ListPos = -1
			             ); 

	static BOOL SetMemoryGadgetValue( CWindowID WindowID, 
									  CGadgetID Gadget, 
									  UINT32 value, 
									  BOOL EndOfList = TRUE, 
									  INT32 ListPos = 0
			 						); 

	static BOOL SetGadgetHelp( CWindowID WindowID, 
		  					   CGadgetID Gadget, 
					    	   UINT32 BubbleID, 
					    	   UINT32 StatusID, 
					    	   UINT32 ModuleID = 0);   

	// -------------------------------------------------------------------------------------      
	// Methods to get gadget values                                                         
	
	static MILLIPOINT GetUnitGadgetValue(CWindowID WindowID,
							  CGadgetID Gadget,  
				       		  UnitType DefaultType,
			        	      MILLIPOINT StartRange, 
			                  MILLIPOINT EndRange,
			                  UINT32 IDSInvalidMsg = 0,  
			                  BOOL* Valid = NULL);
	
	static INT32 GetLongGadgetValue(CWindowID WindowID,
								   CGadgetID Gadget, 
			            		   INT32 StartRange, 
			            		   INT32 EndRange, 
		           	    		   UINT32 IDSInvalidMsg = 0, 
		           	    		   BOOL* Valid = NULL,
								   Convert::PFNSTRINGTOINT32 pfnParser = Convert::StringToLong);

	static double GetDoubleGadgetValue(CWindowID WindowID,
								   CGadgetID Gadget, 
			            		   double StartRange, 
			            		   double EndRange, 
		           	    		   UINT32 IDSInvalidMsg = 0, 
		           	    		   BOOL* Valid = NULL,
								   Convert::PFNSTRINGTODOUBLE pfnParser = Convert::StringToDouble);

	static BOOL GetBoolGadgetSelected(CWindowID WindowID,
						CGadgetID Gadget,
		           	    UINT32 IDSInvalidMsg = 0,
		           	    BOOL* Valid = NULL,
		           	    INT32 ListPos = 0); 

	static String_256 GetStringGadgetValue(CWindowID WindowID,
										   CGadgetID Gadget,
										   BOOL* Valid = NULL,
										   INT32 ListPos = -1);
	
	static BOOL GetGadgetRange(CWindowID WindowID, 
					    CGadgetID Gadget, 
					    INT32* Min, 
					    INT32* Max);
	
	static MILLIPOINT GetDimensionGadgetValue( CWindowID WindowID, 
						 				CGadgetID Gadget, 
										Node* pNode,
										BOOL* Valid = NULL,
										INT32 ListPos = -1);

	static BOOL GetDoubleAndUnitGadgetValue(double*   pMPValue,
											double*   pUnitValue,
											UnitType* pUnitType,
											CWindowID WindowID,
											CGadgetID Gadget,
											Node*     pNode);

	static UINT32 GetMemoryGadgetValue(CWindowID WindowID,
									  CGadgetID Gadget,  
			        				  UINT32 StartRange, 
									  UINT32 EndRange,
			   						  UINT32 IDSInvalidMsg = 0,  
			         				  BOOL* Valid = NULL);
	
	// -------------------------------------------------------------------------------------
					    
	static BOOL DeleteAllValues(CWindowID WindowID, 
					 	CGadgetID Gadget);

	static BOOL DeleteValue(CWindowID WindowID, 
					 CGadgetID Gadget, 
					 BOOL EndOfList = TRUE, 
					 INT32 ListPos = 0);     
					 
	static BOOL GetValueCount(CWindowID WindowID, 
					   CGadgetID Gadget, 
					   INT32* Count); 
	
	static BOOL GetValueIndex(CWindowID WindowID, 
					   CGadgetID Gadget, 
					   INT32* Index);     

	static BOOL GetValueIndex(CWindowID WindowID, 
					   CGadgetID Gadget, 
					   WORD* Index);     

	static INT32 GetSelectedCount(CWindowID WindowID, CGadgetID Gadget);
	static INT32 GetFirstSelectedItem(CWindowID WindowID, CGadgetID Gadget);
	static INT32* GetSelectedItems(CWindowID WindowID, CGadgetID Gadget);
    
    // ------------------------------------------------------------------------------------- 
    // Functions for changing a gadgets state 
	static BOOL EnableGadget(CWindowID WindowID, CGadgetID Gadget, BOOL Enabled); 
	static BOOL HideGadget(CWindowID WindowID, CGadgetID Gadget, BOOL Hide); 
	static BOOL GadgetRedraw(CWindowID WindowID, CGadgetID Gadget, BOOL Redraw);
	static void Layout(CWindowID WindowID, BOOL CanYield=FALSE);

	// This function should be called after hiding\showing a control,
	// so the dialog can be resized
	static void RelayoutDialog(DialogTabOp* DlgOp);

    // ------------------------------------------------------------------------------------- 
    // Setting the keyboard input focus.
	static BOOL SetKeyboardFocus(CWindowID WindowID, CGadgetID Gadget);
	static BOOL DefaultKeyboardFocus();
	static BOOL HighlightText(CWindowID WindowID, CGadgetID Gadget, INT32 nStart = 0, INT32 nEnd = -1);
	static BOOL CaptureMouse(CWindowID WindowID, CGadgetID Gadget);
	static BOOL ReleaseMouse(CWindowID WindowID, CGadgetID Gadget);
	
    // ------------------------------------------------------------------------------------- 
	// Immediately updates gadget's appearance    
	static void PaintGadgetNow(CWindowID WindowID, CGadgetID gid);
	static void InvalidateGadget(CWindowID WindowID, CGadgetID Gadget, BOOL EraseBackround=TRUE);

				// Invalidates a specific portion of a cc_DialogDraw gadget
	static void InvalidateGadget(CWindowID WindowID, CGadgetID Gadget,
										ReDrawInfoType *ExtraInfo,
										DocRect *InvalidRect);

				// Scrolls (by blitting) the given area of the gadget, and invalidates
				// the bit that has 'scrolled into view'
	static void ScrollKernelRenderedGadget(CWindowID WindowID, CGadgetID Gadget,
											DocRect *RectToScroll, DocCoord *ScrollBy);

				// Gets information on a cc_DislogDraw gadget
	static BOOL GetKernelRenderedGadgetInfo(CWindowID WindowID, CGadgetID Gadget,
											ReDrawInfoType *Result);
	
                                        
    // -------------------------------------------------------------------------------------                                        
	// Methods for setting the types of edit fields      
	static void SetEditGadgetType(CWindowID WindowID, CGadgetID Gadget, EditGadgetType Type);
	static void SetEditGadgetType(CWindowID WindowID, CGadgetID, UINT32 IDSValidChar); 
   
    static void DualFunctionButton(CWindowID DialogWnd, 
							CGadgetID ButtonGadget);  
			
	// -------------------------------------------------------------------------------------
	// Method for making a listbox dragable 						
	static BOOL MakeListBoxDragable(CWindowID WindowID, 
							 CGadgetID Gadget);                                  

	static void SetComboListLength(CWindowID WindowID,CGadgetID Gadget);

	// Methods to allow you to set the bitmaps associated with a control
	static void SetGadgetBitmaps(CWindowID DialogWnd, CGadgetID Gadget, UINT32 Bitmap1, UINT32 Bitmap2);
	static void SetGadgetBitmaps(CWindowID DialogWnd, CGadgetID Gadget, const CGadgetImageList& images);
	static void SetGadgetBitmap(CWindowID WindowID, CGadgetID Gadget, ResourceID Bitmap);
	static ResourceID GetGadgetBitmap(CWindowID WindowID, CGadgetID Gadget);

	/* specific to the cc_BitmapButton control, allows you to specify both 
	selected and unselected bitmaps */
	static void SetBitmapButtonIndexes(CWindowID WindowID, CGadgetID Gadget, 
									UINT32 UnselectedIndex, UINT32 SelectedIndex);

//	static void SetGadgetBitmaps( wxWindow *pGadget,
//								  /* HINSTANCE hResInstance, */
//								  INT32 SelectedIndex = -1,
//								  INT32 UnselectedIndex = -1 );
	static UINT32 GetGadgetImageCount(CWindowID wnd, CGadgetID Gadget);

	static void DeInit(); 

    ~DialogManager(); 

	// This function called by CCamApp::PreTranslateMessage
	static BOOL IsADialogWindow(wxWindow *pWnd);

	// --------------------------------------------------------------------------------------
	// Methods related to tree controls
	static CTreeItemID 	SetTreeGadgetItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hParent, const StringBase& str, CTreeItemID hInsAfter, INT32 iImage, CCObject* pObj = NULL);
	static CCObject* 	GetTreeGadgetItemData(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem);
	static BOOL			SelectTreeGadgetItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem, BOOL bNewState = TRUE);
	static CTreeItemID	GetTreeGadgetRootItem(CWindowID wnd, CGadgetID Gadget);
	static CTreeItemID	GetTreeGadgetFirstSelectedItem(CWindowID wnd, CGadgetID Gadget);
	static BOOL			TreeGadgetExpandItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem);
	static CTreeItemID	GetTreeGadgetNextVisItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem);
	static CTreeItemID	GetTreeGadgetFirstChildItem(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem);
	static UINT32		GetTreeGadgetChildrenCount(CWindowID wnd, CGadgetID Gadget, CTreeItemID hItem, BOOL bRecursive = FALSE);

	// --------------------------------------------------------------------------------------
	// Functions which are specific to Tabbed dialogs
	// This function adds a page to a tabbed dialog
	static BOOL AddAPage(DialogTabOp* pDialogTabOp, CDlgResID DialogResID, CGadgetID Gadget=0); 

	// This function maps a property sheet window ID, and a PageID to a Page window ID
	static CWindowID GetPageWindow(CWindowID Win, CDlgResID PageID,  INT32* PageIndex = NULL);

	// Used to set the dialog's titlebar text
	static BOOL SetTitlebarName(CWindowID Win, String_256* Name);

    // When a dialog becomes active its window ID is assigned to hDlgCurrent. This is required
	// for IsDialogMessage handling

	static wxWindow *pDlgCurrent; 

	// Used to determine if there is an open modal dialog
	static BOOL ModalDialogOpen(DialogOp** pModal = NULL); 

	// Used to test the visibility status of the window
	static BOOL IsWindowVisible(CWindowID Win);

	// Used to test the visibility status of the window (primarily for stopping rampant escape
	// keys within the custom control)
	static BOOL IsCustomComboDropdownVisible(CWindowID WindowID, CGadgetID Gadget);

	// Used in conjunction with the above, this command can be used to close the controls
	// dropdown on the event of a suitable keypress
	static BOOL CloseDropdown (CWindowID WindowID, CGadgetID Gadget, BOOL CloseVal);

	// This routine comes in handy when we create a modal dialog box. If any modeless dialogs
	// exist then they need disabling. a modal dialog only disables its parent ! (Learnt from MSDN)
	static void EnableAllDialogs(BOOL Enable, wxWindow *pExceptMe = NULL); 

	// Functions to Save and restore the active dialog state
	static BOOL RecordActiveDialogState();
	static void  RestoreActiveDialogState();

	// functions to access property pages
	static CDlgResID GetActivePage(CWindowID WindowID, CGadgetID Gadget = 0);
	void SetPropertyPageModified(BOOL Value);

	// adds the control to the dialog helper class
	static BOOL AddDialogControlToHelper(CWindowID WindowID, CGadgetID Gadget);
	static BOOL RemoveDialogControlFromHelper(CWindowID, CGadgetID);

	static BOOL GetStatusLineText(String_256* ptext, CWindowID window);
	static CWindowID GetWindowUnderPointer(WinCoord * wc = NULL);

public:
	// Setting up and killing timer events for dialog boxes
	static UINT32 SetTimer(DialogOp *pDialogOp, CWindowID WindowID, UINT32 nIDEvent, UINT32 nElapse, void (* lpfnTimer)(void *) = NULL, void * param=NULL, BOOL OneShot=FALSE);
	static BOOL KillTimer( DialogOp * pDialogOp, CWindowID WindowID, INT32 nIDEvent);

	// Get and set the position of a window or a gadget
	static BOOL GetWindowPosition(CWindowID WindowID, wxRect *pRect);
	static BOOL GetWindowPosition(CWindowID WindowID, RECT *pRect);
	static BOOL GetGadgetPosition(CWindowID WindowID, CGadgetID Gadget, wxRect *pRect);
	static BOOL GetGadgetPosition(CWindowID WindowID, CGadgetID Gadget, RECT *pRect);
	static BOOL SetWindowPosition(CWindowID WindowID, const wxRect &Rect);
	static BOOL SetWindowPosition(CWindowID WindowID, const RECT &Rect);
	static BOOL SetGadgetPosition(CWindowID WindowID, CGadgetID Gadget, const wxRect &Rect);
	static BOOL SetGadgetPosition(CWindowID WindowID, CGadgetID Gadget, const RECT &Rect);

	static INT32 GetScreenDpi();
	static BOOL GetScreenSize(INT32 * pWidth, INT32 * pHeight);

private:
	static void DeletePropShtDetails(DialogTabOp* pOp);

	// Merges two dialog resources together
	static DLGTEMPLATE *MergeDialog( /*HINSTANCE MainInst, */  CDlgResID Main, 
									 /*HINSTANCE OtherInst, */ CDlgResID Other );
	
	static void SetGadgetIDToFocus(wxWindow *pDialogWnd);
	static List *GetControlList( CWindowID );
	static void DeleteControlList( CWindowID );
	
	// Helper functions for merging dialogs.
	static LPWSTR MovePastWideStr(LPWSTR pWideStr);
	static size_t SizeDlgHeader(DLGTEMPLATE *pHeader);
	static size_t SizeCtrlData(DLGITEMTEMPLATE *pData);

    // DiscardStrList holds a pointer to all strings which must be deleted after the dialog 
    // has been deleted. 
	static List DiscardStrList; 
	
	// Stores a list of the DialogPositions of all dialogs which have ever been created using the
	// Dialog Manager. Initially when a dialog is created the dialog is positioned centrally 
	// on the display. When the dialog is destroyed its position is recorded so that if it is 
	// ever created again it can have its position restored. 
	static List DialogPositionList; 
                            
    // The ScrollPageIncList holds a list of all scroll bars along with their page increment 
    // values which are specified by the SetGadgetRange function.                         
	static List ScrollPageIncList; 	

	static wxPropertySheetDialog* GetPropertySheetFromOp( DialogTabOp* pDialogTabOp );
	
	static BOOL HandleScrollBarMsg(wxWindow *pScrollWnd, 
											      UINT32 wParam, 
											      INT32 lParam,   
									   			  WORD CurrentThumbPos);

	static DialogPosition* FindDialogPositionRecord(CDlgResID DialogID);

									   			  
	// -------------------------------------------------------------------------------------
	//Control subclass procs
	  
	// Proc to enable edit controls to restrict their range of valid characters
	static INT32 FAR PASCAL EXPORT ValidateEditGadgetProc(wxWindow *pwnd, 
				 									     UINT32 message, 
											  			 UINT32 wParam, 
											   			 INT32 lParam); 
                             
	// Proc to enable a control to react to a right mouse button click                              
	static INT32 FAR PASCAL EXPORT RgtMouseButtonProc(wxWindow *pwnd, 
				 									 UINT32 message, 
											  		 UINT32 wParam, 
											   		 INT32 lParam); 

	static UINT32 MsgDragList; // Drag list message as used by the Drag list box common control
	static UINT32 MsgSlaveDrawItem;	// Used to field WM_DRAWITEM messages back to the controls which need them
//	static ATOM GetListAtom;

	static BOOL CustomControlMsg(wxWindow *pwnd, UINT32 wParam, INT32 lParam);

private:
	// The ActiveDialogStack is used to restore previously active dialogs after a Modal dialog
	// is closed.
	static ActiveDlgStateStack ActiveDlgStack;


protected:
	// Now our event handlers
	static void Event (DialogEventHandler *pEvtHandler, wxEvent &event);

	static void EnsurePanePreferenceDeclared(wxString key);
	static void InitPaneInfoHash();
	static void LoadPaneInfo(wxString key, wxPaneInfo &paneinfo);
	static void SavePaneInfo(wxString key, wxPaneInfo &paneinfo);
	static IdToSerializedPaneInfo * s_pPaneInfoHash;

public:
	static void FreePaneInfoHash();
};         





/********************************************************************************************

>	class ScrollPageInc : public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Purpose:	ScrollPageInc records the number of units a scroll bars thumb should move 
				for a page up or down scrollbar action. A ScrollPageInc is created and added 
				to the DialogManager's ScrollPageIncList whenever the 
				DialogManager::SetGadgetRange is called for a scrollbar or trackbar control. 
				
	SeeAlso:	DialogManager

********************************************************************************************/
                       
class ScrollPageInc: public ListItem
{  
public: 
	wxWindow		   *pDlgWindow;    // The dialog's window 
	wxWindow		   *pScrollBarWnd; // The scroll bars window
	INT32					PageInc; 	   // Scroll bar page up/down unit increase
};



/********************************************************************************************

>	class DlgDiscardString: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Purpose:	This simple class holds a pointer to a string which must be deleted when a 
				dialog is deleted. 
	SeeAlso:	DialogManager

********************************************************************************************/

class DlgDiscardString: public ListItem
{                         
public:
	wxWindow		   *DlgWindow; 
	StringBase		   *pStr; 
};                 



/********************************************************************************************

>	class ControlInfo: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Purpose:	A list of  ConrolInfo ListItem's are stored 'in' a dialog box. They are used 
				to store information which is required in a subclassed controls proc. Currently
			    Edit controls and Button controls create a ControlInfo item when they 
				are subclassed.  
	SeeAlso:	DialogManager

********************************************************************************************/

class ControlInfo: public ListItem
{      
public:   
	~ControlInfo();   
	wxWindow *pControlWnd;      // The windows control (Identifier)
    UINT32 IDSValidCh;      // Resource ID of the valid characters allowed in an edit control
 	FARPROC lpfnOldProc;  // Pointer to the old proc of the control before it was subclassed
 						  // this is generally called at the end of the new proc. 
};           


                                      
/********************************************************************************************

>	class CWindowIDItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/93
	Purpose:	A very simple class which contains a CWindowID. A list of CWindowIDItems are 
				stored in the DialogPosition list.   
	SeeAlso:	DialogPosition

********************************************************************************************/
                                    
class CWindowIDItem: public ListItem
{                 
	public:
	CWindowID DlgWin; // Dialog window
}; 





#endif		// !INC_DLGMGR
