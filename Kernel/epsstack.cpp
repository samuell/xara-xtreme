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

// Implementation of a PostScript style stack for the EPS reader


#include "camtypes.h"
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "basestr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"
//#include "epsfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "epsstack.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "epssitem.h"

/********************************************************************************************

>	void FIXEDPOINT::FromAscii(const TCHAR *FltPtString)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		FltPtString - string form of required number.
	Returns:	TRUE if ok, FALSE if not (e.g. syntax error in string)
	Purpose:	Decodes the ASCII string containing a floating point number and stores it
				as fixed point in this object.
	SeeAlso:	FIXEDPOINT

********************************************************************************************/

void FIXEDPOINT::FromAscii(const TCHAR *Str)
{
	// Initialise data member.
	Long = 0;

	// keep a check on the sign
	BOOL neg = (Str[0] == '-');

	// Look for the decimal place
	INT32 i = 0;
	while ((Str[i] != 0) && (Str[i] != '.'))
		i++;

	if (Str[i] == '.')
	{
		// Found a decimal point - extract the integer and fractional parts and use them
		// to construct the fixed point number.
		TCHAR Tmp[20];
		camStrcpy(Tmp, Str);
		Tmp[i] = 0;
		Long = 0;
		camScanf(Tmp, _T("%d"), &Long);
		Long *= 1000;

		// Force fraction to be 3 digits at the most (as we only store with 3 digits accuracy)
		Tmp[i + 4] = 0;

		// Convert fraction to integer
		INT32 Frac = 0;
		camScanf(&(Tmp[i+1]), _T("%d"), &Frac);

		// The fraction can be of the form .1, .10, or .100 - we must scale it correctly.
		INT32 FracLen = camStrlen(Tmp + i + 1);

		if (FracLen == 1)
			Frac *= 100;
		else if (FracLen == 2)
			Frac *= 10;

		// Fraction is now scaled correctly - add to integer part.
		if (neg)
			// Negative number, so subtract the fractional part
			Long -= Frac;
		else
			// Positive number, so add the fractional part
			Long += Frac;
	}
	else
	{
		// No decimal point found - just scale the integer part.
		Long = 0;
		camScanf(Str, _T("%d"), &Long);
		Long *= 1000;
	}
}

CC_IMPLEMENT_MEMDUMP(EPSStack, CC_CLASS_MEMDUMP)

/********************************************************************************************

>	EPSStack::EPSStack(UINT32 StackLimit = 50)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	Constructor.  Initialises scaling factor to be 1.

********************************************************************************************/

EPSStack::EPSStack() : List ()
{
	// Set scaling to unity.
	ScaleFactor = 1;

	// Set origin to default
	Origin.x = 0;
	Origin.y = 0;

	// By default, no additional transformation matrix is used.	
	UseXformMatrix = FALSE;
}

/********************************************************************************************

>	EPSStack::~EPSStack()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	Destructor - deallocates all storage associated with the stack.

********************************************************************************************/

EPSStack::~EPSStack()
{
	// Delete the contents of the list.
	DeleteAll ();
}

/********************************************************************************************

>	BOOL EPSStack::Push(const INT32 Long)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		Long - the data to push onto the stack.
	Returns:	TRUE if successful, FALSE if out of stack space.
	Purpose:	Push a INT32 value onto the stack.

********************************************************************************************/

