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

// BrushRef implementation


#include "camtypes.h"
#include "brushref.h"
#include "nodebldr.h"
#include "docrect.h"
#include "doccoord.h"
#include "fillval.h"
#include "fillattr.h"
#include "nodebldr.h"
#include "fixmem.h"
#include "nodepath.h"
#include "nodecomp.h"
#include "gblend.h"
#include "nodeblnd.h"
#include "nodeclip.h"
#include "lineattr.h"

CC_IMPLEMENT_MEMDUMP(BrushRef, BlendRef)
CC_IMPLEMENT_MEMDUMP(BrushRefBlender, CC_CLASS_MEMDUMP);

#define DELPTR(p) if (p != NULL) { delete p; p = NULL; }
#define FLATNESS 1024

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW    

/***********************************************************************************************

>	void BlendPathOffset::CalculateAngleAndMagnitude()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	takes the x and y offset members and calculates the angle and magnitude of that vector

***********************************************************************************************/

void BlendPathOffset::CalculateAngleAndMagnitude()
{
	MILLIPOINT Xsq = m_XOffset * m_XOffset;
	MILLIPOINT Ysq = m_YOffset * m_YOffset;
	double Hypotenuse = sqrt((double)Xsq + (double)Ysq);
	double Calc = ((double)m_XOffset / Hypotenuse);
	double RadAngle = acos(Calc);
//	double DegAngle = RadAngle * (180 / PI);

	m_Angle = RadAngle;
	m_Magnitude = (MILLIPOINT)Hypotenuse;
}


/***********************************************************************************************

>	void BlendPathOffset::GetAngleAndMagnitude(double* pAngle, MILLIPOINT* pMagnitude)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		-
	Outputs:	pAngle
				pMagnitude
	Returns:	-
	Purpose:	to get the angle and magnitude of this vector

***********************************************************************************************/

void BlendPathOffset::GetAngleAndMagnitude(double* pAngle, MILLIPOINT* pMagnitude)
{
	*pAngle = m_Angle;
	*pMagnitude = m_Magnitude;
}


/***********************************************************************************************

>	void BlendPathOffset::RotateByAngle(double Angle, MILLIPOINT* pXOffset, MILLIPOINT* pYOffset)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		Angle - the angle to rotate by
	Outputs:	pXOffset - the resulting scalar value of the vector
				pYOffset
	Returns:	-
	Purpose:	to find out the x and y offset values if this blendpathoffset were to be rotated
				by the given angle

***********************************************************************************************/

void BlendPathOffset::RotateByAngle(double Angle, MILLIPOINT* pXOffset, MILLIPOINT* pYOffset)
{
	double NewAngle = m_Angle + Angle;

	*pXOffset = (MILLIPOINT)(m_Magnitude * cos(NewAngle));
	*pYOffset = (MILLIPOINT)(m_Magnitude * sin(NewAngle));
}



/***********************************************************************************************

> BrushRef::BrushRef()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor.

***********************************************************************************************/

BrushRef::BrushRef() 
{
	m_pNode 			= NULL;
	m_NumBlendPaths 	= 0;
	m_pBlendPathAttrMap = NULL;
	m_CachedRect		= DocRect(0,0,0,0);
	m_pCurrentBlendPath = NULL;
}

/***********************************************************************************************

> BrushRef::~BrushRef()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor.

***********************************************************************************************/

BrushRef::~BrushRef()
{
	FreeAttributes();
	m_BlendPathList.DeleteAll();
	m_OffsetList.clear();
}

/***********************************************************************************************

> BOOL BrushRef::Initialise(NodeRenderableInk* pInitNode)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		pInitNode 	= ptr to the node this BrushRef refers to.
			
	Outputs:	-
	Returns:	TRUE if successfully initialises, FALSE otherwise
	Purpose:	This inits the BrushRef object with the given node.  Essentially a wrapper for
				the blendref initialise function.  However it also works out 
				the blendpath offsets, if there are any

***********************************************************************************************/

BOOL BrushRef::Initialise(NodeRenderableInk* pInitNode)
{
	ERROR2IF(pInitNode == NULL, FALSE, "Ink node is NULL in BrushRef::Initialise");
	
	// Note that BrushDefinition::GenerateBrush now prevents objects with brush attributes being
	// used to generate brushes, instead it converts them to groups of nodepaths as per Convert Line to Shapes

	pInitNode->MakeAttributeComplete(); // just in case we're missing any

	BOOL ok = BlendRef::InitialiseForBrush(pInitNode); 

	if (ok)
	{
		// if we have made a brushref from a group then the different 
		// blendpaths will be offset from the centre of.  work out these
		// offsets and store them in a list
		m_OffsetList.clear();
		DocRect BRect = GetBoundingRect();
		DocCoord Centre = BRect.Centre();
		
		BlendPath* pBlendPath = GetFirstBlendPath();
		
		while (pBlendPath != NULL)
		{	
			Path* pPath = pBlendPath->GetPath();
			if (pPath)
			{
				DocRect BlendPathRect = pPath->GetBoundingRect();
				DocCoord BlendPathCentre = BlendPathRect.Centre();
				
				BlendPathOffset ThisOffset;
				ThisOffset.m_XOffset = BlendPathCentre.x - Centre.x ;
				ThisOffset.m_YOffset = BlendPathCentre.y - Centre.y;
				ThisOffset.CalculateAngleAndMagnitude();
				m_OffsetList.push_back(ThisOffset);
				pBlendPath->FindAppliedAttributes();
				pBlendPath = GetNextBlendPath(pBlendPath);
			}
			else
			{
				ERROR3("BlendPath has no path");
				return FALSE;
			}
		}
		// if we have a bitmap fill we need to do some special things..
		SetBitmapFillFlags();
		PreBlend();
		if (pInitNode->IsABlend())
			((NodeBlend*)pInitNode)->Deinit();
		return TRUE;
	}
	

	return FALSE;

}


