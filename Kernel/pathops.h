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
// More path releated operations - pathedit.cpp is getting big

#ifndef INC_PATHOPS
#define INC_PATHOPS

#define OPTOKEN_MOVEPATHPOINT	_T("MovePathPoint")
#define OPTOKEN_MAKELINESOP		_T("MakeLines")
#define OPTOKEN_MAKECURVESOP	_T("MakeCurves")
#define OPTOKEN_SELECTALLPATHPOINTS _T("SelectAllPathPoints")
#define OPTOKEN_DESELECTALLPATHPOINTS _T("DeSelectAllPathPoints")

#include "opdesc.h"
#include "transop.h"
#include "selop.h"
#include "msg.h"


/********************************************************************************************

>	class OpBaseConvertPathSegment : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/95
	Purpose:	This is the base class for the two operations that convert curve segments into
				line segments and vice-versa.
	SeeAlso:	OpMakeSegmentsCurves, OpMakeSegmentsLines

********************************************************************************************/

class OpBaseConvertPathSegment: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpBaseConvertPathSegment )    

public:
	OpBaseConvertPathSegment();
	void Do(OpDescriptor*);
	static OpState BaseGetState(INT32 PathType);

protected:
	virtual INT32 GetProcessPathType() { ERROR3("Dont call the base class"); return 0;}
	virtual INT32 GetPathType() { ERROR3("Dont call the base class"); return 0;}
	virtual BOOL ProcessSegment(NodePath* pPath, INT32* Index, INT32 PrevIndex) { ERROR3("Dont call the base class"); return 0;}
};  




/********************************************************************************************

>	class OpMakeSegmentsLines : public OpBaseConvertPathSegment

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	This class will change all the selected segments in all selected paths from
				curves into lines (ignoring segments that are already lines!).
	SeeAlso:	OpMakeSegmentsCurves

********************************************************************************************/

class OpMakeSegmentsLines: public OpBaseConvertPathSegment
{         
	CC_DECLARE_DYNCREATE( OpMakeSegmentsLines )    

public:
	OpMakeSegmentsLines();								
	static BOOL	Init();
	static OpState GetState(String_256*, OpDescriptor*);		

protected:
	virtual INT32 GetProcessPathType() {return PT_BEZIERTO;}
	virtual INT32 GetPathType() {return PT_LINETO;}
	BOOL ProcessSegment(NodePath* pPath, INT32* Index, INT32 PrevIndex);
};



 /********************************************************************************************

>	class OpMakeSegmentsCurves : public OpBaseConvertPathSegment

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	This class will change all the selected segments in all selected paths from
				lines into curves (ignoring segments that are already curves!).
	SeeAlso:	OpMakeSegmentsLines

********************************************************************************************/

class OpMakeSegmentsCurves: public OpBaseConvertPathSegment
{         
	CC_DECLARE_DYNCREATE( OpMakeSegmentsCurves )    

public:
	OpMakeSegmentsCurves();								
	static BOOL	Init();				
	static OpState GetState(String_256*, OpDescriptor*);		
	static BOOL CarryOut(INT32, INT32, NodePath*, UndoableOperation*, ActionList* pActions);

protected:
	virtual INT32 GetProcessPathType() {return PT_LINETO;}
	virtual INT32 GetPathType() {return PT_BEZIERTO;}
	virtual BOOL ProcessSegment(NodePath* pPath, INT32* Index, INT32 PrevIndex);
};  



/***********************************************************************************************

>	class MovePointsParams : public OpParam

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	The OpParam to use when calling OpMovePathPoints.  Used to pass in the path to
				alter and which coords to change.
	SeeAlso:	OpMovePathPoint::DoWithParam

***********************************************************************************************/

struct ElementCoord
{
	INT32		Element;
	DocCoord	Coordinate;
};

class MovePointsParams : public OpParam
{
	CC_DECLARE_MEMDUMP(MovePointsParams)	

public:
	MovePointsParams(NodePath* pPath, ElementCoord* ChangeData, INT32 NumChanges);

	NodePath		*PathToEdit;
	ElementCoord	*PathChanges;
	INT32				ChangesCount;
};


