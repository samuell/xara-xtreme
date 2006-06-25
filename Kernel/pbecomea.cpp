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
// Implementation of PathBecomeA.
//

#include "camtypes.h"	// pre-compiled header.
#include "pbecomea.h"	// make shapes stuff.

#include "strkattr.h"	// AttrStrokeType use.
#include "ppstroke.h"

#include "nodepath.h"	// NodePath reference.
#include "lineattr.h"	// for line width & colour attributes.
#include "ndclpcnt.h"	// for ClipView path constants.
#include "attrmap.h"

DECLARE_SOURCE("$Revision$");

// dynamic class creation stuff.
CC_IMPLEMENT_MEMDUMP(PathBecomeA, BecomeA)
CC_IMPLEMENT_MEMDUMP(ContourBecomeA2, PathBecomeA)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	PathBecomeA::PathBecomeA(	BecomeAReason baReason, CCRuntimeClass* pClass,
								UndoableOperation* pUndoOp, BOOL bSel, Path* pResultPath,
								enum OutlineRules OutlineRule = IGNORE_OUTLINES )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		baReason	the reason for the call to this function.
				pClass		the type of class required - ie a NodePath.
				pUndoOp		ptr to the Op requesting the BecomeA.
				bSel		whether to select the newly created node (unused in this class).
				pResultPath	points to a Path object in which to leave the results.

				fRemoveOutlines		if set, then we strip the area obscured by any object
									outlines, from the resultant path.

	Outputs:	After a call to DoBecomeA with this parameter object, baReason set to
				BECOMEA_PASSBACK and pClass set to NodePath, pResultPath should contain the 
				outline(s) of whatever node was asked to DoBecomeA.

	Notes:		It is up to the caller to create, initialise and destroy the Path object 
				pointed to by pResultPath. All we do is fill the path object with the
				required information.

				Outline rules - as an example, imagine processing two overlapping circles
				with 20pt line width:
				IGNORE_OUTLINES		the default; result - the path which would enclose
									both circles if their line widths were set to 0.

				STRIP_OUTLINES		the path which would result if you drew the circles,
									then erased the areas covered by their thick line widths.

				ADD_OUTLINES		the path which would result if you drew the circles,
									complete with thick line widths.

********************************************************************************************/
PathBecomeA::PathBecomeA(	BecomeAReason baReason, CCRuntimeClass* pClass,
							UndoableOperation* pUndoOp, BOOL bSel, Path* pResultPath,
							enum OutlineRules OutlineRule )
							: BecomeA(baReason, pClass, pUndoOp, bSel)
{
	// initialise our pointer to the result-path.
	m_pResultPath = pResultPath;

	// initialise our outlines behaviour.
	m_OutlineRule = OutlineRule;
	if (m_OutlineRule == STRIP_OUTLINES)
		m_ClipPathRule = 4;
	else if (m_OutlineRule == ADD_OUTLINES)
		m_ClipPathRule = 7;
}



PathBecomeA::~PathBecomeA()
{
	// empty our pointer to the result-path.
	m_pResultPath = NULL;
}