BOOL EPSStack::Push ( const INT32 Long )
{
	EPSStackItem *pItem = new EPSStackItem;

	// Ensure that the item has been created.
	if ( pItem == NULL)
	{
		// Clear the stack as the caller will now abort.
		DeleteAll ();
			
		// Tell caller about the error
		ERROR ( _R(IDT_EPS_STACKLIMIT), FALSE );
	}
	
	// Initialise stack item.
	pItem->Init ( Long );

	// Add it to the list.
	AddHead ( pItem );

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::Push(const FIXEDPOINT FixedPoint)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		FixedPoint - the data to push onto the stack.
	Outputs:	-
	Returns:	TRUE if successful, FALSE if out of stack space.
	Purpose:	Push a fixed point value onto the stack.

********************************************************************************************/

BOOL EPSStack::Push ( const FIXEDPOINT FixedPoint )
{
	EPSStackItem *pItem = new EPSStackItem;

	// Ensure that the item has been created.
	if ( pItem == NULL)
	{
		// Clear the stack as the caller will now abort.
		DeleteAll ();
			
		// Tell caller about the error
		ERROR ( _R(IDT_EPS_STACKLIMIT), FALSE );
	}
	
	// Initialise stack item.
	pItem->Init ( FixedPoint );

	// Add it to the list.
	AddHead ( pItem );

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::Push(const double Double)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		Double - the data to push onto the stack.
	Outputs:	-
	Returns:	TRUE if successful, FALSE if out of stack space.
	Purpose:	Push a double value onto the stack.

********************************************************************************************/

BOOL EPSStack::Push ( const double Double )
{
	EPSStackItem *pItem = new EPSStackItem;

	// Ensure that the item has been created.
	if ( pItem == NULL)
	{
		// Clear the stack as the caller will now abort.
		DeleteAll ();
			
		// Tell caller about the error
		ERROR ( _R(IDT_EPS_STACKLIMIT), FALSE );
	}
	
	// Initialise stack item.
	pItem->Init ( Double );

	// Add it to the list.
	AddHead ( pItem );

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::Push ( const TCHAR *pString,
						  BOOL IsName = FALSE )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		pString - pointer to the string data to push onto the stack.
	Returns:	TRUE if successful, FALSE if out of memory or stack space.
	Purpose:	Push a TCHAR* string item onto the stack.

********************************************************************************************/

BOOL EPSStack::Push ( const TCHAR *pString,
					  BOOL IsName )
{
	EPSStackItem *pItem = new EPSStackItem;

	// Ensure that the item has been created.
	if ( pItem == NULL)
	{
		// Clear the stack as the caller will now abort.
		DeleteAll ();
			
		// Tell caller about the error
		ERROR ( _R(IDT_EPS_STACKLIMIT), FALSE );
	}
	
	// Try to copy string
	TCHAR *NewStr = new TCHAR[camStrlen(pString) + 1];
	ERRORIF(NewStr == NULL,  _R(IDT_EPS_NOMEMORY), FALSE);
	camStrcpy(NewStr, pString);

	// Initialise stack item.
	pItem->Init ( NewStr );

	// Add it to the list.
	AddHead ( pItem );

	return TRUE;
}
	
/********************************************************************************************

>	BOOL EPSStack::Push ( const EPSCommand Cmd )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		Cmd - the EPS command to push onto the stack.
	Returns:	TRUE if successful, FALSE if out of stack space.
	Purpose:	Push an EPS command onto the stack.

********************************************************************************************/

BOOL EPSStack::Push ( const EPSCommand Cmd )
{
	EPSStackItem *pItem = new EPSStackItem;

	// Ensure that the item has been created.
	if ( pItem == NULL)
	{
		// Clear the stack as the caller will now abort.
		DeleteAll ();
			
		// Tell caller about the error
		ERROR ( _R(IDT_EPS_STACKLIMIT), FALSE );
	}
	
	// Initialise stack item.
	pItem->Init ( Cmd );

	// Add it to the list.
	AddHead ( pItem );

	return TRUE;
}

	
/********************************************************************************************

>	BOOL EPSStack::Pop(INT32 *pLong)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pLong - the value popped from the stack.
	Returns:	TRUE if there is a numeric value on the top of the stack; FALSE if not.
	Purpose:	Pop a INT32 value from the top of the stack.  If there is a floating point
				value on the stack, it is converted before being returned to the caller.

********************************************************************************************/

BOOL EPSStack::Pop( INT32 *pLong )
{
	// Get pointer to the stack item.
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( RemoveHead () );

	// Return error if no items on stack.
	if ( pItem == NULL )
		return FALSE;

	// Check that the item is of the correct type
	switch ( pItem->Type )
	{
		case EPSTYPE_INT32:
			// Correct type - remove item from stack and return to caller.
			*pLong = static_cast<INT32> ( pItem->Data.Long );
			break;

		case EPSTYPE_FIXEDPOINT:
			// Perform implicit type conversion a la PostScript
			*pLong = static_cast<INT32> ( pItem->Data.FixedPoint.Long / FixedPointScale );
			break;

		case EPSTYPE_DOUBLE:
			// Perform implicit type conversion a la PostScript
			*pLong = static_cast<INT32> ( pItem->Data.Double );
			break;

		default:
			// Type mismatch - return error condition
			return FALSE;
	}

	// Delete item fron stack and return
	delete pItem;

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::Pop(UINT32 *pUint)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pUint - the value popped from the stack.
	Returns:	TRUE if there is a numeric value on the top of the stack; FALSE if not.
	Purpose:	Pop a UINT32 value from the top of the stack.  If there is a floating point
				value on the stack, it is converted before being returned to the caller.

********************************************************************************************/

BOOL EPSStack::Pop ( UINT32 *pUint )
{
	// Get pointer to the stack item.
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( RemoveHead () );

	// Return error if no items on stack.
	if ( pItem == NULL )
		return FALSE;

	// Check that the item is of the correct type
	switch ( pItem->Type )
	{
		case EPSTYPE_INT32:
			// Correct type - remove item from stack and return to caller.
			*pUint = static_cast<UINT32> ( pItem->Data.Long );
			break;

		case EPSTYPE_FIXEDPOINT:
			// Perform implicit type conversion a la PostScript
			*pUint = static_cast<UINT32> ( pItem->Data.FixedPoint.Long / FixedPointScale );
			break;

		case EPSTYPE_DOUBLE:
			// Perform implicit type conversion a la PostScript
			*pUint = static_cast<UINT32> ( pItem->Data.Double );
			break;

		default:
			// Type mismatch - return error condition
			return FALSE;
	}

	// Delete item fron stack and return
	delete pItem;

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::Pop(FIXEDPOINT *pFixedPoint)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pFixedPoint - the value popped from the stack.
	Returns:	TRUE if there is a numeric value on the top of the stack; FALSE if not.
	Purpose:	Pop a double value from the top of the stack.  If there is an integer value
				on top of the stack, it is converted to a floating point value before being
				returned to the caller.

********************************************************************************************/

BOOL EPSStack::Pop ( FIXEDPOINT *pFixedPoint )
{
	// Get pointer to the stack item.
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( RemoveHead () );

	// Return error if no items on stack.
	if ( pItem == NULL )
		return FALSE;
	
	// Check that the item is of the correct type
	switch (pItem->Type)
	{
		case EPSTYPE_INT32:
			// Correct type - remove item from stack and return to caller.
			pFixedPoint->Long = ( pItem->Data.Long * FixedPointScale );
			break;

		case EPSTYPE_FIXEDPOINT:
			// Perform implicit type conversion a la PostScript
			*pFixedPoint = pItem->Data.FixedPoint;
			break;

		case EPSTYPE_DOUBLE:
			// Perform implicit type conversion a la PostScript
			pFixedPoint->Long = static_cast <INT32> ( pItem->Data.Double );
			break;

		default:
			// Type mismatch - return error condition
			return FALSE;
	}
		
	// Delete item fron stack and return
	delete pItem;

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::Pop(double *pDouble)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pDouble - the value popped from the stack.
	Returns:	TRUE if there is a numeric value on the top of the stack; FALSE if not.
	Purpose:	Pop a double value from the top of the stack.  If there is an integer value
				on top of the stack, it is converted to a floating point value before being
				returned to the caller.

********************************************************************************************/

BOOL EPSStack::Pop ( double *pDouble )
{
	// Get pointer to the stack item.
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( RemoveHead () );

	// Return error if no items on stack.
	if ( pItem == NULL )
		return FALSE;
	
	// Check that the item is of the correct type
	switch ( pItem->Type )
	{
		case EPSTYPE_INT32:
			// Correct type - remove item from stack and return to caller.
			*pDouble = static_cast<double> ( pItem->Data.Long );
			break;

		case EPSTYPE_FIXEDPOINT:
			// Perform implicit type conversion a la PostScript
			*pDouble = static_cast<double> ( pItem->Data.FixedPoint.Long ) / FixedPointScale;
			break;

		case EPSTYPE_DOUBLE:
			// Perform implicit type conversion a la PostScript
			*pDouble = pItem->Data.Double;
			break;

		default:
			// Type mismatch - return error condition
			return FALSE;
	}
				
	// Delete item fron stack and return	
	delete pItem;

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::Pop(TCHAR* pString)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pString - the buffer to put the string value into.
	Returns:	TRUE if there is a string value on the top of the stack; FALSE if not.
	Purpose:	Pop a TCHAR* string value from the top of the stack.

********************************************************************************************/

BOOL EPSStack::Pop ( TCHAR* pString )
{
	// Get pointer to the stack item.
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( RemoveHead () );

	// Return error if no items on stack.
	if ( pItem == NULL )
		return FALSE;
	
	// Check that the item is of the correct type
	if ( pItem->Type == EPSTYPE_STRING )
	{
		// Correct type - remove item from stack and return to caller.
		camStrcpy ( pString, pItem->Data.pString );
		delete pItem;
		return TRUE;
	}
	else
	{
		// Tidy up the memory.
		delete pItem;

		// Type mismatch - return error condition
		return FALSE;
	}
}

/********************************************************************************************

>	BOOL EPSStack::Pop(StringBase* pString)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pString - the String object to put the string value into.
	Returns:	TRUE if there is a string value on the top of the stack; FALSE if not.
	Purpose:	Pop a string value from the top of the stack.

********************************************************************************************/

BOOL EPSStack::Pop(StringBase* pString)
{
	// Get pointer to the stack item.
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( RemoveHead () );

	// Return error if no items on stack.
	if ( pItem == NULL )
		return FALSE;
	
	// Check that the item is of the correct type
	if ( pItem->Type == EPSTYPE_STRING )
	{
		// Correct type - remove item from stack and return to caller.
		*pString = String_256(pItem->Data.pString);
		delete pItem;
		return TRUE;
	}
	else
	{
		// Tidy up the memory.
		delete pItem;

		// Type mismatch - return error condition
		return FALSE;
	}
}


/********************************************************************************************

>	BOOL EPSStack::PopCmd(EPSCommand *pCmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pCmd - the command on top of the stack.
	Returns:	TRUE if there is an EPS command on the top of the stack; FALSE if not.
	Purpose:	Pop an EPS command from the top of the stack.

********************************************************************************************/

BOOL EPSStack::PopCmd(EPSCommand *pCmd)
{
	// Get pointer to the stack item.
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( RemoveHead () );

	// Return error if no items on stack.
	if ( pItem == NULL )
		return FALSE;
	
	// Check that the item is of the correct type
	if (pItem->Type == EPSTYPE_COMMAND)
	{
		// Correct type - remove item from stack and return to caller.
		*pCmd = pItem->Data.Cmd;
		delete pItem;
		return TRUE;
	}
	else
	{
		// Type mismatch - return error condition
		delete pItem;
		return FALSE;
	}
}

/********************************************************************************************

>	BOOL EPSStack::PopCoord(INT32 *pLong)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pLong - the (scaled) value popped from the stack.
	Returns:	TRUE if there is a numeric value on the top of the stack; FALSE if not.
	Purpose:	Pop a INT32 value from the top of the stack.  If there is a fixed point
				value on the stack, it is converted before being returned to the caller.
				The value is scaled before being returned, so that it is easy to cope with
				EPS files that use brainless coordinate schemes (i.e. all of them).
				Scaling factor is initially 1, but can be changed via SetCoordScaleFactor().
	SeeAlso:	EPSStack::SetCoordScaleFactor

********************************************************************************************/

BOOL EPSStack::PopCoord ( INT32 *pLong )
{
	// Get pointer to the stack item.
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( RemoveHead () );

	// Return error if no items on stack.
	if ( pItem == NULL )
		return FALSE;
	
	// Check that the item is of the correct type
	switch ( pItem->Type )
	{
		case EPSTYPE_INT32:
			// Correct type - remove item from stack and return to caller.
			*pLong = ScaleFactor * (pItem->Data.Long);
			break;

		case EPSTYPE_FIXEDPOINT:
			// Perform implicit type conversion a la PostScript

			// Fixed point values are stored as value * 1000, so if the ScaleFactor is
			// actually 1000, don't bother to scale - just return the value.
			if (ScaleFactor == FixedPointScale)
				*pLong = static_cast<INT32> ( pItem->Data.FixedPoint.Long );
			else
			{
				// Perform the necessary scaling
				*pLong = static_cast<INT32> ( pItem->Data.FixedPoint.Long * 
						 ( static_cast<double> ( ScaleFactor ) ) );
				*pLong /= FixedPointScale;
			}
			break;

		case EPSTYPE_DOUBLE:
			// Perform implicit type conversion a la PostScript
			*pLong = static_cast<INT32> ( static_cast<double> ( ScaleFactor ) *
					 pItem->Data.Double );
			break;

		default:
			// Type mismatch - return error condition
			return FALSE;
	}
	
	// Delete item fron stack and return	
	delete pItem;
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::PopCoordPair(DocCoord *pCoord)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	pCoord - the (scaled and translated) coordinate pair popped from the stack.
	Returns:	TRUE if there is a coord pair on the top of the stack; FALSE if not.
	Purpose:	Pop two INT32 values from the top of the stack.
				If they are in floating point format, they are converted before being
				returned to the caller.
				The coords are scaled and translated before being returned, so that it is 
				easy to cope with EPS files that use brainless coordinate schemes (ArtWorks 
				EPS are you	listening?).
				Scaling factor is initially 1, but can be changed via SetCoordScaleFactor().
				Origin is initially (0,0), but can be changed via SetCoordOrigin().
	SeeAlso:	EPSStack::SetCoordScaleFactor; EPSStack::SetCoordOrigin

********************************************************************************************/

BOOL EPSStack::PopCoordPair(DocCoord *pCoord)
{
	if (PopCoord(&(pCoord->y)) && PopCoord(&(pCoord->x)))
	{
		// Use the transform matrix if necessary.
		if (UseXformMatrix)
			XformMatrix.transform((Coord *) pCoord, 1);

		// Translate coord using given origin and return success.
		pCoord->x += Origin.x;
		pCoord->y += Origin.y;

		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	BOOL EPSStack::PopColour(PColourCMYK *Col, TintType Tint = TINT_NONE,
							 FIXEDPOINT *pTintVal = NULL,
							 String_64 *pName = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/93
	Outputs:	Col - the CMYK values popped from the stack and converted to 0-255 range.

				TintType - indicates what kind of tint is on the stack:
					TINT_NONE        - no tint is on the stack
					TINT_ILLUSTRATOR - An Illustrator type tint is on the stack, i.e. a value
									   between 0 and 1, with 0 being 100% and 1 being 0%.
					TINT_COREL       - An Corel Draw type tint is on the stack, i.e. a value
									   between 0 and 1, with 1 being 100% and 0 being 0%.

				TintVal - the value of the tint operand.
				pName - the textual name of this colour.

				If there is a tint, then the CMYK values are followed by a colour name and a
				tint value (0.0 to 1.0).  The colour name is discarded, and the colour
				is adjusted according to the tint value found.
	Returns:	TRUE if valid colour parameters were found on the stack; FALSE if not enough
				parameters, or if there are not enough, or if they are outside the 0-1 range.
	Purpose:	Extract CYMK values from an ArtWorks EPS format and convert to Camelot range.
				AWEPS colour values are in the range 0 to 1; this function converts them
				to the 0 to 255 range.
	See Also:	EPSStack::PopColourRGB

********************************************************************************************/

BOOL EPSStack::PopColour(PColourCMYK *Col, TintType Tint, 
				   		 FIXEDPOINT *pTintVal, String_64 *pName)
{
	double Cyan, Magenta, Yellow, Key;
	FIXEDPOINT TintValue;

	if (Tint != TINT_NONE)
	{
		// Get the tint.
		if (!Pop(&TintValue))
			return FALSE;

		// Does the caller want the name of the colour?
		if (pName == NULL)
		{
			// Name is not wanted - discard it
			if (!Discard())
				return FALSE;
		}
		else
		{
			// Name is wanted - get it from the stack
			if (!Pop(pName))
				return FALSE;
		}

		ENSURE((TintValue >= 0) && (TintValue <= 1000), "Bad tint");

		// Convert Tint to sensible value, if required
		if (Tint == TINT_ILLUSTRATOR)
			TintValue = (1 * FixedPointScale) - (INT32) TintValue;

		if (pTintVal != NULL)
			*pTintVal = TintValue;
	}
				
	if (Pop(&Key) && Pop(&Yellow) && Pop(&Magenta) && Pop(&Cyan))
	{
		if ((Cyan < 0.0) || (Cyan > 1.0) || (Magenta < 0.0) || (Magenta > 1.0) || 
			(Yellow < 0.0) || (Yellow > 1.0) || (Key < 0.0) || (Key > 1.0))
			// Invalid colour value
			return FALSE;
			
		// Got the values ok - translate from 0->1 range to sensible range.
		// NB. Important that we round up here, because otherwise 8 bit values will
		//	   sometimes fail to match on boundary conditions.
		Col->Cyan 	 = (INT32) ((Cyan 	* 255.0) + 0.5);
		Col->Magenta = (INT32) ((Magenta * 255.0) + 0.5);
		Col->Yellow  = (INT32) ((Yellow 	* 255.0) + 0.5);
		Col->Key   	 = (INT32) ((Key 	* 255.0) + 0.5);

		if (Tint != TINT_NONE)
		{
			// Tint the colour
			Col->Cyan    = (Col->Cyan    * TintValue) / FixedPointScale;
			Col->Magenta = (Col->Magenta * TintValue) / FixedPointScale;
			Col->Yellow  = (Col->Yellow  * TintValue) / FixedPointScale;
			Col->Key     = (Col->Key     * TintValue) / FixedPointScale;
		}
				
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	BOOL EPSStack::PopColourRGB (INT32 * red, INT32 * green, INT32 * blue, 
								TintType Tint = TINT_NONE,
								FIXEDPOINT *pTintVal = NULL,
								String_64 *pName = NULL)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/00
	Outputs:	red, green, blue - the RGB values popped from the stack and converted to 
				0-255 range.

				TintType - indicates what kind of tint is on the stack:
					TINT_NONE        - no tint is on the stack
					TINT_ILLUSTRATOR - An Illustrator type tint is on the stack, i.e. a value
									   between 0 and 1, with 0 being 100% and 1 being 0%.
					TINT_COREL       - An Corel Draw type tint is on the stack, i.e. a value
									   between 0 and 1, with 1 being 100% and 0 being 0%.

				TintVal - the value of the tint operand.
				pName - the textual name of this colour.

				NOTE: The tint is currently not supported for RGB colours, although the 
				syntax has been kept to keep it consistent with the CMYK/Greyscale versions, 
				as well to enable expansion of this function.

	Returns:	TRUE if valid colour parameters were found on the stack; FALSE if not enough
				parameters, or if there are not enough, or if they are outside the 0-1 range.
	Purpose:	Extract RGB values from a generic EPS format and convert to Camelot range. 
				EPS colour values are in the range 0 to 1; this function converts them
				to the 0 to 255 range.
	See Also:	EPSStack::PopColour

********************************************************************************************/

BOOL EPSStack::PopColourRGB (INT32 * red, INT32 * green, INT32 * blue, TintType Tint, 
				   			FIXEDPOINT *pTintVal, String_64 *pName)
{
	double tempRed, tempGreen, tempBlue;
	FIXEDPOINT TintValue;

	if (Tint != TINT_NONE)
	{
		// Get the tint.
		if (!Pop(&TintValue))
			return FALSE;

		// Does the caller want the name of the colour?
		if (pName == NULL)
		{
			// Name is not wanted - discard it
			if (!Discard())
				return FALSE;
		}
		else
		{
			// Name is wanted - get it from the stack
			if (!Pop(pName))
				return FALSE;
		}

		ENSURE((TintValue >= 0) && (TintValue <= 1000), "Bad tint");

		// Convert Tint to sensible value, if required
		if (Tint == TINT_ILLUSTRATOR)
			TintValue = (1 * FixedPointScale) - (INT32) TintValue;

		if (pTintVal != NULL)
			*pTintVal = TintValue;
	}
				
	if (Pop(&tempBlue) && Pop(&tempGreen) && Pop(&tempRed))
	{
		if ((tempRed < 0.0) || (tempRed > 1.0) || 
			(tempBlue < 0.0) || (tempBlue > 1.0) || 
			(tempGreen < 0.0) || (tempGreen > 1.0))
			// Invalid colour value
			return FALSE;
			
		// Got the values ok - translate from 0->1 range to sensible range.
		// NB. Important that we round up here, because otherwise 8 bit values will
		//	   sometimes fail to match on boundary conditions.
		*red	= (INT32) ((tempRed	 * 255.0) + 0.5);
		*green	= (INT32) ((tempGreen * 255.0) + 0.5);
		*blue	= (INT32) ((tempBlue	 * 255.0) + 0.5);

		if (Tint != TINT_NONE)
		{
			// Tint the colour - Not currently done for RGB colours
		}
				
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	BOOL EPSStack::PopGrayScale(PColourCMYK *Col)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Outputs:	Col - the CMYK version of the grayscale popped from the stack and converted 
					  to 0-255 range.
	Returns:	TRUE if a valid grayscale parameter was found on the stack; FALSE if not.
	Purpose:	Extract grayscale value from an EPS format file and convert to Camelot range.
	SeeAlso:	PopColour

********************************************************************************************/

BOOL EPSStack::PopGrayScale(PColourCMYK *Col)
{
	double Gray=0.0;

	if (Pop(&Gray))
	{
		if ((Gray < 0.0) || (Gray > 1.0))
			// invalid gray scale value
			return FALSE;

		// Got the grayscale ok - translate from 0->1 range to sensible range.
		PColourValue GrayVal = 255 - (static_cast<INT32> (Gray * 255));

		Col->Cyan    = 0;
		Col->Magenta = 0;
		Col->Yellow  = 0;
		Col->Key     = static_cast<BYTE> ( GrayVal );

		return TRUE;
	}
	else
		return FALSE;
}



/********************************************************************************************

>	BOOL EPSStack::Pop(Matrix *pMatrix, BOOL UseBrackets)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		pMatrix - the matrix to hold the popped values.
				UseBrackets - TRUE if the matrix is bracketed by [];
							 FALSE if just the numbers are in the file.
	Returns:	TRUE if the matrix was decode ok;
				FALSE otherwise
	Purpose:	Pop a matrix object from the EPS stack.
	Errors:		Not enough parameters/incorrect parameters.

********************************************************************************************/

BOOL EPSStack::Pop(Matrix *pMatrix, BOOL UseBrackets)
{
	if (UseBrackets)
	{
		EPSCommand Cmd;
		PopCmd(&Cmd);
		if (Cmd != EPSC_ArrayEnd)
			return FALSE;
	}

	// Extract the six array values from the stack.
	double M[6];
	INT32 i = 5;
	for (i = 5; i >= 0; i--)
	{
		if (!Pop(&M[i]))
			return FALSE;
	}

	if (UseBrackets)
	{
		EPSCommand Cmd;
		PopCmd(&Cmd);
		if (Cmd != EPSC_ArrayStart)
			return FALSE;
	}

	// Convert the abcd values into FIXED16s, cos that's what we use.
	FIXED16 F16[4];
	for (i = 0; i < 4; i++)
	{
		F16[i] = FIXED16(M[i]);
	}

	// Convert the Tx and Ty to MILLIPOINTS, cos that's what we use.
	INT32 L1 = (INT32) (M[4] * 1000.0);
	INT32 L2 = (INT32) (M[5] * 1000.0);

	// Construct the matrix in the variable supplied by the caller.
	*pMatrix = Matrix(F16[0], F16[1], F16[2], F16[3], L1, L2);

	// Success
	return TRUE;
}

/********************************************************************************************

>	void EPSStack::SetCoordScaleFactor(INT32 NewScaleFactor)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/93
	Inputs:		NewScaleFactor - the new factor to use when scaling coordinates.
	Purpose:	Set the scale factor used by PopCoord().  Scaling factor is initially 1.
	SeeAlso:	EPSStack::PopCoord()

********************************************************************************************/

void EPSStack::SetCoordScaleFactor(INT32 NewScaleFactor)
{
	ScaleFactor = NewScaleFactor;
}
	
/********************************************************************************************

>	void EPSStack::SetCoordOrigin(DocCoord NewOrigin)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/93
	Inputs:		NewOrigin - the new origin to use when translating coordinates.
	Purpose:	Set the translation values used by PopCoord().  Initially (0,0).
	SeeAlso:	EPSStack::PopCoord()

********************************************************************************************/

void EPSStack::SetCoordOrigin(DocCoord NewOrigin)
{
	Origin = NewOrigin;
}

/********************************************************************************************

>	void EPSStack::TranslateCoordOrigin(MILLIPOINT dx, MILLIPOINT dy)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/94
	Inputs:		dx, dy - the translation values.
	Purpose:	Translate the current origin for the EPS file by the given amounts.
				This is used when the %%BoundingBox comment is encountered, so that the
				image appears in a sensible place.
	SeeAlso:	EPSStack::SetCoordOrigin

********************************************************************************************/

void EPSStack::TranslateCoordOrigin(MILLIPOINT dx, MILLIPOINT dy)
{
	Origin.x += dx;
	Origin.y += dy;
}

/********************************************************************************************

>	void EPSStack::SetXformMatrix(Matrix& NewMatrix)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/94
	Inputs:		NewMatrix - the matrix to use from now on.
	Purpose:	Sets up an additional transformation matrix to be used when decoding
				document coordinates (via EPSStack::PopCoordPair).
				When a transform matrix is active, all coords returned from PopCoordPair
				will have been transformed by the matrix after first having had the usual
				origin translation performed.
				The transformation matrix can be disabled by called SetNoXFormMatrix().
	Errors:		-
	SeeAlso:	EPSStack::PopCoordPair; EPSStack::SetNoXFormMatrix

********************************************************************************************/

void EPSStack::SetXformMatrix(Matrix& NewMatrix)
{
	// Install the new matrix
	XformMatrix = NewMatrix;
	UseXformMatrix = TRUE;
}

/********************************************************************************************

>	void EPSStack::SetNoXFormMatrix()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/94
	Purpose:	Disable the transformation matrix used by EPSStack::PopCoordPair().
				See SetXFormMatrix() for details.
	SeeAlso:	EPSStack::PopCoordPair; EPSStack::SetXFormMatrix

********************************************************************************************/

void EPSStack::SetNoXformMatrix()
{
	UseXformMatrix = FALSE;
}

/********************************************************************************************

>	BOOL EPSStack::Discard(UINT32 NumToDiscard = 1, BOOL EvenCommands = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/93
	Inputs:		NumToDiscard - the number of items to remove from the top of the stack.
	Returns:	TRUE if there were sufficient items on the stack to satisfy request, FALSE
				if not.
	Purpose:	Discard a number of items from the stack, irrespective of their type.
				NB. The exception is that you cannot discard a command, only operands (e.g.
				strings, numbers or names) unless EvenCommands == TRUE.
	SeeAlso:	EPSStack::Pop

********************************************************************************************/

BOOL EPSStack::Discard(UINT32 NumToDiscard, BOOL EvenCommands)
{
	// Check we can discard this many items
	if ( GetCount () < NumToDiscard )
		return FALSE;
		
	while (NumToDiscard > 0)
	{
		EPSStackItem *pItem = static_cast<EPSStackItem*> ( GetHead () );

		// Catch NULL pointers.
		if ( pItem == NULL )
		{
			// Cannot discard any more list items.
			break;
		}

		if((pItem->Type == EPSTYPE_COMMAND) && (EvenCommands == FALSE))
		{	// Cannot discard a command
			return FALSE;
		}

		// Clean up any storage for this item
		RemoveHead ();
		delete pItem;
		
		// Move to next item
		NumToDiscard--;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::PopArray(INT32 *Array, INT32* Elements)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/95
	Returns:	TRUE if the array was popped ok, FALSE if an error occured.
	Purpose:	Pop a PostScript array object from the stack.
	Errors:		No syntactically correct array on the stack.
	SeeAlso:	EPSStack::Discard

********************************************************************************************/

BOOL EPSStack::PopArray(INT32 *Array, INT32* Elements)
{
	if (Array == NULL || Elements == NULL)
		return FALSE;

	if (GetType() != EPSTYPE_COMMAND)
	{
		TRACEUSER( "Will", _T("EPSStack::PopArray: Expected ArrayEnd\n"));
		return FALSE;
	}
	
	// First this we find should be the 'Array End' command (']')
	EPSCommand Cmd;
	PopCmd(&Cmd);
	if (Cmd != EPSC_ArrayEnd)
	{
		TRACEUSER( "Will", _T("EPSStack::PopArray: Expected ArrayEnd\n"));
		return FALSE;
	}

	INT32 NumElements = 0;	

	// The array element will be popped off the stack backwards,
	// so we need to make a tempory array, so we can swap them,
	// around at the end.
	INT32* TempArray = new INT32[*Elements];
	if (TempArray == NULL)
		return FALSE;

	// Read the rest of the array
	for(;;)
	{
		if (IsEmpty())
		{
			// Run out of operands!
			TRACEUSER( "Will", _T("EPSStack::PopArray: Run out of operands\n"));
			delete TempArray;
			return FALSE;
		}
			
		if (GetType() != EPSTYPE_COMMAND)
		{
			// Pop the array member
			double Double;
			if (!Pop(&Double))
			{
				delete TempArray;
				return FALSE;
			}

			if (NumElements < *Elements)
			{
				TempArray[NumElements] = (INT32) (Double * EPSScaleFactor);
				NumElements++;
			}
		}
		else
		{
			EPSCommand Cmd = ReadCmd();
			if (!PopCmd(&Cmd) || Cmd != EPSC_ArrayStart)
			{
				TRACEUSER( "Will", _T("EPSStack::PopArray: Expected ArrayStart\n"));
				delete TempArray;
				return FALSE;
			}
			else
				break; // exit loop - end of array found
		}
	}

	*Elements = NumElements;	// Actual number of elements read

	// Now swap the elements so they are the correct way around.
	if (NumElements > 0)
	{
		INT32 LastEl = NumElements - 1;

		for (INT32 el = 0; el < NumElements; el++)
		{
			Array[el] = TempArray[LastEl - el];
		}
	}

	delete TempArray;

	// Array popped ok
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSStack::DiscardArray()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/94
	Returns:	TRUE if the array was discarded ok, FALSE if an error occured.
	Purpose:	Discard a PostScript array object from the stack.  This is normally used
				when an EPS filter is not complete, but the file is still parsed correctly
				by discarding information that is npt yet needed/processed.
	Errors:		No syntactically correct array on the stack.
	SeeAlso:	EPSStack::Discard

********************************************************************************************/

BOOL EPSStack::DiscardArray()
{
	if (GetType() != EPSTYPE_COMMAND)
	{
		TRACEUSER( "Tim", _T("EPSStack::DiscardArray: Expected ArrayEnd\n"));
		return FALSE;
	}
	
	EPSCommand Cmd;
	PopCmd(&Cmd);
	if (Cmd != EPSC_ArrayEnd)
	{
		TRACEUSER( "Tim", _T("EPSStack::DiscardArray: Expected ArrayEnd\n"));
		return FALSE;
	}
	
	// Discard the rest of the array
	for(;;)
	{
		if (IsEmpty())
		{
			// Run out of operands!
			TRACEUSER( "Tim", _T("EPSStack::DiscardArray: Run out of operands\n"));
			return FALSE;
		}
			
		if (GetType() != EPSTYPE_COMMAND)
			// Discard array member
			Discard();
		else
		{
			EPSCommand Cmd = ReadCmd();
			if (!PopCmd(&Cmd) || Cmd != EPSC_ArrayStart)
			{
				TRACEUSER( "Tim", _T("EPSStack::DiscardArray: Expected ArrayStart\n"));
				return FALSE;
			}
			else
				break; // exit loop - end of array found
		}
	}

	// Array discarded ok
	return TRUE;
}

/********************************************************************************************

>	EPSType EPSStack::GetType()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/93
	Returns:	The type of the object on the top of the stack (if the stack is	empty, 
				the value EPSTYPE_NONE is returned).
	Purpose:	Find out what type of object is on the top of the stack.
				The item is not removed from the stack by this operation.

********************************************************************************************/

EPSType EPSStack::GetType()
{
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( GetHead () );

	if ( pItem == NULL )
		return EPSTYPE_NONE;
	else
		return pItem->Type;
}
	
/********************************************************************************************

>	EPSCommand EPSStack::ReadCmd()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/93
	Returns:	The EPS command on the top of the stack, or EPSC_Invalid if there is no 
				EPS command at the top of the stack.
	Purpose:	Find out what command is on top of the stack.  The command is not removed
				from the stack.  Numbers, strings, names, etc. do not count as being
				commands.
	SeeAlso:	EPSStack::GetType

********************************************************************************************/

EPSCommand EPSStack::ReadCmd()
{
	EPSStackItem *pItem = static_cast<EPSStackItem*> ( GetHead () );

	if ( ( pItem == NULL ) || ( pItem->Type != EPSTYPE_COMMAND ) )
		return EPSC_Invalid;
	else
		return pItem->Data.Cmd;
}
	
/********************************************************************************************

>	BOOL EPSStack::IsEmpty()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Returns:	Returns TRUE if the stack is empty, FALSE if it is not.
	Purpose:	Test to see if the stack is empty.
	SeeAlso:	EPSStack::GetCount();

********************************************************************************************/

BOOL EPSStack::IsEmpty()
{
	return ( GetCount () == 0 );
}

/********************************************************************************************

>	void EPSStack::Dump(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/93
	Inputs:		Pointer to the filter that this stack is owned by.
	Purpose:	Dump out the contents of the stack to the debug window.
				The stack is unchanged.

********************************************************************************************/

void EPSStack::Dump(EPSFilter *pFilter)
{
#if defined(_DEBUG)
	if (!Error::IsUserName("Tim") && !Error::IsUserName("Ben"))
		return;

	TRACE( _T("EPS STACK DUMP START\n\n"));	
	
	for ( DWORD i = 0; i < GetCount (); i++ )
	{
		EPSStackItem *pItem = static_cast<EPSStackItem*> ( FindItem ( i ) );

		switch (pItem->Type)
		{
			case EPSTYPE_COMMAND:
				TRACE( _T("Command: %s\n"), pFilter->GetEPSCommand(pItem->Data.Cmd));
				break;
				
			case EPSTYPE_FIXEDPOINT:
				TRACE( _T("Fixed point: %f\n"), ((double) pItem->Data.FixedPoint.Long) / FixedPointScale);
				break;
				
			case EPSTYPE_INT32:
				TRACE( _T("Long: %ld\n"), pItem->Data.Long);
				break;
				
			case EPSTYPE_STRING:
				TRACE( _T("String: '%s'\n"), pItem->Data.pString);
				break;
				
			case EPSTYPE_NAME:
				TRACE( _T("Name: '%s'\n"), pItem->Data.pString);
				break;

			case EPSTYPE_DOUBLE:
				TRACE( _T("Double: %f\n"), pItem->Data.Double);
				break;
				
			case EPSTYPE_EMPTY:
			case EPSTYPE_NONE:
				TRACE( _T("Empty/None\n"));
				break;

			default:
				TRACE( _T("Unknown type\n"));
				break;
		}

	}
	
	TRACE( _T("\nEPS STACK DUMP END\n"));	
#endif
}

