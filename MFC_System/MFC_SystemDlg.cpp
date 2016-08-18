
// MFC_SystemDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "MFC_SystemDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_SystemDlg 對話方塊
IplImage*CMFC_SystemDlg::img_depthS=nullptr;
IplImage*CMFC_SystemDlg::img_rgbS = nullptr;

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
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CMFC_SystemDlg::OnTcnSelchangeTab)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMFC_SystemDlg 訊息處理常式

BOOL CMFC_SystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	m_Img_RGBLive.SetWindowPos(NULL, 10, 10, 320, 240, SWP_SHOWWINDOW);
	m_Img_DepthLive.SetWindowPos(NULL, 10 + 320, 10, 320, 240, SWP_SHOWWINDOW);
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}



void CMFC_SystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CMFC_SystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFC_SystemDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此加入控制項告知處理常式程式碼
	*pResult = 0;
}


void CMFC_SystemDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
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
void CMFC_SystemDlg::Thread_Image_RGB(LPVOID lParam)
{
	CMythreadParam * Thread_Info = (CMythreadParam *)lParam;
	CMFC_SystemDlg * hWnd = (CMFC_SystemDlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	Kinect2Capture kinect;

	kinect.Open(1, 0, 0);

	while (1)
	{

		img_rgbS = kinect.RGBAImage();
		if (img_rgbS != NULL) {
			hWnd->ShowImage(img_rgbS, hWnd->GetDlgItem(IDC_IMAGE_RGBLive), 4);	
			cvReleaseImage(&img_rgbS);
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
		img_depthS = kinect.DepthImage();
		if (img_depthS != NULL)
		{
			hWnd->ShowImage(img_depthS, hWnd->GetDlgItem(IDC_IMAGE_DepthLive), 1);
		}
		cvReleaseImage(&img_depthS);
	}

}