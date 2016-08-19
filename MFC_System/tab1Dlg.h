#pragma once
#include "afxwin.h"
#include"cv.h"
#include"highgui.h"



// tab1Dlg ��ܤ��
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
	tab1Dlg(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~tab1Dlg();

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	
	void ShowImage(IplImage * Image, CWnd * pWnd, int channels);
	int m_XPos;
	int m_YPos;
	CStatic m_Img_Canny;
	int PointNum=0;
	static CvPoint RoiPoint[2];
	/******************************/
	CTab1threadParam m_threadPara;
	CWinThread*  m_lpThread;
	static UINT MythreadFun(LPVOID LParam);
	void Thread_Image_Canny(LPVOID lParam);
	/**********************************/
	void SetRoI(IplImage * img_edge);
	
	
	//afx button
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
