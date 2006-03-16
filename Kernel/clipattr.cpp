// $Id: clipattr.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of ClipRegionAttribute.
//

#include "camtypes.h"	// pre-compiled header
#include "clipattr.h"	// our own header file
#include "grndrgn.h"	// for GRenderRegion.
//#include "mario.h"			// _R(IDE_NOMORE_MEMORY)
#include "blendatt.h"		// BlendAttrParam

DECLARE_SOURCE("$Revision: 662 $")

// runtime class creation stuff.
CC_IMPLEMENT_DYNCREATE(ClipRegionAttribute, AttributeValue)
CC_IMPLEMENT_DYNAMIC(AttrClipView, NodeAttribute)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	ClipRegionAttribute::ClipRegionAttribute()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
ClipRegionAttribute::ClipRegionAttribute()
{
	m_pClipPath	= NULL;
	m_bResponsibleForGrouping = FALSE;
}



/********************************************************************************************

>	ClipRegionAttribute::~ClipRegionAttribute()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
ClipRegionAttribute::~ClipRegionAttribute()
{
	if (m_pClipPath != NULL)
		m_pClipPath = NULL;
}



/********************************************************************************************

>	virtual void ClipRegionAttribute::Render(RenderRegion* pRender, BOOL Temp = FALSE)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void ClipRegionAttribute::Render(RenderRegion* pRender, BOOL Temp)
{
// DEBUG:
//	TRACEUSER( "Karim", _T("ClipRegionAttribute::Render; 0x%x with path 0x%x\n"), (DWORD)this,
//																			(DWORD)m_pClipPath);
//	TRACEUSER( "Karim", _T("pClipAttr 0x%x Render()'ed by RR 0x%x\n"),
//									(DWORD)this, (DWORD)pRender);
//#pragma message( __LOCMSG__ "ClipRegionAttribute::Render - do nothing" )
//	TRACE( _T("Warning - ClipRegionAttribute::Render called\n") );
	pRender->SetClipRegion(this, Temp);
}



/********************************************************************************************

>	virtual void ClipRegionAttribute::Restore(RenderRegion* pRender, BOOL Temp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void ClipRegionAttribute::Restore(RenderRegion* pRender, BOOL Temp)
{
// DEBUG:
//	TRACEUSER( "Karim", _T("ClipRegionAttribute::Restore; 0x%x with path 0x%x\n"), (DWORD)this,
//																			(DWORD)m_pClipPath);
//	TRACEUSER( "Karim", _T("pClipAttr 0x%x Restore()'ed by RR 0x%x\n"),
//									(DWORD)this, (DWORD)pRender);
//#pragma message( __LOCMSG__ "ClipRegionAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - ClipRegionAttribute::Restore called\n") );
	pRender->RestoreClipRegion(this, Temp);
}



/********************************************************************************************

>	virtual void ClipRegionAttribute::GoingOutOfScope(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void ClipRegionAttribute::GoingOutOfScope(RenderRegion* pRender)
{
// DEBUG:
//	TRACEUSER( "MarkH", _T("ClipRegionAttribute::GoingOutOfScope; 0x%x with path 0x%x\n"), (DWORD)this,
//																			(DWORD)m_pClipPath);
}



/********************************************************************************************

>	virtual void ClipRegionAttribute::SimpleCopy(AttributeValue* pSource)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		pSource		the ClipRegionAttribute to copy.
	Outputs:	This ClipRegionAttribute becomes a copy of pSource.

	Purpose:	Make this attribute into a straight copy of the given attribute.

	Notes:		Note that NO error checking is done in this routine (bad?), so MAKE SURE
				that the pointer you pass in as pSource is a VALID ClipRegionAttribute.

********************************************************************************************/
void ClipRegionAttribute::SimpleCopy(AttributeValue* pSource)
{
// DEBUG:
//	TRACEUSER( "Karim", _T("ClipRegionAttribute::SimpleCopy; 0x%x with path 0x%x\n"), (DWORD)this,
//																			(DWORD)m_pClipPath);
	m_pClipPath = ((ClipRegionAttribute*)pSource)->m_pClipPath;
}



/********************************************************************************************

>	NodeAttribute* AttributeValue::MakeNode()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12 April 2000
	Returns:	NULL always.
	Purpose:	This method is here so that an AttributeValue can build and return a 
				corresponding NodeAttribute, for insertion into the tree. We don't do
				this, as we're not linked with NodeAttributes at all.

********************************************************************************************/
NodeAttribute* ClipRegionAttribute::MakeNode()
{
	return NULL;
}



