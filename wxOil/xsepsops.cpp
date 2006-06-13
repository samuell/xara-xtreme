// $Header: /Camelot/winoil/xsepsops.cpp 2     23/03/04 12:01p Alex $
//
// XSEPSExportOptions - implements the options relevant to the export of XS EPS

/*
*/

#include "camtypes.h"
#include "xsepsops.h"
#include "printdlg.h"
#include "mainfrm.h"
#include "epsfiltr.h"
#include "markn.h"
#include "resource.h"
#include "helpuser.h"

DECLARE_SOURCE("$Revision: 2 $");

//---------------------------------

BEGIN_MESSAGE_MAP(XSEPSExportOptions,CDialog)
	//{{AFX_MSG_MAP(CPrintDialog)
	ON_COMMAND_EX(IDC_XSEPSOPSHELP,	OnCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(XSEPSExportOptions,CDialog)

//---------------------------------

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	XSEPSExportOptions::XSEPSExportOptions()

	Author:		Markn
	Created:	30/5/95
	Inputs:		-
	Returns:	-
	Purpose:	Constructs the dlg.
	SeeAlso:	-

********************************************************************************************/

XSEPSExportOptions::XSEPSExportOptions() : CDialog(IDD_XSEPSOPS, GetMainFrame())
{
}


/********************************************************************************************

>	BOOL XSEPSExportOptions::OnInitDialog()

	Author:		Markn
	Created:	30/5/95
	Inputs:		-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Inits the dialog's controls, and calls the base classes OnInitDialog() function
	SeeAlso:	-

********************************************************************************************/

BOOL XSEPSExportOptions::OnInitDialog()
{	
	if (CDialog::OnInitDialog())
	{
		TCHAR s[256];
		wsprintf(s, TEXT("%d"), (int) EPSFilter::XSEPSExportDPI);
		SetDlgItemText(IDC_DPIEDIT, s);

		/*
		CheckDlgButton(IDC_PSLEVELAUTO,	EPSFilter::XSEPSExportPSType == 0);
		CheckDlgButton(IDC_PSLEVEL1,	EPSFilter::XSEPSExportPSType == 1);
		CheckDlgButton(IDC_PSLEVEL2,	EPSFilter::XSEPSExportPSType == 2);
		*/

		CheckDlgButton(IDC_EXPORTTEXTASCURVES, EPSFilter::XSEPSExportTextAsCurves);

		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	void XSEPSExportOptions::OnOK()

	Author:		Markn
	Created:	31/5/95
	Inputs:		-
	Returns:	-
	Purpose:	Overrides the default OnOK() func so that we can check the values set by the user.
				If dodgy values have been put in, the user is warned.
	SeeAlso:	-

********************************************************************************************/

void XSEPSExportOptions::OnOK()
{
	// DPI ed field
	BOOL TranslatedOK;
	UINT DPI = GetDlgItemInt(IDC_DPIEDIT,&TranslatedOK,FALSE);
	if (!TranslatedOK || DPI < 10)
		DPI = 10;

	if (DPI > 600)
		DPI = 600;

	if (DPI > 300)
	{
		int b = InformWarning(IDS_EXPORT_BIGDPI,IDS_OK,IDS_CANCEL);
		if (b != 1)
			return;
	}

	/*
	// Job 10463: remove PS Level bits - default to Level 2
	int ps = 0;
	if (IsDlgButtonChecked(IDC_PSLEVEL1))
		ps = 1;
	if (IsDlgButtonChecked(IDC_PSLEVEL2))
		ps = 2;
	*/

	EPSFilter::XSEPSExportDPI 	 = DPI;
	// EPSFilter::XSEPSExportPSType = ps;

	EPSFilter::XSEPSExportTextAsCurves = IsDlgButtonChecked(IDC_EXPORTTEXTASCURVES);

	CDialog::OnOK();
}

/********************************************************************************************

>	BOOL XSEPSExportOptions::OnCommand()

	Author:		Markn
	Created:	1/6/95
	Inputs:		-
	Returns:	TRUE if all ok, FALSE if an error occurs
	Purpose:	Called when something happends to one of our controls

				Only used to trap Help button events, at the moment

	SeeAlso:	-

********************************************************************************************/

BOOL XSEPSExportOptions::OnCommand(UINT GadgetID)
{
	switch (GadgetID)
	{
		case IDC_XSEPSOPSHELP:
			HelpUser(*this);
			break;
	}

	return TRUE;
}

