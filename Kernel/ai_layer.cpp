// $Id: ai_layer.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "ccobject.h"
#include "ai5_eps.h"
#include "ai_layer.h"

CC_IMPLEMENT_DYNAMIC ( AILayerProcessor, CCObject )

/********************************************************************************************

>	AILayerProcessor::AILayerProcessor ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/00
	Purpose:	Default constructor.
	SeeAlso:	AILayerProcessor::~AILayerProcessor ()

********************************************************************************************/

AILayerProcessor::AILayerProcessor ( void )
{
	// Initialise all member variables to their default state.
	mLayerName.Empty ();			// Clear the layer name string.
	mIsLocked		= FALSE;		// Set the layer locked flag to FALSE.
	mIsPrintable	= FALSE;		// Set the layer printable flag to FALSE.
	mIsVisible		= FALSE;		// Set the layer visible flag to FALSE.
}

/********************************************************************************************

>	AILayerProcessor::~AILayerProcessor ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/00
	Purpose:	Default destructor.
	SeeAlso:	AILayerProcessor::AILayerProcessor ()

********************************************************************************************/

AILayerProcessor::~AILayerProcessor ( void )
{
	// Nothing needs to be done.
}

/********************************************************************************************

>	BOOL AILayerProcessor::DecodeAI5Lb ( AI5EPSFilter	&Filter )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/00
	Inputs:		pFilter	- A pointer to the AI5EPSFilter being used for the export.
	Returns:	TRUE	- Success.
				FALSE	- An error has occurred.
	Purpose:	Extracts the layer information from the Lb record. The AI file format allows
				for more fine-grained control over the layers than the Camelot version, and
				so much of it is rejected.
	SeeAlso:	AILayerProcessor::DecodeLB (), AILayerProcessor::DecodeLn (),
				AI5EPSFilter::ProcessToken()

********************************************************************************************/

BOOL AILayerProcessor::DecodeAI5Lb ( AI5EPSFilter	&Filter )
{
	INT32	IsVisible	= 0;
	INT32	IsLocked	= 0;
	INT32	IsPrintable	= 0;
	
	// Pop the information off the stack.
	if ( !Filter.GetStack ().Discard ( 6 )			||	// Discard the unused data on the stack.
		 !Filter.GetStack ().Pop ( &IsPrintable )	||	// Extract whether it's printable.
		 !Filter.GetStack ().Pop ( &IsLocked )		||	// Extract whether the layer is locked.
		 !Filter.GetStack ().Discard ( 1 )			||	// Discard the unused flag.
		 !Filter.GetStack ().Pop ( &IsVisible ) )		// Extract the layer visibility data.
	{
		// There has been a problem extracting the data. Return a FALSE value to inform the
		// AIEPS filter.
		return FALSE;
	}

	// Set up the layer state flags from the information in the Lb operator. The default
	// state is false, so only one branch needs to be evaluated.
	if ( !IsLocked )
	{
		mIsLocked		= TRUE;							// Set the layer locked flag to TRUE.
	}

	if ( IsPrintable )
	{
		mIsPrintable	= TRUE;							// Set the printable flag to TRUE.
	}
	
	if ( IsVisible )
	{
		mIsVisible		= TRUE;							// Set the visible flag to TRUE.
	}

	// Success.
	return TRUE;
}

/********************************************************************************************

>	BOOL AILayerProcessor::DecodeAI8Lb ( AI5EPSFilter	&Filter )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/00
	Inputs:		pFilter	- A pointer to the AI5EPSFilter being used for the export.
	Returns:	TRUE	- Success.
				FALSE	- An error has occurred.
	Purpose:	The syntax of the Lb operator has changed in AI8, and this function is
				intended to handle the newer version.
	SeeAlso:	AILayerProcessor::DecodeAI5Lb (), AILayerProcessor::DecodeLB ()
				AILayerProcessor::DecodeLn (), AI8EPSFilter::ProcessToken ()

********************************************************************************************/

BOOL AILayerProcessor::DecodeAI8Lb ( AI5EPSFilter	&Filter )
{
	INT32	IsVisible	= 0;
	INT32	IsLocked	= 0;
	INT32	IsPrintable	= 0;
	
	// Pop the information off the stack.
	if ( !Filter.GetStack ().Discard ( 9 )			||	// Discard the unused data on the stack.
		 !Filter.GetStack ().Pop ( &IsPrintable )	||	// Extract whether it's printable.
		 !Filter.GetStack ().Pop ( &IsLocked )		||	// Extract whether the layer is locked.
		 !Filter.GetStack ().Discard ( 1 )			||	// Discard the unused flag.
		 !Filter.GetStack ().Pop ( &IsVisible ) )		// Extract the layer visibility data.
	{
		// There has been a problem extracting the data. Return a FALSE value to inform the
		// AIEPS filter.
		return FALSE;
	}

	// Set up the layer state flags from the information in the Lb operator. The default
	// state is false, so only one branch needs to be evaluated.
	if ( !IsLocked )
	{
		mIsLocked		= TRUE;							// Set the layer locked flag to TRUE.
	}

	if ( IsPrintable )
	{
		mIsPrintable	= TRUE;							// Set the printable flag to TRUE.
	}
	
	if ( IsVisible )
	{
		mIsVisible		= TRUE;							// Set the visible flag to TRUE.
	}

	// Success.
	return TRUE;
}

/********************************************************************************************

>	BOOL AILayerProcessor::DecodeLB ( AI5EPSFilter	&Filter )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/00
	Inputs:		pFilter	- A pointer to the AI5EPSFilter being used for the export.
	Returns:	TRUE	- Success.
				FALSE	- An error has occurred.
	Purpose:	Processes the LB command, which is the end of layer indicator.
	SeeAlso:	AILayerProcessor::DecodeLb (), AI5EPSFilter::ProcessToken()

********************************************************************************************/

BOOL AILayerProcessor::DecodeLB ( AI5EPSFilter	&Filter )
{
	// Push the insert point in the tree up a level, so that any future layers are inserted
	// in the correct place. This is done automatically by the EPSFilter class, so this
	// method is redundant.
	return TRUE;
}

/********************************************************************************************

>	BOOL AILayerProcessor::DecodeLn ( AI5EPSFilter	&Filter )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/00
	Inputs:		pFilter	- A pointer to the AI5EPSFilter being used for the export.
	Returns:	TRUE	- Success.
				FALSE	- An error has occurred.
	Purpose:	Processes the Ln command, which is used to set the layer's name.
	SeeAlso:	AILayerProcessor::DecodeLb (), AI5EPSFilter::ProcessToken()

********************************************************************************************/

BOOL AILayerProcessor::DecodeLn ( AI5EPSFilter	&Filter )
{
	// Pop the layer's name off the stack, and store it in the mLayerName member variable.
	if ( !Filter.GetStack ().Pop ( &mLayerName ) )
	{
		// There's been a problem, so inform the filter that invoked this command.
		return FALSE;
	}

	// Create a new layer using the stored information.
	return Filter.CreateLayer ( mLayerName, mIsLocked, mIsPrintable, mIsVisible );
}
