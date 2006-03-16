// $Id: fillramp.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_FILLRAMP
#define INC_FILLRAMP

// This file holds classes concerned with fill ramps. 
// Currently colour ramps and transparancy ramps have been implemented fully.

#include "list.h"
#include "doccolor.h"
#include "doccoord.h"
#include "fillattr.h"

#define FILLRAMP_INDEXBASE 1024
#define FILLRAMP_ILLEGALINDEX (FILLRAMP_INDEXBASE-1)
#define ABSOLUTEINDEX(x) ((x)+FILLRAMP_INDEXBASE)
#define RELATIVEINDEX(x) ((x)-FILLRAMP_INDEXBASE)
#define ISA_RAMPINDEX(x) ((x)>=FILLRAMP_INDEXBASE)

class FillGeometryAttribute;
class RenderRegion;
class BaseCamelotFilter;
class CamelotFileRecord;

/********************************************************************************************

>	class RampItem : public ListItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/02/97
	Purpose:	Defines whats common to all ramp items, i.e. their position and selection
				state

********************************************************************************************/

class RampItem : public ListItem
{
	CC_DECLARE_DYNCREATE(RampItem);

	public:
		RampItem();
		RampItem(float pos);
		RampItem(const RampItem &other);

		void	SetPosition(float pos);
		void	SetSelState(BOOL selstate);
		
		float	GetPosition() const { return Position; }
		BOOL	GetSelState() const { return Selected; }
		BOOL	IsSelected()  const { return Selected; }

		virtual BOOL IsAColourRampItem() const { return (FALSE); }
		virtual BOOL IsATranspRampItem() const { return (FALSE); }

	private:
		void	ClampPosition();

	protected:
		float	Position;			// 0..1 specifies the position of this colour
		BOOL	Selected;			// whether this item is selected in the UI
};


/********************************************************************************************

>	class ColRampItem : public RampItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/02/97
	Purpose:	Defines a single entry in a colour ramp. 
				An entry is simply made up of a single doc colour definition.

********************************************************************************************/

class ColRampItem : public RampItem
{
	CC_DECLARE_DYNCREATE(ColRampItem);

	public:
		ColRampItem();
		ColRampItem(float pos, DocColour *pCol);
		ColRampItem(const ColRampItem &other);
		
		void		SetColour(DocColour *pCol);
		DocColour	GetColour() const	{ return Colour; }
		DocColour*	GetColourAddr()		{ return &Colour; }
		
		INT32			operator==(const ColRampItem& other) const;

		virtual BOOL IsAColourRampItem() const { return (TRUE); }

	private:
		DocColour	Colour;				// the actual colour.
};


/********************************************************************************************

>	class TranspRampItem : public RampItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/02/97
	Purpose:	Defines a single entry in a transparency ramp. 
				An entry is simply made up of a single long definition.

********************************************************************************************/

class TranspRampItem : public RampItem
{
	CC_DECLARE_DYNCREATE(TranspRampItem);

	public:
		TranspRampItem();
		TranspRampItem(float pos, UINT32 transp);
		TranspRampItem(const TranspRampItem &other);
		
		void	SetTransparency(UINT32 transp);
		UINT32	GetTransparency() const	{ return Transparency; }
		UINT32*	GetTranspAddr()			{ return &Transparency; }
		
		INT32		operator==(const TranspRampItem& other) const;

		virtual BOOL IsATranspRampItem() const { return (TRUE); }

	private:
		UINT32	Transparency;		// the actual transparency.
};



/********************************************************************************************

>	class FillRamp : public List

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/02/97
	Purpose:	The base class fill ramp. This class contains common functionality between
				colour and transparency ramps. Each ramp entry derived from RampItem will 
				contain a selection state and a (float) parameter value. The parameter value
				usually ranges between 0 and 1 and hence can be used to generate a coordinate
				from any given fill geometry.

********************************************************************************************/

class FillRamp : public List
{
	CC_DECLARE_DYNAMIC( FillRamp )

	public:
		void	  GetSelectionState(BOOL *SelState) const;
				  // find the selection state for all blobs
				
		UINT32	  CountSelBlobs() const;
				  // just how many selected blobs are there?

		INT32	  GetSelectedIndex() const;
				  // find the index of the first selected blob

		BOOL	  GetIndexRange(UINT32 *pFirst, UINT32 *pLast) const;
				  // find the index of the start and end blobs (hence the range)

		BOOL	  IsSelected(UINT32 index) const;
				  // is a particular blob selected?

		void	  DeselectAll() const;
				  // throw away any selected bits.

		FillControl		RotateSelRight (BOOL StartPointSelected, BOOL EndPointSelected, FillControl& OldSelectedBlob) const;
						// rotate all selection states down the list (with carry)

		BOOL	  RotateSelLeft(BOOL StartPointSelected, BOOL EndPointSelected, FillControl& OldSelectedBlob) const;
				  // rotate all selection states up the list (with carry)

		void	  SetSelState(UINT32 index, INT32 state);
				  // set, unset or toggle the selection state of a blob

		UINT32	  SetPosition(UINT32 index, float pos);
				  // give the item at 'index' a new position.

		UINT32	  HitBlob(FillGeometryAttribute *pGeom, DocCoord& ClickPos) const;
				  // is the given click position inside a blob?

		DocCoord  GetGeometryCoord(FillGeometryAttribute *pGeom, UINT32 index) const;
				  // get the actual position of a blob on a given geometry

