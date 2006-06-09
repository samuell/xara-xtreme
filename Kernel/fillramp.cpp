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
#include "fillramp.h"
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blendatt.h"
#include "colormgr.h"
#include "colcomp.h"		// for ColourListComponent
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNCREATE(RampItem, ListItem);
CC_IMPLEMENT_DYNCREATE(ColRampItem, RampItem);
CC_IMPLEMENT_DYNCREATE(TranspRampItem, RampItem);

CC_IMPLEMENT_DYNAMIC(FillRamp, List);
CC_IMPLEMENT_DYNAMIC(ColourRamp, FillRamp);
CC_IMPLEMENT_DYNAMIC(TransparencyRamp, FillRamp);

// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/***********************************************************************************************

>	RampItem::RampItem()
	RampItem::RampItem(float pos)
	RampItem::RampItem(const RampItem &other)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Purpose:	Constructors for ramp items

***********************************************************************************************/

RampItem::RampItem()
{
	Position=0.0f;
	Selected=FALSE;
}

RampItem::RampItem(float pos)
{
	Position=pos;
	Selected=FALSE;
	ClampPosition();
}

RampItem::RampItem(const RampItem &other)
{
	Position=other.GetPosition();
	Selected=other.GetSelState();
}


/***********************************************************************************************

>	void RampItem::ClampPosition()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Purpose:	Make sure the position field is in range

***********************************************************************************************/

void RampItem::ClampPosition()
{
	if (Position<0.01f) Position=0.01f;
	if (Position>0.99f) Position=0.99f;
}


/***********************************************************************************************

>	void RampItem::SetPosition(float pos)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		pos = a position value,  0<=pos<=1
	Purpose:	Set the position in the ramp for this colour

***********************************************************************************************/

void RampItem::SetPosition(float pos)
{
	Position=pos;
	ClampPosition();
}

/***********************************************************************************************

>	void RampItem::SetSelState(BOOL selstate)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		a bool to set the state
	Purpose:	Set the state of the selection flag in this colour ramp item

***********************************************************************************************/

void RampItem::SetSelState(BOOL selstate)
{
	Selected=selstate;
}

/***********************************************************************************************

>	class ColRampItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Purpose:	A instance of a single colour ramp entry

***********************************************************************************************/

ColRampItem::ColRampItem()
{
}

ColRampItem::ColRampItem(float pos, DocColour *pCol) : RampItem(pos)
{
	Colour=(*pCol);
}

ColRampItem::ColRampItem(const ColRampItem& other) : RampItem(other)
{
	Colour=other.GetColour();
}


/***********************************************************************************************

>	void ColRampItem::SetColour(DocColour *pCol)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		col = a document colour
	Purpose:	used to set the colour of this ramp entry

***********************************************************************************************/

void ColRampItem::SetColour(DocColour *pCol)
{
	Colour=(*pCol);
}


/***********************************************************************************************

>	INT32 ColRampItem::operator==(const ColRampItem& other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		other = the item to check this against
	Returns:	TRUE if this item is equivalent to other.
				FALSE if not
	Purpose:	Check for ColRampItem equality. Do we get this as an internal function?

***********************************************************************************************/

INT32 ColRampItem::operator==(const ColRampItem& other) const
{
	return (	(Position == other.GetPosition()) && 
				(Colour   == other.GetColour()) &&
				(Selected == other.GetSelState())
		   );
}

/***********************************************************************************************

>	TranspRampItem::TranspRampItem()
	TranspRampItem::TranspRampItem(float pos, UINT32 transp) : RampItem(pos)
	TranspRampItem::TranspRampItem(const TranspRampItem& other)	: RampItem(other)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Purpose:	Various constructors for the transparency ramp item class

***********************************************************************************************/

TranspRampItem::TranspRampItem()
{
}

TranspRampItem::TranspRampItem(float pos, UINT32 transp) : RampItem(pos)
{
	Transparency=transp;
}

TranspRampItem::TranspRampItem(const TranspRampItem& other)	: RampItem(other)
{
	Transparency=other.GetTransparency();
}


/***********************************************************************************************

>	void TranspRampItem::SetTransparency(UINT32 transp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		col = a document colour
	Purpose:	used to set the colour of this ramp entry

***********************************************************************************************/

void TranspRampItem::SetTransparency(UINT32 transp)
{
	Transparency=transp;
}


/***********************************************************************************************

>	INT32 TranspRampItem::operator==(const TranspRampItem& other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		other = the item to check this against
	Returns:	TRUE if this item is equivalent to other.
				FALSE if not
	Purpose:	Check for ColRampItem equality. Do we get this as an internal function?

***********************************************************************************************/

