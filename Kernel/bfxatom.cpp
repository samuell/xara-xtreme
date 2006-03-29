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
#include "errors.h"
#include "bfxatom.h"

#include "bitmap.h"

// for test
#include "app.h"
#include "range.h"
#include "node.h"
#include "nodebmp.h"
#include "bitmpinf.h"
#include "chapter.h"
#include "spread.h"
#include "page.h"
#include "bfxtest.h"
#include "bfxalu.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(BitmapEffectAtom, BitmapEffectBase)
CC_IMPLEMENT_DYNCREATE(BitmapEffectSILT, BitmapEffectAtom)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

// Static inits
INT32 BitmapEffectAtom::TestValue=0;


/********************************************************************************************

>	BOOL BitmapEffectSILT::Run()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		none
	Outputs:	runs the SILT effect
	Returns:	TRUE if OK, FALSE (& error set) if not
	Purpose:	Internal test routine
	Errors:		Many possible (level 1,2,3)
	Scope:		Public
	SeeAlso:	-

Runs the SILT bitmap

********************************************************************************************/

#define DEBUGSILT 0

BOOL BitmapEffectSILT::Run()
{
	KernelBitmap * Temp;
	INT32 Stage;
	double mDest,cDest,mAdd,cAdd,mCurrent,cCurrent;
	BfxALULUT * LUT;
	BfxALULUT * LUT2;
	BYTE CurrentZero, CurrentOne, AddZero, AddOne; // Byte values of areas of solid black and white

#define Stages 8
#define TestSize 5
double Coefficient[Stages+1]=
// {-0.0946028, -1.00875, -2.06186, 5.24511, 10.9885, -12.2837, -19.5609, 12.3327, 15.0588}; //straight butterworth
//{-0.153367, -0.538629, 0.288733, 0.543937, -3.11505, -1.00089, 6.76564, 4.81082, 0.0150588}; //butterworth+end pt norm
{-0.153426, -0.538158, 0.291086, 0.539231, -3.12917, -0.989594, 6.792, 4.80329, 0};//like above but real norm
//{2.12494, 1.2666, -0.917123, -0.599367, -0.062313, -0.0511883, -0.447152, -0.294161, 0.};//sharpen
double t;

#if DEBUGSILT
double TestVars[TestSize];
double RealVars[TestSize];
double ScaleVars[TestSize];
BYTE TestByte[256];
BYTE DestByte[256];
INT32 v;
#endif

#if DEBUGSILT
	for (v=0; v<TestSize; v++) TestVars[v]=RealVars[v]=ScaleVars[v]=(((double)v) / (double)(TestSize-1));

	for (v=0; v<=255; v++) TestByte[v]=DestByte[v]=v;
#endif

	t=0;
	
	for (Stage=0; Stage<=Stages; Stage++)
		t+=(Coefficient[Stage]);//= ((double)rand())/100.0-163.0);
	
	for (Stage=0; Stage<=Stages; Stage++) Coefficient[Stage]=Coefficient[Stage]*1.0/t;
	
#if DEBUGSILT
	TRACEUSER( "Alex", _T("Coefficients "));
	for (Stage=0; Stage<=Stages; Stage++)
		TRACEUSER( "Alex", _T("%d=%5f "),Stage,Coefficient[Stage]);
	TRACEUSER( "Alex", _T("\n"));	
#endif

	if (!(LUT = new BfxALULUT)) return FALSE;
	if (!(LUT2 = new BfxALULUT))
	{
		delete LUT;
		return FALSE;
	}

	if (!(Temp = ALU->NewBitmap(Destination)))
	{
		delete LUT;
		delete LUT2;
		return FALSE;
	}
				 
	if (!(ALU->SetA(Temp) &&
		  ALU->ZeroA() &&
		  ALU->SetA(Destination) &&
		  ALU->ZeroA()))
	{
		delete LUT;
		delete LUT2;
		delete Temp;
		return FALSE;
	}


	// Initialise the fixed resolution parameters
	// realDest = mDest.Dest + cDest;

	Stage=Stages;

	if (Coefficient[Stage]>=0)
	{
		mAdd = Coefficient[Stage];
		cAdd = 0;
		if (!(ALU->SetB(Source) &&
		  ALU->PlotB()
	   	   ))
		{
			delete LUT;
			delete LUT2;
			delete Temp;
			return FALSE;
		}
		CurrentZero=0;
		CurrentOne=255;

#if DEBUGSILT
		for (v=0; v<TestSize; v++)
		{
			RealVars[v]=RealVars[v]*Coefficient[Stage];
			ScaleVars[v]=ScaleVars[v];
		}
		for (v=0; v<=255; v++)
		{
			DestByte[v]=TestByte[v];
		}
#endif
		
	}
	else
	{
		mAdd = -Coefficient[Stage];
		cAdd = -mAdd;
		if (!(ALU->SetB(Source) &&
		  LUT->LinearABK(0,-1,1) &&
		  ALU->PlotBLUT(LUT)
	   	   ))
		{
			delete LUT;
			delete LUT2;
			delete Temp;
			return FALSE;
		}
		CurrentZero=255;
		CurrentOne=1;

#if DEBUGSILT
		for (v=0; v<TestSize; v++)
		{
			RealVars[v]=RealVars[v]*Coefficient[Stage];
			ScaleVars[v]=1.0-ScaleVars[v];
		}
		for (v=0; v<=255; v++)
		{
			DestByte[v]=LUT->GetLUT(0,TestByte[v]);
		}
#endif
	
	}

	mDest=mAdd;
	cDest=cAdd;

#if DEBUGSILT
	TRACEUSER( "Alex", _T("Stage %d "),Stage);
	for (v=0; v<TestSize; v++)
	{
		TRACEUSER( "Alex", _T("%5f|%5f|%5f "),RealVars[v],ScaleVars[v]*mDest+cDest,ScaleVars[v]);
	}
	TRACEUSER( "Alex", _T(" .. %5f,%5f,%5f |  %5f,%5f\n"),0.0,0.0,0.0,mDest,cDest);

	TRACEUSER( "Alex", _T("          "));
	for (v=0; v<=10; v++) TRACEUSER( "Alex", _T("%2x~%2x "),v,DestByte[v]);
	for (v=245; v<=255; v++) TRACEUSER( "Alex", _T("%2x~%2x "),v,DestByte[v]);
	TRACEUSER( "Alex", _T("\n"));
#endif

	if (!(LUT2->LinearABK(2.0,-0.5,0)))
	{
 		delete LUT;
		delete LUT2;
		delete Temp;
		return FALSE;
	}

	for (Stage = Stages-1; Stage>=0; Stage--)
	{
#if 1
		if (!(
			ALU->SetA(Temp) &&

/*
			ALU->SetB(Destination) &&
			ALU->PlotB() &&

			ALU->SetA(Destination) &&
			ALU->SetB(Temp) &&
			ALU->PlotB() &&
*/
			ALU->ZeroA() &&
			
//			ALU->SetB(Destination) &&
//			ALU->AddKAB(0x80) &&

			ALU->SetB(Destination,0,1) &&
			ALU->AddKAB(0x20) &&	

			ALU->SetB(Destination,0,-1) &&
			ALU->AddKAB(0x20) &&	
		
			ALU->SetB(Destination,1,0) &&
			ALU->AddKAB(0x20) &&	
		
			ALU->SetB(Destination,-1,0) &&
			ALU->AddKAB(0x20) &&

			ALU->SetB(Destination,1,1) &&
			ALU->AddKAB(0x10) &&	
		
			ALU->SetB(Destination,-1,1) &&
			ALU->AddKAB(0x10) &&	
		
			ALU->SetB(Destination,1,-1) &&
			ALU->AddKAB(0x10) &&	
		
			ALU->SetB(Destination,-1,-1) &&
			ALU->AddKAB(0x10) &&

			ALU->SetB(Destination) &&
			ALU->PlotBLUT(LUT2) &&


			ALU->SetA(Destination) &&
			ALU->SetB(Temp) &&
			ALU->PlotB() &&
	  		
			TRUE))
		{
			delete LUT;
			delete LUT2;
			delete Temp;
			return FALSE;
		}
#endif	  	
		mAdd = 1;
		cAdd = 0;

		mCurrent = mDest;
		cCurrent = cDest;
		AddZero=0;
		AddOne=255;

		// Calculate cDest and mDest in the worse case situation. This finds the maximum range that an ouput
		// pixel may lie in (i.e. from cDest .. cDest+mDest).

		/*	Here's the maths:
			                                                               
		  	We wish to represent x (a set which may have arbitrary magnitude) as x' where 0<=x'<=1 and two
			reals cx, mx (mx>0) so
					 1
				x' = __	[ x - cx]                x = mx.x' + cx              (1)
					 mx

			We thus chose
				cx = min [x];                    mx = max[x] - min[x] > 0    (2)

			Now let us assume we have other similarly represented sets y, and z, where y is known, and
			z is unknown (as yet) but
				
				z = alpha . x + beta . y         (wouldn't this be a lot easier with mathgreek?)

			We know y', cy, my obviously, but we want z', mz' and cz', and more importantly how to do
			the addition in the dash ("'") transformed space.

			Let us consider:
				z= (alpha.mx)x' + (beta.my)y' + (cx.alpha) + (cy.beta)		 (3)

			To find cz, mz we need to consider the extremites of the range of z as in (2)

			
			             |             cz               |              mz            
			-------------|------------------------------|----------------------------
			             |                              |                            
			alpha>=0     | cx.alpha + cy.beta           |  (alpha) mx + (beta) my                         
			beta>=0      |                              |                            
			             |                              |                            
			-------------|------------------------------|----------------------------
			             |                              |                            
			alpha<0      | cx.alpha + cy.beta           |  (-alpha) mx + (beta) my                    
			beta>=0      |   + alpha.mx                 |                            
			             |                              |                            
			-------------|------------------------------|----------------------------
			             |                              |                            
			alpha<0      | cx.alpha + cy.beta           |  (-alpha) mx + (-beta) my                          
			beta<0       |   + alpha.mx + beta.my       |                            
			             |                              |                            
			-------------|------------------------------|----------------------------
			             |                              |                            
			alpha>=0     | cx.alpha + cy.beta           |  (alpha) mx + (-beta) my                          
			beta<0       |   + beta.my                  |                            
			             |                              |                            
			-------------|------------------------------|----------------------------
			

		Note that in our current use alpha is always 1 which simplifies the equations and
		means we only need to consider two case. Beta corresponds to Coefficient[stage].

		Note we can actually use *any* valid value of cz, mz. The values above are those
		guaranteed to give the smallest output range which includes all possibilities (i.e.
		all combinations of values from the input domains).

		The *full* range isn't really very useful. Of course there are going to be some pixels which will
		clip. Take (for example) the case of a sharpen on a full black to full white edge. Allowing full range
		through all the time with our fixed resolution of 255 sampling points will reduce the apparent bpp
		for solid colour areas (which are unaffected by the unit gain SILTs we are considering here). Thus
		we also work out the range of values obtainable in a sitatuion where x=y.

		We do this by maintaining two BYTE values, CurrentZero and CurrentOne (the top & bottom of the range).
		These are what bytes areas of solid black and white would be represented by in the incoming bitmap.
		These come straight out of the LUT calculations. AddZero & AddOne do the same for the incoming
		bitmap.


		The output range will run between two numbers:
			LimitZ = ((CurrentZ * mx) + cx) * alpha + ((AddZ * my) + cy) * beta
		where Z is "Zero" or "One". But is LimitZero < LimitOne or vice-versa?

		LimitOne-LimitZero = (CurrentOne-CurrentZero)*mx*alpha+(AddOne-AddZero)*my*beta


		In order to formulate z', we note

					 1
				x' = __	[ z - cz]                                            (4)
					 mz

		And substitute (3) to give

					 alpha.mx	  beta.my	  cx . alpha + cy.beta - cz
				z' = -------- x + ------- y + -------------------------      (4)
					    mz			mz					 mz
					|________|	  |______|	  |_______________________|
					   PropA		PropB				Offset

		*/

		if (Coefficient[Stage]>=0) // beta, alpha is 1
		{
			cDest = cCurrent + cAdd * Coefficient[Stage];
			mDest = mCurrent + mAdd * Coefficient[Stage];
		}
		else
		{
			cDest = cCurrent + (cAdd+mAdd) * Coefficient[Stage];
			mDest = mCurrent - mAdd * Coefficient[Stage];
		}

		double mDestIso;
		double cDestIso;
		double LimitZero;
		double LimitOne;


		// The *full* range isn't really very useful. Of course there are going to be some pixels which will
		// clip. Take (for example) the case of a sharpen on a full black to full white edge. Allowing full range
		// through all the time with our fixed resolution of 255 sampling points will reduce the apparent bpp
		// for solid colour areas (which are unaffected by the unit gain SILTs we are considering here). Thus
		// we also work out the range of values obtainable in a sitatuion where x=y.

		LimitZero=((((double)CurrentZero)/255.0*mCurrent)+cCurrent)+cAdd*Coefficient[Stage];
		LimitOne=((((double)CurrentOne)/255.0*mCurrent)+cCurrent)+((1.0*mAdd)+cAdd)*Coefficient[Stage];
		if (LimitOne>LimitZero)
		{
			cDestIso=LimitZero;
			mDestIso=LimitOne-LimitZero;
		}
		else
		{
			cDestIso=LimitOne;
			mDestIso=LimitZero-LimitOne;
		}
			

   		// Now bodge the ranges together - note this isn't really a bodge, just that it should be calculated
		// accoridng to the dynamics of the image (complicated).
		//
		// Note the two forms of errors:
		// * If we use too much 'full range' stuff we will introduce errors at higher and higher bits in the
		//   image. This occurs because when the 0..1 range is compressed, 2 grey levels become equal at 8bpp.
		//   If we then subtract 0.1 x (original) which should still give an increasing image, if the rounding
		//   turns bad, we get a downward step not an upward step.
		//
		// * If we use too little 'full range' stuff, we will not see details near the full range which are
		//   brought out by the processing.
		//
		// Note always mDest=>mDestIso, cDest<=cDestIso, & cDest<=cDestIso<=cDestIso+mDestIso<=cDest+mDest
		//
		// The algorithm currently used is to chose an interpolation factor depending on the magnitude of
		// mDest compared to mDestIso, and interpolate between two range limited versions.
		//
		// STOP PRESS: Currently we're just using the IsoRange. Methinks I'm going to have to resort to the
		// mixture for different FIRs
#if 0
		const double HighestIsoRange =9999.0;
		const double HighestRange = 4.0;
		double interp;

		interp = 0.75 + 0.25 * (mDestIso/mDest);
		// Limit the two ranges

		if (mDest<0.5/255.0) mDest = 0.5/255.0;
		if (mDestIso<0.5/255.0) mDestIso = 0.5/255.0;

		if (mDestIso>HighestIsoRange)
		{
			cDestIso+=(mDestIso-HighestIsoRange)/2.0;
			mDestIso=HighestIsoRange;
		}

		if (mDest>HighestRange)
		{
			cDest+=(mDest-HighestRange)/2.0;
			mDest=HighestRange;
		}

		mDest = (1-interp) * mDest + interp * mDestIso;
		cDest = (1-interp) * cDest + interp * cDestIso;
#endif

mDest=mDestIso;
cDest=cDestIso;

		// To reduce the possiblity of rounding errors, we ensure that 0 corresponds to an integer grey level
		// and that the gamut is an integer number of grey levels wide. This process should be guaranteed to
		// ensure that black stays as black, and white stays as white.
		double oldcDest = cDest;
		cDest=(floor(cDest * 255.0))/255.0;
		mDest = (ceil((oldcDest + mDest)*255.0))/255.0 - cDest;
			   
		double PropA;
		double PropB;
		double Offset;


		PropA = mCurrent / mDest;
		PropB = Coefficient[Stage] * mAdd / mDest;	
		Offset = (cCurrent+cAdd * Coefficient[Stage]-cDest)/ mDest;
	
#if DEBUGSILT
		for (v=0; v<TestSize; v++)
		{
			RealVars[v]+=Coefficient[Stage]*TestVars[v];
			ScaleVars[v]=PropA*ScaleVars[v]+PropB*TestVars[v]+Offset;
		}
#endif
		
		if (!(LUT->LinearABK(PropA, PropB, Offset) &&
			 ALU->SetA(Destination) &&
			 ALU->SetB(Source) &&
			 ALU->PlotBLUT(LUT) &&
			 TRUE))
		{
			delete LUT;
			delete LUT2;
			delete Temp;
			return FALSE;
		}

		CurrentZero=LUT->GetLUT(CurrentZero,0);
		CurrentOne=LUT->GetLUT(CurrentOne,255);

#if DEBUGSILT
		for (v=0; v<=255; v++)
		{
			DestByte[v]=LUT->GetLUT(DestByte[v],TestByte[v]);
		}


		TRACEUSER( "Alex", _T("Stage %d "),Stage);
		for (v=0; v<TestSize; v++)
		{
			TRACEUSER( "Alex", _T("%5f|%5f|%5f "),RealVars[v],ScaleVars[v]*mDest+cDest,ScaleVars[v]);
		}
		TRACEUSER( "Alex", _T(" .. %5f,%5f,%5f |  %5f,%5f\n"),PropA,PropB,Offset,mDest,cDest);

		TRACEUSER( "Alex", _T("          "));
		for (v=0; v<=10; v++) TRACEUSER( "Alex", _T("%2x~%2x "),v,DestByte[v]);
		for (v=245; v<=255; v++) TRACEUSER( "Alex", _T("%2x~%2x "),v,DestByte[v]);
		TRACEUSER( "Alex", _T("\n"));
#endif


	}

	if (!(LUT->LinearABK(mDest, 0, cDest) // Accumulator already set up
		 && ALU->PlotBLUT(LUT)))
	{
		delete LUT;
		delete LUT2;
		delete Temp;
		return FALSE;
	}

#if DEBUGSILT
	for (v=0; v<=255; v++)
	{
		DestByte[v]=LUT->GetLUT(DestByte[v],TestByte[v]);
	}

	TRACEUSER( "Alex", _T("End       "));
	for (v=0; v<=10; v++) TRACEUSER( "Alex", _T("%2x~%2x "),v,DestByte[v]);
	for (v=245; v<=255; v++) TRACEUSER( "Alex", _T("%2x~%2x "),v,DestByte[v]);
	TRACEUSER( "Alex", _T("\n"));
#endif

	delete LUT;
	delete LUT2;
	delete Temp;
	return(TRUE);
		
}
	


