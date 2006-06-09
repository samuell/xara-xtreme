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
// This file implents the BitmapEffect class

/*
*/

#include "camtypes.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bfxbase.h"
#include "bfxalu.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(BitmapEffectBase, ListItem)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

// statics
BfxALU * BitmapEffectBase::ALU = NULL;

/********************************************************************************************

>	BitmapEffectBase::BitmapEffectBase()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for bitmap effect object
	Errors:		Causes a level 3 if the setting the parameters to the default fails
	SeeAlso:	-

This constructs an object and sets the parameters to the default set. As contructors can't fail
we explode if we can't set the parameters to the default set on the basis that this is an
operation defined algorithmically to work 100% of the time!

********************************************************************************************/

BitmapEffectBase::BitmapEffectBase()
{
	Source = NULL;
	Destination = NULL;
//	Monitor = NULL;

	if (!(SetParameters())) /* virtual */
	{
		ERROR3("Bitmap effect default parameter set failed in constructor");
	}
}

/********************************************************************************************

>	BitmapEffectBase::~BitmapEffectBase()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for bitmap effect object
	Errors:		Causes a level 3 if the setting the parameters to the default fails
	SeeAlso:	-

This constructs an object and sets the parameters to the default set. As contructors can't fail
we explode if we can't set the parameters to the default set on the basis that this is an
operation defined algorithmically to work 100% of the time!

********************************************************************************************/

BitmapEffectBase::~BitmapEffectBase()
{
	ERROR3IF((Destination!=NULL),"Bitmap effect destructor called when destination exists");
	// Unforunately we habe to use ERROR3 not ERROR2 as we can't return a flag	
}

/********************************************************************************************

>	static BOOL BitmapEffectBase::Init()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Inputs:		None
	Outputs:	Initialises subsystem
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Initialises the bitmap effect subsystem
	Errors:		None from this one as yet
	SeeAlso:	-

This call's primary purposes is to initialise the BfxALU, getting its static pointer.

********************************************************************************************/

BOOL BitmapEffectBase::Init()
{
	ALU=new BfxALU;
	return (ALU) ? (ALU->Init()) : FALSE;
}

void BitmapEffectBase::TestGD()
{
	ALU->TestGD();
}

/********************************************************************************************

>	static BOOL BitmapEffectBase::DeInit()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Inputs:		None
	Outputs:	DeInitialises subsystem
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Initialises the bitmap effect subsystem
	Errors:		None from this one as yet
	SeeAlso:	-

This call's primary purposes is to deinitialise the BfxALU.

********************************************************************************************/

BOOL BitmapEffectBase::DeInit()
{
	if ( (ALU) && (ALU->DeInit()) )
	{
		delete(ALU);
		ALU=NULL;
		return TRUE;
	}
	else return FALSE;
}

/********************************************************************************************

>	BOOL BitmapEffectBase::SetParameters()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	Sets up the parameters to the default
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Sets the bitmap effect params up to the default
	Errors:		None from this one as yet
	SeeAlso:	-

Normally this would be overridden to det a default set of parameters, but the base class
version might be used if the effect has no parameters

********************************************************************************************/

BOOL BitmapEffectBase::SetParameters()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL BitmapEffectBase::SetSourceImage(KernelBitmap * theSource)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		theSource is pointer to the bitmap to use, or NULL to unset it
	Outputs:	Records a pointer to the bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Sets up the effect to use a chosen bitmap
	Errors:		None from this one as yet
	SeeAlso:	-

Normally this would be overridden to check the bitmap was suitable.

********************************************************************************************/

BOOL BitmapEffectBase::SetSourceImage(KernelBitmap * theSource)
{
	Source = theSource;
	return TRUE;
}

/********************************************************************************************

>	BOOL BitmapEffectBase::GetEffectInfo(UINT32 * pOrderBitmap, UINT32 * pOrderEffect,
				   UINT32 * pPlotEstimate)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	pOrderBitmap filled in with the order of the operation wrt size of bitmap
				pOrderEffect filled in with the order of the operation wrt size of effect
				pPlotEstimate filled in with the number of sprite plots this is roughly
					equivalent to in time terms.
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Gets information about the given effect
	Errors:		None from this one as yet
	SeeAlso:	-

This should be overridden to give the user some idea of how long a bitmap operation is going
to take, so they can be marked as fast, slow or whatever. If the source image is not set up,
pPlotEstimate should be returned as zero.

Any parameters passed as NULL will not be filled

********************************************************************************************/