INT32 TranspRampItem::operator==(const TranspRampItem& other) const
{
	return (	(Position	  == other.GetPosition()) && 
				(Transparency == other.GetTransparency()) &&
				(Selected	  == other.GetSelState())
		   );
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FillRamp
//
//	Author:		Mike
//	Created:	2/2/97
//	Purpose:	The base class for our fill ramps. We keep an item of interest which follows
//				the last selected or clicked item
//
////////////////////////////////////////////////////////////////////////////////////////////////


/***********************************************************************************************

>	RampItem* FillRamp::FindInsertPosition(float pos, INT32* pInsert, UINT32* pIndex)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		pos = position of the current entry
	Outputs:	pInsert = 0, then insert the entry before ColRampItem*
						= 1, then insert the entry after ColRampItem*
				index   = then index of the returned item.
	Returns:	RampItem* , a pointer to an item to place pos before or after, 
						    NULL if the list is empty.
	Purpose:	Finds an insert context given a position. The idea is, if you have a new 
				record to add, it needs to be added to the list after all items with a position
				less than pos. This func finds the insert position and returns a value indicating
				before or after. The index of your new item will depend on the pInsert value. If
				0, then the new index will be (*pIndex) otherwise its (*pIndex)+1;

***********************************************************************************************/

RampItem* FillRamp::FindInsertPosition(float pos, INT32* pInsert, UINT32* pIndex) const
{
	ERROR3IF(pInsert==NULL, "NULL insert pointer passed to FillRamp::FindInsertPosition()");

	// need to search for
	RampItem *pLastItem=NULL, *pItem = (RampItem*)GetHead();
	(*pInsert) = 1;
	(*pIndex) = FILLRAMP_INDEXBASE-1;

	while (pItem)
	{
		(*pIndex)++;
		if (pItem->GetPosition() > pos)
		{
			(*pInsert) = 0;
			return pItem;
		}
		pLastItem = pItem;
		pItem = (RampItem*)GetNext(pItem);	
	}
	return pLastItem;
}


/***********************************************************************************************

>	UINT32 FillRamp::InsertNewItem(RampItem* pNewItem)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		pNewItem = a new item to insert in the list.
	Outputs:	-
	Returns:	the index of the inserted item
	Purpose:	Adds pNewItem to the growing list

***********************************************************************************************/

UINT32 FillRamp::InsertNewItem(RampItem* pNewItem)
{
	ERROR3IF(pNewItem==NULL, "NULL pointer passed to FillRamp::InsertNewItem");

	// keep an ordered list on pos please!
	INT32 insert;
	UINT32 index;
	RampItem *pContext = FindInsertPosition(pNewItem->GetPosition(),&insert,&index);
	if (pContext==NULL)
	{
		AddTail(pNewItem);
		return FILLRAMP_INDEXBASE;
	}
	if (insert==0)
	{
		InsertBefore(pContext, pNewItem);
		return index;
	}
	InsertAfter(pContext, pNewItem);
	return (index+1);
}


/***********************************************************************************************

>	void FillRamp::DeselectAll() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		-
	Purpose:	Deselect all selection states in this fill ramp

***********************************************************************************************/

void FillRamp::DeselectAll() const
{
	RampItem *pItem = (RampItem*)GetHead();
	while (pItem)
	{
		pItem->SetSelState(FALSE);
		pItem = (RampItem*)GetNext(pItem);	
	}
}

/***********************************************************************************************

>	void FillRamp::SortRamp()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/99
	Inputs:		
	Outputs:	
	Purpose:	Ensures all positions of all items in the list are in numerical order

***********************************************************************************************/
void FillRamp::SortRamp()
{
	// bubble sort algorithm - not fast but should be fast enough for our purposes
	RampItem * pPrevItem = (RampItem *)GetHead();
	RampItem * pThisItem = (RampItem *)GetNext(pPrevItem);

	while (pThisItem)
	{
		if (pPrevItem->GetPosition() > pThisItem->GetPosition())
		{
			// swap the values around, and then restart the list
			// first, remove this item
			RampItem * pRemovedItem = (RampItem *)RemoveItem(pThisItem);
			
			if (InsertBefore(pPrevItem, pRemovedItem) == NULL)
			{
				ERROR3("Insert before failed");
				return;
			}

			pPrevItem = (RampItem *)GetHead();
			pThisItem = (RampItem *)GetNext(pPrevItem);
		}
		// now, check for multiple points
		else if (pPrevItem->GetPosition() == pThisItem->GetPosition())
		{
			pThisItem->SetPosition(pPrevItem->GetPosition() + 0.01f);
			pThisItem = (RampItem *)GetNext(pThisItem);
			pPrevItem = (RampItem *)GetNext(pPrevItem);
		}
		else
		{
			pThisItem = (RampItem *)GetNext(pThisItem);
			pPrevItem = (RampItem *)GetNext(pPrevItem);
		}
	}
}



/***********************************************************************************************

>	void FillRamp::Blend (BlendAttrParam* pBlendParam, FillRamp* OtherRamp)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/2000
	Inputs:		
	Outputs:	
	Purpose:	Blends mutistage fill ramps.  Well actually its support functions do, but
				this one works out which blending case we are doing.

				CGS:  I've added the ability to avoid inverting ratios (by passing notInvert as TRUE).
				(only DoManyToNoneBlend () makes use of this at the moment)

***********************************************************************************************/

void FillRamp::Blend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillGeometryAttribute* pOtherFill, BOOL notInvert)
{
	//ERRORIF (!pOtherFill, "Dodgy entry params!", NULL);

	FillRamp* OtherRamp = pOtherFill->GetColourRamp ();
	
	// if neither contains a fill ramp - then return
	if ((this == NULL) && (OtherRamp == NULL))
	{
		return;
	}

	// ok we've got at least one fillramp - so proceed ....
	// now decide the type of blending that we need to do ....
	
	enum MappingType { MT_ONETOONE,			// equal numbers of fill blobs
					   MT_NONETOMANY,		// no fill blobs to many
					   MT_MANYTONONE,		// many fill blobs to none
					   MT_MANYTOMANY,		// many fills blobs to many fills blobs
											// (but different numbers of fill blobs)
					 //MT_MANYTOMANYLESS,	// less fill blobs to more fill blobs
					 //MT_MANYTOMANYMORE,	// more fill blobs to less fill blobs
					   MT_NONE };			// sensible default
	
	MappingType FunctionMapper = MT_NONE;

	if (this == NULL)
	{
		if (OtherRamp != NULL)
		{
			FunctionMapper = MT_NONETOMANY;
		}
		// else - nothing (this is impossible)
	}
	else if (OtherRamp == NULL)
	{
		if (this != NULL)
		{
			FunctionMapper = MT_MANYTONONE;
		}
		// else - nothing (this is impossible)
	}
	else
	{
		if (GetCount () == OtherRamp->GetCount ())
		{
			FunctionMapper = MT_ONETOONE;
		}
		else
		{
			// were in the dreaded many-to-many case ....

			FunctionMapper = MT_MANYTOMANY;
		}
	}

	// invoke the appropriate support function ....

	switch (FunctionMapper)
	{
		case MT_ONETOONE:
		{
			DoOneToOneBlend (pNewAttr, pBlendParam, OtherRamp, notInvert);
		}
		break;
		case MT_NONETOMANY:
		{
			DoNoneToManyBlend (pNewAttr, pBlendParam, OtherRamp, notInvert);
		}
		break;
		case MT_MANYTONONE:
		{
			DoManyToNoneBlend (pNewAttr, pBlendParam, pOtherFill, notInvert);
		}
		break;
		case MT_MANYTOMANY:
		{
			DoManyToManyBlend (pNewAttr, pBlendParam, OtherRamp, notInvert);
		}
		break;
		default:
			return;		// um .....
		break;
	}
}



/***********************************************************************************************

>	void FillRamp::DoOneToOneBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/2000
	Inputs:		
	Outputs:	
	Purpose:	Blends mutistage fill ramps.  This function blends equal numbers of blobs.

***********************************************************************************************/

void FillRamp::DoOneToOneBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp, BOOL notInvert)
{
	// find what point along the blend we are at
	double Ratio = pBlendParam->GetBlendRatio ();

	RampItem* pItem = (RampItem*) GetHead ();
	RampItem* pOtherItem = (RampItem*) OtherRamp->GetHead ();

	// simply interpolate between the respective colour ramp blobs ....

	DocColour BlendedColour;

	ColourContext *cc = ColourManager::GetColourContext (COLOURMODEL_RGBT);
	
	while (pItem && pOtherItem)
	{
		DocColour StartColour = ((ColRampItem*) pItem->GetItem ())->GetColour ();
		DocColour EndColour = ((ColRampItem*) pOtherItem->GetItem ())->GetColour (); 
		BlendedColour.Mix (
							&StartColour,		// start blend colour
							&EndColour,			// end blend colour
							Ratio,				// the blend ratio
							cc					// RGB space please
						  );

		((ColRampItem*) pItem->GetItem ())->SetColour (&BlendedColour);
		
		pItem = (RampItem*) GetNext (pItem);
		pOtherItem = (RampItem*) OtherRamp->GetNext (pOtherItem);
	}
}



/***********************************************************************************************

>	void FillRamp::DoNoneToManyBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/2000
	Inputs:		
	Outputs:	
	Purpose:	Blends mutistage fill ramps.  This function blends none to many blobs.

***********************************************************************************************/

