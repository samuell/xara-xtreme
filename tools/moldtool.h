// $Id: moldtool.h 670 2006-03-15 13:00:45Z alex $
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
// Header for the mould tool implementation

#ifndef INC_MOULDTOOL
#define INC_MOULDTOOL

#include "tool.h"
#include "doccoord.h"
#include "bars.h"
#include "nodemold.h"
#include "moldedit.h"
#include "ops.h"
#include "dragtool.h"

class Cursor;
class Spread;
class MouldInfoBarOp;
class CCAttrMap;

#define OPTOKEN_CREATENEWMOULD			_T("CreateMould")
#define OPTOKEN_REMOVEMOULD				_T("RemoveMould")
#define	OPTOKEN_COPYMOULD 				_T("CopyMould")
#define OPTOKEN_PASTEENVELOPE			_T("PasteEnvelope")
#define OPTOKEN_PASTEPERSPECTIVE		_T("PastePerspective")
#define OPTOKEN_TOGGLEMOULDGRID			_T("ToggleMouldGrid")
#define OPTOKEN_DETACHMOULD				_T("DetachMould")
#define OPTOKEN_ROTATEMOULD				_T("RotateMould")
#define OPTOKEN_RECTANGULARENVELOPE		_T("DefaultEnvelope")
#define OPTOKEN_RECTANGULARPERSPECTIVE	_T("DefaultPerspective")

typedef enum EnvelopeType {
	ENV_DEFAULT,
	ENV_CIRCULAR,
	ENV_ELLIPTICAL,
	ENV_CONCAVE,
	ENV_BANNER
};

typedef enum PerspectiveType {
	PER_DEFAULT,
	PER_LEFT,
	PER_RIGHT,
	PER_FLOOR,
	PER_ROOF
};


/********************************************************************************************

>	class MouldTool : public Tool_v1

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/94
	Purpose:	A template tool class

********************************************************************************************/

class CCAPI MouldTool : public DragTool
{
// Give tool name in memory dumps
CC_DECLARE_MEMDUMP(MouldTool);

public:
	 MouldTool();
	~MouldTool();

	// Standard calls your tool must provide
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_MOULD; };		// defined in kernel\tool.h
	
	// Some very common event handlers
	virtual void SelectChange(BOOL isSelected);
	virtual void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	virtual void OnMouseMove(DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMods);
	virtual void RenderToolBlobs(Spread* pSpread,DocRect* pDocRect);
	virtual BOOL GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord coord, ClickModifiers mods);

	// Some useful static functions
	static	MouldInfoBarOp* GetMouldInfoBarOp()  { return pMouldInfoBarOp; }
	static	BOOL 			IsCurrentTool() 	 { return CurrentTool; }
	static	void 			DisplayStatusBarHelp(UINT32 StatusID);

	void 	CreateEnvelope(EnvelopeType EnvType);
	void 	CreateEnvelope2x2(EnvelopeType EnvType);
	void 	CreatePerspective(PerspectiveType PerType);
	void	PasteMould(MouldSpace m_space);

private:
	BOOL 	CreateCursors();						// Create  your tool's cursors in this func
	void 	DestroyCursors();						// Destroy your tool's cursors in this func
	void 	ChangeCursor(Cursor* cursor);			// Change the cursor shape
	Cursor* ChooseCursor(INT32);
	void 	GenerateStatusLineText(INT32 msgres, String_256* pStatusMsg);
	BOOL 	DetermineClickEffect(DocCoord PointerPos, Spread* pSpread, ClickModifiers ClickMods, INT32* ctype, INT32* msgres);

	static 	BOOL 			CurrentTool;		// Can be useful to keep a "is my tool current?" flag
	static 	MouldInfoBarOp*	pMouldInfoBarOp;	// Ptr to your tool's infobar

	Cursor*	pcNormalMouldCursor;				// Your standard cursor to use when your tool becomes active
	Cursor*	pcCurrentCursor;					// The cursor your tool is currently displaying
	Cursor* pcTransOriginCursor;				// The translate perspective vanishing point cursor
	Cursor* pcTransCoordCursor;					// the translate mould path coordinates cursor
	INT32	CurrentCursorID;					// ID of the current cursor on the stack

	// Standard tool static vars
	static 	TCHAR* FamilyName;					// The Tools Family Name
	static 	TCHAR* ToolName;						// The Tool Name
	static 	TCHAR* Purpose;						// What the tool is for
	static 	TCHAR* Author;						// Who wrote it
};




