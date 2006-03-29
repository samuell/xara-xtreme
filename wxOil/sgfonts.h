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
// SGFonts.h - the FontsSGallery (Fonts SuperGallery) class

#ifndef INC_SGFONTS
#define INC_SGFONTS

#ifndef EXCLUDE_GALS

#include "sgallery.h"
#include "sgscan.h"
#include "sglib.h"
#include "dragbmp.h"
#include "sgdrag.h"
#include "fontbase.h"

// LibraryGallery
#include "sglbase.h"

class Document;
class SuperGallery;
class String_64;
class String_256;
class KernelBitmap;
class SGDisplayNode;
class DocCoord;
class Msg;
class SGDisplayGroup;
class PathName;
class NodeRenderableInk;
class List;
class OpDescriptor;		

/***********************************************************************************************

>	class SGFontsGroup : public SGDisplayGroup

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95
	Purpose:	This is a display tree group specifically for the fonts gallery - it disables
				various documenty things, and provides overrides for the group text
	SeeAlso:	SuperGallery; SGDisplayGroup;

***********************************************************************************************/

class CCAPI SGFontsGroup : public SGDisplayGroup
{
	CC_DECLARE_DYNAMIC(SGFontsGroup);

public:
	SGFontsGroup(SuperGallery *ParentGal,
					Document *ParentDoc = NULL, Library *ParentLib = NULL,
					String_64 *TText = NULL);

	virtual void ReadGroupTitle(void);

	virtual BOOL HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo);

	virtual BOOL GetBubbleHelp(DocCoord *MousePos, String_256 *Result);

	virtual BOOL GetStatusLineHelp(DocCoord *MousePos, String_256 *Result);

	virtual BOOL CanVirtualise(void);
};
					 
/***********************************************************************************************

>	class SGDisplayPreviewFonts : public SGDisplayItem

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/95
	Purpose:	This is the base-class for the installed font items in the fonts gallery.
				An instance of this class should never be created, instead SGTTFItem and SGATMItem's
				should be used.

	SeeAlso:	FontsSGallery; SuperGallery; SGDisplayItem

***********************************************************************************************/

class CCAPI SGDisplayPreviewFonts : public SGDisplayItem
{
	CC_DECLARE_DYNAMIC(SGDisplayPreviewFonts);

public:
	SGDisplayPreviewFonts();
	~SGDisplayPreviewFonts();

 	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);

	void DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);
	
protected:	// Internal handlers

	// Determines item size for the current DisplayMode and calculates FormatRect
	virtual void CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);

	// Redraws the item into the current FormatRect
	virtual void HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo);

	// Draw a thumbnail for this item
	virtual BOOL DrawThumb(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo,
			SGMiscInfo *MiscInfo, DocRect *Rectangle, BOOL Background = FALSE);

	// Draw the text for this item
	virtual void DrawItemText(RenderRegion *Renderer, SGRedrawInfo *RedrawInfo,
			SGMiscInfo *MiscInfo, DocRect *Rectangle, DocRect *BmpRect, BOOL Selected);

	void MoveAfter(SGDisplayNode *NodeToMove);

	void MoveBefore(SGDisplayNode *NodeToMove);

	BOOL GetBubbleHelp(DocCoord *MousePos, String_256 *Result);

	BOOL GetStatusLineHelp(DocCoord *MousePos, String_256 *Result);

public:		// Specialisations of this class

	// Return the display type for the mode
	static LibDisplayType GetDisplayType(SGMiscInfo *MiscInfo);

	virtual void GetNameText(String_256 *Result);
	virtual void GetFullInfoText(String_256 *Result);
	virtual INT32 CompareTo(SGDisplayNode *Other, INT32 SortKey);

	virtual BOOL CreateThumbnail(KernelBitmap **Bitmap);

	KernelBitmap *GetDisplayedKernelBitmap(SGMiscInfo *MiscInfo, BOOL Background);

	inline String_64 *GetDisplayedTextDescription(void);

	// Get the thumbnail size details for a specific mode
	static BOOL GetThumbnailDetails(INT32 Mode, UINT32 *X, UINT32 *Y, String_256 *Text);

	// generic searching/sorting function
	virtual void GetKeyWords(String_256 *Result);

	// Draw the specified 'type icon' for the item
	static void DrawTypeIcon(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo, DocRect *TypeRect, FontClass Type);

	// Is this item being used by any of the loaded documents ?
	BOOL IsFontBeingUsed(void);