void FillRamp::DoNoneToManyBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp, BOOL notInvert)
{
	// find what point along the blend we are at
	double Ratio = pBlendParam->GetBlendRatio ();

	DocColour* StartCol = pNewAttr->GetStartColour ();
	DocColour* EndCol = pNewAttr->GetEndColour ();

	// first see if weve got something that we can safely blend with
	// if either colour is NULL, then forget it; just set the colour ramp to NULL ....

	if (!((StartCol == NULL) && (EndCol == NULL)))
	{
		ColourRamp* NewColourRamp = new ColourRamp ();

		DocColour BlendedColour;
		DocColour BlendedColour2;

		ColourContext *cc = ColourManager::GetColourContext (COLOURMODEL_RGBT);

		if (NewColourRamp)
		{
			ColRampItem* pOtherItem = (ColRampItem*) OtherRamp->GetHead ();

			while (pOtherItem)
			{
				ColRampItem* pNewItem = new ColRampItem (*pOtherItem);
				
				// first mix the two endpoints based upon the other fillramp blob positions ....
				
				BlendedColour.Mix (
									StartCol,		// start blend colour
									EndCol,			// end blend colour
									((ColRampItem*) pOtherItem->GetItem ())->GetPosition (),	// the blend ratio
									cc															// RGB space please
								  );

				// then mix this new colour with the other fillramp blob colours ....

				DocColour EndColour = ((ColRampItem*) pOtherItem->GetItem ())->GetColour ();
				BlendedColour2.Mix (
									&BlendedColour,
									&EndColour,
									Ratio,													// the blend ratio
									cc														// RGB space please
								   );

				pNewItem->SetColour (&BlendedColour2);

				NewColourRamp->InsertNewItem (pNewItem);
				
				pOtherItem = (ColRampItem*) OtherRamp->GetNext (pOtherItem);
			}
			pNewAttr->SetColourRamp (NewColourRamp);
		}

		delete (NewColourRamp);
	}
	else
	{
		pNewAttr->SetColourRamp (NULL);
	}
}



/***********************************************************************************************

>	void FillRamp::DoManyToNoneBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/2000
	Inputs:		
	Outputs:	
	Purpose:	Blends mutistage fill ramps.  This function blends many to no none fill blobs.

***********************************************************************************************/

void FillRamp::DoManyToNoneBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillGeometryAttribute* pOtherFill, BOOL notInvert)
{
	// find what point along the blend we are at
	double Ratio = 1;
		
	if (!notInvert)
	{
		Ratio = 1 - pBlendParam->GetBlendRatio ();
	}
	else
	{
		Ratio = pBlendParam->GetBlendRatio ();
	}

	DocColour* StartCol = pOtherFill->GetStartColour ();//pNewAttr->GetStartColour ();
	DocColour* EndCol = pOtherFill->GetEndColour ();//pNewAttr->GetEndColour ();

	if (!EndCol)
	{
		EndCol = StartCol;
	}

	if ((EndCol == NULL) && (StartCol == NULL))
	{
		// do nothing
		return;
	}

	ColourRamp* NewColourRamp = new ColourRamp ();

	DocColour BlendedColour;
	DocColour BlendedColour2;

	ColourContext *cc = ColourManager::GetColourContext (COLOURMODEL_RGBT);

	if (NewColourRamp)
	{
		ColRampItem* pItem = (ColRampItem*) GetHead ();

		while (pItem)
		{
			ColRampItem* pNewItem = new ColRampItem (*pItem);
			
			// first mix the two endpoints based upon the other fillramp blob positions ....
			
			BlendedColour.Mix (
								StartCol,		// start blend colour
								EndCol,			// end blend colour
								((ColRampItem*) pItem->GetItem ())->GetPosition (),	// the blend ratio
								cc															// RGB space please
							  );

			// then mix this new colour with the other fillramp blob colours ....
			DocColour EndColour = ((ColRampItem*) pItem->GetItem ())->GetColour ();
			BlendedColour2.Mix (
								&BlendedColour,
								&EndColour,
								Ratio,													// the blend ratio
								cc														// RGB space please
							   );

			pNewItem->SetColour (&BlendedColour2);

			NewColourRamp->InsertNewItem (pNewItem);
			
			pItem = (ColRampItem*) GetNext (pItem);
		}
		pNewAttr->SetColourRamp (NewColourRamp);
	}
	
	delete (NewColourRamp);
}

// the following two functions do the following to achieve blending ....

/*
	Fill (less blobs):			x			x			x					x		
	Mapping:			|			|			|					|				|
	Fill (more blobs):	x			x			x					x				x
*/

/*	'superimposes' the ramp positions of that with with the greatest onto that with the lesser.
	 colour values are then ccalculated for for these new blobs with respect to the ranges of the
	 old ....
*/

/***********************************************************************************************

>	void FillRamp::DoManyToManyBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/2000
	Inputs:		
	Outputs:	
	Purpose:	Blends mutistage fill ramps.  This function handles the many to many case.

***********************************************************************************************/

void FillRamp::DoManyToManyBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp, BOOL notInvert)
{
	// find what point along the blend we are at
	double Ratio = pBlendParam->GetBlendRatio ();

	DocColour BlendedColour;
	ColourContext *cc = ColourManager::GetColourContext (COLOURMODEL_RGBT);

	RampItem* pItem = (RampItem*) GetHead ();
	RampItem* pOtherItem = (RampItem*) OtherRamp->GetHead ();

	ColourRamp* NewColourRamp1 = new ColourRamp ();
	*NewColourRamp1 = *((ColourRamp*) this);
	ColourRamp* NewColourRamp2 = new ColourRamp ();
	*NewColourRamp2 = *((ColourRamp*) OtherRamp);
	ColourRamp* NewColourRamp3 = new ColourRamp ();
	
	ColRampItem* pFirstItem = (ColRampItem*) GetHead ();
	ColRampItem* pSecondItem = (ColRampItem*) OtherRamp->GetHead ();

	DocColour* StartCol = NULL;//pNewAttr->GetStartColour ();
	DocColour* EndCol = NULL;//&pFirstItem->GetColour ();

	DocColour tempStartCol;
	DocColour tempEndCol; // these avoid taking the address of a temporary

	// now insert the first blobs into the second ....

	float PrevPos = -1, NextPos = -1;

	while (pSecondItem)
	{
		ColRampItem* pNewItem = new ColRampItem (*pSecondItem);

		NewColourRamp1->InsertNewItem (pNewItem);

		ColRampItem* pPrevItem = (ColRampItem*) NewColourRamp1->GetPrev (pNewItem);
		ColRampItem* pNextItem = (ColRampItem*) NewColourRamp1->GetNext (pNewItem);

		if (!pPrevItem)
		{
			StartCol = pNewAttr->GetStartColour ();
			PrevPos = 0.0;
		}
		else
		{
			tempStartCol = pPrevItem->GetColour ();
			StartCol = &tempStartCol;
			PrevPos = pPrevItem->GetPosition ();
		}

		if (!pNextItem)
		{
			EndCol = pNewAttr->GetEndColour ();
			NextPos = 1.0;
		}
		else
		{
			tempEndCol = pNextItem->GetColour ();
			EndCol = &tempEndCol;
			NextPos = pNextItem->GetPosition ();
		}

		BlendedColour.Mix (
								StartCol,						// start blend colour
								EndCol,							// end blend colour
								NextPos - PrevPos,				// the blend ratio
								cc								// RGB space please
							  );

		pNewItem->SetColour (&BlendedColour);
		
		pSecondItem = (ColRampItem*) GetNext (pSecondItem);
	}

	pFirstItem = (ColRampItem*) GetHead ();
	pSecondItem = (ColRampItem*) OtherRamp->GetHead ();

	// now insert the first blobs into the second ....

	PrevPos = -1, NextPos = -1;

	while (pFirstItem)
	{
		ColRampItem* pNewItem = new ColRampItem (*pFirstItem);

		NewColourRamp2->InsertNewItem (pNewItem);

		ColRampItem* pPrevItem = (ColRampItem*) NewColourRamp2->GetPrev (pNewItem);
		ColRampItem* pNextItem = (ColRampItem*) NewColourRamp2->GetNext (pNewItem);

		if (!pPrevItem)
		{
			StartCol = pNewAttr->GetStartColour ();
			PrevPos = 0.0;
		}
		else
		{
			tempStartCol = pPrevItem->GetColour ();
			StartCol = &tempStartCol;
			PrevPos = pPrevItem->GetPosition ();
		}

		if (!pNextItem)
		{
			EndCol = pNewAttr->GetEndColour ();
			NextPos = 1.0;
		}
		else
		{
			tempEndCol = pNextItem->GetColour ();
			EndCol = &tempEndCol;
			NextPos = pNextItem->GetPosition ();
		}

		BlendedColour.Mix (
								StartCol,						// start blend colour
								EndCol,							// end blend colour
								NextPos - PrevPos,				// the blend ratio
								cc								// RGB space please
							  );

		pNewItem->SetColour (&BlendedColour);
		
		pFirstItem = (ColRampItem*) GetNext (pFirstItem);
	}
	
	// simply interpolate between the respective colour ramp blobs ....

	pItem = (RampItem*) NewColourRamp1->GetHead ();
	pOtherItem = (RampItem*) NewColourRamp2->GetHead ();
	
	while (pItem && pOtherItem)
	{
		ColRampItem* pNewItem = new ColRampItem (*((ColRampItem*) pItem));
		
		NewColourRamp3->InsertNewItem (pNewItem);
		
		tempStartCol = ((ColRampItem*) pItem->GetItem ())->GetColour ();
		tempEndCol = ((ColRampItem*) pOtherItem->GetItem ())->GetColour ();
		BlendedColour.Mix (
							&tempStartCol,		// start blend colour
							&tempEndCol,		// end blend colour
							Ratio,				// the blend ratio
							cc					// RGB space please
						  );

		pNewItem->SetColour (&BlendedColour);
		
		pItem = (RampItem*) NewColourRamp1->GetNext (pItem);
		pOtherItem = (RampItem*) NewColourRamp2->GetNext (pOtherItem);
	}
	pNewAttr->SetColourRamp (NewColourRamp3);

	delete (NewColourRamp1);
	delete (NewColourRamp2);
	delete (NewColourRamp3);
}



