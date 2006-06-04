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
//	ColourEditDlg		Colour Editor dialogue box


#ifndef INC_COLDLOG
#define INC_COLDLOG

#include "collist.h"
//#include "colordlg.h"		// Winoil dialogue resources
#include "colourix.h"
#include "dialogop.h"
#include "dragtrgt.h"
#include "dragcol.h"


struct ReDrawInfoType;		// pre-declare Kernel-redraw structure to reduce dependencies
class ColourDropDown;		// Colour drop-down menus
class OpColEditCommand;		// Pop-up menu
class Cursor;
class KeyPress;
class Layer;
class ColourListComponent;


/***********************************************************************************************

<	ColEditDragArea

	Comment:	This enumerated type describes where the last drag in the colour editor 
				originated. This may be any of the colour patches, or in the colour
				picker display - these gadgets may be further subdivided (e.g. the
				HSV picker subdivides into the H/S colour wheel and the Value slider)

***********************************************************************************************/

typedef enum
{
	CEDRAG_NONE,						// Not dragging

	CEDRAG_DEFAULT_COMPONENT1,			// Dragging a default-picker component slider
	CEDRAG_DEFAULT_COMPONENT2,
	CEDRAG_DEFAULT_COMPONENT3,
	CEDRAG_DEFAULT_COMPONENT4,

	CEDRAG_HSVPICKER_HSWHEEL,			// Dragging the circular HSV picker wheel/slider
	CEDRAG_HSVPICKER_VSLIDER,

	CEDRAG_HSVPICKER_HSLIDER,			// Dragging the square HSV picker square/slider
	CEDRAG_HSVPICKER_VSSQUARE,

	CEDRAG_RGBPICKER_XY,				// Dragging the RGB picker XY cube face or Z cube side
	CEDRAG_RGBPICKER_Z,
	CEDRAG_RGBPICKER_BRIGHT,			//    (Brightness slider, if implemented)

	CEDRAG_CMYKPICKER_XY,				// Dragging the CMYK picker XY cube face or Z cube side
	CEDRAG_CMYKPICKER_Z,
	CEDRAG_CMYKPICKER_KEY,				//    Key slider

	CEDRAG_TINTORSHADE					// Dragging tint or shade value
} ColEditDragArea;



typedef struct
{
	IndexedColourType	ColType;		// The last colour type the editor displayed
	INT32				Folded;
	ColourModel		DisplayModel;		// The last display model the editor was put in
	IndexedColour	*ResultColour;		// The last result colour we were editing
	BOOL			Initialised;		// TRUE once the controls have been init'd once
	BOOL			ParentListOK;		// TRUE while we believe the parent list is up to date
} ColEditState;



typedef enum							// Return value indicating degrees of constraint
{
	CROSSCONSTRAINED_NONE,
	CROSSCONSTRAINED_HORZ,
	CROSSCONSTRAINED_VERT,
	CROSSCONSTRAINED_BOTH
} CrossConstraint;

// do we need to set the abort colour?  or have we already set it ....  This is used by the
// custom colour picker control.

enum AbortColourState { SAVE_ABORTCOLOUR, SAVED_ABORTCOLOUR };




/***********************************************************************************************

>	class ColourEditDlg: public DialogOp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94

	Purpose:	A kernel colour editor dialogue. This is called if necessary
				by the winoil ColourPicker class to provide a colour editor.
				This calls back down into ColourPicker winoil to do some things,
				but this system allows 95% of the code to be kept in the kernel,
				while still allowing ColourPicker to give access to a local OS
				editor if a suitable one is available.
	
	SeeAlso:	ColourPicker

***********************************************************************************************/

#define OPTOKEN_COLOUREDITDLG _T("ColourPicker")

#define COLED_TIMER_ID 76
#define COLED_TIMER_FREQUENCY 100

class ColourEditDlg: public DialogOp
{
friend class ColEditorDragTarget;
friend class ColourPicker;				// Only for BODGE for status line text - see w\colpick.cpp
friend class OpColEditCommand;			// For context sensitive menu stuff

	CC_DECLARE_DYNCREATE(ColourEditDlg)

public:		// Construction/destruction
	ColourEditDlg();
	~ColourEditDlg();


public:		// Public interface
	static void InvokeDialog(ColourList *ParentList, IndexedColour *ColourToEdit);
			// Show the dialogue, and edit the given colour (or if NULL, choose a nice
			// local colour to edit)

