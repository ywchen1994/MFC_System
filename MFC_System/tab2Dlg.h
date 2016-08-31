#pragma once
#include"cv.h"
#include"highgui.h"
#include "afxwin.h"
#include"stdlib.h"
#include"math.h"



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

	void ApproxPoly(IplImage * img_roi);
	void findinside(IplImage * Img);
	int grabDecision(int pictureSelcet, CvPoint3D32f* pushPoint, float* degree);
	void SpecilGrabDecision(int pictureSelcet, CvPoint3D32f* pushPoint, float* degree);

	CvPoint GetClosestPoint(IplImage * src, CvPoint center);

	void sequencePoint(CvPoint* corner, CvPoint center, CvPoint2D32f* outside);
	
	int priority = 0;
	CvPoint3D32f m_pushPoint[2];
	float m_degree;

	int ObjectCounter=0;

	void CornerDetection(IplImage * edge_roi, IplImage * CornerImg_Modified, int* cornerCount);
	void HarrisCornerToPoint(IplImage * Cornerimage, IplImage * dst, int* cornerCount);
	CvPoint GetCenter(IplImage * src);


	void Img2SCARA(int x, int y, float * SCARAX, float * SCARAY, float * SCARAZ);

	CvPoint	CornerPoint[10];
	
	
	float phi[4];
	int FindElement(float fitemp);

	int caseClassify(CvPoint3D32f * objPoint);

	float getDegree(CvPoint3D32f first, CvPoint3D32f second);

	int findHighestSide(CvPoint3D32f* objPoint);
	
	CvPoint3D32f extendPoint(CvPoint3D32f first, CvPoint3D32f second, int value);

	

	afx_msg void OnBnClickedButtonstartgrab();
	afx_msg void OnBnClickedButtonautobinpick();
	int m_priorityShow;
	CListBox m_list_detectNum;
	afx_msg void OnLbnSelchangeListdetectnum();
	CListBox m_list_priority;
	afx_msg void OnBnClickedButtontoppriority();
	afx_msg void OnBnClickedButtongopushpoint1();
	afx_msg void OnBnClickedButtongopushpoint2();
	afx_msg void OnBnClickedButtongopushpoint3();
	afx_msg void OnBnClickedButtondebugmode();
};
