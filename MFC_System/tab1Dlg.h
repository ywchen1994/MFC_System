#pragma once
#include "afxwin.h"
#include"cv.h"
#include"highgui.h"
// tab1Dlg 對話方塊
struct CTab1threadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};
class tab1Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(tab1Dlg)

public:
	tab1Dlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~tab1Dlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void ShowImage(IplImage * Image, CWnd * pWnd, int channels);
	int m_XPos;
	int m_YPos;
	CStatic m_Img_Canny;
	static IplImage *CannyRoi;
	int PointNum=0;
	CvPoint RoiPoint[4];
	/******************************/
	CTab1threadParam m_threadPara;
	CWinThread*  m_lpThread;
	static UINT MythreadFun(LPVOID LParam);
	void Thread_Image_Canny(LPVOID lParam);
	/**********************************/
	void SetRoI(IplImage * img_edge);
	IplImage* CannyRoiC3;
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
