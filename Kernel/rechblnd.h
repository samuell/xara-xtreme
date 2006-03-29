// $Id$
// Header for the blend record handling class for the v2 file format
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

#ifndef INC_RECHBLND
#define INC_RECHBLND

#include "cxfrech.h"

class CXaraFileRecord;
class NodeBlender;
class NodeBlend;
class NodeBlendPath;
/***********************************************************************************************

>	class BlendRecordHandler : public CamelotRecordHandler

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Purpose:	Handles the reading of all blend records in the v2 file format
	SeeAlso:	-

***********************************************************************************************/

class BlendRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(BlendRecordHandler);

public:
	BlendRecordHandler() : CamelotRecordHandler() { m_pLastInsertedNodeBlender = NULL; 
													m_pLastInsertedNodeBlend = NULL;
													m_pLastNodeBlendPath = NULL;
													m_bLoadedProfiles    = FALSE;}
	~BlendRecordHandler() {}

	// functions that must be implemented.
	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);

#ifdef XAR_TREE_DIALOG
	virtual void GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr);
#endif

	static void SetLastInsertedNodeBlender(NodeBlender* pNodeBlender)	{ m_pLastInsertedNodeBlender = pNodeBlender; }
	static void SetLastInsertedNodeBlend(NodeBlend* pNodeBlend) { m_pLastInsertedNodeBlend = pNodeBlend; }
	static void SetLastInsertedNodeBlendPath(NodeBlendPath* pNodeBlendPath) { m_pLastNodeBlendPath = pNodeBlendPath; }

private:
	static NodeBlender* GetLastInsertedNodeBlender()					{ return m_pLastInsertedNodeBlender; }
	static NodeBlend* GetLastInsertedNodeBlend()						{ return m_pLastInsertedNodeBlend; }
	static NodeBlendPath* GetLastInsertedNodeBlendPath()               { return m_pLastNodeBlendPath; }


	BOOL HandleBlendRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleBlenderRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleBlenderCurvePropRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleBlenderCurveAnglesRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleBlendPathRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleBlendProfileRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleBlenderExtraRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleBlendPathFlags(CXaraFileRecord* pCXaraFileRecord);
	
	static NodeBlender* m_pLastInsertedNodeBlender;
	static NodeBlend* m_pLastInsertedNodeBlend;
	static NodeBlendPath* m_pLastNodeBlendPath;

	// DY 12/7/2000 Due to problems loading v.3 files into the browser plug in I had to 
	// reverse the order in which blend data was saved and loaded.  We now need to cache
	// profile data as it comes in and give it to the next blend to be loaded
	static BOOL m_bLoadedProfiles;
	static CProfileBiasGain m_LastObjectProfile;
	static CProfileBiasGain m_LastAttrProfile;
	static CProfileBiasGain m_LastPositionProfile;
};

#endif //INC_RECHBLND
