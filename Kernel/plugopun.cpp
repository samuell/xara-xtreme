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
// 


#include "camtypes.h"

#include "plugopun.h"		// useful plug-in related items
#include "plugop.h"			// useful plug-in related items
//#include "plugres.h"		// _R(IDS_PLUGIN_FAMILY)

#include "plugmngr.h"		// main plug-in manager
#include "app.h"			// GetApplication()

//#include "simon.h"			// _R(IDS_NO_DOC) _R(IDS_NO_OBJECTS_SELECTED)
#include "nodebmp.h"		// NodeBitmap
#include "fillattr.h"		// AttrFillGeometry

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNCREATE(PlugInUndoOp, SelOperation);
CC_IMPLEMENT_DYNCREATE(ActionApplyPlugInToBitmap, Action)

// We want better memory tracking
#define new CAM_DEBUG_NEW

/*******************************************************************

>	PlugInUndoOp::PlugInUndoOp() : SelOperation()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for PlugInUndoOp operation. It is undoable.
	Errors:		None

*******************************************************************/

PlugInUndoOp::PlugInUndoOp() : SelOperation()
{
	m_pOpDesc = NULL;
}

/********************************************************************************************

>	static BOOL PlugInUndoOp::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Returns:	TRUE if the Op was started ok
	Purpose:	Creates all the opDescriptors that call this operation.

********************************************************************************************/

BOOL PlugInUndoOp::Init()
{
	BOOL ok = TRUE;
	// Set up some standard operations
//	ok = ok && RegisterOpToken(OPTOKEN_PLUGINS, _R(IDS_PLUGINS));

//	ok = ok && RegisterOpToken(OPTOKEN_PHOTOSHOP_PLUGINS, _R(IDS_PHOTOSHOP_PLUGINS));
//	ok = ok && RegisterOpToken(OPTOKEN_PHOTOSHOP_APPLYLAST, _R(IDS_PHOTOSHOP_APPLYLAST));

//	ok = ok && RegisterOpToken(OPTOKEN_PLUGINONE, _R(IDS_PLUGIN_ONE));
//	ok = ok && RegisterOpToken(OPTOKEN_PLUGINTWO, _R(IDS_PLUGIN_TWO));
//	ok = ok && RegisterOpToken(OPTOKEN_PLUGINTHREE, _R(IDS_PLUGIN_THREE));
	return ok;
}

/********************************************************************************************

>	static BOOL PlugInUndoOp::RegisterOpToken(TCHAR *OpToken, const String_64& NewMenuText,
										  CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInUndoOp))

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Inputs:		OpToken		the OpToken for the operation
				NewMenuText	the text for the menu item for this operation
				pClass		the runtime class to use, defaults to PlugInUndoOp.
				gs			the GetState function to use
	Returns:	TRUE if the Op was registered ok
	Purpose:	Allows OpDescriptors to be defined to the operation system.

********************************************************************************************/