/***********************************************************************************************

>  BOOL BrushRef::MakeCopiesForRendering()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise (e.g. out of memory)
				
	Purpose:	Asks the blendpaths to make copies of their paths and attributes which
				we will use to render with

***********************************************************************************************/

BOOL BrushRef::MakeCopiesForRendering()
{
//	BlendPath* pLastPath = NULL;
	BlendPath* pBlendPath = GetFirstBlendPath();
	BOOL ok = TRUE;
	while (pBlendPath != NULL)
	{
		if (ok)
			ok = pBlendPath->MakeCopyPathAndAttributes();
		pBlendPath = GetNextBlendPath(pBlendPath);
	
	}
	return ok;

}


/***********************************************************************************************

>  BOOL BrushRef::UpdateCopies()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/01/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise (e.g. out of memory)
				
	Purpose:	Asks the blendpaths to make copies of their paths and attributes which
				we will use to render with

***********************************************************************************************/

BOOL BrushRef::UpdateCopies()
{
//	BlendPath* pLastPath = NULL;
	BlendPath* pBlendPath = GetFirstBlendPath();
	BOOL ok = TRUE;
	while (pBlendPath != NULL && ok)
	{
		pBlendPath->UpdateCopyPathAndAttributes();
		pBlendPath = GetNextBlendPath(pBlendPath);
	
	}
	return ok;

}


/***********************************************************************************************

>  BOOL BrushRef::MakeCopiesForRendering()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
				
	Purpose:	Asks the blendpaths to delete the copies of paths and attributes

***********************************************************************************************/

void BrushRef::DeleteRenderCopies()
{
//	BlendPath* pLastPath = NULL;
	BlendPath* pBlendPath = GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		pBlendPath->DeleteCopyPathAndAttributes();
		pBlendPath = GetNextBlendPath(pBlendPath);
	
	}
}


/***********************************************************************************************

>  Path* BrushRef::GetFirstCopyPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	the copy of the path made by the first blendpath that we should use for 
				rendering
				
	Purpose:	Makes the first blendpath our current blendpath member and returns the 
				copy of its path.
				Basically you should use the system like this:

				
				Path* pPath = pBrushRef->GetFirstCopyPath();
				CCAttrMap* pMap = NULL;

				while (pPath)
				{
					pMap = pBrushRef->GetCurrentAttributeCopy();

					pMap->Render(or whatever)
					pPath->Render (or whatever)
					
					pPath = pBrushRef->GetNextCopyPath();
				}

***********************************************************************************************/

Path* BrushRef::GetFirstCopyPath()
{
	m_pCurrentBlendPath = GetFirstBlendPath();
	if (m_pCurrentBlendPath == NULL)
	{
		ERROR3("First blendpath is NULL in BrushRef::GetFirstCopyPath");
		return NULL;
	}
	return m_pCurrentBlendPath->GetCopyPath();
}


/***********************************************************************************************

>  Path* BrushRef::GetFirstCopyPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	the next path to be rendered
				
	Purpose:	Increments our current blendpath member and returns the copy path of
				the next blendpath

***********************************************************************************************/

Path* BrushRef::GetNextCopyPath()
{
	// If our current blendpath isn't set yet then its an error
	if (m_pCurrentBlendPath == NULL)
	{
		ERROR3("Current blendpath is NULL in BrushRef::GetNextCopyPath");
		return NULL;
	}
	m_pCurrentBlendPath = GetNextBlendPath(m_pCurrentBlendPath);
	
	if (m_pCurrentBlendPath != NULL)
		return m_pCurrentBlendPath->GetCopyPath();

	// we've hit the end of the line
	return NULL;
}



/***********************************************************************************************

>  CCAttrMap* BrushRef::GetCurrentAttributeCopy()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	the attribute map that we should render with our current copy path
				
	Purpose:	

***********************************************************************************************/

CCAttrMap* BrushRef::GetCurrentAttributeCopy()
{
	if (m_pCurrentBlendPath == NULL)
	{
		ERROR3("Current blendpath is NULL in BrushRef::GetCurrentAttributeCopy");
		return NULL;
	}
	return m_pCurrentBlendPath->GetCopyAttributes();

}

/***********************************************************************************************

>   MILLIPOINT  BrushRef::GetFirstOffset(POSITION* pHeadPosition)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		pListPosition - uninitialised POSITION variable
	Outputs:	pListPosition - the position of the head of the list - use it to pass into GetNextOffset
	Returns:	the value of the first offset in the m_offset list.  If the list is empty it
				returns -1.
	Purpose:	as above

***********************************************************************************************/

BlendPathOffset *BrushRef::GetFirstOffset( iterator *pHeadPosition )
{
	if (m_OffsetList.empty())
		return NULL;
	else
	{
		*pHeadPosition = m_OffsetList.begin();
		return &( *(*pHeadPosition)++ );
	}
}


/***********************************************************************************************

>   MILLIPOINT  BrushRef::GetNextOffset(POSITION* pPosition)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		pPosition -  the position to get in the list
	Outputs:	pPosition - the next position in the list (or NULL)
	Returns:	the value of the next offset in the m_offset list.  If the list is empty it
				returns NULL.
	Purpose:	as above

***********************************************************************************************/

BlendPathOffset* BrushRef::GetNextOffset( iterator *pHeadPosition )
{
	if( m_OffsetList.empty() )
		return NULL;
	else
		return &( *(*pHeadPosition)++ );
}


/***********************************************************************************************

>   DocRect  BrushRef::GetBoundingRect()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		
	Outputs:	
	Returns:	the bounding rect of this brushref
	Purpose:	as above, unions the bounding boxes 

***********************************************************************************************/

