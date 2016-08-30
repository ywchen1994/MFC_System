
// MFC_SystemDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "MFC_SystemDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CvPoint CMFC_SystemDlg::RoiPoint[2] = { cvPoint(0,0),cvPoint(512,424) };

// CMFC_SystemDlg 對話方塊
Kinect2Capture CMFC_SystemDlg::kinect;
IplImage*CMFC_SystemDlg::img_DepthS=nullptr;
float CMFC_SystemDlg::DepthPointsBase[512][424]= { 0 };
IplImage*CMFC_SystemDlg::img_RgbS = nullptr;
IplImage*CMFC_SystemDlg::img_CannyS = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, 1);
IplImage*CMFC_SystemDlg::sImg_CannyRoiS = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, 1);
float CMFC_SystemDlg::CamRefX = 0;
float CMFC_SystemDlg::CamRefY = 0;
float CMFC_SystemDlg::CamRefZ = 0;
CvPoint CMFC_SystemDlg::Center = cvPoint(0, 0);
float CMFC_SystemDlg::s_Xpos = 0;
float CMFC_SystemDlg::s_Ypos = 0;
float CMFC_SystemDlg::s_Zpos = 0;
float CMFC_SystemDlg::s_Tdeg = 0;
 int const CMFC_SystemDlg::objectdata[3] = {85,56,19};

 CString CMFC_SystemDlg::ip_SCARA = _T("192.168.1.3");

CMFC_SystemDlg::CMFC_SystemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_SYSTEM_DIALOG, pParent)
	, m_Xpos(0)
	, m_Ypos(0)
	, m_Zpos(0)
	, m_Tdeg(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_SystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_RGBLive, m_Img_RGBLive);
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_IMAGE_DepthLive, m_Img_DepthLive);
	DDX_Text(pDX, IDC_EDIT_SCARACommandX, m_Xpos);
	DDX_Text(pDX, IDC_EDIT_SCARACommandY, m_Ypos);
	DDX_Text(pDX, IDC_EDIT_SCARACommandZ, m_Zpos);
	DDX_Text(pDX, IDC_EDIT_SCARACommandT, m_Tdeg);
	DDX_Control(pDX, IDC_IPControl_SCARAIP, m_SCARAIP);
	DDX_Control(pDX, IDC_CHECK_ImgLockerMDLG, m_ImgLockerMDLG);
}

BEGIN_MESSAGE_MAP(CMFC_SystemDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CMFC_SystemDlg::OnTcnSelchangeTab)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	
	ON_BN_CLICKED(IDC_BUTTON_Home, &CMFC_SystemDlg::OnBnClickedButtonHome)
	ON_BN_CLICKED(IDC_BUTTON_Ref, &CMFC_SystemDlg::OnBnClickedButtonRef)
	ON_BN_CLICKED(IDC_BUTTON_grab, &CMFC_SystemDlg::OnBnClickedButtongrab)
	ON_BN_CLICKED(IDC_BUTTON_DownRef, &CMFC_SystemDlg::OnBnClickedButtonDownref)
END_MESSAGE_MAP()


// CMFC_SystemDlg 訊息處理常式

BOOL CMFC_SystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示
	m_Tab.InsertItem(0, _T("設定"));/*給予標籤名稱*/
	m_Tab.InsertItem(1, _T("視覺"));
	m_Tab.InsertItem(2, _T("運送"));
	/********************************************/
	m_TabPage1.Create(IDD_DIALOG_tab1, &m_Tab);
	m_TabPage2.Create(IDD_DIALOG_tab2, &m_Tab);
	m_TabPage3.Create(IDD_DIALOG_tab3, &m_Tab);

	//設定tab control 的第一頁在開始時能正確顯示
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
	m_SCARAIP.SetWindowText(_T("192.168.1.3"));
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
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
	// TODO: 在此加入控制項告知處理常式程式碼
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
int LBottomClicktimes = 0;
void CMFC_SystemDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_ImgLockerMDLG.GetCheck()) {
		if (point.x > 10 && point.x < (10 + 320) && point.y > 10 && point.y < (10 + 240) &&   LBottomClicktimes<2)
		{
			m_threadPara.m_case = 0;
			m_threadPara.hWnd = m_hWnd;
			m_lpThread = AfxBeginThread(&CMFC_SystemDlg::MythreadFun, (LPVOID)&m_threadPara);
		}
		if (point.x > 10 + 320 && point.x < (10 + 320 + 320) && point.y > 10 && point.y < (10 + 240) && LBottomClicktimes <2)
		{
			m_threadPara.m_case = 1;
			m_threadPara.hWnd = m_hWnd;
			m_lpThread = AfxBeginThread(&CMFC_SystemDlg::MythreadFun, (LPVOID)&m_threadPara);
		}
	}
	if(point.x > 10 && point.x < (10 + 320 + 320) && point.y > 10 && point.y < (10 + 240))
	LBottomClicktimes++;
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
	kinect.Open(1, 1, 1);
	

	
	while (1)
	{
		img_DepthS = kinect.DepthImage();
		if (img_DepthS != NULL)
		{
			hWnd->ShowImage(img_DepthS, hWnd->GetDlgItem(IDC_IMAGE_DepthLive), 1);
			cvCanny(img_DepthS, img_CannyS,8,20);//@canny value

			cvReleaseImage(&img_DepthS);
		}
	}
}

