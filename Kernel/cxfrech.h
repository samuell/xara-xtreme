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
// Header for the record handler class of the v2 file format

#ifndef INC_CXFRECH
#define INC_CXFRECH

#include "listitem.h"
#include "doccoord.h"

#if !defined(EXCLUDE_FROM_XARLIB)
#include "camfiltr.h"
#endif

#define CXFRH_TAG_LIST_END	(0xffffffff)

class CXaraFileRecord;
class BaseCamelotFilter;
class DocColour;
class KernelBitmap;
class Node;
class ColourListComponent;
class BitmapListComponent;
class UnitListComponent;
class DocInfoComponent;
class PrintComponent;
class FontComponent;
class CXaraFile;

class Document;
class Chapter;
class Spread;
class Layer;
class NodePath;

class AtomicTagListItem;
class EssentialTagListItem;
class TagDescriptionListItem;

/********************************************************************************************

>	class CXaraFileRecordHandler : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	This is the class that encapsulates a record handler in the new v2 format.

********************************************************************************************/

class CXaraFileRecordHandler : public ListItem
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(CXaraFileRecordHandler);

public:
	CXaraFileRecordHandler();
	~CXaraFileRecordHandler();

	// Pure virtual functions that must be overridden.
	virtual UINT32*	GetTagList() = 0;
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord) = 0;
	virtual BOOL	HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber);

	// Find out if the record is streamed or not
	virtual BOOL IsStreamed(UINT32 Tag) = 0;

	virtual void IncProgressBarCount(UINT32 n) = 0;

	virtual BOOL IsTagInList(UINT32 Tag);

#if defined(EXCLUDE_FROM_XARLIB)
	virtual BOOL Init(CXaraFile* pCXFile);
#else
	virtual BOOL Init(BaseCamelotFilter* pBaseCamelotFilter);
#endif
	virtual BOOL BeginImport();
	virtual BOOL EndImport();
	virtual BOOL BeginSubtree(UINT32 Tag);
	virtual BOOL EndSubtree(UINT32 Tag);

#ifdef XAR_TREE_DIALOG
	virtual void GetTagText(UINT32 Tag,String_256& Str);
	virtual void GetRecordInfo(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	virtual void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);
	virtual void DescribePath(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);
	virtual void DescribePathRelative(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);
#endif
};

/********************************************************************************************

>	class CamelotRecordHandler : public CXaraFileRecordHandler

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	This is the class that encapsulates a camelot-specific record handler in the new v2 format.

				It's main purpose is to make it easy for record handlers to access BaseCamelotFilter
				functions by providing mirror functions into the filter.

				It also hides the fact that there is an underlying filter.

********************************************************************************************/

class CamelotRecordHandler : public CXaraFileRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(CamelotRecordHandler);

public:
	CamelotRecordHandler();
	~CamelotRecordHandler();

	virtual BOOL IsCamelotHandler() { return TRUE; }

#if defined(EXCLUDE_FROM_XARLIB)
	virtual BOOL Init(CXaraFile* pCXFile);
#else
	virtual BOOL Init(BaseCamelotFilter* pBaseCamelotFilter);

	// Funcs for extracting reusable data items
	BOOL GetDocColour(INT32 ColourRef,DocColour* pDocColour);
	KernelBitmap* GetReadBitmapReference(INT32 BitmapRef);

	// functions that map onto BaseCamelotFilter functions
	ColourListComponent*	GetColourDocComponent();
	BitmapListComponent*	GetBitmapDocComponent();
	UnitListComponent*		GetUnitDocComponent();
	DocInfoComponent*		GetDocInfoComponent();
	PrintComponent*			GetPrintComponent();
	FontComponent*			GetFontComponent();

	BOOL		IsImporting();
	BOOL		IsImportingAtPosition();

	BOOL		InsertNode(Node* pNode);
	BOOL		IncInsertLevel();
	BOOL		DecInsertLevel();
	Node*		GetInsertContextNode();
	void		SetInsertContextNode(Node* pNode);
	void		InsertNextNodeAsChild();
	void		InsertNextNodeAsSibling();

	// Saving & restoring tree-building contexts
	InsertTreeContext *GetInsertContext(void);
	void RestoreInsertContext(InsertTreeContext *pOldState);


	// These calls are used for building the document structure when load a file
	//-----------
	Document*	GetLastDocumentInserted();
	Chapter*	GetLastChapterInserted();
	Spread*		GetLastSpreadInserted();
	Layer*		GetLastLayerInserted();
	NodePath*	GetLastNodePathInserted();

	void	SetLastDocumentInserted(Document* pNode);
	void	SetLastChapterInserted(Chapter* pNode);
	void	SetLastSpreadInserted(Spread* pNode);
	void	SetLastLayerInserted(Layer* pNode);
	void	SetLastNodePathInserted(NodePath* pNodePath);
	
	Layer * GetFirstImportedLayer();
	void	SetFirstImportedLayer(Layer* pLayer);

	UINT32	GetDocumentInsertedCount();
	UINT32	GetChapterInsertedCount();
	UINT32	GetSpreadInsertedCount();
	UINT32	GetLayerInsertedCount();
	UINT32	GetSetSentinelInsertedCount();

	void	IncDocumentInsertedCount();
	void	IncChapterInsertedCount();
	void	IncSpreadInsertedCount();
	void	IncLayerInsertedCount();
	void	IncSetSentinelInsertedCount();

	BOOL AddTagDescription(TagDescriptionListItem* pItem);
	TagDescriptionListItem* GetTagDescription(UINT32 Tag);

	BOOL SetDocumentNudgeSize(UINT32 newVal);

