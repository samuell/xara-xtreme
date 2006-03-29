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

// Declaration of the following classes:
/*
 	
		SelectionState 
		SelNdRng
		SelNd
*/ 

// Check in comments
/*
*/         
      
#ifndef INC_SELSTATE
#define INC_SELSTATE

class Node;

/********************************************************************************************

>	class SelNdRng

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Purpose:	SelNdRng objects are stored in an array within SelectionState objects, they 
				record a range of upto 255 contiguous nodes as being selected.  
	SeeAlso:	SelectionState      

********************************************************************************************/

class SelNdRng : public CC_CLASS_MEMDUMP
{
CC_DECLARE_MEMDUMP(SelNdRng);

public:
	Node* FirstNode;        // First selected node in a contiguous range of selected nodes
	BYTE NumSelected;        // Number of selected nodes including first (Max 255) 
							 // Stored as a byte to minimise space. If there are >  
							 // 255 selected nodes then > 1 SelNdRng should be created. 	
};        

/********************************************************************************************

>	class SelectionState

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Purpose:	Used to record and restore all selections in the Camelot tree. 
				 
	SeeAlso:	-

********************************************************************************************/

class SelectionState : public CC_CLASS_MEMDUMP
{
CC_DECLARE_MEMDUMP(SelectionState);

public:  
	SelectionState(); 
	BOOL Record(); 	     						 // Records the current selected state
	void Restore(BOOL RestoreSelBlobs = FALSE, BOOL RemoveBlobs = TRUE); // Restores the recorded selected state   
    ~SelectionState(); 	 						 // Deletes lists 
	UINT32 GetSize(); 							 // For finding the size of the Selection
												 // state in bytes
	BOOL operator==(SelectionState& SelState); 	 // Compares this SelectionState with another to determine
												 // if they are the same. 

	SelNdRng* GetSelectionList();              // Diccon 9/99 access functions
	UINT32 GetNumRanges();
	Node** GetNodeList();                       
	UINT32 GetNumNodes();                         
private:	
	// Funcs called from Restore() to ensure every single node in the selected spread is deselected.
	void DeselectAll(BOOL RemoveBlobs = TRUE);
	void DeselectAll(Node* pNode);

	// When Record() is invoked all selected nodes will be recorded on either the SelNdRng 
	// list, or the SelNdLst. If a node is contiguous with other selected nodes then it 
	// will be stored on the SelNdRngList (Therby reducing the amount of storage required for 
	// recording selected nodes). Otherwise the node is stored on the SelNdLst. 
	
	UINT32 NumNdRng; 		  // The number of SelNdRng items in the SelNdRngList
	UINT32 NumNd; 			  // The number of Node* items in the SelNdList
	SelNdRng* SelNdRngList;   // An array of node ranges 
	Node** SelNdList; 		  // An array of pointers to nodes in the selection 
}; 


#endif
