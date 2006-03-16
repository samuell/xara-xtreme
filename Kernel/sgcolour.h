// $Id: sgcolour.h 662 2006-03-14 21:31:49Z alex $
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
// SGColour.h - the ColourSGallery (Colour SuperGallery) class

#ifndef INC_SGCOLOUR
#define INC_SGCOLOUR


#include "coplfilr.h"		// For PalettePrefix
#include "doccolor.h"
#include "dragcol.h"
#include "pathname.h"
#include "sgallery.h"
#include "sgdrag.h"
#include "sgtree.h"


// Prototypes
class IndexedColour;
class ColourList;
class ColourListComponent;



/********************************************************************************************

>	class GalleryColourDragInfo : public ColourDragInformation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Purpose:	A colour drag, which was started from the colour gallery

	Notes:		Note that Library/Document colour dragging is handled in the base class

	SeeAlso:	ColourDragInformation
				
********************************************************************************************/
class SGDisplayColour;

class GalleryColourDragInfo : public ColourDragInformation
{ 
	CC_DECLARE_DYNCREATE(GalleryColourDragInfo)
 
public:
	GalleryColourDragInfo();

		// Drag of a "document colour"
 	GalleryColourDragInfo(Document *pDocument, SGDisplayColour *pSourceItem,
							SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 							BOOL IsAdjust = FALSE);

		// Drag of a "library colour"
 	GalleryColourDragInfo(SGDisplayColour *pSourceItem,
							SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 							BOOL IsAdjust = FALSE);
	
	virtual void OnClick(INT32 Flags, POINT Point);

	SGDisplayColour *GetDraggedColour(void)	{ return(SourceItem); }


protected:
	SGDisplayColour *SourceItem;
	SGMouseInfo		MouseInfo;
	SGMiscInfo		MiscInfo;
};



/*********************************************************************************************

>	class 	SGColourDragTarget : public SGListDragTarget

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95

	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.

				This particular target is used for handling drags of ColourSGalllery items
				within their displayed list. It automatically handles drags with draginfo
				derived from ColourDragInformation.

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget;
				SGColourDragTarget::ProcessEvent

	Documentation:	Docs\Specs\DragMan.doc; Docs\HowToUse\SGallery.doc

*********************************************************************************************/

class SGColourDragTarget : public SGListDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(SGColourDragTarget)

public:	// Public interface
	SGColourDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL);

protected:
		// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								OilCoord *pMousePos, KeyPress* pKeyPress);
};





/***********************************************************************************************

>	class SGDisplayColour : public SGDisplayItem

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Purpose:	This DisplayTree node is used by the Colour SuperGallery
				It is responsible for providing the ability to handle/redraw one IndexedColour
				displayed in said gallery.

	SeeAlso:	ColourSGallery; SuperGallery; SGDisplayItem

***********************************************************************************************/

class CCAPI SGDisplayColour : public SGDisplayItem
{
	CC_DECLARE_DYNAMIC(SGDisplayColour);

public:
	SGDisplayColour();
	SGDisplayColour(IndexedColour *ColourToDisplay);


public:		// Virtual overrides
	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);

	virtual void DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);
		// Special handler called by GalleryColourDragInfo when a drag turns out to be a click


	virtual void GetNameText(String_256 *Result);
	virtual void GetFullInfoText(String_256 *Result);
		// Read the name and full-information text for this item

	virtual BOOL GetBubbleHelp(DocCoord *MousePos, String_256 *Result);
		// Returns a bubble help string (or returns FALSE if it couldn't supply one)
	
	virtual BOOL GetStatusLineHelp(DocCoord *MousePos, String_256 *Result);
		// Returns a status-line help string (or returns FALSE if it couldn't)

	virtual void MoveAfter(SGDisplayNode *NodeToMove);
	virtual void MoveBefore(SGDisplayNode *NodeToMove);
		// Move items around in the tree, and also move the original in the colour list

	virtual INT32 CompareTo(SGDisplayNode *Other, INT32 SortKey);
		// Compare two colours using the given sort key. Returns -ve, 0, or +ve


public:		// Specialisations
	virtual BOOL IsALibraryColour(void) const;
			// Determine if this is really an SGDisplayColour (IndexedColour) or derived
			// SGDisplayLibColour (library DocColour) object

	virtual BOOL IsASpotColour(void);
			// Determine if the colour in this item is a Spot colour


