// $Id: snap.h 662 2006-03-14 21:31:49Z alex $
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
// The Snap class - used to do all types of snapping in Camelot

#ifndef INC_SNAP
#define INC_SNAP

class Spread;
class DocView;
class DocCoord;
class DocRect;
class Node;
class Layer;
class Path;

enum GuidelineType	{ GUIDELINE_NOTYPE, GUIDELINE_HORZ, GUIDELINE_VERT };

/********************************************************************************************

>	class CSnap : public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Purpose:	Performs the snapping required by Camelot

********************************************************************************************/

class CSnap : public CC_CLASS_MEMDUMP
{
CC_DECLARE_MEMDUMP(CSnap);

public:
	CSnap(DocView* pThisDocView,Spread* pThisSpread);

	// General snap functions of DocView.
	BOOL Snap(DocCoord* pDocCoord, BOOL TryMagSnap = TRUE,
								   BOOL TryGridSnap = TRUE);
	BOOL Snap(DocRect* pDocRect, const DocCoord& PrevCoord, const DocCoord& CurCoord);

	BOOL TryToSnapToObject(Node* pNode, DocCoord* pDocCoord);

	BOOL SnapToGrids(DocCoord* pDocCoord);

	void SetSpread(Spread* pThisSpread) { pSpread = pThisSpread; }

	static MILLIPOINT GetSnapDist() { return SnapDist; }
	static MILLIPOINT GetSnapToCoordDist() { return SnapToCoordDist; }

	static BOOL SnapCoordToPath(DocCoord* pDocCoord, Path* pPath);

	static BOOL SnapRectToRect(DocRect* pSnappedRect, const DocRect& RefRect);

private:
	// Internal snap-to-grid funcs
	BOOL SnapToGrids(DocRect* pDocRect, const DocCoord& PrevCoord, const DocCoord& CurCoord);

	// Internal snap-to-objects funcs
	// if Magnetic is TRUE, only magnetic objects are snapped to, otherwise all objects are snapped to.
	BOOL SnapToObjects(Node* pRootNode,DocCoord* pDocCoord,BOOL Magnetic,BOOL GuideLayersOnly);
	BOOL SnapToObjects(DocRect* pDocRect, const DocCoord& PrevCoord, const DocCoord& CurCoord, BOOL Magnetic,BOOL GuideLayersOnly);

	// Internal snap-to-guidelines funcs
	BOOL SnapToGuidelines(DocCoord* pDocCoord,GuidelineType Type = GUIDELINE_NOTYPE);
	BOOL SnapToGuidelines(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);
	BOOL SnapToGuidesInLayer(Layer* pLayer,DocCoord* pDocCoord,GuidelineType Type);

	// Internal snap-to-guide-objects funcs
	BOOL SnapToGuideObjects(DocCoord* pDocCoord);
	BOOL SnapToGuideObjects(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);

	// Internal snap-to-pages funcs
	BOOL SnapToPages(DocCoord* pDocCoord);
	BOOL SnapToPages(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);

	// Internal snap-to-value function
	static BOOL SnapToValue(const INT32 RefValue, INT32* pTestValue, INT32* pLockedValue);

private:
	void CalcSnapDist();
	
	DocView*	pDocView;
	Spread*		pSpread;
	Node*		pLastSnappingObject;

	// This is how close a point needs to be to a shape before it can snap to it
	// It is relative to the scale factor of the CSnap object's DocView, and is calculated
	// when the CSnap object is constructed
	static MILLIPOINT SnapDist;
	static MILLIPOINT SnapToCoordDist;

	// This flag controls the snapping of points to the pages in the relevant spread
	// Page boundary snapping will only occur when 'snap to objects' is turned on.
	static BOOL TrySnapToPages;
};

#endif  // INC_SNAP
