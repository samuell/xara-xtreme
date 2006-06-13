// $Header: /camelot/winoil/XSEPSOPS.H 3     1/06/95 14:03 Markn $
// Header for dlg that controls the XS EPS options

#ifndef INC_XSEPSOPS
#define INC_XSEPSOPS

/********************************************************************************************

>	class XSEPSExportOptions : public CDialog

	Author:		Markn
	Created:	26/5/95
	Purpose:	This gets options relevant to the export of XS EPS
	SeeAlso:	

********************************************************************************************/

class XSEPSExportOptions : public CDialog
{
public:
	XSEPSExportOptions::XSEPSExportOptions();
	virtual ~XSEPSExportOptions() { }

	virtual void OnOK();
	virtual BOOL OnInitDialog();

protected:
	//{{AFX_MSG(XSEPSExportOptions)
	afx_msg BOOL OnCommand(UINT GadgetID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DECLARE_DYNAMIC(XSEPSExportOptions)
};


#endif  // INC_XSEPSOPS
