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
// sgline.h - the LineGallery (Line SuperGallery) class

#ifndef INC_SGLINE
#define INC_SGLINE

#include "sgallery.h"
#include "sgtree.h"
#include "brshgdgt.h"


// These define the default size and formatting of items within the line gallery.
const MILLIPOINT c_nHorzSizeOfItem      = 85000L;
const MILLIPOINT c_nVertSizeOfItem      = 20000;
const MILLIPOINT c_nHorzSizeOfText      = 28000;
const MILLIPOINT c_nVertSizeOfText      = 15000;
const MILLIPOINT c_nHorzGapAfterText    = 4000;
const MILLIPOINT c_nVertGapAboveText    = 10000;
const MILLIPOINT c_nHorzGapBetweenItems = 22000;
const MILLIPOINT c_nVertGapBetweenItems = 4000;
const MILLIPOINT c_nDefaultLineWidth    = 3000;
const MILLIPOINT c_nAveCharWidth		= 6000;


class StrokeTypeAttrValue;
class BrushAttrValue;

/********************************************************************************************
>   class LineAttrGroup : public SGDisplayGroup

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    17/2/95
	Purpose:    Specialises the SGDisplayGroup class, overriding the function that
				builds up the title string.  This derived class can have any string
				as its title.
	SeeAlso:    -
********************************************************************************************/

class CCAPI LineAttrGroup : public SGDisplayGroup
{
public:
	// Creation & destruction.
	LineAttrGroup(const String_64& strTitle, SuperGallery* pGal);

	// Returns a reference to this group's title.
	const String_64& GetTitle() const;
	
};



/***********************************************************************************************
>   class LineAttrItem : public SGDisplayItem

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:    15/2/95
	Purpose:    This DisplayTree node is used by the Line SuperGallery
				It is responsible for providing the ability to handle/redraw one LineAttrItem
				displayed in said gallery.
	SeeAlso:    LineGallery; SuperGallery; SGDisplayItem
***********************************************************************************************/

class CCAPI LineAttrItem : public SGDisplayItem
{
public:
	// This used to pass text positions to the Render() method, below.
	enum TextPosition
	{
		NO_LABEL,           // don't display a text description
		LEFT,               // display a description to the left of the item
		BELOW               // display a description below the item
	};

	// These functions read and write the description string associated with this item.
	const String& GetDescription() const;
	void SetDescription(StringBase *pNewDescription);

	// Call this function to calculate how large this item is, in millipoints.
	void GetSize(TextPosition eTextPos, MILLIPOINT* pxSize, MILLIPOINT* pySize) const;

	// Render() will draw the object of the derived class.  It is called by HandleRedraw()
	// and by the gallery dragging code.
	void Render(RenderRegion* pRegion, const DocRect& drBounds, TextPosition eTextPos) const;

	// This function is called if a drag was really an ordinary click.
	virtual void DragWasReallyAClick(SGMouseInfo* pMouse, SGMiscInfo* pMiscInfo);

	// This creates a new line attribute of the appropriate kind.  Derived classes MUST
	// provide an implementation of this function.
	virtual NodeAttribute* CreateNewAttribute(BOOL fIsAdjust) const = 0;

	// This function is called on selection and attribute messages.  It updates the
	// "current attribute" flag of each item.
	static BOOL UpdateCurrentAttribStatus();

	// This function is called by the gallery base-class (?) whenever the mouse is over
	// an item
	virtual BOOL GetStatusLineHelp(DocCoord* pMousePos, String_256* pResult);

	// Used to return the displayed text for this item
	virtual void GetNameText(String_256 *pResult) {if(pResult != NULL) *pResult = m_strDescription;};

	// Added Diccon 1/2000 - overridden in BrushAttrItem
	virtual BOOL ItemSelected (NodeAttribute* pAttr);

protected:
	// Creation & destruction.
	LineAttrItem(const String& strDescription, TextPosition eTextPos);

	// These functions are called by CalculateMyRect below, and determine the size and
	// spacing between items of this type.
	virtual MILLIPOINT GetWidth() const;
	virtual MILLIPOINT GetHeight() const;
	virtual MILLIPOINT GetHorzGap() const;
	virtual MILLIPOINT GetVertGap() const;