void CMFC_SystemDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!m_ImgLockerMDLG.GetCheck()) {
		if (point.x > 10 + 320 && point.x < (10 + 320 + 320) && point.y > 10 && point.y < (10 + 240))
		{
			for (int i = 0; i < 512; i++)
				for (int j = 0; j < 424; j++)
					DepthPointsBase[i][j] = kinect.pDepthPoints[i + 512 * j];//單位是mm
		}
	}
	MessageBox(_T("Set Depth Done"));
	CDialogEx::OnRButtonDown(nFlags, point);
}

void CMFC_SystemDlg::SetPos(float x)
{
	m_Xpos = x;
}


void CMFC_SystemDlg::packetCreat_toPoint(float x, float y, float z, float t)
{
	AfxSocketInit();
	CSocket client_socket;

	char resp[32];

	int temp;

	int X_1 = x*0.01;
	int X_2 = x - X_1 * 100;
	temp = x * 100;
	int X_3 = temp % 100;

	int Y_1 = y*0.01;
	int Y_2 = y - Y_1 * 100;
	temp = y * 100;
	int Y_3 = temp % 100;

	z = 183 - z;
	int Z_1 = z*0.01;
	int Z_2 = z - Z_1 * 100;
	temp = z * 100;
	int Z_3 = temp % 100;

	int theta_1 = t*0.01;
	int theta_2 = t - theta_1 * 100;
	temp = t * 100;
	int theta_3 = temp % 100;

	resp[0] = 'N';
	resp[1] = '1';
	resp[2] = 'r';
	resp[3] = '2';
	resp[4] = X_1;
	resp[5] = X_2;
	resp[6] = X_3;
	resp[7] = Y_1;
	resp[8] = Y_2;
	resp[9] = Y_3;
	resp[10] = Z_1;
	resp[11] = Z_2;
	resp[12] = Z_3;
	resp[13] = theta_1;
	resp[14] = theta_2;
	resp[15] = theta_3;

	if (!client_socket.Create())
	{
		MessageBox(L"Create Faild");
	}

	if (client_socket.Connect(ip_SCARA, 8888))
	{
		client_socket.Send(resp, sizeof(resp));
		client_socket.Close();
	}
	else
	{
		MessageBox(L"Connect fail");
	}

}

void CMFC_SystemDlg::OnBnClickedButtonHome()
{
	UpdateData(false);
	m_SCARAIP.GetWindowTextW(ip_SCARA);


	float tarX = 550;
	float tarY = 0;
	float tarZ =133;
	float tarTheta = 0;

	packetCreat_toPoint(tarX, tarY, tarZ, tarTheta);
}


void CMFC_SystemDlg::OnBnClickedButtonRef()
{
	
	float tarX = 280;
	float tarY = -410;
	float tarZ = 133;
	float tarTheta = 0;

	packetCreat_toPoint(tarX, tarY, tarZ, tarTheta);

}
void CMFC_SystemDlg::grab()
{
	AfxSocketInit();
	CSocket client_socket;

	char resp[]= "N1r3";
	if (!client_socket.Create())
	{
			MessageBox(L"Create Faild");
		return;
	}
	else if (client_socket.Connect(ip_SCARA, 8888))
	{
		client_socket.Send(resp, sizeof(resp));
	}
	client_socket.Close();
}

void CMFC_SystemDlg::OnBnClickedButtongrab()
{
	grab();
}


void CMFC_SystemDlg::OnBnClickedButtonDownref()
{
	grab();
	float tarX = 280;
	float tarY = -410;
	float tarZ = 33;
	float tarTheta = 0;
	packetCreat_toPoint(tarX, tarY, tarZ, tarTheta);
	Sleep(5000);
	grab();

	tarX = 550;
	tarY = 0;
	tarZ = 133;
	tarTheta = 0;

	packetCreat_toPoint(tarX, tarY, tarZ, tarTheta);

}
