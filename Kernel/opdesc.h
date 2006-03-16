// $Id: opdesc.h 662 2006-03-14 21:31:49Z alex $
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
// OpDescriptor allows camelot to maintain a list of all the operations 
// which can be performed


#ifndef INC_OPDESC
#define INC_OPDESC

#include "listitem.h"
#include "list.h"
#include "strings.h"
#include "pump.h"
#include "msg.h"

// AutoStateFlags (parameter to OpDescriptor's constructor)
// These flags are used to specify when the Operation should automatically be greyed, ticked etc.
// they cut down the number of tests that need to be made in the GetState fn
// Format <What happens>_WHEN_<Condition>
// <What happens> -> GREY | TICKED

#define GREY_WHEN_NO_CURRENT_DOC 1	  // Document::GetCurrent == NULL
#define GREY_WHEN_NO_SELECTION   2	  // There are no selected objects
#define DONT_GREY_WHEN_SELECT_INSIDE 4
// Add others here

class OpParam;

/******************************************************************************************

>	void BTNOP(NAME, OPCLASS, SYSTEM)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/94
	Inputs:		NAME:		Operation name
				OPCLASS:	Name of Operation class in which GetState will be called and
							which will be instatiated by the OpDescriptor's Invoke()
							function.
				SYSTEM:		Name of system bar which op should be attached to.
							Currently this can be one of:
								FILE
								EDIT
								ATTR
								ARRANGE
								UTIL
								WINDOW
								HELP
	Purpose:	MACRO
				Construct a complete OpDescriptor given three simple parameters. The
				parameters are used to build all the various resource IDs required by the
				call to RegisterOpDescriptor. Each use of this macro replaces the following
				code;
				BOOL Blobby = RegisterOpDescriptor(
												  	0, 
												  	_R(IDS_FILEOPEN),
												  	CC_RUNTIME_CLASS(DocOps), 
												  	OPTOKEN_FILEOPEN,
												  	DocOps::GetState,
												  	HID_FILEOPEN,
												  	_R(IDBBL_FILEOPEN),
												  	_R(IDD_BARCONTROLSTORE),
													_R(IDC_BTN_FILEOPEN),
													SYSTEMBAR_FILE,
													TRUE // ReceiveMessages
													);
			  	ERRORIF(!Blobby, _R(IDS_OUT_OF_MEMORY), FALSE);

				As you can see this macro causes the function that called it to exit and
				return FALSE if the registration fails. This is normally correct for calls
				made from Operation::Init functions, like this:
				BOOL BlobbyOp::Init()
				{
					BTNOP(...);
					BTNOP(...);
					return(TRUE);
				}
				Note: 	This macro uses new format IDs. Don't use the REGOP macro in
						menuops.cpp because it uses old format IDs.
				
	SeeAlso:	CTRLOP

******************************************************************************************/

