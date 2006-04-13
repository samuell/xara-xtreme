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

// file defining ASCII strings of the product name so it can easily be changed

#ifndef INC_PRODUCT
#define	INC_PRODUCT

/******************************************
 Notes on PRODUCT_NAMEx macros (markn 27/11/96)

  The numbered product macros are for use in resource files.  The resource compiler doesn't have the same
  ability to concatinate strings as the C preprocessor, so product name string macros & related substitution
  can't be done in the usual way.

  The above macros behave in the following way:
	PRODUCT_NAME1(Superb)									produces "Superb Xara Webster"
	PRODUCT_NAME2(\040is superb)							produces "Xara Webster is superb"
	PRODUCT_NAME3(Stunning,\040is great)					produces "Stunning Xara Webster is great"
	PRODUCT_NAME4(Stunning,\040is great\054 and,\040shines)	produces "Stunning Xara Webster is great, and Xara Webster shines"

  Notes:
  * The parameter strings should not include double quote (") chars.  This are produced by the macro.
  * \040 is octal for Space ( ).  As you are not providing a string param, this is the only way to include
    leading or trailing spaces.
  * \054 is octal for a comma (,).  Literal commas are interpreted as param separators, so you have to 
    insert these using the ASCII code.
  * \047 is octal for a quote mark/apostrophe (').  Needed for words like "Don't" and "product's" but not "hatstands".  Have to be done via
    ASCI codes because single (') chars cause syntax errors.
  * \050 & \051 define round brackets - these will be need too, if you want brackets in you strings.

**********************************************/

//---------------------------------------------------------------------------------
// These are common to all builds

// use this one as the manufacturer name
// WEBSTER - markn 7/2/97

// use this one for showing copyright information
#define	PRODUCT_COPYRIGHTNAME "Xara Group Ltd."

// use this one for looking at the gallery index files
// for the Xara Studio V1.00a release
#define	PRODUCT_OLDINDEXNAME	"XaraInfo"

// use this one for looking at the gallery index files
// for versions generated by this version
#define	PRODUCT_INDEXNAME	"XaraInfo"

// this is the version number that is displayed to the user
// in the splash screen and on the about box, and that is
// used to determine if a new version is available by 
// building up the update URL
// It should probably have "CD" after it for a CD build

#ifdef CD_BUILD
	#define PRODUCT_RETAIL_VERSION_NO	"1.2 CD"
#else
	#define PRODUCT_RETAIL_VERSION_NO	"1.2 unknown"
#endif




//---------------------------------------------------------------------------------

// define the bits that are peculiar to the programs e.g. Studio, Viewer
#ifndef STANDALONE

	#define	PRODUCT_MANUFACTURER		_T("Xara")
	#define	PRODUCT_COMPANYNAME			_T("Xara Ltd.")

	// use this one as the base for filenames e.g. ini
	#define	PRODUCT_BASENAME			_T("XaraX")

	#define	PRODUCT_HELPFILENAME		_T("XaraX")			// e.g. "CorelXra.hlp" and "CorelXra.cnt"
	#define	PRODUCT_MOVIESHELPFILENAME	_T("Movies")			// e.g. "CorelXra.hlp" and "CorelXra.cnt"

	#define	PRODUCT_EXENAME				_T("XaraX.exe")

	// Use these for the specific ini files mentioned
	#define PRODUCT_BARSFULL_INIFILE	_T("XaraXBF.ini")
	#define PRODUCT_BARSNORMAL_INIFILE	_T("XaraXBN.ini")
	#define PRODUCT_GALLERY_INIFILE		_T("XaraXGR.ini")
	#define PRODUCT_RESOURCES_INIFILE	_T("XaraWRS.ini")
	#define PRODUCT_PLUGINS_INIFILE		_T("XaraXPL.ini")

	// The resource DLL leaf name (the last 3 chars define the locale)
	// Note: You'll also need to change the RESOURCELEAF var in the make file too
	#define PRODUCT_RESOURCELEAF		_T("XaraX")

	// use this one to display the name of the product to the user
	// WEBSTER - markn 26/11/96
	// (See notes at the top of this file about the PRODUCT_NAME macros)
	#define	PRODUCT_NAME				_T("Xara X")
	#define STRINGIZE(x)				#x
	#define	PRODUCT_NAME1(s)			STRINGIZE(s Xara X)
	#define	PRODUCT_NAME2(s)			STRINGIZE(Xara X##s)
	#define	PRODUCT_NAME3(s1,s2)		STRINGIZE(s1 Xara X##s2)
	#define	PRODUCT_NAME4(s1,s2,s3)		STRINGIZE(s1 Xara X##s2 Xara X##s3)
	#define	PRODUCT_NAME5(s1,s2)		STRINGIZE(Xara X##s1 Xara X##s2)

	#define	PRODUCT_WEBLINKNAME			_T("XaraLink")
	#define	PRODUCT_WEBLINKEXENAME		_T("XaraLink.exe")

	// use this one as the product name
	#define	PRODUCT_PRODUCTNAME			_T("XaraX2")

	// use this one as the running name
	#define	PRODUCT_RUNNINGNAME			_T("XaraX")

	// use this as the product version as shown to the user
	#define PRODUCT_VERSION_NO			_T("3.0")

	#define PRODUCT_INTERNALNAME		_T("Camelot")

	#define PRODUCT_TRADEMARKS			_T("Xara and Xara X are registered trademarks of Xara Ltd")