protected:	// Internal handlers
	virtual void CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);
			// Determines item size for the current DisplayMode and calculates FormatRect

	virtual void HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo);
			// Redraws the item into the current FormatRect


public:		// Specialisations of this class
	inline DocColour *GetDisplayedColour(void);


protected:	// Special member variables
	DocColour TheColour;
};



/********************************************************************************************

>	inline DocColour *SGDisplayColour::GetDisplayedColour(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Returns:	A pointer to the colour which this Display Item is used to display.
			
	Purpose:	To find out the colour this object is responsible for displaying

********************************************************************************************/

DocColour *SGDisplayColour::GetDisplayedColour(void)
{
	return(&TheColour);
}





/***********************************************************************************************

>	class SGDisplayLibColour : public SGDisplayColour

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Purpose:	This DisplayTree node is used by the Colour SuperGallery
				It is responsible for providing the ability to handle/redraw one library
				colour displayed in said gallery.

	SeeAlso:	ColourSGallery; SuperGallery; SGDisplayColour; SGDisplayItem

***********************************************************************************************/

typedef struct
{
	UINT32 Prefix			: 6;
	UINT32 NewlineAfter	: 1;
	UINT32 SpotColour		: 1;
} LibColourFlags;

class CCAPI SGDisplayLibColour : public SGDisplayColour
{
	CC_DECLARE_DYNAMIC(SGDisplayLibColour);

public:
	SGDisplayLibColour();
	SGDisplayLibColour(DocColour *ColourToDisplay, StringBase *Name,
						PalettePrefix Prefix = PalettePrefix_None,
						BOOL NewlineAfter = FALSE, BOOL IsSpotColour = FALSE);

public:
	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);

	virtual void GetNameText(String_256 *Result);
		// Read the name text for this item

	virtual INT32 CompareTo(SGDisplayNode *Other, INT32 SortKey);
		// Compare two colours using the given sort key. Returns -ve, 0, or +ve


public:		// Specialisations
	virtual BOOL IsALibraryColour(void) const;
			// Determine if this is really an SGDisplayColour (IndexedColour) or derived
			// SGDisplayLibColour (library DocColour) object

	virtual BOOL IsASpotColour(void);
			// Determine if the colour in this item is a Spot colour

private:
	StringBase ColourName;		// NOTE: This must be initialised with Alloc() before use
	LibColourFlags Flags;		// State flags
};


/***********************************************************************************************

>	class SGDisplayColourGroup : public SGDisplayGroup

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/97
	Purpose:	A derived SGDisplayColourGroup class for the Colour gallery groups.
	SeeAlso:	SuperGallery; SGDisplayGroup; SGDisplayRoot; SGDisplayItem

***********************************************************************************************/

class CCAPI SGDisplayColourGroup : public SGDisplayGroup
{
	CC_DECLARE_DYNAMIC(SGDisplayColourGroup);

public:
	SGDisplayColourGroup(SuperGallery *ParentGal,
					Document *ParentDoc = NULL, Library *ParentLib = NULL);

public:
	virtual BOOL IsLibrary() { return FALSE; }

	virtual BOOL DisplayInColourLine();
	virtual BOOL SetDisplayInColourLine(BOOL NewState);
	virtual BOOL ToggleDisplayInColourLine();
};

/***********************************************************************************************

>	class SGDisplayLibColGroup : public SGDisplayColourGroup

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Purpose:	A derived SGDisplayLibColGroup class for Colour gallery libraries.

	SeeAlso:	SuperGallery; SGDisplayGroup; SGDisplayRoot; SGDisplayItem

***********************************************************************************************/

class CCAPI SGDisplayLibColGroup : public SGDisplayColourGroup
{
	CC_DECLARE_DYNAMIC(SGDisplayLibColGroup);

public:
	SGDisplayLibColGroup(SuperGallery *ParentGal, PathName *LibraryFile);


public:	// Functions for virtualisation of groups (building groups but not adding items until we need them)
	virtual BOOL CanVirtualise(void);
		// Can this group virtualise out to disk? (yes)

	virtual BOOL Virtualise(void);
		// Virtualise out (throw away the group contents)

	virtual BOOL DeVirtualise(void);
		// Virtualise back in (pull contents back in from disk)

	virtual BOOL IsLibrary() { return TRUE; }

