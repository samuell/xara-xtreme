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

#ifndef INC_XPFRGN
#define INC_XPFRGN

//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class PluginNativeFilter;
class CapabilityTree;
class TextStory;
class TextLine;
class FormatRegion;
class CommonTransInfo;

/********************************************************************************************

>	class XPFRenderRegion : public RenderRegion

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the custom render region for the capabilities conversion

********************************************************************************************/

class XPFRenderRegion : public RenderRegion
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(XPFRenderRegion);

	// Construction and destruction
	XPFRenderRegion(PluginNativeFilter* pFilter, CapabilityTree* pPlugCaps, CommonTransInfo* pTransInfo = NULL);
	~XPFRenderRegion();

	virtual BOOL AttachDevice(View*, CNativeDC*, Spread* SpreadToAttach = NULL);

	// Starting and stopping the rendering process
	virtual BOOL StartRender();
	virtual BOOL StopRender();

	// Functions that are meant to do the drawing
	virtual void DrawPathToOutputDevice(Path *PathToRender, PathShape shapePath=PATHSHAPE_PATH);
	virtual void DrawRect(DocRect *RectToRender);
	virtual void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint);
	virtual void DrawPixel(const DocCoord &Point);

	// Functions that are supposed to draw bitmaps
	virtual void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap);
	virtual void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL);
	virtual BOOL DrawTransformedBitmap(NodeBitmap *pNodeBitmap);

	// Other Pure virtual drawing functions that have to be overidden
	virtual void DrawDragRect(DocRect *RectToRender);
	virtual void DrawBlob(DocCoord p, BlobType type);
	virtual void DrawCross(const DocCoord &Point, const UINT32 Size);
	virtual void DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape);
	virtual void DrawBitmapBlob(const DocCoord &Point, ResourceID resID);

	// Pure Virtual functions that have to be overidden to use this class.
	// They all do nothing in this class
	virtual void InitClipping();
	virtual void InitAttributes();
	virtual void SetLineAttributes();
	virtual void SetOSDrawingMode();
	virtual void SetQualityLevel();
	virtual MILLIPOINT CalcPixelWidth();
	virtual MILLIPOINT CalcScaledPixelWidth();

	// Force this to return a sensible value so that the capture system
	// doesn't get horribly upset
	virtual double GetPixelsPerInch() { return(m_PixelsPerInch); }

	// Make sure that offscreen attrs are rendered when out of bounds
	virtual BOOL CheckOffscreenBounds() { return(FALSE); }

	// Function to work out what things this type of render region is capable of rendering
	virtual void GetRenderRegionCaps(RRCaps* pCaps);

	// We must have all nodes rendered by RenderTree
	virtual BOOL RenderPaperAsInk() { return(TRUE); }

protected:
	PluginNativeFilter* m_pFilter;
	double m_PixelsPerInch;
	CommonTransInfo* m_pTransInfo;
};


class CommonTransInfo
{
public:
	CommonTransInfo();

	BOOL IsCommonType() { return(m_bCommonType); }
	void UpdateCommonType(UINT32 Type);
	UINT32 GetCommonType();
	BOOL UsesNonAlpha() { return(m_bUsesNonAlpha); }

protected:
	BOOL m_bCommonType;
	UINT32 m_CommonType;
	BOOL m_bUsesNonAlpha;
};


class SpanListItem : public ListItem
{
public:
	CC_DECLARE_DYNAMIC( SpanListItem )
	SpanListItem(Node* pFirstNode = NULL, Node* pLastNode = NULL, BOOL bNonAlpha = FALSE)
	{
		m_pFirstNode = pFirstNode;
		m_pLastNode = pLastNode ? pLastNode : pFirstNode;
		m_bNonAlphaTrans = bNonAlpha;
	}
	
	Node* m_pFirstNode;					// pointer to first node in span
	Node* m_pLastNode;					// pointer to last node in span
	BOOL m_bNonAlphaTrans;				// TRUE if span contains any non-alpha transparency
};


class NodeThreeBoolListItem : public ListItem
{
public:
	CC_DECLARE_DYNAMIC( NodeThreeBoolListItem )

	NodeThreeBoolListItem(Node* pNode = NULL, BOOL bFirst = FALSE, BOOL bSecond = FALSE, BOOL bThird = FALSE)
	{
		m_pNode = pNode;
		m_bFirst = bFirst;
		m_bSecond = bSecond;
		m_bThird = bThird;
	}
	
	Node* m_pNode;			// pointer to node
	BOOL m_bFirst;			// first flag
	BOOL m_bSecond;			// second flag
	BOOL m_bThird;			// third flag
};



