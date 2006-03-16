// $Id: swfbuttn.h 662 2006-03-14 21:31:49Z alex $
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
#ifndef INC_FLASH_BUTTON_RECORD
#define INC_FLASH_BUTTON_RECORD

#include "ccobject.h"
#include "strings.h"
#include "swfplace.h"

/********************************************************************************************

>	class FlashButtonRecord : public CObject

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Purpose:	Maintains a record of the buttons used by the Flash file.

********************************************************************************************/

class FlashButtonRecord : public CCObject
{
public:
	// Constructor and destructor.
	FlashButtonRecord ( void );									// Constructor.
	~FlashButtonRecord ( void );								// Destructor.

	// Add and remove elements to a linked list.
	FlashButtonRecord*	AddElement ( void );					// Adds an element.
	void				DeleteNextElement ( void );				// Deletes the next element.
	void				DeleteLastElement ( void );				// Deletes the last element.

	// Data access functions.
	WORD				GetButtonID ( void );					// Gets mButtonID.
	WORD				GetHitID ( void );						// Gets mHitID.
	WORD				GetDownID ( void );						// Gets mDownID.
	WORD				GetOverID ( void );						// Gets mOverID.
	WORD				GetUpID ( void );						// Gets mUpID.
	WORD				GetRawHitID ( void );					// Gets the raw mHitID.
	WORD				GetRawDownID ( void );					// Gets the raw mDownID.
	WORD				GetRawOverID ( void );					// Gets the raw mOverID.
	WORD				GetRawUpID ( void );					// Gets the raw mUpID.
	DocCoord			GetHitPosition ( void );				// Gets mHitPosition.
	DocCoord			GetDownPosition ( void );				// Gets mDownPosition.
	DocCoord			GetOverPosition ( void );				// Gets mOverPosition.
	DocCoord			GetUpPosition ( void );					// Gets mUpPosition.
	BOOL				GetTransformHit ( void );				// Gets mTransformHit.
	BOOL				GetTransformDown ( void );				// Gets mTransformDown.
	BOOL				GetTransformOver ( void );				// Gets mTransformOver.
	BOOL				GetTransformUp ( void );				// Gets mTransformUp.
	FlashType			GetHitType ( void );					// Gets mHitType.
	FlashType			GetDownType ( void );					// Gets mDownType.
	FlashType			GetOverType ( void );					// Gets mOverType.
	FlashType			GetUpType ( void );						// Gets mUpType.
	TCHAR*				GetURL ( void );						// Gets mURL.
	TCHAR*				GetFrame ( void );						// Gets mpFrame.
	TCHAR*				GetName ( void );						// Gets a pointer to mName.
	BOOL				IsAMatch ( const TCHAR *pName );		// Do the states match?
	FlashButtonRecord*	GetNext ( void );						// Gets mpNext.
	FlashButtonRecord*	GetLast ( void );						// Gets mpLast.

	// Set value functions.
	void				SetButtonID ( WORD ID );				// Sets mButtonID.
	void				SetHitID ( WORD ID,						// Sets mHitID.
								   FlashType Type );
	void				SetDownID ( WORD ID,					// Sets mDownID.
									FlashType Type );
	void				SetOverID ( WORD ID,					// Sets mOverID.
									FlashType Type );
	void				SetUpID ( WORD ID,						// Sets mUpID.
								  FlashType Type );
	void				SetHitPosition ( DocCoord Position,		// Sets mHitPosition.
										  BOOL DoTransform );
	void				SetDownPosition ( DocCoord Position,	// Sets mDownPosition.
										   BOOL DoTransform );
	void				SetOverPosition ( DocCoord Position,	// Sets mOverPosition.
										   BOOL DoTransform );
	void				SetUpPosition ( DocCoord Position,		// Sets mUpPosition.
										 BOOL DoTransform );
	void				SetURL ( String_256 Address );			// Sets mURL.
	void				SetFrame ( TCHAR *pFrame );				// Sets mFrame.
	void				SetName ( const TCHAR* pName );			// Sets mName.
	void				SetNext ( FlashButtonRecord *pNext );	// Sets the value of mpNext.
	void				SetLast ( FlashButtonRecord *pLast );	// Sets the value of mpLast.

private:
	WORD				GetActualID ( WORD ID,					// Gets the actual Flash ID.
									  FlashType Type );

	// Member variables.
	WORD				mButtonID;								// ID value for the button.

	WORD				mHitID;									// Object used for hit test.
	WORD				mDownID;								// Button down state object.
	WORD				mOverID;								// Over button state object.
	WORD				mUpID;									// Release state object.

	FlashType			mHitType;								// Is hit object a shape?
	FlashType			mDownType;								// Is down object a shape?
	FlashType			mOverType;								// Is over object a shape?
	FlashType			mUpType;								// Is up object a shape?

	DocCoord			mHitPosition;							// The hit state's position.
	DocCoord			mDownPosition;							// The down state's position.
	DocCoord			mOverPosition;							// The over state's position.
	DocCoord			mUpPosition;							// The up state's position.

	BOOL				mTransformHit;							// Transform hit position?
	BOOL				mTransformDown;							// Transform down position?
	BOOL				mTransformOver;							// Transform over position?
	BOOL				mTransformUp;							// Transform up position?

	BOOL				mHasHit;								// Has a hit state been set?
	BOOL				mHasDown;								// Has a down state been set?
	BOOL				mHasOver;								// Has an over state been set?
	BOOL				mHasUp;									// Has an up state been set?

	BOOL				mHasURL;								// Has an URL name been set?
	BOOL				mHasFrame;								// Has a frame name been set?

	String_256			mURL;									// Pointer to the URL called.
	String_256			mFrame;									// The frame to open URL in.
	String_256			mName;									// The name of the button.

	FlashButtonRecord	*mpNext;								// Pointer to the next node.
	FlashButtonRecord	*mpLast;								// Pointer to the next node.
};

#endif