	// These functions are called by Render(), above.  SetAttribute() will set the
	// appropriate attributes in the RenderRegion for the derived class.  DrawLine()
	// actually renders the item's line once the attribute(s) have been set.
	virtual void SetAttributes(RenderRegion* pRegion) const = 0;
	virtual void DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const;

	// This function determines whether this item will be labelled on-screen with a text
	// description, and if so, where the description will appear.
	void SetTextPosition(TextPosition eNewTextPos);

	// These functions are called by UpdateCurrentAttribStatus above.  The first returns
	// the run-time class of the attributes this item represents; the second returns TRUE
	// if this item represents the same attribute value as the given attribute.
	virtual CCRuntimeClass** GetAttribRuntimeClasses() const = 0;
	virtual BOOL IsEqualValueToAny(NodeAttribute** pOtherAttribs) const = 0;

private:
	// Event handlers.
	virtual BOOL HandleEvent(SGEventType EventType, void* EventInfo, SGMiscInfo* MiscInfo);
	virtual void HandleRedraw(SGRedrawInfo* RedrawInfo, SGMiscInfo* MiscInfo);

	// Determines item size for the current DisplayMode and calculates FormatRect.
	virtual void CalculateMyRect(SGFormatInfo* FormatInfo, SGMiscInfo* MiscInfo);

	// This overrides the default click behaviour so that it is possible to simple-click
	// on items in more than one group.
	virtual BOOL DefaultClickHandler(SGMouseInfo* pMouse, SGMiscInfo* pMiscInfo);

	// Data members.
	String          m_strDescription;           // description of what this item is
	TextPosition    m_eDefaultTextPosition;     // the default text position for this item
	BOOL            m_fIsCurrentAttrib;         // TRUE if this item is included amongst the
												// selection's current attributes.
	CC_DECLARE_DYNAMIC(LineAttrItem);

protected:
	// The base class returns TRUE, so that an item of this type will auto-close the gallery
	// when CTRL-double-clicked. However, any derived class (e.g. arrowheads) which uses 
	// ctrl-double-click for a special purpose can override this to stop the gallery
	// from auto-closing when a ctrl-double-click occurs.
	virtual BOOL ShouldCloseOnDoubleClick(void);
};



/***********************************************************************************************
>   class LineGallery : public SuperGallery

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:    15/2/95 (Based on the Colour SGallery code)
	Purpose:    The Line SuperGallery class
	Notes:      SuperGalleries are specced in a number of docs including
					specs\sgallery.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)
				There is also howtouse\sgallery.doc which describes creating a supergallery
	SeeAlso:    SuperGallery; SGDisplayLine
***********************************************************************************************/

class LineGallery : public SuperGallery
{
public:
	// Called on program startup.
	static BOOL Init();

	// Returns a pointer to the line gallery, or NULL if there isn't one.
	static LineGallery* GetInstance();

	// Returns TRUE if the line gallery is currently visible on-screen, FALSE otherwise.
	static BOOL IsActive();

	// Creation & destruction.
	LineGallery();
	virtual ~LineGallery();

	// This handles events.
	virtual MsgResult Message(Msg* pMessage);

	// This will create the gallery groups (unless they're already there) and return TRUE if ok.
	BOOL MakeSureGroupsHaveBeenCreated() {return(PreCreate());};

public:
	static void AddNewStrokeItem(StrokeTypeAttrValue *pNewStroke);
		// Adds a new stroke type to the strokes group
	static void AddNewBrushItem(BrushAttrValue* pNewBrush);
		// adds a new brush to the brush group

	BOOL DeleteBrushItem(BrushHandle Handle);  
		// delete the item with the handle supplied

PORTNOTE("other", "Removed CBrushGadget")
#ifndef EXCLUDE_FROM_XARALX
	CBrushGadget* GetBrushGadget() { return &BrushGadget;}
#endif

// Overridden Command interface (for Ops and menu support)
public:	
	virtual BOOL InitMenuCommands(void);
		// Called on startup to initialise any desired menu commands. Will make repeated calls
		// to InitMenuCommand(), below.

