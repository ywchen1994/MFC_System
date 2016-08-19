#pragma once
#include"cv.h"
#include"highgui.h"
#include "afxwin.h"


struct CTab2threadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};


// tab2Dlg ��ܤ��

class tab2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(tab2Dlg)

public:
	tab2Dlg(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~tab2Dlg();

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩
	virtual BOOL OnInitDialog();
	void ShowImage(IplImage* Image, CWnd* pWnd, int channels,CvSize size);
	DECLARE_MESSAGE_MAP()
public:

	

	//thread
	CTab2threadParam m_threadPara;
	CWinThread*  m_lpThread;
	static UINT MythreadFun(LPVOID LParam);
	void Thread_Image_CannyRoi(LPVOID lParam);

	CStatic m_img_CannyRoi;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
