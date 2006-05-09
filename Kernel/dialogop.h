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

// Declaration of the following classes
//
//	DialogOp 		   Abstract base class which all dialog classes derive from
//		MenuPrefDlg	   Menu preferences demo (This has a very short life!)
//  	DebugTreeDlg   Dialog showing Camelots debug tree


/*
*/       


#ifndef INC_DIALOGOP
#define INC_DIALOGOP         
     
#include "ops.h"  	   	// Operation class  
#include "dlgtypes.h"  	// The DialogOp types
#include "dlgmgr.h" 	// For the wierd inclass typedef in GetLongGadgetValue

class String_256;    
class Node;
class RenderRegion;
class DocRect;
class DialogEventHandler;
struct ReDrawInfoType;

  
/********************************************************************************************

>	class DialogOp: public Operation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93    
	
	Purpose:	The abstract DialogOp class is used as the base class for all Dialog box
				classes in Camelot. It provides methods required to create dialogs and to 
				communicate with gadgets.  
				
	SeeAlso:	DialogManager

********************************************************************************************/

class DialogOp: public Operation
{       
	friend class DialogEventHandler;
	friend class DialogManager;

	CC_DECLARE_DYNCREATE( DialogOp )  
public:      
	// The Constructor of the DialogOp class simply sets the resource ID of the dialog
	// and its modality. 
	DialogOp(CDlgResID DialogResID, CDlgMode Mode, CDlgResID SubDialogID = 0,
			 /*HINSTANCE MainInst = 0, HINSTANCE SubInst= 0, */
			 CCRuntimeClass* Class = CC_RUNTIME_CLASS(DialogOp),
			 INT32 OpeningPage = -1, CWindowID ParentWnd = NULL);

	virtual BOOL IsABar() { return FALSE; }
	virtual BOOL IsAGallery() {return FALSE; }
	virtual BOOL IsVertical() { return FALSE; }
  			
	// The create method reserves everything needed to guarantee that the dialog will 
	// appear (it does not show the dialog). 
	virtual BOOL Create(); 

	// The open method displays the dialog box on the display. 
	void Open(); 

	// The close method removes the dialog from the display but keeps all system 	 	
	// resources associated with it. It hides the dialog.  
	void Close(); 

	// BODGE *** Don't use this it's marked for destruction
	BOOL IsOpen();

	// Is the DialogOp's window visible	(replaces the old IsOpen)
	BOOL IsWindowVisible();

	// Is the DialogOp's custom control dropdown open
	BOOL IsCustomComboDropdownVisible(CGadgetID Gadget);

	// Used in conjunction with the above, this command can be used to close the controls
	// dropdown on the event of a suitable keypress
	BOOL CloseDropdown (CGadgetID Gadget, BOOL CloseVal);

	// Used to test if the DialogOp has an associated window
	BOOL HasWindow();

	// Brings an open dialog to the top	of the z-order
	BOOL BringToTop();

	// Used to determine if a particular gadget can be ticked or not, it only
	// makes sense to tick buttons at the moment
	BOOL IsGadgetTickable(CGadgetID Gadget); 

	// Used to abort the dragging of the custom colour picker control
	// from within the colour editor
	BOOL ColourPickerAbort(CGadgetID Gadget, WPARAM wParam=0);

	// The DialogOP destructor destroys the instance of the DialogOp and all  	
	// associated resources. If the dialog was open then it is closed. 
	~DialogOp();        
	
	// The following methods call same named functions defined in the DialogManager.
	// The parameters of the DialogManager functions are identical except for an additional
	// CWindowID parameter.  
	
	//--------------------------------------------------------------------------------------
	// Methods to aid rendering into the dialogue via render regions
	RenderRegion* CreateGRenderRegion(DocRect* pRequiredSize, ReDrawInfoType* ExtraInfo,
										BOOL UseSelViewColContext = FALSE);
	BOOL 		  DestroyGRenderRegion(RenderRegion* pRender);

	RenderRegion* CreateOSRenderRegion(DocRect* pRequiredSize, ReDrawInfoType* ExtraInfo,
										BOOL UseSelViewColContext = FALSE);
	BOOL 		  DestroyOSRenderRegion(RenderRegion* pRender);

	//--------------------------------------------------------------------------------------
	// Methods to set gadget values 

	void SetComboListLength( CGadgetID Gadget );
		