DocRect BrushRef::GetBoundingRect()
{
	DocRect Rect;

	BlendPath		   *pBlendPath = GetFirstBlendPath();
	BlendPathOffset		Offset;
	iterator			ListPos = m_OffsetList.begin();
	CCAttrMap		   *pAttrMap = NULL;
	while (pBlendPath != NULL)
	{
		DocRect PathRect;
		pAttrMap = pBlendPath->FindAppliedAttributes();
		pBlendPath->m_pPath->GetTrueBoundingRect(&PathRect, 0, pAttrMap);
		if (ListPos != m_OffsetList.end() )
		{
			Offset = *ListPos++;
		/*	PathRect.hix += Offset.m_XOffset;
			PathRect.lox += Offset.m_XOffset;
			PathRect.hiy += Offset.m_YOffset;
			PathRect.loy += Offset.m_YOffset;
		*/
		}
		Rect = Rect.Union(PathRect);
		pAttrMap = NULL;
		pBlendPath = GetNextBlendPath(pBlendPath);
	}
	
	return Rect;
}


/***********************************************************************************************

>   BOOL  BrushRef::SetBitmapFillFlags()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		
	Outputs:	
	Returns:	TRUE if we don't have a bitmap fill, or if we do and successfully made a copy
	Purpose:	Because brush definitions exist independently of documents, but bitmap fills do not.
				Therefore if we make a brush with a bitmap fill and then delete them document we 
				created it in we lose the fill. However the brush still exists, therefore this 
				function searches for a bitmap fill attribute, and sets flags that tell the bitmap
				not to delete itself.
				NOTE: This means that we are responsible for deleting the bitmap fills that belong to us
***********************************************************************************************/

BOOL BrushRef::SetBitmapFillFlags()
{
	// for each blendpath we wish to find out if they have an applied bitmap fill attribute.
	// if so then we want to copy it, and replace it in the attribute map
	BlendPath* pBlendPath = GetFirstBlendPath();

	while (pBlendPath != NULL)
	{
		// there may be both a colour fill and transparency attribute, so get both
		AttrBitmapFill* pBMPFill = pBlendPath->GetAppliedBitmapColourFill();
		if (pBMPFill != NULL)
		{
			// Get the attribute value
			BitmapFillAttribute* pVal = (BitmapFillAttribute*)pBMPFill->GetAttributeValue();

			if (pVal == NULL)
			{
				ERROR3("Couldn't retrieve attribute value");	
				return FALSE;
			}

			KernelBitmap* pKBitmap = pVal->GetBitmap();
			if (pKBitmap != NULL)
			{
				OILBitmap* pOBitmap = pKBitmap->GetActualBitmap();
				
				// tell the bitmaps that they are being used by us so that if the 
				// document closes they don't get deleted
				if (pOBitmap != NULL)
				{
					pOBitmap->SetUsedByBrush(TRUE);
					pKBitmap->SetUsedByBrush(TRUE);
				}

			}
		}
		
		AttrBitmapFill* pTranspFill = pBlendPath->GetAppliedBitmapTranspFill();
		if (pTranspFill != NULL)
		{
			// Get the attribute value
			BitmapFillAttribute* pVal = (BitmapFillAttribute*)pTranspFill->GetAttributeValue();

			if (pVal == NULL)
			{
				ERROR3("Couldn't retrieve attribute value");	
				return FALSE;
			}

			KernelBitmap* pKBitmap = pVal->GetBitmap();
			if (pKBitmap != NULL)
			{
				OILBitmap* pOBitmap = pKBitmap->GetActualBitmap();
				
				// tell the bitmaps that they are being used by us so that if the 
				// document closes they don't get deleted
				if (pOBitmap != NULL)
				{
					pOBitmap->SetUsedByBrush(TRUE);
					pKBitmap->SetUsedByBrush(TRUE);
				}

			}
		}

		pBlendPath = GetNextBlendPath(pBlendPath);
	}
	return TRUE;
}



/***********************************************************************************************

>   void  BrushRef::DeleteBitmapFills()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		
	Outputs:	
	Returns:	-
	Purpose:	Searches for bitmap fill attributes and deletes their bitmaps, see previous function
				for the rationale.
				Note that this function was designed to be called by the BrushDefinition destructor.
				I cannot think of a good reason for wanting to use it anywhere else.
***********************************************************************************************/

void BrushRef::DeleteBitmapFills()
{
	// loop through each blendpath
	BlendPath* pBlendPath = GetFirstBlendPath();

	while (pBlendPath != NULL)
	{
		// retrieve the attribute node
		AttrBitmapFill* pBMPFill = pBlendPath->GetAppliedBitmapColourFill();
		if (pBMPFill != NULL)
		{
			// get the attribute value
			BitmapFillAttribute* pVal = (BitmapFillAttribute*)pBMPFill->GetAttributeValue();
			if (pVal != NULL)
			{
				// get the bitmap
				KernelBitmap* pBitmap = pVal->GetBitmap();
				// if it has been flagged by us then zap it
				if (pBitmap != NULL && pBitmap->IsUsedByBrush())
				{
					pBitmap->SetUsedByBrush(FALSE);
					pBitmap->Detach();
					delete pBitmap;
				}
			}
		}
		pBlendPath = GetNextBlendPath(pBlendPath);
	}
}



/***********************************************************************************************

>   void  BrushRef::DeleteAttributesAndPath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		
	Outputs:	
	Returns:	-
	Purpose:	Goes through all of the blendpaths and sets a flag which ensures that all the 
				applied attributes of this blendpath are deleted upon destruction.  This must not 
				be used under normal circumstances because these attributes will still be in use
				by other objects.  It was designed for when we are blending brush attributes and have
				to generate our attribute maps from scratch
***********************************************************************************************/

void BrushRef::DeleteAttributesAndPath()
{
//	BlendPath* pLastPath = NULL;
	BlendPath* pBlendPath = GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		// ensures that all the applied attributes will be deleted when the object is deleted
		pBlendPath->DeleteAttributesAndPath();
		pBlendPath = GetNextBlendPath(pBlendPath);
	
	}
}


