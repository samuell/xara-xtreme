// $Id$
//
// File which implements plug-in related items. At present this includes
// implementations of:-
//		PlugInItem
//		PlugInPath
//		PlugInPathList
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

#include "camtypes.h"

#include "plugin.h"

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNAMIC(PlugInItem, ListItem)
CC_IMPLEMENT_DYNAMIC(PlugInPath, ListItem)
CC_IMPLEMENT_DYNAMIC(PlugInPathList, List)

// We want better memory tracking
#define new CAM_DEBUG_NEW

/**************************************************************************************

>	PlugInItem::PlugInItem()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/96
	Purpose:	Constructor for a PlugInItem. 

**************************************************************************************/

PlugInItem::PlugInItem()
{
	// Set up the class variables to known states
	m_PathNameID = -1;
	m_ModuleName.Empty();
	m_FilterName.Empty();
	m_FamilyName.Empty();
	m_PlugInName.Empty();
}

/**************************************************************************************

>	virtual BOOL PlugInItem::About()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Returns:	True if works ok, False otherwise.
	Purpose:	Find out about a plug-in. 

**************************************************************************************/

BOOL PlugInItem::About()
{
	ERROR3("PlugInItem::About baseclass invoked!");
	return TRUE;
}

/**************************************************************************************

>	virtual BOOL PlugInItem::Apply(KernelBitmap *pInputBitmap, Document* pDocument,
								   KernelBitmap **ppOutputBitmap = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96
	Inputs:		pInputBitmap	the bitmap to apply the effect to
				pDocument		document the bitmap is in
	Outputs:	ppOutputBitmap	if specified, a pointer to the output bitmap generated
	Returns:	True if works ok, False otherwise.
	Purpose:	Applies a plug-in's bitmap effect to the specified bitmap. 

**************************************************************************************/

BOOL PlugInItem::Apply(KernelBitmap *pInputBitmap, Document* pDocument,
					   KernelBitmap **ppOutputBitmap)
{
	ERROR3("PlugInItem::Apply baseclass invoked!");
	return TRUE;
}

/**************************************************************************************

>	virtual BOOL PlugInItem::ApplyLastFilter(KernelBitmap *pInputBitmap, Document* pDocument,
											 KernelBitmap **ppOutputBitmap = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96
	Inputs:		pInputBitmap	the bitmap to apply the effect to
				pDocument		document the bitmap is in
	Outputs:	ppOutputBitmap	if specified, a pointer to the output bitmap generated
	Returns:	True if works ok, False otherwise.
	Purpose:	Applies a plug-in's bitmap effect to the specified bitmap using the last
				applied parameters. Assumes that Apply for this plug-in has been called
				already. 

**************************************************************************************/

BOOL PlugInItem::ApplyLastFilter(KernelBitmap *pInputBitmap, Document* pDocument,
								 KernelBitmap **ppOutputBitmap)
{
	ERROR3("PlugInItem::ApplyLastFilter baseclass invoked!");
	return TRUE;
}

/**************************************************************************************

>	virtual BOOL PlugInItem::IsBitmapModeOk(INT32 ColourDepth)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/97
	Inputs:		ColourDepth		the colour depth or bits per pixel of the bitmap
				GreyScale		only relevent if bitmap is 8bpp, True if greyscale
	Returns:	True if the bitmap is compatible and False otherwise.
	Purpose:	Checks to see if a plug-in can cope with the specified colour depth.

**************************************************************************************/

BOOL PlugInItem::IsBitmapModeOk(INT32 ColourDepth, BOOL GreyScale)
{
	return FALSE;
}

/**************************************************************************************
**************************************************************************************/

/**************************************************************************************

>	PlugInPath::PlugInPath( const PathName& NewPath, INT32 ID, BOOL IsHidden = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Inputs:		Path			- the pathname to put in the list.
				ID				- the ID a plug-in item uses to associate itself with a pathname
				IsHidden		- the pathname is not one added by the user (Defaults to FALSE)
	Purpose:	Create a new plug-in pathname list item from the given pathname.
	SeeAlso:	PlugInPathList;

**************************************************************************************/

PlugInPath::PlugInPath( const PathName& NewPath, INT32 ID, BOOL IsHidden)
{
	// Initialise our class variables to these specified values.
	m_Path = NewPath;
	m_PathNameID = ID;
	m_IsHidden = IsHidden;
}

/**************************************************************************************
**************************************************************************************/

/**************************************************************************************

>	PlugInPathList::~PlugInPathList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the list of pathnames to parse plug-ins for.
	SeeAlso:	PlugInPath

**************************************************************************************/

PlugInPathList::PlugInPathList()
{
	m_IDCounter = 0;
}

/**************************************************************************************

>	PlugInPathList::~PlugInPathList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroy the list of pathnames to parse plug-ins for.
	SeeAlso:	PlugInPath

**************************************************************************************/

PlugInPathList::~PlugInPathList()
{
	// Destroy the list of pathnames which we may have
	/* PlugInPath* pPath = (PlugInPath *)GetHead();
	while (pPath)
	{
		delete pPath;

		// Try the next pathname in the list
		pPath = (PlugInPath *)GetNext(pPath);
	} */
	
	DeleteAll();
}

