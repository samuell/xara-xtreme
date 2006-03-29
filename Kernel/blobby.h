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

// Declaration of the following classes
/*
	BlobbyOp         : Demonstration temporary operation which changes the colour of a 
					   documents paper. 
					   and it's actions

*/	



/*
 */        

#ifndef INC_BLOBBY
#define INC_BLOBBY
    
#include "undoop.h"
#include "opdesc.h"   
#include "doccolor.h"
#include "ops.h"  	   	// Operation class  
#include "dlgtypes.h"  	// The DialogOp types
#include "dlgmgr.h"		// the dialogue box manager 
#include "convert.h"
#include "dialogop.h"

#define OPTOKEN_BLOBBY _T("Blobby")

class CCAPI BlobbyOp: public UndoableOperation
{                             
	CC_DECLARE_DYNCREATE( BlobbyOp )

public:
	BlobbyOp();								// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		
    
	void			Do(OpDescriptor*);		// "Do" function
    

};
     
class RedoColAction;      
     
/********************************************************************************************

>	class UndoColAction : public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	Simple BlobbyOp action which restores the colour of a documents page to 
				the  colour it was prior to the operation being performed. 
	SeeAlso:	BlobbyOp

********************************************************************************************/

class CCAPI UndoColAction: public Action
{                                
	CC_DECLARE_DYNCREATE( UndoColAction )
    friend class RedoColAction;
    friend class BlobbyOp; 

public:
	UndoColAction(); 
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 	
						   UINT32 ActionSize,    
						   Action** NewAction);   
private:
	DocColour Color;  
};
  

/********************************************************************************************

>	class RedoColAction : public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	Simple BlobbyOp action which restores the colour of a documents page to 
				the  colour it was prior to the operation being undone. 
	SeeAlso:	BlobbyOp

********************************************************************************************/

  
class CCAPI RedoColAction: public Action
{              
	CC_DECLARE_DYNCREATE( RedoColAction )  
	friend class UndoColAction; 

public:
	RedoColAction(); 
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   UINT32 ActionSize, 
						   Action** NewAction);
private:
	DocColour Color; 
};
       

/********************************************************************************************

>	class RedoAction2 : public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	Dummy BlobbyOp REDO action. Does nothing when it executes.
	SeeAlso:	BlobbyOp

********************************************************************************************/

class CCAPI RedoAction2: public Action
{              
	CC_DECLARE_DYNCREATE( RedoAction2 )  
	friend class UndoColAction; 

public:
	RedoAction2(); 
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   UINT32 ActionSize, 
						   Action** NewAction);
private:
	DocColour Color; 
};

 

/********************************************************************************************

>	class RedoAction3 : public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	Dummy BlobbyOp REDO action. Does nothing when it executes.
	SeeAlso:	BlobbyOp

********************************************************************************************/
 
  
class CCAPI RedoAction3: public Action
{              
	CC_DECLARE_DYNCREATE( RedoAction3 )  
	friend class UndoColAction; 

public:
	RedoAction3(); 
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   UINT32 ActionSize, 
						   Action** NewAction);
private:
	DocColour Color; 
};
               

/********************************************************************************************

>	class UndoAction2 : public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	Dummy BlobbyOp UNDO action. Does nothing when it executes.
	SeeAlso:	BlobbyOp

********************************************************************************************/
               
               
class CCAPI UndoAction2: public Action
{              
	CC_DECLARE_DYNCREATE( UndoAction2 )  
	friend class UndoColAction; 

public:
	UndoAction2(); 
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   UINT32 ActionSize, 
						   Action** NewAction);
private:
	DocColour Color;                           
	
};


/********************************************************************************************

>	class UndoAction3 : public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Purpose:	Dummy BlobbyOp UNDO action. Does nothing when it executes.
	SeeAlso:	BlobbyOp

********************************************************************************************/

  
class CCAPI UndoAction3: public Action
{              
	CC_DECLARE_DYNCREATE( UndoAction3 )  
	friend class UndoColAction; 

public:
	UndoAction3(); 
	virtual ActionCode Execute();    
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   UINT32 ActionSize, 
						   Action** NewAction);
private:
	DocColour Color; 
};       


// -----------------------------------------------------------------------------------------
// BlobbyDlg used to test the Dialog system

#define OPTOKEN_BLOBBYDLG _T("BlobbyDlg")    
   
class BlobbyDlg: public DialogOp
{         
	CC_DECLARE_DYNCREATE( BlobbyDlg )  
public:

	BlobbyDlg(); 
	MsgResult Message( Msg* Message );  
	void Do(OpDescriptor*);		// "Do" function        
	static BOOL Init();                        
	BOOL Create(); 
	static OpState GetState(String_256*, OpDescriptor*);	

	static const UINT32 IDD;
//	enum { IDD = _R(IDD_BLOBBYDLG) };     
	static const CDlgMode Mode;   
	
	String_256 UnitStrList[9];    
	UnitType UnitList[9];   
	UnitType DefaultUnit; 
	UnitType ConvUnit; 
}; 

// -----------------------------------------------------------------------------------------
// BlobbyBar used to test the Dialog system

#define OPTOKEN_BLOBBYBAR _T("BlobbyBar")    
   
class BlobbyBar: public DialogOp
{         
	CC_DECLARE_DYNCREATE( BlobbyBar )  
public:

	BlobbyBar(); 
	MsgResult Message( Msg* Message );  
	void Do(OpDescriptor*);		// "Do" function        
	static BOOL Init();
	BOOL Create(); 
	static OpState GetState(String_256*, OpDescriptor*);	

	static const UINT32 IDD;
	static const CDlgMode Mode;   

	BOOL IsABar() { return TRUE; }

}; 

// -----------------------------------------------------------------------------------------

// BlobbyTabDlg used to test the Dialog system

#define OPTOKEN_BLOBBYTABDLG _T("BlobbyTabDlg")    
   
class BlobbyTabDlg: public DialogTabOp
{         
	CC_DECLARE_DYNCREATE( BlobbyTabDlg )  
public:

	BlobbyTabDlg(); 
	MsgResult Message( Msg* Message ); 
	 
	void Do(OpDescriptor*);		// "Do" function        
	static BOOL Init();                        
	static OpState GetState(String_256*, OpDescriptor*);	
	virtual BOOL RegisterYourPagesInOrderPlease(); 
	static const CDlgMode Mode; 

	// We need to give our Tab dialog's a unique resource ID as this is used to 
	static const UINT32 IDD;
private:
	// Message handlers for each page in the tabbed dialog
	void HandleCoordsMsg(DialogMsg* Msg); 
	void HandleRadioDazeMsg(DialogMsg* Msg); 
	void HandleConvertMsg(DialogMsg* Msg);

  	// Used by Convert page	
	String_256 UnitStrList[9];    
	UnitType UnitList[9];   
	UnitType DefaultUnit; 
	UnitType ConvUnit; 

}; 
                   

       
#endif
                   