/********************************************************************************************

>	class XPFRenderCallback : public RenderCallback

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the custom render callback class for the capabilities conversion

********************************************************************************************/

class XPFRenderCallback : public RenderCallback
{
public:
	XPFRenderCallback(PluginNativeFilter* pFilter, XPFRenderRegion* pXPFRegion, CapabilityTree* pCapTree, INT32 ConvertPass);
	~XPFRenderCallback();

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode);
	virtual BOOL BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState);
//	virtual BOOL AfterSubtree(RenderRegion* pRegion, Node* pNode);

public:
	Spread* GetNewSpread() { return(m_pNewSpread); }
	void SetAttachContext(Node* pContext, AttachNodeDirection Direction)
	{
		m_pContextNode = pContext;
		m_Direction = Direction;
	}

	BOOL ConvertNodes();

protected:
	BOOL ConvertNodesForPass2();
	BOOL ConvertNodesForPass3();
	BOOL ConvertNodesForPass4();
	BOOL ConvertNodesForPass5();

	void RemoveChildAttrs(Node* pNode, CCRuntimeClass* pClass);
	NodeAttribute* FindChildAttr(Node* pNode, CCRuntimeClass* pClass);

	Node* RenderNodesToBitmap(Node* pFirstNode, Node* pLastNode, BOOL bNonAlphaTrans);
	KernelBitmap* RenderFillToBitmap(Node* pNode, DocRect& BoundsRect);
	KernelBitmap* RenderTransToBitmap(Node* pNode, DocRect& BoundsRect, UINT32* pTransType);
	KernelBitmap* RenderFillAndTransToBitmap(Node* pNode, DocRect& BoundsRect);

	TextStory* ReformatTextStory(TextStory* pStory);
	Node* ReformatTextLine(TextLine* pLineNode, FormatRegion* pFormatRegion);
	BOOL CopyAttributesFromNode(Node* pDestNode, Node* pSrcNode);

	BOOL DoesNodeUseNonAlphaTrans(Node* pRootNode);
	BOOL FindCommonTransTypeToApply(Node* pFirstNode, Node* pLastNode, UINT32* pCommonType);

private:
	INT32 m_ConvertPass;
	PluginNativeFilter* m_pFilter;
	XPFRenderRegion* m_pXPFRegion;
	CapabilityTree* m_pCapTree;
	Spread* m_pNewSpread;
	Node* m_pContextNode;
	AttachNodeDirection m_Direction;
	Node* m_pSpanParent;
	List m_ConvertList;
	List m_ParentList;
};



class XPFSpanRenderCallback : public RenderCallback
{
public:
	typedef enum
	{
		RS_BEFORESPAN = 0,
		RS_INSPAN,
		RS_AFTERSPAN,
	} RenderStateType;

	XPFSpanRenderCallback(Node* pFirst, Node* pLast, BOOL bBackground)
	{
		m_pFirstNode = pFirst;
		m_pLastNode = pLast;
		m_bBackground = bBackground;
		m_RenderState = RS_BEFORESPAN;
	}

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode);
	virtual BOOL BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState);
	virtual BOOL AfterSubtree(RenderRegion* pRegion, Node* pNode);

protected:
	Node* m_pFirstNode;					// pointer to first node in span
	Node* m_pLastNode;					// pointer to last node in span
	BOOL m_bBackground;					// True if nodes before first should be rendered
	RenderStateType m_RenderState;
};


/********************************************************************************************

>	class XPFView : public View

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/08/2005

	Purpose:	Simple view class for use when converting for plugin filters

	SeeAlso:	View

********************************************************************************************/

class XPFView : public View
{
	CC_DECLARE_DYNAMIC(XPFView);
public:
	virtual ~XPFView();	
	BOOL Init(double DPI);

public:
	virtual BOOL ViewStateChanged();			// informs us that ViewState has changed

public:
	virtual void SetViewPixelSize();			// Set pixel size according to this view.

public:
	virtual void ContinueRenderView(RenderRegion*, Spread*, BOOL = TRUE, BOOL = TRUE,
									 BOOL bForceImmediate = FALSE);
	virtual CNativeDC *GetRenderDC();

public:
	virtual BOOL GetForeBackMode();
	virtual void SetForeBackMode(BOOL);
	virtual double GetConvertToEditableShapesDPI();

public:
	virtual DocRect GetDocViewRect(Spread*);
	virtual void SetExtent(DocCoord, DocCoord);	// Reset the extent of the document
	virtual WorkRect GetViewRect();				// The physical rect of the view

protected:
	double m_DPI;
};



#endif  // INC_XPFRGN