BOOL PlugInUndoOp::RegisterOpToken(TCHAR *OpToken, const String_64& NewMenuText,
							   CCRuntimeClass* pClass, pfnGetState gs)
{
	// Try to create the OpDescriptor
	PlugInOpDescriptor* pOpDesc = new PlugInOpDescriptor(OpToken, NewMenuText, pClass, gs);
	if (pOpDesc == NULL)
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	static BOOL PlugInUndoOp::RegisterOpToken(TCHAR *OpToken, UINT32 MenuTextID,
										  CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInUndoOp),
										  pfnGetState gs = PlugInUndoOp::GetState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Inputs:		OpToken		the OpToken for the operation
				MenuTextId	the text for the menu item for this operation
				pClass		the runtime class to use, defaults to PlugInUndoOp.
				gs			the GetState function to use
	Returns:	TRUE if the Op was registered ok
	Purpose:	Allows OpDescriptors to be defined to the operation system.

********************************************************************************************/

BOOL PlugInUndoOp::RegisterOpToken(TCHAR *OpToken, UINT32 MenuTextID, CCRuntimeClass* pClass,
							   pfnGetState gs)
{
	// register some option descriptors
	if (!UndoableOperation::RegisterOpDescriptor(
						0,							// Tool ID
						MenuTextID,					// String resource ID
						pClass,						// Runtime class
						OpToken, 					// Token string
						gs,							// GetState function
						NULL,						// help ID
						NULL,						// bubble help
						NULL,						// resource ID
						NULL,						// control ID
						SYSTEMBAR_ILLEGAL,			// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// Smart duplicate operation
						FALSE,						// Clean operation
//						NULL,						// No vertical counterpart (not present in undoable versions)
						NULL,						// String for one copy only error
						(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
						))
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	static OpState PlugInUndoOp::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Inputs:		
	Outputs:	String - if MenuItem is enabled Then return the name of the operation to 
							be undone Else return the reason why it is disabled
	Returns:	OpState the state of the operation
	Purpose:	To provide greying and ticking functionality for the
				operation.
	Errors:		None

********************************************************************************************/

OpState PlugInUndoOp::GetState(String_256*, OpDescriptor* OpDesc)
{
	// At present, this item is always available.
	OpState OpSt;
	return OpSt;
}

/********************************************************************************************

>	virtual void Operation::GetOpName(String_256* pOpName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Outputs:	The name of the operation
	Purpose:	To get the name of the operation to put on the undo/redo menu items.
				We overide the baseclass version which tries to use the runtime class of
				this class to work out the OpDescriptor it was invoked with. Of course, we
				use one operation with multiple OpDescriptors and so this is not good enough.
				Unfortunately the operation class does not seem to save away the OpDescriptor
				that invoked it and so we must do this and then use this to track down the
				text to use.

********************************************************************************************/

void PlugInUndoOp::GetOpName(String_256* pOpName)
{
	if (pOpName == NULL)
		return;
		                         
	// Use our stored copy of the OpDescriptor and ask it for the text
	if (m_pOpDesc)             
		m_pOpDesc->GetText(pOpName, OP_UNDO_TEXT);
}  

/********************************************************************************************

>	virtual void PlugInUndoOp::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Inputs:		pOpDesc		the OpDescriptor of the item to invoke
	Outputs:	None
	Returns:	None
	Purpose:	Invokes the plug-in specified by the OpDescriptor.
	Errors:		if no OpDescriptor is supplied

********************************************************************************************/

void PlugInUndoOp::Do(OpDescriptor* pOpDesc)
{
	if (pOpDesc == NULL)
	{
		ERROR3IF(pOpDesc == NULL,"PlugInUndoOp::Do null OpDescriptor");
		return;
	}
	//ERROR3("PlugInUndoOp - do");

	// Store away a copy of the OpDescriptor for later use
	m_pOpDesc = pOpDesc;

	// Search the plug-ins list for the specified plug-in and invoke it
	PlugInManager* pManager = GetApplication()->GetPlugInManager();
	if (pManager == NULL)
		return;

	PlugInItem * pPlugIn = pManager->GetFirstPlugIn();
	String_32 OpToken;
	while (pPlugIn)
	{
		OpToken = pPlugIn->GetUniqueID();
		OpToken += pPlugIn->GetPlugInName();
		if (pOpDesc->Token == OpToken)
		{
			// Do Something
			break;
		}

		pPlugIn = pManager->GetNextPlugIn(pPlugIn);
	}

	// If we reached here then everything has happened ok and we can just end the
	// operation and exit 
	End();

	return;
}

/*******************************************************************************************

>	static BOOL PlugInUndoOp::FindBitmapInSelection(KernelBitmap * pFoundBitmap = NULL,
													KernelBitmapRef ** ppFoundBitmapRef = NULL,
													NodeBitmap ** ppFoundNode = NULL,
													AttrFillGeometry ** ppFoundFillAttribute = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Inputs:		None
	Outputs:	if supplied, it will return a pointer to the bitmap that is in the selection
				if supplied, it will return a pointer to the bitmap ref that is in the selection
				if supplied, it will return a pointer to the node bitmap that is in the selection
				if supplied, it will return a pointer to the fill geometry that is in the selection
	Returns:	TRUE if succeeded in finding a bitmap, FALSE if not
	Purpose:	Find out if there is a valid bitmap in the selection and return useful information
				to the caller.
	SeeAlso:	-

*******************************************************************************************/

/*
	This should be in RANGE for God's sake!
BOOL PlugInUndoOp::FindBitmapInSelection(KernelBitmap ** ppFoundBitmap,
										 KernelBitmapRef ** ppFoundBitmapRef,
										 NodeBitmap ** ppFoundNode,
										 AttrFillGeometry ** ppFoundFillAttribute)
{
	SelRange *pSelRange = GetApplication()->FindSelection();
	if (pSelRange == NULL)
		return FALSE;

	Range Sel(*pSelRange);

	Node* pFirstSelectedNode = Sel.FindFirst(); 
	KernelBitmap * pBitmap = NULL;
	KernelBitmapRef* pBitmapRef = NULL;
	if (pFirstSelectedNode != NULL) // No nodes selected so End
	{
	    Node* pCurrentNode = pFirstSelectedNode;       
	    Node* pNextCurrent = NULL; 
	    
	    // Do all bitmaps. OK this should pick up the fill as well. Never mind
		while (pCurrentNode != NULL)
		{
			pNextCurrent = Sel.FindNext(pCurrentNode);
	       	if  ( (pCurrentNode->IsSelected()) && (pCurrentNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeBitmap)) ) 
	       	{         
				NodeBitmap * pNodeBmp = (NodeBitmap *)(pCurrentNode);
				pBitmap = pNodeBmp->GetBitmap();
				pBitmapRef = pNodeBmp->GetBitmapRef();
				// If the caller wanted it, return the found bitmap to them
				if (ppFoundBitmap)
					*ppFoundBitmap = pBitmap;
				// ditto found bitmap ref
				if (ppFoundBitmapRef)
					*ppFoundBitmapRef = pBitmapRef;
				// ditto found node
				if (ppFoundNode)
					*ppFoundNode = pNodeBmp;
				// ditto found fill attribute
				if (ppFoundFillAttribute)
					*ppFoundFillAttribute = NULL;
				return TRUE;
	       	}

			pCurrentNode = pNextCurrent; 
		}

	} 

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapColourFill)))
		{
			pBitmap = pAttrNode->GetBitmap();
			pBitmapRef = pAttrNode->GetBitmapRef();
			// If the caller wanted it, return the found bitmap to them
			if (ppFoundBitmap)
				*ppFoundBitmap = pBitmap;
			// ditto found bitmap ref
			if (ppFoundBitmapRef)
				*ppFoundBitmapRef = pBitmapRef;
			// ditto found node
			if (ppFoundNode)
				*ppFoundNode = NULL;
			// ditto found fill attribute
			if (ppFoundFillAttribute)
				*ppFoundFillAttribute = pAttrNode;
			return TRUE;
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	// If the caller wanted it, return the found bitmap to them
	if (ppFoundBitmap)
		*ppFoundBitmap = NULL;
	// ditto found bitmap ref
	if (ppFoundBitmapRef)
		*ppFoundBitmapRef = NULL;
	// ditto found node
	if (ppFoundNode)
		*ppFoundNode = NULL;
	// ditto found fill attribute
	if (ppFoundFillAttribute)
		*ppFoundFillAttribute = NULL;

	return FALSE;
}
*/

/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	ActionApplyPlugInToBitmap::ActionApplyPlugInToBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Purpose:	ActionApplyPlugInToBitmap constructor

********************************************************************************************/
    
ActionApplyPlugInToBitmap::ActionApplyPlugInToBitmap()
{
	m_pDocument = NULL;
	m_pBitmap = NULL;
	m_pBitmapRef = NULL;
	m_pFoundNode = NULL;
	m_pFoundFillAttribute = NULL;
}

/********************************************************************************************

>	ActionApplyPlugInToBitmap::~ActionApplyPlugInToBitmap() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Purpose:	ActionApplyPlugInToBitmap destructor

********************************************************************************************/

ActionApplyPlugInToBitmap::~ActionApplyPlugInToBitmap()
{
}

/********************************************************************************************

>	virtual ActionCode ActionApplyPlugInToBitmap::Execute()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the ActionApplyPlugInToBitmap to go and apply a new bitmap to
				the currently selected fill or node bitmap and generate another
				ActionApplyPlugInToBitmap to undo this change.
				Forces a redraw of the fill or node.

********************************************************************************************/

ActionCode ActionApplyPlugInToBitmap::Execute()
{
	ActionApplyPlugInToBitmap ApplyPlugInAct;
	ActionCode ActCode = AC_FAIL;

	// Get the details on the stored bitmap node or fill so that we can use this to undo what we
	// are about to try and do
	KernelBitmap *		pBitmap = NULL;
	KernelBitmapRef *	pBitmapRef = NULL;
	if (m_pDocument != NULL && m_pBitmap != NULL && m_pBitmapRef != NULL)
	{
		// Note the bitmap currently in use on that fill or node
		if (m_pFoundNode != NULL)
		{
			pBitmap = m_pFoundNode->GetBitmap();
			pBitmapRef = m_pFoundNode->GetBitmapRef();
		}
		else if (m_pFoundFillAttribute != NULL)
		{
			pBitmap = m_pFoundFillAttribute->GetBitmap();
			pBitmapRef = m_pFoundFillAttribute->GetBitmapRef();
		}
		else
		{
			ERROR3("Called ActionApplyPlugInToBitmap::Execute with no node or fill selected!");
			return AC_FAIL; 
		}
	}
	else
	{
		ERROR3("Called ActionApplyPlugInToBitmap::Execute with no selected bitmap in the range!");
		return AC_FAIL; 
	}

	if (pBitmap == NULL || pBitmapRef == NULL)
	{
		ERROR3("Called ActionApplyPlugInToBitmap::Execute with no selected bitmap in the range!");
		return AC_FAIL; 
	}

    // Create an action to restore the changes we are about to make
    ActCode = ActionApplyPlugInToBitmap::Init(  pOperation,
								     			pOppositeActLst,
								     			sizeof(ActionApplyPlugInToBitmap),
												m_pDocument,
												pBitmap,
												pBitmapRef,
												m_pFoundNode,
												m_pFoundFillAttribute,
						 			 			( Action**)(&ApplyPlugInAct),
												m_bUpdateAspect
												);
	if (ActCode != AC_FAIL)
	{
		// Make sure we have quite a few valid items before even attempting to continue
		if (m_pDocument != NULL && m_pBitmap != NULL && m_pBitmapRef != NULL)
		{
			// Change that bitmap
			if (m_pFoundNode != NULL)
			{
				if (m_bUpdateAspect)
				{
					// Ensure that the aspect ratio of the new bitmap is retained
					// (Must be done before the new bitmap is attached)
					m_pFoundNode->SetAspectRatio(m_pBitmap->GetWidth(), m_pBitmap->GetHeight(), TRUE);
				}

				// Its a node bitmap so change its bitmap reference to be the new one
				m_pBitmapRef->Attach(m_pBitmap, m_pDocument);

				BROADCAST_TO_ALL(BitmapListChangedMsg(m_pBitmap->GetParentBitmapList(), m_pBitmap));

				// Say that the bitmap has changed
				((UndoableOperation*)pOperation)->DoInvalidateNodeRegion((NodeRenderableInk*)m_pFoundNode, TRUE);
			}
			else if (m_pFoundFillAttribute != NULL)
			{
				// Its a bitmap fill so change its bitmap reference to be the new one
				m_pBitmapRef->Attach(m_pBitmap, m_pDocument);

				BROADCAST_TO_ALL(BitmapListChangedMsg(m_pBitmap->GetParentBitmapList(), m_pBitmap));
			}
			else
			{
				ERROR3("Called ActionApplyPlugInToBitmap::Execute with no node or fill selected!");
				return AC_FAIL; 
			}
		}
	}

	return (ActCode);
}



/********************************************************************************************

>	ActionCode ActionApplyPlugInToBitmap::Init( Operation* const pOp, 
												ActionList* pActionList, 	
							  					UINT32 ActionSize,
												Document *pDocument,
												KernelBitmap *pBitmap,
												KernelBitmapRef *pBitmapRef,
												NodeBitmap * pFoundNode,
												AttrFillGeometry * pFoundFillAttribute,
				  								Action** NewAction)
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97

	Inputs:		pOp: The operation to which the action should be added
				pActionList: The action list in the operation object
				ActionSize:  The size of the action in bytes. This should be the total  
							 size of the action (including any objects pointed to by the
							 action).
				pDocument:	 The target document which contains the bitmap
				pBmp:		 The target bitmap to have the plug-in applied to
				pBitmapRef:	 The target bitmap reference to be changed
				pFoundNode:	 The possible target nodebitmap or NULL
				pFoundFillAttribute:	The possible target attribute or null.

 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to 
							 continue without undo. 

				AC_OK      : The action was successfully initialised and added to the 
							 operation. 
				         
				           
	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations 
				action list. 
				
				The function calls the Action::Init function passing the runtime class 
				of an ActionApplyPlugInToBitmap.
	Errors:		-
	SeeAlso:	Action::Init; PlugInUndoOp; 

********************************************************************************************/
   
ActionCode ActionApplyPlugInToBitmap::Init( Operation* const pOp, 
											ActionList* pActionList, 	
						  					UINT32 ActionSize,
											Document *pDocument,
											KernelBitmap *pBitmap,
											KernelBitmapRef *pBitmapRef,
											NodeBitmap * pFoundNode,
											AttrFillGeometry * pFoundFillAttribute,
				  							Action** NewAction,
											BOOL bUpdateAspect)
{
	ActionCode Ac = (Action::Init(pOp,
									pActionList,
									ActionSize,
									CC_RUNTIME_CLASS(ActionApplyPlugInToBitmap), 
									NewAction));
	if (*NewAction != NULL)
	{
		ActionApplyPlugInToBitmap *ACC = (ActionApplyPlugInToBitmap*) (*NewAction);
		ACC->m_pDocument  = pDocument;
		ACC->m_pBitmap  = pBitmap;
		ACC->m_pBitmapRef = pBitmapRef;
		ACC->m_pFoundNode = pFoundNode;
		ACC->m_pFoundFillAttribute = pFoundFillAttribute;
		ACC->m_bUpdateAspect = bUpdateAspect;
	}

	return (Ac);
}

/********************************************************************************************
********************************************************************************************/
