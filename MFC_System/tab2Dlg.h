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


// tab2Dlg 對話方塊

class tab2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(tab2Dlg)

public:
	tab2Dlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~tab2Dlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
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
	CStatic m_img_approxPoly;

	void ImageProcessing(IplImage * img_roi);
	void ApproxPoly(IplImage * img_roi);
	void findinside(IplImage * Img);
	CComboBox m_combo_objList;
	afx_msg void OnCbnSelchangeComboobjlist();
	int ObjectCounter=0;

	void CornerDetection(IplImage * edge_roi, IplImage * CornerImg_Modified,int radius);
	void CornerFind(IplImage * Cornerimage, IplImage * dst, int radius);
	CvPoint GetCornerPoint(IplImage * src, IplImage * dst, int radius);
	CvPoint GetCenterPoint(IplImage * src);
	int CornerCounter = 0;
	CvPoint	CornerPoint[10];


};
