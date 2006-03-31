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

#include "camtypes.h"

#include "app.h"		//For GetApplication::FindSelection

#include "fimagdlg.h"
//#include "imagdlg.h"	//Imagemap dialog resources
//#include "resimmap.h"	//General imagemap resources

#include "dialogop.h"
#include "msg.h"

#include "winrect.h"	//For WinRect
#include "osrndrgn.h"	//For OSRenderRegion::BitmapDocRectToWin
#include "bitfilt.h"	//For BaseBitmapFilter::GetSizeOfDrawing
#include "bmpfiltr.h"	//For BMPFilter::GetDefaultExportDPI
#include "strlist.h"	//For class StringListItem

#include "xshelpid.h"	//For the help ID

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(ImagemapDlg, DialogOp)

#define new CAM_DEBUG_NEW

// Set up the dialog box details stored as statics in the class
CDlgResID ImagemapDlg::IDD = _R(IDD_EXPORT_IMAGEMAP);
const CDlgMode ImagemapDlg::Mode = MODAL;

BOOL ImagemapDlg::DontHandleNextMessage=FALSE;
BOOL ImagemapDlg::DialogWasCancelled=FALSE;

//And our static "Options" member variable
ImagemapFilterOptions ImagemapDlg::ms_Options=ImagemapFilterOptions();		
List* ImagemapDlg::ms_plstNames=NULL;		

/******************************************************************************************

>	BOOL ImagemapDlg::InvokeDialog(ImagemapFilterOptions* pifoDefault, List* plstNames) 


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		pifoDefault - The options that will be displayed when
						     the dialog starts

				plstNames	- A list of suggested imagemap names that
							  will be displayed when the dialog starts.
							  May be NULL.

	Outputs:	pifoDefault	- The options that the user entered
	Returns:	TRUE - OK was pressed
				FALSE - Cancel was pressed
	Purpose:	Starts up the Imagemap Export Options dialog box
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ImagemapDlg::InvokeDialog(ImagemapFilterOptions* pifoDefault, List* plstNames) 
{
	// Make a local copy of the options
	ms_Options=*pifoDefault;
	ms_plstNames=plstNames;

	//Say that the dialog wasn't cancelled
	DialogWasCancelled=FALSE;

	//Find the dialog's op descriptor
	OpDescriptor *OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(ImagemapDlg));

	//And start up the dialog
	if (OpDesc != NULL)
		OpDesc->Invoke();

	//And return the options we've got back
	*pifoDefault=ms_Options;

	return !DialogWasCancelled;
}


/******************************************************************************************

>	ImagemapDlg::ImagemapDlg(): DialogOp(ImagemapDlg::IDD, ImagemapDlg::Mode) 


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

ImagemapDlg::ImagemapDlg(): DialogOp(ImagemapDlg::IDD, ImagemapDlg::Mode) 
{

}

/******************************************************************************************

>	MsgResult ImagemapDlg::Message(CDlgMessage DlgMsg, CGadgetID Gadget) 


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all messages.

				This function simply maps the message to one of our message handling
				functions below.

	Errors:		-
	SeeAlso:	-

******************************************************************************************/

MsgResult ImagemapDlg::Message(Msg* Message)
{
	//If the message came from within our dialog 
	if (IS_OUR_DIALOG_MSG(Message))
	{
		//Then cast it into a dialog message and handle it
		DialogMsg* Msg = (DialogMsg*)Message;

		//If this flag gets set, we should close the dialog
		BOOL EndDialog = FALSE;

		//Now pass the message to a message handling function
		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				OnCreate();
				break;

			case DIM_COMMIT:
				OnCommit();
				EndDialog=TRUE;
				break;

			case DIM_CANCEL:
				DialogWasCancelled=TRUE;
				EndDialog = TRUE;
				break;

			case DIM_LFT_BN_CLICKED:
				// A control on the dialog box has been clicked...
				switch (Msg->GadgetID)
				{
					case _R(IDC_IMAGEMAP_AREA_SELECTION):
						OnAreaSelectionClicked();
						break;

					case _R(IDC_IMAGEMAP_AREA_DRAWING):
						OnAreaDrawingClicked();
						break;
				}
				break;

			case DIM_TEXT_CHANGED:
				// The user has typed in an edit field
				switch (Msg->GadgetID)
				{
					case _R(IDC_IMAGEMAP_WIDTH):
						if (ImagemapDlg::DontHandleNextMessage)
							ImagemapDlg::DontHandleNextMessage=FALSE;
						else
							OnWidthChanged();
						break;

					case _R(IDC_IMAGEMAP_HEIGHT):
						if (ImagemapDlg::DontHandleNextMessage)
							ImagemapDlg::DontHandleNextMessage=FALSE;
						else
							OnHeightChanged();
						break;
				}
				break;

 		}

		//And end the dialog if necessary
		if (EndDialog) 
		{
			Close();		
			End();			
		}
	}

	//Now pass the message down to the base class
	return DialogOp::Message(Message); 
}

