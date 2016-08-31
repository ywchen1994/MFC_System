// tab3Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "tab3Dlg.h"
#include "afxdialogex.h"
#include "Aria.h"
#include"MFC_SystemDlg.h"
class CMFC_SystemDlg;

using namespace zbar;
using namespace cv;

//P3DX
ArTcpConnection con;
ArSerialConnection serial;
ArRobot robot;
ArTime start;

bool tab3Dlg::FLAG_open = true;
int tab3Dlg::Xpos = 0;
int tab3Dlg::Ypos = 0;
int tab3Dlg::Ypos_Stop = 512;
// tab3Dlg 對話方塊
IMPLEMENT_DYNAMIC(tab3Dlg, CDialogEx)

tab3Dlg::tab3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_tab3, pParent)
{

}

tab3Dlg::~tab3Dlg()
{
}



void tab3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_msg, m_listBox_msg);
}

BEGIN_MESSAGE_MAP(tab3Dlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_robotFrount, &tab3Dlg::OnBnClickedButtonrobotfrount)
	ON_BN_CLICKED(IDC_BUTTON_connect, &tab3Dlg::OnBnClickedButtonconnect)
	ON_BN_CLICKED(IDC_BUTTON_Back, &tab3Dlg::OnBnClickedButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_start, &tab3Dlg::OnBnClickedButtonstart)
	ON_BN_CLICKED(IDC_BUTTON_stop, &tab3Dlg::OnBnClickedButtonstop)
	ON_BN_CLICKED(IDC_BUTTON_goRobot, &tab3Dlg::OnBnClickedButtongorobot)
	ON_BN_CLICKED(IDC_BUTTON_setY, &tab3Dlg::OnBnClickedButtonsety)
END_MESSAGE_MAP()


// tab3Dlg 訊息處理常式
void tab3Dlg::ShowImage(IplImage* Image, CWnd* pWnd, int channels, CvSize size)
{
	CDC	*dc = pWnd->GetWindowDC();
	IplImage *Temp = NULL;
	if (channels != 4) {
		Temp = cvCreateImage(size, IPL_DEPTH_8U, channels);
		cvResize(Image, Temp, CV_INTER_LINEAR);
	}
	if (channels == 4)
	{
		Temp = cvCreateImage(size, IPL_DEPTH_8U, 3);
		IplImage *Temp_transfer = cvCreateImage(size, IPL_DEPTH_8U, 4);
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

void tab3Dlg::OnBnClickedButtonconnect()
{
	Aria::init();
	robot.lock();
	con.setPort("192.168.0.201", 8101);

	if (!con.openSimple())
	{
		printf("Open failed.");
		Aria::shutdown();
	}
	robot.setDeviceConnection(&con);
	if (!robot.blockingConnect())
	{
		printf("Could not connect to robot... exiting\n");
		Aria::shutdown();
	}


	robot.enableMotors();
	//	robot.comInt(ArCommands::ENABLE, 1);
	robot.disableSonar();		                 // Disables the sonar.
	robot.requestEncoderPackets();// Starts a continuous stream of encoder packets.

	robot.runAsync(true);
	robot.unlock();
}

void tab3Dlg::OnBnClickedButtonrobotfrount()
{
	//robot.setVel(10);
	robot.move(50);
}


void tab3Dlg::OnBnClickedButtonBack()
{
	robot.move(-50);
}

UINT tab3Dlg::MythreadFun(LPVOID LParam)
{
	CTab3threadParam* para = (CTab3threadParam*)LParam;
	tab3Dlg* lpview = (tab3Dlg*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_Image_RGB(LParam);
	case 1:
		lpview->Thread_goRobot(LParam);
	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;
}

void tab3Dlg::Thread_Image_RGB(LPVOID lParam)
{
	CTab3threadParam * Thread_Info = (CTab3threadParam *)lParam;
	tab3Dlg * hWnd = (tab3Dlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);

	CListBox* m_listBox_msg = (CListBox*)hWnd->GetDlgItem(IDC_LIST_msg);

	CMFC_SystemDlg mainDlg;
	IplImage* temp = nullptr;


	while (FLAG_open)
	{
		ImageScanner scanner;
		scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

		temp = cvCreateImage(cvSize(1920, 1080), 8, 3);

		cvResize(mainDlg.img_RgbSetS, temp, 1);	

		Mat frame = temp;
		Mat grey;
		cvtColor(frame, grey, CV_BGR2GRAY);

		int width = frame.cols;
		int height = frame.rows;
		uchar *raw = (uchar *)grey.data;
		// wrap image data  
		Image image(width, height, "Y800", raw, width *height);
		// scan the image for barcodes  
		int n = scanner.scan(image);
		// extract results 
		for (Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
		{
			vector<Point> vp;
			int n = symbol->get_location_size();
			for (int i = 0; i < n; i++)
			{
				vp.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
			}
			RotatedRect r = minAreaRect(vp);
			Point2f pts[4];
			r.points(pts);

			line(frame, pts[0], pts[1], Scalar(255, 0, 0), 3);
			line(frame, pts[1], pts[2], Scalar(0, 255, 0), 3);
			line(frame, pts[2], pts[3], Scalar(0, 0, 255), 3);
			line(frame, pts[3], pts[0], Scalar(255, 0, 255), 3);

			IplImage* image2 = cvCloneImage(&(IplImage)frame);
			hWnd->ShowImage(image2, hWnd->GetDlgItem(IDC_IMAGE_Show), 3, cvSize(320, 240));

			CvPoint midPts[3];
			midPts[0] = pts[0]; midPts[2] = pts[2];
			midPts[1].x = (midPts[0].x + midPts[2].x) / 2;
			midPts[1].y = (midPts[0].y + midPts[2].y) / 2;

			// do something useful with results  
			CString msg;
			CString symbolName(symbol->get_type_name().c_str());
			CString data(symbol->get_data().c_str());
			msg = _T("decoded") + symbolName + _T(" symbol \"") + data;
			m_listBox_msg->InsertString(0, msg);

			msg.Format(_T("%d"), midPts[1].x);
			hWnd->GetDlgItem(IDC_EDIT_XPos)->SetWindowText(msg);
			Xpos = midPts[1].x;
			msg.Format(_T("%d"), midPts[1].y);
			hWnd->GetDlgItem(IDC_EDIT_YPos)->SetWindowText(msg);
			Ypos = midPts[1].y;
			msg.Format(_T("(x,y) = ( %d,%d)"), midPts[1].x, midPts[1].y);
			m_listBox_msg->InsertString(0, msg);

			cvReleaseImage(&image2);
		}
		cvReleaseImage(&temp);
	}

}

void tab3Dlg::Thread_goRobot(LPVOID lParam)
{
	robot.setVel(30);
	while (1)
	{
		if (Ypos >= 430 && Ypos < Ypos_Stop)
		{
			robot.setVel(10);
		}
		else if (Ypos >= Ypos_Stop)
		{
			robot.stop();
			FLAG_open = false;
			break;
		}
	}
}


void tab3Dlg::OnBnClickedButtonstart()
{
	FLAG_open = true;
	m_threadPara.m_case = 0;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&tab3Dlg::MythreadFun, (LPVOID)&m_threadPara);
}


void tab3Dlg::OnBnClickedButtonstop()
{
	FLAG_open = false;
}


void tab3Dlg::OnBnClickedButtongorobot()
{
	m_threadPara.m_case = 1;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&tab3Dlg::MythreadFun, (LPVOID)&m_threadPara);
}


void tab3Dlg::OnBnClickedButtonsety()
{
	Ypos_Stop = Ypos;
}
