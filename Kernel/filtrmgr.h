// $Id: filtrmgr.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_FILTRMGR
#define INC_FILTRMGR

#include "ccarray.h"
#include "list.h"

typedef UINT32 FileFormatID;

class Filter;
/**************************************************************************************

>	class FilterList : public List

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	Today
	Purpose:	Take out some of the functionality from filters

**************************************************************************************/
class FilterList : public List
{
	CC_DECLARE_MEMDUMP(FilterList)
public:
	FilterList();

	BOOL AddFilter(Filter* const pFilter, const UINT32 ID);
};


class FileFormat;
class FormatEntry;
class FilterEntry;
class FilterList;
class FilterSearchCriteria;

class FormatArray : public CCArray<FormatEntry*, FormatEntry*&>
{
};

class FileFormat;
/**************************************************************************************

>	class FilterManager : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	Today
	Purpose:	Take out some of the functionality from filters

**************************************************************************************/
class FilterManager : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(FilterManager);

public:
	FilterManager();

	BOOL Init();
	INT32 DeInit();

	// Registering functions
	// What to do with filters, formats & families at the end of the day
	enum DESTRUCTION_METHOD
	{
		DM_INVALID			= 0,	// don't use this, it's for catching duff initialization
		DM_DELETE_WHEN_DONE	= 1,
		DM_KEEP_WHEN_DONE
	};

	UINT32			RegisterFilter(	Filter* const pFilter, 
									const DESTRUCTION_METHOD DestructMethod);
	FileFormatID	RegisterFileFormat(	FileFormat* const pFormat, 
										const DESTRUCTION_METHOD DestructMethod);

	// Aggregation functions
	BOOL	AssociateFilterWithFormat(	const UINT32 FilterID, 
										const FileFormatID FileFormatID);

	// Search functions
	// Filters...
	Filter*	FindFilterFromID(const UINT32 FilterID) const;

	Filter*	FindFilter(FilterSearchCriteria& Criteria) const;
	FilterList* ListMatchingFilters(FilterSearchCriteria& Criteria);

	// FileFormats...
	FileFormat*		FindFormatFromID(const FileFormatID FormatID) const;
	FileFormat*		FindFormatFromName(const StringBase& FormatName) const;
	FileFormatID	FindFormatIDFromName(const StringBase& Name) const;

protected:
	List&				GetFilters() const;
	const UINT32			FILTERID_START;
	UINT32				GetNewFilterID();

	FormatArray&		GetFormats();
	const FormatArray&	GetFormats() const	{	return m_Formats;	}
	const FileFormatID	FORMATID_START;
	FileFormatID		GetNewFormatID();
	FileFormatID		GetLastFormatID() const	{	return m_NextFormatID - 1;	}

	// And for searching
	FormatEntry*	FindFormatEntryFromID(const FileFormatID FormatID) const;
	FormatEntry*	FindFormatEntryFromName(const StringBase& FormatName) const;

private:
	// Lists of things the filter manager works with
	UINT32			m_NextFilterID;

	FormatArray		m_Formats;
	FileFormatID	m_NextFormatID;
};


/********************************************************************************************

>	class FilterEntry : public ListItem

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim)
	Created:	28/03/94
	Purpose:	A simple ListItem derived class to allow a list of filters to be
				maintained - the list holds all the filters that claim to understand a
				particular file (see FilterFamily).
				It is not derived from the Filter class itself because although the Filter 
				class is derived from ListItem too, we want to store a list of pointers to 
				existing filters, rather than actual instances of the Filter class.
				For each entry, a 'compatibility rating' is held, to allow the 
				FilterFamily class (the client of this class) to choose the most
				compatible filter from the list.
	SeeAlso:	CompatibleFilterList; Filter; ListItem

********************************************************************************************/
class FilterEntry : public ListItem
{
	CC_DECLARE_MEMDUMP(FilterEntry)

public:
	FilterEntry(Filter* const pFilter, const UINT32 ID);

protected:
	// Compatibility rating for this filter.
	Filter*		m_pFilter;
	UINT32		m_FilterID;

};

/********************************************************************************************

>	class FormatEntry : protected CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/96
	Purpose:	A simple ListItem derived class to allow a list of filters to be
				maintained
	SeeAlso:	FilterList
	Scope:		private to FilterManager

********************************************************************************************/
class FilterManager;

class FormatEntry : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FormatEntry)

public:
	FormatEntry(const FileFormatID ID, FileFormat* const pFormat, 
				const FilterManager::DESTRUCTION_METHOD DestructMethod);

	~FormatEntry();

	FileFormat*			GetFormat() const				{ return m_pFormat;	}
	FileFormatID		GetID() const					{ return m_FormatID;	}
	FilterList&			GetFilters()					{ return m_AssociatedFilters;	}

	FilterManager::DESTRUCTION_METHOD	GetDestructionMethod() const 
														{ return m_DestructMethod;	}

protected:

	FileFormat*			m_pFormat;				// Pointer to the format in question.
	FileFormatID		m_FormatID;				// and it's ID
	FilterList			m_AssociatedFilters;	// A list of filters that think they can cope

	// What to do with the format at the end...
	FilterManager::DESTRUCTION_METHOD	m_DestructMethod;	
};


#endif	// INC_FILTRMGR
