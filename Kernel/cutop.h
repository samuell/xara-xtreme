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
// Clipboard operations

#ifndef INC_CUTOP
#define INC_CUTOP

#include "selop.h"        
#include "trans2d.h"

class DocCoord;

#define OPTOKEN_CUT _T("Cut")
#define OPTOKEN_COPY _T("Copy")
#define OPTOKEN_PASTE _T("Paste")
#define OPTOKEN_PASTEATSAMEPOS _T("PasteAtSamePos")
#define OPTOKEN_DELETE _T("Delete")
#define OPTOKEN_DUPLICATE _T("Duplicate")
#define OPTOKEN_CLONE _T("EditClone")
#define OPTOKEN_COPYANDTRANSFORM _T("CopyAndTransform")
#define OPTOKEN_PASTEATTRIBUTES _T("PasteAttributes")

   
/********************************************************************************************

>	class OpCut : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:    This class represents the Cut operation. When executed this operation hides
				all selected objects and places a copy of them on the Internal clipboard.

********************************************************************************************/

class CCAPI OpCut: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpCut )    

public:
	OpCut();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

};

/********************************************************************************************

>	class OpCopy : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the Copy operation. When executed this operation makes
				a copy of all selected objects and places them on the internal clipboard

********************************************************************************************/

class CCAPI OpCopy: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpCopy )    

public:
	OpCopy();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};  



/********************************************************************************************
>   class OpParamPasteAtPosition

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/96

	Purpose:	Holds information allowing the paste op to paste objects at a specific position

	SeeAlso:	Operation::OpParam

********************************************************************************************/

class OpParamPasteAtPosition : public OpParam
{
CC_DECLARE_DYNAMIC(OpParamPasteAtPosition)

public:
	OpParamPasteAtPosition()
			{ pSpread = NULL; Centre.x = Centre.y = 0; }

	OpParamPasteAtPosition(Spread* pTheSpread,DocCoord& TheCentre)
			{ pSpread = pTheSpread; Centre = TheCentre; }

	Spread*  GetSpread() { return pSpread; }
	DocCoord GetCentre() { return Centre; }

private:
	Spread* pSpread;
	DocCoord Centre;
};

/********************************************************************************************

>	class OpPaste : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the Paste operation. When executed this takes a copy
				of all objects on the internal clipboard and adds them to the visible spread.

********************************************************************************************/

class CCAPI OpPaste: public SelOperation
{         
	CC_DECLARE_DYNCREATE( OpPaste )    

public:
	OpPaste();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);
    void			DoWithParam(OpDescriptor*, OpParam* pOpParam);

	// Matt 22/01/2001 - Static function to remove names from pasted attribs if they would interfere with stretching info
	static BOOL RemoveNamesAlreadyUsedInStretching(Node * pAttrib, UndoableOperation* pOp);

protected:
	void DoPaste(OpDescriptor*,Spread* pSpread, DocCoord* pCentre);

	BOOL FindCentreInsertionPosition(Spread** Spread, DocCoord* Position);
	BOOL DoPasteStandard(BOOL PasteAtSamePos, BOOL ExternalData, Spread* pSpread = NULL, DocCoord* pCentre = NULL);
};

/********************************************************************************************

>	class OpPasteAttributes : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/08/95
	Purpose:	This class represents the PasteAttributes operation. The operation applies
				all attributes common to the objects on the clipboard. This operation is
				not in itself undoable. It invokes another UndoableOperation to actually
				apply the attributes.

				Phil 16/02/2005
				That is no longer true - the op is now derived from SelOperation and calls
				"Do" functions to add their actions to this operation.

********************************************************************************************/

class CCAPI OpPasteAttributes: public SelOperation 
{         
	CC_DECLARE_DYNCREATE( OpPasteAttributes )    

public:
	OpPasteAttributes();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);
};  
  
  
/********************************************************************************************

>	class OpDelete : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the Delete operation.	When executed this operation hides
				all selected objects.

********************************************************************************************/

class CCAPI OpDelete: public SelOperation
{         

