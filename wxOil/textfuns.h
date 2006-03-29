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
// Header for the TextFuns implementation

#ifndef INC_TEXTFUNS
#define INC_TEXTFUNS

#include "fntcache.h"
#include "pathtype.h"
//#include "textinfo.h"
class CharDescription;
class DocCoord;


// enums ...
enum CharCase { Failed=0, Lower, Upper, Swap, Read, UnknownType };

// constants
const WCHAR FONTEMCHAR = 'M';	// for finding the width of an 'em' etc

/********************************************************************************************
>	class CharMetrics : public CC_CLASS_MEMDUMP

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Purpose:	parameter passing of char metrics
********************************************************************************************/

class CharMetrics : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(CharMetrics);

public:
	CharMetrics();
	void Scale(double ScaleX, double ScaleY);

public:
	MILLIPOINT CharWidth;
	MILLIPOINT FontAscent;
	MILLIPOINT FontDescent;
	MILLIPOINT FontEmWidth;
};

/********************************************************************************************
>	struct MillipointKerningPair

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/2000
	Purpose:	Same as windows structure KERNINGPAIR but using millipoints
********************************************************************************************/
struct MillipointKerningPair
{
	WORD		wFirst; 
	WORD		wSecond; 
	MILLIPOINT  iKernAmount; 
};

/********************************************************************************************
>	struct FontInfo

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/95
	Purpose:	parameter passing of info extracted from a LogFont
********************************************************************************************/

struct FontInfo
{
	WORD       Handle;
	MILLIPOINT Size;
	FIXED16    Aspect;
	BOOL       Bold;
	BOOL       Italic;
	ANGLE      Rotation;
};


/********************************************************************************************
>	class TextManager:	public CCObject

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95
	Purpose:	You name it and its assocaited with text then this probably isn't it
********************************************************************************************/

class TextManager: public CCObject
{
	//CC_DECLARE_DYNAMIC(TextManager)// not required as this is a static class

public:
	static BOOL GetLogFontFromCharDescriptor(wxDC* pDC, CharDescription& ChDesc, LPLOGFONT pLogFont,
												INT32 LogicalHeight=-1);

	static BOOL GetTTCharPath(CharDescription& ChDesc, DocCoord** ppCoords, PathVerb** ppVerbs,
																		UINT32* pNumCoords, wxDC* pDC );

  	static BOOL GetBezierFromChar(wxDC* pDC, WCHAR CharNumber, LPLOGFONT pLogFont,
  								  DWORD* NoElements, POINT* pPolyCordBuffer, BYTE* pPolyVerbBuffer);
  
	static CharCase ProcessCharCase(WCHAR* pChar, CharCase NewState);

	static MILLIPOINT GetDefaultHeight() { return 72000; }	// size of char-paths/advance-widths in caches etc

	static BOOL GetInfoFromLogFont(FontInfo* pFontInfo, LOGFONT* pLogFont, FontClass Class=FC_UNDEFINED);

	static INT32 GetDesignSize(wxDC* pDC);

	static BOOL GetCharWidth(    wxDC* pDC, WCHAR FirstChar, WCHAR LastChar, INT32* pCharWidthsBuf);
	static BOOL GetCharABCWidths(wxDC* pDC, WCHAR FirstChar, WCHAR LastChar, ABC* pCharABCWidthsBuf);

	static INT32	GetKernCount(wxDC* pDC);
	static bool FillKernArray(wxDC* pDC, MillipointKerningPair* pKerningPairs, INT32 count);
};

#endif