/***********************************************************************************************

>   void  BrushRef::FreeAttributes()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		
	Outputs:	
	Returns:	-
	Purpose:	Goes through the blendpaths and asks them to delete their cached attribute map
***********************************************************************************************/

void BrushRef::FreeAttributes()
{
//	BlendPath* pLastPath = NULL;
	BlendPath* pBlendPath = GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		pBlendPath->SetFreeAttributeFlag(TRUE);
		pBlendPath = GetNextBlendPath(pBlendPath);
	
	}
	//DeleteBitmapFills(); // disabled until I get it working properly
}

/***********************************************************************************************

>   void BrushRef::TranslateTo(DocCoord Destination)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		Destination - the coord to translateto
			
	Outputs:	-
	Returns:	-
	Purpose:	Translates the blendpaths and their applied attributes to Destination
***********************************************************************************************/

void BrushRef::TranslateTo(DocCoord Destination)
{
	BlendPath* pBlendPath = GetFirstBlendPath();
	CCAttrMap* pAttrMap = NULL;
	DocRect BRect;
	DocCoord CurrentCentre;
	Trans2DMatrix Trans;

	while (pBlendPath != NULL)
	{
		// find out where we are
		BRect = pBlendPath->m_pPath->GetBoundingRect();
		CurrentCentre = BRect.Centre();

		// set the translation in the matrix
		Trans.SetTransform(Destination.x - CurrentCentre.x, Destination.y - CurrentCentre.y);
		
		// we want to transform attributes too
		pAttrMap = pBlendPath->FindAppliedAttributes();
		if (pAttrMap == NULL)
		{
			ERROR3("No applied attributes");
		}
		else
		{
			pAttrMap->Transform(Trans);
			pAttrMap = NULL;
		}
		pBlendPath->Transform(Trans);
		
		pBlendPath = GetNextBlendPath(pBlendPath);
	}
}


/***********************************************************************************************

>   void BrushRef::TransformBitmapAttributes(TransformBase& Trans)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		Trans - the transformation to perform
			
	Outputs:	-
	Returns:	-
	Purpose:	When a brush stroke is transformed in the document we do not really need to transform
				the blendpaths however we do need tot transform the attributes, otherwise bitmap
				fills become incorrect as do line widths etc.
				However if we transform all attributes then we also need to transform the blendpaths,
				because such things as radial fills are translated by the brush code from the position
				of the blendpaths.
				It is important however that we do not scale the blendpaths, as the scaling code
				assumes that they always stay the same size.
***********************************************************************************************/

void BrushRef::TransformAttributes(TransformBase& Trans)
{
	BlendPath		   *pBlendPath = GetFirstBlendPath();
	CCAttrMap		   *pAttrMap = NULL;
	
	CCRuntimeClass	   *pType;
	void			   *pVal;
	NodeAttribute	   *pNodeAttr;

	while (pBlendPath != NULL)
	{	
		// we want to transform attributes too
		pAttrMap = pBlendPath->FindAppliedAttributes();
		if (pAttrMap == NULL)
		{
			ERROR3("No applied attributes");
		}
		else
		{
			// monster bodge - currently we only actually want to transform line widths and bitmap fills
			for( CCAttrMap::iterator Pos = pAttrMap->GetStartPosition(); Pos != pAttrMap->GetEndPosition(); )
			{
				// Get attr at position Pos
				pAttrMap->GetNextAssoc( Pos, pType, pVal );

				// pVal is actually an attribute, so get a ptr to it and render it
				pNodeAttr = (NodeAttribute *)pVal;

				if (pNodeAttr->IsABitmapFill()) 
					pNodeAttr->Transform(Trans);
			
				if( pNodeAttr->IsALineWidthAttr() && FALSE != Trans.TransLines )
				{
					INT32 Test = labs( INT32(Trans.GetScalar().MakeDouble() * ((AttrLineWidth*)pNodeAttr)->Value.LineWidth) );
					if (Test >= 10)
						pNodeAttr->Transform(Trans);
				}
				

			}

			pAttrMap = NULL;		
		}
	
		pBlendPath = GetNextBlendPath(pBlendPath);
	}
	return;
}

/*---------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------
------------------The BrushRefBlender Class----------------------------------------------------
----------------------------------------------------------------------------------------------*/

/***********************************************************************************************

> BrushRefBlender::BrushRefBlender()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor.

***********************************************************************************************/

BrushRefBlender::BrushRefBlender()
{
	m_pStartBrushRef	= NULL;  
	m_pEndBrushRef		= NULL;

	m_pTempCoords		= NULL;
	m_pTempVerbs		= NULL;
 	m_pTempFlags		= NULL;
	m_GBlendBuffSize	= 0;
	m_pGBlendBuff		= NULL;
	m_ArrayLength		= 0;
}


/***********************************************************************************************

> BrushRefBlender::~BrushRefBlender()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor.

***********************************************************************************************/

BrushRefBlender::~BrushRefBlender()
{
	DeallocTempBuffers();
}

/***********************************************************************************************

>	void BrushRefBlender::SetBrushref(BrushRef* pBrushRef, BOOL Start)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		the brushref to set, and whether it is the first or last brushref
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor.

***********************************************************************************************/

void BrushRefBlender::SetBrushRef(BrushRef* pBrushRef, BOOL Start)
{
	if (pBrushRef == NULL)
		return;
	if (Start)
		m_pStartBrushRef = pBrushRef;
	else
		m_pEndBrushRef = pBrushRef;
}


