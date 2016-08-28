// tab3Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "tab3Dlg.h"
#include "afxdialogex.h"
#include "Aria.h"
#include"MFC_SystemDlg.h"
class CMFC_SystemDlg;


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
	
	serial.setBaud(9600);
	serial.setPort("COM5");
	//con.setPort("192.168.0.100", 8266);

	if (!serial.openSimple())
	{
		MessageBox(L"Open failed.");
		Aria::shutdown();
	}

	robot.setDeviceConnection(&serial);
	if (!robot.blockingConnect())
	{
		MessageBox(L"Could not connect to robot... exiting\n");
		//Aria::shutdown();
	}

	robot.enableMotors();
	robot.disableSonar();		                 // Disables the sonar.
	robot.requestEncoderPackets();// Starts a continuous stream of encoder packets.

	robot.runAsync(true);
	robot.unlock();
}
