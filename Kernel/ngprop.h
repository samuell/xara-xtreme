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
	$Header: /wxCamelot/Kernel/ngprop.h 13    13/07/05 9:26 Luke $
	Attribute gallery named set properties.
*/

#ifndef NGPROP_H
#define NGPROP_H

#include "undoop.h"
#include "extender.h"	// for extend byte-flags
#include <list>

class SGNameItem;
class CXaraFileRecord;
class BitmapExportOptions;
class OpExportSets;


/***********************************************************************************************
>	class CCAPI SGNameProp : public CC_CLASS_MEMDUMP

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Purpose:	Abstract base class for the various kinds of properties associated
				with SGNameItems.
	SeeAlso:	NamedTickboxProp; SGNameItem; NameGallery
***********************************************************************************************/

class CCAPI SGNameProp : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(SGNameProp);
public:
	// Range of valid property indices: 0 ... (nPropertyCount - 1)
	enum { nPropertyCount = 3 };

	// Creation & destruction.
	SGNameProp(const StringBase& strName, INT32 nIndex);
	SGNameProp(const SGNameProp& other);
	virtual ~SGNameProp();

	// Create a default attribute of the given index (type) for the given item.
	static SGNameProp* CreateDefault(const StringBase& strName, INT32 nIndex);

	// Return type information for this property.
	const StringBase& GetName() const		{ return m_strName; }
	INT32 GetIndex() const					{ return m_nIndex; }

	// Returns TRUE if this property has been edited at some time, FALSE if has not
	// been edited since it was first created.
	BOOL IsVirgin() const					{ return m_fIsVirgin; }
	void SetDirty()							{ m_fIsVirgin = FALSE; }

PORTNOTE("other","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
	// Event & redraw handlers are extensions of the given SGNameItem's.
	virtual BOOL HandleMouse(SGNameItem*, SGMouseInfo*, SGMiscInfo*) = 0;
	virtual void CalcUiBounds(SGNameItem*, SGFormatInfo*, SGMiscInfo*, DocRect*) = 0;
	virtual BOOL HandleRedraw(SGNameItem*, SGRedrawInfo*, SGMiscInfo*, const DocRect&) = 0;
#endif

	// Version 2 native streaming.  Derived classes must override these to read into
	// or write from this object.
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

	// Makes a copy of this kind of SGNameProp.  Derived classes must override this
	// to return a copy of themselves in the heap.
	virtual SGNameProp* Clone() = 0;


protected:	
	// Runs OpChangeSetProperty to change the item's NodeSetProperty in the tree. The
	// optional third parameter, if supplied, renames the item as well.
	virtual BOOL Change(SGNameItem* pItem, SGNameProp* pNewProp, StringBase* pstrNewName = 0);

private:
	// Data.
	StringBase	m_strName;			// name of set (SGNameItem) associated with this property
	INT32			m_nIndex;			// index of this property
	BOOL		m_fIsVirgin;		// TRUE if just created and has never been edited

#if DEBUG_TREE
public:
	virtual void GetDebugInfo(StringBase* pStr) = 0;
#endif
};



/***********************************************************************************************
>	class CCAPI NamedTickboxProp : public SGNameProp

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Purpose:	Abstract base class for the various kinds of properties associated with
				SGNameItems that have a UI of a (tri-state) tick-box.
	SeeAlso:	SGNameProp; SGNameItem; NameGallery
***********************************************************************************************/

class CCAPI NamedTickboxProp : public SGNameProp
{
public:
	// Returns the state of this property's tickbox.
	INT32 GetState() const
		{ return m_nState; }

protected:
	// Creation & destruction.
	NamedTickboxProp(const StringBase& strName, INT32 nIndex, INT32 nState = 0);
	NamedTickboxProp(const NamedTickboxProp& other);

PORTNOTE("other","Removed SuperGallery usage")
	// Event & redraw handlers.
#ifndef EXCLUDE_FROM_XARALX
	virtual BOOL HandleMouse(SGNameItem*, SGMouseInfo*, SGMiscInfo*);
	virtual void CalcUiBounds(SGNameItem*, SGFormatInfo*, SGMiscInfo*, DocRect*);
	virtual BOOL HandleRedraw(SGNameItem*, SGRedrawInfo*, SGMiscInfo*, const DocRect&);
#endif

	// Version 2 native streaming.  Writes out tickbox state.
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

	// Data.
	INT32 m_nState;
};



/***********************************************************************************************
>	class CCAPI NamedExportProp : public SGNameProp

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Purpose:	Class to manage the UI and execution of batch-export properties.
	SeeAlso:	SGNameProp; NamedTickboxProp; SGNameItem; NameGallery
***********************************************************************************************/

class CCAPI NamedExportProp : public SGNameProp
{
public:
	// Creation & destruction.
	NamedExportProp(const StringBase& strName);
	NamedExportProp(const NamedExportProp& other);
	virtual ~NamedExportProp();

	// The index (offset into array of properties) of this kind of SGNameProp.
	enum { nIndex = 0 };

	// Access the stored export path and options.
	Filter* GetFilter() const;
	PathName& GetPath();
	BitmapExportOptions* GetOptions() const;