	static BOOL CanYouEditThis(ColourList *ParentList, IndexedColour *ColourToEdit,
								BOOL ForceNewMode = FALSE);
			// Returns TRUE if the colour editor will accept the given colour for editing.
			// If you need to enable/disable UI which calls InvokeDialog(), use this method.
			// Do NOT use ForceNewMode - it is an internal parameter which causes side effects!

	static void ForceLineOrFillMode(BOOL PreferLineColour = FALSE);
			// Set whether editor will edit line or fill local colours


public:		// Overridden DialogOp interfaces
	MsgResult Message(Msg *Message);

	void Do(OpDescriptor *MyOpDesc);
	void DoWithParam(OpDescriptor *MyOpDesc, OpParam *Param);

	static BOOL Init();
	BOOL Create();
	static OpState GetState(String_256*, OpDescriptor*);

	static const ResourceID IDD;

	static const CDlgMode Mode;

	virtual BOOL TimedProcessing(void);	// Processing which has to be performed at fixed intervals
	BOOL UpdateColourEditor ();			// replacement of above

	BOOL StartingPickerDrag();
	BOOL EndingPickerDrag();

private:
	BOOL MakeNewNColour(void);			// Internal helper function for Message()
	BOOL MakeColourASpot(void);			// Internal helper function for Message()


protected:		// Menu upcalls
	OpState GetCommandState(StringBase *Command, StringBase *ShadeReason);
						// Determines the (Op)State of the given menu command

	void DoCommand(StringBase *Command);
						// Applies the given menu command


public:			// Upcall from Application::OnKeyPress
	static BOOL OnKeyPress(KeyPress* pKeyPress);
			// Handles a keypress, and swallows all keyboard inputs during drags.


protected:		// Preference values
	static INT32		DefaultDisplayModel;	// Preference: [0..15] Default display colour model (LOCAL)
	static INT32		DefaultDisplayModelN;	// Preference: [0..15] Default display colour model (STYLE)
	static INT32 		AutoModelChange;		// Preference: TRUE for auto DisplayModel change (LOCAL)
	static INT32 		AutoModelChangeN;		// Preference: TRUE for auto DisplayModel change (STYLE)
	static INT32 		Folded;					// Preference: TRUE if the window is folded
	static INT32		SplitLineDisplay;		// Preference: Show sliders in split gradfill way
	static INT32		Use3DDisplay;			// Preference: Use 3D displays where available
	static BOOL		bHSVHueAtTop;
	static BOOL		bUsePrintCMYK;

	static BOOL		SetUnitGroupDefaults(ColourModel ColModel);	// Preference: the units for the colour values

	static AbortColourState abortColourState;	// do we need to set the abort colour?  or have
												// we already set it ....  This is used by the
												// custom colour picker control.
	static BOOL		EscapeKeyWasPressed;		// records the fact of whether the escape key has
												// been pressed

public:
	/*****************************************************************************************\
		
		Custom colour picker interface functions

	\*****************************************************************************************/
	
	// these functions allows camelots custom colour picker control to interface directly
	// with the ColourEditDlg ....
	
	static ColourEditDlg* GetColourEditDlg () { return (TheEditor); }
	
	static void SetEditingColour (IndexedColour* newEditingColour, Node* pSourceNode = NULL);
	static void SetAbortColourState (AbortColourState state) { abortColourState = state; }
	
	static void ApplyEditingColourNow (BOOL UseAbortColour = FALSE);
	static BOOL DoAbortColourNow (CWindowID colourPicker);
	static void DoHandleNamedcolour ();

	static void SetDefaultDisplayModel (INT32 newDisplayModel);	// really a support function for
																// the colour dialog, BUT
																// solves problems that the
																// above creates

	void CloseColourEditor ();   // closes the dialog

	void SetDoTimerProcessing(BOOL Value);
	BOOL GetDoTimerProcessing();

	static void AbortColourPickerDrag();						// Abort any colour picker drag
																// without trying to apply the colour
	/*****************************************************************************************\
		
		End of custom colour picker interface functions

	\*****************************************************************************************/

protected:		// Internal data
	static ColourEditDlg *TheEditor;	// NULL or points to the one & only colour editor instantiation

	BOOL			AmShaded;			// TRUE if the window is currently shaded

