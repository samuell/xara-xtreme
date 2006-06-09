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
// Header file for Camelots Print Marks Doc component classes

/*
*/

#ifndef INC_DOCPRINTMARKS
#define INC_DOCPRINTMARKS

//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccomp.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class BaseCamelotFilter;
class CXaraFileRecord;
class CamelotRecordHandler;
class PrintMarkItem;


/********************************************************************************************

>	class DocPrintMark : public ListItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	A document print mark item. This item references a cached print mark held
				by the print mark manager
	SeeAlso:	PrintMarksMan, ListItem

********************************************************************************************/

class DocPrintMark : public ListItem
{
	CC_DECLARE_DYNCREATE(DocPrintMark);
	
	public:
		 DocPrintMark();
		 DocPrintMark(UINT32 hndle);
		~DocPrintMark();

		inline UINT32 GetHandle() const { return Handle; }

	private:
		UINT32	Handle;		// That is all!
};


/********************************************************************************************

>	class DocPrintMarkList : public List

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	Provide a class that will add Print mark components to new documents.
	SeeAlso:	DocComponentClass

********************************************************************************************/

class DocPrintMarkList : public List
{
	CC_DECLARE_DYNAMIC(DocPrintMarkList)
	
	public:
		 DocPrintMarkList();
		~DocPrintMarkList();

		BOOL AddMark(UINT32 handle);
		void RemoveMark(UINT32 handle);
		void RemoveAllMarks();
		BOOL IsVirgin() const { return Virgin; }

		inline DocPrintMark* GetFirstMark() const;
		inline DocPrintMark* GetNextMark(DocPrintMark* pItem) const;

		DocPrintMark* FindMark(UINT32 SearchHandle) const;

	private:
		BOOL Virgin;
};


/********************************************************************************************
>	inline DocPrintMark* DocPrintMarkList::GetFirstMark() const
	inline DocPrintMark* DocPrintMarkList::GetNextMark(DocPrintMark* pItem) const
	Purpose - Quick inline mark access functions.
********************************************************************************************/

inline DocPrintMark* DocPrintMarkList::GetFirstMark() const
{
	return ((DocPrintMark*)GetHead());
}
		
inline DocPrintMark* DocPrintMarkList::GetNextMark(DocPrintMark* pItem) const
{
	return ((DocPrintMark*)GetNext(pItem));
}



/********************************************************************************************

>	class PrintMarkComponentClass : public DocComponentClass

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	The class that gets a print mark doc component going on start up
				We need to register our class as a document component and this is where
				we do it all.
	SeeAlso:	DocComponentClass

********************************************************************************************/

class PrintMarkComponentClass : public DocComponentClass
{
	CC_DECLARE_DYNAMIC(PrintMarkComponentClass)

	public:
		static BOOL Init();

		BOOL AddComponent(BaseDocument *);
};



/********************************************************************************************

>	class PrintMarksComponent : public DocComponent

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	An actual print mark document component. This component contains a list of
				handles which refer to print marks cached by the print marks manager. On
				saving, the mark handles will be resolved into physical marks which will be
				exported and subsequently imported and registered.
	SeeAlso:	DocComponent

********************************************************************************************/

class PrintMarksComponent : public DocComponent
{
	CC_DECLARE_DYNAMIC(PrintMarksComponent)

	public:
		PrintMarksComponent();
		virtual ~PrintMarksComponent();

		BOOL AddMark(UINT32 handle);
		void RemoveMark(UINT32 handle);
		void RemoveAllMarks();
		BOOL IsVirgin() const;
		
			   DocPrintMark* FindMark(UINT32 SearchHandle) const;
		inline DocPrintMark* GetFirstMark() const;
		inline DocPrintMark* GetNextMark(DocPrintMark* pItem) const;

	public:			// External V2 native file import/export handlers
		virtual BOOL StartImport(BaseCamelotFilter *pFilter);
		virtual BOOL EndImport(BaseCamelotFilter *pFilter, BOOL Success);
		virtual BOOL StartExport(BaseCamelotFilter *pFilter);
		virtual BOOL EndExport(BaseCamelotFilter *pFilter, BOOL Success);

		void ImportDefaultPrintMark(CXaraFileRecord* Rec);
		void StartImportCustomPrintMark(CamelotRecordHandler *pHandler, CXaraFileRecord* Rec);
		void EndImportCustomPrintMark(CamelotRecordHandler *pHandler);

	private:
		Node *CustomPreviousContext;
		Node *CustomCurrentContext;
		PrintMarkItem *pNewMark;

	private:		// Internal native file handlers
		BOOL ExportPrintMark(BaseCamelotFilter *pFilter, PrintMarkItem *pMarkItem);

	private:
		DocPrintMarkList DocMarks;
};


/********************************************************************************************
>	inline DocPrintMark* PrintMarksComponent::GetFirstMark() const
	inline DocPrintMark* PrintMarksComponent::GetNextMark(DocPrintMark* pItem) const
	Purpose - Quick inline mark access functions.
********************************************************************************************/

inline DocPrintMark* PrintMarksComponent::GetFirstMark() const
{
	return DocMarks.GetFirstMark();
}

inline DocPrintMark* PrintMarksComponent::GetNextMark(DocPrintMark* pItem) const
{
	return DocMarks.GetNextMark(pItem);
}

#endif