/********************************************************************************************

>	void BitmapEffectAtom::Test(INT32 Value)
					
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

void BitmapEffectAtom::Test(INT32 Value)
{
	TRACEUSER( "Alex", _T("Value is %d\n"),Value);
	// Obtain the current selections
	TestValue=Value;
}

void BitmapEffectAtom::Test2(UndoableOperation * Op)
{
	 
	BOOL CarryOn=TRUE;
	Range Sel(*(GetApplication()->FindSelection()));

	Node* FirstSelectedNode = Sel.FindFirst(); 
	if (FirstSelectedNode != NULL) // No nodes selected so End
	{
	    Node* CurrentNode = FirstSelectedNode;       
	    Node* NextCurrent; 
	    
	    // Do all bitmaps
		while ((CurrentNode != NULL) && CarryOn)
		{
			NextCurrent = Sel.FindNext(CurrentNode);
	       	if  ( (CurrentNode->IsSelected()) && (CurrentNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeBitmap)) ) 
	       	{         
				KernelBitmap * pBitmap = ((NodeBitmap *)(CurrentNode))->GetBitmap();
				BitmapInfo BMInfo;
				UINT32 bpp;
				pBitmap->ActualBitmap->GetInfo(&BMInfo);
				bpp=BMInfo.PixelDepth;
		   		
  				TRACEUSER( "Alex", _T("Bitmap found %d bpp\n"),bpp);

				if ((bpp==32) || TRUE)
				{
					CarryOn = FALSE;
					NodeBitmap *pNodeBitmap = new NodeBitmap;
					if ((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12)))
						return;

					Spread *pSpread;
					DocCoord Origin;

					// For now, position Draw objects on 1st page of spread 1
					Node *pNode = (Document::GetSelected())->GetFirstNode()->FindNext()->FindFirstChild();
					while ((pNode != NULL) && (!pNode->IsKindOf(CC_RUNTIME_CLASS(Chapter))))
						pNode = pNode->FindNext();
		
					ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(Chapter)), 
						   "Filter::GetFirstSpread(): Could not find Chapter");
					Chapter *pChapter = (Chapter *) pNode;
	
					// pSpread is a child of pChapter
					pSpread = (Spread *) pChapter->FindFirstChild();
					ENSURE(pSpread->IsKindOf(CC_RUNTIME_CLASS(Spread)),
						   "Filter::GetFirstSpread(): Could not find Spread");

					Page *pPage = (Page *) pSpread->FindFirstPageInSpread();
					ENSURE(pPage->IsKindOf(CC_RUNTIME_CLASS(Page)),
						   "BaseBitmapFilter::DoImport(): Could not find first Page");
		
					// Use bottom left of page as origin
					DocRect PageRect = pPage->GetPageRect();
					Origin = PageRect.lo;
	
					KernelBitmap* kb = new KernelBitmap(BMInfo.PixelWidth,BMInfo.PixelHeight,32,100);
					// Get a new bitmap object for this node.
					pNodeBitmap->GetBitmapRef()->Attach(kb);
					if (pNodeBitmap->GetBitmap() != kb)
					{
						// It didn't use the bitmap we gave it, so we can delete it
						delete kb;
					}
						
					ENSURE(pNodeBitmap->GetBitmap()->ActualBitmap != NULL, "No bitmap object found!");
					
					// Import worked - try to add the bitmap object into the tree.
					// First, set the rectangle to the right size for the bitmap...
					BitmapInfo Info;
					pNodeBitmap->GetBitmap()->ActualBitmap->GetInfo(&Info);

					DocRect BoundsRect;
					BoundsRect.lo = Origin;
					BoundsRect.hi.x = BoundsRect.lo.x + Info.RecommendedWidth;
					BoundsRect.hi.y = BoundsRect.lo.y + Info.RecommendedHeight;

					// And set this in our bitmap node
					pNodeBitmap->CreateShape(BoundsRect);

					// Apply some default attrs for the bitmap
					// This Must be done before the NodeBitmap is inserted into the tree
					if (!pNodeBitmap->ApplyDefaultBitmapAttrs(Op))
					{
						return;
					}
					
					// Insert the node, but don't invalidate its region
					if (!Op->DoInsertNewNode(pNodeBitmap, pSpread, FALSE))
					{
						// It didn't work - delete the sub-tree we just created, and report error.
						delete pNodeBitmap;
						return;
					}
									
					// Invalidate the region
					Op->DoInvalidateNodeRegion(pNodeBitmap, TRUE, FALSE);

					// Right, now we're going to do some processing...
					BitmapEffectBase * pEffect;

					pEffect=new /*TestBitmapEffect*/ BitmapEffectSILT;

					pEffect->SetParameters();
					pEffect->SetSourceImage(pBitmap);
					pEffect->UseDestinationBitmap(pNodeBitmap->GetBitmap());

					pEffect->Run();

					pEffect->UseDestinationBitmap(NULL);

					delete(pEffect);
				}
	       	}
			CurrentNode = NextCurrent; 
		}

	} 

//	if (CarryOn) BitmapEffectSILT::RunA();

	return;
}