/***********************************************************************************************

>   BrushRef*  BrushRefBlender::Blend(BrushRef* pStartBrush, BrushRef* pEndBrush, double Ratio)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		pStartBrush - the first brushref to blend 
				pEndBrush - the other brushref to blend to 
				Ratio - the ratio of that to this
	Outputs:	
	Returns:	a newly allocated brushref created by blending this with pOther
	Purpose:	as above.  A lot of this code is similar to that of NodeBlender::CreateBlends, however
				it is actuall used by the BrushComponent
***********************************************************************************************/

BrushRef* BrushRefBlender::Blend(BrushRef* pStartBrush, BrushRef* pEndBrush, double Ratio)
{
	if (pStartBrush == NULL || pEndBrush == NULL)
	{
		ERROR3("brush ref is NULL in BrushRef::Blend");
		return NULL;
	}

	if (Ratio < 0 || Ratio > 1)
	{
		ERROR3("Invalid blend ratio");
		return NULL;
	}

	// allocate the extra memory that we need
	if (!ReallocTempBuffers(1000))
		return NULL;

	// right, lets go to work.  The first thing to do is translate both sets of paths to 0,0
	// the reason for this is that in the course of its normal existence the blendpaths can get
	// translated all over the place, so in order for our blend to make sense they should at least
	// be centered in the same place
	// We will also want to store their current location in order to translate them back when we're done
//	DocRect StartRect = pStartBrush->GetBoundingRect();
//	DocRect EndRect   = pEndBrush->GetBoundingRect();
//	DocCoord StartCoord = StartRect.Centre();
//	DocCoord EndCoord   = EndRect.Centre();

//	pStartBrush->TranslateTo(DocCoord(0,0));
//	pEndBrush->TranslateTo(DocCoord(0,0));


	// Find num blend paths in start and end, and keep hold of the MAX value
	UINT32 NumPathsInStart = pStartBrush->GetNumBlendPaths();
	UINT32 NumPathsInEnd   = pEndBrush->GetNumBlendPaths();
	UINT32 MaxNumPaths	 = max(NumPathsInStart,NumPathsInEnd);

	if (NumPathsInStart == 0 || NumPathsInEnd == 0)
		return NULL;

	// This object is used to step along the the two lists of objects choosing a pair to go and blend.
	// It mainly comes into play when there are different numbers of objects in the two lists.
	ListStepper Stepper;
	Stepper.Init(NumPathsInStart, NumPathsInEnd, 0);
	INT32 NextPathStart, NextPathEnd;

	//allocate our new brushref
	BrushRef* pBlendedBrushRef = new BrushRef;
	if (pBlendedBrushRef == NULL)
		return NULL;

	// set up our variables, they will be allocated in the loop (slow I know)
	Path*	   pBlendedPath = NULL;
	BlendPath* pNewBlendPath = NULL;
	CCAttrMap* pBlendedAttrMap = NULL;

	// Get the first pair of objects to blend
	Stepper.GetNext(&NextPathStart,&NextPathEnd);

	// The first pair of blend paths become the current blend path pair
	// Get ptrs to these, plus the subpath IDs of these
	BlendPath* 	pCurrBlendPathStart = pStartBrush->GetBlendPath(NextPathStart);
	BlendPath* 	pCurrBlendPathEnd   = pEndBrush->GetBlendPath(NextPathEnd  );
	UINT32 		CurrSubPathIDStart  = pCurrBlendPathStart->GetSubPathID();
	UINT32 		CurrSubPathIDEnd    = pCurrBlendPathEnd  ->GetSubPathID();

	// We also need info on the next blend path pair, so get some vars ready
	BlendPath* 	pNextBlendPathStart = NULL;
	BlendPath* 	pNextBlendPathEnd   = NULL;
	UINT32 		NextSubPathIDStart = CurrSubPathIDStart; // This was uninitialized - AMB guessed a good value
	UINT32 		NextSubPathIDEnd = CurrSubPathIDEnd; // This was uninitialized - AMB guessed a good value

	for (UINT32 NextPath = 0; NextPath < MaxNumPaths; NextPath++)
	{
		// allocate the variables
		pBlendedPath = new Path;
		pBlendedAttrMap = new CCAttrMap(30);
		
		if (pBlendedPath == NULL || pBlendedAttrMap == NULL )
			goto ExitNull;
		if (!pBlendedPath->Initialise())
			goto ExitNull;

		// Get the next pair of objects to blend
		Stepper.GetNext(&NextPathStart,&NextPathEnd);
		
		// if we haven't run out then assign the next path variables
		if (NextPathStart >= 0 && NextPathEnd >= 0)
		{
			pNextBlendPathStart = pStartBrush->GetBlendPath(NextPathStart);
			pNextBlendPathEnd   = pEndBrush->GetBlendPath(NextPathEnd);
			NextSubPathIDStart  = pNextBlendPathStart->GetSubPathID();
			NextSubPathIDEnd    = pNextBlendPathEnd->GetSubPathID();
		}
	

		if (BlendAttributes(pCurrBlendPathStart, pCurrBlendPathEnd, pBlendedAttrMap, Ratio))
		{
			// Blend the paths together, putting the blended path in BlendedPath
			if (BlendPaths(pCurrBlendPathStart,pCurrBlendPathEnd,Ratio))
			{
				// The blended path will be filled  if either of the paths are filled
				// The blended path will be stroked if either of the paths are stroked
				BOOL Filled  = pCurrBlendPathStart->IsFilled()  || pCurrBlendPathEnd->IsFilled();
				BOOL Stroked = pCurrBlendPathStart->IsStroked() || pCurrBlendPathEnd->IsStroked();
					
				if (!pBlendedPath->MakeSpaceInPath(m_ArrayLength)) 
					goto ExitNull;
				pBlendedPath->MergeTwoPaths(m_pTempCoords,m_pTempVerbs,m_pTempFlags,m_ArrayLength,Filled);
				pBlendedPath->IsFilled  = Filled;
				pBlendedPath->IsStroked = Stroked;
				
				// We haven't yet got a complete path to render or pass back if next path has the same ID and is
				// a different path
				// (this applies to the start OR the end path)
				BOOL NotACompletePath = ((CurrSubPathIDStart == NextSubPathIDStart && pCurrBlendPathStart != pNextBlendPathStart) ||
					(CurrSubPathIDEnd   == NextSubPathIDEnd   && pCurrBlendPathEnd   != pNextBlendPathEnd  ));
				
				if (!NotACompletePath)
				{	
					// it all worked, lets allocate a new blendpath
					pNewBlendPath = new BlendPath;
					if (pNewBlendPath == NULL)
						goto ExitNull;
				
					// tell it to use our blended path and attributes
					pNewBlendPath->SetPath(pBlendedPath);
					pNewBlendPath->SetAppliedAttributes(pBlendedAttrMap);

					// give it to the brushref
					pBlendedBrushRef->AddBlendPath(pNewBlendPath);
				}
				else
					goto ExitNull;

				pNewBlendPath = NULL;
				pBlendedPath = NULL;
				pBlendedAttrMap = NULL;
			}
			else // end if blendpaths
				goto ExitNull;
		} // end if blendattributes
		else
			goto ExitNull;
	} // end for

//	DeallocTempBuffers();
	
	// translate the brushes back
	//pStartBrush->TranslateTo(StartCoord);
	//pEndBrush->TranslateTo(EndCoord);
	return pBlendedBrushRef;

// just so I don't have to keep repeating this code, if an allocation fails then come here where everything is deleted
ExitNull:
	if (pBlendedPath != NULL)
		delete pBlendedPath;
	if (pBlendedBrushRef != NULL)
		delete pBlendedBrushRef;
	if (pBlendedAttrMap != NULL)
		delete pBlendedAttrMap;
	if (pNewBlendPath != NULL)
		delete pNewBlendPath;
	ERROR3("Something went wrong in BrushRefBlender::Blend");
	return NULL;
}