/********************************************************************************************

>	class OpMovePathPoint : public TransOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Purpose:	This operation allows the exact positioning of elements in a path.

********************************************************************************************/

class OpMovePathPoint: public TransOperation
{         
	CC_DECLARE_DYNCREATE( OpMovePathPoint )    

public:
	OpMovePathPoint();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			DoWithParam(OpDescriptor*, OpParam*);

private:
	BOOL ClearSmoothAndRotate(NodePath* pPath, INT32 PathIndex);
};  



/*********************************************************************************************
>	class NewPathCreatedMsg : public Msg

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Purpose:	This message is sent whenever a new path is created.  The only client of this
				message (at the moment) is the Line tool, which needs to clear it's floating
				endpoint when a new path is created.
	SeeAlso:	OpNewPath, BezierTool

*********************************************************************************************/

class NewPathCreatedMsg : public Msg
{
	CC_DECLARE_DYNAMIC(NewPathCreatedMsg)

public:
	NodePath*	NewPath;				// Points to the new path	
	Operation*	CurrentOp;				// Points to the active operation
	ActionList*	UndoActs;				// Points to the operations undo list

	NewPathCreatedMsg(NodePath* pPath, Operation* CurOp, ActionList* Undos);
};



/*********************************************************************************************
>	class PathEditedMsg : public Msg

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	This message is sent whenever a single control point or an endpoint on a path
				is edited.  A client of this message is the Line tool, which uses the data in
				this message to update the edit fields in its infobar.
	SeeAlso:	OpEditControlPoint, OpEditEndPoint, BezierTool

*********************************************************************************************/

class PathEditedMsg : public Msg
{
	CC_DECLARE_DYNAMIC(PathEditedMsg)

public:
	Path*		EditPath;				// Points to the edited path	
	Spread*		EditSpread;				// The spread containing the path
	INT32		EndPoint;				// The edited endpoint OR the endpoint next to the
										// edited control point

	PathEditedMsg(Path* pPath, Spread* pSpread, INT32 Index);
};



/********************************************************************************************
>	class OpMenuSelectPathPoints : public Operation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/95
	Purpose:	This operation is the base of select/deselect all endpoints in a path
********************************************************************************************/

class OpMenuSelectPathPoints: public Operation
{         
	CC_DECLARE_DYNCREATE( OpMenuSelectPathPoints )    

public:
	OpMenuSelectPathPoints() {};								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);	

protected:
	BOOL DoAction(BOOL SelectPoints);	
};  



/********************************************************************************************
>	class OpSelectAllPathPoints : public OpMenuSelectPathPoints

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/95
	Purpose:	This operation is used to select all endpoints in a path
********************************************************************************************/

class OpSelectAllPathPoints: public OpMenuSelectPathPoints
{         
	CC_DECLARE_DYNCREATE( OpSelectAllPathPoints )    

public:
	OpSelectAllPathPoints() {};								
	void Do(OpDescriptor*);
};  



/********************************************************************************************
>	class OpDeSelectAllPathPoints : public OpMenuSelectPathPoints

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/95
	Purpose:	This operation is used to deselect all endpoints in a path
********************************************************************************************/

class OpDeSelectAllPathPoints: public OpMenuSelectPathPoints
{         
	CC_DECLARE_DYNCREATE( OpDeSelectAllPathPoints )    

public:
	OpDeSelectAllPathPoints() {};								
	void Do(OpDescriptor*);
};



/********************************************************************************************

>	class OpChangeFillProfile : public SelOperation

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Purpose:	The operation class that is responsible for changing fill profiles.
	SeeAlso:	-

********************************************************************************************/

#define OPTOKEN_REVERSEPATH	_T("ReversePath")				// our all important optoken!

class OpReversePath : public SelOperation
{
	CC_DECLARE_DYNCREATE (OpReversePath)

public:
	OpReversePath ();
	~OpReversePath ();
	
	static BOOL	Init();
	static OpState GetState (String_256* Description, OpDescriptor*);

	void Do (OpDescriptor*);
};



#endif  // INC_PATHOPS
