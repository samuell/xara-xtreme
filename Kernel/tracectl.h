// $Id: tracectl.h 662 2006-03-14 21:31:49Z alex $
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
// Trace dialog

#ifndef INC_TRACECTL
#define INC_TRACECTL

#include "bfxbase.h"

class KernelBitmap;
class TraceRegion;
class LinearFillAttribute;
class NodePath;
class Node;
class KernelStatistics;				
class BfxErrorRegionList;
class BfxPixelOp;

#define NUM_TRACECTL_DEBUG_BITMAPS 7

enum TraceMethod { 	TRACEMETHOD_MONO = 0,
					TRACEMETHOD_GREYSCALE,
					TRACEMETHOD_256COL,
					TRACEMETHOD_TRUECOL,

					// the end
					TRACEMETHOD_ILLEGAL,
					NUM_TRACEMETHOD=TRACEMETHOD_ILLEGAL};


/********************************************************************************************

>	class TraceControl : public CCObject

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Dialog for tracer

********************************************************************************************/

class TraceControl : public BitmapEffectBase
{
	// Give my name in memory dumps
	CC_DECLARE_DYNCREATE(TraceControl);

public:
	// Construction etc
	TraceControl();
	virtual ~TraceControl();

	virtual BOOL InitBitmap(KernelBitmap * theOriginal = NULL);
	virtual BOOL GetBitmaps( KernelBitmap * * theOriginal = NULL,
							 KernelBitmap * * theProposed = NULL,
							 KernelBitmap * * theUndithered = NULL	);

	virtual BOOL Trace(BOOL * Done);

	virtual BOOL SetPath(NodePath * pNodePath);
	virtual Node * GetPaths();
	virtual BOOL RemoveTree(BOOL DeleteIt = TRUE);

	virtual BOOL GetParams(
		TraceMethod * pMethod = NULL,
		INT32 * pMinimumArea = NULL,
		double * pMaximumInitialAreaErrorThreshold = NULL,
		double * pMinimumInitialAreaErrorThreshold = NULL,
		double * pInitialAreaErrorRatio = NULL,
		double * pMinGradFillError = NULL,
		double * pGradFillErrorRatio = NULL,
		double * pMinPixelError = NULL,
		double * pMaxPixelError = NULL,
		double * pPixelErrorRatio = NULL,
		double * pQuantColours = NULL,
		double * pBlur = NULL,
		double * pSmooth = NULL
		);

	virtual BOOL SetParams(
		TraceMethod * pMethod = NULL,
		INT32 * pMinimumArea = NULL,
		double * pMaximumInitialAreaErrorThreshold = NULL,
		double * pMinimumInitialAreaErrorThreshold = NULL,
		double * pInitialAreaErrorRatio = NULL,
		double * pMinGradFillError = NULL,
		double * pGradFillErrorRatio = NULL,
		double * pMinPixelError = NULL,
		double * pMaxPixelError = NULL,
		double * pPixelErrorRatio = NULL,
		double * pQuantColours = NULL,
		double * pBlur = NULL,
		double * pSmooth = NULL
);

	virtual BOOL GetProgress(INT32 * pPercent = NULL,
							 INT32 * pNumPaths = NULL,
							 INT32 * pNumCusps = NULL,
							 INT32 * pNumPoints = NULL,
							 BOOL * pProgressDone = NULL);
	
protected:

	virtual BOOL MarkInitialArea();
//	virtual BOOL MarkPositiveArea();

//	virtual BOOL FindInitialPoint();

	virtual BOOL TracePath(BfxPixelOp * pBfxPixelOp);
	virtual BOOL CalculateStatistics();
	virtual BOOL FormGradFill();

	virtual BOOL PlotGradFill(BOOL ToProposed, BOOL UsePath=TRUE);

//	virtual BOOL SwapTempAndProposed();

	virtual BOOL CheckMinimumArea(BfxPixelOp * pBfxPixelOp);

	KernelBitmap * pOriginal;
	KernelBitmap * pProposed;
	KernelBitmap * pUndithered;

#ifdef _DEBUG
public:
	KernelBitmap * pDebug[NUM_TRACECTL_DEBUG_BITMAPS];
	BOOL DebugSwitch[NUM_TRACECTL_DEBUG_BITMAPS];
	BOOL DebugInvalid[NUM_TRACECTL_DEBUG_BITMAPS];
protected:
#endif

	NodePath * pCurrentPath;
	LinearFillAttribute * pFill;

	Node * pTree;
	Node * pJoinNode;

	TraceRegion * pTraceRegion;
	BfxErrorRegionList * pErrorRegionList;

	KernelStatistics * pStats;

	BfxPixelOp * pThresholdPixelOp;
	BfxPixelOp * pColourThresholdPixelOp;
	BfxPixelOp * pPositivePixelOp;

	double InitialAreaErrorThreshold;
	double MaximumInitialAreaErrorThreshold;
	double MinimumInitialAreaErrorThreshold;
	double InitialAreaErrorRatio;

	double GradFillError;
	double NewGradFillError;
	double MinGradFillError;
	double GradFillErrorRatio;

	double PixelError;
	double MinPixelError;
	double MaxPixelError;
	double PixelErrorRatio;
	double QuantColours;

	double Blur;
	double Smooth;

	TraceMethod Method;
	INT32 Log2BPP;

	INT32 MinimumArea;

	// For progrss indicator only
	INT32 TotalPasses;
	INT32 NumPasses;
	INT32 NumPaths;
	INT32 NumCusps;
	INT32 NumPoints;
	BOOL ProgressDone;

	BOOL HavePath;
	BOOL FirstShape;
	BOOL ShapeFreePass;

	BOOL FoundRegion;
	INT32 InitialX;
	INT32 InitialY;
	INT32 RegionArea;

	INT32 ScanPointX;
	INT32 ScanPointY;
	DWORD ScanPointColour;
	
	INT32 XSize;
	INT32 YSize;

	INT32 GFStartX;
	INT32 GFStartY;
	INT32 GFEndX;
	INT32 GFEndY;
	DWORD GFStartC;
	DWORD GFEndC;

};

#endif  // INC_TRACECTL


