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
/*

	sgline2.h

	This header contains declarations of the various classes derived from
	LineAttrItem, as used in the line-attributes gallery.
*/

#ifndef SGLINE2_H
#define SGLINE2_H

#include "sgline.h"
//#include "attr.h" - in camtypes.h [AUTOMATICALLY REMOVED]


/********************************************************************************************
>	class LineWidthItem : public LineAttrItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Represents a line-width user-interface item within the line gallery.
	SeeAlso:	-
********************************************************************************************/

class LineWidthItem : public LineAttrItem
{
public:
	// Creation & destruction.
	LineWidthItem(double nPointWidth, const String& strDescription);

private:
	// Overridables.
	virtual void SetAttributes(RenderRegion* pRegion) const;
	virtual NodeAttribute* CreateNewAttribute(BOOL fIsAdjust) const;
	virtual MILLIPOINT GetWidth() const;
	virtual CCRuntimeClass** GetAttribRuntimeClasses() const;
	virtual BOOL IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const;

	// Data members.
	MILLIPOINT	m_nWidth;
};



/********************************************************************************************
>	class LineDashItem : public LineAttrItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Represents a line dash-pattern user-interface item within the line gallery.
	SeeAlso:	-
********************************************************************************************/

class LineDashItem : public LineAttrItem
{
public:
	// Creation & destruction.
	LineDashItem(StockDash nDashType, const String& strDescription);

private:
	// Overridables.
	virtual void SetAttributes(RenderRegion* pRegion) const;
	virtual NodeAttribute* CreateNewAttribute(BOOL fIsAdjust) const;
	virtual MILLIPOINT GetHeight() const;
	virtual CCRuntimeClass** GetAttribRuntimeClasses() const;
	virtual BOOL IsEqualValueToAny(NodeAttribute** pOtherAttribs) const;

	// Data members.
	StockDash	m_nDashType;
};



/********************************************************************************************
>	class LineArrowItem : public LineAttrItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Class to represent a arrowhead/tail user-interface item within the
				line gallery.
	SeeAlso:	-
********************************************************************************************/

class LineArrowItem : public LineAttrItem
{
public:
	// Creation & destruction.
	LineArrowItem(StockArrow nArrowType, double fpScale, const String& strDescription);

protected:
	// Implementation.
	virtual void SetAttributes(RenderRegion* pRegion) const;
	virtual void DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const;
	virtual NodeAttribute* CreateNewAttribute(BOOL fIsAdjust) const;
	virtual MILLIPOINT GetWidth() const;
	virtual MILLIPOINT GetHeight() const;
	virtual MILLIPOINT GetHorzGap() const;
	virtual MILLIPOINT GetVertGap() const;
	virtual CCRuntimeClass** GetAttribRuntimeClasses() const;
	virtual BOOL IsEqualValueToAny(NodeAttribute** pOtherAttribs) const;

	// Override this to return TRUE if your item is a start arrow, FALSE if it is an
	// end-arrow.
	virtual BOOL IsStartArrow() const;

	// Data members.
	ArrowRec  m_arArrow;
	BOOL	  m_fCentred;

protected:
	// The base class returns TRUE, so that an item of this type will auto-close the gallery
	// when CTRL-double-clicked. However, this derived class (e.g. arrowheads) overrides
	// this to stop the gallery from auto-closing when a ctrl-double-click occurs.
	virtual BOOL ShouldCloseOnDoubleClick(void);
};



/********************************************************************************************
>	class LineNoArrowItem : public LineArrowItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Purpose:	Class to represent a no-arrowhead/tail user-interface item within the
				line gallery.  Only used as a base class for LineNoStartArrowItem and
				LineNoEndArrowItem
	SeeAlso:	-
********************************************************************************************/

class LineNoArrowItem : public LineArrowItem
{
protected:
	LineNoArrowItem(const String& strDescription);

private:
	virtual MILLIPOINT GetWidth() const;
	virtual MILLIPOINT GetHeight() const;
	virtual MILLIPOINT GetVertGap() const;
	virtual void SetAttributes(RenderRegion* pRegion) const;
	virtual void DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const;
};



/********************************************************************************************
>	class LineNoStartArrowItem : public LineNoArrowItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Purpose:	Class to represent a no-start-arrowhead/tail user-interface item within the
				line gallery.
	SeeAlso:	-
********************************************************************************************/

class LineNoStartArrowItem : public LineNoArrowItem
{
public:
	LineNoStartArrowItem(const String& strDescription);

private:
	virtual BOOL IsEqualValueToAny(NodeAttribute** pOtherAttribs) const;
	virtual BOOL IsStartArrow() const;
};



/********************************************************************************************
>	class LineNoEndArrowItem : public LineNoArrowItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Purpose:	Class to represent a no-start-arrowhead/tail user-interface item within the
				line gallery.
	SeeAlso:	-
********************************************************************************************/

class LineNoEndArrowItem : public LineNoArrowItem
{
public:
	LineNoEndArrowItem(const String& strDescription);

private:
	virtual BOOL IsEqualValueToAny(NodeAttribute** pOtherAttribs) const;
	virtual BOOL IsStartArrow() const;
};



/********************************************************************************************
>	class LineCapItem : public LineAttrItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Represents a line-cap user-interface item within the line gallery.
	SeeAlso:	-
********************************************************************************************/

class LineCapItem : public LineAttrItem
{
public:
	// Creation & destruction.
	LineCapItem(LineCapType nCapType, const String& strDescription);

private:
	// Implementation.
	virtual void SetAttributes(RenderRegion* pRegion) const;
	virtual NodeAttribute* CreateNewAttribute(BOOL fIsAdjust) const;
	virtual CCRuntimeClass** GetAttribRuntimeClasses() const;
	virtual BOOL IsEqualValueToAny(NodeAttribute** pOtherAttribs) const;

	// Data members.
	LineCapType	m_nCapType;
};



/********************************************************************************************
>	class LineJoinTypeItem : public LineAttrItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Represents a line-join user-interface item within the line gallery.
	SeeAlso:	-
********************************************************************************************/

class LineJoinTypeItem : public LineAttrItem
{
public:
	// Creation & destruction.
	LineJoinTypeItem(JointType nJoinType, const String& strDescription);

private:
	// Overridables.
	virtual void SetAttributes(RenderRegion* pRegion) const;
	virtual void DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const;
	virtual NodeAttribute* CreateNewAttribute(BOOL fIsAdjust) const;
	virtual CCRuntimeClass** GetAttribRuntimeClasses() const;
	virtual BOOL IsEqualValueToAny(NodeAttribute** pOtherAttribs) const;

	// Data members.
	JointType	m_nJoinType;
};

#endif	// !SGLINE2_H
