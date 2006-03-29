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
// Header for kernel bar implementation

#ifndef INC_BARS
#define INC_BARS

//#include "pump.h"
#include "dialogop.h"
//#include "list.h"
//#include "ccfile.h"
#include "dockbar.h"
//#include "opdesc.h"
//#include "barmsg.h"
#include "biasgdgt.h"
#include "biasgain.h"

// Markn 7-3-97
// DEFAULT_TOOL_INFOBAR_SLOT defines the default tool info bar slot.  The infobar should always go in the slot underneath
// the last bar of the program.  In CorelXARA v1.5, this slot was slot 1 (i.e. underneath the Standard & Gallery bars,
// which appeared on slot 0)
//
// Since the arrival of the Animation system, the new control bar has highlighted the need for an extra
// slot to be used.  Instead of having to remember to change all tool bar .ini files to the new slot position,
// all we have to change in the future is this constant.

#define DEFAULT_TOOL_INFOBAR_SLOT 2

class BarCreate;
class LoadRegistryEntries;
class SaveRegistryEntries;

/********************************************************************************************

>	class SimpleBarControl : public CCObject

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	Contains all info about the control, plus a ptr to a message handler to 
				pump all the messages to.

********************************************************************************************/

class  SimpleBarControl : public CCObject
{
public:
	BarControlInfo 	BarCtrlInfo;
	OpDescriptor*	pOpDesc;
	UINT32			BubbleID;
	UINT32			StatusID;

	SimpleBarControl& operator=(SimpleBarControl& other)
	{
		BarCtrlInfo = other.BarCtrlInfo;
		pOpDesc     = other.pOpDesc;
		BubbleID    = other.BubbleID;
		StatusID    = other.StatusID;

		return *this;
	}

	SimpleBarControl() : BarCtrlInfo(0, 0, 0) {}
};


/********************************************************************************************

>	class BarIten : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	A virtual class used for deriving all types of bar item.

********************************************************************************************/

class BarItem : public ListItem
{
	CC_DECLARE_DYNAMIC( BarItem )

public:
	virtual BOOL Read(CCLexFile& file)  = 0;
	virtual BOOL Write(CCLexFile& file) = 0;

	virtual BOOL Read(String_256*)		= 0;
	virtual BOOL Write(String_256*)		= 0;
};

/********************************************************************************************

>	class BarControlBase: public BarItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Purpose:	Base class from which all bar control classes are derived. 
	
				This class was created so that the new (at the time) BarToolButton would
				behave correctly. It contains methods and data which used to be in the 
				BarOpControl class. 

	SeeAlso:	BarOpControl
	SeeAlso:	BarToolButton

********************************************************************************************/

class BarControlBase: public BarItem
{
	CC_DECLARE_DYNAMIC( BarControlBase )
	
public:

	// Called to obtain the bar items BubbleID
	// Note: there is no Set fn in base class cos BarControl extracts this info from the 
	// OpDescriptor.
	virtual UINT32 GetBubbleID(BOOL Horz);  

	// ( Bodge) Don't no what this is (If it's the status bar description then it will
	//  be required) ?	(Mark could you confirm this)
	virtual UINT32 GetStatusID(BOOL Horz); 

	// Set/get controls windowID 
	CWindowID GetWinID()   			  { return WinID; }
	virtual void SetWinID(CWindowID NewWinID) { WinID = NewWinID; }

private:

	CWindowID WinID;  // ID of control
}; 

/********************************************************************************************

>	class BarControl : public BarControlBase

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	This defines a control and its associated OpDescriptor within a bar.  
				It holds separate horz and vert versions of the control in case the 
				appearence and/or functionality changes with the orientation of the bar.

********************************************************************************************/

class BarControl : public BarControlBase
{
	CC_DECLARE_DYNAMIC( BarControl )

public:
	BarControl()
	{
		Visible = TRUE;
	}

	BarControl( BarControl &other)
	{
		SetEnabledState(other.IsEnabled()); 
		SetVisibleState(other.IsVisible());
		Horizontal = other.Horizontal;
		Vertical = other.Vertical;
	}

