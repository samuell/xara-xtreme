// $Id: brushmsg.h 662 2006-03-14 21:31:49Z alex $
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

// Headers for the brush message and message handler classes

#ifndef BRUSHMSG
#define BRUSHMSG

#include "msg.h"
#include "brshcomp.h"

/********************************************************************************************

>	class BrushMsg: public Msg

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Purpose:	A message broadcast by a brush when it becomes selected as the default brush 
	SeeAlso:	-

********************************************************************************************/

class BrushMsg: public Msg
{
	CC_DECLARE_DYNAMIC(BrushMsg); 

public:	
	BrushMsg() {m_BrushSelected = BrushHandle_NoBrush;}
	BrushMsg(BrushHandle Handle) {m_BrushSelected = Handle;}

	BrushHandle m_BrushSelected;

};

/********************************************************************************************

>	class ScreenChangeMsg: public Msg

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Purpose:	A message to be broadcast whenever what is on the screen changes.  This can be
				a dialog closing, or scrollbar moving or object being moved.  It is vital to 
				the brush that it be informed whenever something like this happens
	SeeAlso:	Currently the only place this message is received is in the freehand info bar

********************************************************************************************/

class ScreenChangeMsg: public Msg
{
	CC_DECLARE_DYNAMIC(ScreenChangeMsg); 

public:	
	ScreenChangeMsg() {m_bClear = FALSE;}
	ScreenChangeMsg(BOOL Clear) {m_bClear = Clear;}
	BOOL m_bClear; 
};


/********************************************************************************************

>	class BrushDefMsg: public Msg

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Purpose:	A message broadcast when a brush definition is edited.  This enables things
				that represent the brush definition (such as the combo in the freeinfobar) to 
				update themselves
	SeeAlso:	-

********************************************************************************************/

class BrushDefMsg: public Msg
{
	CC_DECLARE_DYNAMIC(BrushDefMsg); 

public:	
	BrushDefMsg() {m_BrushEdited = BrushHandle_NoBrush;}
	BrushDefMsg(BrushHandle Handle) {m_BrushEdited = Handle;}

	BrushHandle m_BrushEdited;

};


/********************************************************************************************

>	class NewBrushMsg: public Msg

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Purpose:	A message broadcast when a new brush is created from the Brush edit dialog. It
				should be picked up by the Freehand infobar and used to update its brush list.
	SeeAlso:	-

********************************************************************************************/

class NewBrushMsg : public Msg
{
	CC_DECLARE_DYNAMIC(NewBrushMsg);

public:
	NewBrushMsg() {}

};

/********************************************************************************************

>	class StrokeMsg: public Msg

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/94
	Purpose:	A message broadcast by a Stroke when it becomes selected as the default Stroke 
	SeeAlso:	-

********************************************************************************************/

class StrokeMsg: public Msg
{
	CC_DECLARE_DYNAMIC(StrokeMsg); 

public:	
	StrokeMsg() {}

};
#endif
