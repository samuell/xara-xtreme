// $Id: layermgr.h 662 2006-03-14 21:31:49Z alex $
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
*/ 

/* Declaration of the following classes
	LyrDetails
	LayerManager
	OpLayerChange
*/

#ifndef INC_LAYERMGR
#define INC_LAYERMGR

// Include files 
#include "layer.h"
#include "listitem.h"
#include "list.h"
#include "undoop.h"        
#include "selop.h"

#define OPTOKEN_LYRCHNG _T("LayerChange")	// Optoken for the layer change operation 
#define OPTOKEN_MOVE_SEL_TO_ACTIVE_LAYER _T("MoveSelToActiveLayer")

class String_256; 


/********************************************************************************************

>	class LyrDetails: public ListItem 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/94
	Purpose:	LyrDetails objects are stored in a list within the LayerManager. 
				They record changes made to a layer. 

	SeeAlso:	LayerManager

********************************************************************************************/


class LyrDetails: public ListItem 
{
	CC_DECLARE_MEMDUMP(LyrDetails);

public:
	LyrDetails(); 		// Constructor 
	Layer *m_pLayer; 	// A pointer to the Layer that the LyrDetails record describes. 
						// NULL if the layer is New

	LayerStatus Status; // The LayerStatus structure contains all the layer details which the 
						// user can change 

	BOOL New; 	 	    // Flag indicating if the layer is new. i.e. it does not exist in the 
					    // Camelot tree. 

	BOOL Deleted; 	    // Flag indicating if the layer has been deleted 

};

/********************************************************************************************

>	class LayerManager: public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/94
	Purpose:	The LayerManager is used to make changes to layers in the document tree. It 
				holds a pointer to a current spread, and a list of layer details for the 
				current spread (LyrDetLst). The layerDlg and InfoBar make changes to layers 
				in the current spread indirectly by changing the LyrDetList. Layer changes 
				are deferred until the CommitLayerChanges() method is called. Changes made 
				to the LyrDetList can also be cancelled by calling the CancelLayerChanges() 
				method.  

	SeeAlso:	LyrDetails
	SeeAlso:	LayerDlg
	SeeAlso:	OpLayerChange 

********************************************************************************************/

class LayerManager:public CCObject
{
public:

	LayerManager();			 // Constructor 
	~LayerManager();			 // Destructor 
	
	String_256 CreateUniqueLayerID(); 		 // Creates a layer name which is unique for 
											 // the current spread.  
											 	
   	BOOL CommitLayerChanges(); 				 // Performs a LayerChangeOp which modifies the 
   											 // layers in the Camelot tree based on changes 
   											 // made to the LyrDetList. 

	BOOL NewCurrentSpread(Spread* NewCurrentSpread); // NewCurrentSpread becomes the 
													 // new current spread 

	Spread* GetCurrentSpread(); 			 // Returns the current spread 

	BOOL CancelLayerChanges(); 				 // Causes the changes made to the LyrDetList 
											 // to be discarded and new values read. 

	BOOL RefreshLayerDetails(); 			 // Refreshes the layer details list 

	void UpdateInterface(); 				 // Called to update the interface 



	// The layer details list is a list of LyrDetails for the current spread.
	List LyrDetList; 

private:	
	Spread* CurrentSpread;			 // The current spread 

	void DeleteLayerDetList();  	 // Deletes the contents of the LyrDetList 
	BOOL ReadSpreadLayerDetails();   // Read the current spread's layer details into the 
									 // LyrDetList
	void BroadcastNewCurrentSpread();// Makes calls to other subsystems that need to know 
									 // that the current spread has changed. 
	BOOL LayerDetailsChanged(); 	 // Used to determine if the layer details have been 
									 // changed. We don't want to generate a LayerChangeOp
									 // if nothing has changed. 

};


   
/********************************************************************************************

>	class OpLayerChange: public UndoableOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the LayerChange operation which is created by the 
				LayerManager to make changes to the layers of the current spread. 
	SeeAlso:	LayerManager

********************************************************************************************/

class CCAPI OpLayerChange: public UndoableOperation
{         

	CC_DECLARE_DYNCREATE( OpLayerChange );

public:
	OpLayerChange();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);

	virtual BOOL Undo(); 
	virtual BOOL Redo(); 	
};  

/********************************************************************************************

>	class ChangeLyrAttribAction: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/94
	Purpose:	When executed the ChangeLyrAttribAction	changes a layer's Status
	SeeAlso:	-

********************************************************************************************/

class CCAPI ChangeLyrAttribAction: public Action
{                                
	CC_DECLARE_DYNCREATE( ChangeLyrAttribAction )

public:
	ChangeLyrAttribAction(); 
	~ChangeLyrAttribAction();
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 	
						   UINT32 ActionSize,
						   LayerStatus& Status,  // The layer status which is restored 
						   					     // when we execute the action. 
						   Layer* Layer, 		 // The layer to change when we execute 
						   Action** NewAction); 

private:
	Layer* ChangeLayer; // The layer to change 
	LayerStatus Status; // the new attributes of the layer 
};   
       




/********************************************************************************************

>	class CCAPI OpMoveToLayer: public UndoableOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> K
	Created:	3/5/00
	Purpose:	An op to move the selection from where-ever it is to the active layer.

********************************************************************************************/
class CCAPI OpMoveToLayer: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpMoveToLayer );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);
};  





#endif
					

