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
#ifndef INC_SWF_EXPORT
#define INC_SWF_EXPORT

#include "swfrndr.h"	// Definition of FlashRenderRegion.
//#include "filters.h"	// Base filter class. - in camtypes.h [AUTOMATICALLY REMOVED]

/********************************************************************************************

>	class SWFFilter : public VectorFilter

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/08/99
	Purpose:	Enables Camelot to export images in the Flash (SWF) file format.
	SeeAlso:	VectorFilter

********************************************************************************************/

class FlashFilter : public VectorFilter
{
	friend class FlashRenderCallback;

	CC_DECLARE_DYNAMIC( FlashFilter );

public:
	FlashFilter( void );							// Constructor.
	BOOL Init( void );								// Initialise the filter.

	BOOL DoImport( SelOperation *Op,				// Do an import.
					CCLexFile* pFile, 
					Document *DestDoc,
					BOOL AutoChosen,
					ImportPosition *Pos,
					KernelBitmap** ppImportedBitmap,
					DocCoord* pPosTranslate,
					String_256* URL );

	BOOL DoExport( Operation* pOp,					// Export file.
					CCLexFile* pFile,
					PathName* pPath, 
					Document* TheDocument,
					BOOL ShowOptions );

	BOOL PrepareToExport( CCLexFile* pFile,
						   Spread* pSpread );

	virtual BOOL ExportSelectionOnly(BOOL MaskedRender = FALSE);	// Should only export selected nodes.
	virtual BOOL ExportVisibleLayersOnly();							// Should hidden layers be exported.

	// Gets the colour of a page background.
	static DocColour GetPageColour( Spread	*pSpread,
									 Node** pBackground );

protected:
	// Custom export render method.
	virtual BOOL WriteNodes( RenderRegion *pRegion,
							  ExportDC *pDC,
							  BOOL VisibleLayersOnly,
							  BOOL CheckSelected,
							  BOOL ShowProgress );

private:
	void CleanUpAfterExport( BOOL Success );		// Tidy up after export.

	// Functions for extracting button state details.
	LayerState GetButtonState( Node *pNode );		// Determine whether pNode is a button.
	BOOL GetButtonName( Node *pNode,				// Get the name of pNode's button.
						 String_256 &ButtonName );

	// Members
	Node*		mpBackgroundNode;						// Pointer to the background colour node.
	BOOL		mOnlySelected;						// Only render selected nodes.
	DocColour*	mpBackgroundCol;					// Background colour for the image.
	DocRect		mPageRect;							// Bounding rectangle for spread.
};


/********************************************************************************************

>	class FlashRenderCallback : public RenderCallback

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/2004
	Purpose:	Handling callbacks from RenderTree
	Notes:		RenderTree function calls the main function in this class when it's about
				to render any Node.

********************************************************************************************/

class FlashRenderCallback : public RenderCallback
{
public:
	FlashRenderCallback(FlashFilter* pFilter, FlashRenderRegion* pFRR, BOOL bVisibleLayersOnly, BOOL bSelectedOnly, Node* pBackgroundNode)
	{
		m_pFilter = pFilter;
		m_pFRR = pFRR;
		m_bVisibleLayersOnly = bVisibleLayersOnly;
		m_bSelectedOnly = bSelectedOnly;
		m_pBackgroundNode = pBackgroundNode;
	}

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode);
	virtual BOOL BeforeSubtree(RenderRegion* pRegion,
								Node* pNode,
								Node** ppNextNode,
								BOOL bClip,
								SubtreeRenderState* pState);

private:
	FlashFilter* m_pFilter;
	FlashRenderRegion* m_pFRR;
	Node* m_pBackgroundNode;
	BOOL m_bVisibleLayersOnly;
	BOOL m_bSelectedOnly;
};


#endif // INC_SWF_EXPORT