public:

	// Pointer to bitmap for this display item (these are stored in the thumb cache)
	KernelBitmap *FontBitmap;
	String_64 FontDescription;

 	// Needed to reference the thumbnails in the cache (a bodge in other words)
	// Each time we recreate the list of installed fonts for the gallery we 
	// assign new IDs to the items.
	UINT32 ID;

	// The current display mode
	static INT32 DMode;

	// enum of which type of font item we are - saves doing iskindof's everywhere
	FontClass Type;

	// The cached logfont
	LOGFONT *CachedLogFont;
	INT32 IntLeading;

	// If set, draw a null bounding rect for this item, and don't use the bitmap
	BOOL Invalid;
};

/***********************************************************************************************

>	class SGTTFItem : public SGDisplayPreviewFonts

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/95
	Purpose:	A DisplayTree node for TrueType fonts in the Fonts SuperGallery

	SeeAlso:	FontsSGallery; SuperGallery; SGDisplayItem

***********************************************************************************************/

class SGTTFItem : public SGDisplayPreviewFonts
{
	CC_DECLARE_DYNAMIC(SGTTFItem);

public:
	SGTTFItem();
	~SGTTFItem();

	SGTTFItem(KernelBitmap *PreviewFontsToDisplay, String_64 *Name,
		INT32 IL = 0, PLOGFONT lpelf = NULL, UINT32 TheID = 0);
							
	virtual void GetFullInfoText(String_256 *Result);
	virtual BOOL CreateThumbnail(KernelBitmap **Bitmap);
 };

/***********************************************************************************************

>	class SGATMItem : public SGDisplayPreviewFonts

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/95
	Purpose:	A DisplayTree node for ATM fonts in the Fonts SuperGallery

	SeeAlso:	FontsSGallery; SuperGallery; SGDisplayItem

***********************************************************************************************/

class SGATMItem : public SGDisplayPreviewFonts
{
	CC_DECLARE_DYNAMIC(SGATMItem);

public:
	SGATMItem();
	~SGATMItem();

	SGATMItem(KernelBitmap *PreviewFontsToDisplay, String_64 *Name,
		INT32 IL = 0, PLOGFONT lpelf = NULL, UINT32 TheID = 0);

	virtual void GetFullInfoText(String_256 *Result);
	virtual BOOL CreateThumbnail(KernelBitmap **Bitmap);
};

/********************************************************************************************

>	inline String_64 *SGDisplayPreviewFonts::GetDisplayedTextDescription(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	03/02/95

	Returns:	A pointer to the Description string which this Display Item uses.
			
	Purpose:	To find the Text description for this object

********************************************************************************************/

String_64 *SGDisplayPreviewFonts::GetDisplayedTextDescription(void)
{
	return(&FontDescription);
}

/***********************************************************************************************

>	class FontsSGallery : public LibraryGallery

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/95
	Purpose:	The Fonts SuperGallery class

	Notes:		SuperGalleries are specced in a number of docs including
					specs\sgallery.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)
				There is also howtouse\sgallery.doc which describes creating a supergallery

	SeeAlso:	SuperGallery; SGDisplayFonts

***********************************************************************************************/

class FontsSGallery : public LibraryGallery
{
	CC_DECLARE_DYNCREATE(FontsSGallery)

public:
	FontsSGallery();
	~FontsSGallery();

	// TRUE if the SuperGallery is also a LibraryGallery & has no non-library groups
	virtual BOOL IsLibraryGalleryWithNonLibraryGroups(void) {return FALSE;};

public:

	// SG Message centre
	virtual MsgResult Message(Msg* Message);

	// Install / Deinstall fonts
	BOOL InstallFonts(BOOL Copy);
	BOOL DeinstallFonts(BOOL Delete);		

	// Install / Deinstall font
	BOOL InstallFont(PathName *FontFile, String_256 *Description, BOOL Temp);
	BOOL DeinstallFont(String_256 *FontDesc, BOOL Delete, LOGFONT *LogFnt = NULL);

	// Install / Deinstall TTF font
	BOOL InstallTTFFont(PathName *FontFile, String_256 *Description, BOOL Temp);
	BOOL DeinstallTTFFont(String_256 *FontDesc, BOOL Delete);
 