	BarControl& operator=( BarControl &other)
	{
		SetEnabledState(other.IsEnabled());
		SetVisibleState(other.IsVisible());
		Horizontal = other.Horizontal;
		Vertical = other.Vertical;
		return *this; 
	}
	void					SetHorzOpDesc(OpDescriptor* pHorzOpDesc);
	void					SetVertOpDesc(OpDescriptor* pVertOpDesc);
	const SimpleBarControl*	GetHorzControl() { return (&Horizontal); }
	const SimpleBarControl*	GetVertControl() { return (&Vertical); }
	BarControlInfo			GetBarControlInfo(BOOL Horz);
	OpDescriptor*			GetOpDescriptor(BOOL Horz);
	UINT32					GetBubbleID(BOOL Horz);  
	UINT32					GetStatusID(BOOL Horz); 

	BOOL					Read(CCLexFile& file);
	BOOL					Write(CCLexFile& file);

	// the new registry forms of the above functions
	virtual BOOL Read(String_256*);
	virtual BOOL Write(String_256*);

//	CWindowID				GetWinID()                   { return WinID; }
//	void						SetWinID(CWindowID NewWinID) { WinID = NewWinID; }
	UINT32					GetUniqueGadgetID()			 { return UniqueGadgetID; }
	void					SetUniqueGadgetID(UINT32 UniqueGadget) {UniqueGadgetID = UniqueGadget;}

	BOOL					IsEnabled()			  		 { return Enabled; }
	BOOL					IsVisible()			  		 { return Visible; }
	void					SetEnabledState(BOOL enabled) {Enabled = enabled;}
	void					SetVisibleState(BOOL visible) {Visible = visible;}

private:
	// New registry based code	
	BOOL					ReadSimpleBarControl(String_256 *pString, SimpleBarControl* pSBC);
	// older ini file based code
	BOOL					ReadSimpleBarControl(CCLexFile& file,SimpleBarControl* pSBC);
	
	void 					SetOpDesc(SimpleBarControl* pSimpleBC, OpDescriptor* pOpDesc);
	SimpleBarControl		Horizontal;
	SimpleBarControl		Vertical;

	UINT32	UniqueGadgetID; // Because bars can have controls taken from more that one resource
							// the ControlID cannot be assumed to be unique. The UniqueGadgetID
							// is filled in when a bar is created (in the oil layer) and given
							// to the control. Before an OpDescriptor control message is sent 
							// the UniqueGadgetID is mapped to the controlID and it is the 
							// ControlID which will be found in the messages GadgetID field. 

	BOOL	Enabled;		// Current gadget enabled state. This needs to be cached to reduce
							// the number of Enable messages we need to send to controls during 
							// idle time, 

	BOOL	Visible;		// To stop the resuffle HideDisableGadgetAndResuffleBar and
							// ShowEnableGadgetAndResuffleBar from performing their operation
							// on a single controls more than once (once one of these have
							// already been applied)

//	CWindowID				WinID;
};


/********************************************************************************************

>	class BarToolButton : public BarControlBase

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/94
	Purpose:	This defines a tool button used within a ToolBarOp.  It is different from a 
				BarControl in that it has no associated OpDescriptor and is not specified
				in a resource.

********************************************************************************************/

enum BitmapType { Active, Inactive }; 

class BarToolButton : public BarControlBase
{
	CC_DECLARE_DYNAMIC( BarToolButton )

public:

	// Default constructor
	BarToolButton();

// I don't no why these methods were in this class as BarToolButtons are not 
// ever read from a file I don't think (Mark could you confirm this)
	BOOL	Read(CCLexFile&)  			{ return TRUE; }
	BOOL	Write(CCLexFile&)	 		{ return TRUE; }

	// the new registry forms of the above functions
	virtual BOOL	Read(String_256 *) { return TRUE; }
	virtual BOOL	Write(String_256 *) { return TRUE; }

	UINT32	GetToolID()			 		{ return ToolID; }
	void	SetToolID(UINT32 ThisToolID) 	{ToolID = ThisToolID;}
								 
	// Returns the resource ID of the button's Bubble help string
	UINT32 	GetBubbleID(BOOL)			{ return BubbleID;}  
	void 	SetBubbleID(UINT32 Id)		{ BubbleID = Id;}

