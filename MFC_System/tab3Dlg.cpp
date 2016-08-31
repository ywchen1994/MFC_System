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


//P3DX
ArTcpConnection con;
ArSerialConnection serial;
ArRobot robot;
ArTime start;

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
}


BEGIN_MESSAGE_MAP(tab3Dlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_robotFrount, &tab3Dlg::OnBnClickedButtonrobotfrount)
	ON_BN_CLICKED(IDC_BUTTON_connect, &tab3Dlg::OnBnClickedButtonconnect)
	ON_BN_CLICKED(IDC_BUTTON_Back, &tab3Dlg::OnBnClickedButtonBack)
END_MESSAGE_MAP()


// tab3Dlg 訊息處理常式


void tab3Dlg::OnBnClickedButtonrobotfrount()
{
	//robot.setVel(10);
	robot.move(50);
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


void tab3Dlg::OnBnClickedButtonBack()
{
	// TODO: 在此加入控制項告知處理常式程式碼
}

UINT tab3Dlg::MythreadFun(LPVOID LParam)
{
	CTab2threadParam* para = (CTab2threadParam*)LParam;
	tab2Dlg* lpview = (tab2Dlg*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_Image_CannyRoi(LParam);
	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;
}