		void	  RenderRampBlobs(FillGeometryAttribute *pGeom, RenderRegion *pRender, BOOL *pSelState=NULL) const;
				  // render all the blob positions to a render region

		void	  RenderSelectedBlob (FillGeometryAttribute *pGeom, RenderRegion *pRender);
				  // render just the selected blob (so that we don't get blob redraw ficker)

		void	  SortRamp();
				  // ensures that all positions in the ramp are in numerical order

		RampItem* GetValidIndexedItem(UINT32 index) const;
				  // find the i'th item in our blob list, do some error checking on index

		void	  Blend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillGeometryAttribute* pOtherFill, BOOL notInvert = FALSE);
				  // blend those fill ramps!

	protected:

		RampItem* GetIndexedItem(INT32 index) const;
				  // find the i'th item in our blob list
		
		UINT32	  InsertNewItem(RampItem* pItem);
				  // place a new item somewhere in the list.

	private:
		RampItem* FindInsertPosition(float pos, INT32* pInsert, UINT32* index) const;
				  // return a list insertion point for this new item (helper function)

		UINT32	  ShuffleItem(RampItem *pItem, UINT32 index);
				  // shuffle pItem around in the list after a position change

		// private support functions for FillRamp::Blend () ....
		
		void	  DoOneToOneBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp, BOOL notInvert);
		void	  DoNoneToManyBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp, BOOL notInvert);
		void	  DoManyToNoneBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillGeometryAttribute* pOtherFill, BOOL notInvert);
		void	  DoManyToManyBlend (FillGeometryAttribute* pNewAttr, BlendAttrParam* pBlendParam, FillRamp* OtherRamp, BOOL notInvert);
};


/***********************************************************************************************

>	class ColourRamp : public FillRamp

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/02/97
	Purpose:	This class holds a list of colour records, each of which describe a colour
				position in a colour ramp. The list contains the usual add/delete entry members
				and other suitable interfaces to allow colour ramps to be edited.
				The list is used inside GradFillAttribute to describe multicoloured fills.

***********************************************************************************************/

class ColourRamp : public FillRamp
{
	CC_DECLARE_DYNAMIC( ColourRamp )

	public:
		virtual ~ColourRamp();

			   ColRampItem* AddEntry(float pos, DocColour* pCol);
		inline ColRampItem* GetFirstCol() const;
		inline ColRampItem* GetLastCol() const;
		inline ColRampItem* GetNextCol(ColRampItem* pItem) const;
		inline ColRampItem* GetPrevCol(ColRampItem* pItem) const;
			   ColourRamp&  operator=(const ColourRamp& other);
			   DocColour*   GetFirstSelectedColour();
			   DocColour*	EnumerateColourFields(UINT32 index);

			   BOOL IsDifferentTo(ColourRamp *pRamp);

		INT32    operator==(const ColourRamp& other) const;
		void   DeleteAll();

		BOOL   SetItemColour(UINT32 index, DocColour* pCol);
		INT32   SetSelectedColours(DocColour *pCol);

		BOOL CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc);

		BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

#ifdef _DEBUG
		// dump all fill ramp entries
		void Dump();
#else
		void Dump() { } ;
#endif

};


// inline function to return the 'correctly typed ' first entry.
ColRampItem* ColourRamp::GetFirstCol() const
{
	return (ColRampItem*)GetHead();
}

// inline function to return the 'correctly typed ' last entry.
ColRampItem* ColourRamp::GetLastCol() const
{
	return (ColRampItem*)GetTail();
}

// inline function to return the 'correctly typed ' next entry.
ColRampItem* ColourRamp::GetNextCol(ColRampItem* pItem) const
{
	return (ColRampItem*)GetNext(pItem);
}

// inline function to return the 'correctly typed ' previous entry.
ColRampItem* ColourRamp::GetPrevCol(ColRampItem* pItem) const
{
	return (ColRampItem*)GetPrev(pItem);
}





/***********************************************************************************************

>	class TransparencyRamp : public FillRamp

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/02/97
	Purpose:	This class holds a list of transparency records, each of which describe a
				specific entry in the transparency ramp

***********************************************************************************************/

class TransparencyRamp : public FillRamp
{
	CC_DECLARE_DYNAMIC( TransparencyRamp )

	public:
		virtual ~TransparencyRamp();

			   TranspRampItem* AddEntry(float pos, UINT32 transp);
		inline TranspRampItem* GetFirstTransp() const;
		inline TranspRampItem* GetNextTransp(TranspRampItem* pItem) const;
			   TransparencyRamp& operator=(const TransparencyRamp& other);

			   BOOL IsDifferentTo(TransparencyRamp *pRamp);
			   UINT32* GetFirstSelectedTransp();

		INT32    operator==(const TransparencyRamp& other) const;
		void   DeleteAll();
		
		BOOL   SetItemTransp(UINT32 index, UINT32 transp);
		INT32   SetSelectedTransp(UINT32 transp);
};


// inline function to return the 'correctly typed ' first entry
TranspRampItem* TransparencyRamp::GetFirstTransp() const
{
	return (TranspRampItem*)GetHead();
}

// inline function to return the 'correctly typed ' next entry
TranspRampItem* TransparencyRamp::GetNextTransp(TranspRampItem* pItem) const
{
	return (TranspRampItem*)GetNext(pItem);
}



#endif // INC_FILLRAMP

