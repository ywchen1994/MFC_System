#pragma once
#include "zbar.h"  
#include "cv.h" 
#include "highgui.h"  
#include "afxwin.h"

struct CTab3threadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};

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
	afx_msg
		void ShowImage(IplImage * Image, CWnd * pWnd, int channels, CvSize size);
	void OnBnClickedButtonconnect();
	afx_msg void OnBnClickedButtonBack();


	//thread
	CTab3threadParam m_threadPara;
	CWinThread*  m_lpThread;
	static UINT MythreadFun(LPVOID LParam);
	void Thread_Image_RGB(LPVOID lParam);
	void Thread_goRobot(LPVOID lParam);
	static bool FLAG_open;
	static int Xpos;
	static int Ypos;
	static int Ypos_Stop;

	CListBox m_listBox_msg;
	afx_msg void OnBnClickedButtonstart();
	afx_msg void OnBnClickedButtonstop();
	afx_msg void OnBnClickedButtongorobot();
	afx_msg void OnBnClickedButtonsety();
};