	virtual BOOL DisplayInColourLine() { return m_DisplayInColourLine; } 
	virtual BOOL SetDisplayInColourLine(BOOL NewState);
	virtual BOOL ToggleDisplayInColourLine();

	// count the number of children that this group has
	INT32 CountChildren();
	
	// find the nth child of this group and return the associated DocColour
	DocColour *GetItemColour(UINT32 Index, SGDisplayLibColour **ppLibColour = NULL);

protected:		// Internal handlers
	virtual void ReadGroupTitle(void);
		// (Re)Reads a group's title text from the parent doc/lib

protected:
	PathName Filename;

	BOOL m_DisplayInColourLine;
};





/***********************************************************************************************

>	class ColourSGallery : public SuperGallery

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Purpose:	The Colour SuperGallery class

	Notes:		SuperGalleries are specced in a number of docs including
					specs\gallib.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)

	SeeAlso:	SuperGallery; SGDisplayColour

***********************************************************************************************/

class ColourSGallery : public SuperGallery
{
friend class SGDisplayColour;		// Allow DisplayColours access to the ISentTheMessage lock

	CC_DECLARE_DYNCREATE(ColourSGallery)

public:
	ColourSGallery();
	~ColourSGallery();

	virtual MsgResult Message(Msg* Message);

	static BOOL Init(void);

	// Returns a pointer to the line gallery, or NULL if there isn't one.
	static ColourSGallery* GetInstance();

protected:				// Overridden upcall methods
	virtual BOOL PreCreate(void);

	virtual BOOL ApplyAction(SGActionType Action);

	virtual void HandleDragStart(DragMessage *DragMsg);

	virtual void SelectionHasChanged(void);

	virtual void DoShadeGallery(BOOL ShadeIt);
		// Called by SuperGallery::ShadeGallery to allow the derived gallery to add code
		// to shade/unshade any extra controls it may have in the window

public:					// Overridden upcall methods
	virtual SGDisplayItem *CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup,
								SGDisplayItem *TargetPosition = NULL);
		// Asks the gallery to copy (or if appropriate, move) the given item to
		// the given group. This hould be done by copying the given item, and
		// then calling InsertCopiedItem() to insert it appropriately (see above)

	virtual void AllItemsCopied(SGDisplayGroup *DestGroup);
		// Informs the gallery when a sequence of MOVE or COPY actions have been applied
		// to the given gallery (see SGDisplayNode::MoveBefore; SGallery::CopyDisplayItem)
		// (this allows you to cause a message to be broadcast which sums up all of the
		// changes in one go, which is a good thing if a selection of thousands of objects
		// has just been moved up one position in the list!)


protected:		// RenderRegion replacement - override to provide anything other than OSRndRgn
	virtual RenderRegion *CreateRenderRegion(DocRect *VirtualSize, ReDrawInfoType *DlgRedrawInfo);
	virtual void DestroyRenderRegion(RenderRegion *pRender);
		// Overridden to make us use a colour-corrected/separated render region.


public:			// Overridden Command interface (for Ops and menu support)
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


public:			// Colour library support
	void SetCurrentImportGroup(SGDisplayLibColGroup *TheGroup)	{ CurrentImportGroup = TheGroup; };
	SGDisplayLibColGroup *GetCurrentImportGroup(void)			{ return(CurrentImportGroup); };
		// Sets/Gets the current group into which we're importing (see DeVirtualise())


	static BOOL CanAddColourToGallery(IndexedColour *pCol,ColourList* pColList);
			// Helper function for CreateNewSubtree()
			// WEBSTER - markn 5/12/96

protected:		// Personal/Internal methods
	void CreateNewSubtree(Document *ParentDoc, SGDisplayColourGroup *ExistingGroup = NULL);
			// Creates a new subtree group to display the colours for the given document
			// If ExistingGroup != NULL, that group will be re-cached for the given doc

	void RedrawColourDisplay(Document *TheDocument, IndexedColour *TheColour);
			// Redraws the area of the window showing the one specified colour (no reformat)

	void EditColour(ColourList *ParentList, IndexedColour *TheColour);
			// Opens the colour editor on the given colour (splat! ;-)

	BOOL OKToDeleteSelection(SGDisplayGroup *DocumentGroup);
			// Checks if it is OK to delete the selection; May return TRUE, but deselect
			// some items selectively to change the set to be deleted

	BOOL DeleteSelection(SGDisplayGroup *DocumentGroup, ColourList *ParentList);
			// Forcibly deletes all the selected colours in the given document group

	void SetSelectionFromDocument(BOOL AlwaysScroll = FALSE);
			// Selects the current line/fill colours from the document