	IndexedColour	OriginalColour;		// The original colour definition (before editing)
	IndexedColour	*ResultColour;		// The colour into which the approved result goes
	IndexedColour	*EditingColour;		// The colour we're currently editing
	IndexedColour	*OriginalEditingColour;		// The colour we're currently editing
	ColourList		*ParentList;		// The list in which it resides

	BOOL			NoFillButtonDown;	// we selected a no fill coloured object or press the button
	static BOOL		EditingLineColour;	// For unnamed col: Editing line(T) or fill(F)?
	BOOL			FirstEdit;			// For unnamed col: We have not changed this colour
										// at all yet (used to avoid undo on subsequent edits)

	static BOOL		UpdateOnNextIdle;	// To stop update flicker while loading files, etc.
										// TRUE if the next idle event should also update all window controls

	static ColourModel DisplayModel;	// The model in which the colour is currently being displayed
	static INT32		ColourPickerMode;	// (Axis orientation) of the picker, where applicable

	BOOL			ISentTheMessage;	// Lock used to stop us responding to msgs we sent

	ColourDropDown	*NameDropDown;		// Colour drop-down list support for the colour list and
	ColourDropDown	*ParentDropDown;	// the parent-colour list

	static BOOL resetColPickPos;		// We need to reset the location of camelots custom
										// colour picker control depending upon the colour model
	static RECT colPickOrigRect;		// Obviously to do this, we need to know where it was
										// originaly located
	static BOOL needColPickHidden;		// Is the custom colour picker control hidden?
	static BOOL colPickHidden;			// Is the custom colour picker control hidden?

protected:		// Drag data
	ColEditDragArea	DragStartArea;		// Which area of the picker a drag started in
										// Will be CEDRAG_NONE if we don't think we're dragging
	DocCoord		LastDragPos;		// Where the last drag position was, to allow us
										// to only update if the position changes significantly
	DocCoord		DragAnchor;			// Memory of where the drag started (Used relative to the
										// point being dragged in 3d modes to anchor the Z-slider)

	BOOL			DragUpdatedOnIdle;	// Flag to stop multiple idle events doing multiple updates
	ColourGeneric	ColourBeforeDrag;	// Memory of the colour before we dragged, for constrain
										// (Held in the current display ColourModel)

	IndexedColour	*AbortColour;		// Memory of the colour before drag, for escape-pressed aborts


protected:		// Internal methods
	void CloseMyself(void);				// Closes the window and tidies up

										// Shades/unshades all gadgets in the window
	void ShadeMyself(BOOL UnShade = FALSE, BOOL OtherGadgetsOnly = FALSE);

	void SetControls(void);				// Sets the controls up from the colour
	
	void SetColourNameList(void);		// Sets the name field combobox list

	void RedrawColourNameList(void);	// Ensures that the colour name list shows the correct colour
										// for the newly edited EditingColour

	void SetExtent(void);				// Sets the extent of the window

	void InvalidateAndSetControls(void);
										// Sets the controls and redraws GDraw bits

	void InvalidatePatchGadget(ReDrawInfoType *Info = NULL);
										// Invalidates the current colour patch

	void SetColour(BOOL SetComponents = TRUE);
										// Reads the colour back from the controls

										// Sets and shades/unshades a component name
	void SetComponentInfo(UINT32 ComponentID, UINT32 NameGadget,
							UINT32 WritableGadget, UINT32 InheritGadget);
										// Sets a slider/writable gadget combination

	void SetAllHexComponentsInfo(UINT32 ComponentID, UINT32 NameGadget,
								 UINT32 WritableGadget);
										// Sets a slider/writable gadget combination (for web-rbgt)

	void FindUsefulColourToEdit(BOOL WantLineColour = FALSE,
								BOOL TheWindowIsOpen = TRUE);
										// Finds a useful colour, and 'EditThisColour's it

	//(WEBSTER - markn 14/12/96)
	void ApplyNoColour(BOOL LineColour);					// Applies no colour to the selection 
	void LimitTo216Only();									// Snaps the colour to the closest colour in the web browser palette
	FIXED24 Nearest20Percent(FIXED24 n);		// Helper function for LimitTo216Only()

	void EditThisColour(ColourList *ParentList, IndexedColour *ColourToEdit,
						BOOL TheWindowIsOpen = TRUE);
										// Switches the editor over to a different colour

	void EditingColourHasChanged(BOOL RedrawControls = TRUE, BOOL RedrawPicker = TRUE,
									BOOL FastTrackPicker = FALSE,
									Node* pSourceNode = NULL);
										// Handles changes to the EditingColour

