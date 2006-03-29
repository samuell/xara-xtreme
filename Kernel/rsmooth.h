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
// Contains class definitions for retrosmoothing functions

/*
*/

#ifndef INC_RETSMOOTH
#define INC_RETSMOOTH

#include "selop.h"
#include "nodepath.h"
#include "doccoord.h"

#define SMOOTH_MAX (2000*2000)
#define SMOOTH_MIN (256*256)

#define OPTOKEN_RETROSMOOTH _T("RetroSmooth")


//	define a region type to hold region start, end values

struct Set {
	INT32 first;
	BOOL Finclusive;
	INT32 last;
	BOOL Linclusive;
};

#define ITEM_TYPE Set



/******************************************************************************

	class DynArray

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created: 	16/11/94
	Purpose: 	A temporary class to control a dynamic array of doccoords.
				This will be replaced by a template class whenever we can
				use the template directive.
	
******************************************************************************/

class DynArray {

	public:
		DynArray();
		~DynArray();
        BOOL		Initialise(INT32 inititems, INT32 newitems);
        ITEM_TYPE*	GetFirstItem();
        INT32		NumItems() { return UsedSlots; };
        BOOL		AddItem(ITEM_TYPE&);
		void		DeleteItems(INT32 firstitem, INT32 numitems);
        void    	Empty(BOOL compress=TRUE);
        ITEM_TYPE&	operator[](const INT32 i);
		void 		Compact();
		BOOL 		CloneFrom(DynArray* SourceArray);

	private:
        BOOL	MakeSpaceInList(const INT32 numitems);

	private:
		MHANDLE BlockHandle;

		INT32	UsedSlots;
        INT32	UnusedSlots;
        INT32	SlotInitSize;
        INT32	SlotAllocSize;

};


/******************************************************************************

> class RetroSmooth

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created: 	11/11/94
	Purpose: 	A new class to control retro smoothing of path regions.

******************************************************************************/

class RetroSmooth
{
	public:
    	RetroSmooth();
		BOOL	Initialise();
		void 	Changing(NodePath* pNodePath, Spread* pSpread, double smooth);
		void 	Finished();
		void	Invalidate();
		double	ReturnCachedAccuracy(Path* pPath);

		// non-undoable version of finished. does not launch an operation
		BOOL	FinishedNoUndo(NodePath* pNodePath);
		void	SetRenderFlag(BOOL Render);
	private:
		BOOL 	Starting(NodePath* pDocNodePath);
		void	Update();
		BOOL 	RenderRegions();
		BOOL	NewPathToSmooth(NodePath* pNodePath);
		BOOL 	MatchRegions(NodePath* pNodePath);
		BOOL 	RecordPathRegions(Path* pPath);
		void	AlterRegion(INT32 index, INT32 newstart, INT32 newend);
		INT32	FindMasterRegion();
		void	TranslateRetroData(Path* pPath);

	private:
		BOOL		RetroSmoothing;
		NodePath* 	pRetroNode;				// a pointer to the current 'retro' node
		Spread*		pRetroSpread;
		Path		RetroPath;				// the original path points we need
		Path		RetroEorPath;			// the current path points after smoothing
		DynArray	RetroEorRegion;			// the working path regions
		double		RetroSmoothPercent;		// the current cached percentage
		double		RetroSmoothAcc;			// the current retro smoothing accurary
		INT32		RetroPathCRC;			// cyclic reduncancy for the EorPath
		INT32		RetroSelCRC;			// cyclic redundancy for the Selected points

		BOOL		m_bRender;				// flag to say whether or not we should do EOR rendering
};


/********************************************************************************************

>	class OpRetroSmooth : public SelOperation

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Purpose:	

********************************************************************************************/

class OpRetroSmooth : public SelOperation
{

CC_DECLARE_DYNCREATE(OpRetroSmooth)

public:
	OpRetroSmooth();
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			GetOpName(String_256* OpName);
	void			DoRetroSmooth(NodePath* pNode, Path* pPath, double smoothacc);
};


#endif