	virtual BOOL BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID);
		// Builds the command menu for the given menu pop-up (over an item, or options button)

	virtual OpState GetCommandState(StringBase *CommandID, String_256 *ShadeReason);
		// Returns an OpState indicating the state of this command, much like an Op GetState

	virtual void DoCommand(StringBase *CommandID);
		// Attempts to "do" the given command. Should call down to the base class to handle
		// any commands it can't deal with.
	
	
	void UpdateBrushItem(BrushHandle Handle);
		// updates the given brush item

	// DY 1/2000 sets the current line width member
	MILLIPOINT	SetPreviousLineWidth();
	MILLIPOINT  GetPreviousLineWidth();
private:
	// Overridden upcall methods (what are they, then?)
	virtual BOOL PreCreate();
	virtual BOOL PostCreate();
	virtual BOOL ApplyAction(SGActionType Action);

	// This overrides the default click-selection model, so items in more than one group
	// can be selected simultaneously.
	virtual void SelectionHasChanged();

	// This handles the state of the selection changing by updating the "current attribute"
	// status of all items. This fn is now redundant. It's replaced by OnCommonAttrsChangedMsg
	//virtual BOOL OnSelChangingMsg(SelChangingMsg::SelectionState st);
	virtual BOOL OnCommonAttrsChangedMsg();
	
	// This is called when the user selects the "Apply" button.
	virtual BOOL OnApplyButton(BOOL fIsAdjust);

	// Works out what status text to show, according to what's selected within the gallery.
	void SetStatusText();

	// We override these to create/destroy GDraw RenderRegions, which (currently) is
	// the only kind that can render line-attributes properly.
	virtual RenderRegion* CreateRenderRegion(DocRect* pSize, ReDrawInfoType* pInfo);
	virtual void DestroyRenderRegion(RenderRegion* pRegion);

	static BOOL PrepareBrushFolders();

	// Create a specialised subtree for each type of line attribute.
	BOOL CreateDashGroup();
	BOOL CreateWidthGroup();
	BOOL CreateArrowGroup();
	BOOL CreateCapGroup();
	BOOL CreateJoinTypeGroup();
	BOOL CreateVarWidthGroup();
	BOOL CreateStrokeTypeGroup();
	LineAttrGroup* CreateBrushAttrGroup(String_256 GroupName );
	BOOL CreateBrushGroups();

	// Create the group nodes (headings) and items (helper functions).
	LineAttrGroup* CreateGroup(const String_256& strTitle, BOOL fOpen);
	BOOL CreateItem(LineAttrGroup* pGroup, LineAttrItem* pItem);

	// Run-time type-checking etc.
	CC_DECLARE_DYNCREATE(LineGallery)

	// Data members.
	static LineGallery* m_pTheGallery;
	static double m_anArrowScales[4];

	// used by the new brush to store the old line width so that it can be
	// restored when the brush is deselcted
	MILLIPOINT m_PreviousLineWidth;

PORTNOTE("other", "Removed CBrushGadget")
#ifndef EXCLUDE_FROM_XARALX
	CBrushGadget	BrushGadget;
#endif

	static String_256* m_pFileNameArray; // stores the filenames of the default brush files loaded on startup
	static UINT32	m_NumDefaultFiles;
};



// To remain compatible with existing code, these are the old names for the above classes.
typedef LineAttrItem    SGDisplayLineAttrItem;
typedef LineAttrGroup   SGLineDisplayGroup;
typedef LineGallery     LineSGallery;



/********************************************************************************************
>   class OpDisplayLineGallery: public Operation

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    9/2/95 (Based on the Colour SGallery code)
	Purpose:    Displays the Line gallery.
				This op can be attached to a menu, button or keypress
	SeeAlso:    LineGallery
********************************************************************************************/

#define OPTOKEN_DISPLAY_LINE_GALLERY    TEXT("DisplayLineGallery")

class CCAPI OpDisplayLineGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayLineGallery );

public:
	static BOOL     Init();             
	static OpState  GetState(String_256*, OpDescriptor*);       
	void            Do(OpDescriptor*);

private:
	// Finds the Line gallery class instance
	static SuperGallery* FindGallery(void);
};  

#endif
