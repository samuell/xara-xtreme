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
/*
 * */
// Camelot's Internal clipboard

#ifndef INC_CLIPINT
#define INC_CLIPINT

//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]


class Layer;
class CCamDoc;
class Range; 


/********************************************************************************************
>	class InternalClipboard : public Document

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (Connection to ExternalClipboard by Jason, OLE stuff by JustinF)
	Created:	05/07/94 (19/4/95)

	Purpose:	Camelot's internal clipboard document.

				This is a cache, if you like, of the contents of the ExternalClipboard
				
				When Camelot "owns" the external clipboard, the InternalClipboard contains
				the objects which will be pasted into other applications. (When external
				apps paste, they will request that Camelot exports the InternalClipboard
				in some external exchange format)

				When Camelot doesn't own the external clipboard, the InternalClipboard
				will either be empty, or will be a cached copy of the external clipboard
				(if the user has pasted once into Camelot, while the clipboard remains
				unchanged, the internal clipboard will have the internal camelot tree
				representation of the clipboard cached - thus, pasting a .cdr file into
				camelot will only do the slow conversion once, and subsequent pastes
				will be 'native')

	SeeAlso:	ExternalClipboard; Document
*********************************************************************************************/

class InternalClipboard : public Document
{

/////////////////////////////////////////////////////////////////////////////////////////
//	Interface.

public:

	// The identifier's of the two clipboards.
	enum ID { CLIPBOARD, DRAGDROP };

	// Access the the two clipboards through these.
	static InternalClipboard* Instance()	{ return pInstance; }
	static InternalClipboard* Other()		{ return pOther; }
	static void Swap();
	static ID GetCurrentID();

	// Attributes.
	static BOOL IsEmpty(BOOL AskExternalClipboard = TRUE);
	static void DescribeContents(String_64* Result);
	static DocRect GetObjectsBounds();
	static Layer* GetInsertionLayer();
	static Range* GetClipboardRange()		{ return pInstance->pClipboardRange; }

	// Copying.
	static BOOL PrepareForCopy();
	static BOOL CopyInProgress();
	static BOOL CopyObjects(Range& RangeToCopy, Operation * pOp = NULL);
	static BOOL CopyComponentData(BaseDocument* pSrcDoc);
	static BOOL CopyCompleted();

	//Graham 24/4/97
	//A quick way of putting some text on the internal clipboard
	static BOOL CopyText(TCHAR* pBuffer);


	// Pasting.
	static BOOL PrepareForPaste(BOOL* pExternalSource = 0);
	static BOOL PasteCompleted();
	static DocCoord GetOriginalCentrePoint();
	static void SetOriginalCentrePoint(DocCoord NewCentre);
	// Notifications.
	static void Clear();
	static void AttrsHaveChanged(); 


/////////////////////////////////////////////////////////////////////////////////////////
//	Implementation.

public:

	// Startup / shutdown.  These manage the two internal clipboards, Instance and Other.
	static BOOL Initialise();
	static void Deinit();

protected:

	// Creation & destruction.
	InternalClipboard(ID id, BOOL fHide = TRUE);
	virtual ~InternalClipboard();
	virtual BOOL Init(CCamDoc* pOilDoc);

	// RTTI (of sorts).
	virtual BOOL IsNotAClipboard() const	{ return FALSE; };
	virtual BOOL IsAClipboard() const		{ return TRUE; };

private:

	// Helpers.
	BOOL OnNewClipboardObjects();
	static void DescribeContentsInternal(String_64* Result);

	// Data members.
	static	 InternalClipboard* pInstance;		// The only instance of the InternalClipboard
	static	 InternalClipboard* pOther;			// the other instance (temp save) of the clipboard
	Range*	 pClipboardRange;					// the other instance's range
	BOOL	 ClipOp;							// TRUE while we're doing a copy operation
	DocCoord dcCentre;							// centre of copied objects
	ID		 m_id;								// identifier of this clipboard
	BOOL		OriginalCentreLock;				// TRUE when calls to SetOriginalCentrePoint are disallowed
												// (only used when clipboard data was from an external source)

	CC_DECLARE_DYNAMIC(InternalClipboard);
		
	friend class ExternalClipboard;
	friend class OpDeleteBitmap;
};

#endif
