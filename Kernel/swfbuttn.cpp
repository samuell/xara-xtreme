// $Id: swfbuttn.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "camtypes.h"
#include "swfplace.h"
#include "swfbuttn.h"

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	FlashButtonRecord::FlashButtonRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		-
	Returns:	-
	Purpose:	Creates a FlashButtonRecord list node, and initialises all member variables.

********************************************************************************************/

FlashButtonRecord::FlashButtonRecord ( void )
{
	// Create constant variables for initial states.
	const DocCoord	Position	( 0, 0 );		// Default position for all co-ordinates.

	// Pointers should always be initialised to NULL when created.
	mpNext			= NULL;					// No subsequent nodes.
	mpLast			= NULL;					// No previous nodes.

	// Attached objects.
	mButtonID		= 0;
	mHitID			= 0;
	mDownID			= 0;
	mOverID			= 0;
	mUpID			= 0;

	// Clear the strings.
	mURL.Empty ();
	mFrame.Empty ();
	mName.Empty ();

	// Place object positions.
	mHitPosition	= Position;
	mDownPosition	= Position;
	mOverPosition	= Position;
	mUpPosition		= Position;

	// Object states.
	mHitType		= FLASH_SHAPE;
	mDownType		= FLASH_SHAPE;
	mOverType		= FLASH_SHAPE;
	mUpType			= FLASH_SHAPE;

	mHasHit			= FALSE;
	mHasDown		= FALSE;
	mHasOver		= FALSE;
	mHasUp			= FALSE;

	mTransformHit	= FALSE;
	mTransformDown	= FALSE;
	mTransformOver	= FALSE;
	mTransformUp	= FALSE;

	mHasURL			= FALSE;
	mHasFrame		= FALSE;
}

/********************************************************************************************

>	FlashButtonRecord::~FlashButtonRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	-
	Purpose:	Destroys this instance of FlashButtonRecord.

********************************************************************************************/

FlashButtonRecord::~FlashButtonRecord ( void )
{
	// Clean up any stray values.
}

/********************************************************************************************

>	FlashButtonRecord* FlashButtonRecord::AddElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	pTail - The pointer to the new node.
	Purpose:	Adds an element to the tail of the list.

********************************************************************************************/

FlashButtonRecord* FlashButtonRecord::AddElement ( void )
{
	FlashButtonRecord *pTail = new FlashButtonRecord;
	ASSERT ( pTail != NULL );

	// Set the appropriate pointers.
	pTail->SetLast ( this );		// Ensure that a reference exists to this object...
	pTail->SetNext ( NULL );		// Avoids any problems if mpLast isn't NULL.
	mpNext = pTail;					// ... and a reference exists to the new one.

	return pTail;
}

/********************************************************************************************

>	void FlashButtonRecord::DeleteLastElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the previous item in the list.

********************************************************************************************/

void FlashButtonRecord::DeleteLastElement ( void )
{
	FlashButtonRecord *pToDelete = mpLast;
	
	// Reset mpLast to be mpLast->GetLast (), so that the list isn't broken.
	mpLast = mpLast->GetLast ();

	delete pToDelete;
}

/********************************************************************************************

>	void FlashButtonRecord::DeleteNextElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the next item in the list.

********************************************************************************************/