/********************************************************************************************

>	virtual BOOL PathBecomeA::Passback(	NodeRenderableInk* pNewNode,
										NodeRenderableInk*& pCreatedByNode,
										CCAttrMap* pAttrMap = NULL)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Inputs:		pNewNode		points to a new NodeRenderableInk created as a result of the 
								BecomeA operation.
				pCreatedByNode	the node which called us.
				pAttrMap		an attribute map - useful for stroking paths...

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Called by DoBecomeA when a node is asked to convert itself to another type of 
				node. The passed-in new node is expected to be a NodePath. We extract its 
				actual Path, and add it to the path referenced by a pointer we were given at 
				construction.

	Notes:		The possible node tree pointed to by pNewNode, together with pAttrMap, must 
				both be destroyed by this function - no-one else is going to do it.

	Errors:		ERROR2 with FALSE if pNewNode is NULL or is not a path.
	See also:	BecomeA::PassBack()

********************************************************************************************/
BOOL PathBecomeA::PassBack(	NodeRenderableInk* pNewNode,
							NodeRenderableInk* pCreatedByNode,
							CCAttrMap* pAttrMap )
{
	// validate inputs.
	ERROR2IF(	pNewNode == NULL || !pNewNode->IsNodePath(),
				FALSE,
				"PathBecomeA::PassBack; Passed back node was NULL, or not a path" );

	ERROR2IF(	m_pResultPath == NULL, FALSE,
				"PathBecomeA::PassBack; NULL result path!"	);

	BOOL fDeleteAttributes	= TRUE;
	BOOL fDeleteFillPath	= FALSE;

	const INT32 Flatness = NodeClipViewController::EstimatePathFlatness();

	// try to get an attribute map for converting the outline to shapes.
	if (pAttrMap == NULL)
	{
		fDeleteAttributes = FALSE;
		pAttrMap = CCAttrMap::MakeAppliedAttrMap(pCreatedByNode);
	}

	NodePath* pNewNodePath = (NodePath*)pNewNode;

	Path* pFillPath = &( pNewNodePath->InkPath );
	Path* pLinePath = NULL;

	// ok, first try to get the node's outline as a closed path.
	// we obviously won't get anything unless the outline's non-transparent.
	pLinePath = ExtractOutlineAsPath(pNewNodePath, pAttrMap);

	// providing the fill-path is filled, we'll use it as is.
	if (pFillPath->IsFilled)
	{
		if (pFillPath->GetNumCoords() < 3)
			pFillPath->ClearPath();
	}

	// if the fill-path is open, then we'll use the line path instead.
	else
	{
		fDeleteFillPath = TRUE;
		pFillPath = pLinePath;
		pLinePath = NULL;
	}

	// provided it exists, add the fill-path to the results path.
	if (pFillPath != NULL && pFillPath->GetNumCoords() > 0)
	{
		// if our results path is empty, just plonk the fill path in it.
		if (m_pResultPath->GetNumCoords() == 0)
			m_pResultPath->CloneFrom(*pFillPath);

		// otherwise, add our fill path to the current results path.
		// there's a slight optimisation here - if the bounds of the two paths don't overlap,
		// then the path union can be done simply by merging them together.
		else
		{
			BOOL fBoundsOverlap = FALSE;
			DocRect drResultPath(0, 0, 0, 0);
			DocRect drFillPath(0, 0, 0, 0);
			if (m_pResultPath->GetTrueBoundingRect(&drResultPath))
				if (pFillPath->GetTrueBoundingRect(&drFillPath))
					if (drFillPath.IsIntersectedWith(drResultPath))
						fBoundsOverlap = TRUE;

			if (!fBoundsOverlap)
				m_pResultPath->MergeTwoPaths(*pFillPath);

			else
			{
				m_pResultPath->ClipPathToPath(	*pFillPath, m_pResultPath, 7,
												NodeClipViewController::CLIPVIEW_TOLERANCE,
												Flatness,
												Flatness );

				// there is a bug here somewhere.  If pCreatedByNode just happens to have a stroke
				// applied to it, then the above call to ClipPathToPath seems to invert the path!
				// I'm not sure what to do about this, so for now it going to have to be left ....
				// NOTE:  to observe the bug, group two (overlapping) quickshapes, apply a stroke to
				// the group and slice with a line.
			}
		}
	}

	// if we're not ignoring outlines and we have a valid outline,
	// then throw it into the soup.
	if (m_OutlineRule != IGNORE_OUTLINES)
	{
		if (pLinePath != NULL)
		{
			m_pResultPath->ClipPathToPath(	*pLinePath, m_pResultPath, m_ClipPathRule | 0x10,
											NodeClipViewController::CLIPVIEW_TOLERANCE,
											Flatness,
											Flatness );
		}

		// a side-effect of the ClipPathToPath call above is that it ensures the path is tidy.
		// this ClipPathToPath call does the same thing.
		else
			m_pResultPath->ClipPathToPath(*m_pResultPath, m_pResultPath, 3, 20, 75, 75);
	}

	// PathBecomeA tries to return closed paths, so ensure our results path is marked so.
	if (!m_pResultPath->IsFilled)
		m_pResultPath->IsFilled = TRUE;

	// clear away all our temporary info.
	// no NULL assignments required, as these vars are all about to go out of scope.
	if (pLinePath != NULL)
		delete pLinePath;

	if (fDeleteFillPath && pFillPath != NULL)
		delete pFillPath;

	if (pAttrMap != NULL)
	{
		if (fDeleteAttributes)
			pAttrMap->DeleteAttributes();

		delete pAttrMap;
	}

	pNewNode->CascadeDelete();
	delete pNewNode;

	return TRUE;
}



