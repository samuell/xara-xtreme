// $Id: opbevel.h 671 2006-03-15 14:30:01Z alex $
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
// Header for the Bevel tool operations

#ifndef INC_OPBevel
#define INC_OPBevel

#include "nodebev.h"
#include "nbevcont.h"

#if BUILDSHADOWS

//#include "selop.h"

#define OPTOKEN_CREATEBEVEL	_T("CreateBevel")
#define OPTOKEN_REMOVEBEVEL	_T("RemoveBevel")
#define OPTOKEN_CHANGEBEVEL	_T("ChangeBevel")
#define OPTOKEN_CHANGEATTRIBUTE _T("ChangeAttribute")
#define OPTOKEN_SELECTBEVEL _T("SelectBevel")

class BevelInfo;


/********************************************************************************************
>	class BevelTools

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/99
	Purpose:	Useful static functions for bevel manipulations
********************************************************************************************/

class BevelTools
{
public:
	// builds a list of all selected nodes of given class
	static BOOL BuildListOfSelectedNodes(List *pList,
								const CCRuntimeClass * pClass,
								BOOL bPromoteToParents = TRUE);

	// recursion down to find nodes
	static BOOL GetAllNodesUnderNode(const Node * pNode, List * pList,
		const CCRuntimeClass * pClass, INT32 depth = 0);

	// whether any of the child nodes of this node are selected
	static BOOL HasSelectedChild(NodeRenderableInk * pNode);

private:
	static BOOL IsNodeInList(List * pList, const Node * pNode);
};


/********************************************************************************************
>	class OpCreateBevel : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Create a Bevel object in the tree giving the current selection a Bevel.
********************************************************************************************/
class OpCreateBevel : public SelOperation
{

CC_DECLARE_DYNCREATE(OpCreateBevel)

public:
	// Construction/Destruction
	OpCreateBevel();
	~OpCreateBevel();

	virtual void Do(OpDescriptor *pOpDesc);
	virtual void DoWithParam(OpDescriptor* pOp, OpParam* pParam);

//	BOOL GenerateBitmapShapes(NodeBevel* pCurrent, SelRange SelectionRng);

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	void GetOpName(String_256*);

	// sjk 26/09/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return TRUE; }

private:
	void ApplyBevelToNode(Node * pNode, BevelInfo* pParam);
	
	// converts all nodes (recursively) into paths
	void ConvertToPaths(Node * pNode);

	NodeAttribute * DoDepthSearchForFill(Node * pStart);

	// the resource ID of the name of the op
	UINT32 m_NameID;
};

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpRemoveBevel class
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************
>	class OpRemoveBevel : public SelOperation

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Purpose:	Remove a Bevel object 
********************************************************************************************/
class OpRemoveBevel : public SelOperation
{

CC_DECLARE_DYNCREATE(OpRemoveBevel)

public:
	// Construction/Destruction
	OpRemoveBevel();
	~OpRemoveBevel();

	virtual void Do(OpDescriptor *pOpDesc);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	void GetOpName(String_256*);

	// sjk 26/09/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return TRUE; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpRemoveBevel class
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************************
>	class BevelInfo : public OpParam

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	9/10/98
	Purpose:	Bevel information structure
********************************************************************************************/

class BevelInfo : public OpParam
{
CC_DECLARE_DYNCREATE(BevelInfo)

public:
	BevelInfo()
	{
		m_BevelType					= 0;
		m_Indent					= 0;
		m_LightAngle				= 135;
		m_bOuter					= FALSE;
		m_Contrast					= 100;
		m_Tilt						= 32;
		m_bBevelTypeChanged			= FALSE;
		m_bBevelIndentChanged		= FALSE;
		m_bBevelLightAngleChanged	= FALSE;
		m_bBevelContrastChanged		= FALSE;
		m_bBevelDirectionChanged	= FALSE;
		m_bBevelLightTiltChanged    = FALSE;
		m_bCreateNewBevels			= FALSE;
	}
	
	INT32			m_BevelType;				// the kind of bevel
	BOOL		m_bBevelTypeChanged;		// has the bevel type changed ?
	INT32			m_Indent;					// the bevel distance in/out (in millipoints)
	BOOL		m_bBevelIndentChanged;		// has the indent changed ?
	double		m_LightAngle;				// the angle (from -ve left, clockwise) of the light source
	BOOL		m_bBevelLightAngleChanged;	// has the light angle changed ?
	BOOL		m_bOuter;					// whether its an inner or outer bevel
	BOOL		m_bBevelDirectionChanged;	// has the direction changed ?
	INT32		m_Contrast;					// the contrast setting (0 - 100)
	BOOL		m_bBevelContrastChanged;	// has the contrast changed ?
	double		m_Tilt;
	BOOL		m_bBevelLightTiltChanged;

