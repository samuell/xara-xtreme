// $Id$
// Header for the controlling class of the v2 file format
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

// This class has been implemented completely inline for speed reasons
// It is called from every WriteXXXX function in the CXaraFileRecord class...

#ifndef INC_FTTYPLIS
#define INC_FTTYPLIS

#include "fixmem.h"

typedef enum
{
	FTT_BYTE = 1,
	FTT_UINT32,			// 2
	FTT_INT32,			// 3
	FTT_UINT16,			// 4
	FTT_INT16,			// 5
	FTT_FLOAT,			// 6
	FTT_DOUBLE,			// 7
	FTT_BINHEX,			// 8
	FTT_WCHAR,			// 9
	FTT_ASCII,			// A
	FTT_UNICODE,		// B
	FTT_COORD,			// C
	FTT_INTCOORD,		// D
	FTT_REFERENCE,		// E
	FTT_MAXTYPE			// F
} FTType;

// Now a couple of macros to make life easier in the write functions

#define ENTERWRITEFUNCTION(type)							\
	BOOL bFTTLUnlock = FALSE;								\
	if (m_pTypeList)										\
		bFTTLUnlock = m_pTypeList->AddToListAndLock(type);

#define LEAVEWRITEFUNCTION		\
	if (bFTTLUnlock)			\
		m_pTypeList->Unlock();

#define STORELENGTH(len)				\
	((bFTTLUnlock) ? (m_pTypeList->StoreLength(len)) : (ok))

/********************************************************************************************

>	class FTTypeList : public CCObject

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97
	Purpose:	Object used by CXaraFileRecord for recording type information while writing

********************************************************************************************/

class FTTypeList : public CCObject
{
	// Give my name in memory dumps
	// The matching implement macro is in cxfrec.cpp
	CC_DECLARE_DYNAMIC(FTTypeList);

public:
	FTTypeList()
	{
		m_pArray = NULL;
		m_Used = 0;
		m_bLock = FALSE;
		m_Length = 0;
	};

	virtual ~FTTypeList()
	{
		if (m_pArray)
			CCFree(m_pArray);
	};

	DWORD GetCount(void) { return(m_Used); };

	BOOL StoreLength(DWORD len)
	{
		// Ensure 4 bytes of space (chances are this is the last thing to be written)
		if (!Enlarge(4))
			return(FALSE);

		DWORD* pLen = (DWORD*)(m_pArray + m_Used);
		*pLen = len;
		m_Used += 4;
		return(TRUE);
	};
	
	BOOL AddToListAndLock(FTType Type)
	{
		if (m_bLock)
			return(FALSE);

		// Add the type to the list

		if (m_Used == m_Length)
		{
			if (!Enlarge())
				return(FALSE);
		}

		// This must exist now
		m_pArray[m_Used] = Type;
		m_Used++;

		m_bLock = TRUE;
		return(TRUE);
	};

	void Unlock(void) { m_bLock = FALSE; };

	BYTE* GetBuffer(void) { return(m_pArray); };

protected:
	// Ensures at least Size entries available
	// FALSE if failed
	BOOL Enlarge(DWORD Size = 16)
	{
		if ((m_pArray == NULL) ||
			((m_Length - m_Used) < Size))
		{
			DWORD NewLength = m_Length + Size;
			BYTE* pNew = (BYTE*)CCRealloc(m_pArray, NewLength);

			if (pNew == NULL)
				return(FALSE);

			m_pArray = pNew;
			m_Length = NewLength;
		}
		return(TRUE);
	};

protected:
	DWORD m_Length;
	DWORD m_Used;
	BOOL m_bLock;
	BYTE* m_pArray;
};

#endif	// INC_FTTYPLIS