	UINT32	GetStatusID(BOOL) 			{ return StatusID; }
	void 	SetStatusID(UINT32 Id)		{ StatusID	= Id;}

	UINT32    GetBitmap(BitmapType Type);
	void 	SetBitmap(BitmapType Type, UINT32 ThisBitmap);

private:
	UINT32	ToolID;			// This is the ID that is placed into the GadgetID field of the
							// message passed to the message handler of the ToolBarOp that
							// contains this control (if you follow me).
							// The ToolID of the tool that this control represents is used.

	UINT32	Bitmap;			// This is the bitmap displayed when the tool is unselected 
	UINT32	BitmapActive;	// This is the bitmap displayed when the tool is selected

	UINT32 	BubbleID;		// Resource ID of the bubble help string for both horizontal 
							// and vertical bars.

	UINT32 	StatusID;
};


/********************************************************************************************

>	class DDeckerTop : public BarItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	Defines the start of the top row of double decker bar controls

********************************************************************************************/

class DDeckerTop : public BarItem
{
	CC_DECLARE_DYNAMIC( DDeckerTop )

public:
	BOOL Read(CCLexFile&) 			{ return TRUE; }
	BOOL Write(CCLexFile& file);

	virtual BOOL Read(String_256*)		 	{ return TRUE; }
	virtual BOOL Write(String_256*);
};


/********************************************************************************************

>	class DDeckerBottom : public BarItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	Defines the start of the bottom row of double decker bar controls

********************************************************************************************/

class DDeckerBottom : public BarItem
{
	CC_DECLARE_DYNAMIC( DDeckerBottom )

public:
	BOOL Read(CCLexFile&) 			{ return TRUE; }
	BOOL Write(CCLexFile& file);

	virtual BOOL Read(String_256*)		 	{ return TRUE; }
	virtual BOOL Write(String_256*);
};


/********************************************************************************************

>	class DDeckerEnd : public BarItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	Defines the end of a row of double decker bar controls

********************************************************************************************/

class DDeckerEnd : public BarItem
{
	CC_DECLARE_DYNAMIC( DDeckerEnd )

public:
	BOOL Read(CCLexFile&) 			{ return TRUE; }
	BOOL Write(CCLexFile& file);

	virtual BOOL Read(String_256*)		 	{ return TRUE; }
	virtual BOOL Write(String_256*);
};



/********************************************************************************************

>	class BarNewPage : public BarItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	Defines a gap between the item before this one and the one that follows. Can
				be viewed as a 'space' char when look at as a formatting item.

********************************************************************************************/

class BarNewPage : public BarItem
{
	CC_DECLARE_DYNAMIC( BarNewPage)

public:
	BOOL Read(CCLexFile&)		 	{ return TRUE; }
	BOOL Write(CCLexFile& file);

	virtual BOOL Read(String_256*)		 	{ return TRUE; }
	virtual BOOL Write(String_256*);
};

/********************************************************************************************

>	class BarSeparator : public BarItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	Defines a gap between the item before this one and the one that follows. Can
				be viewed as a 'space' char when look at as a formatting item.

********************************************************************************************/

class BarSeparator : public BarItem
{
	CC_DECLARE_DYNAMIC( BarSeparator )

public:
	BOOL Read(CCLexFile&)		 	{ return TRUE; }
	BOOL Write(CCLexFile& file);

	virtual BOOL Read(String_256*)		 	{ return TRUE; }
	virtual BOOL Write(String_256*);
};


/********************************************************************************************

>	class BarLineFeed : public BarItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	Defines a new line after the item before this one. Can
				be viewed as a 'LF/CR' char pair when look at as a formatting item.

********************************************************************************************/

class BarLineFeed : public BarItem
{
	CC_DECLARE_DYNAMIC( BarLineFeed )

public:
	BOOL Read(CCLexFile&) 			{ return TRUE; }
	BOOL Write(CCLexFile& file);

	virtual BOOL Read(String_256*)		 	{ return TRUE; }
	virtual BOOL Write(String_256*);
};



