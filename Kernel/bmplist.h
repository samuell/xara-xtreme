// $Id: bmplist.h 662 2006-03-14 21:31:49Z alex $
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

// kernel class to encompass bitmaps lists

#ifndef	INC_BMPLIST
#define	INC_BMPLIST

#include "list.h"
#include "msg.h"
#include "ops.h"

class OILBitmap;
class BitmapList;
class Document;
class KernelBitmap;
class KernelBitmapRef;
class Node;
class Operation;
class Action;

/***********************************************************************************************

>	class GreyscaleBitmapList : public List

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Purpose:	A list of Greyscale bitmaps
	SeeAlso:	-

***********************************************************************************************/

/*class GreyscaleBitmapList : public List
{
	CC_DECLARE_MEMDUMP(GreyscaleBitmapList)

public:
	GreyscaleBitmapList();
	~GreyscaleBitmapList();

	static BOOL Init();
	static BOOL DeInit();

	BOOL AddItem(OILBitmap *NewBitmap);

	virtual void AddHead( ListItem* );	
	virtual void AddTail( ListItem* );

	virtual ListItem* RemoveHead();							
	virtual ListItem* RemoveTail();	
	
	virtual ListItem* RemoveItem(ListItem* Item);

	virtual LISTPOS InsertBefore(LISTPOS here, ListItem* item);
	virtual ListItem *InsertBefore(ListItem* here, ListItem* item);

	virtual LISTPOS InsertAfter(LISTPOS here, ListItem* item);
	virtual ListItem *InsertAfter(ListItem* here, ListItem* item);

	INT32 GetCurrentGreyscaleSize();

	void SetMaxGreyscaleSize(INT32);
	INT32 GetMaxGreyscaleSize();

#ifdef _DEBUG
	void DumpGreyscaleBitmapListToTrace();
#endif

protected:
	void CheckSizeIsOk();

protected:
	static INT32 m_lMaxSize;

private:
	INT32 m_CurrentSize;

};
*/

/***********************************************************************************************

>	class GlobalBitmapList : public List

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Purpose:	A list of all the OIL Bitmaps available in the application
	SeeAlso:	-

***********************************************************************************************/

class GlobalBitmapList : public List
{
	CC_DECLARE_MEMDUMP(GlobalBitmapList)

public:
	GlobalBitmapList();
	~GlobalBitmapList();

	static BOOL Init();
	static BOOL DeInit();

public:
	BOOL AddItem(OILBitmap *NewBitmap);

	void MakeNameUnique(String_256*);

	// Functions to iterate through the bitmaps
	ListItem* GetFirstBitmapName(String_256*);
	ListItem* GetNextBitmapName(ListItem*, String_256*);

	// Find a bitmap with a given name
	OILBitmap* GetBitmapFromName(String_256*);

	// Check to see if the bitmap exists already
	OILBitmap* FindDuplicateBitmap(OILBitmap* pBitmap);

	void DeleteAllUnusedBitmaps();
	BOOL IsBitmapOnClipBoard(OILBitmap* pOILBmp);
	BOOL IsUsedInDocument(Document * pDoc, OILBitmap* pOILBmp);

	// Functions to find statistics about the bitmap system
	INT32 GetDocumentBitmapCount(Document* pDoc);
	INT32 GetDocumentBitmapSize(Document* pDoc);

//	GreyscaleBitmapList* GetGreyscaleBitmapList();

#ifdef _DEBUG
	void DumpGlobalBitmapListToTrace();
#endif

protected:
//	GreyscaleBitmapList GreyscaleBitmaps;
};


/********************************************************************************************

>	class BitmapListChangedMsg: public Msg

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	This message is sent whenever the document bitmap list changes in some way.
	SeeAlso:	-

********************************************************************************************/

class BitmapListChangedMsg: public Msg
{
	CC_DECLARE_DYNAMIC(BitmapListChangedMsg);

public:
	// BitmapList changing messages are sent on these events.
	enum BitmapListState
	{
		SELECTNEWBITMAP,			// Sent when a new bitmap has been added to the list and
									// this new bitmap should be the new selection in the
									// bitmap gallery.
		LISTCHANGED,				// Sent when the list has changed. This is the default message.
	};

	BitmapList* pChangedBmpList;	// The bitmap list being changed
	KernelBitmap* pNewBitmap;		// The new bitmap added
	BitmapListState  State;			// The message specifics, as enumerated above

	// Default message, just saying the bitmap list has changed and so update anything that
	// relies on it
	BitmapListChangedMsg(BitmapList* pList):
		pChangedBmpList(pList), pNewBitmap(NULL), State(LISTCHANGED)	{ } 
	
	// New message where a new bitmap has been added and this should become the new selection.
	// At present, just used by the PhotoShop plug-ins so that this bitmap can become selected
	// in the gallery.
	BitmapListChangedMsg(BitmapList* pList, KernelBitmap *pBitmap):
		pChangedBmpList(pList), pNewBitmap(pBitmap), State(SELECTNEWBITMAP) { } 
};


// Optoken for the delete bitmap operation
#define OPTOKEN_DELETEBITMAP _T("DeleteBitmap")	

/********************************************************************************************

>	class OpDeleteBitmap: public Operation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Purpose:	Removes all references to a bitmap, and deletes it from memory.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpDeleteBitmap: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDeleteBitmap );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		

	// The 'do' function
	void DoDelete(OILBitmap*);

protected:
	void ForceRedrawOfNode(Document*, Node*, ObjChangeParam*);
};  

#endif