#else	// STANDALONE

	// registry keys.
	#define	PRODUCT_MANUFACTURER			"Xara"	

	// use this one as the base for filenames e.g. ini, etc
	#define	PRODUCT_BASENAME				"XXVIEWER"

	#define	PRODUCT_HELPFILENAME			"Xara"	// e.g. "Xara.hlp" and "Xara.cnt"

	#define	PRODUCT_EXENAME					"XXViewer.exe"	

	// Use these for the specific ini files mentioned
	#define PRODUCT_BARSFULL_INIFILE		"XaraXVBF.ini"
	#define PRODUCT_BARSNORMAL_INIFILE		"XaraXVBN.ini"
	#define PRODUCT_GALLERY_INIFILE			"XaraX.ini"
	#define PRODUCT_RESOURCE_INIFILE		"XaraXVRS.ini"

	// use this one to display the name of the product to the user
	#define	PRODUCT_NAME					"Xara X Viewer"

	// use this one as the product name
	#define	PRODUCT_PRODUCTNAME				"Viewer"

	// use this one as the running name
	#define	PRODUCT_RUNNINGNAME				"XaraXViewer"

	// use this as the product version as shown to the user
	#define PRODUCT_VERSION_NO				"1.5"

#endif

// General modifications to the above macros

// This defines a different program (product) name for debug and retail builds.
#ifdef _DEBUG
	#define PROGRAM_NAME					PRODUCT_PRODUCTNAME " (Debug)"
	#define PRODUCT_APP_DOC_ID				PRODUCT_PRODUCTNAME ".Document.Debug"
#else
	#define PROGRAM_NAME					PRODUCT_PRODUCTNAME
	#define PRODUCT_APP_DOC_ID				PRODUCT_PRODUCTNAME ".Document"
#endif


// define the registry keys that each section of the old ini file is saved under
#define PRODUCT_REGISTRYKEY_OPTIONS			"Options"
#define PRODUCT_REGISTRYKEY_BARSETTINGS		"BarSettings"
#define PRODUCT_REGISTRYKEY_BARSFULL		"BarsFull"
#define PRODUCT_REGISTRYKEY_BARSNORMAL		"BarsNormal"
#define PRODUCT_REGISTRYKEY_GALLERY			"Gallery"
#define PRODUCT_REGISTRYKEY_PLUGINS			"PlugIns"
#define PRODUCT_REGISTRYKEY_RESOURCE		"Resources"


// The path to the product's registry settings

// define the current user key first (so it can be passed to the WebFileUpdater Component)

#define PRODUCT_REGISTRYKEY_USERPATH		"Software\\" PRODUCT_MANUFACTURER "\\" PROGRAM_NAME "\\" 
#define PRODUCT_REGISTRYKEY_FULLPATH		"HKEY_CURRENT_USER\\" PRODUCT_REGISTRYKEY_USERPATH

// The full registry path to various sections
#define PRODUCT_REGISTRYKEY_OPTIONS_FULLPATH		PRODUCT_REGISTRYKEY_FULLPATH PRODUCT_REGISTRYKEY_OPTIONS
#define PRODUCT_REGISTRYKEY_BARSNORMAL_FULLPATH		PRODUCT_REGISTRYKEY_FULLPATH PRODUCT_REGISTRYKEY_BARSNORMAL

#endif // INC_PRODUCT