/***********************************************************************************************

>	BOOL FillRamp::RotateSelRight(BOOL RollIn) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		RollIn = a boolean value rolled in from the previous item on the left
	Outputs:	A bool, the state of the last item rolled out of the right
	Purpose:	Roll the selection state of the items in the list to the right. That equates to
				rolling the selection down the list by one element.

***********************************************************************************************/

FillControl FillRamp::RotateSelRight(BOOL StartPointSelected, BOOL EndPointSelected, FillControl& OldSelectedBlob) const
{
	if (StartPointSelected)
	{
		// select first blob in list ....
		RampItem *pItem = (RampItem*) GetHead();
		pItem->SetSelState (TRUE);

		FillControl Ret = GetSelectedIndex ();
		pItem->SetSelState (FALSE);

		OldSelectedBlob = FILLCONTROL_NULL;
		
		return (Ret);
	}

	if (EndPointSelected)
	{
		OldSelectedBlob = FILLCONTROL_ENDPOINT;
		return (FILLCONTROL_STARTPOINT);
	}

	// if we haven't returned yet, then the selection will within our ramp ....

	RampItem* pItem = (RampItem*) GetHead();
	RampItem* pSelectedBlob = NULL;
	while (pItem)
	{
		if (pItem->IsSelected ())
		{
			pSelectedBlob = pItem;
			break;
		}
		pItem = (RampItem*)GetNext(pItem);
	}

	RampItem* pSelNext = (RampItem*) GetNext (pSelectedBlob);

	if (pSelNext != NULL)
	{
		OldSelectedBlob = GetSelectedIndex ();
		pSelectedBlob->SetSelState (FALSE);
		pSelNext->SetSelState (TRUE);

		FillControl Ret = GetSelectedIndex ();
		pSelNext->SetSelState (FALSE);
		
		return (Ret);
	}
	else
	{
		OldSelectedBlob = GetSelectedIndex ();
		pSelectedBlob->SetSelState (FALSE);
		return (FILLCONTROL_ENDPOINT);
	}
}

/***********************************************************************************************

>	BOOL FillRamp::RotateSelLeft(BOOL RollIn) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		RollIn = a boolean value rolled in from the previous item on the right
	Outputs:	A bool, the state of the last item rolled out of the left
	Purpose:	Roll the selection state of the items in the list to the left. That equates to
				rolling the selection up the list by one element.

***********************************************************************************************/

BOOL FillRamp::RotateSelLeft(BOOL StartPointSelected, BOOL EndPointSelected, FillControl& OldSelectedBlob) const
{
	if (StartPointSelected)
	{
		OldSelectedBlob = FILLCONTROL_STARTPOINT;
		return (FILLCONTROL_ENDPOINT);
	}

	if (EndPointSelected)
	{
		// select first blob in list ....
		RampItem *pItem = (RampItem*) GetTail();
		pItem->SetSelState (TRUE);

		FillControl Ret = GetSelectedIndex ();
		pItem->SetSelState (FALSE);

		OldSelectedBlob = FILLCONTROL_NULL;
		
		return (Ret);
	}

	// if we haven't returned yet, then the selection will within our ramp ....

	RampItem* pItem = (RampItem*) GetTail();
	RampItem* pSelectedBlob = NULL;
	while (pItem)
	{
		if (pItem->IsSelected ())
		{
			pSelectedBlob = pItem;
			break;
		}
		pItem = (RampItem*)GetPrev(pItem);
	}

	RampItem* pSelNext = (RampItem*) GetPrev (pSelectedBlob);

	if (pSelNext != NULL)
	{
		OldSelectedBlob = GetSelectedIndex ();
		pSelectedBlob->SetSelState (FALSE);
		pSelNext->SetSelState (TRUE);

		FillControl Ret = GetSelectedIndex ();
		pSelNext->SetSelState (FALSE);
		
		return (Ret);
	}
	else
	{
		OldSelectedBlob = GetSelectedIndex ();
		pSelectedBlob->SetSelState (FALSE);
		return (FILLCONTROL_STARTPOINT);
	}

	/* perform a ROL on the selection in this fill ramp
	BOOL Temp;
	RampItem *pItem = (RampItem*)GetTail();
	while (pItem)
	{
		Temp = pItem->GetSelState();
		pItem->SetSelState(RollIn);
		RollIn = Temp;
		pItem = (RampItem*)GetPrev(pItem);	
	}
	return RollIn;*/
}
	

/***********************************************************************************************

>	UINT32 FillRamp::CountSelBlobs() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		-
	Returns:	The number of selected items in this list
	Purpose:	Simply count all the selected items in this list.

***********************************************************************************************/

UINT32 FillRamp::CountSelBlobs() const
{
	// perform a ROL on the selection in this fill ramp
	UINT32 count=0;
	RampItem *pItem = (RampItem*)GetHead();
	while (pItem)
	{
		count += ((pItem->GetSelState()==TRUE) ? 1 : 0);
		pItem = (RampItem*)GetNext(pItem);	
	}
	return count;
}


/***********************************************************************************************

>	INT32 FillRamp::GetSelectedIndex() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		-
	Returns:	The index of the first selected item in the ramp list [0..n-1]
				the index is adjusted by FILLRAMP_INDEXBASE
				-1 if there are no selected items in this ramp
	Purpose:	Return the index of the first selected item in this list. The index is
				adjusted by FILLRAMP_INDEXBASE i.e. this constant value is always added to
				an index to give the range [FILLRAMP_INDEXBASE...FILLRAMP_INDEXBASE+n-1].
				This is done so one can distinguish between indexes for fill geometry end
				points (usually 0..7 or thereabouts) and the fill ramp blobs.

***********************************************************************************************/

