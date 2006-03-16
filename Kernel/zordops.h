// $Id: zordops.h 662 2006-03-14 21:31:49Z alex $
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
/*$Log*/

#ifndef INC_ZORDOPS
#define INC_ZORDOPS


#include "selop.h"        

#define OPTOKEN_BRINGTOFRONT _T("BringToFront")
#define OPTOKEN_PUTTOBACK _T("PutToBack")  
#define OPTOKEN_MOVEFORWARDS _T("MoveForwards")
#define OPTOKEN_MOVEBACKWARDS _T("MoveBackwards")
#define OPTOKEN_MOVELAYERBEHIND _T("MoveBackwardsALayer")
#define OPTOKEN_MOVELAYERINFRONT _T("MoveForwardsALayer") 


enum MODE { LAYER, FRAME };

   
/********************************************************************************************

>	class OpBringToFront : public SelOperation

	Author:			Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:		9/7/93
	Purpose:		This class represents the BringToFront operation.
	SeeAlso:		OpPutToBack
	SeeAlso:    	OpMoveForwards
	SeeAlso: 		OpMoveBackwards
	Documentation:  specs\grpzord.doc

********************************************************************************************/

class CCAPI OpBringToFront: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpBringToFront )    

public:
	OpBringToFront();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

};


/********************************************************************************************

>	class OpPutToBack : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the PutToBack operation.
	SeeAlso:	OpBringToFront
	SeeAlso:    OpMoveForwards
	SeeAlso: 	OpMoveBackwards
	Documentation:  specs\grpzord.doc


********************************************************************************************/

class CCAPI OpPutToBack: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpPutToBack )    

public:
	OpPutToBack();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};  
 
/********************************************************************************************

>	class OpMoveForwards : public UndoableOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the OpMoveForwards operation.
	SeeAlso:	OpBringToFront
	SeeAlso:	OpPutToBack
	SeeAlso: 	OpMoveBackwards
	Documentation:  specs\grpzord.doc


********************************************************************************************/

class CCAPI OpMoveForwards: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpMoveForwards )    

public:
	OpMoveForwards();								
	static BOOL		Init();				
//	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
}; 

/********************************************************************************************

>	class OpMoveBackwards : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the MoveBackwards operation.
	SeeAlso:	OpBringToFront
	SeeAlso:	OpPutToBack
	SeeAlso:    OpMoveForwards
	Documentation:  specs\grpzord.doc


********************************************************************************************/

class CCAPI OpMoveBackwards: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpMoveBackwards )    

public:
	OpMoveBackwards();								
	static BOOL		Init();				
//	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};   


/********************************************************************************************

>	class OpMoveToLyrInFront : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/94
	Purpose:	This class represents the move to layer in front operation.
	SeeAlso:	OpBringToFront
	SeeAlso:	OpPutToBack
	SeeAlso:    OpMoveForwards
	SeeAlso:	OpMoveBackwards
	SeeAlso:	OpMoveToLyrBehind
	Documentation:  specs\grpzord.doc


********************************************************************************************/

class CCAPI OpMoveToLyrInFront: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpMoveToLyrInFront )    

public:
	OpMoveToLyrInFront();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};   


/********************************************************************************************

>	class OpMoveToLyrBehind : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the move to layer behind operation.
	SeeAlso:	OpBringToFront
	SeeAlso:	OpPutToBack
	SeeAlso:    OpMoveForwards
	SeeAlso:	OpMoveBackwards
	SeeAlso:	OpMoveToLyrInFront
	Documentation:  specs\grpzord.doc


********************************************************************************************/

class CCAPI OpMoveToLyrBehind: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpMoveToLyrBehind )    

public:
	OpMoveToLyrBehind();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};   

/********************************************************************************************

>	class OpCombineLayersToFrameLayer : public SelOperation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/97
	Purpose:	This class represents the OpCombineLayersToFrameLayer operation.

********************************************************************************************/

#define OPTOKEN_COMBINELAYERSTOFRAMELAYER _T("CombineLayersToFrameLayer")

class CCAPI OpCombineLayersToFrameLayer: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpCombineLayersToFrameLayer )    

public:
	OpCombineLayersToFrameLayer();
	
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		

	virtual void Do( OpDescriptor * pOpDesc );
	virtual void DoWithParam(OpDescriptor* pOpDesc, OpParam* pParam);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};   

/********************************************************************************************

>	class FrameInFrontOpDescriptor : public OpDescriptor

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Purpose:	New FrameInFrontOpDescriptor class whose purpose is to allow an OpDescriptor
				to be created which allows us to over-ride the virtual function GetText.

********************************************************************************************/

class FrameInFrontOpDescriptor : public OpDescriptor
{
	CC_DECLARE_DYNAMIC( FrameInFrontOpDescriptor );

public:
	FrameInFrontOpDescriptor(	const TCHAR* pcszToken,
								CCRuntimeClass* pClass = CC_RUNTIME_CLASS(FrameInFrontOpDescriptor),
								pfnGetState gs = FrameInFrontOpDescriptor::GetState
							);

	// This will use the TextID to obtain a string resource text description of the operation. 
	virtual BOOL GetText(String_256* Description, OpTextFlags WhichText);

	static OpState GetState(String_256* psName, OpDescriptor* pOpDesc);
};


/********************************************************************************************

>	class FrameBehindOpDescriptor : public OpDescriptor

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Purpose:	New FrameBehindOpDescriptor class whose purpose is to allow an OpDescriptor
				to be created which allows us to over-ride the virtual function GetText.

********************************************************************************************/

class FrameBehindOpDescriptor : public OpDescriptor
{
	CC_DECLARE_DYNAMIC( FrameBehindOpDescriptor );

public:
	FrameBehindOpDescriptor(	const TCHAR* pcszToken,
								CCRuntimeClass* pClass = CC_RUNTIME_CLASS(FrameBehindOpDescriptor),
								pfnGetState gs = FrameBehindOpDescriptor::GetState
							);

	// This will use the TextID to obtain a string resource text description of the operation. 
	virtual BOOL GetText(String_256* Description, OpTextFlags WhichText);

	static OpState GetState(String_256* psName, OpDescriptor* pOpDesc);
};

#endif
 

 