#endif

	void AddAtomicTag(AtomicTagListItem* pItem);
	void AddEssentialTag(EssentialTagListItem* pItem);

	BOOL IsTagInAtomicList(UINT32 Tag);
	BOOL IsTagInEssentialList(UINT32 Tag);

	BOOL UnrecognisedTag(UINT32 Tag);

#if !defined(EXCLUDE_FROM_XARLIB)
	// Path record references
	void		AddPathRecordRefToList(NodePath* pNodePath, UINT32 RecordNumber);
	UINT32		FindPathRecordRefRecordNumber(NodePath* pNodePath);
	NodePath*	FindPathRecordRefPath(UINT32 RecordNumber);

//-----------

	Document*	GetDocument();
	Spread*		GetSpread();

	void		SetCoordOrigin(const DocCoord& Origin);
	DocCoord	GetCoordOrigin();

	void	EndOfFile();
	void	SetTotalNumBytesToRead(UINT32 s);
	void	IncProgressBarCount(UINT32 n);
	void	SetTotalProgressBarCount(UINT32 n);

	BaseCamelotFilter* GetBaseCamelotFilter() { return pBaseCamelotFilter; }

	// Function to turn Compression on or off on the underlying CCFile
	virtual BOOL  SetPreCompression(UINT32 Flags);
	virtual UINT32 GetPreCompression();
#endif

	virtual BOOL  SetCompression(BOOL NewState);

	// Find out if the record is streamed or not
	virtual BOOL IsStreamed(UINT32 Tag);

#if !defined(EXCLUDE_FROM_XARLIB)
	virtual void	SetImportFileType(char* pFileType);
	virtual	BOOL	IsOpeningMinimalWebFormat();
#endif

protected:
#if !defined(EXCLUDE_FROM_XARLIB)
	BaseCamelotFilter* pBaseCamelotFilter;
#else
	CXaraFile* m_pCXFile;
#endif
};

/********************************************************************************************

>	class StripSubTreeRecordHandler : public CamelotRecordHandler

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Purpose:	This is the class is used to strip a subtree of records from the file.

				It works counting the number of TAG_DOWN & TAG_UP records it gets.

				If the next record it gets is a TAG_DOWN, it increments the tree level counter.
				If the next record it gets is a TAG_UP, it decrements the tree level counter.
				If the next record is neither a TAG_UP or TAG_DOWN, and the level counter > 0, it is thrown away
				If the next record is neither a TAG_UP or TAG_DOWN, and the level counter == 0, it is passed on to it's proper handler

********************************************************************************************/

class StripSubTreeRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(StripSubTreeRecordHandler);

public:
	StripSubTreeRecordHandler();

	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);
	virtual BOOL	IsTagInList(UINT32 Tag);
	virtual BOOL	BeginImport();
	virtual void	IncProgressBarCount(UINT32 n) {};

	virtual void StripSubTreeOn()	{ StripSubTree = TRUE; }
	virtual void StripSubTreeOff()	{ StripSubTree = FALSE; }
	virtual BOOL IsStripSubTreeOn()	{ return StripSubTree; }

private:
	BOOL StripSubTree;
	UINT32 LevelCounter;
};

#endif	// INC_CXFRECH