/********************************************************************************************

	ImagemapDlg message handling functions

	The following group of functions are called from ImagemapDlg::Message
	when a message is received.

	They call other functions which do whatever needs to be done.

********************************************************************************************/


/********************************************************************************************

	void ImagemapDlg::OnCreate()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the dialog has just been created.

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::OnCreate()
{	
	//Set up the controls from our ImagemapFilterOptions member variables
	SetOptions(ms_Options);

	//Set up our internal member variables
	SetMemberVariables(ms_Options);

	//And put the keyboard focus in the Map Name field
	SetKeyboardFocus(_R(IDC_IMAGEMAP_MAPNAME));
}

/********************************************************************************************

	void ImagemapDlg::OnCommit()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the "Export" button has been clicked

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::OnCommit()
{	
	//Get the values from the dialog box and
	//put them into our ImagemapFilterOptions member variable
	ms_Options=GetOptions();
}

/********************************************************************************************

	void ImagemapDlg::OnAreaSelectionClicked()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the "Area for imagemap - Selection" button has
				been clicked.

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::OnAreaSelectionClicked()
{	
	//Make sure the "Area for imagemap - Drawing" button is off
	SetLongGadgetValue(_R(IDC_IMAGEMAP_AREA_DRAWING), FALSE);

	//And set the width and height fields to reflect the width
	//and height of the selection scaled by 96 DPI
	SetDPI(SELECTION, BMPFilter::GetDefaultExportDPI());
}

/********************************************************************************************

	void ImagemapDlg::OnAreaDrawingClicked()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the "Area for imagemap - Drawing" button has
				been clicked.

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::OnAreaDrawingClicked()
{	
	//Make sure the "Area for imagemap - Selection" button is off
	SetLongGadgetValue(_R(IDC_IMAGEMAP_AREA_SELECTION), FALSE);

	//And set the width and height fields to reflect the width
	//and height of the drawing scaled by 96 DPI
	SetDPI(DRAWING, BMPFilter::GetDefaultExportDPI());
}

/********************************************************************************************

	void ImagemapDlg::OnWidthChanged()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the user has typed in the "Width" field

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::OnWidthChanged()
{	
	//First we set up a flag to find if the value in the Width field
	//is a valid value
	BOOL fValid=TRUE;

	//Get the value of the width field and find out the value
	//is between 1 and the maximum for a INT32 type
	INT32 lWidth = GetLongGadgetValue(_R(IDC_IMAGEMAP_WIDTH), 1, PIXELWIDTH_MAX, NULL ,&fValid);

	//Set the height to zero by default
	INT32 lHeight=0;

	//If the width is valid...
	if (fValid)
	{
		//Then get the height by dividing our width by the width height
		//ratio, rounding to the nearest integer
		lHeight=(INT32) ((lWidth/m_dWidthHeightRatio)+0.5);	
	}

	//And set our height into the dialog
	ImagemapDlg::DontHandleNextMessage=TRUE;
	SetLongGadgetValue(_R(IDC_IMAGEMAP_HEIGHT), lHeight);
	
}

/********************************************************************************************

	void ImagemapDlg::OnHeightChanged()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the user has typed in the "Height" field

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::OnHeightChanged()
{	
	//First we set up a flag to find if the value in the Height field
	//is a valid value
	BOOL fValid=TRUE;

	//Get the value of the height field and find out the value
	//is between 1 and our maximum pixel width
	INT32 lHeight = GetLongGadgetValue(_R(IDC_IMAGEMAP_HEIGHT), 1, PIXELWIDTH_MAX, NULL ,&fValid);

	//Set the width to zero by default
	INT32 lWidth=0;

	//If the height is valid...
	if (fValid)
	{
		//Then get the width by multiplying our height by the width height
		//ratio, rounding to the nearest integer
		lWidth=(INT32) ((lHeight*m_dWidthHeightRatio)+0.5);	
	}

	//And set our width into the dialog
	ImagemapDlg::DontHandleNextMessage=TRUE;
	SetLongGadgetValue(_R(IDC_IMAGEMAP_WIDTH), lWidth);
}

/********************************************************************************************

	ImagemapDlg toolkit functions

********************************************************************************************/


