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

// Node representing a bitmap object.

#ifndef INC_NODEBMP
#define INC_NODEBMP


#include "bitmap.h"
#include "noderect.h"
#include "doccolor.h"
//#include "xpehost.h"	// For OPTOKEN_XPE_EDIT

class SelOperation;
class AttrBitmapTranspFill;

/********************************************************************************************

>	class NodeBitmap : public NodeRect

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	A class for including bitmap objects within the tree.
	SeeAlso:	NodeRect, KernelBitmap

********************************************************************************************/

class NodeBitmap : public NodeRect
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE(NodeBitmap);

public:
	NodeBitmap(); 	
	NodeBitmap(	Node* ContextNode,
				AttachNodeDirection Direction,
				BOOL Locked = FALSE,
				BOOL Mangled = FALSE,
				BOOL Marked = FALSE,
				BOOL Selected = FALSE
			  );

	virtual Node* SimpleCopy();
	void CopyNodeContents(NodeBitmap* NodeCopy);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	virtual String Describe(BOOL Plural, BOOL Verbose);

	virtual KernelBitmap *EnumerateBitmaps(UINT32 Count);

	// Function for interrogation by clipboard to determine exportable data types
	virtual BOOL SupportsClipboardFormat(InternalClipboardFormat *Format) const;

	UINT32 GetBitmapDPI();

	// Render the bitmap
	virtual void Render( RenderRegion* pRender );
	virtual void PreExportRender(RenderRegion* pRender );
	virtual BOOL ExportRender( RenderRegion* pRender ); 
	virtual INT32 GetSizeOfExport(Filter*);

	virtual BOOL RequiresAttrib(NodeAttribute* pAttrib, BOOL Search = FALSE);
	virtual BOOL ApplyDefaultBitmapAttrs(UndoableOperation* pOp, AttrBitmapTranspFill* pTranspBitmap = NULL);

	// Popup menus
	virtual	BOOL OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu);

	// Information functions
	virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes
	virtual BOOL IsABitmap() const;
	virtual BOOL NeedsTransparency() const;

	// Make Shapes functions
	virtual BOOL CanBecomeA(BecomeA* pBecomeA);
	virtual BOOL DoBecomeA(BecomeA* pBecomeA);

	// AllowOp stuff
	BOOL AllowOp(ObjChangeParam*, BOOL);
	BOOL MakeContoneBitmap(UndoableOperation*);

	static KernelBitmap* CheckGreyscaleBitmap(KernelBitmap* pBitmap, UINT32 PromptID, UINT32 OkID);

	KernelBitmapRef BitmapRef;

	virtual KernelBitmapRef* GetBitmapRef() { return &BitmapRef; }
	virtual KernelBitmap* GetBitmap();

	virtual BOOL HidingNode();
	virtual BOOL ShowingNode();

	virtual void SetStartColour(DocColour*);
	virtual void SetEndColour(DocColour*);

	virtual DocColour* GetStartColour();
	virtual DocColour* GetEndColour();

	virtual BOOL GetApplyContoneColour() {return ApplyContoneColour;};

	// For render region capabilities
	BOOL HasSimpleOrientation(RenderRegion *);

	virtual BOOL OnClick(DocCoord PointerPos,
						 ClickType Click,
						 ClickModifiers ClickMods,
						 Spread* pSpread);

PORTNOTE("other","Change GetDefaultOpToken - OPTOKEN_XPE_EDIT reference")
	// Find out the optoken of the default operation for this node. (See EditSelectionOp and Return hotkey.)
	virtual TCHAR* GetDefaultOpToken() {return _T("None");}
		
	virtual void SetAspectRatio(double dExWidth, double dExHeight, BOOL bPathAndFill);
	virtual BOOL ReleaseCached(BOOL bAndParents = TRUE, BOOL bAndChildren = TRUE, BOOL bSelf = TRUE, BOOL bAndDerived = TRUE);

	// Functions to give effects info about this instance of the bitmap so that
	// they can operate in the the upright bitmap domain, then do the appropriate
	// instance transformation at the top of the effects stack
	virtual DocRect GetOriginalBitmapRect();
	virtual Matrix GetInstanceTransform();

	virtual BOOL CanSupplyDirectBitmap() {return TRUE;}
	virtual BOOL GetDirectBitmap(RenderRegion* pRender, LPBITMAPINFO* plpInfo, LPBYTE* plpBits, DocRect* pRect, Matrix* pMat, double* pdRes);

protected:
	virtual BOOL HasBitmapAttrs();

public:
	// new file format export functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	
protected:
	DocColour Colour;
	DocColour EndColour;

	BOOL ApplyContoneColour;
}; 

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

// Optoken for the CreateNodeBitmap operation
#define OPTOKEN_CREATENODEBITMAP _T("CreateNodeBitmap")	

/********************************************************************************************

>	class OpCreateNodeBitmap: public UndoableOperation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Purpose:	Creates a new Node Bitmap, at a specific position on the page.
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpCreateNodeBitmap: public UndoableOperation
{         
	CC_DECLARE_DYNCREATE( OpCreateNodeBitmap );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		

	void DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam);	
	virtual void GetOpName(String_256* OpName); // Overloaded so that we can store the undo string in 
												// the operation rather than the OpDescriptor.  

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return TRUE; }
};  

/********************************************************************************************

>	class ChangeBitmapPtrAction : public Action

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Purpose:	An action which undoes the modification of Grad Fill data. This action can
				be used for undoing changing control points.
	SeeAlso:	-

********************************************************************************************/

class ChangeBitmapPtrAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeBitmapPtrAction)

public:
	ChangeBitmapPtrAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							Action** NewAction);
	void StoreChanges(NodeBitmap* pBmpNode);

	KernelBitmapRef* GetBitmapRef() { return &BitmapRef; }
	KernelBitmap* GetBitmap() { return BitmapRef.GetBitmap(); }

protected:

	NodeBitmap* 		pChangedBmpNode;
	KernelBitmapRef 	BitmapRef;
};

#endif  // INC_NODEBMP