	static BOOL GetWindowsFontDirectory(String_256 *Result);

	static UINT32 CALLBACK FileHook(HWND hDlg, UINT32 iMsg, UINT32 wParam, INT32 lParam);

	// Return the offset which to start the font drag
	static INT32 GetDragFontOffset(KernelBitmap *Bmp);

public:
	// Library Galleries must override these

	// Can we create indexes ?
	virtual BOOL CanCreateIndexes(void);

	// Get various default bits 'n' pieces
	virtual BOOL GetDefaults(String_256 *DefaultIndex, String_256 *IndexDesc, SGLibType *Type);

	// Get the default CD directory name for the gallery
	virtual BOOL GetLibraryDirectoryName(String_256 *LibDirName);

	// Check the entry in the index.txt file with our gallery, and return TRUE if it's a match
	virtual BOOL CheckForIndexMatch(StringBase *Txt);

	// Return a Section name for this gallery...
	virtual void WorkOutSectionName(String_256 *Section);

	// Get the quiet status of the gallery
	virtual BOOL GetQuietStatus(void);

	// Set the quiet status of the gallery
	virtual void SetQuietStatus(BOOL Status);

	// Scan for location of default CD library
	virtual BOOL ScanForLocation(SGLibType Type, StringBase *Result);

	// Return gallery type
	virtual SGLibType GetGalleryType() { return SGLib_Font;}

	virtual String_256* GetDefaultLibraryPath() {return &DefaultLibraryPath;}

	void SelectionHasChanged(void);

private:

	// Button greying functions
	void DoShadeGallery(BOOL ShadeIt);
	BOOL InstalledFontsSelected(void);
	INT32 LibraryFontsSelected(void);

	// callback routine to expand a family into the individual members
	static INT32 CALLBACK EnumInstalledFamily(ENUMLOGFONT FAR*lpelf, NEWTEXTMETRIC FAR* lpntm, INT32 FontType, LPARAM handle);
	
	// callback routine for windows to call for each installed font present
	static INT32 CALLBACK EnumInstalledFonts(ENUMLOGFONT FAR*lpelf, NEWTEXTMETRIC FAR* lpntm, INT32 FontType, LPARAM handle);

	// Can we support keywords ?
	virtual BOOL CanSearchKeywords(void);

	// Sort the installed group
	void SortInstalledFonts(void);
		
protected:				// Overridden upcall methods

	// After opening the gallery - create the groups...
	virtual BOOL PreCreate(void);

	// Action code - called for various sg messages
	virtual BOOL ApplyAction(SGActionType Action);

	// Adds an item to the given library display group
	virtual SGDisplayItem *AddLibraryItem(SGDisplayGroup *LibraryGroup, Library *ParentLib,
											LibraryIndex ItemIndex, BOOL bNew = FALSE);

	// Gets a FOT filename from the registry / win.ini file, etc...
	BOOL GetFOTNameFromRegistry(String_256 *Desc, PathName *FOTFile);

	// Writes an FOT filename and its association in the registry / ini file, etc...
	BOOL SetFOTNameInRegistry(String_256 *Desc, PathName *FOTFile);

	// Returns the registry key where the font info is stored
	BOOL GetFontsKey(String_256 *Key);

	void HandleDragStart(DragMessage *DragMsg);

	// Override for creating font library groups rather than display groups
	virtual SGDisplayGroup *AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems);

	// Dragging fonts between sections
	SGDisplayItem *CopyDisplayItem(SGDisplayItem *SourceItem, 
							SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition);

	// A group of fonts has finished its drag
	virtual void AllItemsCopied(SGDisplayGroup *DestGroup);

	virtual void ApplySortNow(BOOL ApplyToEntireList);

	static BOOL UseWin95FontInstallMethod();

protected:
	
	// Creates a new subtree for the installed fonts
	void CreateNewSubtreeForInstalledFonts(SGFontsGroup *ExistingGroup = NULL);

	// Optionally kills the old library font groups and adds in new ones it can find for the path
	BOOL CreateNewSubtreeForLibraryFonts(PathName *LibPath, BOOL WipeOld = TRUE);


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

