// $Id: layergal.h 662 2006-03-14 21:31:49Z alex $
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
// The Layers Gallery header file

#ifndef INC_LAYERGAL
#define INC_LAYERGAL

#include "dialogop.h"
#include "undoop.h"
#include "layer.h"

class LayerSGallery;

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

enum MoveLayerType
{
	MOVELAYER_UPONE,
	MOVELAYER_DOWNONE
};



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



/********************************************************************************************

>	class OpLayerGalParam: 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Purpose:	Holds the parameters for the OpLayerGalChange to perform its stuff on
	SeeAlso:	-

********************************************************************************************/

enum OpLayerGalReason
{
	LAYER_NONE,
	LAYER_NEW,
	LAYER_DELETE,
	LAYER_CHANGE,
	LAYER_CHANGE_NAME,
	LAYER_MOVE,
	LAYER_COPY,
	LAYER_VISIBLE,
	LAYER_ALLVISIBLE,
	LAYER_LOCKED,
	LAYER_MULTILAYER,
	LAYER_TOGGLEBACKGROUND,
	// special frame flags
	LAYER_SOLID,
	LAYER_OVERLAY,
	LAYER_MAKEFRAME,
	LAYER_ACTIVE,
	// special frame related items
	FRAME_SELECT,
	FRAME_NEW,
	FRAME_COPY,
	FRAME_DELETE,
	FRAME_MOVE,
	FRAME_ALLVISIBLE,
	FRAME_MULTILAYER,
	// Special page background reason 
	PAGEBACKGROUND_DEFAULT		// actually deletes the page background layer
};

class OpLayerGalParam
{
public:

	Layer*				pLayer;
	LayerStatus			Status;
	BOOL				NewState;
	BOOL				ToggleBackground;
	Node*				pContextNode;
	LayerSGallery*		pLayerSGal;
	AttachNodeDirection	AttDir;
	BOOL				MoveRedraw;	// Flag that tells the LAYER_MOVE code whether it should redraw after the op

	// Vars used by LAYER_CHANGE
	BOOL		OldVisibleState;
	BOOL		OldLockedState;
	String_256 	OldName;
	BOOL		VisibleState;
	BOOL		LockedState;
	String_256 	NewName;

	OpLayerGalParam(OpLayerGalParam& Other)
	{
		pLayer 			 = Other.pLayer;
		Status 			 = Other.Status;
		NewState 		 = Other.NewState;
		ToggleBackground = Other.ToggleBackground;
		pContextNode 	 = Other.pContextNode;
		pLayerSGal 		 = Other.pLayerSGal;
		AttDir 			 = Other.AttDir;
		MoveRedraw 		 = Other.MoveRedraw;

		VisibleState 	 = Other.VisibleState;
		LockedState 	 = Other.LockedState;
		OldVisibleState  = Other.OldVisibleState;
		OldLockedState   = Other.OldLockedState;
		OldName 		 = Other.OldName;
		NewName 		 = Other.NewName;

		Reason 			 = Other.Reason;
		pParentSpread 	 = Other.pParentSpread;
	}

//-------------
	OpLayerGalParam(OpLayerGalReason ThisReason,Spread* pThisParentSpread)
	 : 	Reason(ThisReason), pParentSpread(pThisParentSpread), pLayer(NULL), 
	 	pContextNode(NULL), pLayerSGal(NULL), ToggleBackground(FALSE)  { /* empty */ }

	OpLayerGalParam() : Reason(LAYER_NONE), pParentSpread(NULL), pLayer(NULL), 
	 	pContextNode(NULL), pLayerSGal(NULL) { /* empty */ }

	Spread*				GetParentSpread() 	{ return pParentSpread; }
	OpLayerGalReason 	GetReason() 		{ return Reason; }

private:
	// These params can only be initialised via the constructor
	OpLayerGalReason	Reason;
	Spread*				pParentSpread;
};


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/********************************************************************************************

>	class LayerStateAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Purpose:	An action which changes the state of a layer or the layer system
	SeeAlso:	-

********************************************************************************************/

class LayerStateAction : public Action
{

CC_DECLARE_DYNCREATE(LayerStateAction)

public:
	LayerStateAction();
	~LayerStateAction();
	virtual ActionCode Execute();
	static ActionCode Init( UndoableOperation* pOp,
							ActionList* pActionList,
							OpLayerGalParam Param);
protected:
	OpLayerGalParam Param;
};

