// $Id: penedit.h 662 2006-03-14 21:31:49Z alex $
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
// The pen edit header file, providing operations used by the pen tool
// Created by Mike on 25/9/94


#ifndef INC_PENEDIT
#define INC_PENEDIT

#include "selop.h"
#include "stockcol.h"

// #include "pentool.h"

#define OPTOKEN_PENHANDLES _T("PenHandles")
#define OPTOKEN_PENCREATEINTERNAL _T("CreatePenPoint")
#define OPTOKEN_PENEDITINTERNAL _T("EditPenPoint")
#define OPTOKEN_PENCREATEPATH _T("CreatePenPath")
#define OPTOKEN_PENADDELEMENT _T("AddPenElement")
#define OPTOKEN_PENCLOSEPATH _T("ClosePenPath")

#define	OPTOKEN_ADDPATH _T("AddPath")
#define OPTOKEN_ADDNEWPATH _T("AddNewPath")
#define OPTOKEN_ADDPATHTOPATH _T("AddPathToPath")
#define OPTOKEN_CLOSEPATHWITHPATH _T("ClosePathWithPath")


// define some types to act as return parameters. These tell a calling
// do function what type of endop execute to call
enum ExecuteType {
	ExeNone,
	ExeInclusive,
	ExeExclusive
};


/********************************************************************************************

>	class HandleFlags

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94
	Purpose:	Used to determin what to do when rendering drag handles. ie which handles
				should be rendered and how they relate to the drag mouse coord.

********************************************************************************************/

class HandleFlags
{
public:
	HandleFlags(BOOL rTrackEnd = TRUE,
				BOOL rGhostEnd = TRUE,
				BOOL TrackPtMoves = TRUE,
				BOOL GhostPtMoves = TRUE,
				BOOL TrackPtSpins = FALSE,
				BOOL GhostPtSpins = FALSE);

	BYTE RenderTrackEnd  : 1; 
	BYTE RenderGhostEnd  : 1;
	BYTE TrackPointMoves : 1;
	BYTE GhostPointMoves : 1;
	BYTE TrackPointSpins : 1;
	BYTE GhostPointSpins : 1;
 	BYTE Spare1 : 1;
	BYTE Spare2 : 1;
};



/********************************************************************************************

>	class WobbleFlags

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94
	Purpose:	Used to determin what to wobble when recalculating curve control points
				during a drag. The flags affect which handles around a curve endpoint are
				repositioned.
	
********************************************************************************************/

class WobbleFlags
{
public:
	WobbleFlags(BOOL pSister  = FALSE,
				BOOL pBrother = TRUE,
				BOOL nBrother = FALSE,
				BOOL nSister  = FALSE);

	BYTE PrevSister	 : 1;
	BYTE PrevBrother : 1;
	BYTE NextBrother : 1;
	BYTE NextSister  : 1;
};


/********************************************************************************************

>	class ControlPts

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94
	Purpose:	

********************************************************************************************/

class ControlPts
{
	public:
		 ControlPts();
		~ControlPts();	

	public:
		DocCoord HndClick;
		DocCoord HndDrag;
		Spread*  pHndSpread;
};




/********************************************************************************************

>	class OpPenHandles : public Operation

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Purpose:	This operation simply provides functions for other operations to use.
				It is not intended to be used as an operation in its own right. Most
				useful pen classes are derived from this.
	
********************************************************************************************/

class  OpPenHandles : public Operation
{

CC_DECLARE_DYNCREATE(OpPenHandles)

public:
	OpPenHandles();
			   
protected:
	void 		RenderHandles();
	void		UpdateHandles( RenderRegion* );
	DocRect		GetHandlesRect();
	void		RecalcHandles(DocCoord, BOOL);
	void        SetDragHandles(HandleFlags, DocCoord, DocCoord, DocCoord, Spread* );
	void		ChangeTrackHandle(DocCoord, BOOL);
	DocCoord	GetTrackHandle() const;
	DocCoord	GetGhostHandle() const;
	DocCoord	GetMidHandle() const;
	DocCoord	CalcGhostEnd(DocCoord start, DocCoord end) const;
	void		RenderTestRect(	DocRect, Spread*, StockColour );
	BOOL		HasMouseMoved(DocCoord& OriginalPoint, DocCoord& PointerPos, double threshold=2);

	HandleFlags Hflags;					// set some flags to control the handle positions

	DocCoord	StartMousePos;			// first click position
	DocCoord	CurrentMousePos;		// follow the mouse around
	Spread*		StartSpread;			// first click spread

private:
	void		SetHandleEnds(DocCoord, DocCoord, DocCoord);