BOOL BitmapEffectBase::GetEffectInfo(UINT32 * pOrderBitmap, UINT32 * pOrderEffect,
				   UINT32 * pPlotEstimate)
{
	
	if (pOrderBitmap) *pOrderBitmap=0;
	if (pOrderEffect) *pOrderEffect=0;
	if (pPlotEstimate) *pPlotEstimate=0;
	return(TRUE);
}

/********************************************************************************************

>	BOOL BitmapEffectBase::GetDestinationInfo(UINT32 * pDestinationSize,
									INT32 * pDestinationDepth,
									UINT32 * pDestinationWidth,
									UINT32 * pDestinationHeight)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	pDestinationSize filled with size in bytes of dest bitmap
				pDestinationDepth filled with bpp of dest bitmap
				pDestinationWidth|Height filled with width height of dest in pixels
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Gets information about the given effect
	Errors:		Error2 if source hasn't been set up
	SeeAlso:	-

This should be overridden to give an accurate representation of what the dest bitmap will
look like. It is an	error to call this without setting the source up.

********************************************************************************************/


BOOL BitmapEffectBase::GetDestinationInfo(UINT32 * pDestinationSize,
									INT32 * pDestinationDepth,
									UINT32 * pDestinationWidth,
									UINT32 * pDestinationHeight)
{
	
	ERROR2IF(!Source,FALSE,"No source for bitmap effect");

	if (pDestinationSize) *pDestinationSize=0;
	if (pDestinationDepth) *pDestinationDepth=0;
	if (pDestinationWidth) *pDestinationWidth=0;
	if (pDestinationHeight) *pDestinationHeight=0;
	
	return(TRUE);
}


/********************************************************************************************

>	BOOL BitmapEffectBase::UseDestinationBitmap(KernelBitmap * theDestination)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		theDestination is pointer to the bitmap to use, or NULL to unset it
	Outputs:	Records a pointer to the dest bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Indicates the Run method should use this bitmap for output
	Errors:		None from this one as yet
	SeeAlso:	-

Normally this would be overridden to check the bitmap was suitable. The bitmap specified will
be used if specified, else a new KernelBitmap will be used. If the KernelBitmap does not have
an OilBitmap attached and created of the correct size, it will be created and attached.	If a
NULL is passed (this is the default situation) then a new KernelBitmap will be created.

This routine can also be called with NULL to indicate that responsibility for an output image
after a run has been assumed by the caller. The object won't destruct when it has a destination
bitmap.

********************************************************************************************/

BOOL BitmapEffectBase::UseDestinationBitmap(KernelBitmap * theDestination)
{
	Destination = theDestination;
	return TRUE;
}

/********************************************************************************************

>	KernelBitmap * BitmapEffectBase::GetDestination();
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	None
	Returns:	Pointer to destination imagwe
	Purpose:	Returns a pointer to the destination image
	Errors:		None from this one as yet
	SeeAlso:	-

The dest image will be present if it has been set using UseDestinationBitmap, or if the
effect has been run and UseDestinationBitmap has not been set to NULL subsequently.

********************************************************************************************/

KernelBitmap * BitmapEffectBase::GetDestination()
{
	return (Destination);
}

/********************************************************************************************

>	BOOL BitmapEffectBase::IsSourceSpecified();
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if source image is specified else false
	Purpose:	Indicates whether or a source image has been specified yet
	Errors:		None from this one as yet
	SeeAlso:	-

The source image will be specified if the last call to SetSourceImage was non null. Overridden
if multiple source images need to be specified.

********************************************************************************************/

BOOL BitmapEffectBase::IsSourceSpecified()
{
	return (Source != NULL);
}
								

/********************************************************************************************

>	BOOL BitmapEffectBase::IsAbleToRun();
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if you've done enough setting up to do a Run
	Purpose:	Indicates whether you've done enough setting up to do a Run
	Errors:		None from this one as yet
	SeeAlso:	-
 
For now just requires IsSourceSpecified

********************************************************************************************/

BOOL BitmapEffectBase::IsAbleToRun()
{
	return IsSourceSpecified();
}
					
/********************************************************************************************

>	BOOL BitmapEffectBase::Run();
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if failed
	Purpose:	Runs a transform!
	Errors:		ERROR2 if called when parameters aren't set up properly
	SeeAlso:	-

It's obviously important to override this!

********************************************************************************************/

BOOL BitmapEffectBase::Run()
{
	ERROR2IF(!IsAbleToRun(),FALSE,"BitmapEffectRun called at inappropriate time");
	return TRUE;
}
