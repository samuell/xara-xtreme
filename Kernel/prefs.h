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

#ifndef INC_PREFS
#define INC_PREFS

#include <limits.h>
#include <float.h>

#include "list.h"

// Opaque types
class PreferenceSection;
class OILPreferences;
class StringBase;

// Union for accessing polymorphic preference data
// used to be in oilprefs.h before 25/3/97
typedef union
{
	INT32		*pInt;
	UINT32		*pUInt;
	double		*pDouble;
	StringBase	*pString;
} PrefData;

// This is here because OILPreferences needs it
typedef enum
{
	PREF_INT,
	PREF_UINT,
	PREF_DOUBLE,
	PREF_STRING
} PreferenceType;

/********************************************************************************************

>	class Preferences : public List

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Purpose:	Maintains a list of PreferenceSection objects, and allows clients to declare
				new sections, and add new preferences to the sections.
				
				For details of how to declare/use preferences, see the Application class.
				
				
				When the application wants to save the preferences, the Write() function
				of this class should be used.
				
	SeeAlso:	-

********************************************************************************************/


class CCAPI Preferences : public List
{
	CC_DECLARE_MEMDUMP(Preferences)
	
public:
	Preferences();
	~Preferences();
	
	BOOL Init();
	void Deinit();
	
	void Write();

	void WipePreferenceFile();
	void WipeDangerousPrefs();
	
	BOOL DeclareSection(TCHAR *Section, UINT32 InitialSize);
	
	BOOL DeclarePref(TCHAR *Section, TCHAR *Pref, 
					 INT32 *PrefVar, INT32 Min = INT_MIN, INT32 Max = INT_MAX);
					 
	BOOL DeclarePref(TCHAR *Section, TCHAR *Pref, 
					 UINT32 *PrefVar, UINT32 Min = 0, UINT32 Max = UINT_MAX);
					 
	BOOL DeclarePref(TCHAR *Section, TCHAR *Pref, 
					 double *PrefVar, double Min = DBL_MIN, double Max = DBL_MAX);
	
	BOOL DeclarePref(TCHAR *Section, TCHAR *Pref, StringBase *PrefVar);

	// These next functions allow the preferences to be read/written by other
	// people than the owner e.g. the application options dialog box

	BOOL GetPrefValue(TCHAR *Section, TCHAR *Pref, INT32 *PrefVar);
	BOOL GetPrefValue(TCHAR *Section, TCHAR *Pref, UINT32 *PrefVar);
	BOOL GetPrefValue(TCHAR *Section, TCHAR *Pref, double *PrefVar);

	BOOL SetPrefValue(TCHAR *Section, TCHAR *Pref, INT32 *PrefVar);
	BOOL SetPrefValue(TCHAR *Section, TCHAR *Pref, UINT32 *PrefVar);
	BOOL SetPrefValue(TCHAR *Section, TCHAR *Pref, double *PrefVar);

	BOOL SetPrefDirect(TCHAR *Section, TCHAR *Pref, const char *pValue, BOOL Force = FALSE);
	BOOL SetPrefDirect(TCHAR *Section, TCHAR *Pref, PreferenceType Type, PrefData Data);
	BOOL SetPrefDirect(TCHAR *Section, TCHAR *Pref, INT32 *pValue);
	BOOL SetPrefDirect(TCHAR *Section, TCHAR *Pref, UINT32 *pValue);
	BOOL SetPrefDirect(TCHAR *Section, TCHAR *Pref, double *pValue);

	BOOL GetPrefDirect(TCHAR *Section, TCHAR *Pref, StringBase *pValue);
	BOOL GetPrefDirect(TCHAR *Section, TCHAR *Pref, PreferenceType Type, PrefData Data);
	BOOL GetPrefDirect(TCHAR *Section, TCHAR *Pref, INT32 *pValue);
	BOOL GetPrefDirect(TCHAR *Section, TCHAR *Pref, UINT32 *pValue);
	BOOL GetPrefDirect(TCHAR *Section, TCHAR *Pref, double *pValue);

private:
	// Used to cache the section object when declaring preferences.
	PreferenceSection *CurrentSection;
	PreferenceSection *GetSection(TCHAR *SectionName);
	
	// Link to the OIL layer
	OILPreferences *OILPrefs;

};

#endif