/********************************************************************************************

	void ImagemapDlg::SetOptions(ImagemapFilterOptions ifoSet)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		ifoSet	The ImagemapFilterOptions to set as current within the dialog
	Outputs:	-
	Returns:	-
	
	Purpose:	Sets some ImagemapFilterOptions into the dialog

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::SetOptions(ImagemapFilterOptions ifoSet)
{	
	//First let's set the options which we should get from ifoSet.
	//These are the controls which remember the values which the user set
	//for the last export.

	//Set the "save clickable areas as rectangles"
	SetLongGadgetValue(_R(IDC_IMAGEMAP_ALLRECTANGLES), ifoSet.m_fAllRectangles);

	//Set the "copy imagemap to clipboard"
	SetLongGadgetValue(_R(IDC_IMAGEMAP_CLIPBOARD), ifoSet.m_fClipboard);

	//Set up the "Approximate curves with lines" drop down box
	DeleteAllValues(_R(IDC_IMAGEMAP_APPROX));

	SetStringGadgetValue(_R(IDC_IMAGEMAP_APPROX), &String_256(_R(IDS_IMAGEMAP_APPROX_VERYCLOSELY)), FALSE, 0);
	SetStringGadgetValue(_R(IDC_IMAGEMAP_APPROX), &String_256(_R(IDS_IMAGEMAP_APPROX_CLOSELY)), FALSE, 1);
	SetStringGadgetValue(_R(IDC_IMAGEMAP_APPROX), &String_256(_R(IDS_IMAGEMAP_APPROX_APPROXIMATELY)), FALSE, 2);
	SetStringGadgetValue(_R(IDC_IMAGEMAP_APPROX), &String_256(_R(IDS_IMAGEMAP_APPROX_NOTATALL)), FALSE, 3);
	
	SetComboListLength(_R(IDC_IMAGEMAP_APPROX));

	//And choose the appropriate value
	switch (ifoSet.m_ffApprox)
	{
	case FF_VERYCLOSELY:
		SetSelectedValueIndex(_R(IDC_IMAGEMAP_APPROX), 0);
		break;

	case FF_CLOSELY:
		SetSelectedValueIndex(_R(IDC_IMAGEMAP_APPROX), 1);
		break;

	case FF_APPROXIMATELY:
		SetSelectedValueIndex(_R(IDC_IMAGEMAP_APPROX), 2);
		break;

	default:
		ERROR3("ImagemapDlg::SetOptions - invalid flattening factor");
	
	case FF_NOTATALL:
		SetSelectedValueIndex(_R(IDC_IMAGEMAP_APPROX), 3);
		break;
	
		
		
	}

	//Now, let's set up the remaining controls with default values. These
	//controls do not remember their settings between exports

	//The "Map Name" combo box

	//First delete all the values that were previously in this box
	DeleteAllValues(_R(IDC_IMAGEMAP_MAPNAME));

	//Have we been passed a list of names to put in the combo box?
	if (ms_plstNames)
	{
		//Yes. So go through them, one by one, putting them into the combo box
		StringListItem* psliThis=(StringListItem*) ms_plstNames->GetHead();

		INT32 iListPos=0;

		while (psliThis!=NULL)
		{
			String_256 strToSet=psliThis->GetString();

			SetStringGadgetValue(_R(IDC_IMAGEMAP_MAPNAME), &strToSet, FALSE, iListPos);

			iListPos++;

			psliThis=(StringListItem*) ms_plstNames->GetNext(psliThis);
		}

		//And select the first item
		SetSelectedValueIndex(_R(IDC_IMAGEMAP_MAPNAME),0);

	}
	else
		//No. So simply set a default name into the combo box
		SetStringGadgetValue(_R(IDC_IMAGEMAP_MAPNAME), &String_256(_R(IDS_IMAGEMAP_DEFAULTMAPNAME)), FALSE,-1);



	SetComboListLength(_R(IDC_IMAGEMAP_MAPNAME));

	//Set the Area For Imagemap radio buttons according to whether there is a selection
	//or not
	
	//Is there a selection?
	SelRange* pSelRange=GetApplication()->FindSelection();
	
	if (pSelRange && !pSelRange->IsEmpty())
	{
		//Yes. So enable the selection radio button (in case it was disabled before)
		EnableGadget(_R(IDC_IMAGEMAP_AREA_SELECTION), TRUE);

		//And select it
		SetLongGadgetValue(_R(IDC_IMAGEMAP_AREA_SELECTION), TRUE);
		SetLongGadgetValue(_R(IDC_IMAGEMAP_AREA_DRAWING), FALSE);

		//And set the width and height fields to reflect the width
		//and height of the selection scaled by 96 DPI
		SetDPI(SELECTION, BMPFilter::GetDefaultExportDPI());
	}
	else
	{
		//No. So disable the selection radio button
		EnableGadget(_R(IDC_IMAGEMAP_AREA_SELECTION), FALSE);

		//And select the drawing radio button
		SetLongGadgetValue(_R(IDC_IMAGEMAP_AREA_SELECTION), FALSE);
		SetLongGadgetValue(_R(IDC_IMAGEMAP_AREA_DRAWING), TRUE);

		//And set the width and height fields to reflect the width
		//and height of the drawing scaled by 96 DPI
		SetDPI(DRAWING, BMPFilter::GetDefaultExportDPI());

	}
									
}

/********************************************************************************************

	ImagemapFilterOptions ImagemapDlg::GetOptions()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	The set of Imagemap Filter Options showing in the dialog
	Returns:	-
	
	Purpose:	Gets a set of imagemap filter options from the dialog

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

ImagemapFilterOptions ImagemapDlg::GetOptions()
{	
	//This variable will hold the imagemap filter options to return. Base these options
	//on the options we were given
	ImagemapFilterOptions ifoGet=ms_Options;

	//Get the map name
	ifoGet.m_strName=GetStringGadgetValue(_R(IDC_IMAGEMAP_MAPNAME));

	//Get the "Approximate curves with lines" drop down box
	switch (GetSelectedValueIndex(_R(IDC_IMAGEMAP_APPROX)))
	{
	case 0:
		ifoGet.m_ffApprox=FF_VERYCLOSELY;
		break;

	case 1:
		ifoGet.m_ffApprox=FF_CLOSELY;
		break;

	case 2:
		ifoGet.m_ffApprox=FF_APPROXIMATELY;
		break;

	default:
		ERROR3("ImagemapDlg::GetOptions - invalid flattening factor");

	case 3:
		ifoGet.m_ffApprox=FF_NOTATALL;
		break;

	}

	//Get the Area For Imagemap radio buttons
	if (GetLongGadgetValue(_R(IDC_IMAGEMAP_AREA_SELECTION), 0, 1)==1)
		ifoGet.m_stExportArea=SELECTION;
	else
		ifoGet.m_stExportArea=DRAWING;
													
	//Get the DPI from the Width and Height and the Area For Imagemap
	ifoGet.m_dDPI=GetDPI(ifoGet.m_stExportArea);

	//Get the "save clickable areas as rectangles"
	ifoGet.m_fAllRectangles=(GetLongGadgetValue(_R(IDC_IMAGEMAP_ALLRECTANGLES), 0, 1)==1);

	//Get the "copy imagemap to clipboard"
	ifoGet.m_fClipboard=(GetLongGadgetValue(_R(IDC_IMAGEMAP_CLIPBOARD), 0 ,1)==1);

	//And return our new set of imagemap options
	return ifoGet;
}

/********************************************************************************************

	void ImagemapDlg::SetDPI(SelectionType stExportArea, double dDPI)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		stExportArea	The current Area to Export
	Outputs:	-
	Returns:	-
	
	Purpose:	Sets the Width and Height fields according to the DPI
				and the Export Area.

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::SetDPI(SelectionType stExportArea, double dDPI)
{	
	//First we need to get the bounding box of the area to export
	DocRect rectBounds=ImagemapFilterOptions::GetSizeOfExportArea(stExportArea);

	//And scale it by the DPI we have been given. The easiest
	//way to do this is to use this function
	Matrix Identity;
	WinRect	Rect = OSRenderRegion::BitmapDocRectToWin( Identity, rectBounds, dDPI );

	//Then set the result into the width and height fields
	ImagemapDlg::DontHandleNextMessage=TRUE;
	SetLongGadgetValue(_R(IDC_IMAGEMAP_WIDTH), Rect.Width());

	ImagemapDlg::DontHandleNextMessage=TRUE;
	SetLongGadgetValue(_R(IDC_IMAGEMAP_HEIGHT), Rect.Height());
}

/********************************************************************************************

	double ImagemapDlg::GetDPI(SelectionType stExportArea)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		-
	Outputs:	-
	Returns:	The DPI based on the Width and Height fields
				and the export area
	
	Purpose:	Gets the DPI as shown in the dialog box by the Width
				and Height fields.

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

double ImagemapDlg::GetDPI(SelectionType stExportArea)
{	
	//This variable will hold the value to return
	//It is 96 by default
	double dDPI=BMPFilter::GetDefaultExportDPI();

	//First we need to get the bounding box of the area to export
	DocRect rectBounds=ImagemapFilterOptions::GetSizeOfExportArea(stExportArea);

	//So, get the width of the area to export in millipoints
	INT32 lMillipointWidth=rectBounds.Width();

	//Now get the value of the width fields, which will be
	//the width of the area to export in pixels
	INT32 lPixelWidth=GetLongGadgetValue(_R(IDC_IMAGEMAP_WIDTH), rectBounds.Width(), -INT32_MAX, INT32_MAX);

	//Is the width in pixels greater than zero?
	if (lPixelWidth>0)
	{
		//Yes. So get the DPI from the ratio between the pixel height and the pixel width,
		//rounded upwards
		dDPI=((double) lPixelWidth * 72000.0)/((double) lMillipointWidth);

		//Is that DPI within reasonable bounds?
		if (dDPI<0)
			dDPI=BMPFilter::GetDefaultExportDPI();

	}

	//And return that value
	return dDPI;
}

/********************************************************************************************

	void ImagemapDlg::SetMemberVariables(ImagemapFilterOptions ifoOptions)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		ifoOptions - a set of ImagemapFilterOptions
	Outputs:	-
	Returns:	-
	
	Purpose:	Sets up our internal member variables based on the contents
				of ifoOptions.

				At the moment, this function only sets the width/height ratio
				member variable (which contains the ratio of the width and
				height of the export area).

	Errors:		-
	SeeAlso:	ImagemapDlg::Message()

********************************************************************************************/

