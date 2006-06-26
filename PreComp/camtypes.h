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

#ifndef INC_CAMTYPES
#define INC_CAMTYPES

#if defined(EXCLUDE_FROM_XARLIB) && !defined(EXCLUDE_FROM_XARALX)
#define EXCLUDE_FROM_XARALX
#endif

//This removes the UNWIND_SEMATICS warnings during a build
#pragma warning(disable:4530)

// this file includes the types which everyone generally needs but which are NOT
// machine-dependent. Any machine-dependent types should go in winoil\oiltypes.h
// NOTE: ENSURE #include lines are less than 25 lines apart (if you don't,
// nmake depend will not work) - markn & neville 4-8-97

// Include machine-specific types
#include "stdwx.h"

// This has been placed here to ensure that min and max are available
// if ccfile.h is not included (via document.h below)
using namespace std;

// This must be the first file included of ours as it defines _R()
// note it does include other stuff, but only after it has defined _R()
#include "camresource.h"

// ADDR is a general pointer type. Using void* would mean casts of every use so it is a 
// pointer to BYTE, to make address calculation marginally easier.
typedef BYTE* ADDR;

// This is the correct definition of MILLIPOINT
typedef INT32 MILLIPOINT;

// Type for Tags (unique IDs)
typedef UINT32 	TAG;


// This must come after MILLIPOINT as it includes strings.h, which uses millipoints.
#include "ccmaths.h"						// as XMILLIPOINT needs XLONG

// Try to speed up compilation times
#include "ccobject.h"
#include "errors.h"
#include "camstring.h"

#include "ccfile.h"
#include "list.h"
#include "listitem.h"
#include "fixmem.h"
#include "memblk.h"

#if !defined(EXCLUDE_FROM_XARLIB)

#include "paldefs.h"
#include "app.h"
#include "document.h"
#include "docview.h"
#include "range.h"
#include "rndrgn.h"
#include "tool.h"
#include "epsfiltr.h"
#include "ink.h"
#include "nodeattr.h"
#include "attrval.h"
#include "undoop.h"
#include "bars.h"
#include "sgallery.h"

#endif

#include "paths.h"
#include "cxfdefs.h"
#include "cxfrech.h"
#include "cxfrec.h"

#if !defined(EXCLUDE_FROM_XARLIB)
// Alex added these which are included by more than 100 files
#include "camfiltr.h"
#include "npaper.h"
#include "iprofile.h"
#include "biasgain.h"
#include "fillattr.h"
#include "animparams.h"
#include "spread.h"
#include "nodecomp.h"
#include "rrcaps.h"
#include "fillval.h"
#include "becomea.h"
#include "group.h"

#endif



//-----------------------------------------------------------------------------------------//
// Some globally useful coordinate values...

// The following two values define the boundaries within which all physical coords must exist. 
// No data structure should EVER hold these values - they just represent the limits.
// They are measured in millipoints.

const MILLIPOINT MinDocCoord = -0x40000000;			 // Smallest allowable physical coordinate
const MILLIPOINT MaxDocCoord =  0x40000000;			 // Largest allowable physical coordinate
const MILLIPOINT MaxPasteboard = MILLIPOINT(500 * 72000 / 25.4); // Maximum pasteboard width/height

//-----------------------------------------------------------------------------------------//
// LogicalCoord
//
// Structure holding a 64-bit cartesian coordinate.
// Its main use is to hold the physical coordinates of objects in chapter space.
//
// IMPORTANT: do not add any virtual functions or other members to this structure as it is
// crucial that it stays in this form, else the union below in Rect will stop working.

struct LogicalCoord
{
	XMILLIPOINT	x;
	XMILLIPOINT	y;
};

// The following are included automatically by the above (for reference)
//#include "arrows.h"
//#include "attr.h"
//#include "attrmgr.h"
//#include "barmsg.h"
//#include "basedoc.h"
//#include "basestr.h"
//#include "binds.h"
//#include "binreloc.h"
//#include "bitmap.h"
//#include "bitmapcache.h"
//#include "bitmapcachekey.h"
//#include "bmplist.h"
//#include "camtypes.h"
//#include "capturemanager.h"
//#include "ccfile.h"
//#include "clikmods.h"
//#include "colcarry.h"
//#include "colmodel.h"
//#include "compatdef.h"
//#include "convert.h"
//#include "coord.h"
//#include "cursor.h"
//#include "defalloc.h"
//#include "dialogop.h"
//#include "dibconv.h"
//#include "dibutil.h"
//#include "dlgtypes.h"
//#include "doccolor.h"
//#include "doccomp.h"
//#include "doccoord.h"
//#include "dockbar.h"
//#include "docmsgs.h"
//#include "docrect.h"
//#include "docvmsg.h"
//#include "dragtrgt.h"
//#include "ensure.h"
//#include "epsclist.h"
//#include "epsstack.h"
//#include "exception.h"
//#include "filters.h"
//#include "fix24.h"
//#include "fixed.h"
//#include "fixed16.h"
//#include "fixed24.h"
//#include "fixmem.h"
//#include "fixst128.h"
//#include "fixst256.h"
//#include "fixstr16.h"
//#include "fixstr32.h"
//#include "fixstr64.h"
//#include "fixstr8.h"
//#include "flags.h"
//#include "fontclass.h"
//#include "fraclist.h"
//#include "gconsts.h"
//#include "genlist.h"
//#include "genstack.h"
//#include "gradtype.h"
//#include "handles.h"
//#include "iterator.h"
//#include "list.h"
//#include "listitem.h"
//#include "matrix.h"
//#include "memblk.h"
//#include "memdebug.h"
//#include "memory.h"
//#include "mfccontainer.h"
//#include "monotime.h"
//#include "msg.h"
//#include "node.h"
//#include "oilcoord.h"
//#include "oilfltrs.h"
//#include "oilrect.h"
//#include "opdesc.h"
//#include "ops.h"
//#include "outptdib.h"
//#include "outptgif.h"
//#include "pathname.h"
//#include "pathshap.h"
//#include "pathtype.h"
//#include "prefs.h"
//#include "pump.h"
//#include "quality.h"
//#include "ralphint.h"
//#include "rect.h"
//#include "release.h"
//#include "rendtype.h"
//#include "rgnlist.h"
//#include "rndstack.h"
//#include "selmsg.h"
//#include "selop.h"
//#include "selstate.h"
//#include "sgscan.h"
//#include "sgtree.h"
//#include "slowjob.h"
//#include "stockcol.h"
//#include "thumb.h"
//#include "tranform.h"
//#include "trans2d.h"
//#include "txtattr.h"
//#include "units.h"
//#include "unittype.h"
//#include "varstr.h"
//#include "view.h"
//#include "wincoord.h"
//#include "winrect.h"
//#include "wrkcoord.h"
//#include "xlong.h"
//#include "xmatrix.h"


#endif		// !INC_CAMTYPES
