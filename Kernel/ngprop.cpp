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
/*
	$Header: /wxCamelot/Kernel/ngprop.cpp 21    20/07/05 15:39 Luke $
	Attribute gallery named set properties.
*/

#include "camtypes.h"

//#include "ngcore.h"
#include "ngitem.h"
#include "ngprop.h"
//#include "ngscan.h"
#include "ngsentry.h"
#include "ngsetop.h"

#include "extender.h"
//#include "extendsetsdlg.h"

//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "mkshapes.h"
#include "impexpop.h"
#include "bitfilt.h"
#include "bmpprefs.h"
#include "bmapprev.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "extfilts.h"
#include "exjpeg.h"
//#include "filedlgs.h"
#include "fileutil.h"

#include <algorithm>

//#include "extendres.h"
//#include "resource.h"
//#include "galres.h"
//#include "mario.h"
//#include "nev.h"
//#include "tim.h"
//#include "justin3.h"

//#include "fixmem.h"		// For CCFree() - in camtypes.h [AUTOMATICALLY REMOVED]

// Matt - 13/02/2001 - For TemplateAttribute and Controller Nodes...
//#include "galstr.h"
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "userattr.h"
#include "tmpltatr.h"
#include "nodershp.h"
#include "nodeshad.h"
#include "nbevcont.h"
#include "ncntrcnt.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(SGNameProp, CC_CLASS_MEMDUMP);

// This line must come after any CC_IMPLEMENT_... definitions.
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// The export dialogs should check this and use an Apply button rather than an
// Export button if it is true.
BOOL NamedExportProp::m_fApplyNotExport = FALSE;


/***********************************************************************************************
>   SGNameProp::SGNameProp(const StringBase& strName, INT32 nIndex)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		strName		---		the name of the set associated with this property
				nIndex		---		the index (type) of the property
	Purpose:    Constructs an SGNameProp object.
***********************************************************************************************/

SGNameProp::SGNameProp(const StringBase& strName, INT32 nIndex)
  :	m_nIndex(nIndex),
	m_fIsVirgin(TRUE)
{
	ERROR3IF(nIndex < 0 || nIndex >= nPropertyCount,
					"SGNameProp::SGNameProp: index out of range");
	m_strName.Alloc(strName.Length() + 1);
	m_strName = strName;
}



/***********************************************************************************************
>   virtual SGNameProp::~SGNameProp()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:    Destroys an SGNameProp object.
***********************************************************************************************/

SGNameProp::~SGNameProp()
{
	// Empty.
}



/***********************************************************************************************
>   SGNameProp::SGNameProp(const SGNameProp& other)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:    Copy constructor.
***********************************************************************************************/

SGNameProp::SGNameProp(const SGNameProp& other)
  :	m_nIndex(other.m_nIndex),
	m_fIsVirgin(other.m_fIsVirgin)
{
	m_strName.Alloc(other.m_strName.Length() + 1);
	m_strName = other.m_strName;
}



/***********************************************************************************************
>   static SGNameProp* SGNameProp::CreateDefault(const StringBase& strName, INT32 nIndex)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		strName		---		the name of the set to create a default SGNameProp for
				nIndex		---		the type of property to create
	Returns:    A default property of the given index (type) created in the heap.
***********************************************************************************************/

SGNameProp* SGNameProp::CreateDefault(const StringBase& strName, INT32 nIndex)
{
	SGNameProp* pProp;
	switch (nIndex)
	{
	case NamedExportProp::nIndex:
		pProp = new NamedExportProp(strName);
		break;

	case NamedSliceProp::nIndex:
		pProp = new NamedSliceProp(strName, TRUE);		// by default is a slice
		break;

	case NamedStretchProp::nIndex:
		pProp = new NamedStretchProp(strName);
		break;

	default:
		ERROR3("SGNameProp::CreateDefault: index out of range");
		pProp = 0;
		break;
	}

	ERRORIF(pProp == 0, _R(IDE_NOMORE_MEMORY), 0);
	return pProp;
}



/***********************************************************************************************
>   virtual BOOL SGNameProp::Change(SGNameItem* pItem, SGNameProp* pNewProp,
									StringBase* pstrNewName = 0)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		pItem			---		the item whose property is to be changed
				pNewProp		---		the new property to set for it
				pstrNewName		---		if specified, the new name of the item.
	Returns:	FALSE if there's an error.
	Purpose:	Changes the property of a set item in an undoable way, and optionally
				renames the set as well.
	SeeAlso:	OpChangeSetProperty::DoWithParam
***********************************************************************************************/

