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
// Header for the handling code for the Page tab of the options dialog box 

#ifndef INC_OPTSGRID
#define INC_OPTSGRID

#include "unittype.h"
#include "dialogop.h"
#include "units.h"
#include "appprefs.h"

//#include "undoop.h"
#include "transop.h"
#include "grid.h" 

#define OPTOKEN_OPGRIDRESIZE  _T("OpGridResize")    

typedef struct
{
	Spread 	*pSpread;					// The spread to be changed

	// Grid resize parameters
	double GridSpacing;					// The new grid spacing 
	double GridDivisions;				// The new number of grid divisions 
	UINT32 GridSubDivisions;				// The new number of grid subdivisions
	UnitType GridUnits;					// The new grid units
 	GridType TypeOfGrid;				// The new grid type

	MILLIPOINT OriginX;					// The new grid origin x
	MILLIPOINT OriginY;					// The new grid origin y

} GridResizeInfo;


/********************************************************************************************

>	class GridTab : public OptionsTabs

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/10/94
	Purpose:	Allows the user to set any options which are normally associated
				with the document's default grid and hence the page rulers.
				This includes such things as:-
					Grid and rulers origin
					Grid and rulers spacing and divisions
					Grid type

	SeeAlso:	OptionsTabs; AppPrefsDlg;

********************************************************************************************/

class GridTab: public OptionsTabs
{         
	CC_DECLARE_DYNCREATE( GridTab )  
public:

	GridTab();
	~GridTab();

	virtual BOOL Init();                        

	// Function to handle the messages for this tab/pane window
	virtual BOOL HandleMsg(DialogMsg* Msg);

	// Function to set up the values on this tab when being opened
	virtual BOOL InitSection();

	// Function to commit the values on this tab on ok
	virtual BOOL CommitSection();

	// Function called when new default units come into operation.
	virtual BOOL NewUnitsInSection();

	// Functions to handle document changing messages
	virtual BOOL GreySection();			// Called when the user has closed all documents
	virtual BOOL UngreySection();		// Called when the user has opened a document
	BOOL ChangeControlStatus(const BOOL Status); // used to grey/ungrey all controls
	virtual BOOL UpdateSection(String_256 *DocumentName);	// Called when the user has switched to a new document

	BOOL UpdateGridSection();			// Called when the user has undone a default grid resize

	virtual CDlgResID GetPageID();		// Return section/page id system

	virtual BOOL IsDocumentOption();	// Return True if document option  

	virtual BOOL IsProgramOption();		// Return True if program option

protected:
	BOOL InitGridSection();

public:
	// New function for doing the default grid resize as one operation.
	BOOL InvokeResize(GridResizeInfo *Param);

private:
	double OldGridSpacing; 		// entry default page grid divisions
	UINT32 OldGridSubDivisions;	// entry default page grid subdivisions
	UnitType OldGridUnits;		// entry default page grid units
	GridType OldTypeOfGrid;		// entry default grid type

	MILLIPOINT OldOriginX;		// entry grid origin x
	MILLIPOINT OldOriginY;		// entry grid origin y

	BOOL GreyStatus;			// Remember the state that we are currently in
}; 


/****************************************************************************

>	class OpGridResize: public UndoableOperation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Purpose:	This class represents the Undo-able operation to resize the
				page and spread.
	SeeAlso:	ActionGridResize

****************************************************************************/
#define OPTOKEN_GRIDRESIZE _T("GridResize")	// Optoken for the grid resize operation

class OpGridResize: public UndoableOperation
{         
	CC_DECLARE_DYNCREATE(OpGridResize)

public:
	OpGridResize();	
	~OpGridResize();	

	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);

	void	Do(OpDescriptor *);
	void	DoWithParam(OpDescriptor* pOp, OpParam* pResizeParam);	     

protected:
	BOOL DoResizeGrid(GridResizeInfo* pResizeInfo);

private:
	DocRect PasteboardRect;

	MILLIPOINT OriginX;			// entry grid origin x
	MILLIPOINT OriginY;			// entry grid origin y
}; 


/********************************************************************************************

>	class ActionSetDefaultGrid: public Action

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Purpose:	When executed, this sets up an action to redo/undo a default grid resize.
	
	SeeAlso:	OpSetDefaultGrid

********************************************************************************************/

class CCAPI ActionSetDefaultGrid : public Action
{
	CC_DECLARE_DYNCREATE( ActionSetDefaultGrid )

public:
	ActionSetDefaultGrid();
	~ActionSetDefaultGrid();
	virtual ActionCode Execute();
	static ActionCode Init(Operation* const pOp,
							ActionList *pActionList,
							UINT32 ActionSize,
							Spread *pSpread,
							double GridDivisions,
							UINT32 GridSubDivisions,
							UnitType GridUnits,
							GridType TypeOfGrid,
							MILLIPOINT OriginX,
							MILLIPOINT OriginY,
							Action **NewAction);

protected:

private:
	Spread *pSpread;
	double GridDivisions;
	UINT32 GridSubDivisions;
	UnitType GridUnits;
	GridType TypeOfGrid;

	MILLIPOINT OriginX;			// entry grid origin x
	MILLIPOINT OriginY;			// entry grid origin y
};


#endif // INC_OPTSGRID