/**************************************************************************************

>	PlugInPath * PlugInPathList::GetFirstPath()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/97
	Returns:	Pointer to the first PlugInPath or NULL if there are none.
	Purpose:	Finds the first pathname in the list of plug-in pathnames.
				When searching the pathname list, you MUST use these form of the list
				classes GetHead/GetNext	as otherwise you will get unknown effects.
	SeeAlso:	GetNextPath

**************************************************************************************/

PlugInPath * PlugInPathList::GetFirstPath()
{
	// Get the real first item, if there is one
	PlugInPath * pPath = (PlugInPath * ) GetHead();
	// While there is an item and it is hidden then search for the next one in the list
	while (pPath && pPath->IsHidden())
	{
		pPath = (PlugInPath * ) GetNext(pPath);
	}

	return pPath;
}

/**************************************************************************************

>	PlugInPath * PlugInPathList::GetNextPath(const PlugInPath * pCurrentItem)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/97
	Returns:	Pointer to the first PlugInPath or NULL if there are none.
	Purpose:	Finds the next pathname in the list of plug-in pathnames.
				When searching the pathname list, you MUST use these form of the list
				classes GetHead/GetNext	as otherwise you will get unknown effects.
	SeeAlso:	GetNextPath

**************************************************************************************/

PlugInPath * PlugInPathList::GetNextPath(const PlugInPath * pCurrentItem)
{
	ERROR3IF(pCurrentItem == NULL,"GetNextPath has been given a null current item");
	if (pCurrentItem == NULL)
		return NULL;

	// Get the real next item, if there is one
	PlugInPath * pPath = (PlugInPath * ) GetNext(pCurrentItem);
	// While there is an item and it is hidden then search for the next one in the list
	while (pPath && pPath->IsHidden())
	{
		pPath = (PlugInPath * ) GetNext(pPath);
	}

	return pPath;
}

/**************************************************************************************

>	BOOL PlugInPathList::RemoveHiddenItems()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/97
	Returns:	True if completed succesfully, False otherwise.
	Purpose:	Removes all hidden items, i.e. ones not added by the user, from the list
				of pathnames.

**************************************************************************************/

BOOL PlugInPathList::RemoveHiddenItems()
{
	// Go through all the paths in the list and remove all those with the hidden
	// attribute set.
	PlugInPath* pPath = (PlugInPath *)GetHead();
	while (pPath)
	{
		if (pPath->IsHidden())
		{
			// The item is hidden so remove it
			// First, find the next item, if there is one
			PlugInPath* pNextPath = (PlugInPath *)GetNext(pPath);

			RemoveItem(pPath);
			// remove item returns NULL if problem
			if (pPath == NULL)
				return FALSE;
			
			// remove the old path item from memory
			delete pPath;
			// move to considering the next item
			pPath = pNextPath;
		}
		else
		{
			// Try the next pathname in the list
			pPath = (PlugInPath *)GetNext(pPath);
		}
	}

	return TRUE;
}

/**************************************************************************************

>	PlugInPath * PlugInPathList::AddPathName( const PathName& NewPath, BOOL IsHidden = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Inputs:		NewPath			- the new pathname to put in the list.
				IsHidden		- the pathname is not one added by the user (Defaults to FALSE)
	Returns:	Pointer to the newly added pathname, or 
				NULL if out of memory => ERROR1
	Purpose:	Add a new pathname to the list of plug-in pathnames.
				Hidden pathnames are ones not added by the user but are the sub-directories
				of the user added ones.
	Errors:		Out of memory => ERROR1
	SeeAlso:	PlugInPath

**************************************************************************************/

PlugInPath * PlugInPathList::AddPathName( const PathName& NewPath, BOOL IsHidden )
{
	PlugInPath *pPlugPath = new PlugInPath(NewPath, m_IDCounter, IsHidden);

	// Increment our counter which a plug-in uses to associate itself with a pathname
	m_IDCounter++;

	// Check for out of memory
	if (pPlugPath == NULL)
		return NULL;

	// Add to the list and return success
	AddTail(pPlugPath);
	return pPlugPath;
}

/**************************************************************************************

>	BOOL PlugInPathList::DeletePathName( const PathName& OldPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/96
	Inputs:		OldPath			- the pathname to delete from the list.
	Returns:	True if deleted ok, False otherwise.
	Purpose:	Delete a pathname from the list of plug-in pathnames.
	SeeAlso:	PlugInPath

**************************************************************************************/

BOOL PlugInPathList::DeletePathName( const PathName& OldPath )
{
	String_256 OldPathStr = OldPath.GetPath();
	PlugInPath* pPath = (PlugInPath *)GetHead();
	while (pPath)
	{
		PathName Path = pPath->GetPathName();
		if (Path.GetPath() == OldPathStr)
		{
			RemoveItem(pPath);
			// remove item returns NULL if problem
			if (pPath == NULL)
				return FALSE;
			delete pPath;
			return TRUE;
		}

		// Try the next pathname in the list
		pPath = (PlugInPath *)GetNext(pPath);
	}

	// Failed to find the path in the list
	return FALSE;
}

/**************************************************************************************
**************************************************************************************/