	BOOL SetUnitGadgetValue( CGadgetID Gadget, 
					         UnitType Unit, 
			                 MILLIPOINT value, 
			                 BOOL EndOfList = TRUE, 
			     	    	 INT32 ListPos = 0);

	BOOL SetDimensionUnitGadgetValue(CGadgetID Gadget, 
					     	    	 UnitType  units, 
			                		 double    value, 
									 Node*     pNode,
									 BOOL      IncludeUnitSpecifier = TRUE,
			                 		 BOOL      EndOfList = FALSE, 
			     	    	 		 INT32       ListPos   = -1);

	BOOL SetLongGadgetValue(CGadgetID Gadget, 
							INT32 value,    
							BOOL EndOfList = TRUE, 
			    			INT32 ListPos = 0);

	BOOL SetDoubleGadgetValue(CGadgetID Gadget, 
							double value,    
							BOOL EndOfList = TRUE, 
			    			INT32 ListPos = 0);

	BOOL SetBoolGadgetSelected(CGadgetID Gadget,
								BOOL IsSelected,
								INT32 ListPos = 0);

	BOOL SetRadioGroupSelected(CGadgetID *GroupGadgets,
								CGadgetID SelectedGadget);

	BOOL SetStringGadgetValue(CGadgetID Gadget, 
							  UINT32 IDStr,    
							  BOOL EndOfList = TRUE, 
			     			  INT32 ListPos = 0);

	BOOL SetStringGadgetValue(CGadgetID Gadget, 
							  const StringBase& StrVal,   
							  BOOL EndOfList = TRUE, 
			     			  INT32 ListPos = 0);

	//////////////////////////////////////////////////////////////////////////////////////
	// CUSTOM COMBOBOX STUFF ....

	BOOL SetCustomComboGadgetValue (CGadgetID Gadget, 
							  CustomComboBoxControlDataItem* TheItem,   
							  BOOL EndOfList = TRUE, 
			     			  INT32 ListPos = 0);

	BOOL SelectCustomComboGadgetValueOnString (CGadgetID Gadget,
											   StringBase* StrVal);

	//////////////////////////////////////////////////////////////////////////////////////
	
	void					BuildResDropList(const CGadgetID DropListID,
											const BOOL		bIncludeAuto = TRUE,
											const BOOL		bVerbose = FALSE,
											const INT32 		Default=0
											);
	void					SetResDropListValue( const CGadgetID DropListID,
											const BOOL		bIncludeAuto = TRUE,
											const INT32 		iValue=0
											);
	INT32						GetResDropListValue(const CGadgetID DropListID,
												const BOOL		bIncludeAuto = TRUE,
												BOOL*			bValid = NULL
												);

	BOOL SetGadgetRange(CGadgetID Gadget, INT32 Min, INT32 Max, INT32 PageInc = 1);  
	
	BOOL SetSelectedValueIndex(CGadgetID Gadget, INT32 Index);

	BOOL SetSelectedValueRange(CGadgetID Gadget,
								WORD StartIndex,
								WORD EndIndex,
								BOOL Select = TRUE);

	BOOL SetDimensionGadgetValue( 	CGadgetID Gadget, 
			                		MILLIPOINT value, 
									Node* pNode,
									BOOL IncludeUnitSpecifier = TRUE,
			                 		BOOL EndOfList = FALSE, 
			     	    	 		INT32 ListPos = 0);

	BOOL SetMemoryGadgetValue( CGadgetID Gadget, 
							   UINT32 value, 
							   BOOL EndOfList = TRUE, 
							   INT32 ListPos = 0);

	BOOL SetGadgetHelp( CGadgetID Gadget, 
					    UINT32 BubbleID, 
					    UINT32 StatusID, 
					    UINT32 ModuleID = 0);   

    //--------------------------------------------------------------------------------------                              
	// Methods to get gadget values                                                         
	
	MILLIPOINT GetUnitGadgetValue(CGadgetID Gadget,  
				       		  UnitType DefaultType,
			        	      MILLIPOINT StartRange, 
			                  MILLIPOINT EndRange,
			                  UINT32 IDSInvalidMsg = 0,  
			                  BOOL* Valid = NULL);
	