INT32 FillRamp::GetSelectedIndex() const
{
	UINT32 index=FILLRAMP_INDEXBASE;
	RampItem *pItem = (RampItem*)GetHead();
	while (pItem)
	{
		if (pItem->GetSelState())
			return index;
		index++;
		pItem = (RampItem*)GetNext(pItem);	
	}
	return -1;
}


/***********************************************************************************************

>	BOOL FillRamp::GetIndexRange(UINT32 *pFirst, UINT32 *pLast) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		-
	Outputs:	pFirst = assigned the index of the first ramp item
				pLast  = assigned the index of the last ramp item
	Returns:	FALSE if the list is actually empty
				TRUE if there are entries in this list
	Purpose:	Return the index of the first and last entries in this list. As the list
				indexes are biased by an unknown value, at least unknown to everyone but the
				author, one needs to use this class function to find out the values of the
				first and last indexes. All I'll tell you is that they behaviour linearly
				between the two extreems. 

***********************************************************************************************/

BOOL FillRamp::GetIndexRange(UINT32 *pFirst, UINT32 *pLast) const
{
	INT32 c=GetCount();
	if (pFirst!=NULL)
		*pFirst = FILLRAMP_INDEXBASE;
	if (pLast!=NULL)
		*pLast  = FILLRAMP_INDEXBASE+c-1;
	return (c>0);
}
	



/***********************************************************************************************

>	BOOL FillRamp::HitBlob(GeomType geom, DocCoord p0, DocCoord p1, DocCoord ClickPos) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/97
	Inputs:		geom = the geometry, we currently support linear and circular
				p0,p1 depend on geom
				geom = GEOM_LINEAR
					p0 = a coord describing the start of a line
					p1 = a coord describing the start of a line or a point on the cir
				geom = GEOM_CIRCULAR
					p0 = a coord describing the centre of a circle
					p1 = a coord describing the start of a line or a point on the cir
	
	Returns:	TRUE if a blob was hit
				FALSE if no blob was hit
	Effects:	LastItemOfInterest is set to NULL if no blob was hit, or points to the item hit
	Purpose:	Check to see if a ramp item was hit by this click. If so return yes. We also
				set and internal pointer on the result of this check, which can be used to
				make edits.

***********************************************************************************************/

UINT32 FillRamp::HitBlob(FillGeometryAttribute *pAttrValue, DocCoord& ClickPos) const
{
	DocRect BlobRect;
	DocCoord point;
	float pos;
	
	if (pAttrValue!=NULL)
	{	
		// search from the last first
		RampItem *pItem = (RampItem*)GetHead();
		UINT32 i = FILLRAMP_INDEXBASE;
		// find a suitable blob rect around this point			
		(GetApplication()->GetBlobManager())->GetBlobRect(ClickPos, &BlobRect, TRUE);
		while (pItem)
		{
			pos = pItem->GetPosition();
			point = pAttrValue->GetGeometryCoord(pos);
			// See if the Click Position is within the rectangle
			if ( BlobRect.ContainsCoord(point) )
				return i;
			// otherwise continue onwards
			i++;
			pItem = (RampItem*)GetNext(pItem);	
		}
	}
	// nothing was under this click position
	return FILLRAMP_ILLEGALINDEX;
}


/***********************************************************************************************

>	void FillRamp::GetSelectionState(BOOL* SelState) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		SelState = a pointer to an array of BOOLs
	Purpose:	Record the selection state of all the ramp blobs in this array.

***********************************************************************************************/

void FillRamp::GetSelectionState(BOOL* SelState) const
{
	RampItem *pItem = (RampItem*)GetHead();
	while (pItem)
	{
		*SelState++ = (pItem->GetSelState());
		pItem = (RampItem*)GetNext(pItem);
	}
}


/***********************************************************************************************

>	BOOL FillRamp::IsSelected(UINT32 index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		index = the ramp index
	Purpose:	Find out whether this ramp index is selected or not.

***********************************************************************************************/

BOOL FillRamp::IsSelected(UINT32 index) const
{
	RampItem* pItem = GetValidIndexedItem(index);
	if (pItem)
		return pItem->IsSelected();

	return FALSE;
}
	

/***********************************************************************************************

>	RampItem* FillRamp::GetIndexedItem(INT32 index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		index = the ramp index
	Returns:	A pointer to the item at this index position in the ramp, or NULL if none
	Purpose:	Find the i'th item in this fill ramp

***********************************************************************************************/

RampItem* FillRamp::GetIndexedItem(INT32 index) const
{
	if (index>=0)
	{
		RampItem *pItem = (RampItem*)GetHead();
		while (pItem)
		{
			if (index==0)
				return pItem;
			index--;
			pItem = (RampItem*)GetNext(pItem);
		}
	}
	return NULL;
}

/***********************************************************************************************

>	RampItem* FillRamp::GetValidIndexedItem(UINT32 index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		index = the ramp index
	Returns:	A pointer to the item at this index position in the ramp, or NULL if none
	Purpose:	Find the i'th item in this fill ramp. The index is the normal biased index
				passed into us from the external world. We convert the index and perform
				some error checking on it before we attempt to find the relevant item.

***********************************************************************************************/

RampItem* FillRamp::GetValidIndexedItem(UINT32 index) const
{
	INT32 i = ((INT32)(index)) - FILLRAMP_INDEXBASE;
	if (i<0)
	{
		ERROR3("Illegal index passed to FillRamp::GetValidIndexedItem()");
		return NULL;
	}

	RampItem *pItem = GetIndexedItem(i);
	if (pItem==NULL)
	{
		ERROR3("Index out of range in FillRamp::GetValidIndexedItem()");
		return NULL;
	}

	return pItem;
}


/********************************************************************************************

>	void FillRamp::SetSelState(UINT32 HitControl, INT32 state)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		HitControl, the FillControl that is to be selected.
				state = 0 = deselect blob
						1 = select blob
						2 = toggle blob selection
	Purpose:	Sets the state of a fill ramp blob

********************************************************************************************/

void FillRamp::SetSelState(UINT32 index, INT32 NewState)
{
	INT32 i = ((INT32)(index)) - FILLRAMP_INDEXBASE;
	ERROR3IF(i<0, "Illegal index passed to FillRamp::ChangeBlobState()");
	if (i>=0)
	{
		RampItem* pItem = GetIndexedItem(i);
		if (pItem)
		{
			BOOL CurrState = pItem->GetSelState();
			switch (NewState)
			{
				case 0:	CurrState=FALSE;
				break;
				case 1: CurrState=TRUE;
				break;
				case 2: CurrState=!CurrState;
			}
			pItem->SetSelState(CurrState);
		}	
	}
}


/********************************************************************************************

>	DocCoord FillRamp::GetGeometryCoord(FillGeometryAttribute *pGeom, UINT32 index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		geom  = the geometry, we currently support linear and circular
				index =	the index of an item in our list.
	Returns:	The absolute doc coord on the geometry pGeom of Blob[index]
	Purpose:	Find the absolute coordinate of Blob[index]. The start coordinate
				will be returned if index is out of range

********************************************************************************************/

