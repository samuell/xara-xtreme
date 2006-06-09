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
// Provides a class for dealing with Camelots various Blob types

#ifndef INC_BLOBS
#define INC_BLOBS

//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "pump.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class NodeRenderable;
class RenderRegion;
class DocRect;
class Spread;
class Tool_v1;


//static BOOL bToolBlobsAreOff;

/********************************************************************************************

>	class BlobStyle

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Purpose:	Class for holding which blobs are required and which are not. Objects of
				this class are passed into the Blob Manager to tell it which of the blobs
				a tool wants to display and should be used something like this :-
				MonoOn
				BlobStyle ThisToolsBlobs;
				ThisToolsBlobs.Object = TRUE;		// Display Object blobs in this tool
				ThisToolsBlobs.Fill = TRUE;			// Display Fill blobs in this tool
				BlobManager->ToolInterest(ThisToolsBlobs);
				MonoOff
	SeeAlso:	BlobManager

********************************************************************************************/

class BlobStyle
{
public:
	// Constructors
	BlobStyle();
	BlobStyle(	BOOL BObject, BOOL BArtistic = FALSE, BOOL BFill = FALSE,
				BOOL BTiny = FALSE, BOOL BPen = FALSE, BOOL BToolObject = FALSE, BOOL BEffect = FALSE );
	~BlobStyle() {}


/********************************************************************************************

>	BlobStyle::SetAll()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Purpose:	Sets all the blobs type flags to TRUE

********************************************************************************************/
	void SetAll()  { Object = Artistic = Fill = Tiny = Pen = ToolObject = Effect = TRUE; }


/********************************************************************************************

>	void BlobStyle::SetNone()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Purpose:	Sets all the blobs styles to false

********************************************************************************************/
	void SetNone() { Object = Artistic = Fill = Tiny = Pen = ToolObject = Effect = FALSE; }

	// Vars
	UINT32 Fill		: 1;
	UINT32 Object	: 1;
	UINT32 Artistic	: 1;
	UINT32 Tiny		: 1;
	UINT32 Pen		: 1;
	UINT32 ToolObject: 1;
	UINT32 Effect	: 1;
};



/********************************************************************************************

>	class BlobManager : public MessageHandler

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Purpose:	Class to Manage the various types of selection blobs in Camelot. Currently
				Camelot knows about the following forms of selection blob :-

				Fill Blobs - The blobs rendered by things like Graduated fills to show
				where the fill starts and ends, it direction etc.

				Object Blobs - The blobs that appear to edit the shape of an object. ie
				the control point blobs on a path

				Artistic Blobs - Used to edit information such as pressure details along
				shapes.

				Tiny Blobs - the Blob that is displayed by an object when it is the
				Boundary selection mode of the selector tool to indicate that it is one
				of the selected objects.

				ToolObject Blobs -	These blobs are displayed and used by an object only
									when a specific tool is selected. They are similar to
									object blobs except that the _object_ has the final say
									in whether they are rendered/used or not.

********************************************************************************************/

class BlobManager : public MessageHandler
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(BlobManager);

public:
	// Constructors etc
	BlobManager();
	~BlobManager() {}

	// Functions to allow the tools to register their interest in various groups of blobs
	void ToolInterest(BlobStyle);
	void AddInterest(BlobStyle);
	void RemoveInterest(BlobStyle);

	// Find out what the current blob interest is
	BlobStyle GetCurrentInterest(BOOL bIgnoreOffState = FALSE);

	// Functions to stop and start the rendering of blobs during periods of uncertainy
	void BlobRenderingOff(BOOL Redraw);
	void BlobRenderingOn(BOOL Redraw);

	// Functions to find out about the size of a blob (not a cow)
	INT32 GetBlobSize();
	void GetBlobRect( const DocCoord &Centre, DocRect* Rect, BOOL MFill = FALSE,
								BlobType eBlobType = BT_SELECTEDLARGEST);

	// Functions to render blobs
	void Render(DocRect* Rect, Spread *pSpread);
	void RenderOn(DocRect* Rect, Spread *pSpread);
	void RenderOff(DocRect* Rect, Spread *pSpread);

	void RenderMyBlobs(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode);
	void RenderMyBlobsOn(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode);
	void RenderMyBlobsOff(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode);

	void RenderObjectBlobs(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode);
	void RenderObjectBlobsOn(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode);
	void RenderObjectBlobsOff(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode);

	void RenderToolBlobsOn(Tool_v1* pTool, Spread* pSpread, DocRect* pClipRect);
	void RenderToolBlobsOff(Tool_v1* pTool, Spread* pSpread, DocRect* pClipRect);

	BOOL IsRemovingBlobs() { return RemovingBlobs; }

protected:
	// Functions to help the blob manager with the rendering of the blobs
	void RenderRequiredBlobs(NodeRenderable*, RenderRegion*);
	void RenderSpecificBlobs(NodeRenderable*, RenderRegion*, BOOL, BOOL, BOOL, BOOL, BOOL, BOOL, BOOL);

	void RenderRequiredBlobsOnSelection(DocRect* Rect);
	void RenderSpecificBlobsOnSelection(DocRect* Rect, BOOL, BOOL, BOOL, BOOL, BOOL, BOOL, BOOL);

	BOOL NeedToRenderSelectionBlobs(DocRect* Rect);
	BOOL NeedToRenderNodeBlobs(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode);

protected:
	// message handler
	MsgResult Message(Msg* Msg);

// vars
protected:
	// Which blobs are we displaying?
	BOOL IsFillBlob;
	BOOL IsObjectBlob;
	BOOL IsArtisticBlob;
	BOOL IsTinyBlob;
	BOOL IsPenBlob;
	BOOL IsToolObjectBlob;
	BOOL IsEffectBlob;

	// should we be drawing blobs at all
	BOOL DrawBlobsAtAll;

	// Flag to tell whether blobs are being removed or not
	BOOL RemovingBlobs;
	BOOL bToolBlobsAreOff;
};

#endif  // INC_BLOBS