#define BTNOP(NAME, OPCLASS, SYSTEM)\
	{\
		BOOL Blobby = RegisterOpDescriptor(\
						0,\
						_R(IDS_ ## NAME),/*NORESOURCEFIX*/\
						CC_RUNTIME_CLASS(OPCLASS),\
						OPTOKEN_ ## NAME,\
						OPCLASS::GetState,\
						HID_ ## NAME,\
						_R(IDBBL_ ## NAME),/*NORESOURCEFIX*/\
						_R(IDD_BARCONTROLSTORE),\
						_R(IDC_BTN_ ## NAME),/*NORESOURCEFIX*/\
						SYSTEMBAR_ ## SYSTEM,\
						TRUE\
					  );\
		ERRORIF(!Blobby, _R(IDS_OUT_OF_MEMORY), FALSE);\
	}




/******************************************************************************************

>	void CTRLOP(NAME, OPCLASS, SYSTEM)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/94
	Inputs:		NAME:		Operation name
				OPCLASS:	Name of Operation class in which GetState will be called and
							which will be instatiated by the OpDescriptor's Invoke()
							function.
				SYSTEM:		Name of system bar which op should be attached to.
							Currently this can be one of:
								FILE
								EDIT
								ATTR
								ARRANGE
								UTIL
								WINDOW
								HELP
	Purpose:	MACRO
				Construct a complete OpDescriptor given three simple parameters. The
				parameters are used to build all the various resource IDs required by the
				call to RegisterOpDescriptor. Each use of this macro replaces the following
				code;
				BOOL Blobby = RegisterOpDescriptor(
												  	0, 
												  	_R(IDS_FILEOPEN),
												  	CC_RUNTIME_CLASS(DocOps), 
												  	OPTOKEN_FILEOPEN,
												  	DocOps::GetState,
												  	HID_FILEOPEN,
												  	_R(IDBBL_FILEOPEN),
												  	_R(IDD_BARCONTROLSTORE),
													_R(IDC_BTN_FILEOPEN),
													SYSTEMBAR_FILE,
													TRUE // ReceiveMessages
													);
			  	ERRORIF(!Blobby, _R(IDS_OUT_OF_MEMORY), FALSE);

				As you can see this macro causes the function that called it to exit and
				return FALSE if the registration fails. This is normally correct for calls
				made from Operation::Init functions.
				Note: 	This macro uses new format IDs. Don't use the REGOP macro in
						menuops.cpp because it uses old format IDs.
				
	SeeAlso:	BTNOP

******************************************************************************************/

#define CTRLOP(NAME, OPCLASS, SYSTEM)\
	{\
		BOOL Blobby = RegisterOpDescriptor(\
						0,\
						_R(IDS_ ## NAME),/*NORESOURCEFIX*/\
						CC_RUNTIME_CLASS(OPCLASS),\
						OPTOKEN_ ## NAME,\
						OPCLASS::GetState,\
						HID_ ## NAME,\
						_R(IDBBL_ ## NAME),/*NORESOURCEFIX*/\
						_R(IDD_BARCONTROLSTORE),\
						_R(IDC_CTRL_ ## NAME)/*NORESOURCEFIX*/,\
						SYSTEMBAR_ ## SYSTEM,\
						TRUE\
					  );\
		ERRORIF(!Blobby, _R(IDS_OUT_OF_MEMORY), FALSE);\
	}




// Forward declarations...
class DialogOp; 




/********************************************************************************************

>	class OpState

Purpose:	The Opstate class contains flags describing the state of an operation at any 
			particular time. This state will be used by the Menu system to tick and grey
			menus (and do corresponding things to buttons where necessary).
SeeAlso:	OpDescriptor; Operation

********************************************************************************************/

class CCAPI OpState
{
public:
	// Member vars
	BOOL Ticked : 1;
	BOOL Greyed : 1;
	BOOL RemoveFromMenu : 1;
	
	// Constructor to allow default values to be supplied
	OpState( BOOL tick = FALSE, BOOL grey = FALSE, BOOL Remove = FALSE );
	
};
 

// Operation Ability Flags
/*
Goodbye
#define OP_MACRO			0x0001
#define OP_SIMPLE			0x0002
#define OP_NOTFORMACRO	 	0x0004
#define OP_NEEDSDOC			0x0008
#define OP_SPECIALITEM		0x0010  
*/


// Operation Text Flags

enum OpTextFlags { OP_MENU_TEXT, OP_DESC_TEXT, OP_UNDO_TEXT, OP_MENU_NAME };

// String Resource Text Delimeter

#define STRING_DELIMETER	';'

// End Of String Delimeter

#define END_OF_STRING		'\0'

// Maximum Text String Size

#define MAX_TEXT_SIZE		256


// This creates a new name, pfnGetState, for a pointer to a function that returns
// your OpState information.

class OpDescriptor;
typedef OpState (*pfnGetState)(String_256*, OpDescriptor*);
typedef OpState (*pfnGetParamState)(String_256*, OpDescriptor*, OpParam*);




/********************************************************************************************

>	class BarControlInfo

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> (modified by Phil, 13/04/94)
	Created:	5/4/94
	Purpose:	A class that encapsulates the minimum data required to specify a control on
				a bar.  The info is platform independant.

********************************************************************************************/

class BarControlInfo
{
public:
	UINT32	ResourceID;		// Resource in which control is found
	UINT32	ControlID;		// ID of control within that resource
	UINT32	ToolID;			// Tool (and thus module) in which resource is found

	BarControlInfo& operator=(BarControlInfo& other) 
	{ 
		ResourceID = other.ResourceID;
		ControlID  = other.ControlID;
		ToolID     = other.ToolID;

		return *this;
	}

/********************************************************************************************

> 	BarControlInfo::BarControlInfo(UINT32 resID, UINT32 ctrlID=0, UINT32 toolID=0)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Purpose:	BarControlInfo constructor which fills in the member data fields.
				The caller MUST specify the resource ID but the control ID and tool ID may 
				be ommitted. Their default values are 0 which for the tool ID means that 
				the resource will be found in the kernel resources.

********************************************************************************************/
	BarControlInfo(UINT32 resID, UINT32 ctrlID=0, UINT32 toolID=0) :
					ResourceID(resID), ControlID(ctrlID), ToolID(toolID)
					{}
};




/********************************************************************************************

>	class OpListItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/94
	Purpose:	The reason we need to store OpDescriptors in OpListItems is that 
				OpDescriptors are MessageHandler objects and so are already potentially
				stored on a list. 
	SeeAlso:	-

********************************************************************************************/

class OpListItem: public ListItem
{
	CC_DECLARE_MEMDUMP(OpListItem)

public:	
	OpDescriptor* pOpDesc; 
};

// The OpFlgs structure holds info about the type of an operation
struct CCAPI OpFlgs
{						
	BOOL Clean : 1;			 // Op does not "modify" document
	BOOL Smart : 1;			 // Op can be smart duplicated

	BOOL fCheckable : 1;	 // Op should have checkbox when in menu
};



/********************************************************************************************

>	class ListItemOpPtr: public ListItem

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/10/94
	Purpose:	To enable storage of pointers to operations on lists. 
	SeeAlso:	-

********************************************************************************************/

class ListItemOpPtr: public ListItem
{
	CC_DECLARE_MEMDUMP(ListItemOpPtr)

public:	
	Operation* pOp; 
};



/********************************************************************************************

>	class OpDescriptor : public MessageHandler

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Purpose:	The OpDescriptor class describes a particular operation. It contains
				pointers to the literal token describing the operation (used by the 
				macro language, among other things) and pointers to some static member
				functions of the operation which allow dynamic creation of the operation.
	SeeAlso:	operation

********************************************************************************************/

class OpDescControlCreateMsg;
class OpDescControlMsg;

class CCAPI OpDescriptor : public MessageHandler
{
	CC_DECLARE_DYNAMIC( OpDescriptor )
	
protected:
	
	DWORD			ModuleID;	
    UINT32 			TextID;
	UINT32			HelpID;
	UINT32			BubbleID;
	BarControlInfo	BarCtrlInfo;
//	UINT32			BitmapID;
	CCRuntimeClass* OpClass;
	OpFlgs			Flags; 	

	BOOL			m_bHotKeyEnabled;
	// This set of member vars looks after the aliased state of the op descriptor
    BOOL			Aliased;			// TRUE if the op desc has been aliased
	CCRuntimeClass* AliasOpClass;		// The op class to use to invoke the alias op
	pfnGetState		OldGetState;		// The GetState func before aliasing
	UINT32			OldAutoStateFlags;	// The AutoStateFlags before aliasing
	UINT32			OldTextID;			// The ID of the text string
	UINT32			OldBubbleID;		// The ID of the bubble help string
	
	// When not 0 only one live instance
	// of the operation is permitted, and
	// OneInstID is the string resource
	// ID describing why.This will be useful
	// for 1 open instance dialogs.	
	UINT32 OneInstID;
	UINT32 AutoStateFlgs;   // Flags which indicate conditions when
				 		   // the operation should automatically be
						   // greyed/ticked etc..(This cuts down the 
						   // number of tests that need to be made in the 
						   // GetState function).

private: 
	pfnGetState GetState;				// pointer to the GetState function in the operation

protected:
	pfnGetParamState GetParamState;		// pointer to the GetParamState function in the operation
	
public:
	// Public Member variable so people can actually get at the name of the Op.
	String Token;			// literal token - does not need to be internationalised

	// Calls GetState fn 
	virtual OpState GetOpsState(String_256*, OpParam* pOpParam = NULL);

public:
	
	// Invokes the operation associated with the OpDescriptor calling either the Operations
	// Do or DoWithParam virtual fns.
	void Invoke(OpParam* pOpParam = NULL, BOOL fWithUndo = TRUE);
	
	// Constructor for creating an instance, which also links
	// the object into the linked list.
	// This constructor takes in a resource ID and control ID.
	// If they are both zero (the default case) then this OpDescriptor cannot be accessed
	// by the user through any control in the toolbars.
	// If the resource ID is supplied alone is is treated as a bitmap ID which connects
	// the OpDescriptor to a bitmap button.
	// If both the resource ID and the Control ID are supplied then the OpDescriptor is
	// connected to an aribtrary control which may be on a toolbar somewhere.
	OpDescriptor(
				 UINT32 toolID,                      	// Module Identifier
				 UINT32 txID,                         // String Resource ID
				 CCRuntimeClass* Op,				// pointer to the Op's runtime class object
				 TCHAR* tok,						// pointer to the token string
				 pfnGetState gs,					// pointer to the GetState function
				 UINT32 helpId = 0,					// help identifier 
				 UINT32 bubbleID = 0,					// string resource for bubble help
				 UINT32 resourceID = 0,				// bitmap ("icon") or resource ID
				 UINT32 controlID =0,					// control ID
				 BOOL ReceiveMessages = FALSE,
				 BOOL Smart = FALSE,
				 BOOL Clean = TRUE,
				 UINT32 OneOpenInstID = 0,			// When not 0 only one live instance
				 									// of the operation is permitted, and
													// OneOpenInstID is the string resource
													// ID describing why.This will be useful
				 									// for 1 open instance dialogs.
				 UINT32 AutoStateFlags = 0,			// Flags which indicate conditions when
				 									// the operation should automatically be
													// greyed/ticked etc..(This cuts down the 
													// number of tests that need to be made in the 
													// GetState function).
				 BOOL fCheckable = FALSE
				);

	// standard destructor
	~OpDescriptor();

	OpFlgs GetOpFlags();


	void SetBarControlInfo(BarControlInfo ThisInfo){ BarCtrlInfo = ThisInfo;};
						
	// a search function that returns a pointer to the OpDescriptor that contains token
	static OpDescriptor* FindOpDescriptor(TCHAR* Token);
	
	// a search function that returns a pointer to the OpDescriptor that describes operation
	static OpDescriptor* FindOpDescriptor(CCRuntimeClass* Op);

	// a search function that returns a pointer to the OpDescriptor searching by ResourceID
	static OpDescriptor* FindOpDescriptor(ResourceID res);
	
	// Need a method of killing off all the Descriptors at program end
	static void DestroyAll();

	// Allows you to alias the operation invoked by this OpDesc.
	// When the Invoke() method is called an instance of AliasOp is made instead of the registered Op.
	// Also, you have to provide an alias GetState() function so that menu items, buttons, etc can
	// be greyed, etc, correctly.
	// This function also allows you to change the auto state calculation flags
	void AliasOperation(CCRuntimeClass* AliasOp,pfnGetState AliasGetState,UINT32 AliasAutoStateFlags=0,UINT32 AliasTextID=0,UINT32 AliasBubbleID=0);

	// Reverts the op desc to its default operation, i.e. unaliases it
	void RemoveAlias();

	// Static fn that clears all aliases of all OpDescs in the system
	static void RemoveAllAliases();

	// Returns TRUE if this OpDesc is aliased.
	BOOL IsAliased();

	// For those ops associated with a hot key, the following functions determine the key's viability
	BOOL	IsHotKeyEnabled() const;
	void	DisableHotKey();
	void	EnableHotKey();

	// This will use the TextID to obtain a string resource text description of the 
	// operation. 
	virtual BOOL GetText(String_256* Description, OpTextFlags WhichText);

	// The Default Message function 
	virtual MsgResult Message(Msg* Msg);

	// This iterates through all the controls in the application, adding to the given
	// list those which are associated with this OpDescriptor.  Returns FALSE if there
	// are no such controls.
	BOOL BuildGadgetList(List* pOutputList);

	// --------------------------------------------------------------------------------------
	// OpDescriptor On message handlers
	// These get called by the default OpDescriptor Message handler

	// This function gets called by the OpDescriptors default Message handler whenever the 
	// control associated with an OpDescriptor is created. This base class function does 
	// nothing. 
	virtual void OnControlCreate(OpDescControlCreateMsg* CreateMsg);

	// This function gets called whenever the user selects an item from a list or combo 
	// control associated with an OpDescriptor
	virtual void OnSelectionChange(OpDescControlMsg* SelChangedMsg, List* GadgetList);

	// This function is called whenever a slider control associated with the opdescriptor
	// changes to a new position.
	virtual void OnSliderChanging(OpDescControlMsg* SliderChangingMsg);
	
	// This function is called whenever a slider control associated with the opdescriptor
	// changes to a new position.
	virtual void OnSliderSet(OpDescControlMsg* SelChangedMsg);

	// This function is called whenever a slide associated with the opdescriptor
	// is cancelled.
	virtual void OnSliderCancelled(OpDescControlMsg* SelChangedMsg);

	TCHAR* ReadString(TCHAR* pDesc);
	TCHAR* GetMenuNameString(TCHAR* pDesc);
	void SetStringPos(TCHAR** pDesc, OpTextFlags WhichText);

	UINT32 GetHelpId();
	UINT32 GetToolID();
	UINT32 GetBubbleId();
	const BarControlInfo* GetBarControlInfo();

private:

	static List OpList; 

protected:
	TCHAR* GetDescription(TCHAR* pDesc, OpTextFlags WhichText);

public:

/********************************************************************************************

>	static OpListItem* OpDescriptor::GetFirstDescriptor()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Purpose:	returns the first OpListItem in the list

********************************************************************************************/
	static OpListItem* GetFirstDescriptor()	
	{ 
		return ((OpListItem*)(OpList.GetHead()));  
	}


	
/********************************************************************************************

>	static OpListItem* OpDescriptor::GetNextDescriptor( OpListItem* CurrOp )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		pointer to last OpListItem returned by GetFirst ot GetNextDescriptor
	Purpose:	returns the next OpListItem in the list

********************************************************************************************/

	static OpListItem* GetNextDescriptor( OpListItem* CurrOp )
	{  
		return ((OpListItem*)OpList.GetNext(CurrOp));  
	}

	static void LinkDescriptor( OpDescriptor* ThisOp );
	static BOOL DelinkDescriptor( OpDescriptor* pThisOp );
};



/********************************************************************************************

>	class UndoableOpDescriptor : public OpDescriptor

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	All undoable operations should be registered using an UndoableOpDescriptor. 
				The only difference between an UndoableOpDescriptor and an OpDescriptor
				is that the Operation Flags are set to different default values. 
				
	SeeAlso:	operation

********************************************************************************************/

class CCAPI UndoableOpDescriptor : public OpDescriptor
{
	CC_DECLARE_DYNAMIC( UndoableOpDescriptor )
	
public:

	// Constructor for creating an instance. The params are all the same as for OpDescriptor

	UndoableOpDescriptor(
						 UINT32 toolID,                      	// Tool (Module) Identifier
						 UINT32 txID,                         // String Resource ID
						 CCRuntimeClass* Op,				// pointer to the Op's runtime class object
						 TCHAR* tok,						// pointer to the token string
						 pfnGetState gs,					// pointer to the GetState function
						 UINT32 helpId = 0,					// help identifier 
						 UINT32 bubbleID = 0,					// string resource for bubble help
						 UINT32 resourceID = 0,				// resource ID
						 UINT32 controlID = 0,				// control ID within resource
						 BOOL ReceiveMessages = FALSE, 		
						 BOOL Smart = FALSE, 
						 BOOL Clean = FALSE,
						 UINT32 OneOpenInstID = 0,	// When not 0 only one live instance
				 									// of the operation is permitted, and
													// OneOpenInstID is the string resource
													// ID describing why.This will be useful
				 									// for 1 open instance dialogs.
				 		 UINT32 AutoStateFlags = 0,	// Flags which indicate conditions when
				 									// the operation should automatically be
													// greyed/ticked etc..(This cuts down the 
													// number of tests that need to be made in the 
													// GetState function).
						 BOOL fCheckable = FALSE
				);


	UINT32 ToolId;

};



/********************************************************************************************

>	class ToolOpDescriptor : public OpDescriptor

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	The ToolOpDescriptor class describes a Tool Selection operation. It contains
				pointers to the literal token describing the operation (used by the 
				macro language, among other things) and pointers to some static member
				functions of the operation which allow dynamic creation of the operation.
	SeeAlso:	operation

********************************************************************************************/

class CCAPI ToolOpDescriptor : public OpDescriptor
{
	CC_DECLARE_DYNAMIC( ToolOpDescriptor )
	
public:

	ToolOpDescriptor(	
				 		UINT32 toolID,                    // Tool (Module) Identifier
				 		UINT32 txID,                      // String Resource ID
//				 		UINT32 toolId,					// Tool Identifier
				 		CCRuntimeClass* Op,				// pointer to the runtime class of
														// appropriate Operation object
				 		TCHAR* tok = NULL,				// pointer to the token string 
				 		pfnGetState gs = NULL,			// pointer to the GetState function
						UINT32 hlpID = 0,					// help file "jump" ID
						UINT32 bubID = 0,					// bubble-help string resource ID
						UINT32 resID = 0, 				// resource ID of this OpDesc.
						UINT32 ctlID = 0,					// control of this OpDesc
		 				BOOL ReceiveMessages = TRUE, 		
		 				BOOL Smart = FALSE, 
						BOOL Clean = FALSE,
						BOOL fCheckable = FALSE ); 

private:
	// Set to TRUE if this is the selected tool
	BOOL CurrentTool; 

public:
	void SetCurrentTool(BOOL State) { CurrentTool = State; }
	BOOL IsCurrentTool() { return CurrentTool;}



//	UINT32 ToolId;

};




/********************************************************************************************

>	class ParamOpDescriptor : public OpDescriptor

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from code by Neville Humphrys)
	Created:	11/10/2004
	Purpose:	New ParamOpDescriptor class whose purpose in life is to allow an OpDescriptor
				to be created which instead of using a resource id to get the menu item/operation
				item text, uses a string resource. This then means we can use the name from the
				plug-in instead of having to resource all names!

********************************************************************************************/

class ParamOpDescriptor : public OpDescriptor
{
	CC_DECLARE_DYNAMIC( ParamOpDescriptor );

// Statics
public:
	static OpState	GetState(String_256* psName, OpDescriptor* pOpDesc);

// Methods
public:
	ParamOpDescriptor(const TCHAR* pcszToken,
								CCRuntimeClass* pClass,
								pfnGetParamState gps);

	virtual BOOL	GetText(String_256* Description, OpTextFlags WhichText);

// Properties
public:
//	String_64		MenuText;
};




// ------------------------------------------------------------------------------------------
// OpDescriptor messages	


/********************************************************************************************

>	class OpDescMsg: public Msg

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Base class for all OpDescriptor messages  
	Errors:		-
	SeeAlso:	OpDescControlDestroy
	SeeAlso:	OpDescControlCreate

********************************************************************************************/

class OpDescMsg: public Msg
{
	CC_DECLARE_DYNAMIC(OpDescMsg);
	
public:
	OpDescriptor* OpDesc; // The OpDescriptor the message is meant for
	
	CGadgetID GadgetID;	   // This is the resource gadget ID

	CGadgetID SetGadgetID; // You should use this GadgetID whenever you are sending a message 
						   // to the gadget, eg. if you are using any of the DialogOp set
						   // functions. It is a unique gadget ID for the bar.  

	DialogOp* pDlgOp;  	  // A pointer to the DialogOp 

	
	OpDescMsg(OpDescriptor* OpD, CGadgetID GadID, CGadgetID SetGadget, DialogOp* pDialogOp)
	{
		OpDesc = OpD; 
		GadgetID = GadID; 
		pDlgOp = pDialogOp;
		SetGadgetID = SetGadget;
	};

};

/********************************************************************************************

>	class OpDescControlCreate: public OpDescMsg

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This message is sent to an OpDescriptor after its controls have 
				been created. It gives you a chance to perform control initialisation. 
				eg. Set scrollbar ranges, fill listboxes ...
	Errors:		-
	SeeAlso:	OpDescControlDestroy

********************************************************************************************/

class OpDescControlCreateMsg: public OpDescMsg
{	
	CC_DECLARE_DYNAMIC(OpDescControlCreateMsg);
	OpDescControlCreateMsg(OpDescriptor* OpD, 
			  			   CGadgetID GadID, CGadgetID SetGadget, DialogOp* pDialogOp):
		OpDescMsg(OpD,  GadID, SetGadget, pDialogOp)
	{
	}; 
}; 

/********************************************************************************************

>	class OpDescControlDestroy: public OpDescMsg

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This message is sent to an OpDescriptor after its controls have 
				been destroyed, It gives the OpDescriptor a chance to cleanup 

	Errors:		-
	SeeAlso:	OpDescControlCreateMsg


********************************************************************************************/

class OpDescControlDestroyMsg: public OpDescMsg
{
	CC_DECLARE_DYNAMIC(OpDescControlDestroyMsg); 
	OpDescControlDestroyMsg(OpDescriptor* OpD,
			  			   CGadgetID GadID, CGadgetID SetGadget, DialogOp* pDialogOp):
		OpDescMsg(OpD, GadID, SetGadget, pDialogOp)
	{
	}; 
};

/********************************************************************************************

>	class OpDescControlMsg: public OpDescMsg

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This message is the OpDescriptor equivelant of a DialogMsg. It is sent 
				when an OpDescriptor's control sends a message.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

class OpDescControlMsg: public OpDescMsg
{
	CC_DECLARE_DYNAMIC(OpDescControlMsg); 
	
	public:
	
	CDlgMessage DlgMsg;

	OpDescControlMsg(OpDescriptor* OpD, CDlgMessage DlgM, CGadgetID GadID, 
					 CGadgetID SetGadget, DialogOp* pDialogOp):
		OpDescMsg(OpD, GadID, SetGadget, pDialogOp)
	{
		DlgMsg = DlgM; 
	};
 
};


class DialogBarOp;



/********************************************************************************************
>	class GadgetListItem

Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
Created:	17/5/94
Purpose:	Put on a list by OpDescriptor::BuildGadgetList.  Contains a pointer to a
			DialogBarOp and a CGadgetID for a control associated with an OpDescriptor.
			Data fields are PUBLIC for easy access.
SeeAlso:	OpDescriptor::BuildGadgetList
********************************************************************************************/

class GadgetListItem : public ListItem
{
	CC_DECLARE_MEMDUMP(GadgetListItem)

public:
	DialogOp*    pDialogOp;
	CGadgetID	 gidGadgetID;

	GadgetListItem(DialogOp* pop, CGadgetID gid)
	  : pDialogOp(pop), gidGadgetID(gid) { /* empty */ }
};

#endif		// INC_OPDESC
