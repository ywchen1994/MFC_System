
// MFC_SystemDlg.cpp : ��@��
//

#include "stdafx.h"
#include "MFC_System.h"
#include "MFC_SystemDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_SystemDlg ��ܤ��
IplImage*CMFC_SystemDlg::img_DepthS=nullptr;
IplImage*CMFC_SystemDlg::img_RgbS = nullptr;
IplImage*CMFC_SystemDlg::img_CannyS = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, 1);
IplImage*CMFC_SystemDlg::img_CannyRoiS = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, 1);
CMFC_SystemDlg::CMFC_SystemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_SYSTEM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_SystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_RGBLive, m_Img_RGBLive);
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_IMAGE_DepthLive, m_Img_DepthLive);
}

BEGIN_MESSAGE_MAP(CMFC_SystemDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CMFC_SystemDlg::OnTcnSelchangeTab)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMFC_SystemDlg �T���B�z�`��

BOOL CMFC_SystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�
	m_Tab.InsertItem(0, _T("�]�w"));/*�������ҦW��*/
	m_Tab.InsertItem(1, _T("��ı"));
	m_Tab.InsertItem(2, _T("�B�e"));
	/********************************************/
	m_TabPage1.Create(IDD_DIALOG_tab1, &m_Tab);
	m_TabPage2.Create(IDD_DIALOG_tab2, &m_Tab);
	m_TabPage3.Create(IDD_DIALOG_tab3, &m_Tab);

	//�]�wtab control ���Ĥ@���b�}�l�ɯॿ�T���
	CRect rTab, rItem;
	m_Tab.GetItemRect(0, &rItem);
	m_Tab.GetClientRect(&rTab);
	int x = rItem.left;
	int y = rItem.bottom + 1;
	int cx = rTab.right - rItem.left - 3;
	int cy = rTab.bottom - y - 2;
	m_TabPage3.SetWindowPos(NULL, x, y, cx, cy, SWP_HIDEWINDOW);
	m_TabPage2.SetWindowPos(NULL, x, y, cx, cy, SWP_HIDEWINDOW);
	m_TabPage1.SetWindowPos(NULL, x, y, cx, cy, SWP_SHOWWINDOW);
	/*************************************/
	
	m_Img_RGBLive.SetWindowPos(NULL, 10, 10, 320, 240, SWP_SHOWWINDOW);
	m_Img_DepthLive.SetWindowPos(NULL, 10 + 320, 10, 320, 240, SWP_SHOWWINDOW);
	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
}




void CMFC_SystemDlg::ShowImage(IplImage * Image, CWnd * pWnd, int channels)
{
	CDC	*dc = pWnd->GetWindowDC();
	IplImage *Temp = NULL;
	if (channels != 4) {
		Temp = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, channels);
		cvResize(Image, Temp, CV_INTER_LINEAR);
	}
	if (channels == 4)
	{
		Temp = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);
		IplImage *Temp_transfer = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 4);
		cvResize(Image, Temp_transfer, CV_INTER_LINEAR);
		cvCvtColor(Temp_transfer, Temp, CV_BGRA2BGR);
		cvReleaseImage(&Temp_transfer);
	}

	CvvImage Temp2;
	Temp2.CopyOf(Temp);
	Temp2.Show(*dc, 0, 0, Temp->width, Temp->height);
	cvReleaseImage(&Temp);
	ReleaseDC(dc);
}

void CMFC_SystemDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �b���[�J����i���B�z�`���{���X
	CRect rTab, rItem;
	m_Tab.GetItemRect(0, &rItem);
	m_Tab.GetClientRect(&rTab);
	int x = rItem.left;
	int y = rItem.bottom + 1;
	int cx = rTab.right - rItem.left - 3;
	int cy = rTab.bottom - y - 2;
	int tab = m_Tab.GetCurSel();

	m_TabPage1.SetWindowPos(NULL, x, y, cx, cy, SWP_HIDEWINDOW);
	m_TabPage2.SetWindowPos(NULL, x, y, cx, cy, SWP_HIDEWINDOW);
	m_TabPage3.SetWindowPos(NULL, x, y, cx, cy, SWP_HIDEWINDOW);

	switch (tab)
	{
	case 0:
		m_TabPage1.SetWindowPos(NULL, x, y, cx, cy, SWP_SHOWWINDOW);
		break;
	case 1:
		m_TabPage2.SetWindowPos(NULL, x, y, cx, cy, SWP_SHOWWINDOW);
		break;
	case 2:
		m_TabPage3.SetWindowPos(NULL, x, y, cx, cy, SWP_SHOWWINDOW);
		break;
	}
	*pResult = 0;
}

void CMFC_SystemDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	if (point.x >10 && point.x < (10 + 320) && point.y > 10 && point.y < (10 + 240))
	{
		m_threadPara.m_case = 0;
		m_threadPara.hWnd = m_hWnd;
		m_lpThread = AfxBeginThread(&CMFC_SystemDlg::MythreadFun, (LPVOID)&m_threadPara);
	}
	if (point.x > 10+320 && point.x < (10 + 320+320) && point.y > 10 && point.y < (10 + 240))
	{
		m_threadPara.m_case = 1;
		m_threadPara.hWnd = m_hWnd;
		m_lpThread = AfxBeginThread(&CMFC_SystemDlg::MythreadFun, (LPVOID)&m_threadPara);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}
UINT CMFC_SystemDlg::MythreadFun(LPVOID LParam)
{
	CMythreadParam* para = (CMythreadParam*)LParam;
	CMFC_SystemDlg* lpview = (CMFC_SystemDlg*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_Image_RGB(LParam);

	case 1:
		lpview->Thread_Image_Depth(LParam);
	case 2:
		
	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;

}
void CMFC_SystemDlg::Thread_Image_RGB(LPVOID lParam)
{
	CMythreadParam * Thread_Info = (CMythreadParam *)lParam;
	CMFC_SystemDlg * hWnd = (CMFC_SystemDlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	Kinect2Capture kinect;

	kinect.Open(1, 0, 0);

	while (1)
	{

		img_RgbS = kinect.RGBAImage();
		if (img_RgbS != NULL) {
			hWnd->ShowImage(img_RgbS, hWnd->GetDlgItem(IDC_IMAGE_RGBLive), 4);	
			cvReleaseImage(&img_RgbS);
		}
	}

}
void CMFC_SystemDlg::Thread_Image_Depth(LPVOID lParam)
{
	CMythreadParam * Thread_Info = (CMythreadParam *)lParam;
	CMFC_SystemDlg * hWnd = (CMFC_SystemDlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	Kinect2Capture kinect;

	kinect.Open(1, 1, 1);
	while (1)
	{
		img_DepthS = kinect.DepthImage();
		if (img_DepthS != NULL)
		{
			cvCanny(img_DepthS, img_CannyS,12,15);
			hWnd->ShowImage(img_DepthS, hWnd->GetDlgItem(IDC_IMAGE_DepthLive), 1);
			cvReleaseImage(&img_DepthS);
		}
	}
	
	

}