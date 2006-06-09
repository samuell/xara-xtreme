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
// clipboard mapping for native files

#include "camtypes.h"

#include "clipext.h"
#include "native.h"
#include "natclipm.h"

#include "clipint.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"

CC_IMPLEMENT_DYNCREATE(NativeClipMap, ClipboardMapping)


/********************************************************************************************

>	NativeClipMap::NativeClipMap(ClipboardMappingType TheType, UINT32 ClaimType = 0)
	
	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/96

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - specifies the text type that this mapping will claim from the
				windows clipboard - that is, create 3 of these mappings, specifying
				CF_TEXT, CF_UNICODETEXT, and CF_OEMTEXT, and they will all ask the
				clipboard for UNICODE text when they actually go to import. This allows
				us to detect the 3 implicitly-converted clipboard formats, and map them
				all to UNICODE, i.e. use the UNICODE mapping for all 3 available formats.
				If ClaimType == 0, UNICODE is assumed

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	BodgeTextClipMap::CreateAndRegister

********************************************************************************************/

NativeClipMap::NativeClipMap(ClipboardMappingType TheType, UINT32 ClaimType)
				: ClipboardMapping(TheType, NULL, InternalClipboardFormat(CLIPTYPE_VECTOR),
					ClaimType, 130)
{
	if (ClaimType != 0)
		ExternalDataType = ClaimType;
}

NativeClipMap::NativeClipMap()
{
	ERROR3("Please don't press that button again");
}

/********************************************************************************************

>	static void NativeClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType = 0)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/96

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - specifies the text type that this mapping will claim from the

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

********************************************************************************************/

void NativeClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType)
{
	NativeClipMap *Mapping = new NativeClipMap(TheType, ClaimType);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}

/********************************************************************************************

>	virtual BOOL NativeClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/96
	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.
	Returns:	TRUE for success
	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.

********************************************************************************************/

BOOL NativeClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData, InternalClipboard *Dest)
{
/*
	Ben Summers wrote this on 12/9/96:

		The following code does work as far as getting data off the clipboard is concerned.
		
		However, it can't work as it should as there are are fundamental problems with the
		clipboard document and the way the native file format is imported.

		The native filter ignores the old system for importing data, and grafts the new
		subtree into a document tree all by it's very own. It always imports with layers,
		and this shafts the internal clipboard which expects everything to go onto a
		special layer it creates, preferably getting the Op to do the attaching.

		It's also possible that the Op can get a bit confused with the nodes going
		directly into the tree, but this could be a incorrect rumour.

		It could be done with some major bodging, but it has been decided not to do this.

		So we've got an export only thingy for OLE.

		It's stuff like this which makes me think that programming is in fact a
		particularly bad career choice. Why can't we just sit down and do lots
		of mathematics all day?

		We could also sit down more often for tea and biscuits, be nice to
		each other, design systems without letting spurious unjustified religious
		ideas from assembly language ideas hang over into OO programs, and maybe
		even stop inventing the wheel.

		Maybe then we'd all be happier, and Camelot smaller, neater and possible
		to maintain.

  Markn wrote this on 9/10/96:
		
		This func can now import native files safely using the current paste system by doing
		a bit of housekeeping on the doc before returning, ensuring that all the objects on the 
		clipboard are pastable.	This housekeeping is done in RemoveMultipleLayers().

		It is a little bit of a bodge, but it's not horrendous.  It gives the same level of pasting you
		get when you copy objects from one doc, and paste into another doc, where both docs live in the
		same instance of Camelot.

		I agree with Ben that we should have tea and biscuits more often.  But do we have to be nice
		to each other as well?

---------------------------- */

	BOOL ok = FALSE;

	// Get a scratch file - if TMP isn't set, this will try for c:\temp.
	// The filename will have XS as a prefix
	char *tempname = GetTempFileName();
	if (tempname == NULL)
	{
		ERROR3("Couldn't get a temp filename");
		return(FALSE);
	}

	void *pMem = GlobalLock(ClipboardData);
	ok = (pMem != NULL);

	// you may not want to stop importing with layers -- but only
	// do this if you're not importing into the InternalClipboard
	// document -- it's not too happy if you bung layers in it
	BOOL Layers = Filter::ImportWithLayers;
	Filter::ImportWithLayers = FALSE;

	// pFilter is a class var which is the filter which will be used
	// for importing this file
	pFilter = 0;

	// find the cam native filter...
	Filter* pSearchFilter = Filter::GetFirst();
	while (pSearchFilter != NULL)
	{
		// is this the right filter?
		if(pSearchFilter->FilterID == FILTERID_NATIVE)
		{
			// got it!
			pFilter = pSearchFilter;
			break;
		}

		// next!
		pSearchFilter = Filter::GetNext(pSearchFilter);
	}

	TRACEUSER( "Ben", _T("NativeClipMap prospective native filter is 0x%x\n"), pFilter);			

	// got one?
	ok = (pFilter != NULL);

	CCDiskFile File;
	if(ok)
	{
		// get a file and write to it
		PathName pathname(tempname);
		if(!File.open(pathname, (ios::out | ios::binary)))
			ok = FALSE;		// error!
	}

	// bung out the data
	if(ok)
	{
		File.write(pMem, GlobalSize(ClipboardData));

		File.close();
	}

	// unlock the block of data
	GlobalUnlock(ClipboardData);

	// do our funky stuff with the large lump of stuff
	if(ok)
	{
		ok = ImportFromTempFile(tempname, Caller, Dest);
	}

	// we don't like temp files which aren't needed any more
	RemoveTempFile();

	// Make sure all the objects in the clipboard live on the first layer, so that paste works correctly
	ok = RemoveMultipleLayers(Dest);

	// unset our pointer to the filter
	pFilter = NULL;

	// we don't want to screw things up with importing layers, because that would be bad
	Filter::ImportWithLayers = Layers;

	return(ok);
}

