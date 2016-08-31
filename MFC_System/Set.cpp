// Set.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "Set.h"
#include "afxdialogex.h"

IplImage* CSet::image_set = nullptr;

// CSet 對話方塊

IMPLEMENT_DYNAMIC(CSet, CDialogEx)

CSet::CSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_set, pParent)
{

}

CSet::~CSet()
{
}

void CSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_Set, m_Img_Set);
}


BEGIN_MESSAGE_MAP(CSet, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CSet 訊息處理常式
BOOL CSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Img_Set.SetWindowPos(NULL, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
	
	m_threadPara.m_case = 0;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&CSet::MythreadFun, (LPVOID)&m_threadPara);

	return TRUE;
}
UINT CSet::MythreadFun(LPVOID LParam)
{
	CSetthreadParam* para = (CSetthreadParam*)LParam;
	CSet* lpview = (CSet*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_RGBSet(LParam);
		break;
	case 1:
	
		break;
	case 2:
		
		break;
	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;

}
void CSet::Thread_RGBSet(LPVOID lParam)
{
	CSetthreadParam * Thread_Info = (CSetthreadParam *)lParam;
	CSet * hWnd = (CSet *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	CMFC_SystemDlg mainDlg;

		image_set = cvCreateImage(cvGetSize(mainDlg.img_RgbSetS), mainDlg.img_RgbSetS->depth,3);
		cvCopy(mainDlg.img_RgbSetS, image_set);
		
		hWnd->ShowImage(image_set, hWnd->GetDlgItem(IDC_IMAGE_Set),3);
		cvReleaseImage(&image_set);	
}
void CSet::ShowImage(IplImage * Image, CWnd * pWnd, int channels)
{
	CDC	*dc = pWnd->GetWindowDC();
	IplImage *Temp = NULL;

	Temp = cvCreateImage(cvSize(1920, 1080), IPL_DEPTH_8U, channels);
	cvResize(Image, Temp, CV_INTER_LINEAR);



	CvvImage Temp2;
	Temp2.CopyOf(Temp);
	Temp2.Show(*dc, 0, 0, Temp->width, Temp->height);
	cvReleaseImage(&Temp);
	ReleaseDC(dc);
}

void CSet::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMFC_SystemDlg mainDlg;
	mainDlg.RGBRefPoint = cvPoint(point.x, point.y);

	OnCancel();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CSet::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_threadPara.m_case = 0;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&CSet::MythreadFun, (LPVOID)&m_threadPara);

	CDialogEx::OnRButtonDown(nFlags, point);
}
