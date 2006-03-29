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
// 
// Header file for AppPrefs.cpp
//
// 

#ifndef INC_PropertyTabsS
#define INC_PropertyTabsS

#include "dialogop.h"
#include "unittype.h"
#include "units.h"		// units e.g. Millimeters

/********************************************************************************************

>	class PropertyTabs : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	28/9/94
	Purpose:	These encompass some useful functionality which is required to get a property
				dialog tab up and running. This is the baseclass and should always be derived
				off from. Tabs currently available are:-
					- layer properties dialog box

	SeeAlso:	LayerPropertyTabs

********************************************************************************************/

class CCAPI PropertyTabs : public ListItem
{
	CC_DECLARE_DYNAMIC(PropertyTabs);

public:
	PropertyTabs();							// Constructor

	virtual BOOL Init();					// Initialisation

	// Function to declare a tab handling class to the property tabs system
//	static BOOL DeclarePropertyTab(PropertyTabs *pProperty);
//	static BOOL UndeclarePropertyTab(PropertyTabs *pProperty);
//
//	static PropertyTabs *GetFirst();
//	static PropertyTabs *GetNext(PropertyTabs*);

	virtual BOOL SetUpDocUnits();					// function to set up CurrentPageUnits

	static Document * pScopeDocument;				// current document (for message broadcasts) 

	virtual void SetCurrentDocAndSpread(Document *pDoc);	// set up the spread that we are currently workign on
	virtual void SetDefaultUnits();	  						// set up our default display units

	//MsgResult HandleMessage(Msg* Message);
	virtual DialogTabOp * GetTabbedDlg()			= 0;	// return pointer to the main dialog box

	//virtual BOOL SetTabbedDlg(DialogTabOp * pDlg)	= 0;	// return pointer to the main dialog box

	// Overridable by the tab section handlers. These are the must be specified functions
	// and so are pure.
	virtual CDlgResID GetPageID()					= 0;	// Return section/page id system
	virtual	BOOL InitSection()						= 0;	// Called when page first opened
	virtual BOOL CommitSection()					= 0;	// Called when ok action is required
	virtual BOOL HandleMsg(DialogMsg* Msg)			= 0;	// Called when messages arrive

	// Public access to the current layer.
	virtual void SetActiveLayer(Layer * pNewLayer);
	virtual Layer * GetActiveLayer();

	// Public access to the guide layer.
	virtual Layer * GetGuideLayer();

	// Public access to the current layer.
	virtual void SetCurrentLayer(Layer * pNewLayer);
	virtual Layer * GetCurrentLayer();

	// Some special handlers for things like new default units being specified or the last
	// document being closed.
	// These are the virtual handlers which each tab should specify, if not required then
	// these base class definitions can be used.
	virtual BOOL NewUnitsInSection();			// Called when new default units chosen
	virtual BOOL GreySection();					// Called when the user has closed all documents
	virtual BOOL UngreySection();				// Called when the user has opened a document
	virtual BOOL UpdateSection();				// Called when the user has switched to a new document

	// Virtual functions used to indicate that details on the current spread and current layer need updating
	virtual BOOL UpdateSpreadSection();			// Called when the user has switched to a new spread
	virtual BOOL UpdateLayerSection();			// Called when the user has switched to a new layer
	virtual BOOL UpdateGuidelineSection();		// Called when the user has changed guidelines on the active layer

	// Colour list functions
	virtual BOOL ColourListChanged(Document* pDoc);

	// Some functions to determine whether this particular property tab is required.
	virtual BOOL IsPropertyRequired();			// Return True if required


	// Functions for getting the message and appply now state for this tabbed dialog box
	virtual BOOL GetApplyNowState()						= 0;	// Get current state of ApplyNow flag
	virtual BOOL SetApplyNowState(BOOL NewState)		= 0;	// set new state for ApplyNow 

	virtual BOOL SetInitMessageState(BOOL NewState)		= 0;	// set new state of InitMessage
	virtual BOOL GetInitMessageState()					= 0;	// get current state of InitMessage

	virtual BOOL GreyApplyNow();						// grey the apply now button
	virtual BOOL UngreyApplyNow();						// grey the apply now button

	virtual BOOL SetOkState(BOOL Val);					// Set the state of the OK button on the tabbed dialog.

	Spread*	GetSpread()									{ return  pSpread; }

protected:
	// Allow the options tabs access but nobody else to these useful variables
	static Document *pDocument;					// The document we are working on
	static Spread   *pSpread; 					// The spread we are working on 

	static UnitType CurrentPageUnits;			// The units used to display page measurements

private:
	static Layer	*pActiveLayer;				// The active layer we are working on 
	static Layer 	*pCurrentLayer; 			// pointer to the layer to be used,

	// The list of property tabs that is understood.
	//static List PropertyTabsList;		// list of available tab handlers
};

#endif  // INC_PropertyTabsS


