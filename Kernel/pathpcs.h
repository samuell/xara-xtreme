// $Id: pathpcs.h 662 2006-03-14 21:31:49Z alex $
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
// pathpcs - Definition of rendering PathProcessor class

#ifndef INC_PATHPCS
#define INC_PATHPCS


class Path;
class RenderRegion;
class AttributeValue;


/********************************************************************************************

>	class PathProcessor : public CCObject

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96

	Purpose:	This is the abstract base class for all PathProcessors.

				These classes are created by attributes (or similar) when rendered,
				and placed on a stack in the RenderRegion. Whenever RenderRegion::DrawPath
				is invoked to render a path, all stacked PathProcessors will be called in
				turn, and they call back to RenderRegion functions to render whatever they
				wish (usually a modified form of the original path).

				This can be used to "filter" almost any rendering, but is mainly intended
				for use by stroke and fill providers, which replace input paths with more
				suitable shapes to be rendered.

	SeeAlso:	RenderRegion::DrawPath; PathProcessor::ProcessPath

********************************************************************************************/

class PathProcessor : public CCObject
{
friend class RenderRegion;
CC_DECLARE_DYNAMIC(PathProcessor);

public:			// Construction/destruction
	PathProcessor();
	virtual ~PathProcessor();


public:			// RenderRegion path-processing interface
	virtual void ProcessPath(Path *pPath,
							 RenderRegion *pRender,
							 PathShape ShapePath = PATHSHAPE_PATH) = 0;
	virtual BOOL WillChangeFillAndStrokeSeparately(void);


public:			// "Parent Attribute" interface
	AttributeValue *GetParentAttr(void)				{ return(ParentAttr); };
	void SetParentAttr(AttributeValue *pParent)		{ ParentAttr = pParent; };
			// Set/Get the parent attribute pointer. Used by some derived classes to
			// remember the attribute which created this processor.


	virtual BOOL IsAPathProcessorBrush() { return FALSE;} // identifier
	virtual BOOL IsAPathProcessorStroke() { return FALSE;}

		// friend (RenderRegion) access functions - for stacking
	PathProcessor *GetNextProcessor(void)			{ return(NextProcessor); };
	void SetNextProcessor(PathProcessor *pNext)		{ ERROR3IF(pNext == this, "Awooga!"); NextProcessor = pNext; };

	virtual BOOL DoBecomeA(BecomeA* pBecomeA, Path* pPath, Node* pParent) {return FALSE;}

	virtual BOOL NeedsTransparency() const {return FALSE;};
			// Returns TRUE if this path processor makes use of transparency

	virtual BOOL DoesActuallyDoAnything(RenderRegion* pRender); 
			// returns TRUE if the PP actually alters the path to be rendered

	void SetDisabled(BOOL Value);
	BOOL IsDisabled();

public:
	virtual BOOL AllowsPropertiesDialog(void) {return FALSE;}
			// Returns TRUE if this path processor allows property editing via the gallery menu/dialog

private:
	PathProcessor *NextProcessor;			// 1-way linked list link for RenderRegion PP stack
	AttributeValue *ParentAttr;				// NULL, or pointer back to the AttributeValue which created us

	BOOL m_DisableMe; // flag which indicates whether or not we should disable ourselves
};

#endif

