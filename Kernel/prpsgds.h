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
// Header for the handling code for the Tuneup tab of the options dialog box 

// WEBSTER - markn 15/1/97
// No need for guide properties tab
#ifdef WEBSTER
#define INC_PROPERTIESGUIDES
#endif

#ifndef INC_PROPERTIESGUIDES
#define INC_PROPERTIESGUIDES

//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layerprp.h"
#include "snap.h"
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "undoop.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class NodeGuideline;
class Layer;
class ColourDropDown;
class IndexedColour;

/********************************************************************************************

>	class GuidelineListItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Purpose:	Used to generate a sorted list of guidelines within a layer
	SeeAlso:	-

********************************************************************************************/

class GuidelineListItem : public ListItem
{
public:
	GuidelineListItem(NodeGuideline* pTheGuideline) { pGuideline = pTheGuideline; }

	NodeGuideline* pGuideline;
};

/********************************************************************************************

>	class GuidesPropertiesTab: public LayerProperties

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Purpose:	Allows the user to set various layer properties, such as:-

	SeeAlso:	LayerProperties; LayerProperties;

********************************************************************************************/

class GuidesPropertiesTab : public LayerPropertyTabs
{         
	CC_DECLARE_DYNCREATE( GuidesPropertiesTab )  
public:

	GuidesPropertiesTab();
	~GuidesPropertiesTab();

	virtual BOOL Init();                        

	// Function to handle the messages for this tab/pane window
	virtual BOOL HandleMsg(DialogMsg* Msg);

	// Function to set up the values on this tab when being opened
	virtual BOOL InitSection();

	// Function to commit the values on this tab on ok
	virtual BOOL CommitSection();

	// Called when layer-related UI needs updating
	virtual BOOL UpdateLayerSection();

	// Called when guideline-related UI needs updating
	virtual BOOL UpdateGuidelineSection();

	// Called when the colour list changes
	virtual BOOL ColourListChanged(Document* pDoc);

	// Functions to handle document changing messages
	virtual BOOL GreySection();			// Called when the user has closed all documents
	virtual BOOL UngreySection();		// Called when the user has opened a document
	BOOL ChangeControlStatus(const BOOL Status); // used to grey/ungrey all controls
	virtual BOOL UpdateSection();		// Called when the user has switched to a new document

	virtual CDlgResID GetPageID();		// Return section/page id system

	// Some functions to determine whether this particular property tab is required.
	virtual BOOL IsPropertyRequired();			// Return True if required

protected:

private:
	BOOL ShowDetails();
	BOOL ShowGuidelines(Layer* pLayer);
	BOOL ShowColours();

	BOOL PropertiesClicked();
	BOOL NewClicked();
	BOOL DeleteClicked();
	BOOL ColourChanged(INT32 Index);

	BOOL BuildGuidelineList(Layer* pLayer);
	//NodeGuideline* 	FindClosestGuideline(MILLIPOINT UserOrdinate);

	BOOL 			GreyStatus;					// Remember the state that we are currently in
	GuidelineType	GuideType;					// The type of guideline to show in the guideline list
	ColourDropDown*	pColourDropDown;
	List 			GuidelineList;
}; 

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/********************************************************************************************

>	class OpChangeLayerColourParam : public OpParam

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Purpose:	Param used to change the colour of a layer
	SeeAlso:	OpChangeLayerColour

********************************************************************************************/

class OpChangeLayerColourParam : public OpParam
{
	CC_DECLARE_MEMDUMP(OpChangeLayerColourParam)

public:
	OpChangeLayerColourParam() { pDoc = NULL; pLayer = NULL; pColour = NULL; }

	OpChangeLayerColourParam(Document* pTheDoc,Layer* pTheLayer,IndexedColour* pTheColour)
	{
		pDoc	= pTheDoc;
		pLayer 	= pTheLayer;
		pColour = pTheColour;
	}	

	OpChangeLayerColourParam(OpChangeLayerColourParam& Other)
	{
		pDoc 	= Other.pDoc;
		pLayer 	= Other.pLayer;
		pColour = Other.pColour;
	}

	Document*		pDoc;
	Layer* 			pLayer;
	IndexedColour* 	pColour;
};


/********************************************************************************************

>	class OpChangeLayerColour : public UndoableOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Purpose:	Allows layers to have their colour changed

********************************************************************************************/

class OpChangeLayerColour : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(OpChangeLayerColour);

public:
	#define OPTOKEN_CHANGELAYERCOLOUR _T("ChangeLayerColour")
	
	// Construction/Destruction
	OpChangeLayerColour();
	~OpChangeLayerColour();

	virtual void DoWithParam(OpDescriptor* pOpDesc,OpParam* pOpParam);

	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);
	void GetOpName(String_256* OpName);

private:
	UINT32	UndoIDS;
};


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/********************************************************************************************

>	class LayerColourAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Purpose:	An action which changes the colour of a guide layer
	SeeAlso:	-

********************************************************************************************/

class LayerColourAction : public Action
{

CC_DECLARE_DYNCREATE(LayerColourAction)

public:
	LayerColourAction();
	~LayerColourAction();
	virtual ActionCode Execute();
	static ActionCode Init( UndoableOperation* pOp,
							ActionList* pActionList,
							OpChangeLayerColourParam Param);
protected:
	OpChangeLayerColourParam Param;
};


#endif // INC_PROPERTIESGUIDES
