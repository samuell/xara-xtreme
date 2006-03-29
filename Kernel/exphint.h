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
//
// Declaration of ExportHint class

#ifndef INC_EXPHINT
#define INC_EXPHINT

#include "cxfrech.h"

enum ExportHintBitmapType
{
	HINTTYPE_BAD = 0,
	HINTTYPE_JPEG,
	HINTTYPE_GIF,
	HINTTYPE_PNG,
	HINTTYPE_MAX
};

/********************************************************************************************

>	class ExportHint : public CC_CLASS_MEMDUMP

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Purpose:	Encapsulates the Export Hint for a document

********************************************************************************************/

class ExportHint : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(ExportHint);

public:
	// Construction and initialisation
	ExportHint();
	ExportHint(const ExportHint& other);
	~ExportHint() {};

public:
	// Public access functions
	UINT32 GetType(void) { return(m_Type); };
	UINT32 GetWidth(void) { return(m_Width); };
	UINT32 GetHeight(void) { return(m_Height); };
	UINT32 GetBPP(void) { return(m_BPP); };
	const String_256& GetOptionsString(void) { return(m_sOptions); };

	void SetType(UINT32 Type) { m_Type = Type; };
	void SetWidth(UINT32 Width) { m_Width = Width; };
	void SetHeight(UINT32 Height) { m_Height = Height; };
	void SetBPP(UINT32 BPP) { m_BPP = BPP; };
	void SetOptionsString(const String_256& sOpt) { m_sOptions = sOpt; };

public:
	// Other public functions
	const ExportHint& operator=(const ExportHint& other);

public:
	// Functions solely for the use of the filters...
	BOOL WriteExportHintRecord(BaseCamelotFilter* pFilter);
	BOOL InitFromRecord(CXaraFileRecord* pRecord);

protected:
	UINT32 m_Type;
	UINT32 m_Width;
	UINT32 m_Height;
	UINT32 m_BPP;
	String_256 m_sOptions;
};

/********************************************************************************************

>	class ExportHintRecordHandler : public CamelotRecordHandler

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/96

	Purpose:	Handles TAG_EXPORT_HINT

********************************************************************************************/

class ExportHintRecordHandler : public CamelotRecordHandler
{
	// Support full runtime classing
	CC_DECLARE_DYNAMIC(ExportHintRecordHandler);

public:
	ExportHintRecordHandler() {};
	virtual ~ExportHintRecordHandler() {};

	// functions that must be implemented.
	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);

#if XAR_TREE_DIALOG
	virtual void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);
	virtual void GetTagText(UINT32 Tag,String_256& Str);
#endif
};

#endif  // INC_EXPHINT
