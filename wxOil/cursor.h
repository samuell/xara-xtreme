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
// The Cursor class (interface).

#ifndef	CURSOR__INC
#define	CURSOR__INC

// Forward references:-
class Tool_v1;

/***************************************************************************************************

>	class Cursor : public CC_CLASS_MEMDUMP

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 November 1993
	Purpose:	Encapsulates OIL cursors, hiding OS specific functions behind a standard
				sensible interface.

***************************************************************************************************/

class Cursor : public CC_CLASS_MEMDUMP
{
public:
	static Cursor* Arrow;							// the standard arrow cursor
	static Cursor* Busy;							// the standard hour-glass cursor
	static Cursor* Insert;							// the standard I-beam cursor
	static Cursor* CrossHair;						// the standard crosshair cursor
	static Cursor* Progress;						// placeholder for the progress-counter cursor
	static Cursor* PointingHand;					// the Netscape-style pointing hand
	static Cursor* SizeNWSE;						// standard resize north-west / south-east

	static BOOL Init();								// creates platform-specific arrow and
	static void DeInit();							// hour-glass cursors etc.

	Cursor(LPCSTR lpStdCsrName);					// load a standard cursor, eg. the hour-glass
	Cursor(UINT32 wCsrID);							// load a cursor from the kernel's resources
	Cursor(Tool_v1* pTool, UINT32 wCsrID);			// load a cursor from a tool's resources
	Cursor(UINT32 wToolID, UINT32 wCsrID);				// ditto, but using the tool's ID
	Cursor(wxBitmap* pbmpAND, wxBitmap* pbmpXOR);		// create a cursor from a bitmap

	virtual ~Cursor();								// destroy a cursor

	BOOL IsValid() const;							// TRUE if cursor constructed OK
	void SetActive() const;							// set the system's cursor to this
	wxCursor *Handle() {return &hCursor;}			// returns the (wxWindows) handle of the cursor

//	static BOOL Show();								// show the cursor, if invisible
//	static BOOL Hide();								// hide the cursor, if visible
	BOOL Show();									// show the cursor, if invisible
	BOOL Hide();									// hide the cursor, if visible
	
	static INT32 Width();								// width and height of cursors, in pixels,
	static INT32 Height();							// on this system.

private:
	CC_DECLARE_MEMDUMP(Cursor);

	wxCursor		   hCursor;					// the Windows handle of the cursor
	BOOL				bMustDestroy;				// whether to deallocate on destruction
};



#endif	// CURSOR__INC
