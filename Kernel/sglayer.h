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
// SGLayer.h - the LayerSGallery (Layer SuperGallery) class


#ifndef INC_SGLAYER
#define INC_SGLAYER


//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "sgtree.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "layergal.h"

class DocView;
class Layer;

// WEBSTER - markn 15/1/97
// Don't need this gallery in Webster
// Only need the bits left by ralph
//#ifdef WEBSTER
//	#ifndef EXCLUDE_GALS
//		#define UNDEF_EXCLUDE_GALS	// Remember to undefine EXCLUDE_GALS at the end
//		#define EXCLUDE_GALS
//	#endif
//#endif

#ifndef EXCLUDE_GALS
/***********************************************************************************************

>	class SGDisplayLayer : public SGDisplayItem

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/01/95
	Purpose:	This DisplayTree node is used by the Layer SuperGallery
				It is responsible for providing the ability to handle/redraw one Layer
				displayed in said gallery.

	SeeAlso:	LayerSGallery; SuperGallery; SGDisplayItem

***********************************************************************************************/

class CCAPI SGDisplayLayer : public SGDisplayItem
{
friend class SGLayerGroup;		// Allows access to CalculateButtonRects

	CC_DECLARE_DYNAMIC(SGDisplayLayer);

public:
	SGDisplayLayer();
	SGDisplayLayer(Layer *LayerToDisplay);
	~SGDisplayLayer();

	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);

	// Functions that handle dragging of layers
	virtual void MoveBefore(SGDisplayNode *NodeToMove);
	virtual void MoveAfter(SGDisplayNode *NodeToMove);
	virtual void MoveLayer(SGDisplayNode *NodeToMove,BOOL Before,BOOL ToggleBackground);

protected:	// Internal handlers
	virtual void CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);
			// Determines item size for the current DisplayMode and calculates FormatRect

	static void CalculateButtonRects(SGMiscInfo *MiscInfo, DocRect *MyRect,
									 DocRect *VisibleBtn, DocRect *LockedBtn, DocRect *SnapButton = NULL);
			// Chops off two (three) button rects from the given 'myrect'

	virtual void HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo);
			// Redraws the item into the current FormatRect

	virtual void DoChangeState(OpLayerGalReason Reason,BOOL NewState);
			// Changes the state of the layer this object displays, in some way

	virtual void ChangeRangeState(SGDisplayLayer *Start, SGDisplayLayer *End,
								  BOOL Range, INT32 WhichButton, SGMiscInfo *MiscInfo);
			// Changes the state (visible or locked) of a range of layers


public:		// Specialisations of this class
	inline Layer *GetDisplayedLayer(void);


protected:		// Special member variables (Used to be private but needed by SGDisplayFrame)
	Layer *TheLayer;

private:		// Special member variables
	static SGDisplayLayer *LastClickedLayer;		// Remembers the last layer clicked on (for shift-clicks)
};



/***********************************************************************************************

>	class SGLayerGroup : public SGDisplayGroup

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Purpose:	This is a display tree group specifically for the layer gallery - it disables
				group things like folding, and provides column headings for the layers display.
	SeeAlso:	SuperGallery; SGDisplayGroup;

***********************************************************************************************/


class CCAPI SGLayerGroup : public SGDisplayGroup
{
	CC_DECLARE_DYNAMIC(SGLayerGroup);

public:
	SGLayerGroup(SuperGallery *ParentGal,
					Document *ParentDoc = NULL, Library *ParentLib = NULL, BOOL IsForeground = TRUE);

	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);
		// Handles a display list event (redraw, mouse click, etc)

	// Is this group the foreground layer group (TRUE) or the background layer group (FALSE)
	BOOL IsForeground() { return Foreground; }

private:
	BOOL Foreground;
};




/********************************************************************************************

>	inline Layer *SGDisplayLayer::GetDisplayedLayer(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/1/95

	Returns:	A pointer to the Layer which this Display Item is used to display.
			
	Purpose:	To find out the Layer this object is responsible for displaying

********************************************************************************************/

Layer *SGDisplayLayer::GetDisplayedLayer(void)
{
	return(TheLayer);
}



#endif


/***********************************************************************************************

>	class LayerSGallery : public SuperGallery

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/1/95 (Based on the Colour SGallery code)
	Purpose:	The Layer SuperGallery class

	Notes:		SuperGalleries are specced in a number of docs including
					specs\sgallery.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)
				There is also howtouse\sgallery.doc which describes creating a supergallery

	SeeAlso:	SuperGallery; SGDisplayLayer

***********************************************************************************************/

