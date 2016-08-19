// tab2Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "tab2Dlg.h"
#include "afxdialogex.h"


// tab2Dlg 對話方塊

IMPLEMENT_DYNAMIC(tab2Dlg, CDialogEx)

tab2Dlg::tab2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_tab2, pParent)
{

}

tab2Dlg::~tab2Dlg()
{
}

void tab2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(tab2Dlg, CDialogEx)
END_MESSAGE_MAP()


// tab2Dlg 訊息處理常式
