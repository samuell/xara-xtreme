// $Id: moldenv.h 662 2006-03-14 21:31:49Z alex $
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
// Header for the envelope mould shapes implementation

#ifndef INC_ENVELOPE
#define INC_ENVELOPE

#include "moldshap.h"
#include "ops.h"
#include "nodemold.h"
#include "gmould.h"

#define ENV_NUMCOORDS		12
#define ENV_NUMCOORDS2X2	24
#define ENV_MAXCOORDS 		24

/********************************************************************************************

>	class MouldEnvelopeBase	: public MouldGeometry

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Purpose:	The envelope mould node. Provides control over a coons patch shape made
				from 4 connected 3rd order bezier splines.

********************************************************************************************/

class MouldEnvelopeBase : public MouldGeometry
{
	friend class RecordEnvelopeAction;

	CC_DECLARE_DYNAMIC(MouldEnvelopeBase);

	public:
		 MouldEnvelopeBase();
		~MouldEnvelopeBase();

		virtual void RenderControlBlobs(RenderRegion* pRegion);
		virtual void RenderDragBlobs(Spread* pSpread);
		virtual void DisableControlBlobs();
		virtual void EnableControlBlobs();
		virtual void ToggleControlBlobs(Spread* pSpread);

		// functions to create a moulded object
		virtual	BOOL MouldPathToPath(Path* pSourcePath, Path* PDestinPath);
	    virtual BOOL MouldBitmapToTile(KernelBitmap* pSourceBlit, KernelBitmap* pDestinBlit);
		virtual BOOL MouldPoint(DocCoord p,DocCoord& q);

        // functions to render an object through the mould
	    virtual	void MouldPathRender(Path* pPath, Spread* pSpread);
	    virtual void MouldBitmapRender(KernelBitmap* pBlit, DocCoord* pParallel, RenderRegion* pRegion);

		virtual DocRect GetSourceRect();

	protected:
		BOOL	Define(Path* const pPath, DocRect* const pOrigBBox, INT32 ncoords);

		RECT 	BuildRectangle(DocRect* const pOrigBBox);
		BOOL 	BuildShape(POINT* pPoints, DocRect* const pOrigBBox);
		BOOL 	ConvertShape(INT32 ncoords, POINT* dPoints, Path* const pPath, const INT32 numcoords);

		void 	RenderGrid(Spread* pSpread);
		void 	RenderGrid(RenderRegion* pRegion);
		void 	RenderGridPoints(RenderRegion* pRegion);

		BOOL	ValidMouldingPath(INT32 ncoords, INT32 nelements, Path* const pPath, UINT32& errorID);
		static  BOOL WillBeValid(INT32 ncoords, POINT* P);

	protected:
		GMould	*pEnvelope;

	private:
		INT32 	BlobState;
};



/********************************************************************************************

>	class MouldEnvelope	: public MouldEnvelopeBase

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Purpose:	The envelope mould node. Provides control over a coons patch shape made
				from 4 connected 3rd order bezier splines.

********************************************************************************************/

class MouldEnvelope : public MouldEnvelopeBase
{
	CC_DECLARE_DYNAMIC(MouldEnvelope);

	public:
		 MouldEnvelope();
		~MouldEnvelope();

		virtual BOOL		Initialise();
		
		virtual BOOL		Validate(Path* const pPath, UINT32& errorID);
		virtual BOOL		Define(Path* const pPath, DocRect* const pRect);
		virtual BOOL		MakeValidFrom(Path **Out, Path *In, INT32 *CornersHint = 0);
		virtual MouldSpace	Describe() {return MOULDSPACE_ENVELOPE;}

		virtual void		Transform(Path* const pPath, DocRect* const pRect, TransformBase& Trans);
		virtual ChangeCode	RecordContext(UndoableOperation*);
		virtual UINT32		GetByteSize() const { return sizeof(MouldEnvelope); };
		
		virtual MouldGeometry* MakeCopy();

	public:
		// none virtual functions local to this class.
		static BOOL WillBeValid(POINT* P);

	protected:
		BOOL CopyContents(MouldEnvelope* pGeom);

	private:
		const INT32 EnvNumCoords;
};



/********************************************************************************************

>	class MouldEnvelope2x2 : public MouldEnvelopeBase

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/95
	Purpose:	Another envelope mould. Provides control over a coons patch shape made
				from 2x2 4 connected 3rd order bezier splines.

********************************************************************************************/

class MouldEnvelope2x2 : public MouldEnvelopeBase
{
	CC_DECLARE_DYNAMIC(MouldEnvelope2x2);

	public:
		 MouldEnvelope2x2();
		~MouldEnvelope2x2();

		virtual BOOL		Initialise();

		virtual BOOL		Validate(Path* const pPath, UINT32& errorID);
		virtual BOOL		Define(Path* const pPath, DocRect* const pRect);
		virtual BOOL		MakeValidFrom(Path **Out, Path *In, INT32 *CornersHint = 0);
		virtual MouldSpace	Describe() {return MOULDSPACE_ENVELOPE2X2;}

		virtual void 		Transform(Path* const pPath, DocRect* const pRect, TransformBase& Trans);
		virtual ChangeCode	RecordContext(UndoableOperation*);
		virtual UINT32 		GetByteSize() const { return sizeof(MouldEnvelope2x2); };

		virtual MouldGeometry* MakeCopy();

	public:
		// none virtual functions local to this class.
		static BOOL WillBeValid(POINT* P);

	protected:
		BOOL CopyContents(MouldEnvelope2x2* pGeom);

	private:
		const INT32 EnvNumCoords;
};


/********************************************************************************************

>	class EnvelopeShapes

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Purpose:	The default envelope shapes class. This class provides a bunch of static
				functions to create some default shapes. All shapes are defined on a 
				(0,0),(72000,72000) coordinate system. 

********************************************************************************************/

class EnvelopeShapes
{
	public:
		static BOOL Rectangular(Path* pPath);
		static BOOL Rectangular2x2(Path* pPath);
		static BOOL Circular(Path* pPath);
		static BOOL Concave(Path* pPath);
		static BOOL Banner(Path* pPath);

	private:
		static BOOL MakeEllipse(DocRect* pRect, Path* pPath,PathFlags* pFlags);
};



/********************************************************************************************

	class RecordEnvelopeAction : public Action

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Purpose:	This class records undo/redo information for the Envelope shape object.
	
********************************************************************************************/

class RecordEnvelopeAction : public Action
{
	CC_DECLARE_DYNCREATE(RecordEnvelopeAction)

	public:
		 RecordEnvelopeAction();
		~RecordEnvelopeAction();
		virtual ActionCode Execute();
		static ActionCode Init( Operation* pOp,
								ActionList* pActionList,
								MouldEnvelopeBase* pRecEnvelope,
								Action** NewAction);
	private:
		MouldEnvelopeBase* pCEnvelope;
		POINT RecordArray[ENV_MAXCOORDS];
};

#endif