#ifdef EXCLUDE_GALS
class LayerSGallery : public Operation
#else
class LayerSGallery : public SuperGallery
#endif
{
	CC_DECLARE_DYNCREATE(LayerSGallery)

public:
	LayerSGallery();
	~LayerSGallery();

#ifndef EXCLUDE_GALS
	virtual MsgResult Message(Msg* Message);


protected:				// Overridden upcall methods
	virtual BOOL PreCreate(void);
	virtual BOOL PostCreate(void);

	virtual BOOL ApplyAction(SGActionType Action);

	virtual void DoShadeGallery(BOOL ShadeIt);

	// handles the copying of items between groups via item drags
	virtual SGDisplayItem *CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup,
								SGDisplayItem *TargetPosition = NULL);


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


public:		   		// Personal/Internal methods
	virtual void CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup = NULL);
			// Creates a new subtree group to display the Layers for the given document
			// If ExistingGroup != NULL, that group will be re-cached for the given doc


public:
	inline Spread *GetSelectedSpread(void);
			// Returns the spread which the layer gallery is currently using


protected:
	Spread	 		*pSpread;			// The Current spread
	Document 		*pDoc;				// The Current document
	SGDisplayGroup  *DisplayForeground;	// Group of foreground layers
	SGDisplayGroup  *DisplayBackground;	// Group of background layers
	BOOL			m_NewDocBorn;		// Flag to determine when a new doc. is born.		

protected:		// Internal methods (directly ported from old LayerGallery class)
	virtual void NewSelectedSpread(Spread* pNewSelSpread = NULL);
	virtual void EnsureSelSpreadSelectionIntegrity();
#endif
public:			// Public so SGDisplayLayer items can call them
	static void ForceRedrawLayer(Document* pDoc,Layer* pLayer);
	static void MakeActiveLayer(Layer* pNewActiveLayer, BOOL TellAll = TRUE);
	static void EnsureActiveLayerIntegrity(Spread* pSpread,Layer** ppActiveLayer = NULL);

public:			// Public access functions for everybody who wants 'em
	static void SetActive(Document* pDoc,Layer* pLayer,BOOL state);
	static void MakeTopLayerActive(Spread* pSpread);
	static String_256 CreateUniqueLayerID(Spread* pSpread);

#ifndef EXCLUDE_GALS
protected:
	virtual void EnsureActiveLayerIntegrity();
	virtual BOOL CheckVarsAreValid();

	virtual SGDisplayLayer *GetSelectedLayerGalItem();

	virtual BOOL HasLayerGotChildren(Layer* pLayer);
 
	virtual BOOL PrepareToDelete(void);
	virtual BOOL DoDeleteSelection(void);

	virtual void DoCreateNewItem(void);
	virtual void DoChangeName();
	virtual void DoCopyLayer();
	virtual void DoLayerProperties();
	virtual void DoCreateGuideLayer();
	virtual void DoChangeLayerStatus(Layer* pLayer, const LayerStatus& NewStatus);

	virtual void DoMoveLayer(MoveLayerType Reason);

protected: // Used to be private but needed by FrameSGallery
	virtual INT32 GetGuideLayerState(DocView* pDocView);
	INT32 OldGuideLayerState;
	INT32 NewGuideLayerState;
	virtual BOOL IsSelectedItemGuideLayer();

public:
	static void DoChangeVisible(Layer* pLayer,BOOL Visible);
	static void DoChangeLocked(Layer* pLayer,BOOL Locked);
	virtual void DoChangeAllVisible(BOOL AllVisible);
	virtual void DoChangeMultilayer(BOOL Multilayer);

	virtual void DoChangeLayerState(Layer* pLayer,OpLayerGalReason Reason,BOOL NewState);

	static BOOL DoMoveSelectionToActiveLayer();

protected:

	// Function to determine the document mode.
	BOOL IsLayerMode(Document* pDoc);		

	// Function to close the frame gallery
	BOOL CloseLayerGallery();

	// Function to open the Layer gallery
	BOOL OpenFrameGallery();

	// Access functions to determine when a new document is born.
	void SetNewDocBorn(BOOL Val)	{ m_NewDocBorn = Val;  }
	BOOL GetNewDocBorn()			{ return m_NewDocBorn; }
	
#endif
};


#ifndef EXCLUDE_GALS


/********************************************************************************************

>	inline Spread *LayerSGallery::GetSelectedSpread(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/95

	Returns:	A pointer to the spread that the layer gallery regards as 'current'

	Purpose:	To find the current layer gallery spread

********************************************************************************************/

Spread *LayerSGallery::GetSelectedSpread(void)
{
	return(pSpread);
}

#endif

// WEBSTER - markn 15/1/97
// Do we need to undefine EXCLUDE_GALS?
//#ifdef UNDEF_EXCLUDE_GALS
//#undef EXCLUDE_GALS
//#undef UNDEF_EXCLUDE_GALS
//#endif

#endif //INC_SGLAYER