	INT32 GetLongGadgetValue(CGadgetID GadgetID, 
			            	INT32 StartRange, 
			            	INT32 EndRange, 
		           	    	UINT32 IDSInvalidMsg = 0,
		           	    	BOOL* Valid = NULL,
		           	    	DialogManager::PFNSTRINGTOINT32 pfnParser = Convert::StringToLong);

	double GetDoubleGadgetValue(CGadgetID GadgetID, 
			            	double StartRange, 
			            	double EndRange, 
		           	    	UINT32 IDSInvalidMsg = 0,
		           	    	BOOL* Valid = NULL,
		           	    	DialogManager::PFNSTRINGTODOUBLE pfnParser = Convert::StringToDouble);

	BOOL GetDoubleAndUnitGadgetValue(double*   pMPValue,
									 double*   pUnitValue,
									 UnitType* pUnitType,
									 CGadgetID GadgetID,
									 Node*     pNode);

	BOOL GetBoolGadgetSelected(CGadgetID GadgetID,
								UINT32 IDSInvalidMsg = 0,
								BOOL* Valid = NULL,
								INT32 ListPos = -1);

	INT32 GetSelectedValueIndex(CGadgetID GadgetID);

	CGadgetID GetRadioGroupSelected(CGadgetID *GroupGadgets);

	String_256 GetStringGadgetValue(CGadgetID GadgetID, BOOL* Valid = NULL, INT32 ListPos=-1);
	
	BOOL GetGadgetRange(CGadgetID GadgetID, INT32* Min, INT32* Max);  

	MILLIPOINT GetDimensionGadgetValue( CGadgetID Gadget, Node* pNode, BOOL* Valid = NULL, INT32 ListPos=-1);

	UINT32 GetMemoryGadgetValue(CGadgetID Gadget,  
			        	       UINT32 StartRange, 
			                   UINT32 EndRange,
			                   UINT32 IDSInvalidMsg = 0,  
			                   BOOL* Valid = NULL);
	
	
	//--------------------------------------------------------------------------------------
	
	BOOL DeleteAllValues(CGadgetID GadgetID);
	BOOL DeleteValue(CGadgetID GadgetID, BOOL EndOfList = TRUE, INT32 ListPos = 0);
	BOOL GetValueCount(CGadgetID GadgetID, INT32* Count);
	BOOL GetValueIndex(CGadgetID GadgetID, WORD* Index);
	BOOL GetValueIndex(CGadgetID GadgetID, INT32* Index);

	INT32 GetSelectedCount(CGadgetID GadgetID);
	INT32 GetFirstSelectedItem(CGadgetID GadgetID);
	INT32* GetSelectedItems(CGadgetID GadgetID);

	BOOL EnableGadget(CGadgetID GadgetID, BOOL Enabled);
	BOOL HideGadget(CGadgetID GadgetID, BOOL Hide);
	BOOL GadgetRedraw(CGadgetID GadgetID, BOOL Redraw);

	//--------------------------------------------------------------------------------------

	BOOL SetKeyboardFocus(CGadgetID gadID);		// sets the keyboard focus
	BOOL HighlightText(CGadgetID gadID, INT32 nStart = 0, INT32 nEnd = -1);
	BOOL CaptureMouse(CGadgetID gadID); //capture the mouse 
	BOOL ReleaseMouse(CGadgetID gadID); //release the mouse 
	
	//--------------------------------------------------------------------------------------
	
	void PaintGadgetNow(CGadgetID gid);		// immediately updates gadget's appearance
	void InvalidateGadget(CGadgetID Gadget, BOOL EraseBackground=TRUE);	// Force the gadget to be repainted

	// Force *part* of a kernel-drawn gadget to repaint
	void InvalidateGadget(CGadgetID Gadget,
							ReDrawInfoType *ExtraInfo,
							DocRect *InvalidRect);

	// Scrolls (by blitting) the given area of the gadget, and invalidates
	// the bit that has 'scrolled into view'
	void ScrollKernelRenderedGadget(CGadgetID Gadget,
									DocRect *RectToScroll, DocCoord *ScrollBy);

	// Gets information on a cc_DialogDraw gadget
	BOOL GetKernelRenderedGadgetInfo(CGadgetID Gadget, ReDrawInfoType *Result);

    
    // -------------------------------------------------------------------------------------
	// Methods for setting the types of edit fields            
	
