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
// Header for the mould tool shapes implementation

#ifndef INC_MOULDSHAPES
#define INC_MOULDSHAPES

#include "paths.h"
#include "binds.h"
#include "tranform.h"
#include "nodepath.h"
#include "nodemold.h"

class KernelBitmap;
// class NodeMould;

#define MOULD_V1THRESHOLD		1024
#define MOULD_V2THRESHOLD		32
#define MOULD_CURRTHRESHOLD		32


struct DPOINT {
	double x, y ;
} ;

/********************************************************************************************

>	class MouldGeometry

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/94
	Purpose:	An abstract base class for mould shapes.

********************************************************************************************/

class MouldGeometry	: public CCObject
{
	CC_DECLARE_DYNAMIC(MouldGeometry);

	public:
		MouldGeometry();
		
		// Shape initialisation functions
		virtual BOOL Validate(Path* const pPath, UINT32& errorID) {return FALSE;}
		virtual BOOL Define(Path* const pPath, DocRect* const pRect) {return FALSE;}
		virtual MouldSpace Describe() {return MOULDSPACE_UNDEFINED;}
		virtual BOOL MakeValidFrom(Path **Out, Path *In, INT32 *CornersHint = 0) {return FALSE;}

		// functions to create a moulded object
	    virtual	BOOL MouldPathToPath(Path* pSourcePath, Path* PDestinPath) {return FALSE;};
	    virtual BOOL MouldBitmapToTile(KernelBitmap* pSourceBlit, KernelBitmap* pDestinBlit) {return FALSE;};
		virtual BOOL MouldPoint(DocCoord p,DocCoord& q) { q=p; return FALSE; }

        // functions to render an object through the mould
	    virtual	void MouldPathRender(Path* pPath, RenderRegion* pRegion);
	    virtual void MouldBitmapRender(KernelBitmap* pBlit, DocCoord* pParallel, RenderRegion* pRegion);

		// functions to render mould blobs
        virtual void RenderControlBlobs(RenderRegion* pRegion) {};
		virtual void RenderDragBlobs(Spread* pSpread) {};
		virtual void DisableControlBlobs() {};
		virtual void EnableControlBlobs() {};
		virtual void ToggleControlBlobs(Spread* pSpread) {};

		// functions concerned with bounding boxes
		virtual DocRect GetBlobBoundingRect() { return DocRect(0,0,0,0); };
		virtual void GetDebugDetails(StringBase*) {};
		virtual DocRect GetSourceRect() { return DocRect(0,0,0,0);};

		// function to actually do things with a mould
        virtual BOOL OnClick(DocCoord, ClickType, ClickModifiers, Spread*, NodeMould*);
		virtual BOOL OnMouseMove(DocCoord, Spread*, ClickModifiers, INT32*,INT32*);
		virtual void Transform(Path* const pNewPath, DocRect* const pRect, TransformBase& Trans) {};
		virtual MouldGeometry* MakeCopy();
		virtual ChangeCode RecordContext(UndoableOperation*) {return CC_OK;}
		virtual ChangeCode RecordBlobs(UndoableOperation*, Spread*) {return CC_OK;}
		virtual UINT32 GetByteSize() const { return sizeof(MouldGeometry); };

		virtual void SetThreshold(const INT32 t);
				INT32 GetThreshold() const { return MouldThreshold; }

	protected:
		BOOL 	CopyContents(MouldGeometry* pGeom);
		DocRect ConvRectToDocRect(RECT& rect0);
		RECT	ConvDocRectToRect(DocRect& rect0);

	protected:
		INT32	MouldThreshold;
};



/********************************************************************************************

>	class MouldTransform : public TransformBase

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/95
	Purpose:	Defines the mould transform class. This class is used by a moulder when
				it needs to ask other objects to transform themselves geometrically.
				It provides various functions for nodes to call to transform coordinates
				or paths.

********************************************************************************************/

class MouldTransform : public TransformBase
{
	CC_DECLARE_DYNAMIC(MouldTransform)

	public:
		 MouldTransform(MouldGeometry* pGeom);
		~MouldTransform();

		virtual void Transform( DocCoord* Coords, INT32 NumCoords);
		virtual BOOL IsInvertable() { return FALSE; }
		virtual BOOL TransformPath( Path* const pSource, Path* pDestin);

	private:
		MouldGeometry* pMouldGeom;

};

#endif