public:
	// Apply the currently selected (or dragged) font from the gallery, optionally installing it, etc...
	static BOOL ApplyFont(BOOL Dropping, SGDisplayNode *TheNode, NodeRenderableInk* pObjectHit = NULL);

	// Install a font via dragging - warn and give option of cancelling
	static BOOL InstallDraggedLibFont(FontsSGallery *TheGallery, SGLibDisplayItem *FontItem, String_64 *Desc);

	// Return true if a font description is found in the installed group
	static BOOL IsFontAlreadyInstalled(String_256 *FontDesc, FontClass Class = FC_UNDEFINED);

	// Return true and the logfont if the described font is found in the installed group
	static BOOL FindLogFont(String_256 *FontDesc, LOGFONT **lplplf, FontClass Class = FC_UNDEFINED);

	// Function to call on font change messages
	static void DoFontChange(void);

	// Change strings ending with ;BOLDITALIC to a displayable version...
	static void MakeBoldItalicReadable(String_256 *Desc);

protected:

	// This could go in a base class
	void SortGallery(void);

	// This is special to the fonts gallery
	BOOL BrowseClicked(void);

public:
	
	// The installed fonts group
	static SGFontsGroup *InsGroup;

protected:

	// ID count for installed fonts - used by thumbnail cache and saving code
	UINT32 IDCount;

	// Flag to say whether second EnumFF added anything
	static BOOL AddedFonts;

	// Sort of font that we're trying to enumerate with EnumFontFamilies
	static FontClass EnumFontClass;

public:
	// Installed fonts thumbnail preview object
 	SGThumbs *InsCache;

	// For use by the callback function
	static FontsSGallery *ThisGallery;		

	// Where to find the default library
	static String_256 DefaultLibraryPath;

	// If true then we're in the middle of installing or deinstalling some fonts
	static BOOL DontUpdate;

	// For keeping the display modes constant when next loaded
	static INT32 DefaultDisplayMode;

	// For keeping the sort keys constant when next loaded
	static UINT32 DefaultSortKeys;

	// Deinstalling fonts option
	static BOOL DeleteTTFandFOTfiles;

	// Use this as the large display mode string (for the installed fonts)
	static String_256 LargeDisplayString;

	// Quiet status of the gallery
	static BOOL QuietStatus;

	// Multiple drag warning status
	static BOOL WarnAboutSectionDragging;

	// Multiple ATM drag warning status
	static BOOL WarnAboutMultipleATMDragging;

};

/******************************************************************************/
				   
// Optoken for the display bitmap gallery operation
#define OPTOKEN_DISPLAYFONTSGALLERY _T("DisplayFontsGallery")	

/********************************************************************************************

>	class OpDisplayFontsGallery: public Operation

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/95
	Purpose:	Displays the fonts gallery. This op can be attached to a menu, button or keypress
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpDisplayFontsGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayFontsGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);
};  

/********************************************************************************************

>	class SGLibFontItem: public SGLibDisplayItem

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95

	Purpose:	A library font gallery item
	SeeAlso:	SGClipartItem
	
********************************************************************************************/

class SGLibFontItem: public SGLibDisplayItem
{         
	CC_DECLARE_DYNAMIC( SGLibFontItem );
	
public:

	SGLibFontItem();
	~SGLibFontItem();

	SGLibFontItem(LibraryIndex LibraryIndexToDisplay, BOOL bNew = FALSE);

	LibDisplayType GetDisplayType(SGMiscInfo *MiscInfo);

	INT32 GetTextWidth(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);

	void GetNameText(String_256 *Result);
	void GetFullInfoText(String_256 *Result);

	UINT32 GetFontID(void);

	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);

	virtual BOOL GetNameTextPtr(TCHAR **Result);

	// Determines item size for the current DisplayMode and calculates FormatRect
	virtual void CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);

	// Redraws the item into the current FormatRect
	virtual void HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo);

	// Returns the 'Type' for this library item
	FontClass GetType(void);

	// Override default library sorting routine to add a font-type comparator
	virtual INT32 CompareTo(SGDisplayNode *Other, INT32 SortKey);

	virtual BOOL GetThumbFileName(String_256* path);


protected:

	void MoveAfter(SGDisplayNode *NodeToMove);

	void MoveBefore(SGDisplayNode *NodeToMove);

	BOOL GetBubbleHelp(DocCoord *MousePos, String_256 *Result);

	BOOL GetStatusLineHelp(DocCoord *MousePos, String_256 *Result);
};

#endif
#endif

