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
// Header for the Text tool implementation

#ifndef INC_TEXTTOOL
#define INC_TEXTTOOL

#include "tool.h"
#include "transop.h"

class Cursor;
class TextStory;
class CaretNode;
class KeyPress;

#if !defined(EXCLUDE_FROM_RALPH)
class TextInfoBarOp;
#endif

/********************************************************************************************
>	class TextToolBlobPosItem : public ListItem

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Purpose:	The class of objects stored in the TextToolBlobPosList.  These items store
				the blobs position and associated story.
********************************************************************************************/
class TextToolBlobPosItem : public ListItem
{
public:
	CC_DECLARE_DYNAMIC(TextToolBlobPosItem);
	TextToolBlobPosItem(DocCoord Left, DocCoord Right, BOOL Connected, TextStory* pStory);
	~TextToolBlobPosItem();

public:
	// Access functions
	DocCoord GetLeftBlobPos() const		{return m_LeftBlobPos;}
	DocCoord GetRightBlobPos() const	{return m_RightBlobPos;}
	BOOL IsConnected() const			{return m_BlobsConnected;}
	TextStory* GetBlobStory() const		{return m_pTextStory;}

protected:
	DocCoord m_LeftBlobPos;
	DocCoord m_RightBlobPos;
	BOOL m_BlobsConnected;
	TextStory* m_pTextStory;
};


/********************************************************************************************
>	class TextToolBlobPosList : public List

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Purpose:	A list the positions of blobs that the text tool is rendering as tool blobs.
				This class is just a wrapper arouind the list class, providing common handling
				functions.
********************************************************************************************/
class TextToolBlobPosList : public List
{
public:
	CC_DECLARE_DYNAMIC(TextToolBlobPosList);
	TextToolBlobPosList();
	~TextToolBlobPosList();

	BOOL BuildFromSelection();
	BOOL RenderAllBlobs(Spread* pSpread = NULL, DocRect* pClipRect = NULL);
	static void RenderBlobPair(Spread* pSpread, DocRect* pClipRect, TextToolBlobPosItem* pPair);
	static void RenderBlob(Spread* pSpread, DocRect* pClipRect, DocCoord BlobPos);
	BOOL FindBlobStory(Spread* pSpread, DocCoord PointerPos, BOOL* pLeft, TextStory** pStory);
	BOOL RenderAndRemove(TextStory* pStory); 
	void RenderAndRemoveAll(); 

	Spread* GetBlobSpread() const {return m_pSelSpread;}

protected:
	Spread* m_pSelSpread;
};





/********************************************************************************************

>	class TextTool : public Tool_v1

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	A template tool class

********************************************************************************************/

class CCAPI TextTool : public Tool_v1
{
CC_DECLARE_MEMDUMP(TextTool);

public:
	TextTool();
	~TextTool();

	// Standard calls your tool must provide
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_TEXT; };

	// This returns TRUE if the text-tool is delaying it's updates until it is idle.  It
	// is used by the OLE update routines to avoid luching redraws ...
	static BOOL DelayingUpdate();
	
	// Some very common event handlers
	void SelectChange(BOOL isSelected);
	void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	void OnMouseMove(DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMods);
	void RenderToolBlobs(Spread *, DocRect*);
	BOOL OnKeyPress(KeyPress* pKeyPress);
	BOOL GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);
	BOOL OnIdle();

	// Some useful static functions
#if !defined(EXCLUDE_FROM_RALPH)
	static TextInfoBarOp* 	GetTextInfoBarOp()  { return pTextInfoBarOp; }
#endif
	static BOOL 			IsCurrentTool() 	 { return CurrentTool; }

	void LocaleChanged();
	BOOL SelectionHasChanged();

	virtual BOOL AreToolBlobsRenderedOnSelection() {return TRUE;}

protected:
	// Status line help functions
	void DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods);
	void GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);

 	// Mouse click and movement functions
 	BOOL IsPointNearUnselectedTextStory(Spread*, DocCoord, TextStory**, Node**, BOOL);
	BOOL IsPointNearNonStoryPath(Spread*, DocCoord, NodePath**, BOOL);
 	BOOL HandleSingleClick(Spread*, DocCoord, ClickModifiers);
	BOOL HandleDoubleClick(Spread*, DocCoord, ClickModifiers);
	BOOL HandleTripleClick(Spread*, DocCoord, ClickModifiers);
	BOOL HandleQuadClick(Spread*, DocCoord, ClickModifiers);
	BOOL HandleDragClick(Spread*, DocCoord, ClickModifiers);

	// Keypress processing
	BOOL RegisterDeadKeys();
	BOOL RemoveDeadKeys();
	BOOL RegisterAdditionalVirtKey(KeyPress** pKey, TCHAR ch);
	BOOL HandleSpecialNonStoryKeys(KeyPress* pKeyPress);
	BOOL HandleSpecialStoryAndNonStoryKeys(KeyPress* pKeyPress);
	BOOL HandleDeadKeys(KeyPress* pKeyPress, WCHAR* pNewUnicode);
	BOOL TestForDeadKey(KeyPress* pKeyPress, KeyPress* pDeadKeypress);
	BOOL HandleSpecialStoryKeys(KeyPress* pKeyPress, TextStory* pStory, CaretNode* pCaret);
	BOOL IsUsableUnicode(WCHAR CodeValue);
	BOOL ApplyBold();
	BOOL ApplyItalic();
	BOOL IncreaseTrackKern();
	BOOL DecreaseTrackKern();
	BOOL CommonApplyKern(MILLIPOINT ChangeAmmount);
	BOOL CommonApplyTracking(MILLIPOINT ChangeAmmount);

	BOOL OnToolSelect();
	BOOL OnToolDeselect();

	BOOL CreateCursors();						// Create  your tool's cursors in this func
	void DestroyCursors();						// Destroy your tool's cursors in this func

	static BOOL 			CurrentTool;		// Can be useful to keep a "is my tool currenmt?" flag
#if !defined(EXCLUDE_FROM_RALPH)
	static TextInfoBarOp*	pTextInfoBarOp;		// Ptr to your tool's infobar
#endif

	Cursor*	pcNormalTextCursor;
	Cursor*	pcBlobTextCursor;
	Cursor*	pcBlankCursor;
	Cursor*	pcIndentCursor;
	Cursor*	pcCurrentCursor;

	INT32 CurrentCursorID; 						// A unique cursor id for Jim's new cursor stacking
	BOOL IsBlankCursorUp;

	// Standard tool static vars
	static 	TCHAR* FamilyName;					// The Tools Family Name
	static 	TCHAR* ToolName;						// The Tool Name
	static 	TCHAR* Purpose;						// What the tool is for
	static 	TCHAR* Author;						// Who wrote it

	BOOL UpdateAfterTyping;

	TextStory* pLastFocusStory;

	// Ini file preferences
	static BOOL UseDeadKeys;

	// Used for dead key accent entry
	KeyPress* PreviousDeadKey;
	KeyPress* GraveVirtKey;
	KeyPress* AcuteVirtKey;
	KeyPress* HatVirtKey;
	KeyPress* TildeVirtKey;
	KeyPress* ColonVirtKey;
	KeyPress* AtVirtKey;
	KeyPress* CommaVirtKey;
	KeyPress* SlashVirtKey;

	// List of current blobs
	TextToolBlobPosList m_BlobPosList;
};


#endif			// INC_TEXTTOOL