/********************************************************************************************

>	Path* PathBecomeA::ExtractOutlineAsPath(NodePath* pSrcNodePath, CCAttrMap* pAttrMap)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2000
	Inputs:		pSrcNodePath	a NodePath whose outline we're going to extract.
				pAttrMap		the attribute map from which we'll get line attributes,
								eg width, cap style, join style etc.
	Returns:	A ptr to a path if successful,
				NULL otherwise.

	Purpose:	Given a NodePath and an attribute map, construct the Path which delineates
				the NodePath's outline. If the line width of the path is 0pt, you'll get
				a NULL ptr back.

	Notes:		This method returns a Path allocated on the heap - it's *your* responsibility
				to delete it!

				Either input can be NULL, in which case you'll get a NULL ptr back.

				This method should really just take a pure Path* for flexibility,
				however we make use of MakeNodePathFromAttributes(), which is a NodePath
				method. I'm fed up of rewriting other people's code at the moment and
				I don't want to potentially destabilise the code base before a release.
				If you want to rewrite MakeNodePath... and this method to take a pure Path*,
				be my guest :o)

********************************************************************************************/
Path* PathBecomeA::ExtractOutlineAsPath(NodePath* pSrcNodePath, CCAttrMap* pAttrMap)
{
	// we can accept NULL inputs - we just don't do any processing for them.
	if (pAttrMap == NULL || pSrcNodePath == NULL)
		return NULL;

	const INT32 Flatness = NodeClipViewController::EstimatePathFlatness();

	Path* pFillPath		= &( pSrcNodePath->InkPath );
	Path* pStrokePath	= NULL;

	// we only need to generate an outline path if the path has non-transparent line colour.
	BOOL bHaveAnOutline = FALSE;
	AttrStrokeColour* pAttrStrokeColour = NULL;
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void*&)pAttrStrokeColour );
	if (pAttrStrokeColour != NULL)
	{
		DocColour* pLineColour = pAttrStrokeColour->GetStartColour();
		if (pLineColour != NULL && !pLineColour->IsTransparent())
		{
			bHaveAnOutline = TRUE;
		}
	}

	// ok, if we don't have an outline, then go home now.
	if (!bHaveAnOutline)
		return NULL;

	// if the line is stroked, then use the stroke's path-processor to give us the outline.
	AttrStrokeType* pAttrStrokeType = NULL;
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeType), (void*&)pAttrStrokeType );
	if (pAttrStrokeType != NULL && pAttrStrokeType->HasPathProcessor())
	{
		PathProcessorStroke* pPPS = pAttrStrokeType->GetPathProcessor();
		if (pPPS != NULL)
		{
			pStrokePath = pPPS->GetProcessedPath(pFillPath, pAttrMap);
			pStrokePath->ClipPathToPath(*pStrokePath, pStrokePath, 3 | 0x10, 20, 75, 75);
		}
	}

	// if it's a simple line, then use DavidMc's MakeNodePath... method.
	else
	{
		NodePath* pNodeOutline = NULL;
		pNodeOutline = pSrcNodePath->MakeNodePathFromAttributes(Flatness,
																pAttrMap,
																TRUE);
		if (pNodeOutline != NULL)
		{
			pStrokePath = pNodeOutline->GetPathCopy();
			delete pNodeOutline;
			pNodeOutline = NULL;
		}
	}

	return pStrokePath;
}



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	ContourBecomeA2::ContourBecomeA2(	UndoableOperation* pUndoOp,
										MILLIPOINT Width,
										Path* pResultPath	)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/2000
	Inputs:		pUndoOp			pointer to an Op, if you have one.
				Width			how far out you want me to contour, in millipoints.
				pResultPath		where you want me to plonk the contour - must point to
								an initialised path which you don't mind me emptying.

	Purpose:	Constructor for a new ContourBecomeA, to do your contouring dirty work :o)

