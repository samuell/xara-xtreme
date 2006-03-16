// $Id: sgbrush.h 662 2006-03-14 21:31:49Z alex $
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

// Line Gallery interface for Brush Attributes - header

#ifndef SGBRUSH
#define SGBRUSH

#include "sgline.h"
#include "brshattr.h"
#include "brshcomp.h"

/********************************************************************************************

>	class BrushAttrItem : public LineAttrItem

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/2000
	Purpose:	Represents a brush attribute user-interface item within the line gallery.

********************************************************************************************/

class BrushAttrItem : public LineAttrItem
{
CC_DECLARE_DYNAMIC(BrushAttrItem);

public:
	// Creation & destruction.
	BrushAttrItem(BrushAttrValue *pBrush, const String &strDescription);
	~BrushAttrItem();

public:
	// Access functions
	virtual BrushAttrValue *GetBrushAttrValue() { return m_pBrushAttrVal;}
	virtual BrushDefinition *GetBrushDefinition();
	virtual BrushHandle GetBrushHandle();
	
public: //various overridables
	virtual void RenderItem(RenderRegion *pRender, DocRect *pDocRect, INT32 Width = c_nDefaultLineWidth);
	virtual void UpdateGalleryItem();

	// performs various actions necessary prior to selection
	virtual BOOL ItemSelected(NodeAttribute* pNewAttr);


public:
	BOOL IsEqualValueToAny(NodeAttribute** ppOtherAttribs) const;	
	BOOL UpdateProcessor();
protected:
	// Overridables.
	virtual void SetAttributes(RenderRegion* pRegion) const;
	void DrawItem(RenderRegion* pRegion, const DocRect& drBounds) const;
	virtual NodeAttribute* CreateNewAttribute(BOOL fIsAdjust) const;
	virtual CCRuntimeClass** GetAttribRuntimeClasses() const;

	// finds the current default brush attribute
	BrushHandle GetCurrentDefaultBrushHandle();

private:
	BrushAttrValue *m_pBrushAttrVal;
};


#endif
