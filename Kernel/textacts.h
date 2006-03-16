// $Id: textacts.h 662 2006-03-14 21:31:49Z alex $
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
// Actions used by text operations

#ifndef INC_TEXTACTS
#define INC_TEXTACTS

#include "ops.h"

class TextStory;
class TextChar;
class KernCode;

/********************************************************************************************

>	class BaseTextAction : public Action

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	Base class of all text actions

********************************************************************************************/

class BaseTextAction : public Action
{
	CC_DECLARE_DYNCREATE(BaseTextAction);

	BaseTextAction();

protected:
	TextStory* pStory;

};



/********************************************************************************************

>	class StoreCharCodeAction : public BaseTextAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	This action is used to restore a character's UniCode value on undo.  Using
				this an op can chage the character code of a TextChar without having to 
				hide the old char and create a new one.
	SeeAlso:	BaseTextAction

********************************************************************************************/

class StoreCharCodeAction : public BaseTextAction
{

CC_DECLARE_DYNCREATE(StoreCharCodeAction)

public:
	StoreCharCodeAction();
	~StoreCharCodeAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation*, ActionList*, TextStory*, TextChar*);
	static BOOL DoStoreCharacterCode( Operation* pOp, ActionList*, TextChar*);

protected:
	TextChar* pChar;
	WCHAR CharCode;
}; 



/********************************************************************************************

>	class StoreKernCodeAction : public BaseTextAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	This action is used to restore a kern value on undo.  Using this an op can
				change the kern value without having to hide the old KernCode and create a
				new one.
	SeeAlso:	BaseTextAction

********************************************************************************************/

class StoreKernCodeAction : public BaseTextAction
{

CC_DECLARE_DYNCREATE(StoreKernCodeAction)

public:
	StoreKernCodeAction();
	~StoreKernCodeAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation*, ActionList*, TextStory*, KernCode*);
	static BOOL DoStoreKernValue( Operation* pOp, ActionList*, KernCode*);

	// Access functions used when op merging
	KernCode* GetKernCode() const {return pKernCode;}
	DocCoord GetKernValue() const {return KernValue;}
	void SetKernCode(DocCoord NewValue) {KernValue = NewValue;}

protected:
	KernCode* pKernCode;
	DocCoord KernValue;
}; 



/********************************************************************************************

>	class ReversePathAction : public BaseTextAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	This action is used to reverse (change direction of) the path inside a TextStory
	SeeAlso:	BaseTextAction

********************************************************************************************/

class ReversePathAction : public BaseTextAction
{

CC_DECLARE_DYNCREATE(ReversePathAction)

public:
	ReversePathAction();
	~ReversePathAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation*, ActionList*, TextStory*);
	static BOOL DoReversePath( Operation* pOp, ActionList*, TextStory*);
}; 



/********************************************************************************************

>	class MatrixFitToPathAction : public BaseTextAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	This action is used when fitting a TextStory on a path in order to undo 
				matrix changes
	SeeAlso:	BaseTextAction

********************************************************************************************/

class MatrixFitToPathAction : public BaseTextAction
{

CC_DECLARE_DYNCREATE(MatrixFitToPathAction)

public:
	MatrixFitToPathAction();
	~MatrixFitToPathAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation*, ActionList*, TextStory*);
	static BOOL DoMatrixRemoveFromPath( Operation* pOp, ActionList*, TextStory*);
}; 



/********************************************************************************************

>	class MatrixRemoveFromPathAction : public BaseTextAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	This action is used when fitting a TextStory on a path in order to undo 
				matrix changes
	SeeAlso:	BaseTextAction

********************************************************************************************/

class MatrixRemoveFromPathAction : public BaseTextAction
{

CC_DECLARE_DYNCREATE(MatrixRemoveFromPathAction)

public:
	MatrixRemoveFromPathAction();
	~MatrixRemoveFromPathAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation*, ActionList*, TextStory*);
	static BOOL DoMatrixFitToPath( Operation* pOp, ActionList*, TextStory*);
}; 


#endif  // INC_TEXTACTS