DocCoord FillRamp::GetGeometryCoord(FillGeometryAttribute *pGeom, UINT32 index) const
{
	if (pGeom==NULL)
	{
		ERROR3("NULL geometry passed to FillRamp::GetGeometryCoord");
		return DocCoord(0,0);	
	}

	RampItem* pItem = GetValidIndexedItem(index);
	if (pItem==NULL)
		return DocCoord(0,0);

	return pGeom->GetGeometryCoord(pItem->GetPosition());
}


/********************************************************************************************

>	void FillRamp::RenderRampBlobs(FillGeometryAttribute *pGeom, RenderRegion *pRender, BOOL *pSelState=NULL) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		geom      = the geometry, we currently support linear and circular
				pRender   = the render region to plot the blobs into
				pSelState = a selection state array which describes an alternate selection
							state (other than that in this ramp) to use to render the blobs
	Returns:	-
	Purpose:	Render the fill blobs in their positions as they appear on the geometry this
				fill ramp is associated with. We simply ask the geometry where they are and
				render appropriately.

********************************************************************************************/

void FillRamp::RenderRampBlobs(FillGeometryAttribute *pGeom, RenderRegion *pRender, BOOL* pSelState) const
{
	BOOL Selected;
	DocCoord coord;
	RampItem* pItem = (RampItem*)GetHead();
	INT32 i=0;
	while (pItem!=NULL)
	{
		if (pSelState!=NULL)
			Selected=pSelState[i];
		else
			Selected=pItem->GetSelState();
		
		coord = pGeom->GetGeometryCoord(pItem->GetPosition());

		// Draw a blob at the end point
		if (Selected)
		{
			// Draw Selected Blob
			pRender->SetLineColour (COLOUR_SELECTEDBLOB);
			pRender->SetFillColour (COLOUR_SELECTEDBLOB);
			pRender->DrawBlob (coord, BT_MSTAGEFILLSELECTED);
		}
		else
		{
			// Draw Unselected Blob
			pRender->SetLineColour (COLOUR_NONE);
			pRender->SetFillColour (COLOUR_BLUE);
			pRender->DrawBlob (coord, BT_MSTAGEFILLUNSELECTED);
		}
	
		i++;
		pItem = (RampItem*)GetNext(pItem);
	}
}

void FillRamp::RenderSelectedBlob (FillGeometryAttribute *pGeom, RenderRegion *pRender)
{
	DocCoord coord;
	
	RampItem* pItem = (RampItem*) GetHead();

	while (pItem)
	{
		if (pItem->IsSelected ())
		{
			coord = pGeom->GetGeometryCoord(pItem->GetPosition());

			pRender->SetLineColour (COLOUR_SELECTEDBLOB);
			pRender->SetFillColour (COLOUR_SELECTEDBLOB);
			pRender->DrawBlob (coord, BT_MSTAGEFILLSELECTED);
			
			return;
		}
		pItem = (RampItem*)GetNext(pItem);
	}
}




/********************************************************************************************

>	UINT32 FillRamp::SetPosition(UINT32 index, float pos)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		index = the index of the item whose position we're setting
				pos   = the new parametric position
	Returns:	a new index.
	Purpose:	Sets the position of an existing ramp item identified by index. As we keep
				our list of items ordered on position, the fact that it is being changed
				will likely result in a shuffling of the item in the list. Hence we return
				a new index (which obviously may or may not be the same as index). Those
				who are keeping up amoung you might even be able to spot when an item
				shuffles by looking at the input and ouput index values.

********************************************************************************************/

UINT32 FillRamp::SetPosition(UINT32 index, float pos)
{
	RampItem *pItem = GetValidIndexedItem(index);
	if (pItem==NULL)
		return FILLRAMP_ILLEGALINDEX;

	pItem->SetPosition(pos);
	//SortRamp (TRUE);
	index = ShuffleItem(pItem,index);
	return index;
}


/********************************************************************************************

>	UINT32 FillRamp::ShuffleItem(RampItem *pItem, UINT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		pItem = a pointer to a ramp item
				index = the ramp items list index
	Returns:	a new index. (The same if no shuffle occured, otherwise err, not the same)
	Purpose:	ShuffleItem checks the validity of pItem. If the item's position in the list
				is invalid, it will be shuffled into a valid position. This needs to be done
				when the position parameter is changed, as we keep an ascending order of
				item positions.

********************************************************************************************/

UINT32 FillRamp::ShuffleItem(RampItem *pItem, UINT32 index)
{
	float curpos = pItem->GetPosition();
	RampItem *pPrevContext = (RampItem*)GetPrev(pItem);
	RampItem *pNextContext = (RampItem*)GetNext(pItem);
	RampItem *pContext = NULL;

	if (pPrevContext == pNextContext)		// I don't know why this occurs, BUT it does sometimes!
	{
		pContext = pPrevContext;
	}
	else
	{
		if ((pPrevContext != NULL) && (pNextContext == NULL))
		{
			pContext = pPrevContext;
		}
		else if ((pNextContext != NULL) && (pPrevContext == NULL))
		{
			pContext = NULL;
		}
		else
		{
			float prevCurpos = pPrevContext->GetPosition ();
			float nextCurpos = pNextContext->GetPosition ();

			float diffPrev = curpos - prevCurpos;
			if (diffPrev < 0) diffPrev = -diffPrev;
			float diffNext = curpos - nextCurpos;
			if (diffNext < 0) diffNext = -diffNext;

			if (diffPrev < diffNext)
			{
				pContext = pPrevContext;
			}
			else
			{
				pContext = NULL;
			}
		}
	}

	if (pContext)
	{
		if (!(pContext->IsSelected ()))
		{
			float contextPos = pContext->GetPosition();
			BOOL shuffle = (curpos < contextPos);

			// do we need to shuffle this item to keep order?
			if (shuffle)
			{
				RampItem *pRemoved = (RampItem*)RemoveItem(pItem);
				pRemoved->ClearPointers ();
				return InsertNewItem(pRemoved);

			}
		}
	}
	else
	{
		pContext = (RampItem*)GetNext(pItem);
		
		if (pContext)
		{
			if (!(pContext->IsSelected ()))
			{
				float contextPos = pContext->GetPosition();
				BOOL shuffle = (curpos > contextPos);

				// do we need to shuffle this item to keep order?
				if (shuffle)
				{
					RampItem *pRemoved = (RampItem*)RemoveItem(pItem);
					pRemoved->ClearPointers ();
					return InsertNewItem(pRemoved);
				}
			}
		}
	}

	return index;
}



/***********************************************************************************************

>	class ColourRamp

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Purpose:	This class holds a list of colour records, each of which describe a colour
				position in a colour ramp. The list contains the usual add/delete entry members
				and other suitable interfaces to allow colour ramps to be edited.
				The list is used inside GradFillAttribute to describe multicoloured fills.

***********************************************************************************************/

ColourRamp::~ColourRamp()
{
	DeleteAll();
}


/***********************************************************************************************

>	void ColourRamp::DeleteAll()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		-
	Purpose:	Deletes all members of the colour ramp list. This is an internal private
				function which is used by other operations in this class

***********************************************************************************************/

void ColourRamp::DeleteAll()
{
	ColRampItem* pItem;
	while ((pItem=((ColRampItem*)RemoveTail()))!=NULL)
		delete pItem;
}


/***********************************************************************************************

>	ColRampItem* ColourRamp::AddEntry(float pos, DocColour *pCol)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		pos  = the position this doc colour occupies in the colour ramp 0..1
				pCol = the doc colour to add
	Outputs:	-
	Returns:	ColRampItem* , a pointer to the list entry created or NULL if out of memory
	Purpose:	Add a new colour ramp entry. Its selected state is FALSE

***********************************************************************************************/