// Slight compiler bodge from Andy here:
// this macro turns a NULL class pointer into DialogBarOp. It is used as the constructors
// have to take a default argument of NULL, to stop VC++ from using the wrong function.
// If the compiler worked, the default argument with be CC_RUNTIME_CLASS(DialogBarOp)
#define	SAFECLASS(TheClass)	(TheClass?TheClass:CC_RUNTIME_CLASS(DialogBarOp))


// All the bar tokens that can be read
enum TokenIndex
{
	TOKEN_NONE = -1,
	TOKEN_BAR,
	TOKEN_BAR_END,
	TOKEN_BAR_CONTROL,
	TOKEN_BAR_CONTROL_HORZ,
	TOKEN_BAR_CONTROL_VERT,
	TOKEN_BAR_CONTROL_BOTH,
	TOKEN_BAR_SEPARATOR,
	TOKEN_BAR_LINEFEED,
	TOKEN_DOCKBAR_LEFT,
	TOKEN_DOCKBAR_RIGHT,
	TOKEN_DOCKBAR_TOP,
	TOKEN_DOCKBAR_BOTTOM,
	TOKEN_DOCKBAR_FLOAT,
	TOKEN_DOCKING_BAR,
	TOKEN_BIG,
	TOKEN_SMALL,
	TOKEN_GALLERY,
	TOKEN_GALLERY_END,
	TOKEN_DDECKER_TOP,
	TOKEN_DDECKER_BOTTOM,
	TOKEN_DDECKER_END,
	TOKEN_VISIBLE,
	TOKEN_INVISIBLE,
	TOKEN_STATUS_BAR,
	TOKEN_COLOUR_BAR,
	TOKEN_SCROLL_BARS,
	TOKEN_FULLSCREEN,
	TOKEN_ON,
	TOKEN_OFF,
	TOKEN_INFO_BAR,
	TOKEN_BAR_NEWPAGE,
	// Add new token indexs BEFORE NUM_TOKENS
	NUM_TOKENS
};

/********************************************************************************************

>	class DialogBarOp : public DialogOp

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	The kernel version of camelot bar.  Allows bar items to be added, removed, moved
				etc within the bar, and is also the place to store any information about the
				bar that will need to live longer that the lifetime of the program.


********************************************************************************************/

#define OPTOKEN_DLGBAROP _T("DialogBarOp")
enum Orientation { Horizontal, Vertical }; 

class DialogBarOp : public DialogOp
{
	CC_DECLARE_DYNCREATE( DialogBarOp )   
public:
	DialogBarOp(CCRuntimeClass* Class = NULL);
	DialogBarOp(String_32& NewName,CCRuntimeClass* Class = NULL);
	DialogBarOp(UINT32 StringID, CCRuntimeClass* Class = NULL);
	~DialogBarOp();
	void DialogBarOpInit(String_32& NewName);

	BOOL IsABar() {return TRUE;}

	virtual MsgResult Message(Msg* Msg);

	virtual void 	UpdateState(); // Called during idle processing to disable controls
	static 	void 	UpdateStateOfAllBars(); 
	static BOOL 	Init();
	static OpState 	GetState(String_256* Description, OpDescriptor*);

	// functions to write bars data out to an ini file
	static BOOL		WriteBars(CCLexFile& file);
	static BOOL		WriteNamedBars(String_256 FileName);
	static BOOL 	WriteBarsToFile(CCLexFile& file);
	static BOOL 	WriteDockingBarsToFile(CCLexFile& file);
	static BOOL		WriteWorkSpaceInfoToFile(CCLexFile& file);
	// functions to read bars data in from an ini file
	static BOOL 	ReadBarsFromFile(	CCLexFile& file,
										BarCreate& BarCreate,
										UINT32* pNumBarsRead = NULL,
										UINT32* pNumDockingBarsRead = NULL);
protected:
	// new registry related items
	static BOOL 	WriteBarsToRegistry(HKEY hSubKey);
	static BOOL		WriteWorkSpaceInfoToRegistry(HKEY hSubKey);
	static BOOL		WriteDockingBarsToRegistry(HKEY hSubKey);
	
	static BOOL		ReadBarsFromRegistry(HKEY hSubKey);
	static BOOL		ReadWorkSpaceInfoFromRegistry(HKEY hSubKey);
	static BOOL		ReadDockingBarsFromRegistry(HKEY hSubKey);