BOOL SGNameProp::Change(SGNameItem* pItem, SGNameProp* pNewProp, StringBase* pstrNewName)
{
	String_256 strName;
	pItem->GetNameText(&strName);

	StringBase* apstr[2];
	apstr[0] = &strName;
	apstr[1] = pstrNewName;
	
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor( OPTOKEN_CHANGE_SET_PROPERTY );
	ERROR3IF(pDesc == 0, "SGNameProp::Change: can't find OpDescriptor");
	OpParam				param( apstr, pNewProp );
	pDesc->Invoke( &param );
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL BOOL SGNameProp::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to write to
	Returns:	TRUE if the object has successfully written out its record.
	Purpose:	Writes out an SGNameProp record.  Derived classes should call this
				before writing their specific data.
********************************************************************************************/

BOOL SGNameProp::Write(CXaraFileRecord* pRec)
{
	// Write out the virgin state.
	return pRec->WriteBYTE((BYTE) m_fIsVirgin);
}



/********************************************************************************************
>	virtual BOOL BOOL SGNameProp::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to read from
	Returns:	TRUE if the object has successfully read in its record.
	Purpose:	Reads in an SGNameProp record.  Derived classes should call this
				before reading their specific data.
********************************************************************************************/

BOOL SGNameProp::Read(CXaraFileRecord* pRec)
{
	// Read in the virgin state.
	BYTE n;
	if (!pRec->ReadBYTE(&n)) return FALSE;
	m_fIsVirgin = (BOOL) n;
	return TRUE;
}



/***********************************************************************************************
>   NamedTickboxProp::NamedTickboxProp(const StringBase& strName, INT32 nIndex, INT32 nState = 0)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     nState		---		the initial state of the property (default is unticked).
	Purpose:    Constructs a NamedTickboxProp object.
***********************************************************************************************/

NamedTickboxProp::NamedTickboxProp(const StringBase& strName, INT32 nIndex, INT32 nState)
  : SGNameProp(strName, nIndex),
	m_nState(nState)
{
	// Empty.
}



/***********************************************************************************************
>   NamedTickboxProp::NamedTickboxProp(const NamedTickboxProp& other)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:	Copy constructor.
***********************************************************************************************/

NamedTickboxProp::NamedTickboxProp(const NamedTickboxProp& other)
  :	SGNameProp(other.GetName(), other.GetIndex()),
	m_nState(other.m_nState)
{
	// Empty.
}



/***********************************************************************************************
>   virtual void NamedTickboxProp::CalcUiBounds(SGNameItem* pItem,  SGFormatInfo* pFormatInfo,
												SGMiscInfo* pMiscInfo, DocRect* pMaxBounds)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		(see SGNameItem::CalcUiBounds)
				pMaxBounds		---		the default size of the property - the whole label.
										The calculation should increase lo.x to move the left
										edge of the UI rightwards.
	Outputs:	pMaxBounds		---		the bounds of this property's UI within its SGNameItem.
	SeeAlso:	SGNameItem::CalcUiBounds; SGNamedProp
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
void NamedTickboxProp::CalcUiBounds(SGNameItem*, SGFormatInfo*,
									SGMiscInfo* pMiscInfo, DocRect* pMaxBounds)
{
	// Tickboxes are 16 pixels wide
	pMaxBounds->lo.x = pMaxBounds->hi.x - pMiscInfo->PixelSize * 16;
}
#endif

/***********************************************************************************************
>   virtual BOOL NamedTickboxProp::HandleRedraw(SGNameItem* pItem, SGRedrawInfo* pRedrawInfo, 
												SGMiscInfo* pMiscInfo, const DocRect& drBounds)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see SGNameItem::HandleRedraw)
				drBounds	---		the bounds to render within
	Returns:	TRUE if successful.
	Purpose:    Draws the UI for the state of this NamedTickboxProp.
	SeeAlso:	SGNameItem::HandleRedraw; NamedTickboxProp::HandleMouse
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
BOOL NamedTickboxProp::HandleRedraw(SGNameItem*, SGRedrawInfo* pRedrawInfo, SGMiscInfo*,
								    const DocRect& drBounds)
{
	UINT32 idBmp = (m_nState) ? _R(IDB_LGAL_TICKON) : _R(IDB_LGAL_TICKOFF);
	pRedrawInfo->Renderer->DrawBitmap(drBounds.lo, idBmp);
	return TRUE;
}
#endif

/***********************************************************************************************
>   virtual BOOL NamedTickboxProp::HandleMouse(SGNameItem* pItem, SGMouseInfo* pMouseInfo,
											   SGMiscInfo* pMiscInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see SGNameItem::HandleEvent)
	Returns:	TRUE if successful.
	Purpose:    Responds to mouse events according to the state of this NamedTickboxProp.
	SeeAlso:	SGNameItem::HandleEvent; NamedTickboxProp::HandleRedraw
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
BOOL NamedTickboxProp::HandleMouse(SGNameItem* pItem, SGMouseInfo*, SGMiscInfo*)
{
/*
 *	Commented out by Karim MacDonald 14/12/1999.
 *	This code performs a check for quickshapes in the named sets being
 *	ticked off, and asks the user whether to convert them to editable shapes.
 *	A test like this must ideally be made whenever a new quickshape is added to
 *	a stretching (target) named set, but not once the user has decided not to change that
 *	quickshape.
 *

	BOOL bCancelled = FALSE;
	BOOL bMakeShapes = FALSE;
	std::list<Node*> lpNodes;
	if (!m_nState)
	{
		// if the user is ticking this set, then we must check whether any new nodes
		// have been added to the set since we last were called. if they have, then
		// we have to test whether each new node's type is NodeRegularShape or NodeBitmap.
		// if the test is positive then we ask the user whether they want to convert these
		// new nodes into editable shapes.

		// NOTE: currently, we test the types of *all* nodes in the set.
		FindBadExtendTypesScan findScan(&lpNodes, pItem);
		findScan.Scan();
		if (!lpNodes.empty())
		{
			INT32 nResult = InformMessage(_R(IDS_QMS_QUERYMAKESHAPES),
										NULL, _R(IDS_QMS_MAKESHAPES), _R(IDS_CANCEL));

			// make editable shapes
			switch (nResult)
			{
			case 1:		// first button pressed - 'OK'.
				// continue to handle the click.
				break;

			case 2:		// 2nd button pressed	- 'Convert'.
				// make a note to convert the objects to editable shapes.
				bMakeShapes = TRUE;
				break;

			case 3:		// 3rd button pressed	- 'Cancel'.
				// make a note note to proceed any further.
				bCancelled = TRUE;
				break;

			default:
				// we shouldn't get here...
				ERROR3("NamedTickboxProp::HandleMouse- Inform message box returned bad result");
				break;
			}
		}
	}

	// if the user chose to cancel this operation, we get out here.
	if (bCancelled) return TRUE;
*/

	// Clone this object and set the new state in the clone.
	NamedTickboxProp* pClone = (NamedTickboxProp*) Clone();
	ERRORIF(pClone == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	pClone->m_nState = !m_nState;

	// Try to run an op to change the property in the tree undoably.
	if (!Change(pItem, pClone)) return FALSE;

/*
 *	Commented out by Karim MacDonald 14/12/1999
 *	See the above commented out block for more info.
 *
	// If we were successful, then convert (or don't) the objects into editable shapes.
	if (bMakeShapes)
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_MAKE_NODES_SHAPES);
		ERROR3IF(pOpDesc == 0, "NamedTickboxProp::HandleMouse- can't find OpDescriptor");
		pOpDesc->Invoke(&OpParamMakeNodesShapes(&lpNodes));
	}
*/

	// Redraw the item.
	pItem->ForceRedrawOfMyself();
	return TRUE;
}
#endif



/********************************************************************************************
>	virtual BOOL BOOL NamedTickboxProp::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to write to
	Returns:	TRUE if the object has successfully written out its record.
	Purpose:	Writes out a NamedTickboxProp record.  Derived classes should call this
				before writing their specific data.
********************************************************************************************/

BOOL NamedTickboxProp::Write(CXaraFileRecord* pRec)
{
	// Write out the tick box state.
	return SGNameProp::Write(pRec) && pRec->WriteINT16((short) GetState());
}



/********************************************************************************************
>	virtual BOOL BOOL NamedTickboxProp::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to read from
	Returns:	TRUE if the object has successfully read in its record.
	Purpose:	Reads in a NamedTickboxProp record.  Derived classes should call this
				before reading their specific data.
********************************************************************************************/

BOOL NamedTickboxProp::Read(CXaraFileRecord* pRec)
{
	// Try to read in the base class.
	if (!SGNameProp::Read(pRec)) return FALSE;

	// Read in the tick box state.
	short n;
	if (!pRec->ReadINT16(&n)) return FALSE;
	m_nState = n;
	return TRUE;
}



/***********************************************************************************************
>   NamedExportProp::NamedExportProp(const StringBase& strName)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:    Constructs a NamedExportProp object.
***********************************************************************************************/

NamedExportProp::NamedExportProp(const StringBase& strName)
  : SGNameProp(strName, NamedExportProp::nIndex),
	m_pOptions(0)
{
	// Create a default export path from the menu operation's default and the set name.
	// If there is no implied extension in the set's name then use the default export
	// format (ie. JPEG).
	String_256			strPath;
	if( !OpMenuExport::DefaultExportFilterPath.IsEmpty() )
		strPath = OpMenuExport::DefaultExportFilterPath;
	else
{
PORTNOTE("other","Removed use of GetCurrentDirectory")
#ifndef EXCLUDE_FROM_XARALX
		FileUtil::GetCurrentDirectory( &strPath );
#else
		strPath = _T("Bodginess abounds.");
#endif
}

	// Construct a path and set the default extension (export type) if necessary.
	strPath += TEXT("\\");
	strPath += strName;
	PathName pth;
	pth.SetPathName(strPath);
	if (pth.GetType().IsEmpty()) pth.SetType(TEXT("gif"));
	ERROR3IF(!pth.IsValid(), "NamedExportProp::NamedExportProp: invalid path");

	// Try to work out the filter from it the set name's implied extension.
	for (m_pFilter = Filter::GetFirst();
		 m_pFilter != 0;
		 m_pFilter = Filter::GetNext(m_pFilter))
			if (m_pFilter->GetFlags().CanExport &&
				m_pFilter->pOILFilter &&
				m_pFilter->pOILFilter->DoesExtensionOfPathNameMatch(&pth))
					break;

	// If we found a filter and made a valid path then the object is essentially
	// well-formed.
	if (m_pFilter == 0) return;
	m_Path = pth;

	// TODO: work out default export options (presently only bitmap exports are
	// supported.)
	if (m_pFilter->IS_KIND_OF(BaseBitmapFilter))
	{
		// Make some default filter options.
		m_pOptions = ((BaseBitmapFilter*) m_pFilter)->CreateExportOptions();
		if (m_pOptions == 0)
		{
			ERROR1RAW(_R(IDE_NOMORE_MEMORY));
			return;
		}

		m_pOptions->RetrieveDefaults();
	}
}

/***********************************************************************************************
>   NamedExportProp::NamedExportProp(const NamedExportProp& other)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:    Copy-constructs a NamedExportProp object.
***********************************************************************************************/

NamedExportProp::NamedExportProp(const NamedExportProp& other)
  : SGNameProp(other.GetName(), other.GetIndex()),
	m_pFilter(other.m_pFilter),
	m_pOptions(0),
	m_Path(other.m_Path)
{
	SetOptions(other.m_pOptions);
}



/***********************************************************************************************
>   virtual ~NamedExportProp::~NamedExportProp()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:    Constructs a NamedExportProp object.
***********************************************************************************************/

NamedExportProp::~NamedExportProp()
{
	if (m_pOptions != NULL)
	{
		delete m_pOptions;
		m_pOptions = NULL;
	}
}

/***********************************************************************************************
>   void NamedExportProp::SetFilter(Filter* pFilter)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		pFilter		---		the export filter reference to store
	Purpose:    Replaces stored export filter with the given parameter.
	SeeAlso:	NamedExportProp::GetFilter
***********************************************************************************************/

void NamedExportProp::SetFilter(Filter* pFilter)
{
	m_pFilter = pFilter;
}



/***********************************************************************************************
>   Filter* NamedExportProp::GetFilter() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Returns:	The stored export filter, if any.
	SeeAlso:	NamedExportProp::SetFilter
***********************************************************************************************/

Filter* NamedExportProp::GetFilter() const
{
	return m_pFilter;
}



/***********************************************************************************************
>   void NamedExportProp::SetPath(const PathName& path)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		path	---		the export path to store
	Purpose:    Replaces stored export path with a copy of the given parameter.
	SeeAlso:	NamedExportProp::GetPath
***********************************************************************************************/

void NamedExportProp::SetPath(const PathName& path)
{
	m_Path = path;
}



/***********************************************************************************************
>   PathName& NamedExportProp::GetPath()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Returns:	The stored export path, if any.
	SeeAlso:	NamedExportProp::SetPath
***********************************************************************************************/

PathName& NamedExportProp::GetPath()
{
	return m_Path;
}



/***********************************************************************************************
>   BitmapExportOptions* NamedExportProp::SetOptions(BitmapExportOptions* pOptions)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		pOptions	---		the export options to store
	Returns:	Address of the copied export options that has been stored.
	Purpose:    Replaces stored export options with a copy of the given parameter.
	SeeAlso:	NamedExportProp::GetOptions
***********************************************************************************************/

BitmapExportOptions* NamedExportProp::SetOptions(BitmapExportOptions* pOptions)
{
	if (m_pOptions != NULL)
	{
		delete m_pOptions;
		m_pOptions = NULL;
	}

	if (pOptions != 0) m_pOptions = pOptions->MakeCopy();

	return m_pOptions;
}

/***********************************************************************************************
>   BitmapExportOptions* NamedExportProp::GetOptions() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Returns:	The stored export options, or 0 if there is none.
	SeeAlso:	NamedExportProp::SetOptions
***********************************************************************************************/

BitmapExportOptions* NamedExportProp::GetOptions() const
{
	return m_pOptions;
}



/***********************************************************************************************
>   virtual SGNameProp* NamedExportProp::Clone()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Returns:	A copy of this object in the heap.
***********************************************************************************************/

SGNameProp* NamedExportProp::Clone()
{
	return new NamedExportProp(*this);
}

/********************************************************************************************
>	virtual BOOL BOOL NamedExportProp::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to write to
	Returns:	TRUE if the object has successfully written out its record.
	Purpose:	Writes out a NamedExportProp record.
********************************************************************************************/

BOOL NamedExportProp::Write(CXaraFileRecord* pRec)
{
	// Write out the base class.
	if (!SGNameProp::Write(pRec)) return FALSE;

	// Check for no export settings.
	if (m_pFilter == 0) return pRec->WriteUINT32(FILTERID_NONE);
	ERROR3IF(m_pOptions == 0, "NamedExportProp::Write: no options");
	
	// Write out the filter ID, the path to the target file, the export options
	// class name, and the export options object.
	return pRec->WriteUINT32(m_pFilter->FilterID) &&
		   pRec->WriteUnicode((LPTSTR) (LPCTSTR) m_Path.GetPath()) &&
		   pRec->WriteASCII((LPTSTR) m_pOptions->GetRuntimeClass()->GetClassName() ) &&
		   m_pOptions->Write(pRec);
}

/********************************************************************************************
>	virtual BOOL BOOL NamedExportProp::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to read from
	Returns:	TRUE if the object has successfully read in its record.
	Purpose:	Reads in a NamedExportProp record.
********************************************************************************************/

BOOL NamedExportProp::Read(CXaraFileRecord* pRec)
{
	// Read in the base class.
	if (!SGNameProp::Read(pRec)) return FALSE;

	// Reconstruct the filter reference from the ID.
	FILTER_ID id;
	if (!pRec->ReadUINT32((UINT32*) &id)) return FALSE;
	m_pFilter = Filter::FindFilterFromID(id);
	ERROR3IF(id == FILTERID_NONE && m_pFilter != 0,
				"NamedExportProp::Read: a filter has _R(ID_NONE)?");
	if (m_pFilter == 0) return id == FILTERID_NONE;
	ERROR3IF(!m_pFilter->IS_KIND_OF(BaseBitmapFilter),
				"NamedExportProp::Read: wrong filter type");

	// Read in the path to the target file and the export options class name.
	String_256			strPath;
	TCHAR				szOptions[256];
	if (!pRec->ReadUnicode(&strPath) ||
		!m_Path.SetPathName(strPath, FALSE) ||
		!pRec->ReadASCII(szOptions, sizeof(szOptions) / sizeof(szOptions[0])))
			return FALSE;

	// Reconstruct the export options object.
	CCRuntimeClass* pClass = CCObject::GetRuntimeClassByName(szOptions);

	if ( pClass == NULL )
	{
		return FALSE;
	}

	// Delete the existing bitmap options. This prevents a memory leak on shutdown.
	delete m_pOptions;

	m_pOptions = static_cast<BitmapExportOptions*> ( pClass->CreateObject () );
	return ( m_pOptions != 0 && m_pOptions->Read ( pRec ) );
}

/***********************************************************************************************
>   virtual void NamedExportProp::CalcUiBounds(SGNameItem* pItem, SGFormatInfo* pFormatInfo,
											   SGMiscInfo* pMiscInfo, DocRect* pMaxBounds)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		(see SGNameItem::CalcUiBounds)
				pMaxBounds		---		the maximum size of the property - the whole label.
										The calculation should increase lo.x to move the left
										edge of the UI rightwards.
	Outputs:	pMaxBounds		---		the bounds of this property's UI within its SGNameItem.
	SeeAlso:	SGNameItem::CalcUiBounds; SGNamedProp
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
void NamedExportProp::CalcUiBounds(SGNameItem*, SGFormatInfo*, SGMiscInfo* pMiscInfo,
								   DocRect* pMaxBounds)
{
	// The choose filter & path invoker is 16 pixels wide and right-justified.
	m_drPathInvoker = *pMaxBounds;
	m_drPathInvoker.lo.x = m_drPathInvoker.hi.x - pMiscInfo->PixelSize * 16;

	// The extension display/choose options invoker is three chars wide.
	pMaxBounds->lo.x = m_drPathInvoker.lo.x - pMiscInfo->PixelSize * 24;
}
#endif

/***********************************************************************************************
>   virtual BOOL NamedExportProp::HandleRedraw(SGNameItem* pItem, SGRedrawInfo* pRedrawInfo,
											   SGMiscInfo* pMiscInfo, const DocRect& drBounds)
	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see SGNameItem::HandleRedraw)
				drBounds	---		the bounds to render within
	Returns:	TRUE if successful.
	Purpose:    Draws the UI for the state of this NamedExportProp.
	SeeAlso:	SGNameItem::HandleRedraw; NamedExportProp::HandleMouse
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
BOOL NamedExportProp::HandleRedraw(SGNameItem*, SGRedrawInfo* pRedrawInfo,
								   SGMiscInfo* pMiscInfo, const DocRect& drBounds)
{
	// Draw the extension text within the extension display/choose options invoker.
	if (m_pFilter != 0)
	{
		DocRect drExtBounds(drBounds);
		drExtBounds.lo.x += 4;
		drExtBounds.hi.x = m_drPathInvoker.lo.x - 4;
		
		String_8 strExt2;
		m_Path.GetType().Left(&strExt2, 3);
		strExt2.toUpper();

		pRedrawInfo->Renderer->SetFillColour(pRedrawInfo->Foreground);
		pRedrawInfo->Renderer->DrawRect(&drExtBounds);
		pRedrawInfo->Renderer->SetFixedSystemTextColours(&pRedrawInfo->Background,
														 &pRedrawInfo->Foreground);
		pRedrawInfo->Renderer->DrawFixedSystemText(&strExt2, drExtBounds);
	}

	// Draw the choose filter & path invoker.
	pRedrawInfo->Renderer->DrawBitmap(m_drPathInvoker.lo, _R(IDB_NAMEGAL_EXPORTABLE));
	return TRUE;
}
#endif

/***********************************************************************************************
>   virtual BOOL NamedExportProp::HandleMouse(SGNameItem* pItem, SGMouseInfo* pMouseInfo,
											  SGMiscInfo* pMiscInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see SGNameItem::HandleEvent)
	Returns:	TRUE if successful, FALSE if there is an ERROR to report.
	Purpose:    Responds to mouse events according to the state of this NamedExportProp.
	SeeAlso:	SGNameItem::HandleEvent; NamedExportProp::HandleRedraw
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
BOOL NamedExportProp::HandleMouse(SGNameItem* pItem, SGMouseInfo* pMouseInfo, SGMiscInfo*)
{
	NamedExportProp* pNewProp;
	String_256 strNewName;

	// Work out what was clicked on.
	if (m_drPathInvoker.ContainsCoord(pMouseInfo->Position))
	{
		 return ExportPropertiesToLocationDlg(pItem);
	}
	else
	{
		// A click on the options invoker.  Only invoke if the set knows of a filter
		// than can export it.
		if (m_pFilter == 0 || !m_Path.IsValid()) return TRUE;

		// Make a working copy of this property.
		pNewProp = (NamedExportProp*) Clone();
		ERRORIF(pNewProp == 0, _R(IDE_NOMORE_MEMORY), FALSE);

		// Save the existing selection and exclusively select the associated set.
		OpMenuExport::SaveSelection();
		if (!SelectScan(pItem, SelectScan::SELECT_EXCLUSIVE, TRUE).Scan())
		{
			OpMenuExport::RestoreSelection();
			delete pNewProp;
			return FALSE;
		}

		// Set up the tree and dialog.
		OpMenuExport::NormaliseSelection();
		PathName pthOld = BmapPrevDlg::m_pthExport;
		BmapPrevDlg::m_pthExport = pNewProp->m_Path;
		m_fApplyNotExport = TRUE;

		// flush the caches so that we display/regen the preview images
		pNewProp->m_pOptions->BitmapSourceHasChanged();

		// Get some options from the user.
		BOOL fOk =
			((BaseBitmapFilter*) pNewProp->m_pFilter)->GetExportOptions(pNewProp->m_pOptions);

		// take responsibility for these export options
		// we are storing them so remove the responibility of the dlg for them
		// by removing the reference to them.
		if (BmapPrevDlg::m_pExportOptions && BmapPrevDlg::m_pExportOptions != pNewProp->m_pOptions)
		{
			// get the correct export options
			pNewProp->m_pOptions = BmapPrevDlg::m_pExportOptions;
			// change the filter
			pNewProp->m_pFilter = pNewProp->m_pOptions->FindBitmapFilterForTheseExportOptions();
			// change the path
			pNewProp->m_Path = BmapPrevDlg::m_pthExport;
		}
		// we have taken charge of the export options
		BmapPrevDlg::m_pExportOptions = 0;
		// flush the caches before we store anything
		pNewProp->m_pOptions->BitmapSourceHasChanged();

		// Undo what we set up.
		m_fApplyNotExport = FALSE;
		BmapPrevDlg::m_pthExport = pthOld;
		OpMenuExport::RestoreSelection();

		// Did the user cancel without making any changes?
		if (!fOk)
		{
			delete pNewProp;
			return TRUE;
		}
	}

	// Undoably replace the old export property with the new, optionally renaming
	// it if something was put in strNewName;
	return Change(pItem, pNewProp, strNewName.IsEmpty() ? 0 : &strNewName);
}
#endif

/***********************************************************************************************
>   virtual void NamedExportProp::GetDebugInfo(StringBase* pStr)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see Node::GetDebugDetails)
	Purpose:    Outputs debugging information on this property.
	SeeAlso:	SGNameItem::HandleEvent; NamedExportProp::HandleRedraw
***********************************************************************************************/

#if DEBUG_TREE

void NamedExportProp::GetDebugInfo(StringBase* pStr)
{
	*pStr += TEXT("    Export: ");
	*pStr += m_Path.GetTruncatedPath();
	*pStr += TEXT("\r\n");
}

#endif


/***********************************************************************************************
>   NamedStretchProp::NamedStretchProp(const StringBase& strName, BYTE nStretchType = 0,
									   INT32 nState = 1)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		strName			---		name of set this property is associated with
				nStretchType	---		how this set is stretched by its triggers
				nState			---		the initial state of the tickbox (default is ticked)
	Purpose:    Constructs a NamedStretchProp object.
***********************************************************************************************/

NamedStretchProp::NamedStretchProp(const StringBase& strName, BYTE nStretchType, INT32 nState)
  : NamedTickboxProp(strName, NamedStretchProp::nIndex, nState),
	m_nStretchType(nStretchType)
{
	// Empty.
}



/***********************************************************************************************
>   NamedStretchProp::NamedStretchProp(const NamedStretchProp& other)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:    Copy constructor.
***********************************************************************************************/

NamedStretchProp::NamedStretchProp(const NamedStretchProp& other)
  : NamedTickboxProp(other.GetName(), other.GetIndex(), other.GetState()),
	m_nStretchType(other.m_nStretchType),
	m_rTargetBounds(other.m_rTargetBounds),
	m_rTriggerBounds(other.m_rTriggerBounds)
{
	// Duplicate the targets.
	m_Triggers.insert(m_Triggers.begin(), other.m_Triggers.begin(), other.m_Triggers.end());
}



/***********************************************************************************************
>   void NamedStretchProp::AddTrigger(const StringBase& strSet)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     strSet		---		the name of the target set to stretch/extend
	Purpose:    Adds a new target to be stretched/extended to this property's target list.
	SeeAlso:	NamedStretchProp::RemoveTrigger
***********************************************************************************************/

void NamedStretchProp::AddTrigger(const StringBase& strSet)
{
	// Remember this trigger set's name.
	ERROR3IF(HasTrigger(strSet), "NamedStretchProp::AddTrigger: trigger already exists");
	m_Triggers.push_back(TriggerSet(strSet));
}



/***********************************************************************************************
>   void NamedStretchProp::RemoveTrigger(const StringBase& strSet)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     strSet		---		the name of the target set to remove stretch/extends from
	Purpose:    Removes a target to be stretched/extended from this property's target list.
	SeeAlso:	NamedStretchProp::AddTrigger
***********************************************************************************************/

void NamedStretchProp::RemoveTrigger(const StringBase& strSet)
{
	std::list<TriggerSet>::iterator p = std::find(m_Triggers.begin(), m_Triggers.end(), strSet);
	ERROR3IF(p == m_Triggers.end(), "NamedStretchProp::RemoveTrigger: can't find trigger");
	m_Triggers.erase(p);
}



/***********************************************************************************************
>   BOOL NamedStretchProp::HasTrigger(const StringBase& strSet)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     strSet		---		the name of the target set to lookup
	Returns:	TRUE if the given set is a trigger of this stretch target.
	Purpose:    Looks up stretch trigger sets.
	SeeAlso:	NamedStretchProp::AddTrigger
***********************************************************************************************/

BOOL NamedStretchProp::HasTrigger(const StringBase& strSet)
{
	std::list<TriggerSet>::iterator p = std::find(m_Triggers.begin(), m_Triggers.end(), strSet);
	return p != m_Triggers.end();
}



/***********************************************************************************************
>   virtual SGNameProp* NamedStretchProp::Clone()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Returns:	A copy of this object in the heap.
***********************************************************************************************/

SGNameProp* NamedStretchProp::Clone()
{
	return new NamedStretchProp(*this);
}



/********************************************************************************************
>	virtual BOOL BOOL NamedStretchProp::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to write to
	Returns:	TRUE if the object has successfully written out its record.
	Purpose:	Writes out a NamedStretchProp record.
********************************************************************************************/

BOOL NamedStretchProp::Write(CXaraFileRecord* pRec)
{
	// Write out the base class, stretch type and relative bounds, number of trigger sets.
	if (!NamedTickboxProp::Write(pRec) ||
		!pRec->WriteBYTE(GetStretchType()) ||
		!pRec->WriteCoord(m_rTriggerBounds.lo) ||
		!pRec->WriteCoord(m_rTriggerBounds.hi) ||
		!pRec->WriteCoord(m_rTargetBounds.lo) ||
		!pRec->WriteCoord(m_rTargetBounds.hi) ||
		!pRec->WriteINT16( INT16(GetTriggers().size()) ) )
			return FALSE;

	// Write out the list of trigger sets.
	for (std::list<TriggerSet>::iterator pt = GetTriggers().begin();
		 pt != GetTriggers().end();
		 pt++)
			if (!pRec->WriteUnicode((TCHAR*) (LPCTSTR) pt->m_strSet))
				return FALSE;

	// Success.
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL BOOL NamedStretchProp::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to read in
	Returns:	TRUE if the object has successfully read in its record.
	Purpose:	Reads in a NamedStretchProp record.
********************************************************************************************/

BOOL NamedStretchProp::Read(CXaraFileRecord* pRec)
{
	// Read in the base class, stretch type and relative bounds, number of trigger sets.
	short n;
	if (!NamedTickboxProp::Read(pRec) ||
		!pRec->ReadBYTE(&m_nStretchType) ||
		!pRec->ReadCoord(&m_rTriggerBounds.lo) ||
		!pRec->ReadCoord(&m_rTriggerBounds.hi) ||
		!pRec->ReadCoord(&m_rTargetBounds.lo) ||
		!pRec->ReadCoord(&m_rTargetBounds.hi) ||
		!pRec->ReadINT16(&n))
			return FALSE;

	// Read in the list of trigger sets.
	String_256 strName;
	while (n--)
	{
		if (!pRec->ReadUnicode(&strName)) return FALSE;
		if (!HasTrigger(strName)) m_Triggers.push_back(TriggerSet(strName));
	}	

	// Success.
	return TRUE;
}



/***********************************************************************************************
>   virtual void NamedStretchProp::CalcUiBounds(SGNameItem* pItem,  SGFormatInfo* pFormatInfo,
												SGMiscInfo* pMiscInfo, DocRect* pMaxBounds)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		(see SGNameItem::CalcUiBounds)
				pMaxBounds		---		the default size of the property - the whole label.
										The calculation should increase lo.x to move the left
										edge of the UI rightwards.
	Outputs:	pMaxBounds		---		the bounds of this property's UI within its SGNameItem.
	SeeAlso:	SGNameItem::CalcUiBounds; NamedTickboxProp
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
void NamedStretchProp::CalcUiBounds(SGNameItem* pItem, SGFormatInfo* pFormatInfo,
									SGMiscInfo* pMiscInfo, DocRect* pMaxBounds)
{
	// Target invoker is 16 pixels wide.
	NamedTickboxProp::CalcUiBounds(pItem, pFormatInfo, pMiscInfo, pMaxBounds);
	m_drInvoker = *pMaxBounds;
	pMaxBounds->lo.x -= pMiscInfo->PixelSize * 16;
}
#endif

/***********************************************************************************************
>   virtual BOOL NamedStretchProp::HandleRedraw(SGNameItem* pItem, SGRedrawInfo* pRedrawInfo, 
												SGMiscInfo* pMiscInfo, const DocRect& drBounds)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see SGNameItem::HandleRedraw)
				drBounds	---		the bounds to render within
	Returns:	TRUE if successful.
	Purpose:    Draws the UI for the state of this NamedStretchProp.
	SeeAlso:	SGNameItem::HandleRedraw; NamedTickboxProp::HandleMouse
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
BOOL NamedStretchProp::HandleRedraw(SGNameItem* pItem, SGRedrawInfo* pRedrawInfo,
									SGMiscInfo* pMiscInfo, const DocRect& drBounds)
{
	// Only draw the trigger invoker if the source isn't empty and there is at
	// least one other set existing, to be a trigger.
	if (!pItem->IsEmpty() && (pItem->GetPrevious() != 0 || pItem->GetNext() != 0))
		pRedrawInfo->Renderer->DrawBitmap(m_drInvoker.lo,
				m_Triggers.empty() ? _R(IDB_NAMEGAL_NO_TRIGGER) : _R(IDB_NAMEGAL_TRIGGER));

	// Now do the default.
	return NamedTickboxProp::HandleRedraw(pItem, pRedrawInfo, pMiscInfo, drBounds);
}
#endif

/***********************************************************************************************
>   virtual BOOL NamedStretchProp::HandleMouse(SGNameItem* pItem, SGMouseInfo* pMouseInfo,
											   SGMiscInfo* pMiscInfo)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see SGNameItem::HandleEvent)
	Returns:	TRUE if successful.
	Purpose:    Responds to mouse events according to the state of this NamedStretchProp.
	SeeAlso:	SGNameItem::HandleEvent; NamedTickboxProp::HandleMouse
***********************************************************************************************/

PORTNOTE("dialog","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
BOOL NamedStretchProp::HandleMouse(SGNameItem* pItem, SGMouseInfo* pMouseInfo,
								   SGMiscInfo* pMiscInfo)
{
	// ticking and unticking the stretching properties
	// must reset the size of the reference rectangles
	// the same as if the triggers have changed (sjk)
	BOOL fUsedOnOffTick = !m_drInvoker.ContainsCoord(pMouseInfo->Position);

	// If the source is empty then it can have no editable triggers.
	if (pItem->IsEmpty()) return TRUE;

	// Count the other non-empty items in the 'Used Names' group.
	INT32 nSets = 0;
	SGUsedNames* pUsedNames = NameGallery::Instance()->GetUsedNames();
	for (SGNameItem* pi = (SGNameItem*) pUsedNames->GetChild();
		 pi != 0;
		 pi = (SGNameItem*) pi->GetNext())
			++nSets;

	// If there isn't another set we can't set any triggers
	if (--nSets == 0) return TRUE;
	
	// Create the trigger flag array for the dialog to show/edit.
	BYTE* pbTicks = new BYTE[nSets];
	ERRORIF(pbTicks == 0, _R(IDE_NOMORE_MEMORY), FALSE);

	// Set up the array with the existing settings.
	BYTE* pbTicksIter = pbTicks;
	for (pi = (SGNameItem*) pUsedNames->GetChild();
		 pi != 0;
		 pi = (SGNameItem*) pi->GetNext())
	{
		// Skip the source set.
		if (pi == pItem) continue;

		// Lookup the current item in the target list and transfer any settings.
		String_256 strName;
		pi->GetNameText(&strName);
		*pbTicksIter++ = HasTrigger(strName);
	}

	BYTE nNewType = m_nStretchType;

	if (!fUsedOnOffTick)
	{
		// Run the Edit Targets dialog.
		BOOL fOk;
		OpParamExtendSetsDlg pm(pbTicks, pItem, &fOk, &nNewType);

		OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_EXTENDSETSDLG);
		ERROR3IF(pDesc == 0, "NamedStretchProp::HandleMouse: can't find descriptor");
		pDesc->Invoke(&pm);

		if (!fOk)
		{
			delete[] pbTicks;
			return TRUE;
		}
	}

	// If the targets have been changed then clone this object and set the new targets
	// in the clone.
	NamedStretchProp* pClone = (NamedStretchProp*) Clone();
	ERRORIF(pClone == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	pClone->m_Triggers.clear();

	// Apply any edits.
	DocRect drTrigBounds(0, 0, 0, 0);
	BOOL fAnyTicks = FALSE;
	pClone->m_nStretchType = nNewType;
	pbTicksIter = pbTicks;

	if (fUsedOnOffTick) pClone->m_nState = !m_nState;
	
	for (pi = (SGNameItem*) pUsedNames->GetChild();
		 pi != 0;
		 pi = (SGNameItem*) pi->GetNext())
	{
		// Skip the target set.
		if (pi == pItem) continue;

		// Add the set and its stretch type if it's a trigger.
		if (*pbTicksIter++)
		{
			fAnyTicks = TRUE;
			String_256 strName;
			pi->GetNameText(&strName);
			pClone->AddTrigger(strName);

			NamedStretchProp* pStretchProp = (NamedStretchProp*) pi->GetProperty(nIndex);
			ERROR3IF(pStretchProp == 0, "NamedStretchProp::HandleMouse: no stretch prop");

			// Include the trigger's bounds in the accumulated union.
			drTrigBounds = drTrigBounds.Union(pi->GetSetBounds());
		}		
	}

	// Record the bounds of the target and union of triggers at the point of
	// (re)definition of the stretch.
	pClone->SetRefTargetBounds(pItem->GetSetBounds());
	pClone->SetRefUnionTriggerBounds(drTrigBounds);

	// Try to run an op to change the property in the tree undoably.
	if (fAnyTicks && !fUsedOnOffTick) pClone->m_nState = TRUE;
	pItem->ForceRedrawOfMyself();

	if (pClone->m_nState)
	{
		if (!pClone->ValidateStretchingObjects(pItem))
		{
			InformWarning(_R(IDE_SGNODEREGULARSHAPESDETECTED));
		}
	}

	delete[] pbTicks;
	return Change(pItem, pClone);
}
#endif

/***********************************************************************************************
>   const DocRect& NamedStretchProp::GetRefTargetBounds() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:	The reference bounding box of this set as a target (ie. its bounds when its
				streches and extensions were last edited).
***********************************************************************************************/

const DocRect& NamedStretchProp::GetRefTargetBounds() const
{
	return m_rTargetBounds;
}

/***********************************************************************************************
>   const DocRect& NamedStretchProp::GetRefUnionTriggerBounds() const

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Returns:	The reference bounding box of this set's union of trigger sets (ie. their
				bounds when this set's streches and extensions were last edited).
***********************************************************************************************/

const DocRect& NamedStretchProp::GetRefUnionTriggerBounds() const
{
	return m_rTriggerBounds;
}



/***********************************************************************************************
>   void NamedStretchProp::SetRefTargetBounds(const DocRect& rRefTarget)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Purpose:	Sets the reference bounding box for this set as a target of stretches and
				extensions.  From this rectangle and the reference union of triggers, we can
				always calculate how this set should be changed during a stretch.
***********************************************************************************************/

void NamedStretchProp::SetRefTargetBounds(const DocRect& rRefTarget)
{
	m_rTargetBounds = rRefTarget;
}



/***********************************************************************************************
>   void NamedStretchProp::SetRefUnionTriggerBounds(const DocRect& rRefTriggers)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Purpose:	Sets the reference bounding box for the union of this set's stretch/extend
				trigger sets.  From this rectangle and the reference union of triggers, we can
				always calculate how this set should be changed during a stretch.
***********************************************************************************************/

void NamedStretchProp::SetRefUnionTriggerBounds(const DocRect& rRefTriggers)
{
	m_rTriggerBounds = rRefTriggers;
}



/***********************************************************************************************
>   virtual void NamedStretchProp::GetDebugInfo(StringBase* pStr)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see Node::GetDebugDetails)
	Purpose:    Outputs debugging information on this property.
	SeeAlso:	SGNameItem::HandleEvent; NamedExportProp::HandleRedraw
***********************************************************************************************/

#if DEBUG_TREE

void NamedStretchProp::GetDebugInfo(StringBase* pStr)
{
	*pStr += TEXT("    Stretch: ");
	*pStr += (m_nState ? TEXT("YES") : TEXT("NO"));

	if (m_nStretchType & X_EXTEND)  *pStr += TEXT(", Extends X");
	if (m_nStretchType & Y_EXTEND)  *pStr += TEXT(", Extends Y");
	if (m_nStretchType & X_STRETCH) *pStr += TEXT(", Stretches X");
	if (m_nStretchType & Y_STRETCH) *pStr += TEXT(", Stretches Y");
	
	for (std::list<TriggerSet>::iterator p = m_Triggers.begin(); p != m_Triggers.end(); ++p)
	{
		*pStr += TEXT("        ");
		*pStr += p->m_strSet;
		*pStr += TEXT("\r\n");
	}
}

#endif


/***************************************************************************************

>	BOOL NamedStretchProp::ValidateStretchingObjects(SGNameItem *pItem)

	Author		: Matt Priestley
	Created		: 13 February 2001
	Purpose		: Checks through the given SGNameItem's members and all linked stretchy objects
				  to ensure that none of them have a NodeRegularShape in them as this Node will
				  not extend in an entirely predictable fashion...

	Returns		: BOOL - TRUE if no problems with this stretchy setup. FALSE otherwise...
	Argument	: SGNameItem *pItem - The SGNameItem we wish to test

***************************************************************************************/

BOOL NamedStretchProp::ValidateStretchingObjects(SGNameItem *pItem)
{
	PORTNOTETRACE("dialog","NamedStretchProp::ValidateStretchingObjects - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	SGUsedNames* pUsedNames = NameGallery::Instance()->GetUsedNames();
	Spread *pSpread = Document::GetSelectedSpread();
	if (!pItem || !pUsedNames || !pSpread)
	{
		ERROR3("NULL SGNameItem, SGUsedNames or Spread in ValidateStretchingObjects()");
		return TRUE;
	}

	SGNameItem *pTempItem = (SGNameItem*) pUsedNames->GetChild();
	while (pTempItem)
	{
		String_256 strName;
		pTempItem->GetNameText(&strName);

		if (HasTrigger(strName) || pTempItem == pItem)
		{
			Node * pNode = SliceHelper::FindNextOfClass((Node *)pSpread, (Node *)pSpread, CC_RUNTIME_CLASS(TemplateAttribute));
			while (pNode)
			{
				if ((((TemplateAttribute *)pNode)->GetParam() == strName) && !pNode->FindParent()->IsNodeHidden())
				{
					if (IS_A(pNode->FindParent(), NodeRegularShape))
					{
						// Then we have found a NodeRegularShape in the stretching selection...
						return FALSE;
					}

					if (IS_A(pNode->FindParent(), NodeGroup) || IS_A(pNode->FindParent(), NodeShadowController)|| IS_A(pNode->FindParent(), NodeContourController) || IS_A(pNode->FindParent(), NodeBevelController))
					{
						// Then we've found a controller node or a group node - look at its children...
						Node * pChild = pNode->FindParent()->FindFirstChild();
						while (pChild)
						{
							if (!pChild->IsAnAttribute() && !pChild->IsNodeHidden() && IS_A(pChild, NodeRegularShape))
							{
								// Then we have found a NodeRegularShape in the stretching selection...
								return FALSE;
							}
							pChild = pChild->FindNext();
						}
					}
				}

				pNode = SliceHelper::FindNextOfClass(pNode, (Node *)pSpread, CC_RUNTIME_CLASS(TemplateAttribute));
			}
		}

		pTempItem = (SGNameItem*) pTempItem->GetNext();
	}

	return TRUE;
#else
	return FALSE;
#endif
}



/***********************************************************************************************
>   TriggerSet::TriggerSet(const StringBase& strSet)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     strSet		---		the name of the TriggerSet set to stretch/extend
	Purpose:    Constructs a TriggerSet record for a stretch/extend.
	SeeAlso:	NamedStretchProp; StringBase::Alloc
***********************************************************************************************/

TriggerSet::TriggerSet(const StringBase& strSet)
{
	if (m_strSet.Alloc(strSet.Length() + 1)) m_strSet = strSet;
}



/***********************************************************************************************
>   TriggerSet::TriggerSet(const TriggerSet& other)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     other	---		NamedStretchProp::TriggerSet to copy
	Purpose:    Copy constructor.
	SeeAlso:	NamedStretchProp; StringBase::Alloc
***********************************************************************************************/

TriggerSet::TriggerSet(const TriggerSet& other)
{
	if (m_strSet.Alloc(other.m_strSet.Length() + 1)) m_strSet = other.m_strSet;
}



/***********************************************************************************************
>   NamedSliceProp::NamedSliceProp(INT32 nState = 0)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     nState		---		the initial state of the tickbox (default is unticked).
	Purpose:    Constructs a NamedSliceProp object.
***********************************************************************************************/

NamedSliceProp::NamedSliceProp(const StringBase& strName, INT32 nState)
  : NamedTickboxProp(strName, NamedSliceProp::nIndex, nState)
{
	// Empty.
}



/***********************************************************************************************
>   virtual SGNameProp* NamedSliceProp::Clone()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Returns:	A copy of this object in the heap.
***********************************************************************************************/

SGNameProp* NamedSliceProp::Clone()
{
	return new NamedSliceProp(*this);
}



/********************************************************************************************
>	virtual BOOL BOOL NamedSliceProp::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to write to
	Returns:	TRUE if the object has successfully written out its record.
	Purpose:	Writes out a NamedSliceProp record.
********************************************************************************************/

BOOL NamedSliceProp::Write(CXaraFileRecord* pRec)
{
	return NamedTickboxProp::Write(pRec);
}



/********************************************************************************************
>	virtual BOOL BOOL NamedSliceProp::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/9/99
	Inputs:		pRec		---		record to read in
	Returns:	TRUE if the object has successfully read in its record.
	Purpose:	Reads in a NamedSliceProp record.
********************************************************************************************/

BOOL NamedSliceProp::Read(CXaraFileRecord* pRec)
{
	return NamedTickboxProp::Read(pRec);
}



/***********************************************************************************************
>   virtual void NamedSliceProp::GetDebugInfo(StringBase* pStr)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     (see Node::GetDebugDetails)
	Purpose:    Outputs debugging information on this property.
	SeeAlso:	SGNameItem::HandleEvent; NamedExportProp::HandleRedraw
***********************************************************************************************/

#if DEBUG_TREE

void NamedSliceProp::GetDebugInfo(StringBase* pStr)
{
	*pStr += TEXT("    Slice: ");
	*pStr += (m_nState ? TEXT("YES\r\n") : TEXT("NO\r\n"));
}

#endif



/***********************************************************************************************
>   BOOL NamedExportProp::ExportPropertiesToLocationDlg(SGNameItem* &pItem)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99 - 24/8/00
	Inputs:     pItem -	the ptr to the named item to export
				NB this is a ptr by reference since the dlg may change the name and properties of the item,
				which changes the instance of the item itself
	Purpose:    Brings up the Save As dlg for exporting of named items
	SeeAlso:	Used if the triangle to the right of the named item in the name gallery is clicked
				or if export is pressed and the item is virgin, ie the user hasn't specified what it is or
				where it should be saved.
***********************************************************************************************/
BOOL NamedExportProp::ExportPropertiesToLocationDlg(SGNameItem* &pItem)
{
	PORTNOTETRACE("dialog","NamedExportProp::ExportPropertiesToLocationDlg - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	NamedExportProp* pNewProp;
	String_256 strNewName;

	// A click on the choose filter/path invoker.  First make a working copy of this
	// property.
	pNewProp = (NamedExportProp*) Clone();
	ERRORIF(pNewProp == 0, _R(IDE_NOMORE_MEMORY), FALSE);

	// Work out which filter and filename to initially select in the dialog.
	UINT32 nFilterID;
	INT32 nChosenID;
	if (pNewProp->m_pFilter != 0)
	{
		// We have good filter and path information to show.
		nFilterID = pNewProp->m_pFilter->FilterID;
		nChosenID = pNewProp->m_pFilter->pOILFilter->Position;
	}
	else
	{
		// The implied format is not recognised by any filters, so suggest GIF
		// and suggest the name of the item stripped of the problematic extension.
		nFilterID = FILTERID_TI_GIF;
		nChosenID = Filter::FindFilterFromID(nFilterID)->pOILFilter->Position;

		String_256 strPath;
		if (!OpMenuExport::DefaultExportFilterPath.IsEmpty())
			strPath = OpMenuExport::DefaultExportFilterPath;
		else
			FileUtil::GetCurrentDirectory(&strPath);

		// Construct a path and set the default extension (export type) if necessary.
		strPath += TEXT("\\");
		strPath += GetName();
		pNewProp->m_Path.SetPathName(strPath);
		pNewProp->m_Path.SetType(TEXT("gif"));
		ERROR3IF(!pNewProp->m_Path.IsValid(),
						"NamedExportProp::HandleMouse: invalid path");
	}

	// Build the contents of the filter listbox.
	char* pchList =
			BaseFileDialog::BuildFilterString(FALSE, nFilterID, &nChosenID, CC_RUNTIME_CLASS(BitmapFilter), pNewProp->GetOptions()->GetDepth());
	if (pchList == 0)
	{
		delete pNewProp;
		return FALSE;
	}

	// Set up the export file dialog.
	ExportFileDialog dlg(pchList);
	dlg.SetTitle(_R(IDS_NAMEGAL_EXP_TITLE));
	dlg.SetSelectedFilterIndex(nChosenID);
	dlg.SetInitialDirectory(pNewProp->m_Path.GetLocation(TRUE));
	dlg.SetDefaultFileName(
			String_256(pNewProp->m_Path.GetFileName(pNewProp->m_pFilter != 0)));

	// Run it with an Apply button rather than Save/Export.
	NamedExportProp::m_fApplyNotExport = TRUE;
	BOOL fOk = dlg.OpenAndGetFileName();
	NamedExportProp::m_fApplyNotExport = FALSE;
	CCFree(pchList);

	// Did the user cancel without making any changes?
	if (!fOk)
	{
		delete pNewProp;
		return TRUE;
	}

	// Retrieve the user's choice and look up the filter.
	dlg.GetChosenFileName(&pNewProp->m_Path);
	nChosenID = dlg.GetSelectedFilterIndex();
	for (pNewProp->m_pFilter = Filter::GetFirst();
		 pNewProp->m_pFilter != 0;
		 pNewProp->m_pFilter = Filter::GetNext(pNewProp->m_pFilter))
			if (pNewProp->m_pFilter->GetFlags().CanExport && 
				pNewProp->m_pFilter->pOILFilter->Position == nChosenID)
					break;

	// Didn't find one?
	if (pNewProp->m_pFilter == 0)
	{
		delete pNewProp;
		Error::SetError(_R(IDT_CANT_FIND_FILTER));
		return FALSE;
	}

	// Needs fixing to match the extension and filter?
	if (!pNewProp->m_pFilter->pOILFilter->DoesExtensionOfPathNameMatch(&pNewProp->m_Path))
		pNewProp->m_pFilter->pOILFilter->FixExtensionOfPathName(&pNewProp->m_Path);

	// Still bad?
	if (!pNewProp->m_Path.IsValid())
	{
		delete pNewProp;
		return FALSE;
	}
	
	// Remember this good path for the next export operation.
	OpMenuExport::DefaultExportFilterPath = pNewProp->m_Path.GetLocation(FALSE);

	// If the filter has been changed then restore the options to the default.
	// TODO: work out default export options (presently only bitmap exports are
	// supported.)
	if (m_pFilter != pNewProp->m_pFilter &&
		pNewProp->m_pFilter->IS_KIND_OF(BaseBitmapFilter))
	{
		// Make some default filter options.
		BitmapExportOptions* pOptions =
				((BaseBitmapFilter*) pNewProp->m_pFilter)->CreateExportOptions();
		if (pOptions == 0)
		{
			delete pNewProp;
			ERROR1RAW(_R(IDE_NOMORE_MEMORY));
			return FALSE;
		}

		pOptions->RetrieveDefaults();
		pNewProp->SetOptions(pOptions);			
	}

	// If the filename has changed then rename the set accordingly.
	String_256 strOldFile(m_Path.GetFileName(FALSE)),
			   strNewFile(pNewProp->m_Path.GetFileName(FALSE));
	if (strNewFile.CompareTo(strOldFile, FALSE) != 0)
		strNewName = strNewFile;

	// Undoably replace the old export property with the new, optionally renaming
	// it if something was put in strNewName;
	BOOL ok = Change(pItem, pNewProp, strNewName.IsEmpty() ? 0 : &strNewName);

	// set pItem to point at the changed item if it has changed
	if (ok && !strNewName.IsEmpty())
	{
		NameGallery * pNameGallery = NameGallery::Instance();
		if (pNameGallery)
		{
			pNameGallery->FastUpdateNamedSetSizes(FALSE); // this will create any new NGItems that are found in the tree
			pItem = SliceHelper::LookupNameGalleryItem(strNewName);
		}
	}

	return ok;
#else
	return FALSE;
#endif
}






