#pragma once
#include "zbar.h"  

struct CTab3threadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};

// tab3Dlg ��ܤ��

class tab3Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(tab3Dlg)

public:
	tab3Dlg(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~tab3Dlg();

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonrobotfrount();
	afx_msg void OnBnClickedButtonconnect();
	afx_msg void OnBnClickedButtonBack();


	//thread
	CTab3threadParam m_threadPara;
	CWinThread*  m_lpThread;
	static UINT MythreadFun(LPVOID LParam);
 
};