	// clean out any registry data for the name bar
	static BOOL		WipeRegistrySettingsForBar(TCHAR * pBarName);
	// find out if the bars stored in the registry are up to date or not
	static BOOL		ReadRegistryBarsVersion();

public:
	static BOOL MakeControlBank();

	static DialogBarOp*		FindDialogBarOp(String_32& DialogBarOpName, INT32 limit = -1);
	static DialogBarOp*		FindDialogBarOp(UINT_PTR WinID);

	static DialogBarOp*		AllBarsOp;
	BOOL				IsAllBarsOp();
	static BOOL 			AddOpToAll(OpDescriptor* pHorzOpDesc,OpDescriptor* pVertOpDesc);


	static UINT32 			FindUniqueBarNumber();

	// List Access functions
	LISTPOS			AddBarItem(BarItem* pBarItem);
	BarItem*		RemoveBarItem(LISTPOS here);
	BarItem*		RemoveBarItem(BarItem * thisItem);
	void			MoveBarItem(LISTPOS here,LISTPOS newpos);
	BOOL 			DeleteBarItem(CWindowID WinID);
	BarItem*		GetBarItem(CWindowID WinID);
	void 			InsertItemAfter(LISTPOS here,BarItem * thisItem);
	
	// dynamic infobar stuff ....
	BOOL HideDisableGadgetAndResuffleBar (CWindowID WinID);
	BOOL ShowEnableGadgetAndResuffleBar (CWindowID WinID);
	BOOL ResetHiddenGadgetStates ();

	void			DeleteAllBarItems();
	BOOL			IsListEmpty();
	BarItem*		GetPtrBarItem(LISTPOS here);
	BarItem* 		GetPtrBarNext(BarItem * ThisItem);
	BarItem* 		GetPtrBarPrev(BarItem * ThisItem);
	BarItem* 		InsertItemBefore(BarItem * here ,BarItem * newItem);
	BarItem* 		GetPtrBarHead();
	DWORD 			GetNumBarItems() const;

	BOOL			Read(CCLexFile& file);
	virtual BOOL	Write(CCLexFile& file);

	// the new registry forms of the above functions
	virtual BOOL	Read(LoadRegistryEntries& Loader);
	virtual BOOL	Write(SaveRegistryEntries& Saver);

	BOOL			IsVisible();
	virtual void	SetVisibility(BOOL Open);

	DockBarType		GetDockBarType() 						{ return Dock; }
	void			SetDockBarType(DockBarType DockValue) 	{ Dock = DockValue; }
	UINT32			GetSlot() 								{ return Slot; }
	void			SetSlot(UINT32 SlotNum) 					{ Slot = SlotNum; }
	UINT32			GetOffset() 							{ return Offset; }
	void			SetOffset(INT32 OffsetValue) 				{ Offset = OffsetValue; }
	wxPoint			GetFloatingCPoint() 					{ return FloatPos; }
	void			SetFloatingCPoint(wxPoint& Pos) 			{ FloatPos = Pos; }
	wxRect			GetBarRect() 							{ return BarRect; }
	void			SetBarRect(wxRect& Rect)					{ BarRect = Rect; }
	String_32&		GetName()								{ return Name; }
	void			SetName(String_32& str);

 	void 			SetCurrentOrientation(Orientation	BarDirection); 
	BOOL			IsHorizontal() { return (BarOrientation == Horizontal); }

	// This function loads the bars.ini file and then creates the initial bars
	static BOOL LoadBars();
	static BOOL LoadDefaultBars();
	static BOOL WipeBarFiles();
	static BOOL LoadNamedBars(String_256 FileName); 

	void 			Delete();

	// This function should be called whenever the state of the system has changed
	// it indicates that the Bar states should be updated. The updating will 
	// occur during idle events.


	static void SetSystemStateChanged(BOOL State = TRUE); 
	static BOOL ShouldUpdateBarState();
	UINT32 GetUniqueID();