	BOOL		m_bCreateNewBevels;			// whether to create new bevels or not
};

/********************************************************************************************
>	class OpChangeLightAnglesParam : public OpParam

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Purpose:	Param used by OpChangeAttribute
********************************************************************************************/
class OpChangeLightAnglesParam : public OpParam
{
public:
	OpChangeLightAnglesParam()
	{
		pNodes = NULL;
		NewLightAngle = 0;
		OldLightAngle = 0;
	}

	List * pNodes;
	INT32 NewLightAngle;
	INT32 OldLightAngle;
};

/********************************************************************************************
>	class OpChangeBevelLightAngle : public SelOperation

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Purpose:	Changes the light angle in the given attribute list
				Used by dragging to change the attributes
********************************************************************************************/
class OpChangeBevelLightAngle : public SelOperation
{

CC_DECLARE_DYNCREATE(OpChangeBevelLightAngle)

public:
	// Construction/Destruction
	OpChangeBevelLightAngle();
	~OpChangeBevelLightAngle();

	virtual void DoWithParam(OpDescriptor* pOp, OpParam * pBevParam);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	void GetOpName(String_256 *);
};

/********************************************************************************************
>	class RegenerateBevelAction : public Action

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Purpose:	Forces a regeneration of the bevel in all nodes in the list
				Used by NodeBevelcontroller::OnChildChange to force redraws in undos/redos
********************************************************************************************/
class RegenerateBevelAction : public Action
{

CC_DECLARE_DYNCREATE(RegenerateBevelAction)

public:
	RegenerateBevelAction();
	~RegenerateBevelAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							List * pNodes,
							RegenerateBevelAction** NewAction,
							BOOL bCache = FALSE);
protected:
	List * m_pNodes;
	BOOL m_bCache;
};

/********************************************************************************************
>	class RegenerateBevelBitmapAction : public Action

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Purpose:	Forces a regeneration of the bevel bitmap in all nodes in the list
				Mainly a performance enhancement so that instead of using RegenerateBevelAction,
				which causes a bevel to regenerate its path as well as the bitmap,
				you can instruct the bevel to just regenerate its bitmap
				Used by NodeBevelcontroller::OnChildChange to force redraws in undos/redos
********************************************************************************************/
class RegenerateBevelBitmapAction : public Action
{

CC_DECLARE_DYNCREATE(RegenerateBevelBitmapAction)

public:
	RegenerateBevelBitmapAction();
	~RegenerateBevelBitmapAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							List * pNodes,
							RegenerateBevelBitmapAction** NewAction);
protected:
	List * m_pNodes;
};


/********************************************************************************************
>	class ChangeLightAnglesAction : public Action

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Purpose:	Changes the light angles on the given list of bevel controller nodes in
				an undoable way
********************************************************************************************/
class ChangeLightAnglesAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeLightAnglesAction)

public:
	ChangeLightAnglesAction();
	~ChangeLightAnglesAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							List * pNodes,
							INT32 NewAngle,
							INT32 OldAngle,
							ChangeLightAnglesAction** NewAction);
protected:
	List m_NodeList;
	INT32 m_NewAngle;
	INT32 m_OldAngle;
};

/********************************************************************************************
>	class RemoveBevelAttributesAction : public Action

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/2000
	Purpose:	Removes all bevel attributes from the tree
********************************************************************************************/
class RemoveBevelAttributesAction : public Action
{

CC_DECLARE_DYNCREATE(RemoveBevelAttributesAction)

public:
	RemoveBevelAttributesAction();
	~RemoveBevelAttributesAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRenderableInk * pNode,
							BOOL m_bShouldDo, 
							RemoveBevelAttributesAction** NewAction);
protected:
	NodeRenderableInk * m_pNode;
	BOOL m_bShouldDo;
	List m_AttrList;
};


/********************************************************************************************
>	class OpSelectBevel : public Operation

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Purpose:	Selects bevel inking nodes in the selection
********************************************************************************************/
class OpSelectBevel : public Operation
{

CC_DECLARE_DYNCREATE(OpSelectBevel)

public:
	// Construction/Destruction
	OpSelectBevel();
	~OpSelectBevel();

	virtual void Do(OpDescriptor* pOp);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);
};


#endif
#endif
