// $Id: draginfo.h 662 2006-03-14 21:31:49Z alex $
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

// draginfo.h - Definitions for global dragging system DragInformation class

#ifndef INC_DRAGINFO
#define INC_DRAGINFO

class Cursor;
class DragTarget;
class ViewDragTarget;
class KernelBitmap;

/*********************************************************************************************

>	class DragInformation : public CCObject

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/95
	Purpose:	Objects derived from this abstract class are used to describe everything
				useful to know about the current global drag. They allow drag targets to
				decide what to do with a recieved drag.

	SeeAlso:	DragManagerOp::StartDrag; DragTarget

	Documentation:	Docs\Specs\DragMan.doc

*********************************************************************************************/

class CCAPI DragInformation : public CCObject
{
	friend class DragManagerOp;
	friend class CaptureWnd;

	CC_DECLARE_DYNCREATE(DragInformation)

	
protected:
	DragInformation(BOOL IsAdjustDrag = FALSE);


public:
	virtual void OnClick(INT32 flags, CNativePoint Point);	

	virtual UINT32 GetCursorID();
	virtual BOOL GetStatusLineText(String_256 * TheText);
	virtual BOOL OnDrawSolidDrag(CNativePoint Origin, CNativeDC * TheDC);

	virtual UINT32 GetCursorID(DragTarget* pDragTarget);
	virtual BOOL GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget);
	virtual BOOL OnDrawSolidDrag(CNativePoint Origin, CNativeDC * TheDC, DragTarget* pDragTarget);

	BOOL IsAnAdjustDrag()						{ return IsAdjustDrag; }


public:		// Solid-dragging support
	virtual INT32 GetDragTransparency()			{ return 0; }		// Not transparent
	virtual KernelBitmap *GetSolidDragMask()	{ return NULL; }	// Not masked


public:		// Special dropping functions called for drop-on-page (called by ViewDragTarget)
	virtual BOOL CanDropOnPage()				{ return FALSE; }
	virtual BOOL OnPageDrop(ViewDragTarget*)	{ return FALSE; }


protected:	// Member variables
	static Cursor	   *MyCursor;

	BOOL				DoesSolidDrag;
	CNativePoint		SolidDragOffset;
	CNativeSize			SolidDragSize;

	BOOL				IsAdjustDrag;
};

#endif

