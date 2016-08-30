#pragma once



// tab3Dlg 對話方塊

class tab3Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(tab3Dlg)

public:
	tab3Dlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~tab3Dlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonrobotfrount();
	afx_msg void OnBnClickedButtonconnect();
	afx_msg void OnBnClickedButtonBack();
};