	DocCoord	TrackEnd;				// end that follows the mouse usually
	DocCoord	MidPoint;
	DocCoord	GhostEnd;				// other end


};



/********************************************************************************************

>	class OpPenDragBlobs : public OpPenHandles

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Purpose:	This op class is defined as a base class only. It provides common dragging
				functionality which is inherited by create and edit pen classes.

********************************************************************************************/

class OpPenDragBlobs : public OpPenHandles
{

CC_DECLARE_DYNCREATE(OpPenDragBlobs)

public:
	OpPenDragBlobs();
	BOOL 		 DoPenDragBlobs();
	virtual void DragPointerMove(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL bSolidDrag);
	virtual void DragFinished(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);
	virtual void RenderDragBlobs( DocRect, Spread* , BOOL bSolidDrag);
};



/********************************************************************************************

>	class OpPenCreateInternal : public OpPenDragBlobs

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Purpose:	This operation is started by the pen tool when it receives a click on the
				document with no previous internal or external state set. ie the pen tool
				is not holding a moveto element inside iteslf from a previous click and there
				are no selected endpoints to add to.

********************************************************************************************/

class OpPenCreateInternal : public OpPenDragBlobs
{

CC_DECLARE_DYNCREATE(OpPenCreateInternal)

public:
	OpPenCreateInternal();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void 			DoPenCreateInternal( DocCoord, Spread*, ControlPts*);
	virtual void 	DragFinished(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);

private:
	ControlPts*		pUserHandles;			// keep a pointer passed in by the caller

};


/********************************************************************************************

>	class OpPenEditInternal : public OpPenDragBlobs

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Purpose:	This operation is started by the pen tool when it receives a click over its
				internal moveto	point. The point will be converted into a dragto if a drag
				occurs.

********************************************************************************************/

class OpPenEditInternal : public OpPenDragBlobs
{

CC_DECLARE_DYNCREATE(OpPenEditInternal)

public:
	OpPenEditInternal();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			DoPenEditInternal(ControlPts*);
	virtual void 	DragFinished(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);

private:
	ControlPts*		pUserHandles;			// keep a pointer passed in by the caller

};



/********************************************************************************************

>	class OpPenEditPath : public OpPenHandles

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	This operation is started by the pen tool when it receives a click on a
				document and needs to create a new path. The idea is, it will drag edit
				a path until such time as the user lets go of the mouse.  and then add the
				path to the tree. The operation handles the internals of rendering the
				eor path.
				
********************************************************************************************/

class OpPenEditPath : public OpPenHandles
{

CC_DECLARE_DYNCREATE(OpPenEditPath)

public:
	OpPenEditPath();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	BOOL			DoPenDragPath();
	virtual void 	DragPointerMove(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL bSolidDrag);
	virtual void 	DragFinished(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);
	virtual void 	RenderDragBlobs(DocRect, Spread*, BOOL bSolidDrag);

protected:
	DocRect		GetBoundingRect();					// return the bounding rect of blobs and path
	void		ConvertToLine(const INT32 index);	// convert the edited curve element to a line
	void		RemoveRotateEnd(const INT32 index);
	BOOL		InsertBezier(DocCoord pt1, DocCoord pt2, DocCoord pt3);
	BOOL    	InsertCurvedLine(DocCoord pt1, DocCoord pt2);
	BOOL		InsertGhostedBezier(DocCoord pt1, DocCoord pt2, DocCoord pt3);
	BOOL		ConvertPathEnd(Path* pDocPath);
	void    	SetWobbleIndex(WobbleFlags wibble, const INT32 index);
	void        WobbleCoords();

	Path*		pEditPath;				// keep track of edit path
	INT32		WobbleIndex;			// index of element being dragged
	WobbleFlags Wobble;					// which controls on the edit curve do we wobble?
	
};


/********************************************************************************************

>	class OpPenCreatePath : public OpPenEditPath

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	This op is concerned with creating a new path in the tree. It is derived
				from OpPenEditPath, which handles all the dragging and redraw of a new 
				path element. This op will use
	
********************************************************************************************/

class OpPenCreatePath : public OpPenEditPath
{

CC_DECLARE_DYNCREATE(OpPenCreatePath)

public:
	OpPenCreatePath();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void 			DoPenCreatePath(ControlPts*, DocCoord, Spread*, Path*);
	virtual void 	DragFinished(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);

private:
	DocCoord	Handle0;		// control handles of previous path element.
	DocCoord	Handle1;

};


/********************************************************************************************

>	class OpPenAddElement : public OpPenEditPath

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	Given a path in the tree with a selected end point, this operation will
				control the creation of a new element (through clicks and drags), at the end
				of the path. It is derived from OpPenEditPath which controls all the eoring
				of path elements such as blobs and lines.

********************************************************************************************/

