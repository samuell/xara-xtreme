// $Id: cliptype.h 662 2006-03-14 21:31:49Z alex $
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
// InternalClipboardFormat - a simple class to describe an internal clipboard format
// (A class is used so that we need not change node.h if we wish to change the contents)


#ifndef INC_CLIPTYPE
#define INC_CLIPTYPE


// Clipboard formats. New formats can be used by external parties by defining an unused value
const INT32 CLIPTYPE_NONE		= 0;
const INT32 CLIPTYPE_VECTOR	= 1;
const INT32 CLIPTYPE_TEXT		= 2;
const INT32 CLIPTYPE_BITMAP	= 3;



/********************************************************************************************

>	class InternalClipboardType : public CC_CLASS_MEMDUMP

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	
	Purpose:	This class is a very simple class which describes an internal clipboard
				data format. Each ClipboardMapping uses one of these to describe the type
				of input data they require in order to export, and thes export mappings
				will only be available to the clipboard if at least one node in the
				InternalClipboard supports output in this internal format.
				
				These formats are (currently):
					CLIPTYPE_VECTOR, CLIPTYPE_TEXT, CLIPTYPE_BITMAP.

				VECTOR includes all nodes, and is always assumed to be exportable.
				i.e. it's an "I can always export" thing.

				TEXT includes text characters only (text can be exported as paths or
				as unicode text). That is, an export mapping that supports Text can
				export text characters only, so will not be placed upon the clipboard
				unless there is some text somewhere on the clipboard.

				BITMAP is used for exporting bitmap fills as simple bitmaps. i.e. If no
				bitmap fills are on the clipboard, Bitmap mappings will not be available.

	SeeAlso:	ExternalClipboard; Node::SupportsClipboardFormat; ClipboardMapping

*********************************************************************************************/

class CCAPI InternalClipboardFormat : public CC_CLASS_MEMDUMP
{
// See clipext.cpp for the IMPLEMENT section
CC_DECLARE_MEMDUMP(InternalClipboardFormat)

public:
	InternalClipboardFormat() 				{ FormatID = CLIPTYPE_NONE; }
	InternalClipboardFormat(INT32 Format)		{ FormatID = Format; }

	BOOL IsSameFormat(InternalClipboardFormat &OtherFormat) const	\
											{ return(FormatID == OtherFormat.FormatID); }
		// Returns TRUE if the formats are the same

public:		// Intended only for use by ClipboardMapping class
	void SetFormatID(INT32 Format)			{ FormatID = Format; }
	INT32 GetFormatID(void)					{ return(FormatID); }


protected:
	INT32 FormatID;
};


#endif