/********************************************************************************************

>	class MouldInfoBarOp : public InformationBarOp

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Purpose:	Class for handling the tool's information bar.

********************************************************************************************/

class MouldInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( MouldInfoBarOp )  

public:
	MouldInfoBarOp();
	MsgResult Message(Msg* Msg);						// All messages to the info bar come through here
	static	INT32 SelInfo(NodeMould**);
	virtual void UpdateState();	
			void UpdateDefEnvButton(BOOL Selection);
			void UpdateDefPerButton(BOOL Selection);
			void UpdateDetachButton(BOOL Selection);
			BOOL CheckSelection();
			void SetTool(MouldTool*);
			void SetDetachState(BOOL state);
			BOOL GetDetachState() const { return DetachState; }
	
private:
	BOOL		DetachState;
	BOOL		GridState;
	MouldTool*	pMouldTool;								// Pointer to instance of mould tool

	// DMc
	// returns TRUE for the selection can be moulded, false otherwise
	// tests for any nodes with NeedParent set
	BOOL CanMouldSelection();
};
	

/********************************************************************************************

>	class MouldInfoBarOpCreate : public BarCreate

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Purpose:	Class for creating MouldInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class MouldInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp* Create() { return (new MouldInfoBarOp); }
};

/***********************************************************************************************

>	class CreateMouldParam : public OpParam

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	The OpParam to use when Invoke()ing the CreateNewMould operation

***********************************************************************************************/

class CreateMouldParam : public OpParam
{
	 CC_DECLARE_MEMDUMP(CreateMouldParam)	

	public:
		CreateMouldParam(Path* pPath, MouldSpace mSpace, BOOL FitObjects, BOOL Lock) : OpParam(0, 0) \
			{ ParamShape = pPath; 
			  ParamSpace = mSpace;
			  FitSelection = FitObjects;
			  LockAspect = Lock;
			}

		Path*		ParamShape;
		MouldSpace	ParamSpace;
		BOOL		FitSelection;
		BOOL		LockAspect;
};




/*******************************************************************************************

>	class OpCreateNewMould: public OpMouldLibSel

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Creates a new mould object and all its various children to form a valid
				mould structure. The operation will execute only when there is one or more
				selected objects to work on. These selected objects will to all intense and
				purpose dissappear from the tree. They will move from their current positions
				and move inside the new mould node. 

********************************************************************************************/

class OpCreateNewMould: public OpMouldLibSel
{
	CC_DECLARE_DYNCREATE( OpCreateNewMould ) 
	
	public:
		OpCreateNewMould();
										
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void 			GetOpName(String_256* OpName);
		void			DoWithParam(OpDescriptor*, OpParam*);

	private:
		BOOL AllMouldObjects(List* pNodeList);

	private:
		MouldSpace m_space;

};



/********************************************************************************************

>	class OpRemoveMould : public OpMouldLibSel

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	An undoable operation to remove the mould shape from a mould object

********************************************************************************************/

class OpRemoveMould: public OpMouldLibSel
{
	CC_DECLARE_DYNCREATE( OpRemoveMould )    

	public:
		OpRemoveMould();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void 			GetOpName(String_256* OpName);
		void			Do(OpDescriptor*);	
};


/********************************************************************************************

>	class OpCopyMouldShape : public OpMouldLibSel

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	An undoable operation to copy the mould shape to the clipboard
	SeeAlso:	-

********************************************************************************************/

class OpCopyMouldShape: public OpMouldLibSel
{
	CC_DECLARE_DYNCREATE( OpCopyMouldShape )    

	public:
		OpCopyMouldShape();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void 			GetOpName(String_256* OpName);
		void			Do(OpDescriptor*);	
};


/********************************************************************************************

>	class OpPasteMouldShape : public OpMouldLibSel

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	An undoable operation to paste the contents of the clipboard as a mould
				shape.
	SeeAlso:	

********************************************************************************************/

class OpPasteMouldShape: public OpMouldLibSel
{
	CC_DECLARE_DYNCREATE( OpPasteMouldShape )    

	public:
		OpPasteMouldShape();								

	protected:
		static BOOL FindPasteObject(Node** pReturnNode);
		void		Do(MouldSpace mSpace);	
};



/********************************************************************************************

>	class OpPasteEnvelope : public OpPasteMouldShape

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	An undoable operation to paste the contents of the clipboard as a mould
				shape.
	SeeAlso:	

********************************************************************************************/

