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
// Support for Graphics Tablets with Pressure Sensitive Pens

/*
*/

#include "camtypes.h"

#include "pen.h"

#include "app.h"
//#include "will.h"
#include "camelot.h"
//#include "gdihlp16.h"
//#include "oilmods.h"
//#include "mainfrm.h"

//#include "rik.h"
//#include "barsdlgs.h"

//#include "tablet.h"

#include "keypress.h"
#include "pathtrap.h"


DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(CCPen, CCObject);
CC_IMPLEMENT_DYNCREATE(OpTogglePressure, Operation);

#define new CAM_DEBUG_NEW

// Default pressure mode preference. This indicates the last pressure mode
// the user used (no-pressure, use pressure pen, or simulate pressure with mouse)
PressureMode CCPen::DefaultPressureMode = PressureMode_None;




/********************************************************************************************

>	CCPen *CCPen::Init()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to applications CCPen object, or NULL if failed.
	Purpose:	Application.LateInit calls this to obtain the default pen object.
	Errors:		->SetError if FALSE
	Scope:		Static

********************************************************************************************/

CCPen *CCPen::Init()
{
	PORTNOTETRACE("other","CCPen::Init - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Check the ini file to see what state we're in
	if (Camelot.DeclareSection("PenSupport", 4))
		Camelot.DeclarePref("PenSupport", "PressureMode", (INT32 *)&DefaultPressureMode, PressureMode_None, PressureMode_MaxEnum);

	// Create a pressure sensitive pen. We create a derived class WinTab pen
	// which will call back to the base class if a WinTab device is unavailable.
	return(new WinTabPressurePen);
#else
	return NULL;
#endif
}



/********************************************************************************************

>	CCPen::CCPen()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CCPen constructor.
	SeeAlso:	-
	Errors:		-

********************************************************************************************/

CCPen::CCPen()
{
	PORTNOTETRACE("other","CCPen::CCPen - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// See if it is likely that we'll be able to initialise as a "real" pen
	PressureAvailable = FALSE;

	// Initalise member variables
	wxWindow		   *pMainFrame = GetMainFrame();
	ERROR3IF(pMainFrame == NULL, "No main frame window when CCPen initialised");
	hMainframeWnd = pMainFrame->m_hWnd;

	PenPressure = 0;
	PressureMax = MAXPRESSURE;			// Set the default Maximum Pressure Value

	CurrentPressureMode = DefaultPressureMode;
	if (CurrentPressureMode == PressureMode_None)
		PenPressure = PressureMax;

	LastMousePoint = CPoint(0,0);
#endif
}



/********************************************************************************************

>	CCPen::~CCPen()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CCPen destructor.
	Errors:		-

********************************************************************************************/

CCPen::~CCPen()
{
}



/********************************************************************************************

>	virtual void CCPen::StartStroke(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/97

	Purpose:	Informs the Pen class that you are starting a stroke (a drag
				operation for which you wish to record pressure information)

				If you do not call this method, then pressure information
				will be "faked" based on mouse speed/direction information.

				It should be called when you start your drag (on button down)

				When the stroke finishes, remember to call CCPen::EndStroke

********************************************************************************************/

void CCPen::StartStroke(void)
{
	CurrentPressureMode = DefaultPressureMode;
}



/********************************************************************************************

>	virtual void CCPen::EndStroke(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/97

	Purpose:	Informs the Pen class that you are completing a stroke (a drag
				operation for which you recorded pressure information)

				This function must be called to "cancel" out a call to StartStroke.
				It should be called in your drag completion routine, and should
				be called under all "end of drag" circumstances.

********************************************************************************************/

void CCPen::EndStroke(void)
{
	// Base class does nothing
}



/********************************************************************************************

>	virtual void CCPen::ReadTabletPressureData(void)

	Author:		Martin_Donelly (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/97

	Returns:	TRUE if it read pressure successfully
				FALSE if it failed - in this case, the caller (base class) will default
				to calculating a faked pressure value from movement information. (e.g.
				if you stop getting pen pressure packets, revert to using movement
				rather than always returning zero!)

	Purpose:	Records the latest pressure info from the pressure sensitive tablet
				Does nothing if this is not a "real" pen

				Does nothing in the base class.

********************************************************************************************/

BOOL CCPen::ReadTabletPressureData(void)
{
	if (!PressureAvailable)
		return(FALSE);

	// Base class does nothing
	return(FALSE);
}



/********************************************************************************************

>	void CCPen::CheckMouseMessage(UINT32 Message, CPoint point)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		Message - WM_MOUSEMOVE if the mouse is moving, or WM_{L/R/M}BUTTONDOWN if 
							we're starting a drag
				point - the mouse position
	
	Outputs:	-
	Returns:	-
	Purpose:	Checks a mouse message to see if it came from a pen, and if so sets the
				current pressure value.
	Errors:		-

********************************************************************************************/

void CCPen::CheckMouseMessage(UINT32 Message, wxPoint point)
{
	PORTNOTETRACE("other","CCPen::CheckMouseMessage - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (IsRealPen() && CurrentPressureMode == PressureMode_Pen)
	{
		// Go and check the tablet for new pressure data. If it fails, we will drop through
		// to the normal mouse handler - this means if a pen user puts down the pen and
		// uses the mouse, we automatically drop back to reading the mouse when we fail to
		// recieve pen packets.
		if (ReadTabletPressureData())
			return;
	}

	switch(CurrentPressureMode)
	{
		case PressureMode_None:
			PenPressure = PressureMax;
			break;

		case PressureMode_Pen:
			// We want to read values from a real pen, but that is unavailable,
			// so we drop through to fake something up based on mouse speed.
			// NOBREAK

			// 12/8/2000 This has changed, we no longer want to fake pressure at all
			PenPressure = PressureMax;
			break;
		case PressureMode_Speed:
			// We've got a fake pen. We use mouse travel speed to generate pressure info
			if (Message == WM_MOUSEMOVE)
			{
				// Diccon - disabled mouse speed pressure for now
				
				PenPressure = PressureMax / 2 ;// - (INT32)(Speed * (double)PressureMax);
//				PenPressure = (INT32)(Speed * (double)PressureMax);
			}
			else
				PenPressure = 0;		// Must be mouse down. Init pressure to 0
			break;


		case PressureMode_Direction:
			// We've got a fake pen. We use mouse travel direction to generate pressure info
			if (Message == WM_MOUSEMOVE)
			{
				// Work out direction of travel to do a calligraphic pen
				NormCoord TravelDir(point.x - LastMousePoint.x, point.y - LastMousePoint.y);

				if (TravelDir.x != 0.0 && TravelDir.y != 0.0)
				{
					TravelDir.Normalise();

					NormCoord PenAngle(1.0, 1.0);
					PenAngle.Normalise();

					// Take the dot product of the travel direction and the brush angle (which gives
					// us cos(angle between them)), which makes an excellent pressure (width) value
					// This actually gives us a value between -1 and +1, which we scale into 0..1 range
					double DotProd = TravelDir.DotProduct(PenAngle);
					DotProd = (DotProd + 1.0) / 2.0;

					PenPressure = (INT32) (DotProd * (double)PressureMax);
				}
			}
			else
				PenPressure = 0;		// Must be mouse down. Init pressure to 0
			break;
	}

	// And remember the last mouse position
	LastMousePoint = point;
#endif
}



/********************************************************************************************

>	void CCPen::SetPressureFromJoystick(WPARAM Buttons, LPARAM JoyPos)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Inputs:		The JoyPos as passed in from the Message Handler (in CCamView) 
	Outputs:	-
	Returns:	-
	Purpose:	Set the pretend Pen Pressure from the Joytick Position.

********************************************************************************************/

void CCPen::SetPressureFromJoystick(WPARAM Buttons, LPARAM JoyPos)
{
	PORTNOTETRACE("other","CCPen::SetPressureFromJoystick - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (this->IsRealPen())
		return;		// Ignore this call if we are a Real Pen

	if (!(Buttons & JOY_BUTTON1))
		return;		// Button1 must be down for the Pressure to be valid

	INT32 Ypos = HIWORD(JoyPos) - (JOYMAX/2);		// Get Y Offset from Joystick Centre

	// Convert the distance from the joystick centre pos, into a value between
	// 0 and PressureMax
	PenPressure = (Ypos<0 ? -Ypos : Ypos)*PressureMax/(JOYMAX/2);
#endif
}


/********************************************************************************************
						TOGGLE PRESSURE ON/OFF OPERATION
*/
/********************************************************************************************

>	OpTogglePressure::OpTogglePressure() : Operation()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpTogglePressure object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpTogglePressure::OpTogglePressure() : Operation()
{

}

/********************************************************************************************

>	OpTogglePressure::~OpTogglePressure()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructs an OpTogglePressure object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpTogglePressure::~OpTogglePressure()
{
	// Empty
}



/********************************************************************************************

>	void OpTogglePressure::Do(OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/93
	Inputs:		Pointer to Operation Descriptor
	Outputs:	-
	Returns:	-
	Purpose:	Actually "DO" a TogglePressure operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpTogglePressure::Do(OpDescriptor*)
{
	PORTNOTETRACE("other","CCPen::Do - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	CCPen *pPen = Camelot.GetPressurePen();
	if (pPen != NULL)
	{
		PressureMode NewState = (pPen->IsPressureOn()) ? PressureMode_None : PressureMode_Pen;
		pPen->SetPressureMode(NewState);
	}
#endif

	End();
}



/********************************************************************************************

>	OpState OpTogglePressure::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Inputs:		Pointer to Operation Descriptor
				Text Description
	Outputs:	-
	Returns:	-
	Purpose:	Find the state of the OpTogglePressure operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpTogglePressure::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState Bob;
	PORTNOTETRACE("other","CCPen::GetState - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	Bob.Ticked = (Camelot.GetPressurePen())->IsPressureOn();
	Bob.Greyed = FALSE;
#endif
	return(Bob);
}



/********************************************************************************************

>	BOOL OpTogglePressure::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Create an OpDescriptor for the TogglePressure operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpTogglePressure::Init()	
{
PORTNOTE("other","Removed init stuff")
#ifndef EXCLUDE_FROM_XARALX
	return(Operation::RegisterOpDescriptor(
											0,
											_R(IDS_TOGGLE_PRESSURE),
											CC_RUNTIME_CLASS(OpTogglePressure), 
											OPTOKEN_TOGGLEPRESSURE,
											OpTogglePressure::GetState,
											_R(IDS_FREEHANDPRESSURE),		// Help string ID
											_R(IDBBL_FREEHANDPRESSURE),		// Bubble help ID
											_R(IDD_FREEHANDTOOL),			// resource ID
											_R(IDC_FREEHANDPRESSURE),		// control ID
											SYSTEMBAR_EDIT,				// Bar ID
											TRUE,						// Recieve system messages
											FALSE,						// No smart duplicate operation
											TRUE,						// Clean operation
											NULL,						// No vertical counterpart
											NULL,						// String for one copy only error
											0							// Auto state flags
										));
#endif
	return false;
}


