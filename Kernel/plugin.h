// $Id$
// This files defines various items required by the plug in manager
// At present, this includes:-
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

// PlugInItem
// This defines the basic requirements to store details on a plug-in

// PlugInPath
// This defines a stored path which is defined by the user and then
// parsed for plug-ins.

// PlugInPathList
// This stores the list of PlugInPath items.

#ifndef INC_PLUGINITEM
#define INC_PLUGINITEM

//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class KernelBitmap;
class Document;

// Define the current types of plug-in that we know about
typedef enum TypeOfPlugIn
{
	PLUGIN_PHOTOSHOP = 0,	// PhotoShop bitmap effect plug-ins
	PLUGIN_XTRA,			// FreeHand Xtra plug-ins
	PLUGIN_BFX,				// Built in bitmap effect plug-ins
	
	PLUGIN_UNKNOWN
}; 

/**************************************************************************************

>	class PlugInItem : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/96
	Purpose:	Provides the minimum requirement to store details on a plug-in. 

**************************************************************************************/

class PlugInItem : public ListItem
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNAMIC(PlugInItem);

public:
	// Construction and destruction
	PlugInItem();

	// return a string describing a unique identifier for the plug-ins this handler is interested in
	virtual String_32 GetUniqueID() const	= 0; 
	// return a unique type for this kind of plug-in
	virtual TypeOfPlugIn GetTypeOfPlugIn() const = 0;

public:
	// Get useful details on a plug-in
	INT32	GetPathNameID() { return m_PathNameID; } const
	String_32& GetModuleName() { return m_ModuleName; } const
	String_32& GetFilterName() { return m_FilterName; } const
	String_32& GetFamilyName() { return m_FamilyName; } const
	String_32& GetPlugInName() { return m_PlugInName; } const
//	String_32& GetOpToken() { return m_OpToken; } const

	// Set useful details on a plug-in, should only be done by the plug-in handler
	void SetPathNameID( const INT32 NewID ) { m_PathNameID = NewID; }
	void SetModuleName( const String_32& NewName ) { m_ModuleName = NewName; }
	void SetFilterName( const String_32& NewName ) { m_FilterName = NewName; }
	void SetFamilyName( const String_32& NewName ) { m_FamilyName = NewName; }
	void SetPlugInName( const String_32& NewName ) { m_PlugInName = NewName; }
//	void SetOpToken( const String_32& NewName ) { m_OpToken = NewName; }

protected:
	INT32 m_PathNameID;				// ID of the pathname that we are associated with
	String_32 m_ModuleName;			// Filename of module
	String_32 m_FilterName;			// Filter name
	String_32 m_FamilyName;			// Family or Main menu name
	String_32 m_PlugInName;			// Plug-in name or Sub menu name

//	String_32 m_OpToken;			// Plug-in name or Sub menu name
	
public:
	// Operations available
	// Find out about a plug-in
	virtual BOOL About();

	virtual BOOL Apply(KernelBitmap *pInputBitmap, Document* pDocument,
					   KernelBitmap **ppOutputBitmap = NULL);
	virtual BOOL ApplyLastFilter(KernelBitmap *pInputBitmap, Document* pDocument,
								 KernelBitmap **ppOutputBitmap = NULL);

	// Checking if a bitmap's colour depth is suitable
	virtual BOOL IsBitmapModeOk(INT32 ColourDepth, BOOL GreyScale);
};

/**************************************************************************************

>	class PlugInPath : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Purpose:	Represents a pathname object that the user has requested that all plug-ins
				be loaded in from.
	SeeAlso:	PlugInPathList;

**************************************************************************************/

class PlugInPath : public ListItem
{
	CC_DECLARE_DYNAMIC(PlugInPath)

public:
	// Adds a newly saved pathname to the list
	PlugInPath( const PathName& NewPath, INT32 ID, BOOL IsHidden = FALSE);

	PathName GetPathName() const { return m_Path; }
	String_256 GetPathNameAsString() const { return m_Path.GetPath(); }
	INT32 GetPathNameID() { return m_PathNameID; }
	BOOL IsHidden() { return m_IsHidden; }

protected:
	// Pointer to the pathname to use.
	PathName m_Path;
	INT32 m_PathNameID;
	BOOL m_IsHidden;
};

/**************************************************************************************

>	class PlugInPathList : public List

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Purpose:	Holds a list of Pathnames that the PlugInManager will use to search for
				plug-ins. The list is set up by the user in the plug-ins options tab.
	SeeAlso:	PlugInPath; 

**************************************************************************************/

class PlugInPathList : public List
{
	CC_DECLARE_DYNAMIC(PlugInPathList)

public:
	PlugInPathList();
	~PlugInPathList();

	// When searching the pathname list, you MUST use the following
	// as otherwise you will get unknown effects
	PlugInPath * GetFirstPath();
	PlugInPath * GetNextPath( const PlugInPath * pCurrentItem);

	PlugInPath *AddPathName( const PathName& NewPath, BOOL IsHidden = FALSE);	
	BOOL DeletePathName( const PathName& OldPath );

	BOOL RemoveHiddenItems();

protected:
	INT32 m_IDCounter;
};

#endif // INC_PLUGINITEM