	void ApplyColourNow(IndexedColour *ColourToApply);
										// Applies a Colour to the doc selection. USE WITH CARE

	IndexedColour *FindSafeParent(IndexedColour *SuggestedParent, BOOL ChildIsATint);
										// Determines a safe parent to link EditingColour to

	void CompileParentColourList(CGadgetID TargetGadget);
										// Compiles a list of potential parents for EditingColour
										// into a combo box list

	IndexedColour *DecodeParentColourListSelection(INT32 Index);
										// Decodes a click on the above-mentioned list

	BOOL OnMakeLocalToFrame();
	BOOL ShouldUngrey(IndexedColour *pCol);

protected:		// Current window gadget/extent state (used to minimise unnecessary redraws)
	void ResetState(void);				// Resets the current state information to 'unknown'
	ColEditState	State;				// Current state of the window

	BOOL	IsSetGadgetValueCalled() const;
	BOOL	EnteredSetGadgetValue();
	BOOL	ExitedSetGadgetValue();

	BOOL	m_bDoingSetGadget;			// Are we calling SetGadgetValue()?

protected:		// Kernel-rendered control support methods

	static BOOL GetColourContext(ColourModel ColModel, ColourContext** ppContext);
	
										// Renders a marker cross into a render region
	void RenderCross(RenderRegion *pRender, DocRect *CrossRect,
							INT32 PixelSize, BOOL Shaded = FALSE);

										// Renders a marker cross for the parent colour
	void RenderParentCross(RenderRegion *pRender, DocRect *CrossRect, INT32 PixelSize);

										// Renders a constraint "bubble" around End1 and End2
	void RenderConstraint(RenderRegion *pRender, DocCoord End1, DocCoord End2);

										// Handles rendering of kernel-rendered controls
	void RenderControl(UINT32 GadgetToRender, ReDrawInfoType* RedrawInfo);

										// Rendering the picker control (for DisplayMode)
	void RenderPickerHSV(RenderRegion *pRender, DocRect *VirtualSize,
							INT32 PixelSize, DialogColourInfo *RedrawColours,
							DocRect *pClipRect);

	void RenderPickerRGB(RenderRegion *pRender, DocRect *VirtualSize,
							INT32 PixelSize, DialogColourInfo *RedrawColours);

	void RenderPickerCMYK(RenderRegion *pRender, DocRect *VirtualSize,
							INT32 PixelSize, DialogColourInfo *RedrawColours);

	void RenderPickerDefault(RenderRegion *pRender, DocRect *VirtualSize,
							INT32 PixelSize, DialogColourInfo *RedrawColours);

	void RenderPickerTint(RenderRegion *pRender, DocRect *VirtualSize,
							INT32 PixelSize, DialogColourInfo *RedrawColours);

	void RenderPickerShade(RenderRegion *pRender, DocRect *VirtualSize,
							INT32 PixelSize, DialogColourInfo *RedrawColours,
							DocRect *pClipRect);

	void RenderPickerSliderH(RenderRegion *pRender, DocRect *SliderRect,
										INT32 PixelSize, FIXED24 DisplayValue,
										DocColour *LeftCol, DocColour *RightCol,
										DocColour *TopLeftCol, DocColour *TopRightCol,
										BOOL Shaded = FALSE);


protected:		// Kernel-rendered control click/drag handlers
				// God, this is getting so hideous!
										// Drag start/continue/end handlers
	void StartDrag(ReDrawInfoType *Info);
	void UpdateDrag(ReDrawInfoType *Info);
	void EndDrag(ReDrawInfoType *Info);
	// SeeAlso: OnIdleEvent(), above

										// Handles setting new colour values from click/drags
	void SetNewValueFromMousePos(ReDrawInfoType *Info, BOOL StartingNewDrag);
																				
										// Handling click/drags (for DisplayMode)
	void SetNewValueHSV(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag);
	void SetNewValueRGB(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag);
	void SetNewValueCMYK(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag);
	void SetNewValueDefault(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag);
	void SetNewValueTint(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag);
	void SetNewValueShade(ReDrawInfoType *Info, INT32 PixelSize, BOOL StartingNewDrag);