/********************************************************************************************

>	BOOL NativeClipMap::RemoveMultipleLayers(InternalClipboard* pImportDoc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/96
	Inputs:		pImportDoc = ptr to the clipboard doc that received the imported file.
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This ensures that there is only one layer after an import into the given clipboard doc.
				Objects that live in any extra layers are moved to the first layer, and the extra layers
				are deleted.
				
				This function is used to get around the limitation of the current pasting system.
				Pasting assumes that all the objects live on the first layer in the clipboard doc.
				This function ensures that the doc is in a state that allows its entire contents to
				be pasted by the paste op.

********************************************************************************************/

BOOL NativeClipMap::RemoveMultipleLayers(InternalClipboard* pImportDoc)
{
	ERROR2IF(pImportDoc == NULL,FALSE,"NULL import doc ptr");

	// We must have a spread.  If not, error
	Spread* pSpread = pImportDoc->FindFirstSpread();
	if (pSpread == NULL)
		return FALSE;

	// There must be a first layer.  If not error
	Layer* pFirstLayer = pSpread->FindFirstLayer();
	if (pFirstLayer == NULL)
		return FALSE;

	// Likewise, there must be a last layer.
	Layer* pLastLayer = pSpread->FindLastLayer();
	if (pLastLayer == NULL)
		return FALSE;

	// If there isn't a layer after the first layer (i.e. there's only one layer), 
	// then return now, as there's nothing to do
	Layer* pLayer = pFirstLayer->FindNextLayer();
	if (pLayer == NULL)
		return TRUE;

	// If the layer after the first layer is also the last layer, then we have only two layers.
	// When there's only two layers, don't bother grouping the objects from the second layer when
	// moving them onto the first layer.
	BOOL GroupLayerContents = (pLayer != pLastLayer);

	while (pLayer != NULL)
	{
		Node* pContextNode = NULL;

		// If moving the contents of the layer into a group, create a group, attach it into the 
		// first layer, and make the group the context node
		if (GroupLayerContents)
		{
			pContextNode = new NodeGroup;
			if (pContextNode == NULL)		// If unable to make a group, fail
				return FALSE;

			pContextNode->AttachNode(pFirstLayer,LASTCHILD);
		}
		else
			pContextNode = pFirstLayer;		// Otherwise just make the first layer the context node

		if (pContextNode != NULL)
		{
			Node* pNode = pLayer->FindFirstChild();
			while (pNode != NULL)
			{
				// Move each child of the layer as a last child of the context node
				Node* pNext = pNode->FindNext();
				pNode->MoveNode(pContextNode,LASTCHILD);
				pNode = pNext;
			}
		}

		// This layer is an extra layer that is now empty, so make it the layer to delete
		Layer* pLayerToDelete = pLayer;
		// First, invalidate the bounding rect
		pLayerToDelete->InvalidateBoundingRect();

		// Find the next layer before we delete the current one
		pLayer = pLayer->FindNextLayer();

		// Delete that empty layer
		pLayerToDelete->CascadeDelete();
		delete pLayerToDelete;
	}

	// Lastly, invalidate the bounding rect of the layer where all the objects are now
	pFirstLayer->InvalidateBoundingRect();

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL NativeClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/96

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard. Usually this just involves calling Filter::DoExport
				for the parent filter, and then returning the handle to the global memory
				block to be placed onto the external clipboard.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

********************************************************************************************/

HANDLE NativeClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
	// get a temporary file name from the base class to use as 
	// something to export files into it -- this is only one
	// possible way of getting a CCLexFile derivative to
	// put the exported data into.
	char *tempname = GetTempFileName();
	if (tempname == NULL)
	{
		ERROR3("Couldn't get a temp filename");
		return(FALSE);
	}

	// make a new disk file of this name
	CCDiskFile File;
	PathName pathname(tempname);
	if(!File.open(pathname, (ios::in | ios::out | ios::binary)))
		return 0;		// error!
	
	// get a filter to export the thingy to
	// we create a new filter to use rather than scanning for the
	// one in the filter list because it's basically just that
	// little bit easier
	CamelotNativeFilter *pFilter = new CamelotNativeFilter;
	BOOL ok = (pFilter != NULL);

	if(ok)
	{
		// export to the file -- the filter will realise it's exporting
		// from a clipboard, so won't get destressed and export a preview
		// bitmap, which won't work as we don't have an OilDoc for
		// the InternalClipboard documents
		ok = pFilter->DoExport(Caller, &File, &pathname, (Document *)Source, FALSE);
	}

	// delete the filter, we don't need it any more
	delete pFilter;
	pFilter = NULL;

	// this is scary Windows stuff. We're allocating a chunk of memory we
	// can throw at the clipboard, and we're going to read the temporary
	// file into it.
	void* pMem;
	INT32 FileSize;
	HANDLE hGlobalMem;

	if (ok)
	{
		// find out how big the file is
		FileSize = File.Size();

	#if (_OLE_VER >= 0x200)
	
		// Is memory already allocated?
		if ((hGlobalMem = m_hMem) != 0)
		{
			// Is the file too big?
			if (UINT32(FileSize) > m_cbMemSize) return 0;
		}
		else
		{
			// No.  Allocate some.
			hGlobalMem = m_hMem = GlobalAlloc(GHND, FileSize);
		}

	#else
		
		// copy the file into the global block
		pMem = GlobalLock(hGlobalMem);
	
	#endif

		// Lock the block.
		pMem = GlobalLock(hGlobalMem);
		ok = (pMem != 0);
	}

	if (ok)
	{
		// seek to beginning of file
		File.seekIn(0);

		// load data..
		File.read(pMem, FileSize);
	}

	// close the file
	if(File.isOpen()) File.close();

	// get rid of the temp file
	RemoveTempFile();

	// We must unlock the block before giving it to the clipboard
	GlobalUnlock(hGlobalMem);

	// and there we go, one block of memory with a native file sans preview bitmap
	// ready to be splatted onto the windows clipboard
	return hGlobalMem;
}