	CC_DECLARE_DYNCREATE( OpDelete )    

public:
	OpDelete();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void			Do(OpDescriptor*);	

private:
	// deletes fill ramp blobs
	// returns TRUE if any are deleted
	BOOL DeleteFillRampBlobs();

};


/********************************************************************************************

>	class CarbonCopyOp : public SelOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Purpose:	This class allows OpClone and OpDuplicate to share common code.  Both are
				derived from it.
	SeeAlso:	OpDuplicate, OpClone

********************************************************************************************/
class CCAPI CarbonCopyOp: public SelOperation
{         
	CC_DECLARE_DYNCREATE( CarbonCopyOp )    

public:
	CarbonCopyOp();								
	static	OpState	GetState(String_256*, OpDescriptor*);		
	BOOL	DoProcessing(Trans2DMatrix Transformer, BOOL SelectCopy);
};    
 

/*********************************************************************************************

	Preference:	DuplicatePlacementX
	Section:	Attributes
	Range:		0..Maximum DocCoord
	Purpose:	Specifies where a duplicate copy of an object will be placed relative to the
				original object (X offset)

	SeeAlso:	DuplicatePlacementY

**********************************************************************************************/ 

/*********************************************************************************************

	Preference:	DuplicatePlacementY
	Section:	Attributes
	Range:		0..Maximum DocCoord
	Purpose:	Specifies where a duplicate copy of an object will be placed relative to the
				original object (Y offset)

	SeeAlso:	DuplicatePlacementX

**********************************************************************************************/ 


/********************************************************************************************

>	class OpDuplicate : public CarbonCopyOp

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the Duplicate operation. When executed this operation
				makes a copy of all selected objects, positioning the new objects a user
				prefered distance from the original.

********************************************************************************************/

#ifndef WEBSTER
// Set to (-.5cm,-.5cm)
#define DEFAULT_DUPLICATE_PLACEMENT_X (PX_MP_VAL*20)
#define DEFAULT_DUPLICATE_PLACEMENT_Y -(PX_MP_VAL*20)
#else
// WEBSTER - markn 6/12/96
// Changed default dup to be a round number of pixels
#define DEFAULT_DUPLICATE_PLACEMENT_X 15000
#define DEFAULT_DUPLICATE_PLACEMENT_Y -15000
#endif // WEBSTER

class CCAPI OpDuplicate: public CarbonCopyOp
{         
	CC_DECLARE_DYNCREATE( OpDuplicate )    

public:
	OpDuplicate();								
	static BOOL	Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual	void	Do(OpDescriptor*);	

public:
	// Preference vars which specify the offset of the duplicate copy from the original
	// BODGE USING INT32 FOR NOW

	// Graham 5/6/97: Made these public, so that other features
	//can use the duplicate offset preference
	static INT32 DuplicatePlacementX;
	static INT32 DuplicatePlacementY;
};    
 

/********************************************************************************************

>	class OpClone : public CarbonCopyOp

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Purpose:	This class represents the Clone operation. When executed this operation
				makes a copy of all selected objects, leaving the new objects on top of the
				existing objects.

********************************************************************************************/

class CCAPI OpClone: public CarbonCopyOp
{         
	CC_DECLARE_DYNCREATE( OpClone )    

public:
	OpClone();								
	static BOOL	Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual	void	Do(OpDescriptor*);	
};    


/********************************************************************************************

>	class OpCopyAndTransform : public CarbonCopyOp

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>, copying Peter's code heavily!
	Created:	25/6/96
	Purpose:	This operation creates a transformed copy of the selected objects.

********************************************************************************************/

class CCAPI OpCopyAndTransform: public CarbonCopyOp
{         
	CC_DECLARE_DYNCREATE( OpCopyAndTransform )    

public:
	OpCopyAndTransform();
	OpCopyAndTransform(Trans2DMatrix Transform, BOOL PassSelectCopy=FALSE);
	static BOOL	Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual	void	Do(OpDescriptor*);	

protected:
	Trans2DMatrix CopyTransform;
	BOOL SelectCopy;
};    
#endif	// INC_CUTOP
