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
#include "bfxtest.h"

//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// for test
#include "bitmpinf.h"
#include "bfxalu.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(TestBitmapEffect, BitmapEffect)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BOOL TestBitmapEffect::Run()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		a value
	Outputs:	none
	Returns:	nothing
	Purpose:	Internal test routine
	Errors:		none
	SeeAlso:	-

A test routine

********************************************************************************************/
 
BOOL TestBitmapEffect::Run()
{

/*
	INT32 x, y, i, j;
	
	fixed16 conv[3][3];
	fixed16 convsum;

	BitmapInfo BMInfo;
	Source->ActualBitmap->GetInfo(&BMInfo);

	conv[0][0]=(fixed16)	2.0;	conv[0][1] =(fixed16)	2.0;	conv[0][2]=(fixed16)	2.0;
	conv[1][0]=(fixed16)	2.0;	conv[1][1] =(fixed16)	2.0;	conv[1][2]=(fixed16)	2.0;
	conv[2][0]=(fixed16)	2.0;	conv[2][1] =(fixed16)	2.0;	conv[2][2]=(fixed16)	2.0;

	convsum=(fixed16) 0.0;

	for (i=0; i<3; i++) for(j=0; j<3; j++) convsum+=conv[i][j];
	if (convsum!=(fixed16)0) for (i=0; i<3; i++) for(j=0; j<3; j++) conv[i][j]=conv[i][j]/convsum;

	for (y=0; y<(INT32) BMInfo.PixelHeight; y++) for (x=0; x<(INT32) BMInfo.PixelWidth; x++)
	{
		fixed16 r=0.5; //0.5 for final rounding
		fixed16 g=0.5;
		fixed16 b=0.5;
		UINT32 v;
		for (i=0; i<3; i++) for(j=0; j<3; j++)
		{
			v=Source->ReadPixel( (i+x-1<0?0:(i+x-1>=(INT32)BMInfo.PixelWidth?BMInfo.PixelWidth-1:i+x-1)),
								 (j+y-1<0?0:(j+y-1>=(INT32)BMInfo.PixelHeight?BMInfo.PixelHeight-1:j+y-1)));								 
			r+=conv[i][j]*(fixed16)((INT32)(v & 0xFF));
			g+=conv[i][j]*(fixed16)((INT32)((v & 0xFF00)>>8));
			b+=conv[i][j]*(fixed16)((INT32)((v & 0xFF0000)>>16));
		}
		if (convsum==(fixed16)0)
		{
			r+=128;
			g+=128;
			b+=128;
		}
		if (r<0) r=0;
		if (g<0) g=0;
		if (b<0) b=0;
		if (r>255) r=255;
		if (g>255) g=255;
		if (b>255) b=255;
		Destination->PlotPixel(x,y,(r.MakeLong())+((g.MakeLong())<<8)+((b.MakeLong())<<16) );
	}
	
	return TRUE;
*/
	TCHAR buf[256];
	
	clock_t Timer = clock();											// start clock

	INT32 i,j;
	INT32 ITER;
	BOOL dummybool=TRUE;
	BOOL ShiftPressed;
	KernelBitmap * Temp;
	
	ShiftPressed = ((GetAsyncKeyState( CAMKEY(SHIFT) ) & 0x8000 )!=0);


	if (ShiftPressed)
	{
		ITER=1;
	}
	else
	{
		ITER=1;
	}
	
	
//BfxALULUT LUT;
//LUT.LinearABK(0.09375,0.90625,0);
//Timer = clock() - Timer;											// stop clock
	
	for (i=1; i<=ITER; i++)
	{
		if ((Temp = ALU->NewBitmap(Source)) &&
			ALU->SetA(Temp) &&
			ALU->ZeroA() &&
			ALU->SetB(Source) &&
			ALU->SetT((DWORD) 0x00) &&
			ALU->PartTAB() &&
			TRUE)
		{	
			for (j=1; j<=10; j++)
				dummybool=(
		
			ALU->SetA(Destination) &&
			ALU->ZeroA() &&
		
			ALU->SetB(Temp) &&
			ALU->AddKAB(0x80) &&

			ALU->ZeroA() &&
		
			ALU->SetB(Temp) &&
			ALU->AddKAB(0x80) &&

			ALU->SetB(Temp,0,1) &&
			ALU->AddKAB(0x18) &&	

			ALU->SetB(Temp,0,-1) &&
			ALU->AddKAB(0x18) &&	
		
			ALU->SetB(Temp,1,0) &&
			ALU->AddKAB(0x18) &&	
		
			ALU->SetB(Temp,-1,0) &&
			ALU->AddKAB(0x18) &&

			ALU->SetB(Temp,1,1) &&
			ALU->AddKAB(0x08) &&	
		
			ALU->SetB(Temp,-1,1) &&
			ALU->AddKAB(0x08) &&	
		
			ALU->SetB(Temp,1,-1) &&
			ALU->AddKAB(0x08) &&	
		
			ALU->SetB(Temp,-1,-1) &&
			ALU->AddKAB(0x08) &&

			ALU->SetA(Temp) &&
			ALU->SetB(Destination) &&
			ALU->PartTAB() &&
		
			TRUE);

			delete Temp;
		}
	}
	Timer = clock() - Timer;											// stop clock

//ALU->SetA(Destination);
//ALU->SetB(Source,-20,20);
//ALU->PlotBLUT(&LUT);

	// as this is for our use only, it is permitted to hard-code English strings

	// this lovely line translates the clock_t value into a useful value without using
	// floats
	wsprintf( buf, "Done %d iterations. Each took %d.%03d secs",	ITER, (INT32)(Timer / (CLOCKS_PER_SEC*ITER)),
								(INT32)((Timer % (CLOCKS_PER_SEC*ITER)) * 1000 / (CLOCKS_PER_SEC*ITER)) );

	if (ShiftPressed) AfxMessageBox( buf );

	return dummybool;
	}