ColRampItem* ColourRamp::AddEntry(float pos, DocColour *pCol)
{
	ERROR2IF(((pos<0.0)||(pos>1.0)), NULL, "position out of range!");
	ERROR2IF(pCol==NULL, NULL, "no colour passed to ColourRamp::AddEntry()");

	// create ourselves a new colour ramp item
	ColRampItem *pNewItem = new ColRampItem(pos,pCol);
	if (pNewItem)
		// insert it in the list
		InsertNewItem(pNewItem);

	// return the item pointer
	return pNewItem;
}

/***********************************************************************************************

>	void ColourRamp::Dump()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		
	Outputs:	-
	Returns:	
	Purpose:	Dumps the data held in the fill ramp to the debug output

***********************************************************************************************/
#ifdef _DEBUG
void ColourRamp::Dump()
{
	ColRampItem *pItem = (ColRampItem *)GetHead();

	TRACE( _T("Colour ramp Dump\n"));

	while (pItem)
	{
		DocColour dc = pItem->GetColour();

		INT32 R = 0;
		INT32 G = 0;
		INT32 B = 0;

		dc.GetRGBValue(&R, &G, &B);
				
		TRACE( _T("Pos %f : Colour %d %d %d\n"), pItem->GetPosition(),
			R, G, B);

		pItem = (ColRampItem *)GetNext(pItem);
	}
}
#endif

/***********************************************************************************************

>	INT32 ColourRamp::operator==(const ColourRamp& other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		other = a colour ramp reference
	Outputs:	-
	Returns:	TRUE if other is equivalent to this
	Purpose:	Try to match this colour ramp with another colour ramp. We simply check the
				numbers in the list, and if the same call the colour ramp entry eqivalence
				operator agains each list entry.

***********************************************************************************************/

INT32 ColourRamp::operator==(const ColourRamp& other) const
{
	if (GetCount() != other.GetCount())
		return FALSE;

	ColRampItem *pThisItem = GetFirstCol();
	ColRampItem *pThatItem = other.GetFirstCol();

	BOOL ok=TRUE;
	while (pThisItem && ok)
	{
		ok = ((*pThisItem)==(*pThatItem));
		pThisItem = GetNextCol(pThisItem);
		pThatItem = other.GetNextCol(pThatItem);
	}

	return ok;
}


/***********************************************************************************************

>	BOOL ColourRamp::IsDifferentTo(ColourRamp *pRamp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		pRamp = the ramp to check against
	Outputs:	-
	Returns:	TRUE if other is different to this
	Purpose:	Another matching function. See if this colour ramp is different to that pointed
				to by pRamp
	See Also:	ColourRamp::operator==

***********************************************************************************************/

BOOL ColourRamp::IsDifferentTo(ColourRamp *pRamp)
{
	ERROR3IF(this==NULL, "NULL 'this' object in ColourRamp::IsDifferentTo()");
	if (pRamp==NULL)
		return TRUE;
	return (!((*this)==(*pRamp)));
}


/***********************************************************************************************

>	ColourRamp& ColourRamp::operator=(const ColourRamp& other)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		other = a colour ramp reference
	Outputs:	-
	Returns:	A reference to this colour ramp
	Purpose:	Assign this colour ramp the same values as those refered to in other.

***********************************************************************************************/

ColourRamp& ColourRamp::operator=(const ColourRamp& other)
{
	// the main problem we need to deal with here is 'what happens
	// when we fail to copy the entire list? Well, it makes no sense
	// to keep the old list as we've not then assigned anything
	// i.e. C=A=B not assigning A and C anything is a bit weird.
	// so we go ahead and toast whatever was in list A and copy B to
	// it (as much as we can at least). If afterwards A!=B then we
	// know we're out of memory

	// toast all colours in our list
	DeleteAll();
	BOOL ok = TRUE;

	ColRampItem *pThisItem, *pOtherItem = other.GetFirstCol();
	while (pOtherItem && ok)
	{
		pThisItem = new ColRampItem(*pOtherItem);
		ok = (pThisItem != NULL);
		if (ok)
		{
			// Add the new colour to our list
			AddTail(pThisItem);
			pOtherItem = other.GetNextCol(pOtherItem);
		}
	}

	// run a debug check.
	ERROR3IF(!ok, "ColourRamp::operator= failed to copy the complete ColourRamp");

	return (*this);
}


/***********************************************************************************************

>	BOOL ColourRamp::SetItemColour(UINT32 index, DocColour *pCol)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/97
	Inputs:		index = the index of the ramp entry
				pCol = the doc colour to set
	Returns:	TRUE if the colour has been set
	Purpose:	Set the colour of an existing colour ramp entry to a new value.

***********************************************************************************************/

BOOL ColourRamp::SetItemColour(UINT32 index, DocColour *pCol)
{
	RampItem *pItem = GetValidIndexedItem(index);
	if (pItem!=NULL)
		((ColRampItem*)pItem)->SetColour(pCol);
	return (pItem!=NULL);
}

/***********************************************************************************************

>	INT32 ColourRamp::SetSelectedColours(DocColour *pCol)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/97
	Inputs:		col = the doc colour to set
	Returns:	the number of colours set.
	Purpose:	Set all selected items in the colour ramp to the colour specified

***********************************************************************************************/

INT32 ColourRamp::SetSelectedColours(DocColour *pCol)
{
	INT32 c=0;
	ColRampItem *pItem = GetFirstCol();
	while (pItem!=NULL)
	{
		if (pItem->IsSelected())
		{
			pItem->SetColour(pCol);
			c++;
		}
		pItem = GetNextCol(pItem);
	}
	return c;
}



/********************************************************************************************

>	BOOL ColourRamp::CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/2000
	Inputs:		SrcDoc:   The document from where this node was copied
				NodesDoc: The document where this node lives 
	Outputs:	-
	Returns:	FALSE if unable to copy data
	Purpose:	If the attribute contains any DocColours which are indexed then a copy
				of the indexed colour is made and added to the NodesDoc
				ColourListComponent.
	Errors:		-
	SeeAlso:	AttrFillGeometry::CopyComponentData

	NOTE:		This is the companion function to the one in AttrFillGeometry.

********************************************************************************************/

BOOL ColourRamp::CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc)
{
	// Get the colour list component
	ColourListComponent *pComponent = 
		(ColourListComponent *) NodesDoc->GetDocComponent(CC_RUNTIME_CLASS(ColourListComponent));

	ENSURE (pComponent != NULL, "Could not find ColourListComponent");
	
	ColRampItem *pItem = GetFirstCol();
	
	// Copy across all DocColours

	while (pItem!=NULL)
	{
		DocColour* pDocCol = pItem->GetColourAddr ();

		if (pDocCol) if (pComponent->CopyColourAcross(pDocCol) == CCCOPY_FAILED) { return (FALSE); }
		
		pItem = GetNextCol(pItem);
	}
	
	return (TRUE);
}


/********************************************************************************************

  >	virtual BOOL ColourRamp::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL ColourRamp::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	BOOL ok = TRUE;

	ColRampItem *pItem = GetFirstCol();

	while (pItem!=NULL)
	{
		DocColour* pDocCol = pItem->GetColourAddr ();
		INT32 ColRef = pFilter->WriteRecord(pDocCol);

		ok = (ColRef != 0);

		if (ok)
		{
			pItem = GetNextCol(pItem);
		}
		else
		{
			pItem = NULL;
		}
	}

	return (ok);
}

/***********************************************************************************************

>	DocColour* ColourRamp::GetFirstSelectedColour()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		col = the doc colour to set
	Returns:	the number of colours set.
	Purpose:	Set all selected items in the colour ramp to the colour specified

***********************************************************************************************/

