/*#pragma once


// ManualDPIDialog dialog

class ManualDPIDialog : public CDialog
{
	DECLARE_DYNAMIC(ManualDPIDialog)

public:
	ManualDPIDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ManualDPIDialog();
// Dialog Data
	enum { IDD = IDD_MANUAL_DPI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeMeasureBox();

	enum MeasurementType {INCHES = 3, MILLIMETERS = 1, PIXELS = 0};
	MeasurementType measurement;
};*/
#pragma once


// ManualDPIDialog dialog

class ManualDPIDialog : public CDialog
{
	DECLARE_DYNAMIC(ManualDPIDialog)

public:
	ManualDPIDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ManualDPIDialog();

// Dialog Data
	enum { IDD = IDD_MAN_DPI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnSelchangeMeasureBox();

	enum MeasurementType {INCHES = 3, CENTIMETERS = 2, MILLIMETERS = 1, PIXELS = 0, CANCEL = -1};
	MeasurementType measurement;
	afx_msg void OnBnClickedCancel();
};
