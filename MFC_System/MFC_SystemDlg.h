
// MFC_SystemDlg.h : 標頭檔
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include"..//Kinect2Capture//Kinect2Capture.h"

#include "tab1Dlg.h"
#include "tab2Dlg.h"
#include "tab3Dlg.h"

#include "CvvImage.h"
#include"cv.h"
#include"highgui.h"
struct CMythreadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};
// CMFC_SystemDlg 對話方塊

class tab1Dlg;
class tab2Dlg;
class tab3Dlg;
class CMFC_SystemDlg : public CDialogEx
{
// 建構
public:
	CMFC_SystemDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_SYSTEM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	void ShowImage(IplImage * Image, CWnd * pWnd, int channels);
	/*******************************/
	static UINT MythreadFun(LPVOID LParam);
	void Thread_Image_Depth(LPVOID lParam);
	void Thread_Image_RGB(LPVOID lParam);
	CMythreadParam m_threadPara;
	CWinThread*  m_lpThread;
	/***************static******************/
	static IplImage* img_RgbS;
	static IplImage* img_DepthS;
	static IplImage* img_CannyS;
	static IplImage* img_CannyRoiS;
	/**********************************/
	CStatic m_Img_RGBLive;
	CStatic m_Img_DepthLive;
	/************************Tab***********************************/
	CTabCtrl m_Tab;
	tab1Dlg m_TabPage1;
	tab2Dlg m_TabPage2;
	tab3Dlg m_TabPage3;
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	/**************************************************************/
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	//new origin
};