/********************************************************************************************

>	virtual BOOL ClipRegionAttribute::IsDifferent(AttributeValue* pOther)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL ClipRegionAttribute::IsDifferent(AttributeValue* pOther)
{
// DEBUG:
//	TRACEUSER( "Karim", _T("ClipRegionAttribute::IsDifferent; we are 0x%x\n"), (DWORD)this);
	ERROR2IF(	GetRuntimeClass() != pOther->GetRuntimeClass(), TRUE,
			   "Attribute difference comparison must be between same types"	);
	return (m_pClipPath != ((ClipRegionAttribute*)pOther)->GetClipPath());
}



/********************************************************************************************

>	static BOOL ClipRegionAttribute::Init()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL ClipRegionAttribute::Init()
{
	ClipRegionAttribute* pAttrVal = NULL;
	pAttrVal = new ClipRegionAttribute;
	if (pAttrVal == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(NodeRenderableInk),
														 pAttrVal);

	if (ID == ATTR_BAD_ID)
		return FALSE;

	ERROR3IF(ID != ATTR_CLIPREGION, "Incorrect ID for attribute!");
	return TRUE;
}

///////////////////////////////////// NEW CLIPVIEW ATTRIBUTE!!! /////////////////////////////
AttrClipView::AttrClipView()
{
	m_DoRender = TRUE;
	Value.SetResponsibleForGrouping (TRUE);
}

AttrClipView::AttrClipView(Node* ContextNode, AttachNodeDirection Direction,
							BOOL Locked, BOOL Mangled, BOOL Marked, BOOL Selected) : 
							NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
	m_DoRender = TRUE;
	Value.SetResponsibleForGrouping (TRUE);
}

AttrClipView::~AttrClipView()
{
}

void AttrClipView::Render( RenderRegion* pRender )
{
//#pragma message( __LOCMSG__ "ClipRegionAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - ClipRegionAttribute::Restore called\n") );
	// check the pointer and also make sure that we`re allowed to render
	if (!pRender || !m_DoRender)
		return;

	// Don`t need to render when hit testing
	if(!pRender->IsHitDetect())
		Value.Render(pRender);
}

#ifdef _DEBUG
void AttrClipView::GetDebugDetails(StringBase* Str)
{
	// Get our base class debug info.
	NodeRenderable::GetDebugDetails(Str);

	// Output the data for our clipping path.
	Path* pClipPath = Value.GetClipPath();

	if (pClipPath == NULL)
	{
		(*Str) += TEXT( "\r\nNo Clipping Path Data\r\n" );
	}
	else
	{
		(*Str) += TEXT( "\r\nClipping Path Data\r\n" );
		pClipPath->GetDebugDetails(Str);
	}
}
#endif

Node* AttrClipView::SimpleCopy()
{
	AttrClipView* pAttr = new AttrClipView();
	ERRORIF(pAttr==NULL, _R(IDE_NOMORE_MEMORY),NULL);

	CopyNodeContents(pAttr);
	return pAttr;
}



/***********************************************************************************************
>   void AttrClipView::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrClipView::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrClipView), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrClipView))
		CopyNodeContents((AttrClipView*)pNodeCopy);
}



void AttrClipView::CopyNodeContents(NodeAttribute *pCopy)
{
	NodeAttribute::CopyNodeContents(pCopy);
	((AttrClipView*)pCopy)->Value.SimpleCopy( &Value );
}

INT32 AttrClipView::operator==(const NodeAttribute& NodeAttrib)
{
	Path* pComparePath = ((ClipRegionAttribute*)&NodeAttrib)->GetClipPath();
	Path* pThisPath = Value.GetClipPath();

	if(pComparePath && pThisPath)
	{
		return pThisPath->ComparePathToPath(pComparePath);
	}

	return -1;
}

void AttrClipView::Transform(TransformBase& Trans)
{
//	static const FIXED16 One(1.0);

//	FIXED16 ScaleFactor = Trans.GetScaleFactor();
}

BOOL AttrClipView::Blend(BlendAttrParam* pBlendParam)
{
	// Don`t want to blend this attribute at the mo!
	return FALSE;
}

BOOL AttrClipView::ContainsAttributeValue(AttributeValue* pVal)
{
	if(!pVal->IS_KIND_OF(ClipRegionAttribute))
	{
		ENSURE(FALSE,"Strange attr value comparison test requested");
		return FALSE;
	}

	return (pVal == &Value);
}

BOOL AttrClipView::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

BOOL AttrClipView::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return TRUE;
}
