// $Id: pen.h 662 2006-03-14 21:31:49Z alex $
/// $Header: /wxCamelot/wxOil/pen.h 5     5/08/05 18:57 Luke $
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

// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
#ifdef WEBSTER
#define	INC_PEN
#endif // WEBSTER

#ifndef INC_PEN
#define	INC_PEN

#include "ops.h"

#define MAXPRESSURE 1023
#define JOYMAX 65536

typedef enum
{
	PressureMode_None,			// Pressure not being recorded (always returns max pressure)
	PressureMode_Pen,			// Pressure is recorded from a real pressure pen
	PressureMode_Speed,			// Pressure is simulated from mouse speed
	PressureMode_Direction,		// Pressure is simulated from mouse direction

	PressureMode_MaxEnum		// Placeholder so we know how many modes there are
} PressureMode;


/********************************************************************************************

>	class CCPen : public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/94
	Purpose:	This provides support for Pressure Sensitive Pen devices.
 				Its job is to keep track of the current pressure of a pen.
				It will automatically mutate itself into either a Real or Pretend pen,
				depending on what is availble when it is created.
				If it is a real pen, then it gets it's pressure from mouse messages.
				If it is a pretend pen, then the pressure is adjusted using either the
				keyboard (temporarily Keypad +/-) or by using a joystick.

********************************************************************************************/

class CCPen : public CCObject
{
	CC_DECLARE_DYNAMIC(CCPen);

public:
	CCPen();				// Don't construct one. Use GetApplication()->GetPressurePen() instead.
	~CCPen();

	// Initialisation function
	static CCPen *Init();

public:		// General Info about the Pen
 	BOOL IsRealPen()		{ return PressureAvailable; }
 	BOOL IsPressureOn()		{ return DefaultPressureMode != PressureMode_None; }
 	UINT32 GetPenPressure()	{ return PenPressure; }
	UINT32 GetPressureMax()	{ return PressureMax; }


public:		// Pen control interfaces
	// Switch the pressure on or off
	void SetPressureMode(PressureMode NewMode)	{ CurrentPressureMode = DefaultPressureMode = NewMode; }
	PressureMode GetPressureMode(void)			{ return(CurrentPressureMode); }

	// Function called by CCamView, when the Joystick moves
	void SetPressureFromJoystick(WPARAM, LPARAM);


public:		// Internal calls to update the pen when mouse events occur (see scrvw.cpp)
	virtual void CheckMouseMessage(UINT32 Message, wxPoint point);
			// Check for pen information whenever we get a mouse message

	virtual void StartStroke(void);
	virtual void EndStroke(void);
			// Called by pen-using tools when they start/end a drag operation
			// This allows the pen to be disabled while not in use, and also
			// lets us detect when the user swaps between their pen and mouse


protected:	// Upcalls to derived classes. If they don't handle this they should call the base class
	virtual BOOL ReadTabletPressureData(void);


protected:
	UINT32 PenPressure;				// Last "pressure" value we read/calculated
	UINT32 PressureMax;				// The maximum pressure value

	BOOL PF_IsPressureOn;			// Are we bothering with pressure data?

	BOOL PressureAvailable;			// Is this a real or pretend pen?
	PressureMode CurrentPressureMode;	// The type of pressure recording used for this stroke

PORTNOTE("other","Removed HWND usage")
#ifndef EXCLUDE_FROM_XARALX
	HWND hMainframeWnd;				// A handle to the camelot main frame window
#endif

	wxPoint LastMousePoint;			// Last recorded mouse position when faking pressure with mouse

public:
	static PressureMode DefaultPressureMode;
									// The type of pressure recording we'll do by default
};





/********************************************************************************************

>	class OpTogglePressure : public Operation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Purpose:	This class represents the TogglePressure operation.
				Creating an instance of this class and calling its "Do" function will toggle
				the state of the flag controlling whether Pressure Information is stored 
				within paths.

********************************************************************************************/

#define OPTOKEN_TOGGLEPRESSURE TEXT("TogglePressure")

class OpTogglePressure: public Operation
{
	CC_DECLARE_DYNCREATE( OpTogglePressure )
	
public:
	OpTogglePressure();											// Constructor
	~OpTogglePressure();										// Destructor

	static BOOL		Init();										// Register an OpDescriptor
	void			Do(OpDescriptor*);							// "Do" function
	static OpState	GetState(String_256*, OpDescriptor*);		// Read the state of an operation
};

#endif 	// INC_PEN