class OpPasteEnvelope: public OpPasteMouldShape
{
	CC_DECLARE_DYNCREATE( OpPasteEnvelope )    

	public:
		OpPasteEnvelope();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void 			GetOpName(String_256* OpName);
		void			Do(OpDescriptor*);	
};



/********************************************************************************************

>	class OpPastePerspective : public OpPasteMouldShape

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	An undoable operation to paste the contents of the clipboard as a mould
				shape.
	SeeAlso:	

********************************************************************************************/

class OpPastePerspective: public OpPasteMouldShape
{
	CC_DECLARE_DYNCREATE( OpPastePerspective )    

	public:
		OpPastePerspective();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void 			GetOpName(String_256* OpName);
		void			Do(OpDescriptor*);	
};


/********************************************************************************************

>	class OpToggleMouldGrid : public Operation

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	An operation to turn a particular moulds grid on and off
	SeeAlso:	-

********************************************************************************************/

class OpToggleMouldGrid: public Operation
{
	CC_DECLARE_DYNCREATE( OpToggleMouldGrid )    

	public:
		OpToggleMouldGrid();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void			Do(OpDescriptor*);	
};


/********************************************************************************************

>	class OpDetachMould : public UndoableOperation

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	An operation to attach and detach a mould manifold from its object
				This allows the manifold to be edited on its own without the moulded objects
				following. Handy for quick edits.
	SeeAlso:	-

********************************************************************************************/

class OpDetachMould: public UndoableOperation
{
	CC_DECLARE_DYNCREATE( OpDetachMould )    

	public:
		OpDetachMould();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void 			GetOpName(String_256* OpName);
		void			Do(OpDescriptor*);
};

/********************************************************************************************

>	class OpRoateMould : public OpMouldLib

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	An operation to rotate a set of objects within a mould manifold.
				The op simply changes the start point of a mould path object by rotating
				it around one logical element. The result is simply to spin the orientation
				of the moulded objects within the mould shape.
	SeeAlso:	-

********************************************************************************************/

class OpRotateMould: public OpMouldLib
{
	CC_DECLARE_DYNCREATE( OpRotateMould )

	public:
		OpRotateMould();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void			Do(OpDescriptor*);	
		void 			GetOpName(String_256* OpName);
};



/***********************************************************************************************

>	class MouldBecomeA: public BecomeA

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/95
	Purpose:	This is the class that is passed to other nodes when the mould tool is trying
				to turn them into other types via their DoBecomeA() method

***********************************************************************************************/

class MouldBecomeA : public BecomeA
{
	CC_DECLARE_MEMDUMP(MouldBecomeA);

	public:
		virtual ~MouldBecomeA();

		MouldBecomeA(BecomeAReason Reason,
					 CCRuntimeClass* pClass,
					 UndoableOperation* pOp) : BecomeA(Reason,pClass,pOp) {pNodePath=NULL;};

		// This function should be called when Reason == BECOMEA_PASSBACK 
		virtual BOOL PassBack(	NodeRenderableInk* pNewNode, 
								NodeRenderableInk* pCreatedByNode,
								CCAttrMap* pAttrMap);

		NodePath* GetMouldPath() const { return pNodePath; }
	
	protected:
		void Destroy();

	private:
		NodePath* pNodePath;

};


/********************************************************************************************

>	class OpRectangularEnvelope : public OpMouldLibSel

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95
	Purpose:	Simple operations to handle the creation of default envelope and perspective
				shapes.
	SeeAlso:	-

********************************************************************************************/

class OpRectangularEnvelope: public OpMouldLibSel
{
	CC_DECLARE_DYNCREATE( OpRectangularEnvelope )

	public:
		OpRectangularEnvelope();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void			Do(OpDescriptor*);	
		void 			GetOpName(String_256* OpName);
};

/********************************************************************************************

>	class OpRectangularPerspective : public OpMouldLibSel

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95
	Purpose:	Simple operations to handle the creation of default envelope and perspective
				shapes.
	SeeAlso:	-

********************************************************************************************/

class OpRectangularPerspective: public OpMouldLibSel
{
	CC_DECLARE_DYNCREATE( OpRectangularPerspective )

	public:
		OpRectangularPerspective();								
		static BOOL		Init();				
		static OpState	GetState(String_256*, OpDescriptor*);		
		void			Do(OpDescriptor*);	
		void 			GetOpName(String_256* OpName);
};


#endif 		// INC_MOULDTOOL