/********************************************************************************************

>	DocCoord*  BrushRefBlender::GetCoordArray(UINT32 MinSize);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

DocCoord* BrushRefBlender::GetCoordArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempCoords;

	if (ReallocTempBuffers(MinSize))
		return m_pTempCoords;
	else
		return NULL;
}

/********************************************************************************************

>	PathVerb*  BrushRefBlender::GetVerbArray(UINT32 MinSize);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

PathVerb* BrushRefBlender::GetVerbArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempVerbs;

	if (ReallocTempBuffers(MinSize))
		return m_pTempVerbs;
	else
		return NULL;
}

/********************************************************************************************

>	PathFlags*  BrushRefBlender::GetFlagArray(UINT32 MinSize);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the array should be
	Outputs:	-
	Returns:	Ptr to the array, or NULL if can't get memory
	Purpose:	Used to get an array you can write to.
	SeeAlso:	-

********************************************************************************************/

PathFlags* BrushRefBlender::GetFlagArray(UINT32 MinSize)
{
	MinSize++;
	if (m_TempArraySize >= MinSize) return m_pTempFlags;

	if (ReallocTempBuffers(MinSize))
		return m_pTempFlags;
	else
		return NULL;
}

/********************************************************************************************

>	UINT32*  BrushRefBlender::GetGBlendBuff(UINT32 MinSize);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		MinSize = min size the buffer should be
	Outputs:	-
	Returns:	Ptr to the buffer, or NULL if can't get memory
	Purpose:	Used to get a buffer you can write to.
	SeeAlso:	-

********************************************************************************************/

UINT32* BrushRefBlender::GetGBlendBuff(UINT32 MinSize)
{
	MinSize++;
	if (m_GBlendBuffSize >= MinSize) return m_pGBlendBuff;

	if (m_pGBlendBuff != NULL) CCFree(m_pGBlendBuff);

	m_pGBlendBuff = (UINT32*) CCMalloc(MinSize*sizeof(UINT32));

	if (m_pGBlendBuff != NULL)
		m_GBlendBuffSize = MinSize;
	else
		m_GBlendBuffSize = 0;

	return m_pGBlendBuff;
}


/********************************************************************************************

> BOOL BrushRefBlender::ReallocTempBuffers(UINT32 Size)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		Size = Size the temp arrays should be
	Outputs:	-
	Returns:	-
	Purpose:	Allocates memory for the temp path arrays, and sets the size var to 0
				You can use calls to GetCoordArray(), GetVerbArray() and GetFlagArray() to get the alloced arrays.
	SeeAlso:	-

********************************************************************************************/

BOOL BrushRefBlender::ReallocTempBuffers(UINT32 Size)
{
	DeallocTempBuffers();

	// Allocate the arrays
	m_TempArraySize = Size;
	m_pTempCoords   = (DocCoord*)  CCMalloc(Size*sizeof(DocCoord));
	m_pTempVerbs    = (PathVerb*)  CCMalloc(Size*sizeof(PathVerb));
	m_pTempFlags    = (PathFlags*) CCMalloc(Size*sizeof(PathFlags));

	// If any of the arrays are not allocated, dealloc the alloced ones, and return FALSE
	if (m_pTempCoords == NULL || m_pTempVerbs == NULL || m_pTempFlags == NULL)
	{
		DeallocTempBuffers();
		return FALSE;
	}

	// It's all OK, so return TRUE
	return TRUE;
}

/********************************************************************************************

>	void BrushRefBlender::DeallocTempBuffers()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Releases memory allocated for the temp path arrays, and sets the size var to 0
	SeeAlso:	-

********************************************************************************************/

void BrushRefBlender::DeallocTempBuffers()
{
	if (m_pTempCoords != NULL) { CCFree(m_pTempCoords); m_pTempCoords = NULL; }
	if (m_pTempVerbs  != NULL) { CCFree(m_pTempVerbs);  m_pTempVerbs  = NULL; }
	if (m_pTempFlags  != NULL) { CCFree(m_pTempFlags);  m_pTempFlags  = NULL; }
	m_TempArraySize = 0;

	if (m_pGBlendBuff != NULL) { CCFree(m_pGBlendBuff); m_pGBlendBuff = NULL; }
	m_GBlendBuffSize = 0;
}


