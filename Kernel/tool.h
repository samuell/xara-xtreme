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


#ifndef INC_TOOL
#define INC_TOOL

#include "doccoord.h"
#include "binds.h"
#include "clikmods.h"
#include "ccobject.h"
#include "dlgtypes.h"
#include "list.h"


class OILTool;          // #include "oiltool.h"
class DocRect;

// Forward decls of ToolList and ToolListItem for use by Tool_v1/Tool classes.
class ToolList;
class ToolListItem;
class Spread;
class KeyPress;

// Everyone tool needs a valid Tool ID. They get allocated here. Camelot
// tools written by us should have an ID in the range 2 - 49. The range
// 49-99 incl are reserved for third-party tools, while the range 0 - 1
// is reserved for special cases!

// these used to be const UINTs, which was fine, except that they are now used in 
// .rc files which needs #defines

#define TOOLID_INVALID          0

#define TOOLID_TEST                     2
#define TOOLID_PUSH                     3
#define TOOLID_ZOOM                     4
#define TOOLID_RECTANGLE        5
#define TOOLID_FREEHAND         6
#define TOOLID_SELECTOR         7
#define TOOLID_RECT                     8
#define TOOLID_ROTATE           9
#define TOOLID_GRID                     10
#define TOOLID_BEZTOOL          11
#define TOOLID_ELLIPSE          12
#define TOOLID_GRADFILL         13
#define TOOLID_PEN                      14
#define TOOLID_BLANK            15
#define TOOLID_BLEND            16
#define TOOLID_TRANSP           17
#define TOOLID_REGSHAPE         18
#define TOOLID_MOULD            19
#define	TOOLID_ACCUSOFT			20
#define TOOLID_TEXT				21
#define TOOLID_SOFTSHADOW		22
#define TOOLID_BEVELTOOL		23
#define TOOLID_CONTOURTOOL		24
#define TOOLID_SLICETOOL		25
#define TOOLID_LIVEEFFECT		26


// The op tokens for each of the tools
// A tool's op token is "TOOL" with the tool ID appended on the end

#define TOOL_OPTOKEN_PUSH       "TOOL3"
#define TOOL_OPTOKEN_ZOOM               "TOOL4"
#define TOOL_OPTOKEN_RECTANGLE  "TOOL5"
#define TOOL_OPTOKEN_FREEHAND   "TOOL6"
#define TOOL_OPTOKEN_SELECTOR   "TOOL7"
#define TOOL_OPTOKEN_RECT               "TOOL8"
#define TOOL_OPTOKEN_ROTATE             "TOOL9"
#define TOOL_OPTOKEN_GRID               "TOOL10"
#define TOOL_OPTOKEN_BEZTOOL    "TOOL11"
#define TOOL_OPTOKEN_ELLIPSE    "TOOL12"
#define TOOL_OPTOKEN_GRADFILL   "TOOL13"
#define TOOL_OPTOKEN_PEN                "TOOL14"
#define TOOL_OPTOKEN_BLANK              "TOOL15"
#define TOOL_OPTOKEN_BLEND              "TOOL16"
#define TOOL_OPTOKEN_TRANSP             "TOOL17"
#define TOOL_OPTOKEN_REGSHAPE   "TOOL18"
#define TOOL_OPTOKEN_MOULD              "TOOL19"
#define TOOL_OPTOKEN_TEXT 		"TOOL21"
#define TOOL_OPTOKEN_SOFTSHADOW "TOOL22"
#define TOOL_OPTOKEN_BEVEL		"TOOL23"
#define TOOL_OPTOKEN_CONTOUR	"TOOL24"
#define TOOL_OPTOKEN_SLICE		"TOOL25"
#define TOOL_OPTOKEN_LIVEEFFECT	"TOOL26"

const UINT32 TOOLID_MAX           =       99;


/********************************************************************************************

>       class ToolInfo_v1 : SimpleCCObject

	Author:         Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:        19/7/93
	Purpose:        Provide information on a tool.
	SeeAlso:        ToolInfo; Tool

********************************************************************************************/

class ToolInfo_v1 : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(ToolInfo_v1);
	
