// tab1Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "tab1Dlg.h"
#include "afxdialogex.h"
#include"MFC_SystemDlg.h"
IplImage*tab1Dlg::CannyRoi = cvCreateImage(cvSize(512,424),IPL_DEPTH_8U,1);
// tab1Dlg 對話方塊

IMPLEMENT_DYNAMIC(tab1Dlg, CDialogEx)

tab1Dlg::tab1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_tab1, pParent)
	, m_XPos(0)
	, m_YPos(0)
{

}

tab1Dlg::~tab1Dlg()
{
}

void tab1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_XPos, m_XPos);
	DDX_Text(pDX, IDC_STATIC_YPos, m_YPos);
	DDX_Control(pDX, IDC_IMAGE_Canny, m_Img_Canny);
}


BEGIN_MESSAGE_MAP(tab1Dlg, CDialogEx)
	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

BOOL tab1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Img_Canny.SetWindowPos(NULL, 10, 10, 512, 424, SWP_SHOWWINDOW);
	return TRUE;
}


void tab1Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (point.x > (10) && point.x < (10 + 512) && point.y > 10 && point.y < (10 + 424))
	{
		
		m_XPos = point.x-10;
		m_YPos = point.y-10;
		UpdateData(false);
		if (nFlags == MK_LBUTTON)
		{
			RoiPoint[0] = cvPoint(m_XPos, m_YPos);
		}
		CDialogEx::OnMouseMove(nFlags, point);
	}
	
}



void tab1Dlg::ShowImage(IplImage * Image, CWnd * pWnd, int channels)
{
	CDC	*dc = pWnd->GetWindowDC();
	IplImage *Temp = NULL;

		Temp = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, channels);
		cvResize(Image, Temp, CV_INTER_LINEAR);
	
	

	CvvImage Temp2;
	Temp2.CopyOf(Temp);
	Temp2.Show(*dc, 0, 0, Temp->width, Temp->height);
	cvReleaseImage(&Temp);
	ReleaseDC(dc);
}

UINT tab1Dlg::MythreadFun(LPVOID LParam)
{
	CTab1threadParam* para = (CTab1threadParam*)LParam;
	tab1Dlg* lpview = (tab1Dlg*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_Image_Canny(LParam);
	
	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;

}
void tab1Dlg::Thread_Image_Canny(LPVOID lParam)
{
	CTab1threadParam * Thread_Info = (CTab1threadParam *)lParam;
	tab1Dlg * hWnd = (tab1Dlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	
	while (1)
	{
		CannyRoiC3 = cvCreateImage(cvGetSize(CannyRoi), CannyRoi->depth,3);
		cvCvtColor(CannyRoi,CannyRoiC3,CV_GRAY2BGR);
		hWnd->ShowImage(CannyRoiC3, hWnd->GetDlgItem(IDC_IMAGE_Canny), 1);
	}

}
void tab1Dlg::SetRoI(IplImage* img_edge)
{
	

	IplImage* img_roiC3 = cvCreateImage(cvGetSize(img_edge), IPL_DEPTH_8U, 3);
	cvCvtColor(img_edge, img_roiC3, CV_GRAY2BGR);

	for (int j = 0; j< img_edge->height; j++) {
		for (size_t i = 0; i < img_edge->width; i++) {
			if (i< RoiPoint [0].x || i>RoiPoint[1].x || j<RoiPoint[0].y || j>RoiPoint[1].y) {
				cvSet2D(img_roiC3, j, i, CV_RGB(0, 0, 0));
			}
		}
	}
	cvCvtColor(img_roiC3, img_edge, CV_BGR2GRAY);

	cvReleaseImage(&img_roiC3);

}

void tab1Dlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	m_threadPara.m_case = 0;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&tab1Dlg::MythreadFun, (LPVOID)&m_threadPara);
	CDialogEx::OnRButtonDown(nFlags, point);
}


void tab1Dlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (point.x > (10) && point.x < (10 + 512) && point.y > 10 && point.y < (10 + 424))
	{
		RoiPoint[1] = cvPoint(point.x-10, point.y- 10);
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}
