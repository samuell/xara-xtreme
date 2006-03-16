// $Id: aligndlg.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_ALIGNDLG
#define INC_ALIGNDLG

#include "rnddlgs.h" 
#include "transop.h" 

#define OPTOKEN_ALIGNDLG _T("ArrangeAlignment")    
#define OPTOKEN_OPALIGN  _T("OpAlign")    

// enumeration of all possible alignments on each axis
enum AlignType
{
	AlignNone		=0,
	AlignLow		=1,		// ie left or bottom
	AlignCentre		=2,
	AlignHigh		=3,		// ie right or top
	DistributeLow	=4,		// ie left or bottom
	DistributeCentre=5,
	DistributeHigh	=6,		// ie right or top
	DistributeEqui	=7
};	

// enumeration of possible target areas
enum TargetType
{
	ToSelection=0,
	ToPage     =1,
	ToSpread   =2
};	

// struct for passing params to OpAlign::DoWithParam()
struct AlignParam
{
	AlignType  h;
	AlignType  v;
	TargetType target;
};

// struct to cache x or y info about object to be affected 
struct ObjInfo
{
	INT32 i;		// cross-ref to pre-sorted state
	INT32 lo;	// left(bottom) coord of object
	INT32 hi;	// right(top)   coord of object
};

// struct to hold 1 dimensional data (1D equivelent of Rect!)
struct LineData
{
	INT32 lo;	// start position of line (x or y)
	INT32 hi;	// end   position of line (x or y)
};

// number of rects in diagram on dialog
const INT32 DiagRects=4;

/****************************************************************************
>	class ArrangeAlignment: public DialogOp

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/94
	Purpose:	Align object dialog
****************************************************************************/

class ArrangeAlignment: public DialogOp
{         
	CC_DECLARE_DYNCREATE( ArrangeAlignment )  
public:
	ArrangeAlignment();	
	void				Do(OpDescriptor*);	     
	static  BOOL		Init();                        
	static  OpState		GetState(String_256*, OpDescriptor*);	
 	static  void 		CalcDiagramRectsOneAxis(
 							LineData x[8][DiagRects],
 							const INT32 width[DiagRects],
 							const INT32 order[DiagRects],
 							const INT32 gap[DiagRects-1],
 							const INT32 DiagWidth
 						);
	virtual	MsgResult	Message(Msg* Message);

	// positions of 4 diagram rectangles in all 8 alignments (x and y independent)
	static LineData DiagRectX[8][DiagRects];
	static LineData DiagRectY[8][DiagRects];

	static const	UINT32 IDD;     
	static const	CDlgMode Mode;

private:
	void	  UpdateState();
	void	  DialogOKed();
	void      RedrawDiagram(ReDrawInfoType* ExtraInfo);
	void      DiagramClicked(ReDrawInfoType* ExtraInfo);
	void	  BuildIDSDropList(const CGadgetID DropListID, const INT32* IDSList, INT32 Default);
	void	  SetRadioGroup( const CGadgetID* IDCList, const CGadgetID IDC);
	CGadgetID ReadRadioGroup(const CGadgetID* IDCList, const CGadgetID IDCDefault=NULL);

	static AlignParam Align;
}; 

/****************************************************************************
>	class OpAlign: public SelOperation

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/94
	Purpose:	Undo-able Op to Align objects
****************************************************************************/

class OpAlign: public TransOperation
{         
	CC_DECLARE_DYNCREATE(OpAlign)

public:
	OpAlign();	
	void	DoWithParam(OpDescriptor* pOp, OpParam* pAlignParam);	     
	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);

private:
	void AlignOneAxis(AlignType Align, INT32 NumObjs, XLONG SumObjWidths,
							   INT32 SelRectLow, INT32 SelRectHigh, ObjInfo* x, INT32* dx);
}; 

#endif