/********************************************************************************************

>	BOOL BrushRefBlender::BlendAttributes(BlendPath* pBlendPathStart, BlendPath* pBlendPathEnd,
									CCAttrMap* pBlendedAttrMap,double BlendRatio)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/2000
	Inputs:		
				pBlendPathStart 	= ptr to blend path to blend from
				pBlendPathEnd   	= ptr to blend path to blend to
				pBlendedAttrMap 	= ptr to map to store the blended attributes in
				BlendRatio   		= amount to blend by (0.0 <= BlendPath <= 1.0)
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Blends the attributes of the two BlendPath objects by the amount specified in BlendRatio
				This is pretty much the same as the fn. by the same name in NodeBlender
	SeeAlso:	-

********************************************************************************************/

BOOL BrushRefBlender::BlendAttributes( BlendPath* pBlendPathStart, BlendPath* pBlendPathEnd,
							   CCAttrMap* pBlendedAttrMap,double BlendRatio)
{
	// Check entry params
	BOOL ok = (pBlendPathStart != NULL &&
				pBlendPathEnd != NULL && pBlendedAttrMap != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok) return FALSE;

	// Find the attributes that are applied to the blend paths
	CCAttrMap* pAttrMapStart = pBlendPathStart->FindAppliedAttributes();
	CCAttrMap* pAttrMapEnd   = pBlendPathEnd->FindAppliedAttributes();

	// If either are NULL, return FALSE
	if (pAttrMapStart == NULL || pAttrMapEnd == NULL) return FALSE;

	// These vars are used as params to the CCAttrMap funcs
	CCRuntimeClass	   *pTypeStart;
	void			   *pValStart;
	void			   *pValEnd;
	double				OldBlendRatio = BlendRatio;
	// Process each attribute in turn
	CCAttrMap::iterator	PosStart = pAttrMapStart->GetStartPosition();
	for (;PosStart != pAttrMapStart->GetEndPosition();)
	{
		// Get a ptr to the attr at position PosStart in the start node's attr map
		pAttrMapStart->GetNextAssoc( PosStart, pTypeStart, pValStart );
		NodeAttribute* pNodeAttrStart = (NodeAttribute *)pValStart;
	
		BlendRatio = OldBlendRatio;	
		// Diccon 10/99 When using non-linear profiles for the objects those attributes
		// that make use of control points were not being profiled, making the objects look strange.
		// to avoid this those attributes now share the same profiles as the objects.
	/*	if (pNodeAttrStart->IsAGradFill())
		{
		
			if (!((AttrFillGeometry*)pNodeAttrStart)->IsAColourFill())
			{
				
				BlendRatio = GetObjectRatio();
			
			}
			else
			{
				BlendRatio = GetInvertedAttributeRatio();
			
			}

		}
		if (pNodeAttrStart->IsAFlatFill() || (pNodeAttrStart->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrLineWidth)))
		{
			BlendRatio = GetInvertedAttributeRatio();
		}

		*/		
		// Get a blended attribute
		NodeAttribute* pBlendedNodeAttr = NULL;

		// Find an attr of the same type in the end object's attr list,
		// and blend the two attrs together
		if( pAttrMapEnd->Lookup( pTypeStart, pValEnd ) )
		{
			// We've found a matching end attr, so try to blend it with the start attr

			// Set up the param object to pass to the start attr's blend method
			BlendAttrParam BlendParam;
			
			// Initialise the BlendParam with the end attr and blend ratio
			if (BlendParam.Init(NULL,
								(NodeAttribute *)pValEnd,BlendRatio,COLOURBLEND_FADE,
								pAttrMapStart, pAttrMapEnd))
			{
				// Successfully initialised, so now try blending the attributes
				if (pNodeAttrStart->Blend(&BlendParam))
				{
					// Attrs successfully blended, now get a ptr to the new attr.
					// Once we get the blended attr ptr, it belongs to us, so we have
					// to delete it when it is not needed
					pBlendedNodeAttr = BlendParam.GetBlendedAttr();
				}
			}
		}

		// If we have a blended attr, pBlendedNodeAttr != NULL
		if (pBlendedNodeAttr != NULL)
		{
			// Get the type of the blended attr
			CCRuntimeClass *pTypeBlend = pBlendedNodeAttr->GetAttributeType();
			void	   *pValBlend;

			// If we already have an attr in the blended attr map of the same type,
			// remove it and delete it, before inserting a new attr of this type
			if (pBlendedAttrMap->Lookup(pTypeBlend,pValBlend))
			{
				if (pValBlend != NULL)
				{
					pBlendedAttrMap->RemoveKey(pTypeBlend);
					delete (NodeAttribute*)pValBlend;
				}
			}
			// add it to the blend map
			pBlendedAttrMap->SetAt( pTypeBlend, pBlendedNodeAttr );
		}
	}


	return TRUE;
}


/********************************************************************************************

>	BOOL BrushRefBlender::BlendPaths(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/2000
	Inputs:		pBlendPathStart = ptr to blend path to blend from
				pBlendPathEnd   = ptr to blend path to blend to
				BlendRatio  	= amount to blend by (0.0 <= BlendPath <= 1.0)
	Outputs:	The blended path is stored in three arrays: the coords, the verbs, and the flags.
				The arrays are:
	
					pTempCoords
					pTempVerbs
					pTempFlags

					ArrayLength = the length of all three arrays

				This allows the caller to decide what to do with the blended path in a very flexible way.

	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Blends two BlendPath objects by the amount specified in BlendRatio
				This is pretty much the same as the fn. by the same name in NodeBlender
	SeeAlso:

********************************************************************************************/

