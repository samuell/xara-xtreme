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

#ifndef INC_OPTSPAGE
#define INC_OPTSPAGE

//#include "unittype.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "units.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "appprefs.h"

//#include "undoop.h"
#include "transop.h"
#include "grid.h" 

#define OPTOKEN_OPPAGERESIZE  _T("OpPageResize")    

typedef struct
{
	Spread 	*pSpread;					// The spread to be changed

	// Page resize parameters
	MILLIPOINT Width;					// The required new page width
	MILLIPOINT Height;					// The required new page height
	MILLIPOINT Margin;					// The required new margin around the spread
	MILLIPOINT Bleed;					// The required new spread bleed
	BOOL Portrait;						// The portrait state (Not reaaly used apart from updating old value)
	BOOL Dps;							// The required new single/double page
										// spread state
	BOOL ShowDropShadow;				// The required new page shadow state

	BOOL ResizeAndCentre;				// Flag a resize and centre operation

} PageResizeInfo;


/********************************************************************************************

>	class PageTab: public OptionsTabs

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	Allows the user to set any options which are normally associated
				with the document's page. This includes such things as:-
					The size of the page as either a predefined page size or
					A custom page with explicit width/height control.
					Choice of Portrait or Landscape.
					Bleed size.
					Show page shadow switch

	SeeAlso:	OptionsTabs; AppPrefsDlg; DocPrefsDlg; PageSize; PageSizesList;

********************************************************************************************/

class PageTab: public OptionsTabs
{         
	CC_DECLARE_DYNCREATE( PageTab )  
public:

	PageTab();
	~PageTab();

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

	BOOL UpdatePageSection();			// Called when the user has undone a page resize

	virtual CDlgResID GetPageID();		// Return section/page id system

	virtual BOOL IsDocumentOption();	// Return True if document option  

	virtual BOOL IsProgramOption();		// Return True if program option

protected:
	// Lots of controlling and handling functions specific to this section
	BOOL InitPageSection();

	BOOL SetUpPageSizeList();

	INT32	 GetPageSizeItem(const MILLIPOINT Width, const MILLIPOINT Height);	// translates width/height to list position
	BOOL HandlePageList(INT32 ItemClicked, BOOL Portrait);// sets up height/width fields
	BOOL PortraitSelected;
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	BOOL HandlePortraitLandscape();		// Handles portrait/Landscape switches
#endif //webster
 	// finds out the page details and allows setting of new details
	BOOL GetSizeOfPage(MILLIPOINT *Width, MILLIPOINT *Height, MILLIPOINT *Margin, MILLIPOINT *Bleed,
					   BOOL *Portrait, BOOL *Dps, BOOL *ShowDropShadow);
//	BOOL SetNewSizeOfPage(MILLIPOINT Width, MILLIPOINT Height, MILLIPOINT Margin, MILLIPOINT Bleed,
//					   BOOL Portrait, BOOL Dps, BOOL ShowDropShadow);

public:
	// New function for doing the page resize and change of the default grid as one operation.
	BOOL InvokeResize(PageResizeInfo *Param);
	BOOL InvokeResizeAndCentre(PageResizeInfo *Param);

private:
//	DocRect OldPageRect;		// entry state page size and position
	MILLIPOINT OldPageWidth;	// entry page width
	MILLIPOINT OldPageHeight;	// entry page height
	BOOL OldDpsState;			// entry state of dps
	BOOL OldPortraitState;		// entry portrait/landscape state
	MILLIPOINT OldBleedSize;	// entry page bleed size
	MILLIPOINT OldMarginSize;	// entry page bleed size
	BOOL OldDropShadowState;	// entry drop shadow state 
	
	BOOL GreyStatus;			// Remember the state that we are currently in

	BOOL InitedPageSizeList;	// Is the page size list set up
}; 





/****************************************************************************

>	class OpPageResize: public UndoableOperation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/95
	Purpose:	This class represents the Undo-able operation to resize the
				page and spread.
	SeeAlso:	ActionPageResize

****************************************************************************/
#define OPTOKEN_PAGERESIZE _T("PageResize")	// Optoken for the page resize operation



class OpPageResize: public TransOperation //UndoableOperation
{         
	CC_DECLARE_DYNCREATE(OpPageResize)

public:
	OpPageResize();	
	~OpPageResize();	

	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);

	void	Do(OpDescriptor *);
	void	DoWithParam(OpDescriptor* pOp, OpParam* pResizeParam);	     

	//virtual BOOL Undo();
	//virtual BOOL Redo();

private:
	// Function to try and reposition all of the objects on the new page structure
	BOOL MoveObjectsOnPage(Spread *pSpread,
						   MILLIPOINT Width, MILLIPOINT Height,
						   MILLIPOINT Margin, BOOL Dps,
						   MILLIPOINT OldWidth, MILLIPOINT OldHeight,
						   MILLIPOINT OldMargin, BOOL OldDps,
						   Range  *pSearchRange,
						   BOOL ResizeAndCentre);

	DocRect PasteboardRect;
}; 

/********************************************************************************************

>	class ActionPageResize: public Action

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95
	Purpose:	When executed, this sets up an action to redo/undo a page resize.
	
	SeeAlso:	OpPageResize

********************************************************************************************/

class CCAPI ActionPageResize : public Action
{
	CC_DECLARE_DYNCREATE( ActionPageResize )

public:
	ActionPageResize();
	~ActionPageResize();
	virtual ActionCode Execute();
	static ActionCode Init(Operation* const pOp,
							ActionList *pActionList,
							UINT32 ActionSize,
							Spread *pSpread,
							MILLIPOINT Width,
							MILLIPOINT Height,
							MILLIPOINT Margin,
							MILLIPOINT Bleed,
							BOOL Dps,
							BOOL ShowDropShadow,
							Action **NewAction);

protected:

private:
	Spread *pSpread;

	DocRect PasteboardRect;
	DocRect PageRect;

	MILLIPOINT Width;					// page width
	MILLIPOINT Height;					// page height
	MILLIPOINT Margin;					// margin around the spread
	MILLIPOINT Bleed;					// spread bleed
	BOOL Dps;							// single/double page spread state
	BOOL ShowDropShadow;				// page shadow state
};

#endif // INC_OPTSPAGE