void FlashButtonRecord::DeleteNextElement ( void )
{
	FlashButtonRecord *pToDelete = mpNext;
	
	// Reset mpNext to be mpNext->GetNext (), so that the list isn't broken.
	mpNext = mpNext->GetNext ();

	delete pToDelete;
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetButtonID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		-
	Returns:	mButtonID - The internal ID number used by the Flash file for the button.
	Purpose:	Gets the value of mButtonID.

********************************************************************************************/

WORD FlashButtonRecord::GetButtonID ( void )
{
	return mButtonID;
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetHitID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		-
	Returns:	mHitID - Hit state ID.
	Purpose:	Gets the value of mHitID.

********************************************************************************************/

WORD FlashButtonRecord::GetHitID ( void )
{
	return GetActualID ( mHitID, mHitType );
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetDownID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		-
	Returns:	mDownID - Down state ID.
	Purpose:	Gets the value of mDownID.

********************************************************************************************/

WORD FlashButtonRecord::GetDownID ( void )
{
	return GetActualID ( mDownID, mDownType );
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetOverID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		-
	Returns:	mOverID - Over state ID.
	Purpose:	Gets the value of mOverID.

********************************************************************************************/

WORD FlashButtonRecord::GetOverID ( void )
{
	return GetActualID ( mOverID, mOverType );
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetUpID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		-
	Returns:	mUpID - Up state ID.
	Purpose:	Gets the value of mUpID.

********************************************************************************************/

WORD FlashButtonRecord::GetUpID ( void )
{
	return GetActualID ( mUpID, mUpType );
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetRawHitID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mHitID - Hit state ID.
	Purpose:	Gets the raw value of mHitID.

********************************************************************************************/

WORD FlashButtonRecord::GetRawHitID ( void )
{
	return mHitID;
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetRawDownID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mDownID - Down state ID.
	Purpose:	Gets the raw value of mDownID.

********************************************************************************************/

WORD FlashButtonRecord::GetRawDownID ( void )
{
	return mDownID;
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetRawOverID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mOverID - Over state ID.
	Purpose:	Gets the raw value of mOverID.

********************************************************************************************/

WORD FlashButtonRecord::GetRawOverID ( void )
{
	return mOverID;
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetRawUpID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mUpID - Up state ID.
	Purpose:	Gets the raw value of mUpID.

********************************************************************************************/

WORD FlashButtonRecord::GetRawUpID ( void )
{
	return mUpID;
}

/********************************************************************************************

>	DocCoord FlashButtonRecord::GetHitPosition ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mHitPosition - Hit state position.
	Purpose:	Gets the position of this button state.

********************************************************************************************/

DocCoord FlashButtonRecord::GetHitPosition ( void )
{
	return mHitPosition;
}

/********************************************************************************************

>	DocCoord FlashButtonRecord::GetDownPosition ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mDownPosition - Down state position.
	Purpose:	Gets the position of this button state.

********************************************************************************************/

DocCoord FlashButtonRecord::GetDownPosition ( void )
{
	return mDownPosition;
}

/********************************************************************************************

>	DocCoord FlashButtonRecord::GetOverPosition ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mOverPosition - Over state position.
	Purpose:	Gets the position of this button state.

********************************************************************************************/

DocCoord FlashButtonRecord::GetOverPosition ( void )
{
	return mOverPosition;
}

/********************************************************************************************

>	DocCoord FlashButtonRecord::GetUpPosition ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mUpPosition - Up state position.
	Purpose:	Gets the position of this button state..

********************************************************************************************/

DocCoord FlashButtonRecord::GetUpPosition ( void )
{
	return mUpPosition;
}

/********************************************************************************************

>	BOOL FlashButtonRecord::GetTransformHit ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mTransformHit - Is a transform required for mHitPosition?
	Purpose:	Do these co-ordinates need to be transformed into Flash co-ordinates? This is
				included to allow objects to be positioned at (0, 0).

********************************************************************************************/

BOOL FlashButtonRecord::GetTransformHit ( void )
{
	return mTransformHit;
}

/********************************************************************************************

>	BOOL FlashButtonRecord::GetTransformDown ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mTransformDown - Is a transform required for mDownPosition?
	Purpose:	Do these co-ordinates need to be transformed into Flash co-ordinates? This is
				included to allow objects to be positioned at (0, 0).

********************************************************************************************/

BOOL FlashButtonRecord::GetTransformDown ( void )
{
	return mTransformDown;
}

/********************************************************************************************

>	BOOL FlashButtonRecord::GetTransformOver ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mTransformOver - Is a transform required for mOverPosition?
	Purpose:	Do these co-ordinates need to be transformed into Flash co-ordinates? This is
				included to allow objects to be positioned at (0, 0).

********************************************************************************************/

BOOL FlashButtonRecord::GetTransformOver ( void )
{
	return mTransformOver;
}

/********************************************************************************************

>	BOOL FlashButtonRecord::GetTransformUp ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mTransformUp - Is a transform required for mUpPosition?
	Purpose:	Do these co-ordinates need to be transformed into Flash co-ordinates? This is
				included to allow objects to be positioned at (0, 0).

********************************************************************************************/

BOOL FlashButtonRecord::GetTransformUp ( void )
{
	return mTransformUp;
}

/********************************************************************************************

>	FlashType FlashButtonRecord::GetHitType ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mHitType - The object type to be rendered.
	Purpose:	Gets the class of object that is to be rendered in this button state.

********************************************************************************************/

FlashType FlashButtonRecord::GetHitType ( void )
{
	return mHitType;
}

/********************************************************************************************

>	FlashType FlashButtonRecord::GetDownType ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mDownType - The object type to be rendered.
	Purpose:	Gets the class of object that is to be rendered in this button state.

********************************************************************************************/

FlashType FlashButtonRecord::GetDownType ( void )
{
	return mDownType;
}

/********************************************************************************************

>	FlashType FlashButtonRecord::GetOverType ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mOverType - The object type to be rendered.
	Purpose:	Gets the class of object that is to be rendered in this button state.

********************************************************************************************/

FlashType FlashButtonRecord::GetOverType ( void )
{
	return mOverType;
}

/********************************************************************************************

>	FlashType FlashButtonRecord::GetUpType ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		-
	Returns:	mUpType - The object type to be rendered.
	Purpose:	Gets the class of object that is to be rendered in this button state.

********************************************************************************************/

FlashType FlashButtonRecord::GetUpType ( void )
{
	return mUpType;
}

/********************************************************************************************

>	TCHAR* FlashButtonRecord::GetURL ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		-
	Returns:	mURL - A pointer to a string containing a URL.
	Purpose:	Gets the contents of mURL.

********************************************************************************************/

TCHAR* FlashButtonRecord::GetURL ( void )
{
	return ( TCHAR * ) mURL;
}

/********************************************************************************************

>	TCHAR* FlashButtonRecord::GetFrame ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/00
	Inputs:		-
	Returns:	mFrame - A pointer to a string containing the frame value to use.
	Purpose:	Gets the contents of mFrame.

********************************************************************************************/

TCHAR* FlashButtonRecord::GetFrame ( void )
{
	return ( TCHAR * ) mFrame;
}

/********************************************************************************************

>	TCHAR* FlashButtonRecord::GetName ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/00
	Inputs:		-
	Returns:	mName - A pointer to a string containing the Camelot name for the button.
	Purpose:	Gets the contents of mName.

********************************************************************************************/

TCHAR* FlashButtonRecord::GetName ( void )
{
	return ( TCHAR * ) mName;
}

/********************************************************************************************

>	BOOL FlashButtonRecord::IsAMatch ( const TCHAR *pName )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/00
	Inputs:		-
	Returns:	TRUE if the names match, otherwise FALSE.
	Purpose:	Compares the names of two buttons (which is how Camelot IDs them), and
				determines whether or not they're identical.

********************************************************************************************/

BOOL FlashButtonRecord::IsAMatch ( const TCHAR *pName )
{
	return mName.IsIdentical ( pName );
}

/********************************************************************************************

>	FlashButtonRecord* FlashButtonRecord::GetLast ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	*mpLast - A pointer to the previous node in the linked list.
	Purpose:	Gets mpLast.

********************************************************************************************/

FlashButtonRecord* FlashButtonRecord::GetLast ( void )
{
	return mpLast;
}

/********************************************************************************************

>	FlashButtonRecord* FlashButtonRecord::GetNext ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	*mpNext - A pointer to the next node in the linked list.
	Purpose:	Gets mpNext.

********************************************************************************************/

FlashButtonRecord* FlashButtonRecord::GetNext ( void )
{
	return mpNext;
}

/********************************************************************************************

>	void FlashButtonRecord::SetButtonID ( WORD mID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		ID - The ID value for this button.
	Returns:	-
	Purpose:	Sets the value of mButtonID.

********************************************************************************************/

void FlashButtonRecord::SetButtonID ( WORD ID )
{
	mButtonID = ID;
}

/********************************************************************************************

>	void FlashButtonRecord::SetHitID ( WORD ID,
									   FlashType Type )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		ID - The ID value for this button state.
				Type - Is it a shape record? (Used for returning the ID value.)
	Returns:	-
	Purpose:	Sets the values of mHitID and mHitType.

********************************************************************************************/

void FlashButtonRecord::SetHitID ( WORD ID,
								   FlashType Type )
{
	mHitID		= ID;		// Set the ID value for this state.
	mHitType	= Type;		// And whether or not it's a shape.
	mHasHit		= TRUE;		// Record that this state has been set.
}

/********************************************************************************************

>	void FlashButtonRecord::SetDownID ( WORD ID,
										FlashType Type )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		ID - The ID value for this button state.
				Type - Is it a shape record? (Used for returning the ID value.)
	Returns:	-
	Purpose:	Sets the values of mDownID and mDownType.

********************************************************************************************/

void FlashButtonRecord::SetDownID ( WORD ID,
									FlashType Type )
{
	mDownID		= ID;		// Set the ID value for this state.
	mDownType	= Type;		// And whether or not it's a shape.
	mHasDown	= TRUE;		// Record that this state has been set.
}

/********************************************************************************************

>	void FlashButtonRecord::SetOverID ( WORD ID,
										FlashType Type )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		ID - The ID value for this button state.
				Type - Is it a shape record? (Used for returning the ID value.)
	Returns:	-
	Purpose:	Sets the values of mOverID and mOverType.

********************************************************************************************/

void FlashButtonRecord::SetOverID ( WORD ID,
									FlashType Type )
{
	mOverID		= ID;		// Set the ID value for this state.
	mOverType	= Type;		// And whether or not it's a shape.
	mHasOver	= TRUE;		// Record that this state has been set.
}

/********************************************************************************************

>	void FlashButtonRecord::SetUpID ( WORD ID,
									  FlashType Type )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		ID - The ID value for this button state.
				Type - Is it a shape record? (Used for returning the ID value.)
	Returns:	-
	Purpose:	Sets the values of mUpID and mUpType.

********************************************************************************************/

void FlashButtonRecord::SetUpID ( WORD ID,
								  FlashType Type )
{
	mUpID		= ID;		// Set the ID value for this state.
	mUpType		= Type;		// And whether or not it's a shape.
	mHasUp		= TRUE;		// Record that this state has been set.

	// I'm treating up as the default state, so ensure that all other states are set.
	if ( !mHasHit )
		SetHitID ( ID, Type );

	if ( !mHasOver )
		SetOverID ( ID, Type );

	if ( !mHasDown )
		SetDownID ( ID, Type );
}

/********************************************************************************************

>	void FlashButtonRecord::SetHitPosition ( DocCoord Position,
											 BOOL DoTransform )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		Position	- The position to place the object.
				DoTransform	- Do the co-ordinates need to be transformed into Flash ones?
	Returns:	-
	Purpose:	Sets the values of mHitPosition and mTransformHit.

********************************************************************************************/

void FlashButtonRecord::SetHitPosition ( DocCoord Position,
										 BOOL DoTransform )
{
	mHitPosition	= Position;
	mTransformHit	= DoTransform;
}

/********************************************************************************************

>	void FlashButtonRecord::SetDownPosition ( DocCoord Position,
											  BOOL DoTransform )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		Position	- The position to place the object.
				DoTransform	- Do the co-ordinates need to be transformed into Flash ones?
	Returns:	-
	Purpose:	Sets the values of mDownPosition and mTransformDown.

********************************************************************************************/

void FlashButtonRecord::SetDownPosition ( DocCoord Position,
										  BOOL DoTransform )
{
	mDownPosition	= Position;
	mTransformDown	= DoTransform;
}

/********************************************************************************************

>	void FlashButtonRecord::SetOverPosition ( DocCoord Position,
											  BOOL DoTransform )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		Position	- The position to place the object.
				DoTransform	- Do the co-ordinates need to be transformed into Flash ones?
	Returns:	-
	Purpose:	Sets the values of mOverPosition and mTransformOver.

********************************************************************************************/

void FlashButtonRecord::SetOverPosition ( DocCoord Position,
										  BOOL DoTransform )
{
	mOverPosition	= Position;
	mTransformOver	= DoTransform;
}

/********************************************************************************************

>	void FlashButtonRecord::SetUpPosition ( DocCoord Position,
											BOOL DoTransform )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		Position	- The position to place the object.
				DoTransform	- Do the co-ordinates need to be transformed into Flash ones?
	Returns:	-
	Purpose:	Sets the values of mUpPosition and mTransformUp.

********************************************************************************************/

void FlashButtonRecord::SetUpPosition ( DocCoord Position,
										BOOL DoTransform )
{
	mUpPosition		= Position;
	mTransformUp	= DoTransform;
}

/********************************************************************************************

>	void FlashButtonRecord::SetURL ( String256 Address )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		Address - A reference to a URL.
	Returns:	-
	Purpose:	Sets the value of mURL. mHasURL is also set, so that only the first URL
				string encountered is used. This is to keep it consistent with the Camelot
				button class.

********************************************************************************************/

void FlashButtonRecord::SetURL ( String_256 Address )
{
	if ( !mHasURL )
	{
		mURL = Address;
		mHasURL = TRUE;
	}
}

/********************************************************************************************

>	void FlashButtonRecord::SetFrame ( TCHAR *pFrame )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/00
	Inputs:		pFrame - A reference to a frame..
	Returns:	-
	Purpose:	Sets the value of mFrame.

********************************************************************************************/

void FlashButtonRecord::SetFrame ( TCHAR *pFrame )
{
	if ( !mHasFrame )
	{
		mFrame = pFrame;
		mHasFrame = TRUE;
	}
}

/********************************************************************************************

>	void FlashButtonRecord::SetName ( String256 Address )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/00
	Inputs:		pName - A reference to a button.
	Returns:	-
	Purpose:	Sets the value of mName.

********************************************************************************************/

void FlashButtonRecord::SetName ( const TCHAR* pName )
{
	mName = pName;
}

/********************************************************************************************

>	void FlashButtonRecord::SetLast ( FlashButtonRecord *pLast )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		pLast - A pointer to the previous FlashButtonRecord in the linked list.
	Returns:	-
	Purpose:	Sets the value of mpLast.

********************************************************************************************/

void FlashButtonRecord::SetLast ( FlashButtonRecord *pLast )
{
	mpLast = pLast;
}

/********************************************************************************************

>	void FlashButtonRecord::SetNext ( FlashButtonRecord *pNext )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		pNext - A pointer to the next FlashButtonRecord in the linked list.
	Returns:	-
	Purpose:	Sets the value of mpNext.

********************************************************************************************/

void FlashButtonRecord::SetNext ( FlashButtonRecord *pNext )
{
	mpNext = pNext;
}

/********************************************************************************************

>	WORD FlashButtonRecord::GetActualID ( WORD ID,
										  FlashType Type )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/99
	Inputs:		ID		- The basic ID value of the object to be used as a button state.
				Type	- The type of object to be rendered.
	Returns:	ID		- The actual value of this object's ID within the Flash file.
	Purpose:	Gets the value of mHitID.

********************************************************************************************/

WORD FlashButtonRecord::GetActualID ( WORD ID,
									  FlashType Type )
{
	// Step 1:	Increment the ID value past the font and bitmap records that are never
	//			rendered directly.
	ID += FlashPlaceObject::GetBitmapCount () + FlashPlaceObject::GetFontCount () +
		  FLASH_FIRST_ID;

	// Step 2:	If the record is a shape record, increment it past the text records.
	if ( Type == FLASH_SHAPE )
	{
		ID += FlashPlaceObject::GetTextCount ();
	}
	else if ( Type == FLASH_SPRITE )
	{
		ID += FlashPlaceObject::GetShapeCount ();
	}

	// Step 3:	Return the ID value.
	return ID;
}