	// This func asks the derived class whether it wants its list box to allow multiple
	// selection or not.
	// This is a very specialised func associated with BaseBar::Create() and galleries.
	virtual	BOOL AllowMulSelInListBox();

protected:
	// Called from LoadBars()
	static BOOL 	LoadNamedBarFile(String_256 FileName,BOOL * Opened);
	static BOOL		LoadBarsFromRes();
	static BOOL		ReadResVersion();
	static BOOL		CreateBars();
	static BOOL		SetValidPath(String_256 Filename);
	static PathName		ValidPath;
		
	// Called when wanting to ignore a bar definition
	static BOOL FlushUpToToken(CCLexFile& file,TokenIndex Token);

	DockBarType		Dock;
	UINT32			Slot;
	INT32				Offset;
	wxPoint			FloatPos;	
	wxRect			BarRect;

	BOOL			InitiallyVisible;	// TRUE if bar should be made visible when program starts up
	List 			BarItemList;
private:
	
	String_32		Name;

	Orientation		BarOrientation;

	static BOOL		SystemStateChanged; // Set to TRUE initially so that the bar
											   // state will be refreshed.

#ifdef _DEBUG
public :
	static void TestStart();
	static void TestEnd();
#endif
};

// Function headers for inlines

/********************************************************************************************

>	inline static void DialogBarOp::SetSystemStateChanged(BOOL State = TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/94

	Inputs:		State: TRUE if the system state has changed
					   FALSE to indicate that the state of the bars has been updated to
					   reflect the new system state 
	Outputs:	-
	Returns:	-
	Purpose:	This function is called whenever the state of the system has changed in a
				way that indicates the bar state needs refreshing. Currently this function
				is called in the Operation End method and when the selection changes.

				In future this function may need to be called from other places as well.

				The state of the bars is updated during Idle events. 
				
	Errors:		-
	SeeAlso:	DialogBarOp::ShouldUpdateBarState

********************************************************************************************/


/********************************************************************************************

>	inline static BOOL DialogBarOp::ShouldUpdateBarState() { return SystemStateChanged == TRUE; }

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if we should update the bars states.  
	Purpose:	To determine if we need to update the state of the visible bars.
	Errors:		-
	SeeAlso:	DialogBarOp::SetSystemStateChanged

********************************************************************************************/


/********************************************************************************************

>	class SystemBarOp : public DialogBarOp

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/94
	Purpose:	A special version of DialogBarOp that is used internally to store all the
				BarItems in a form where they can be used by the Customize dialog.
				They are never seen by the user, never loaded and never saved.

********************************************************************************************/

#define OPTOKEN_SYSTEMBAROP _T("SystemBarOp")

class SystemBarOp : public DialogBarOp
{
	CC_DECLARE_DYNCREATE( SystemBarOp )
public:
	SystemBarOp();					// Default constructor (needed by DYNCREATE)
	SystemBarOp(UINT32 BarNameID, 
				SystemBarType GrpBarID,
				DockBarType DockType = DOCKBAR_BOTTOM,
				UINT32 Slot = 0,
				INT32 Offset = -1);

	virtual MsgResult Message(Msg* Msg);

	// Functions to add BarItems to system bars...
	static SystemBarOp*	FindType(SystemBarType);			// Find a hidden bar by type
	static BOOL		Connect(SystemBarType, 	OpDescriptor* pHorzOpDesc, 
											OpDescriptor* pVertOpDesc = NULL);	// Connects an OpDesc to a bar
	static BOOL		Connect(SystemBarOp*,  	OpDescriptor* pHorzOpDesc, 
											OpDescriptor* pVertOpDesc = NULL);	// Connects an OpDesc to a bar
	static BOOL		Separate(SystemBarType);				// Connects a Separator to a bar
	static BOOL		Separate(SystemBarOp*);					// Connects a Separator to a bar

	// the old ini file based reading/writing functions
	BOOL			Read(CCLexFile& file);	// Override DialogBarOp write function
	BOOL 			Write(CCLexFile& file);	// Override DialogBarOp read function

	// the new registry forms of the above functions
	virtual BOOL	Read(LoadRegistryEntries& Loader); // Override DialogBarOp read function
	virtual BOOL	Write(SaveRegistryEntries& Saver); // Override DialogBarOp write function

	static BOOL 	Init();


// Extra member vars on top of those in DialogBarOp
	SystemBarType	GroupBarID;				// Holds hidden group identity

//	virtual void 	UpdateState() {}; // does nothing 
};