										// Handle pointer changes and bubble help
	BOOL HandleIdlePointer(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp, UINT32 *ControlID);
	BOOL HandleIdlePointerHSV(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp);
	BOOL HandleIdlePointerRGB(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp);
	BOOL HandleIdlePointerCMYK(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp);
	BOOL HandleIdlePointerDefault(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp);
	BOOL HandleIdlePointerTint(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp);
	BOOL HandleIdlePointerShade(ReDrawInfoType *Info, String_128 *BubbleHelp, String_256 *StatusHelp);

										// Internal shared cross-position calculations
	CrossConstraint CalcCrossRectRGB(ColourRGBT *SourceColour, DocRect *SquareRect, INT32 PixelSize,
											DocRect *Result);
	CrossConstraint CalcCrossRectCMYK(ColourCMYK *SourceColour, DocRect *SquareRect, INT32 PixelSize,
											DocRect *Result);

										// Internal function to specify which cursor shape
										// should be made active (Called by HandleIdlePointer)
										// (If not set by HandleIdlePointer, it reverts to arrow)
	void SetCursor(UINT32 CursorResID);


protected:
	void EndTimedProcessing();
	void BeginTimedProcessing();
	UINT32 CurrentCursorID;				// Current cursor resource identifier (or NULL)
	Cursor *CurrentCursor;				// Current cursor object (or NULL)

	BOOL	m_bDoTimerProcessing;     // flag indicates whether or not to do timer processing

	void HideGadgetList(CGadgetID * Gadgets, BOOL Hide=TRUE);
	void HideOrShowColourPicker();
	void CheckDialogSize();

	BOOL m_NeedsResize;
	void NeedsResize() {m_NeedsResize = TRUE;}

	virtual BOOL OnIdleEvent();

private:
	void LoseKeyboardFocus(void);
		// Lobs the keyboard focus back into the document
};



/*********************************************************************************************

>	class 	ColEditorDragTarget : public KernelDragTarget

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Purpose:	Handles dragging of colours into the colour editor window.
				Multiple instances of this class are created to handle the different areas
				of the colour editor (which one they are working on is determined from the
				gadget ID for which the target is created)

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget

	Documentation:	Docs\Specs\DragMan.doc

*********************************************************************************************/

class ColEditorDragTarget : public KernelDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(ColEditorDragTarget)

public:	// Public interface
	ColEditorDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL);
	
	virtual UINT32 GetCursorID();
	virtual BOOL GetStatusLineText(String_256 * TheText);

protected:
	// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								OilCoord *pMousePos, KeyPress* pKeyPress);

	// Determine if we'll accept a drop here
	BOOL CanDropHere(DragInformation *pDragInfo);
};



/********************************************************************************************

>	class ColEditorDragInfo : public ColourDragInformation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/95
	Purpose:	Contains info that a colourdrag may find useful - used for drags
				from the colour editor
				
********************************************************************************************/

class ColEditorDragInfo : public ColourDragInformation
{ 	
CC_DECLARE_DYNCREATE(ColEditorDragInfo)

public:
 	ColEditorDragInfo();
 	ColEditorDragInfo(IndexedColour *Colour, BOOL IsAdjust, Document* pParent);
};



/********************************************************************************************

>	class CCAPI OpMakeColourLocalToFrame: public UndoableOperation

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Purpose:	Operation to create copy of a colour (and its descendants) for the current 
				frame.
				
********************************************************************************************/

class CCAPI OpMakeColourLocalToFrame: public UndoableOperation
{
	CC_DECLARE_DYNCREATE( OpMakeColourLocalToFrame )

public:
	OpMakeColourLocalToFrame();
	~OpMakeColourLocalToFrame();

	static BOOL		Init();
	static OpState	GetState(String_256 *, OpDescriptor *);
	void			Do(OpDescriptor *);
	void			DoWithParam(OpDescriptor *, OpParam *);
	virtual void PerformMergeProcessing() ; 

	virtual BOOL Undo();
	virtual BOOL Redo();

protected:
	BOOL ReplaceColourOnActiveLayer(IndexedColour *pOld, IndexedColour *pNew);
	BOOL IsColourLocalToFrame(Layer *pTheLayer, IndexedColour *pCol);
	IndexedColour *MakeColourLocalToFrame(IndexedColour *pCol);

protected:	
	ColourList		*ParentList;		// The list in which it resides
	Document		*pOurDoc;
	Layer			*pActiveLayer;
	ColourListComponent *ColComp;
	Operation		*m_pPreviousOp;
};




#endif