DocColour* ColourRamp::GetFirstSelectedColour()
{
	ColRampItem *pItem = GetFirstCol();
	while (pItem!=NULL)
	{
		if (pItem->IsSelected())
			return pItem->GetColourAddr();
		pItem = GetNextCol(pItem);
	}
	return NULL;
}

/***********************************************************************************************

>	DocColour* ColourRamp::EnumerateColourFields(UINT32 real_index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		real_index = an unbiased index [0..n-1]
	Returns:	A pointer to the colour record held in this indexed item or NULL
	Purpose:	Enumerates all colour fields in the colour ramp. This linear searching is not
				so quick. We might need to keep a static context pointer here.

***********************************************************************************************/

DocColour* ColourRamp::EnumerateColourFields(UINT32 real_index)
{
	RampItem *pItem = GetIndexedItem(real_index);
	if (pItem)
		return ((ColRampItem*)pItem)->GetColourAddr();
	return NULL;
}


/***********************************************************************************************

>	TransparencyRamp::~TransparencyRamp()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		-
	Purpose:	Deletes all members of the list. This is an internal private
				function which is used by other operations in this class

***********************************************************************************************/

TransparencyRamp::~TransparencyRamp()
{
	DeleteAll();
}


/***********************************************************************************************

>	void TransparencyRamp::DeleteAll()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		-
	Purpose:	Deletes all members of the list. This is an internal private
				function which is used by other operations in this class

***********************************************************************************************/

void TransparencyRamp::DeleteAll()
{
	TranspRampItem* pItem;
	while ((pItem=((TranspRampItem*)RemoveTail()))!=NULL)
		delete pItem;
}


/***********************************************************************************************

>	TranspRampItem* TransparencyRamp::AddEntry(float pos, UINT32 transp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		pos = the position this doc colour occupies in the colour ramp 0..1
				transp = the transparency to add
	Outputs:	-
	Returns:	TranspRampItem* , a pointer to the list entry created or NULL if out of memory
	Purpose:	Add a new transparency ramp entry. Its selected state is FALSE

***********************************************************************************************/

TranspRampItem* TransparencyRamp::AddEntry(float pos, UINT32 transp)
{
	ERROR2IF(((pos<0.0)||(pos>1.0)), FALSE, "position out of range!");

	// create ourselves a new colour ramp item
	TranspRampItem *pNewItem = new TranspRampItem(pos,transp);

	if (pNewItem)
		// insert it into the list
		InsertNewItem(pNewItem);

	// return the item pointer
	return pNewItem;
}



/***********************************************************************************************

>	INT32 TransparencyRamp::operator==(const TransparencyRamp& other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		other = a transparency ramp reference
	Outputs:	-
	Returns:	TRUE if other is equivalent to this
	Purpose:	Try to match this ramp with another ramp. We simply check the
				numbers in the list, and if the same call the transparency ramp entry eqivalence
				operator against each list entry.

***********************************************************************************************/

INT32 TransparencyRamp::operator==(const TransparencyRamp& other) const
{
	if (GetCount() != other.GetCount())
		return FALSE;

	TranspRampItem *pThisItem = GetFirstTransp();
	TranspRampItem *pThatItem = other.GetFirstTransp();

	BOOL ok=TRUE;
	while (pThisItem && ok)
	{
		ok = ((*pThisItem)==(*pThatItem));
		pThisItem = GetNextTransp(pThisItem);
		pThatItem = other.GetNextTransp(pThatItem);
	}

	return TRUE;
}

/***********************************************************************************************

>	BOOL TransparencyRamp::IsDifferentTo(TransparencyRamp *pRamp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		pRamp = the ramp to check against
	Outputs:	-
	Returns:	TRUE if pRamp is different to this
	Purpose:	Another matching function. See if this transparency ramp is different to that pointed
				to by pRamp
	See Also:	TransparencyRamp::operator==

***********************************************************************************************/

BOOL TransparencyRamp::IsDifferentTo(TransparencyRamp *pRamp)
{
	ERROR3IF(this==NULL, "NULL 'this' object in TransparencyRamp::IsDifferentTo()");
	if (pRamp==NULL)
		return TRUE;
	return (!((*this)==(*pRamp)));
}


/***********************************************************************************************

>	TransparencyRamp& TransparencyRamp::operator=(const TransparencyRamp& other)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/97
	Inputs:		other = a colour ramp reference
	Outputs:	-
	Returns:	A reference to this colour ramp
	Purpose:	Assign this transparency ramp the same values as those refered to in other.

***********************************************************************************************/

TransparencyRamp& TransparencyRamp::operator=(const TransparencyRamp& other)
{
	// toast all entries in our list
	DeleteAll();
	BOOL ok = TRUE;

	TranspRampItem *pThisItem, *pOtherItem = other.GetFirstTransp();
	while (pOtherItem && ok)
	{
		pThisItem = new TranspRampItem(*pOtherItem);
		ok = (pThisItem != NULL);
		if (ok)
		{
			// Add the new colour to our list
			AddTail(pThisItem);
			pOtherItem = other.GetNextTransp(pOtherItem);
		}
	}

	// run a debug check.
	ERROR3IF(!ok, "TransparencyRamp::operator= failed to copy the complete TransparencyRamp");

	return (*this);
}



/***********************************************************************************************

>	INT32 TransparencyRamp::SetSelectedTransp(UINT32 transp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/97
	Inputs:		transp = the level of transparency
	Returns:	the number of transparencies set
	Purpose:	Sets all selected items in this ramp to the given transparency.

***********************************************************************************************/

INT32 TransparencyRamp::SetSelectedTransp(UINT32 transp)
{
	INT32 c=0;
	TranspRampItem *pItem = GetFirstTransp();
	while (pItem!=NULL)
	{
		if (pItem->IsSelected())
		{
			pItem->SetTransparency(transp);
			c++;
		}
		pItem = GetNextTransp(pItem);
	}
	return c;
}

/***********************************************************************************************

>	BOOL TransparencyRamp::SetItemTransp(UINT32 index, UINT32 t)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/97
	Inputs:		index = the index of the ramp entry
				t     = the transparency to set
	Returns:	TRUE if the transparency has been set
	Purpose:	Set the transparency of an existing ramp entry to a new value.

***********************************************************************************************/

BOOL TransparencyRamp::SetItemTransp(UINT32 index, UINT32 t)
{
	RampItem *pItem = GetValidIndexedItem(index);
	if (pItem!=NULL)
		((TranspRampItem*)pItem)->SetTransparency(t);
	return (pItem!=NULL);
}

/***********************************************************************************************

>	UINT32* TransparencyRamp::GetFirstSelectedTransp()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		-
	Returns:	A pointer to the first selected transparency
	Purpose:	Get a pointer to the first selected item's transparency field. NULL will
				be returned if there are no selected items

***********************************************************************************************/

UINT32* TransparencyRamp::GetFirstSelectedTransp()
{
	TranspRampItem *pItem = GetFirstTransp();
	while (pItem!=NULL)
	{
		if (pItem->IsSelected())
			return pItem->GetTranspAddr();
		pItem = GetNextTransp(pItem);
	}
	return NULL;
}