public:
	ToolInfo_v1();  // Default constructor - sets up ToolInfo default values

	UINT32            InfoVersion; // Determines how many of the remaining fields are filled in.
	UINT32            InterfaceVersion;
	UINT32            Version;
	UINT32            ID;
	UINT32            ParentModuleID;
	UINT32            TextID;
	TCHAR*            Family;
	TCHAR*            Name;
	TCHAR*			  Purpose;
	TCHAR*			  Author;
	UINT32            BubbleID;       // The bubble help resource ID, to be displayed when the
							// cursor moves over the tool button. 
	UINT32            StatusID;       // The status bar resource ID, to be displayed when cursor 
							// moves over the tool button.
	CDlgResID       InfoBarDialog;

	// In Camelot there are different CurrentAttribute groups for different types of object.
	// For example Text will have a different set of attributes (its attribute group) to path
	// objects. When specifying a current attribute the selected tool determines which current
	// attribute group to add the attribute to. 

	CCRuntimeClass* CurrentAttributeGroup;  // Default = NodeRenderableInk
};

/********************************************************************************************

>       class ToolInfo : public ToolInfo_v1

	Author:         Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:        19/7/93
	Purpose:        Provide information on a tool.  The fields are as follows:
				MonoOn
				InfoVersion                     Indicates how much of this stucture is understood by the
									tool.
				InterfaceVersion    The version of the kernel<->tool interface understood by
									the tool.
				Version                         The tool's own version number.
				ID                                      The unique ID of this tool (allocated by Computer
									Concepts Ltd).
				ParentModuleID          The unique ID of the module that provides this tool
									(allocated by Computer Concepts Ltd).
				Family                          A string describing the tool family this tool belongs
									to - e.g. "Path tools".
				Name                            A string describing the tool - e.g. "Rectangle tool".
				Purpose                         A string decribing what the tool is used for, e.g.
									"For drawing rectangles".
				Author                          A string naming the author(s) of the tool, e.g.
									"Computer Concepts Ltd".
				InfoBarDialog           Resource ID of the dialog used in the infobar for this
									tool.
				MonoOff
									
	SeeAlso:        Tool

********************************************************************************************/

class ToolInfo : public ToolInfo_v1
{
	CC_DECLARE_MEMDUMP(ToolInfo);
	// No extra members needed.
};



/********************************************************************************************

<	DragEndType

	Comment:    Describes how a drag was ended. Dragging operations can call the tools
				virtual function DragFinished() with a var of this type so that the
				tool can take appropriate action.  NB. the tool can cancel the effects
				of a drag by returning FALSE from the DragFinished() function.  THERE IS
				*NO* NEED TO DO THIS IF YOU RECEIVE A CALL TO DragFinished() WITH THE
				DT_CANCELLED PARAMETER!
				MonoOn
				DT_CANCELLED,		// the drag was cancelled by the user (eg. ESC pressed)
				DT_DIDNOTMOVE,      // The Drag did not move at all
				DT_MOVEDTOSAME,     // The Drag moved but ended up in the same place
				DT_MOVEDTODIFF		// The drag moved & ended up in a different place
				MonoOff

********************************************************************************************/

enum DragEndType
{
	DT_CANCELLED,			// the drag was cancelled by the user (eg. ESC pressed)
	DT_DIDNOTMOVE,          // The Drag did not move at all
	DT_MOVEDTOSAME,         // The Drag moved but ended up in the same place
	DT_MOVEDTODIFF         	// The drag moved & ended up in a different place
};



/********************************************************************************************

>       class Tool_v1 : SimpleCCObject

	Author:         Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:        19/7/93
	Purpose:        Specifies the interface between tools and the Kernel/OIL.
	SeeAlso:        Tool

********************************************************************************************/

class CCAPI Tool_v1 : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(Tool_v1);

public:
	// Initialise Parent member
	Tool_v1() { Parent = NULL; };
	
	virtual ~Tool_v1();
	
	// Initialise the tool
	virtual BOOL Init();
	
	// What version of the Tool interface does this tool understand?
	virtual UINT32 GetToolInterfaceVersion() { return 1; } ;
	
	// Efficient access to the Tool's ID
	virtual UINT32 GetID() { return 0; } ;
	
	// Get info on the tool.
	virtual void Describe(void* Info);

