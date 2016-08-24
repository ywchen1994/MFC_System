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
	robot.setVel(10);
}


void tab3Dlg::OnBnClickedButtonconnect()
{
	// TODO: 在此加入控制項告知處理常式程式碼
}