/********************************************************************************************

>	class InformationBarOp : public DialogBarOp

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> & Mark Neves
	Created:	17/4/94
	Purpose:	The kernel version of info bar.	 

********************************************************************************************/

class InformationBarOp: public DialogBarOp
{
	CC_DECLARE_DYNCREATE( InformationBarOp )   
public:
	InformationBarOp(CCRuntimeClass* Class = NULL);
	InformationBarOp(String_32& NewName,CCRuntimeClass* Class = NULL);

	BOOL Create();
	void MakeCurrent(){	InformationBarOp::CurrentInfoBarOp = this;};
	void Delete();
	void SetInfo();
	void GetInfo();
	static BOOL	IsVisible();
	static BOOL IsHidden(){ return Hidden;};
	static void	SetVisibility(BOOL Open,BOOL Force = FALSE);

	// read/write data to/from ini file
	static BOOL Read(CCLexFile& file);
	static BOOL WriteStatic(CCLexFile& file);

	// read/write data to/from registry
	static BOOL Read(HKEY hSubKey);
	static BOOL Write(HKEY hSubKey);

	virtual void CloseProfileDialog (CBiasGainGadget& Gadget);
	virtual void DisallowInteractiveProfiles () { InteractiveProfiles = FALSE; }
	//static void AllowInteractiveProfiles () {}

protected:

	// InformationBarOp::Message should get a look in after you derived class has received
	// a message in case it has to do general infobar message handling.
	// Call this at the end of your own message handler.
	virtual MsgResult Message(Msg* pMsg);

	// Justin says: I am removing this dummy function because it hides the base class version
	// which correctly updates the grey-state of controls with multiple instances.
//	virtual void 	UpdateState() {}; // does nothing 

	virtual void HandleProfileButtonClick (CBiasGainGadget& Gadget, CGadgetID ProfileGadgetID);
	virtual void HandleProfileSelChangingMsg (CBiasGainGadget& Gadget, CGadgetID ProfileGadgetID);
	virtual CProfileBiasGain* GetProfileFromSelection (CGadgetID ProfileGadgetID, BOOL* bMany, BOOL* bAllSameType);
	virtual void ProfileSelectionChange (DialogMsg* Message, CGadgetID GadgetID);
	virtual void ChangeProfile (CProfileBiasGain* Profile, CGadgetID GadgetID);
	virtual void ChangeProfileOnIdle (CProfileBiasGain* Profile, CGadgetID GadgetID);

private:
	static DockBarType			Dock;
	static UINT32					Slot;
	static UINT32					Offset;
	static wxPoint				FloatPos;
	static InformationBarOp*	CurrentInfoBarOp;
	static BOOL					Visible;
	static BOOL					Hidden;
	BOOL						InteractiveProfiles;
};


/********************************************************************************************

>	class BarCreate : public SimpleCCObject

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Abstract class for creating DialogBarOps and derived classes.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile

********************************************************************************************/

class BarCreate : public SimpleCCObject
{
public:
	virtual ~BarCreate() { }
	virtual	DialogBarOp*	Create() = 0;
};


/********************************************************************************************

>	class DialogBarOpCreate : public BarCreate

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Class for creating DialogBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile

********************************************************************************************/

class DialogBarOpCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new DialogBarOp); }
};



/********************************************************************************************

>	class DockingBar : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:	The kernel's version of a docking bar. All information that needs to be saved
				in the BARS.INI file should be represented in this class.

********************************************************************************************/

/*
class DockingBar : public ListItem
{
public:

	DockingBar();
	~DockingBar();

	BOOL			HasBigControls();
	void			SetBigControlsState(BOOL Big);
	BOOL 			Write(CCLexFile& file);
	DockBarType		GetDockBarType()  { return Dock; }

	static List*	GetPtrDockingBarList() { return &DockingBarList; }
	static BOOL		WriteDockingBars(CCLexFile& file);

private:
	struct CCAPI Dummy
	{
		BOOL	Big	: 1;	// TRUE = Big control icons used in all bars docked with this bar
	} DockingBarFlags;

	DockBarType	Dock;

	static List	DockingBarList;
};
*/

#endif // INC_BARS