********************************************************************************************/
ContourBecomeA2::ContourBecomeA2(	UndoableOperation* pUndoOp,
									MILLIPOINT Width,
									Path* pResultPath	)
									: PathBecomeA(	BECOMEA_PASSBACK,
													CC_RUNTIME_CLASS(NodePath),
													pUndoOp,
													FALSE,
													pResultPath,
													ADD_OUTLINES )
{
	m_Width = Width;
	SetDoSilhouette(TRUE);
	SetDoShadowSilhouettes(TRUE);
}



/********************************************************************************************

>	BOOL ContourBecomeA2::PassBack(	NodeRenderableInk* pNewNode,
									NodeRenderableInk* pCreatedByNode,
									CCAttrMap* pAttrMap )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/2000

	Inputs:		pNewNode		ptr to the NodePath which you're passing in to me
								(don't expect it back).
				pCreatedByNode	ptr to the node which created the NodePath - useful to me
								if you don't supply an attr-map.
				pAttrMap		ptr to an attr-map for me to get line attributes out of
								when I stroke the path. Not compulsory

	Outputs:	pResultPath will contain the result of doing a contour outwards from the
				DoBecomeA-ing objects.

	Returns:	TRUE if successful, FALSE if not.

	Purpose:	PassBack function fer lettin' me do ma contour thang.

********************************************************************************************/
BOOL ContourBecomeA2::PassBack(	NodeRenderableInk* pNewNode,
								NodeRenderableInk* pCreatedByNode,
								CCAttrMap* pAttrMap )
{
	Path* pContourPath			= NULL;
	Path* pSummedContourPath	= m_pResultPath;

	m_pResultPath = new Path;
	BOOL	ok = (m_pResultPath != NULL);
	if (ok)	ok = m_pResultPath->Initialise();
	if (ok)
	{
		PathBecomeA::PassBack(pNewNode, pCreatedByNode, pAttrMap);
		ok = (m_pResultPath->GetNumCoords() != 0);
	}
	if (ok)
	{
		pContourPath = new Path;
		ok = (pContourPath != NULL);
	}
	if (ok)	ok = pContourPath->Initialise();
	if (ok)
	{
		TRACEUSER( "Karim", _T("Node at 0x%x; ResultPath - %d coords.\n"),
							pCreatedByNode,
							m_pResultPath->GetNumCoords());

		m_pResultPath->InitializeContourValues(2 * m_Width);
		m_pResultPath->GetContourForStep(pContourPath);

		delete m_pResultPath;
		m_pResultPath = pSummedContourPath;

		if (pContourPath->GetNumCoords() != 0)
			pContourPath->ClipPathToPath(*m_pResultPath, m_pResultPath, 7, 20, 75, 75);

		if (!m_pResultPath->IsFilled)
			m_pResultPath->IsFilled = TRUE;

		delete pContourPath;
	}

	return ok;
}