/********************************************************************************************

>	class OpLayerGalChange: public UndoableOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Purpose:	Performs an undoable change (new, delete, change) to a single layer.
	SeeAlso:	-

********************************************************************************************/

#define OPTOKEN_LAYERGALCHANGE      _T("LayerGalChange")		// Optoken for the layer gallery change operation 

class CCAPI OpLayerGalChange: public UndoableOperation
{         

	CC_DECLARE_DYNCREATE( OpLayerGalChange );

public:
	OpLayerGalChange();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			DoWithParam(OpDescriptor*, OpParam* pOpParam);            
	void 			GetOpName(String_256* OpName);
	static BOOL		IsFrameMode();			

	BOOL DoInvalidateLayerRegion(Layer* pLayer);

	virtual BOOL Undo(); 
	virtual BOOL Redo(); 	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return TRUE; }

private:
	OpLayerGalReason	Reason;
	Spread* 			pSpread;
	Layer*				pOpUndoLayer;	// Ptr to a layer relevent to op reason when an Undo occurs
	Layer*				pOpRedoLayer;	// Ptr to a layer relevent to op reason when a Redo occurs
	UINT32				UndoIDS;
	BOOL				BroadcastLayerChanges;
};  

/********************************************************************************************

>	class CCAPI OpDeletePageBackground : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Purpose:	Performs an undoable change of deleting the page background layer.
	SeeAlso:	-

********************************************************************************************/

#define OPTOKEN_DELETEPAGEBACKGROUND	_T("DeletePageBackground")

class CCAPI OpDeletePageBackground : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpDeletePageBackground );

public:
	OpDeletePageBackground();
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};  



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



/********************************************************************************************

>	class LayerDlgParam: 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Purpose:	Holds the parameters for the LayerNameDlg to perform its stuff on
	SeeAlso:	-

********************************************************************************************/

class LayerDlgParam
{
public:

	LayerDlgParam(OpLayerGalReason ThisReason,Spread* pThisSpread,Layer* pThisLayer)
	 : Reason(ThisReason),pSpread(pThisSpread), pLayer(pThisLayer) { /* empty */ }

	OpLayerGalReason 	GetReason() { return Reason; }
	Spread*				GetSpread()	{ return pSpread; }
	Layer*				GetLayer() 	{ return pLayer; }

private:
	// These params can only be initialised via the constructor
	OpLayerGalReason	Reason;
	Spread*				pSpread;
	Layer*				pLayer;
};



#define OPTOKEN_LAYERNAMEDLG _T("LayerNameDlg")

/********************************************************************************************

>	class LayerNameDlg: public DialogOp

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Purpose:	Allows the user to edit the parts of a layer.
	SeeAlso:	-

********************************************************************************************/

class LayerNameDlg: public DialogOp
{         
	CC_DECLARE_DYNCREATE( LayerNameDlg )  
public:

	LayerNameDlg();
	void DoWithParam(OpDescriptor*,OpParam*);		// "DoWithParam" function        
	BOOL Create(); 

	static const INT32 IDD;
	static const CDlgMode Mode;

	static OpState GetState(String_256*, OpDescriptor*);	
	static BOOL Init();                        
	static void StartUp(LayerDlgParam* pParam);

	virtual MsgResult Message(Msg* Message);

	void ShowLayerDetails();
	void CommitDialogValues();

private:
	static void CreateUniqueLayerCopyName(const String_256& strName, String_256* pCopyName);

	Layer*  			pLayer;
	Spread* 			pSpread;
	OpLayerGalReason 	Reason;

	static BOOL 		 IsOpen;
	static LayerNameDlg* pLayerNameDlg;
}; 



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#define OPTOKEN_DISPLAYLAYERGALLERY _T("DisplayLayerGallery")	// Optoken for the display layer gallery operation

/********************************************************************************************

>	class OpDisplayLayerGallery: public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	Displays the layers gallery. This op can be attached to a menu, button or keypress
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpDisplayLayerGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayLayerGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);
};  


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#define OPTOKEN_DISPLAYFRAMEGALLERY _T("DisplayFrameGallery")	// Optoken for the display frame gallery operation

/********************************************************************************************

>	class OpDisplayFrameGallery: public Operation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Purpose:	Displays the frame gallery. This op can be attached to a menu, button or keypress
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpDisplayFrameGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayFrameGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);
};  

#endif	// INC_LAYERGAL