protected:
	BOOL ISentTheMessage;				// Lock to stop us responding to messages we sent

	ColourListComponent *CurrentColComp;// NULL, or points to the colour component in use
	SGDisplayNode *CurrentTarget;		// NULL (add-tail), or the item to insert before
										// when ending the component copy

public:		// preferences
	static INT32 AutoScrollSelection;		// TRUE to allow auto-scrolling when new colours are
										// selected in the gallery (which can be annoying
										// if you were scrolled where you wanted to be)

	static INT32 DefaultDisplayMode;		// Memory of current display mode between sessions

	static String_256 PalettePath;		// The path to load the palettes from 

	static BOOL	ShowDocumentColours;	// True if want document colours in colour line

	static BOOL ShowNetscapeColours;	// True if want Netscape colours being shown in colour line
										// Webster only at present

private:
	SGDisplayLibColGroup *CurrentImportGroup;	// Group currently being imported into, or NULL

public:
	// This will create the gallery groups (unless they're already there) and return TRUE if ok.
	BOOL MakeSureGroupsHaveBeenCreated();
	
	// Walk through the list of library colour galleries
	SGDisplayLibColGroup * GetFirstLibGroup();
	SGDisplayLibColGroup * GetNextLibGroup(SGDisplayLibColGroup * pLibGroup);

	BOOL ApplySelectedAsBackground();
	BOOL GetDocColourToApply(SGDisplayColour *pFirstSelected, DocColour * pColourToApply,
							 Document **pDoc);
protected:
	static ColourSGallery* m_pTheGallery;
};





/********************************************************************************************

>	class OpDisplayColourGallery: public Operation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (Based on the Colour SGallery code)

	Purpose:	Displays the Colour gallery.
				This op can be attached to a menu, button or keypress

	SeeAlso:	ColourSGallery

********************************************************************************************/

#define OPTOKEN_DISPLAYCOLOURGALLERY _T("DisplayColourGallery")

class CCAPI OpDisplayColourGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayColourGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);

	static DialogBarOp *FindGallery(void);
			// Finds the Colour gallery class instance
};  








#define OPTOKEN_COLOURNAMEDLG _T("ColourNameDlg")

/********************************************************************************************

>	class ColourNameDlg: public DialogOp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Purpose:	Allows the user to edit the name of a colour

	Notes:		Call InvokeDialog to show the dialogue. Two overloaded methods are
				provided, which take a string to set, or a colourlist+colour respectively.

********************************************************************************************/

class ColourNameParam : public OpParam		// Internal parameter-passing class
{
CC_DECLARE_MEMDUMP(ColourNameParam)

public:
	ColourList		*TheList;
	IndexedColour	*TheColour;
	String_64		*TheName;
	BOOL			Result;

public:
	ColourNameParam()	\
				{ TheList = NULL; TheColour = NULL; TheName = NULL; Result = FALSE; }

	ColourNameParam(ColourList *ParentList, IndexedColour *ColourToEdit)		\
				{ TheList = ParentList; TheColour = ColourToEdit; TheName = NULL; Result = FALSE; }

	ColourNameParam(String_64 *NameToFill, IndexedColour *DisplayColour = NULL)	\
				{ TheList = NULL; TheColour = DisplayColour; TheName = NameToFill; Result = FALSE; }
};



class ColourNameDlg : public DialogOp
{         
CC_DECLARE_DYNCREATE(ColourNameDlg)

public:		// External inteface
	static BOOL InvokeDialog(ColourList *Parentlist, IndexedColour *ColourToName);
	static BOOL InvokeDialog(String_64 *NameToFill, IndexedColour *DisplayColour = NULL);


public:		// DialogOp interface
	ColourNameDlg();

	void DoWithParam(OpDescriptor*, OpParam*);
	BOOL Create(); 

	static const INT32 IDD;
	static const CDlgMode Mode;

	static OpState GetState(String_256*, OpDescriptor*);	
	static BOOL Init();                        

	virtual MsgResult Message(Msg* Message);

protected:
	void CommitName(void);		// Reads from the dialogue to commit the name, and End()s

protected:	// Data members
	ColourNameParam *Info;
}; 


#endif