BOOL BrushRefBlender::BlendPaths(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio)
{
	// Check entry params
	BOOL    ok = (pBlendPathStart != NULL && pBlendPathEnd != NULL);
 	if (ok) ok = (pBlendPathStart->GetBlendNode() != NULL && pBlendPathEnd->GetBlendNode() != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok) return FALSE;

	// Get the types of the two paths
	PathTypeEnum PathTypeStart = pBlendPathStart->GetPathType();
	PathTypeEnum PathTypeEnd   = pBlendPathEnd  ->GetPathType();

	// The blended path will be closed if either of the paths is a shape
	BOOL Closed = (PathTypeStart == PATHTYPE_SHAPE) || (PathTypeEnd == PATHTYPE_SHAPE);

	Path * pPathStart = NULL;

	// Find the paths associated with the start and end blend paths
	if (pBlendPathStart->GetBlendNode()->IsNodePath())
	{
		pPathStart = &(((NodePath *)pBlendPathStart->GetBlendNode())->InkPath);
	}
	else
	{
		pPathStart = &(((NodePath *)((NodeCompound *)pBlendPathStart->GetBlendNode())->GetNodeToBlend())->InkPath);
	}

	Path * pPathEnd = NULL;

	if (pBlendPathEnd->GetBlendNode()->IsNodePath())
	{
		pPathEnd   = &(((NodePath *)pBlendPathEnd->GetBlendNode())->InkPath);
	}
	else
	{
		pPathEnd = &(((NodePath *)((NodeCompound *)pBlendPathEnd->GetBlendNode())->GetNodeToBlend())->InkPath);
	}

	// Calculate how large the arrays have to be to store the blended path definition
	INT32 DestPathSize = ((pPathStart->GetNumCoords()+pPathEnd->GetNumCoords())*3)+500;

	// Get some arrays used to hold the blended path data, and error if any are NULL
	DocCoord*  	pDestCoords = GetCoordArray(DestPathSize);
	PathVerb*  	pDestVerbs  = GetVerbArray(DestPathSize);
	PathFlags* 	pDestFlags  = GetFlagArray(DestPathSize);
	UINT32* 		pBuff 		= GetGBlendBuff(DestPathSize);
	if (pDestCoords == NULL || pDestVerbs == NULL || pDestFlags == NULL || pBuff == NULL)
		return FALSE;

	// This section copes with the case when blending a line with a shape.
	// In this case we need to get a temp path the is actually a shape version of the line.
	// The line is simply reversed back onto itself to form a shape that would look identical to the 
	// line if rendered.  This allows the line to appear to open up to the shape when blended.
	Path Shape;
	if (PathTypeStart != PathTypeEnd)
	{
		BOOL ok = FALSE;
		if (!Shape.Initialise()) return FALSE;

		// if going from a line to a shape, convert the start path to a shape
		if (PathTypeStart == PATHTYPE_LINE && PathTypeEnd == PATHTYPE_SHAPE)
		{
			ok = NodeBlender::ConvertLineToShape(pPathStart,&Shape);
			pPathStart = &Shape;
		}

		// if going from a shape to a line, convert the end path to a shape
		if (PathTypeStart == PATHTYPE_SHAPE && PathTypeEnd == PATHTYPE_LINE)
		{
			ok = NodeBlender::ConvertLineToShape(pPathEnd,&Shape);
			pPathEnd = &Shape;
		}

		if (!ok) return FALSE;
	}

	BOOL OneToOne = FALSE;
	// Now actually blend the two paths

	GBlend GBlendObj;

	// Define the blend
	GBlendObj.Define(	(PPOINT)pPathStart->GetCoordArray(),	// Specify the start path
						pPathStart->GetVerbArray(),
						pPathStart->GetNumCoords(),

						(PPOINT)pPathEnd  ->GetCoordArray(),	// Specify the end path
						pPathEnd  ->GetVerbArray(),
						pPathEnd  ->GetNumCoords(),

						OneToOne,								// The one-to-one flag
						FLATNESS,								// Flatness

						pBuff,									// Buffer for GBlend to use
						DestPathSize*sizeof(UINT32));			// The buffer size

	// Blend the paths
	m_ArrayLength = GBlendObj.Blend(BlendRatio,					// The blend ratio, 0.0 < BlendRatio < 1.0
									(PPOINT)pDestCoords,		// Array to store blended coords
									pDestVerbs,					// Array to store blended verbs
									DestPathSize);				// The num elements in the two arrays


	// If we're blending a line to another line, we need to make sure that the blended line
	// is going in a direction that corresponds to the source lines.  This ensures attributes
	// that depend on this direction (e.g. start and end arrows) look correct.
	//
	// When creating blend paths of lines, we can detect if the blend path has been reversed,
	// in relation to the original path, by the original mapping value.
	// If it's 0 it has NOT been reversed, otherwise it's been reversed.
	//
	// If the blend ratio is <=0.5, the blended path is closest to the start blend path,
	// so we look at the start blend path's original mapping.
	//
	// If blend ration > 0.5, look at the end blend path's original mapping.
	//
	// The (BlendRation <= 0.5) cut-off point is the same as the cut-off point used in the blending
	// of attributes.
	if (pBlendPathStart->IsLine() && pBlendPathEnd->IsLine())
	{
		BlendPath* pBlendPath;
		if (BlendRatio <= 0.5) 
			pBlendPath = pBlendPathStart;
		else
			pBlendPath = pBlendPathEnd;

		if (pBlendPath->GetOrigMapping() != 0)
			NodeBlender::ReversePath(pDestCoords,pDestVerbs,m_ArrayLength);
	}

	// We need to do some work on the blended path
	if (!NodeBlender::ProcessBlendedPath(pDestCoords,pDestVerbs,pDestFlags,m_ArrayLength,Closed))
		return FALSE;

	return TRUE;
}