	void SetFilter(Filter* pFilter);
	void SetPath(const PathName& path);
	BitmapExportOptions* SetOptions(BitmapExportOptions* pOptions);

	// The export dialogs should check this and use an Apply button rather than an
	// Export button if it is true.
	static BOOL m_fApplyNotExport;
	BOOL ExportPropertiesToLocationDlg(SGNameItem* &pItem);

#if DEBUG_TREE
	virtual void GetDebugInfo(StringBase* pStr);
#endif

protected:
	virtual SGNameProp* Clone();
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

PORTNOTE("other","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
	virtual BOOL HandleMouse(SGNameItem*, SGMouseInfo*, SGMiscInfo*);
	virtual void CalcUiBounds(SGNameItem*, SGFormatInfo*, SGMiscInfo*, DocRect*);
	virtual BOOL HandleRedraw(SGNameItem*, SGRedrawInfo*, SGMiscInfo*, const DocRect&);
#endif
	// Data.
	Filter* m_pFilter;						// the filter used to export
	BitmapExportOptions* m_pOptions;		// the dialog options for the export
	PathName m_Path;						// path to the exported file
	DocRect m_drPathInvoker;				// bounds of choose filter/path invoker gadget
};



/********************************************************************************************
>	class CCAPI NamedSliceProp : public NamedTickboxProp

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Purpose:	Class to manage the UI and execution of slice properties.
	SeeAlso:	SGNameProp; NamedTickboxProp; SGNameItem; NameGallery
********************************************************************************************/

class CCAPI NamedSliceProp : public NamedTickboxProp
{
public:
	// Creation & destruction.
	NamedSliceProp(const StringBase& strName, INT32 nState = 0);

	// The index (offset into array of properties) of this kind of SGNameProp.
	enum { nIndex = 1 };

protected:
	virtual SGNameProp* Clone();
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

#if DEBUG_TREE
public:
	virtual void GetDebugInfo(StringBase* pStr);
#endif
};



/***********************************************************************************************
>	class CCAPI NamedStretchProp : public NamedTickboxProp

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Purpose:	Class to manage the UI and execution of stretch/extend properties.
	SeeAlso:	SGNameProp; NamedTickboxProp; SGNameItem; NameGallery
***********************************************************************************************/

// This represents a "trigger" (source) set.
struct CCAPI TriggerSet
{
	// Creation & destruction.
	TriggerSet(const StringBase& strSet);
	TriggerSet(const TriggerSet& other);

	// Attributes.
	INT32 operator==(const StringBase& strName) const
		{ return strName == m_strSet; }

	// Data.
	StringBase m_strSet;	// the name of the trigger set
};



class CCAPI NamedStretchProp : public NamedTickboxProp
{
public:
	// Creation & destruction.
	NamedStretchProp(const StringBase& strName, BYTE nStretchType = X_EXTEND | Y_EXTEND ,INT32 nState = 0);
	NamedStretchProp(const NamedStretchProp& other);

	// The index (offset into array of properties) of this kind of SGNameProp.
	enum { nIndex = 2 };

	// Add and remove a trigger set that stretches/extends etc.
	void AddTrigger(const StringBase& strSet);
	void RemoveTrigger(const StringBase& strSet);
	BOOL HasTrigger(const StringBase& strSet);

	// Bounds of triggers and targets.
	const DocRect& GetRefTargetBounds() const;
	const DocRect& GetRefUnionTriggerBounds() const;
	void SetRefTargetBounds(const DocRect& rRefTarget);
	void SetRefUnionTriggerBounds(const DocRect& rRefTriggers);

	// Return a read-only reference to the list of triggers.
	std::list<TriggerSet>& GetTriggers()	{ return m_Triggers; }

	// Set/get the stretch/extend mode for the triggers.
	void SetStretchType(BYTE nType)				{ m_nStretchType = nType; }
	BYTE GetStretchType() const				{ return m_nStretchType; }


	// Matt - 13/02/2001
	// We need some method of checking through the currently selected stretchy stuff to warn the
	// user if they have a NodeRegularShape in their stretchy selection - as they don't extend
	// predictably...
	BOOL ValidateStretchingObjects(SGNameItem *pItem);


protected:
	virtual SGNameProp* Clone();
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

PORTNOTE("other","Removed SuperGallery usage")
#ifndef EXCLUDE_FROM_XARALX
	virtual BOOL HandleMouse(SGNameItem*, SGMouseInfo*, SGMiscInfo*);
	virtual void CalcUiBounds(SGNameItem*, SGFormatInfo*, SGMiscInfo*, DocRect*);
	virtual BOOL HandleRedraw(SGNameItem*, SGRedrawInfo*, SGMiscInfo*, const DocRect&);
#endif

	// Data.
	std::list<TriggerSet> m_Triggers;	// source sets which stretch/extend this set
	DocRect		m_drInvoker;			// bounds of trigger dialog invoker gadget
	BYTE		m_nStretchType;			// how they stretch, eg. X_EXTEND | Y_STRETCH
	DocRect		m_rTargetBounds;		// reference bounds of this set as a target
	DocRect		m_rTriggerBounds;		// reference bounds of union of set's triggers

#if DEBUG_TREE
public:
	virtual void GetDebugInfo(StringBase* pStr);
#endif
};

#endif	/* !NGPROP_H */