    void SetEditGadgetType(CGadgetID Gadget, EditGadgetType Type);
	void SetEditGadgetType(CGadgetID Gadget, UINT32 IDSValidChar);    
	 

	// --------------------------------------------------------------------------------------
	// Methods related to tree controls
	CTreeItemID SetTreeGadgetItem(CGadgetID Gadget, CTreeItemID hParent, const StringBase& str, CTreeItemID hInsAfter, INT32 iImage, CCObject* pObj = NULL);
	CCObject* 	GetTreeGadgetItemData(CGadgetID Gadget, CTreeItemID hItem);
	BOOL		SelectTreeGadgetItem(CGadgetID Gadget, CTreeItemID hItem, BOOL bNewState = TRUE);
	CTreeItemID	GetTreeGadgetRootItem(CGadgetID Gadget);
	CTreeItemID	GetTreeGadgetFirstSelectedItem(CGadgetID Gadget);
	BOOL		TreeGadgetExpandItem(CGadgetID Gadget, CTreeItemID hItem);
	CTreeItemID	GetTreeGadgetNextVisItem(CGadgetID Gadget, CTreeItemID hItem);
	CTreeItemID	GetTreeGadgetFirstChildItem(CGadgetID Gadget, CTreeItemID hItem);
	size_t		GetTreeGadgetChildrenCount(CGadgetID Gadget, CTreeItemID hItem, BOOL bRecursive = FALSE);


	// ------------------------------------------------------------------------------------- 
	
	void DualFunctionButton(CGadgetID ButtonGadget); // Will have no effect on platforms 
													 // which support right mouse button 
													 // clicks  

	// Method to set the bitmaps for a bitmap slider
	void SetGadgetBitmaps(CGadgetID GadgetID, UINT32 Bitmap1, UINT32 Bitmap2);
	void SetGadgetBitmaps(CGadgetID GadgetID, const CGadgetImageList& imagelist);
	void SetGadgetBitmap(CGadgetID Gadget, ResourceID Bitmap);
	ResourceID GetGadgetBitmap(CGadgetID Gadget);
	UINT32 GetGadgetImageCount(CGadgetID);
	
	// DY 23/9/99 allows you to specify indexes within a bitmap strip
	// for both selected and unselected states of a cc_bitmapbutton control
	void SetBitmapButtonIndexes(CGadgetID GadgetID, UINT32 UnselectedIndex, UINT32 SelectedIndex);


	BOOL MakeListBoxDragable(CGadgetID ListGadget);        
             
	virtual MsgResult Message(Msg* Message); 

	static BOOL IsADialogWindow(CWindowID WndID); 

	// Used to set the dialogs title
	BOOL SetTitlebarName(String_256* Name); 
 


	// This function should never get called. It is required because DYNCREATE
	// is a bodge  
	DialogOp(); 


	CWindowID WindowID;    // Window associated with dialog 
	
	BOOL UpdateStringGadgetValue(CGadgetID Gadget, StringBase* NewString);

	OpDescriptor * GetGadgetOpDescriptor(CGadgetID Gadget);

	BOOL SendMessageToControl(OpDescriptor * OpDesc, DialogMsg* Msg, BOOL Processed=FALSE);


protected:
	CDlgResID DlgResID;    // Dialog resource (for main, possibly only dialog)
	CDlgResID SubDlgID;    // Dialog resource (for secondary dialog)
	CDlgMode DlgMode;      // Dialog's mode { Modal, Modeless }
/*	HINSTANCE MainDlgInst;
	HINSTANCE SubDlgInst; */
	CWindowID ParentDlgWnd;

	DialogEventHandler* pEvtHandler;	// This is an opaque class as far as we are concerned

  	DialogManager* DlgMgr; // A pointer to camelot's dialog manager. This is stored so we
  						   // don't have to keep looking it up. 
	CDlgResID ReadWritePage; // This will be NULL for all classes directly derived from DialogOp
							 // It can only be set to a non NULL value for Dialogs which can
							 // have pages i.e. tabbed dialogs derived from DialogTabOp. 
	INT32 PageToOpen;			 // for tabbed dialogs allows opening tab to be chosen, defaults to -1

public:
	CWindowID GetReadWriteWindowID();

public:
	BOOL IsModal() { return (DlgMode == MODAL); }; 
	CDlgResID GetCurrentPageID();	// Returns the current page id. For use when in tabbed dialogs mode

public:
	// Setting up and killing timer events for dialog boxes
PORTNOTE("dialog","Removed HWND and timer usage")
#ifndef EXCLUDE_FROM_XARALX
	UINT32 SetTimer(UINT32 nIDEvent, UINT32 nElapse, void (CALLBACK EXPORT* lpfnTimer)(HWND, UINT32, UINT32, DWORD) = NULL);
	BOOL KillTimer(INT32 nIDEvent);
#endif