void ImagemapDlg::SetMemberVariables(ImagemapFilterOptions ifoOptions)
{	
	//Get the size of the area to export
	DocRect rectExportSize=ImagemapFilterOptions::GetSizeOfExportArea(ifoOptions.m_stExportArea);

	//Then get the ratio of its width and height
	m_dWidthHeightRatio=((double) rectExportSize.Width())/((double) rectExportSize.Height());
}







/******************************************************************************************

>	static OpState	ImagemapDlg::GetState(String_256*, OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ImagemapDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	ImagemapDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}




/******************************************************************************************

>	BOOL ImagemapDlg::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ImagemapDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ImagemapDlg::Init()
{
	BOOL InitOK;

	InitOK = RegisterOpDescriptor(
								0,					/* Tool ID */
								_R(IDS_IMAGEMAPDLG),
								CC_RUNTIME_CLASS(ImagemapDlg),
								OPTOKEN_IMAGEMAPDLG,
								GetState,
								_R(IDH_Dialog_Image_Map_Export_Options), /* help ID */
								0,  				/* bubble help */
								0,					/* resource ID */
								0					/* control ID */
								);

	return (InitOK);
}


/******************************************************************************************

>	void ImagemapDlg::Do(OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ImagemapDlg::Do(OpDescriptor*)
{
	BOOL ok;
	
	// Force the dialog box to be created, as it is modal it will be opened via a message
	ok = Create();

	if ( !ok )
	{
		// Could not create the dialog box so call inform error 
		InformError();
  		End(); 		
	}
}

/******************************************************************************************

>	void ImagemapDlg::DoWithParam(OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user and
				allows values to be passed in and returned to the caller via the
				ImagemapDlgParam class.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ImagemapDlg::DoWithParam(OpDescriptor*, OpParam* Param)
{
	BOOL ok;
	
	// Force the dialog box to be created, as it is modal it will be opened via a message
	ok = Create();

	if ( !ok )
	{
		// Could not create the dialog box so call inform error 
		InformError();
  		End(); 		   // End the operation 
	}
}