// Example handlers - change these to have sensible names and parameters once
// real tool work starts (i.e. when we know what sort of handlers we need).

	// Notify the current tool that the mouse has moved.
	virtual void OnMouseMove(DocCoord, Spread*, ClickModifiers);

	// Notify the current tool of click events on the document...
	virtual void OnClick(DocCoord, ClickType, ClickModifiers, Spread*);

	// Notify the current tool of a key press event...
	virtual BOOL OnKeyPress(KeyPress* pKeyPress);

	// Notify the current tool of an idle event...
	virtual BOOL OnIdle();

	// Interogate the current tool for status line text
	virtual BOOL GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);

	// Allows tools to register a key combination for switching to the tool temporarily
	virtual BOOL RegisterToolSwitch(KeyPress* pKeyPress, BOOL FromFile = FALSE);

	// Notify the tool when a drag ends
	virtual BOOL DragFinished(DragEndType HowEnded);
	
	// Notify the Current Tool that it has to render its blobs
	virtual void RenderToolBlobs(Spread*, DocRect*);

	// Allow the selection and deselection of a tool
	virtual void SelectChange(BOOL IsSelected);

	// Easy access to the tool's info structure
	ToolListItem *Parent;

	// are we the current tool or not. Note NOT virtual
	BOOL IsCurrent() const;

	// this is so we can quickly go from a tool ID to a module ID
	// if 0 is in here then it means the resources are in the .exe, not in any .DLLs
	static UINT32 GetOwnerModule( UINT32 );
	static BOOL SetOwnerModule( UINT32, UINT32 );

	// Return TRUE if you render tool blobs on selection change messages
	virtual BOOL AreToolBlobsRenderedOnSelection() {return FALSE;}

protected:
	static UINT32 OwnerModuleList[ TOOLID_MAX + 1 ];
};




/********************************************************************************************

>       class Tool : public Tool_v1

	Author:         Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:        21/6/93
	Purpose:                The Tool type reflects the latest Tool specification. Do NOT subclass from
				Tool, but use the latest Tool revision (e.g. Tool_v42). The Tool type should
				only be used by the Kernel and OIL layers, never by tools themselves.
	SeeAlso:        ToolInfo

********************************************************************************************/

// This should always inherit from the most modern definition of the Tool_vnn class

class CCAPI Tool : public Tool_v1
{
	CC_DECLARE_MEMDUMP(Tool);

public:
	static BOOL InitToolList();
	static BOOL InitTools();
	static void SelectFirstTool();
	static void DeinitTools();
	static OILTool *GetOILTool(UINT32 ToolID);

	// Making and Finding out which tool is current
	static  Tool *GetCurrent();
	static  UINT32 GetCurrentID();
	void  SetCurrent();
	static  void  SetNoCurrent();
	void SetButtons(BOOL State);
    // ToolList Manipulation Functions
    static UINT32 GetNumTools();
	// initial tool id
	static UINT32 InitTool;
	static ToolListItem *Find(UINT32 ToolID);
	static ToolListItem *Declare(Tool *, UINT32 ParentID);
    static ToolListItem *GetFirstTool();
	static ToolListItem *GetNextTool( ToolListItem* CurrTool );
	static UINT32 GetModuleID(UINT32 ToolID);
	static BOOL SelectATool();
	static Tool* FindTool(UINT32 ToolID);

	// Handle temporary tool activation via hot-keys.
	static void ActivateTemporaryTool(Tool*);
	static void DeactivateTemporaryTool();
	//static BOOL IsCurrentToolTemporary();
	static BOOL SwitchTool(KeyPress* pKeyPress);                // Switches tool on a key press
	static void RemoveDirectRegToolSwitches(Tool* pThisTool);   // Delete directly registered tool switches for this tool
	static void     DeleteTempToolList();                       // Deletes the list of temporary tools

	static void SetToolChangingState(BOOL state);
	static BOOL IsToolChanging();

	static void OnActivateApp(BOOL bActive);					// Tell the tool the app's active state has changed
	static void EndDrag();										// Tell the tool when the drag has ended
	static void ServicePendingToolChange();						// Changes too if needed.


//      operator Tool_v1 *() { return (Tool_v1*) this; }

private:

	static ToolList *Tools;
	static Tool* Current;                                   // Pointer to the current tool
//      static Tool* PrevTool;

	static BOOL ToolChanging;
	static BOOL PendingDeactivateTempTool;
	static List TempToolList;
};


#endif          // INC_TOOL