class OpPenAddElement : public OpPenEditPath
{

CC_DECLARE_DYNCREATE(OpPenAddElement)

public:
	OpPenAddElement();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void 			DoPenAddElement(NodePath*, INT32, DocCoord, Spread*, Path*);
	virtual void 	DragFinished(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);

private:
	NodePath*	pEditNode;		// keep track of the node we are editing in the document
	INT32		EditIndex;
};



/********************************************************************************************

>	class OpPenClosePath : public OpPenEditPath

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Purpose:	This op controls the closing of an open path. It performs the eoring of path
				elements a blobs during a click or drag action.
				It is primarily used by	the pen tool an is brought into being when a click
				occurs over the startpoint of a path, having a selected endpoint.

********************************************************************************************/

class OpPenClosePath : public OpPenEditPath
{

CC_DECLARE_DYNCREATE(OpPenClosePath)

public:
	OpPenClosePath();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);
	void 			DoPenClosePath(NodePath*, INT32,  Spread*, Path*);
	virtual void 	DragPointerMove(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL bSolidDrag);
	virtual void 	DragFinished(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL Success, BOOL bSolidDrag);
	
private:
	BOOL			AddFirstElement(Spread* pSpread);

	NodePath*		pEditNode;		// keep track of the node we are editing in the document
	INT32			EditIndex;
	BOOL			OnMoveto;

	INT32			NumElements;
	BOOL 			FirstDrag;
	WobbleFlags 	FirstWFlags;
	HandleFlags 	FirstHFlags;

};


/********************************************************************************************

>	class OpAddPath : public SelOperation

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Purpose:	The base class for various addpath operations. This is not really an 
				operation in its own right as it does not provide an init and getstate
				function. Its use is to provide common functions required by distinct
				addpath operations.

********************************************************************************************/

class OpAddPath : public SelOperation
{

CC_DECLARE_DYNCREATE(OpAddPath)

public:
	OpAddPath();

protected:
	ExecuteType	AugmentPathWithPath(Path*, NodePath*, INT32);
	ActionCode 	DeselectPoint(NodePath* pDestNode, INT32 position);
	ActionCode 	DeselectHandle(NodePath* pDestNode, INT32 position);

};



/********************************************************************************************

>	class OpAddNewPath : public OpAddPath

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	This op actually adds a new path to the current document. The path has
				been created and edited with OpPenCreatePath() which will handle EOR drawing
				It can now be instantiated as a physical object by this op.

********************************************************************************************/

class OpAddNewPath : public OpAddPath
{

CC_DECLARE_DYNCREATE(OpAddNewPath)

public:
	OpAddNewPath();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);
	void 			GetOpName(String_256* OpName);
	void 			DoAddNewPath(Path* pAddPath, Spread* pSpread);
};


/********************************************************************************************

>	class OpAddPathToPath : public OpAddPath

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/94
	Purpose:	This op will add an element to an existing path within the tree. The new
				element and context path should be passed into the do function as parameters.
	
********************************************************************************************/

class OpAddPathToPath : public OpAddPath
{

CC_DECLARE_DYNCREATE(OpAddPathToPath)

public:
	OpAddPathToPath();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);
	void 			GetOpName(String_256* OpName);
	void 			DoAddPathToPath(NodePath* pDestinNode, Path* pAddPath, INT32 index);

};


/********************************************************************************************

>	class OpClosePathWithPath : public OpAddPath

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Purpose:	This op deals with a destination path being edited, and a source path which
				may be the result of a drag edit operation. 
				The idea is that the op will take the components of the source path and add
				them in a deterministic way	to the destination path, resulting in a closed
				destination path.
				The source path is assumed to have either 1 or 2 elements in it. The first
				element will be added to the end of the destination path and act as a
				closing piece. The second element of the source path will affect the
				first element in the destination path. If both elements match in type, ie
				are two curves, then the none end control handles of the source will be 
				copied to the destination.
				This op as can be seen is rather specific, and is used by the pen tool to
				close an open path.	Imagine clicking on the first control point of a path
				and draging out control handles. The handles will affect the previous and
				next elements of the path. The previous being a close element and the next
				being the first element of the path. Hence the need for this strange op.
	
********************************************************************************************/

class OpClosePathWithPath : public OpAddPath
{

CC_DECLARE_DYNCREATE(OpClosePathWithPath)

public:
	OpClosePathWithPath();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);
	void 			GetOpName(String_256* OpName);
	void 			DoClosePathWithPath(NodePath* pDestinNode, Path* pAddPath, INT32 index);

};



#endif