	// Get and set the position of a window or a gadget
	BOOL GetWindowPosition( wxRect *pRect );
	BOOL GetWindowPosition( RECT *pRect );
	BOOL GetGadgetPosition( CGadgetID Gadget, wxRect *pRect );
	BOOL GetGadgetPosition( CGadgetID Gadget, RECT *pRect );
	BOOL SetWindowPosition( const wxRect &Rect);
	BOOL SetWindowPosition( const RECT &Rect);
	BOOL SetGadgetPosition( CGadgetID Gadget, const wxRect &Rect );
	BOOL SetGadgetPosition( CGadgetID Gadget, const RECT &Rect );

	INT32 GetScreenDpi();
	BOOL GetScreenSize(INT32 * pWidth, INT32 * pHeight);

	static DialogOp*	FindDialogOp(CDlgResID ResID);

	BOOL			IsVisible();
	virtual void	SetVisibility(BOOL Visible);

protected:
	// Need to subclass your dialog controls? Override this to do so
	virtual BOOL AddControlsToHelper();
	virtual void RemoveControlsFromHelper();

	// helpers for the above, call these for each control you wish to subclass
	BOOL AddDialogControlToHelper(CGadgetID GadgetID);
	BOOL AddDialogControlToHelper(CGadgetID GadgetID, CDlgResID PageID); // overridden for tabbed dialogs
	BOOL RemoveDialogControlFromHelper(CGadgetID GadgetID);
	BOOL RemoveDialogControlFromHelper(CGadgetID GadgetID , CDlgResID PageID);

private:
	UINT32 MagicWord;
};  


/********************************************************************************************

>	class DialogTabOp: public DialogOp

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93    
	
	Purpose:	The abstract DialogTabOp class is used as the base class for all tabbed Dialog 
				box classes in Camelot. It is a simple specialisation of a DialogOp
				
	SeeAlso:	DialogManager

********************************************************************************************/
                              
 
class DialogTabOp: public DialogOp
{
	CC_DECLARE_DYNCREATE( DialogTabOp )  
public:      
	// The Constructor of the DialogTabOp class simply sets the resource ID of the dialog
	// and its modality.
	
	// Note the message handler class is deliberately set to DialogOp, as there is no need to
	// distinguish a tab dialog message from a message for any other type of dialog
	DialogTabOp(CDlgResID DummyDialogResID, 
				CDlgMode Mode,
			 	CCRuntimeClass* Class = CC_RUNTIME_CLASS(DialogOp),
			 	INT32	OpeningPage = -1);

	// This function will be called to give you a chance to create your pages. (See AddAPage)
	virtual BOOL RegisterYourPagesInOrderPlease(); 

	// A function to add a dialog page to the tabbed dialog. Note that this function can be
	// called many times before the dialog is made visible. 
   	BOOL AddAPage(CDlgResID DialogResID); 

	// Call this function before calling create. It sets the name displayed in the dialog's
	// title bar window
	void SetName(String_256* Name); 

	// Used to set the current page that we are currently reading or writing to.
	BOOL TalkToPage(CDlgResID PageID); 

	String_256* GetName(void); 

	virtual MsgResult Message(Msg* Message); 
	
	virtual void BrushEditDlgOnOK() {}  // does nothing in the base class

	void SetPropertyPageModified(BOOL Modified);

	// The DialogOP destructor destroys the instance of the DialogTabOp and all  	
	// associated resources. If the dialog was open then it is closed. 
	~DialogTabOp();
	
	DialogTabOp(); // I'm not here !, ignore me
private:


	String_256 MyName; // The name displayed in the title bar of the dialog. We need this because
					   // there is no dialog template associated with the Tab dialog itself. 	
};

// Complete bodge that should me made part of the above class
BOOL UpdateStringGadgetValue(DialogOp* Dialog, UINT32 Gadget, StringBase* NewString);

#endif


