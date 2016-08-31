// tab2Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "tab2Dlg.h"
#include "afxdialogex.h"

#include"MFC_SystemDlg.h"
class CMFC_SystemDlg;

const int BlockStep = 12;

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
	DDX_Control(pDX, IDC_IMAGE_CannyRoi, m_img_CannyRoi);
	DDX_Control(pDX, IDC_IMAGE_ApproxPoly, m_img_approxPoly);
	DDX_Control(pDX, IDC_LIST_detectNum, m_list_detectNum);
	DDX_Control(pDX, IDC_LIST_priority, m_list_priority);
}


BEGIN_MESSAGE_MAP(tab2Dlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_startGrab, &tab2Dlg::OnBnClickedButtonstartgrab)
	ON_BN_CLICKED(IDC_BUTTON_autoBinPick, &tab2Dlg::OnBnClickedButtonautobinpick)
	ON_LBN_SELCHANGE(IDC_LIST_detectNum, &tab2Dlg::OnLbnSelchangeListdetectnum)
	ON_BN_CLICKED(IDC_BUTTON_topPriority, &tab2Dlg::OnBnClickedButtontoppriority)
	ON_BN_CLICKED(IDC_BUTTON_goPushPoint1, &tab2Dlg::OnBnClickedButtongopushpoint1)
	ON_BN_CLICKED(IDC_BUTTON_goPushPoint2, &tab2Dlg::OnBnClickedButtongopushpoint2)
	ON_BN_CLICKED(IDC_BUTTON_goPushPoint3, &tab2Dlg::OnBnClickedButtongopushpoint3)
	ON_BN_CLICKED(IDC_BUTTON_debugMode, &tab2Dlg::OnBnClickedButtondebugmode)
END_MESSAGE_MAP()


// tab2Dlg 訊息處理常式
BOOL tab2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_img_CannyRoi.SetWindowPos(NULL, 10, 10, 400, 400, SWP_SHOWWINDOW);
	m_img_approxPoly.SetWindowPos(NULL, 10, 10+400, 400,400, SWP_SHOWWINDOW);
	return TRUE;
}

void tab2Dlg::ShowImage(IplImage* Image, CWnd* pWnd, int channels, CvSize size)
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

UINT tab2Dlg::MythreadFun(LPVOID LParam)
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

void tab2Dlg::Thread_Image_CannyRoi(LPVOID lParam)
{
	CTab2threadParam * Thread_Info = (CTab2threadParam *)lParam;
	tab2Dlg * hWnd = (tab2Dlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	CMFC_SystemDlg mainDlg;
	IplImage* CannyRoi_Ch1 = nullptr;
	while (1)
	{
		CannyRoi_Ch1 = cvCreateImage(cvGetSize(mainDlg.sImg_CannyRoiS), IPL_DEPTH_8U, 1);
		cvCopy(mainDlg.sImg_CannyRoiS, CannyRoi_Ch1);
		CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x- mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
		cvSetImageROI(CannyRoi_Ch1, rect);
		hWnd->GetDlgItem(IDC_IMAGE_CannyRoi)->SetWindowPos(NULL, 10, 10, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);
		
		hWnd->ShowImage(CannyRoi_Ch1, hWnd->GetDlgItem(IDC_IMAGE_CannyRoi), 1,cvSize(2*(mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2*(mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
		cvReleaseImage(&CannyRoi_Ch1);
	}
}


void tab2Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.x > (10) && point.x < (10 + 400) && point.y > 10 && point.y < (10 + 400))
	{
		m_threadPara.m_case = 0;
		m_threadPara.hWnd = m_hWnd;
		m_lpThread = AfxBeginThread(&tab2Dlg::MythreadFun, (LPVOID)&m_threadPara);
	}
	if (point.x > (10) && point.x < (10 + 400) && point.y > 10+400 && point.y < (10 + 400+400))
	{
		ObjectCounter = 0;
		m_list_detectNum.ResetContent();
		m_list_priority.ResetContent();

		system("del .\\ApproxPolyPics\\*.jpg");
		CMFC_SystemDlg mainDlg;
		IplImage*  img_CannyRoi = cvCreateImage(cvGetSize(mainDlg.sImg_CannyRoiS), IPL_DEPTH_8U, 1);
		cvCopy(mainDlg.sImg_CannyRoiS, img_CannyRoi);
		//影像處理 閉合破碎
		IplConvKernel *pKernel = NULL;
		pKernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, NULL);
		cvDilate(img_CannyRoi, img_CannyRoi, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, NULL), 1);
		cvErode(img_CannyRoi, img_CannyRoi, pKernel, 1);
		
		ApproxPoly(img_CannyRoi);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

void tab2Dlg::ApproxPoly(IplImage *img_roi)
{
	IplImage* src = NULL;
	IplImage* img = NULL;

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvMemStorage* storage1 = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	CvSeq* cont;
	CvSeq* mcont;

	src = cvCreateImage(cvGetSize(img_roi), img_roi->depth, 1);
	cvCopy(img_roi, src);
	IplImage* First = cvCreateImage(cvSize(src->width, src->height), src->depth, 3);
	IplImage* Sec = cvCreateImage(cvSize(src->width, src->height), src->depth, 3);

	cvSetZero(First);
	cvSetZero(Sec);

	cvThreshold(src, src, 150, 255, CV_THRESH_BINARY);
	cvFindContours(src, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	if (contour)
	{
		CvTreeNodeIterator iterator;
		cvInitTreeNodeIterator(&iterator, contour, 1);
		while (0 != (cont = (CvSeq*)cvNextTreeNode(&iterator)))
		{
			cvSetZero(Sec);
			mcont = cvApproxPoly(cont, sizeof(CvContour), storage1, CV_POLY_APPROX_DP, cvContourPerimeter(cont)*0.02, 0);
			cvDrawContours(Sec, mcont, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), 1, 1, 8, cvPoint(0, 0));

			findinside(Sec);
			cvCopy(Sec, First);
		}
	}
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&src);
	cvReleaseImage(&img);

	cvReleaseImage(&First);
	cvReleaseImage(&Sec);
}
void tab2Dlg::findinside(IplImage *Img)
{
	char SaveImgPath[100] = { 0 };
	double tmp;
	bool inside = false;
	IplImage *mask = cvCreateImage(cvSize(Img->width, Img->height), Img->depth, 1);
	cvInRangeS(Img, cvScalar(0, 255, 0), cvScalar(0, 255, 0), mask);
	for (int i = 0; i < Img->height; i++) {
		for (size_t j = 0; j <Img->width; j++) {
			tmp = cvGet2D(mask, i, j).val[0];
			if (tmp > 0)inside = true;
		}
	}

	if (inside)
	{	
		sprintf_s(SaveImgPath, "ApproxPolyPics/inside%d.jpg", ObjectCounter);
		
		/****************新增至combobox*******************/
		CString objNum;
		objNum.Format(_T("%d"), ObjectCounter);
		m_list_detectNum.InsertString(ObjectCounter, objNum);

		/**************************************************/
		
		cvSaveImage(SaveImgPath, mask);
		ObjectCounter++;

	}
	cvReleaseImage(&mask);

}
void Text(IplImage* img, const char* text, int x, int y)
{
	CvFont font;
	double hscale = 0.27;
	double vscale = 0.27;
	int linewidth = 1;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hscale, vscale, 0, linewidth);
	CvScalar textColor = cvScalar(0, 255, 255);
	CvPoint textPos = cvPoint(x, y);
	cvPutText(img, text, textPos, &font, textColor);
}

void tab2Dlg::OnLbnSelchangeListdetectnum()
{
	int select = m_list_detectNum.GetCurSel();
	priority = grabDecision(select, &m_pushPoint[0], &m_degree);
	m_list_priority.SetCurSel(select);
	return;
}


int tab2Dlg::grabDecision(int pictureSelcet, CvPoint3D32f* pushPoint, float* degree)
{
	char path[100];
	sprintf(path, "ApproxPolyPics/inside%d.jpg", pictureSelcet);
	IplImage* ImageLoad = cvLoadImage(path, 0);
	IplImage* ImgApproxPolyLoad = cvCreateImage(cvGetSize(ImageLoad), ImageLoad->depth, 1);
	cvCopy(ImageLoad, ImgApproxPolyLoad);
	cvReleaseImage(&ImageLoad);

	//找中心點
	CMFC_SystemDlg mainDlg;
	mainDlg.Center = GetCenter(ImgApproxPolyLoad);

	//找角點 跟 角點個數  (CornerPoint 已經放入)
	IplImage* imageCorner = cvCreateImage(cvGetSize(ImgApproxPolyLoad), ImgApproxPolyLoad->depth, 3);
	int cornerNum;
	CornerDetection(ImgApproxPolyLoad, imageCorner, &cornerNum);
	cvCvtColor(ImgApproxPolyLoad, imageCorner, CV_GRAY2RGB);

	//若角點為4個   //若不是 則單純顯示圖片(不畫出角點)
	if (cornerNum == 4)
	{

		//重新校正中心點 (利用4個點的中心 而非所有邊框)
		mainDlg.Center.x = (CornerPoint[0].x + CornerPoint[1].x + CornerPoint[2].x + CornerPoint[3].x) / 4;
		mainDlg.Center.y = (CornerPoint[0].y + CornerPoint[1].y + CornerPoint[2].y + CornerPoint[3].y) / 4;


		//排序四個點
		CvPoint2D32f outPoint[4];
		sequencePoint(&CornerPoint[0], mainDlg.Center, &outPoint[0]);

		//畫出 角點
		for (int i = 0; i < 4; i++)
			cvCircle(imageCorner, CornerPoint[i], 3, CV_RGB(0, 255, 0), CV_FILLED);

		//將外點 換成世界座標
		CvPoint3D32f outPoint_World[4];
		for (int i = 0; i < 4; i++)
			Img2SCARA(outPoint[i].x, outPoint[i].y, &outPoint_World[i].x, &outPoint_World[i].y, &outPoint_World[i].z);


		//排除四個點，但是卻不是單一物體或特殊狀況------------------------
		//5個點 轉成 世界座標
		CvPoint3D32f ObjectPoint_World[5];
		Img2SCARA(mainDlg.Center.x, mainDlg.Center.y, &ObjectPoint_World[0].x, &ObjectPoint_World[0].y, &ObjectPoint_World[0].z);
		Img2SCARA(CornerPoint[0].x, CornerPoint[0].y, &ObjectPoint_World[1].x, &ObjectPoint_World[1].y, &ObjectPoint_World[1].z);
		Img2SCARA(CornerPoint[1].x, CornerPoint[1].y, &ObjectPoint_World[2].x, &ObjectPoint_World[2].y, &ObjectPoint_World[2].z);
		Img2SCARA(CornerPoint[2].x, CornerPoint[2].y, &ObjectPoint_World[3].x, &ObjectPoint_World[3].y, &ObjectPoint_World[3].z);
		Img2SCARA(CornerPoint[3].x, CornerPoint[3].y, &ObjectPoint_World[4].x, &ObjectPoint_World[4].y, &ObjectPoint_World[4].z);

		float distence_width[2];
		float distence_length[2];
		distence_width[0] = sqrt(pow((ObjectPoint_World[1].x - ObjectPoint_World[4].x), 2) + pow((ObjectPoint_World[1].y - ObjectPoint_World[4].y), 2));
		distence_width[1] = sqrt(pow((ObjectPoint_World[2].x - ObjectPoint_World[3].x), 2) + pow((ObjectPoint_World[2].y - ObjectPoint_World[3].y), 2));
		distence_length[0] = sqrt(pow((ObjectPoint_World[1].x - ObjectPoint_World[2].x), 2) + pow((ObjectPoint_World[1].y - ObjectPoint_World[2].y), 2));
		distence_length[1] = sqrt(pow((ObjectPoint_World[3].x - ObjectPoint_World[4].x), 2) + pow((ObjectPoint_World[3].y - ObjectPoint_World[4].y), 2));
		const int distence_error = 10;
		if (distence_width[0] > 50 + distence_error || distence_width[0] < 50 - distence_error)//寬度不符合
		{
			//showimage	 
			CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
			cvSetImageROI(imageCorner, rect);
			ShowImage(imageCorner, GetDlgItem(IDC_IMAGE_ApproxPoly), 3, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
			m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);
			return 10;
		}
		if (distence_width[1] > 50 + distence_error || distence_width[1] < 50 - distence_error)//寬度不符合
		{
			//showimage	 
			CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
			cvSetImageROI(imageCorner, rect);
			ShowImage(imageCorner, GetDlgItem(IDC_IMAGE_ApproxPoly), 3, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
			m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);
			return 10;
		}
		if (distence_length[0] > 75 + distence_error || distence_length[0] < 75 - distence_error)//長度不符合
		{
			//showimage	 
			CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
			cvSetImageROI(imageCorner, rect);
			ShowImage(imageCorner, GetDlgItem(IDC_IMAGE_ApproxPoly), 3, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
			m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);
			return 10;
		}
		if (distence_length[1] > 75 + distence_error || distence_length[1] < 75 - distence_error)//長度不符合
		{
			//showimage	 
			CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
			cvSetImageROI(imageCorner, rect);
			ShowImage(imageCorner, GetDlgItem(IDC_IMAGE_ApproxPoly), 3, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
			m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);
			return 10;
		}
		//---------------------------------------------------------------------


		//內縮角點
		for (int i = 0; i < 4; i++)
		{
			CornerPoint[i].x = (mainDlg.Center.x + 4 * CornerPoint[i].x) / 5;
			CornerPoint[i].y = (mainDlg.Center.y + 4 * CornerPoint[i].y) / 5;
		}

		//5個點 轉成 世界座標
		Img2SCARA(mainDlg.Center.x, mainDlg.Center.y, &ObjectPoint_World[0].x, &ObjectPoint_World[0].y, &ObjectPoint_World[0].z);
		Img2SCARA(CornerPoint[0].x, CornerPoint[0].y, &ObjectPoint_World[1].x, &ObjectPoint_World[1].y, &ObjectPoint_World[1].z);
		Img2SCARA(CornerPoint[1].x, CornerPoint[1].y, &ObjectPoint_World[2].x, &ObjectPoint_World[2].y, &ObjectPoint_World[2].z);
		Img2SCARA(CornerPoint[2].x, CornerPoint[2].y, &ObjectPoint_World[3].x, &ObjectPoint_World[3].y, &ObjectPoint_World[3].z);
		Img2SCARA(CornerPoint[3].x, CornerPoint[3].y, &ObjectPoint_World[4].x, &ObjectPoint_World[4].y, &ObjectPoint_World[4].z);


		//畫出外點
		for (int i = 0; i < 4; i++)
			cvCircle(imageCorner, cvPoint(outPoint[i].x, outPoint[i].y), 2, CV_RGB(0, 0, 255), CV_FILLED);
		//畫出內縮角點
		for (int i = 0; i < 4; i++)
			cvCircle(imageCorner, CornerPoint[i], 1, CV_RGB(0, 255, 0), CV_FILLED);
		//畫出中點
		cvCircle(imageCorner, mainDlg.Center, 3, CV_RGB(255, 0, 0), CV_FILLED);

		//畫上座標
		Text(imageCorner, "0", outPoint[0].x, outPoint[0].y);
		Text(imageCorner, "1", outPoint[1].x, outPoint[1].y);
		Text(imageCorner, "2", outPoint[2].x, outPoint[2].y);
		Text(imageCorner, "3", outPoint[3].x, outPoint[3].y);

		Text(imageCorner, "0", CornerPoint[0].x, CornerPoint[0].y);
		Text(imageCorner, "1", CornerPoint[1].x, CornerPoint[1].y);
		Text(imageCorner, "2", CornerPoint[2].x, CornerPoint[2].y);
		Text(imageCorner, "3", CornerPoint[3].x, CornerPoint[3].y);

		//showimage	 
		CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
		cvSetImageROI(imageCorner, rect);
		ShowImage(imageCorner, GetDlgItem(IDC_IMAGE_ApproxPoly), 3, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
		m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);

		//CString Pos;
		//Pos.Format(_T("%d,%d"), CornerPoint[0].x, CornerPoint[0].y);
		//GetDlgItem(IDC_EDIT1_pixel_corner1)->SetWindowTextW(Pos);
		//Pos.Format(_T("%d,%d"), CornerPoint[1].x, CornerPoint[1].y);
		//GetDlgItem(IDC_EDIT1_pixel_corner2)->SetWindowTextW(Pos);
		//Pos.Format(_T("%d,%d"), CornerPoint[2].x, CornerPoint[2].y);
		//GetDlgItem(IDC_EDIT1_pixel_corner3)->SetWindowTextW(Pos);
		//Pos.Format(_T("%d,%d"), CornerPoint[3].x, CornerPoint[3].y);
		//GetDlgItem(IDC_EDIT1_pixel_corner4)->SetWindowTextW(Pos);
		//Pos.Format(_T("%d,%d"), mainDlg.Center.x, mainDlg.Center.y);
		//GetDlgItem(IDC_EDIT1_pixel_cornerCenter)->SetWindowTextW(Pos);

		//判斷擺放case
		int ObjCase = caseClassify(&ObjectPoint_World[0]);

		//判斷有幾個東西在四周
		int blockCount = 0;
		for (int i = 0; i < 4; i++)
		{
			if (outPoint_World[i].z > BlockStep)
			{
				blockCount++;
			}
		}


		switch (ObjCase)
		{
		case 1:  //平放
		{
			switch (blockCount)
			{
			case 0://平面旁邊無東西
			{
				//夾取 
				pushPoint[0] = cvPoint3D32f(ObjectPoint_World[0].x, ObjectPoint_World[0].y, 18);
				pushPoint[1] = pushPoint[0];
				*degree = getDegree(ObjectPoint_World[0], outPoint_World[0]);
				return 2;
			}
			break;
			case 1://平面 旁邊一個block
			{
				if ((outPoint_World[1].z < BlockStep) && (outPoint_World[3].z < BlockStep))//block在長邊
				{
					//從短邊推
					pushPoint[0] = extendPoint(ObjectPoint_World[0], outPoint_World[1], 100);//短邊外面
					pushPoint[1] = ObjectPoint_World[0];//=中心
					pushPoint[0].z = 18;
					pushPoint[1].z = 18;
					*degree = getDegree(pushPoint[0], pushPoint[1]);
					return 6;
				}
				else // block 在短邊
				{
					//夾取
					pushPoint[0] = cvPoint3D32f(ObjectPoint_World[0].x, ObjectPoint_World[0].y, 18);
					pushPoint[1] = pushPoint[0];
					*degree = getDegree(ObjectPoint_World[0], outPoint_World[0]);
					return 4;
				}
			}
			break;
			}
		}
		break;
		case 2:  //平放堆疊
		{
			pushPoint[0] = cvPoint3D32f(ObjectPoint_World[0].x, ObjectPoint_World[0].y, ObjectPoint_World[0].z);
			pushPoint[1] = pushPoint[0];
			*degree = getDegree(ObjectPoint_World[0], outPoint_World[0]);
			return 1;
		}
		break;
		case 3: //斜面
		{
			switch (blockCount)
			{
			case 0:
			{
				//從高邊 往 center推
				pushPoint[0] = extendPoint(ObjectPoint_World[0], outPoint_World[findHighestSide(ObjectPoint_World)], 100);
				pushPoint[1] = ObjectPoint_World[0];
				pushPoint[0].z = 36;
				pushPoint[1].z = 36;
				*degree = getDegree(pushPoint[0], pushPoint[1]);
				return 8;
			}
			break;
			case 1: //斜面 正常狀況
			{
				if ((outPoint_World[1].z > BlockStep) || (outPoint_World[3].z > BlockStep))//block在短邊
				{
					//夾取 2倍高
					pushPoint[0] = cvPoint3D32f(ObjectPoint_World[0].x, ObjectPoint_World[0].y, 30);
					pushPoint[1] = pushPoint[0];
					*degree = getDegree(ObjectPoint_World[0], outPoint_World[0]);
					return 5;
				}
				if ((outPoint_World[0].z > BlockStep) || (outPoint_World[2].z > BlockStep))//block在長邊
				{
					//找高邊
					if (ObjectPoint_World[4].z > ObjectPoint_World[1].z)//表示 block在outPoint 2
					{
						//從 outPoint 2 往 center 推
						pushPoint[0] = extendPoint(ObjectPoint_World[0], outPoint_World[2], 100);
						pushPoint[1] = ObjectPoint_World[0];
						pushPoint[0].z = 36;
						pushPoint[1].z = 36;
						*degree = getDegree(pushPoint[0], pushPoint[1]);
						return 7;
					}
					else if (ObjectPoint_World[4].z < ObjectPoint_World[1].z)//表示 block在outPoint 0
					{
						//從 outPoint 0 往 center 推
						pushPoint[0] = extendPoint(ObjectPoint_World[0], outPoint_World[0], 100);
						pushPoint[1] = ObjectPoint_World[0];
						pushPoint[0].z = 36;
						pushPoint[1].z = 36;
						*degree = getDegree(pushPoint[0], pushPoint[1]);
						return 7;
					}
				}
			}
			break;
			case 2: //斜面 旁邊有一個障礙物
			{
				//從高邊 往 center推
				pushPoint[0] = extendPoint(ObjectPoint_World[0], outPoint_World[findHighestSide(ObjectPoint_World)], 100);
				pushPoint[1] = ObjectPoint_World[0];
				pushPoint[0].z = 36;
				pushPoint[1].z = 36;
				*degree = getDegree(pushPoint[0], pushPoint[1]);
				return 8;
			}
			break;
			case 3:
			{
				//從高邊 往 center推
				pushPoint[0] = extendPoint(ObjectPoint_World[0], outPoint_World[findHighestSide(ObjectPoint_World)], 100);
				pushPoint[1] = ObjectPoint_World[0];
				pushPoint[0].z = 36;
				pushPoint[1].z = 36;
				*degree = getDegree(pushPoint[0], pushPoint[1]);
				return 8;
			}
			break;
			case 4:
			{
				//從高邊 往 center推
				pushPoint[0] = extendPoint(ObjectPoint_World[0], outPoint_World[findHighestSide(ObjectPoint_World)], 100);
				pushPoint[1] = ObjectPoint_World[0];
				pushPoint[0].z = 36;
				pushPoint[1].z = 36;
				*degree = getDegree(pushPoint[0], pushPoint[1]);
				return 8;
			}
			break;
			}//switch (blockCount)
		}
		break;//斜面
		}//switch ObjCase

	}//if (cornerNum == 4)
	else//若角點不是四個 則單純顯示圖片(不畫出角點)
	{
		//畫出 角點
		for (int i = 0; i < cornerNum; i++)
			cvCircle(imageCorner, CornerPoint[i], 2, CV_RGB(0, 255, 0), CV_FILLED);

		//畫出中點
		cvCircle(imageCorner, mainDlg.Center, 3, CV_RGB(255, 0, 0), CV_FILLED);

		//5個點 轉成 世界座標
		CvPoint3D32f ObjectPoint_World[10];
		Img2SCARA(mainDlg.Center.x, mainDlg.Center.y, &ObjectPoint_World[0].x, &ObjectPoint_World[0].y, &ObjectPoint_World[0].z);
		for (int i = 0; i < cornerNum; i++)
		{
			Img2SCARA(CornerPoint[i].x, CornerPoint[i].y, &ObjectPoint_World[i+1].x, &ObjectPoint_World[i+1].y, &ObjectPoint_World[i+1].z);
		}

		//showimage
		CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
		cvSetImageROI(ImgApproxPolyLoad, rect);
		ShowImage(ImgApproxPolyLoad, GetDlgItem(IDC_IMAGE_ApproxPoly), 1, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
		m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);
		return 10;//沒優先權 不夾取
	}

	cvReleaseImage(&ImgApproxPolyLoad);
	cvReleaseImage(&imageCorner);
	return 99;
}

void tab2Dlg::SpecilGrabDecision(int pictureSelcet, CvPoint3D32f * pushPoint, float * degree)
{
	char path[100];
	sprintf(path, "ApproxPolyPics/inside%d.jpg", pictureSelcet);
	IplImage* ImageLoad = cvLoadImage(path, 0);
	IplImage* ImgApproxPolyLoad = cvCreateImage(cvGetSize(ImageLoad), ImageLoad->depth, 1);
	cvCopy(ImageLoad, ImgApproxPolyLoad);
	cvThreshold(ImgApproxPolyLoad, ImgApproxPolyLoad, 100, 255, CV_THRESH_BINARY);
	cvReleaseImage(&ImageLoad);

	//找中心點
	CMFC_SystemDlg mainDlg;
	mainDlg.Center = GetCenter(ImgApproxPolyLoad);

	//找角點 跟 角點個數  (CornerPoint 已經放入)
	IplImage* imageCorner = cvCreateImage(cvGetSize(ImgApproxPolyLoad), ImgApproxPolyLoad->depth, 3);
	int cornerNum;
	CornerDetection(ImgApproxPolyLoad, imageCorner, &cornerNum);
	cvCvtColor(ImgApproxPolyLoad, imageCorner, CV_GRAY2RGB);

	//取得與中心最近點
	CvPoint clostestPoint;
	clostestPoint = GetClosestPoint(ImgApproxPolyLoad, mainDlg.Center);

	//將clostestPoint轉世界座標
	CvPoint3D32f clostestPoint_World;

	Img2SCARA(clostestPoint.x, clostestPoint.y, &clostestPoint_World.x, &clostestPoint_World.y, &clostestPoint_World.z);

	//5個點 轉成 世界座標
	CvPoint3D32f ObjectPoint_World[5];
	Img2SCARA(mainDlg.Center.x, mainDlg.Center.y, &ObjectPoint_World[0].x, &ObjectPoint_World[0].y, &ObjectPoint_World[0].z);
	Img2SCARA(CornerPoint[0].x, CornerPoint[0].y, &ObjectPoint_World[1].x, &ObjectPoint_World[1].y, &ObjectPoint_World[1].z);
	Img2SCARA(CornerPoint[1].x, CornerPoint[1].y, &ObjectPoint_World[2].x, &ObjectPoint_World[2].y, &ObjectPoint_World[2].z);
	Img2SCARA(CornerPoint[2].x, CornerPoint[2].y, &ObjectPoint_World[3].x, &ObjectPoint_World[3].y, &ObjectPoint_World[3].z);
	Img2SCARA(CornerPoint[3].x, CornerPoint[3].y, &ObjectPoint_World[4].x, &ObjectPoint_World[4].y, &ObjectPoint_World[4].z);

	
	pushPoint[0] = extendPoint(ObjectPoint_World[0], clostestPoint_World, 150);
	pushPoint[1] = ObjectPoint_World[0];
	if (ObjectPoint_World[0].z < 18)
	{
		pushPoint[0].z = 18;
		pushPoint[1].z = 18;
	}
	else if (ObjectPoint_World[0].z > 80)
	{
		pushPoint[0].z = 80;
		pushPoint[1].z = 80;
	}
	else
	{
		pushPoint[0].z = ObjectPoint_World[0].z;
		pushPoint[1].z = ObjectPoint_World[0].z;
	}

	*degree = getDegree(pushPoint[0], pushPoint[1]);
}

CvPoint tab2Dlg::GetClosestPoint(IplImage * src, CvPoint center)
{
	CvPoint ClosestP = cvPoint(0, 0);
	long double length = 441920;
	long double lengthTemp;
	CvScalar pixel;
	for (int i = 0; i < src->width; i++) {
		for (int j = 0; j < src->height; j++) {
			pixel = cvGet2D(src, j, i);
			if (pixel.val[0] > 0) {
				lengthTemp = pow(i - center.x, 2) + pow(j - center.y, 2);
				if (length>lengthTemp) {
					length = lengthTemp;
					if (length == lengthTemp) {
						ClosestP.x = i;
						ClosestP.y = j;
					}
				}
			}
		}
	}

	return ClosestP;
}

void tab2Dlg::sequencePoint(CvPoint * corner, CvPoint center, CvPoint2D32f * outside)
{

	//-----排序 CornerPoint x 小到大(左到右)--------------------
	for (int n = 0; n < 3; n++)//做三次
	{
		for (int i = 0; i < 3; i++)//0跟1比......2跟3比
		{
			if (corner[i].x > corner[i + 1].x)
			{
				//交換
				CvPoint temp;
				temp = corner[i];
				corner[i] = corner[i + 1];
				corner[i + 1] = temp;
			}
		}
	}
	//-------------------------------------------------------


	//特殊狀況 偵測到梯形---------
	if (corner[0].y >corner[1].y && corner[3].y >corner[2].y)
	{
		CvPoint temp_23change;
		temp_23change = corner[2];
		corner[2] = corner[3];
		corner[3] = temp_23change;
	}
	if (corner[1].y >corner[0].y && corner[2].y >corner[3].y)
	{
		CvPoint temp_23change;
		temp_23change = corner[2];
		corner[2] = corner[3];
		corner[3] = temp_23change;
	}
	//---------------------------

	//算出外圈的點 供推方塊使用
	CvPoint2D32f mediumPoint[4];
	mediumPoint[0] = cvPoint2D32f(0.5*(corner[0].x + corner[1].x), 0.5*(corner[0].y + corner[1].y));
	mediumPoint[1] = cvPoint2D32f(0.5*(corner[1].x + corner[3].x), 0.5*(corner[1].y + corner[3].y));
	mediumPoint[2] = cvPoint2D32f(0.5*(corner[3].x + corner[2].x), 0.5*(corner[3].y + corner[2].y));
	mediumPoint[3] = cvPoint2D32f(0.5*(corner[2].x + corner[0].x), 0.5*(corner[2].y + corner[0].y));
	float length[4];
	length[0] = sqrt(pow((center.x - mediumPoint[0].x), 2) + pow((center.y - mediumPoint[0].y), 2));
	length[1] = sqrt(pow((center.x - mediumPoint[1].x), 2) + pow((center.y - mediumPoint[1].y), 2));
	length[2] = sqrt(pow((center.x - mediumPoint[2].x), 2) + pow((center.y - mediumPoint[2].y), 2));
	length[3] = sqrt(pow((center.x - mediumPoint[3].x), 2) + pow((center.y - mediumPoint[3].y), 2));


	const int radius1 = 35;
	//CvPoint2D32f outPoint1[4];
	for (int i = 0; i < 4; i++)
	{
		float x = (mediumPoint[i].x - center.x);
		float y = (mediumPoint[i].y - center.y);
		outside[i] = cvPoint2D32f((x / length[i] * radius1) + center.x, (y / length[i] * radius1) + center.y);
	}

	//2 3 對調 使 點 順時針照順序
	CvPoint temp;
	temp = corner[2];
	corner[2] = corner[3];
	corner[3] = temp;

	//0 1 要是長邊
	float distance1, distance2;
	distance1 = pow((corner[1].x - corner[0].x), 2) + pow((corner[1].y - corner[0].y), 2);
	distance2 = pow((corner[3].x - corner[0].x), 2) + pow((corner[3].y - corner[0].y), 2);
	if (distance1 < distance2)
	{
		CvPoint temp;
		temp = corner[1];
		corner[1] = corner[2];
		corner[2] = corner[3];
		corner[3] = corner[0];
		corner[0] = temp;

		CvPoint2D32f tempOut;
		tempOut = outside[1];
		outside[1] = outside[2];
		outside[2] = outside[3];
		outside[3] = outside[0];
		outside[0] = tempOut;
	}



	//轉成順時針
	int Array1[] = { corner[1].x - corner[0].x ,corner[1].y - corner[0].y };
	int Array2[] = { corner[3].x - corner[0].x ,corner[3].y - corner[0].y };
	int cross = Array1[0] * Array2[1] - Array1[1] * Array2[0];

	if (cross < 0)
	{
		CvPoint temp;
		temp = corner[0];
		corner[0] = corner[1];
		corner[1] = temp;

		temp = corner[2];
		corner[2] = corner[3];
		corner[3] = temp;

		CvPoint2D32f tempOut;
		tempOut = outside[1];
		outside[1] = outside[3];
		outside[3] = tempOut;
	}
}

void tab2Dlg::CornerDetection(IplImage* edge_roi, IplImage *CornerImg_Modified, int* cornerCount)
{
	
	IplImage* src = nullptr;
	src = cvCreateImage(cvGetSize(edge_roi), edge_roi->depth, 1);
	cvCopy(edge_roi, src);

	IplImage *dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
	IplImage *dst_8U = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	cvSetZero(CornerImg_Modified);
	cvSetZero(dst);
	cvCornerHarris(src, dst, 5, 7);//@harris
	cvConvertScale(dst, dst_8U, 255,0);
	cvThreshold(dst_8U, dst_8U, 1, 255, CV_THRESH_BINARY);

	//這裡只做Harris由於Harris的角點是範圍的所以才要有HarrisCornerToPoint
	HarrisCornerToPoint(dst_8U, CornerImg_Modified, cornerCount);

	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst_8U);
}
void tab2Dlg::HarrisCornerToPoint(IplImage* Cornerimage, IplImage *dst, int* cornerCount)
{
	IplImage *Sec = cvCreateImage(cvGetSize(Cornerimage), Cornerimage->depth, 3);
	IplImage *First = cvCreateImage(cvGetSize(Cornerimage), Cornerimage->depth, 3);

	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq*contours;
	cvFindContours(Cornerimage, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	
	int CornerCounter = 0;
	for (; contours != 0; contours = contours->h_next)
	{
		cvSetZero(Sec);
		cvDrawContours(Sec, contours, CV_RGB(255, 255, 255), CV_RGB(0, 0, 0), -1, CV_FILLED, 8, cvPoint(0, 0));

		CvPoint corner = GetCenter(Sec);
		cvCircle(dst, corner, 0, CV_RGB(255, 255, 255), CV_FILLED);
		CornerPoint[CornerCounter] = corner;
		CornerCounter++;

		cvCopy(Sec, First);
	}

	*cornerCount = CornerCounter;
	cvReleaseImage(&Sec);
	cvReleaseImage(&First);
}

CvPoint tab2Dlg::GetCenter(IplImage *src)
{
	CvPoint corner;
	int x0 = 0, y0 = 0, sum = 0;
	CvScalar pixel;
	for (int i = 0; i < src->width; i++)
		for (int j = 0; j < src->height; j++)
			if (cvGet2D(src, j, i).val[0] > 0)
			{
				x0 = x0 + i;
				y0 = y0 + j;
				sum = sum + 1;
			}

	if (sum == 0) { corner.x = 0; corner.y = 0; }

	corner.x = x0 / sum;
	corner.y = y0 / sum;

	return corner;
}

void tab2Dlg::Img2SCARA(int x, int y, float *SCARAX, float *SCARAY, float *SCARAZ)
{
	CMFC_SystemDlg mainDlg;
	
	mainDlg.kinect.Depth2CameraSpace(x, y);
	*SCARAX = mainDlg.CamRefY - (mainDlg.kinect.CameraY * 1000) + 280 ;

	*SCARAY = mainDlg.CamRefX - (mainDlg.kinect.CameraX * 1000) - 410;

	*SCARAZ = (mainDlg.DepthPointsBase[x][y] - mainDlg.kinect.CameraZ * 1000);
}
int tab2Dlg::FindElement(float fitemp)
{
	for (int i = 0; i < 4; i++)
		if (phi[i] == fitemp)
			return i;
}
int cmp(const void *a, const void *b)
{
	return *(float *)a > *(float *)b ? 1 : -1;
}

int tab2Dlg::caseClassify(CvPoint3D32f* objPoint)
{
	float slope12 = (objPoint[2].z - objPoint[1].z);
	float slope14 = (objPoint[4].z - objPoint[1].z);

	float flatAllowError = 8;
	if ((slope12 > 0 - flatAllowError && slope12 < 0 + flatAllowError) && (slope14 > 0 - flatAllowError && slope14 < 0 + flatAllowError))
		if (objPoint[0].z > 25)
			return 2;
		else
			return 1;

	if ((slope12 < 0 - flatAllowError || slope12 > 0 + flatAllowError) || (slope14 < 0 - flatAllowError || slope14 > 0 + flatAllowError))
		return 3;
}

float tab2Dlg::getDegree(CvPoint3D32f first, CvPoint3D32f second)
{
	float x = second.x - first.x;
	float y = second.y - first.y;
	float m = y / x;
	float degree;

	degree = atan(m);

	return degree*180/ 3.1415926535897932384626433832;
}

int tab2Dlg::findHighestSide(CvPoint3D32f * objPoint)
{
	float slope12 = (objPoint[2].z - objPoint[1].z);
	float slope14 = (objPoint[4].z - objPoint[1].z);

	float flatAllowError = 5;
	if ((slope12 > 0 - flatAllowError && slope12 < 0 + flatAllowError))// 1 2 objPoint 水平
	{
		//比較 1 4 objPoint 哪個高
		if (objPoint[1].z > objPoint[4].z)
			return 0;//outpoint 0 是高邊
		else if (objPoint[4].z > objPoint[1].z)
			return 2;//outpoint 2 是高邊
	}
	else if((slope14 > 0 - flatAllowError && slope14 < 0 + flatAllowError))// 1 4 objPoint 水平
	{
		//比較 1 2 objPoint 哪個高
		if (objPoint[1].z > objPoint[2].z)
			return 3;//outpoint 3 是高邊
		else if (objPoint[2].z > objPoint[1].z)
			return 1;//outpoint 1 是高邊
	}
}

CvPoint3D32f tab2Dlg::extendPoint(CvPoint3D32f first, CvPoint3D32f second, int value)
{
	float length;
	length = sqrt(pow((first.x - second.x), 2) + pow((first.y - second.y), 2));

	CvPoint3D32f extendPoint;
	float x = (second.x - first.x);
	float y = (second.y - first.y);
	extendPoint = cvPoint3D32f((x / length * value) + first.x, (y / length * value) + first.y , 18);

	return extendPoint;
}


//夾取整合
void tab2Dlg::OnBnClickedButtonstartgrab()
{
		CMFC_SystemDlg mainDlg;
		switch (mainDlg.workSpace1Color)
		{
		case 0://如果分類區沒東西
		{
			//1.pushPoint[0] 的 x y
			mainDlg.packetCreat_toPoint(m_pushPoint[0].x, m_pushPoint[0].y, 100, m_degree);
			//2.pushPoint[0] 的 z
			mainDlg.packetCreat_toPoint(m_pushPoint[0].x, m_pushPoint[0].y, m_pushPoint[0].z, m_degree);
			//3.pushPoint[1]
			mainDlg.packetCreat_toPoint(m_pushPoint[1].x, m_pushPoint[1].y, m_pushPoint[1].z, m_degree);

			if (priority < 6)//表示不是用推的 有東西要夾
			{
				Sleep(5000);
				mainDlg.grab();
				Sleep(500);
			}
			//5. Z起來
			mainDlg.packetCreat_toPoint(m_pushPoint[1].x, m_pushPoint[1].y, 100, m_degree);
			
			if (priority < 6)//表示不是用推的 有東西要放在分類區
			{
				//6. to 分類區
				mainDlg.packetCreat_toPoint(400, -180, 100, -90);
				//7. down
				mainDlg.packetCreat_toPoint(400, -180, 3, -90);

				//放
				Sleep(4500);
				mainDlg.grab();
				Sleep(500);

				// up
				mainDlg.packetCreat_toPoint(400, -180, 50, -90);
			}

			//8. to home
			mainDlg.packetCreat_toPoint(550, 0, 133, 0);
		}
		break;
		case 1: //分類區 red
		{
			//to 分類區
			mainDlg.packetCreat_toPoint(400, -180, 100, -90);
			//down
			mainDlg.packetCreat_toPoint(400, -180, 3, -90);
			//grab
			Sleep(3500);
			mainDlg.grab();
			Sleep(500);
			// up
			mainDlg.packetCreat_toPoint(400, -180, 50, -90);
			
			//to red
			mainDlg.packetCreat_toPoint(441, 165, 50, 0);
			//to red Down
			mainDlg.packetCreat_toPoint(441, 165, 18, 0);

			//放
			Sleep(4000);
			mainDlg.grab();
			Sleep(500);
			
			//up
			mainDlg.packetCreat_toPoint(441, 165, 136, 0);
			
			//回家
			mainDlg.packetCreat_toPoint(550,0, 136,0);
		}
		break;
		case 2: //分類區 green
		{
			//to 分類區
			mainDlg.packetCreat_toPoint(400, -180, 100, -90);
			Sleep(500);
			//down
			mainDlg.packetCreat_toPoint(400, -180, 3, -90);
		
			//grab
			Sleep(3500);
			mainDlg.grab();
			Sleep(500);
			// up
			mainDlg.packetCreat_toPoint(400, -180, 50, -90);
		
			//to green
			mainDlg.packetCreat_toPoint(366, 165, 50, 0);
			//to green Down
			mainDlg.packetCreat_toPoint(366, 165, 18, 0);

			//放掉
			Sleep(4000);
			mainDlg.grab();
			Sleep(500);
			//up
			mainDlg.packetCreat_toPoint(366, 165, 136, 0);
			
			//回家
			mainDlg.packetCreat_toPoint(550, 0, 136, 0);
		}
		break;
		case 3: //分類區 blue
		{
			//to 分類區
			mainDlg.packetCreat_toPoint(400, -180, 100, -90);
		
			//down
			mainDlg.packetCreat_toPoint(400, -180, 3, -90);
			//grab
			Sleep(4500);
			mainDlg.grab();
			Sleep(500);
			// up
			mainDlg.packetCreat_toPoint(400, -180, 50, -90);
			//to blue
			mainDlg.packetCreat_toPoint(291, 165, 50, 0);
			//to blue Down
			mainDlg.packetCreat_toPoint(291, 165, 18, 0);

			//放掉
			Sleep(4000);
			mainDlg.grab();
			Sleep(500);
			//up
			mainDlg.packetCreat_toPoint(291, 165, 136, -90);
			
			//回家
			mainDlg.packetCreat_toPoint(550, 0, 136, 0);
			
			
		}
		break;
		case 4: //分類區 brown
		{
			//to 分類區
			mainDlg.packetCreat_toPoint(400, -180, 100, -90);
			
			//down
			mainDlg.packetCreat_toPoint(400, -180, 3, -90);
			//grab
			Sleep(3500);
			mainDlg.grab();
			Sleep(500);
			// up
			mainDlg.packetCreat_toPoint(400, -180, 50, -90);
			
			//to trash
			mainDlg.packetCreat_toPoint(290, 260, 50, -90);
		
			//放掉
			Sleep(5000);
			mainDlg.grab();
			Sleep(500);
			//回家
			mainDlg.packetCreat_toPoint(550, 0, 136, 0);
		}
		break;
		}



}

void tab2Dlg::OnBnClickedButtonautobinpick()
{
	//1. 偵測多邊形
	ObjectCounter = 0;
	m_list_detectNum.ResetContent();
	m_list_priority.ResetContent();
	//SetDlgItemText(IDC_EDIT1_pixel_corner1, _T("0"));
	//SetDlgItemText(IDC_EDIT1_pixel_corner2, _T("0"));
	//SetDlgItemText(IDC_EDIT1_pixel_corner3, _T("0"));
	//SetDlgItemText(IDC_EDIT1_pixel_corner4, _T("0"));
	//SetDlgItemText(IDC_EDIT1_pixel_cornerCenter, _T("0"));

	system("del .\\ApproxPolyPics\\*.jpg");
	CMFC_SystemDlg mainDlg;
	IplImage*  img_CannyRoi = cvCreateImage(cvGetSize(mainDlg.sImg_CannyRoiS), IPL_DEPTH_8U, 1);
	cvCopy(mainDlg.sImg_CannyRoiS, img_CannyRoi);
	//影像處理 閉合破碎
	IplConvKernel *pKernel = NULL;
	pKernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, NULL);
	cvDilate(img_CannyRoi, img_CannyRoi, cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, NULL), 1);
	cvErode(img_CannyRoi, img_CannyRoi, pKernel, 1);

	ApproxPoly(img_CannyRoi);

	//2. 所有多邊形全部選擇一次
	int number = m_list_detectNum.GetCount();
	for (int i = number - 1; i >= 0; i--)
	{
		priority = grabDecision(i, &m_pushPoint[0], &m_degree);
		CString priority_str;
		priority_str.Format(_T("%d"), priority);
		m_list_priority.InsertString(0, priority_str);
		UpdateData(0);
	}

	//3. 將優先權最高的做夾取
	SendMessage(WM_COMMAND, IDC_BUTTON_topPriority, 0);
	
	//4. 重複 2 3 直到 優先權皆為0

	//5. (case4 的處理)

}


//找最大優先權 並 顯示
void tab2Dlg::OnBnClickedButtontoppriority()
{
	CString listPriorityMax_str;
	m_list_priority.GetText(0, listPriorityMax_str);
	int listPriorityMin = _ttoi(listPriorityMax_str);
	CString listPriority_str;
	//找最大的優先 不包含0(不夾取的優先值)
	int numMin = 0;

	int count = m_list_detectNum.GetCount();
	if (count == 0)
	{
		MessageBox(L"沒有偵測到貨品");
		return;
	}
	for (int i = 0; i < count; i++)
	{
		m_list_priority.GetText(i, listPriority_str);
		int listPriority = _ttoi(listPriority_str);
		if (listPriority < listPriorityMin)
		{
			listPriorityMin = listPriority;
			numMin = i;
		}
	}
	if (listPriorityMin != 10) //若有可以一般處理的東西 先處理
	{
		//將標示放在最大優先的地方
		m_list_priority.SetCurSel(numMin);
		m_list_detectNum.SetCurSel(numMin);


		priority = grabDecision(numMin, &m_pushPoint[0], &m_degree);


		//顯示pushPoint值
		CString Pos;
		//1.pushPoint[0] 的 x y
		Pos.Format(_T("%f"), m_pushPoint[0].x);
		SetDlgItemText(IDC_EDIT_Wx_Pos1, Pos);
		Pos.Format(_T("%f"), m_pushPoint[0].y);
		SetDlgItemText(IDC_EDIT_Wy_Pos1, Pos);
		Pos.Format(_T("%f"), 100.00);
		SetDlgItemText(IDC_EDIT_Wz_Pos1, Pos);
		Pos.Format(_T("%f"), m_degree);
		SetDlgItemText(IDC_EDIT_Wt_Pos1, Pos);
		//2.pushPoint[0] 的 z
		Pos.Format(_T("%f"), m_pushPoint[0].x);
		SetDlgItemText(IDC_EDIT_Wx_Pos2, Pos);
		Pos.Format(_T("%f"), m_pushPoint[0].y);
		SetDlgItemText(IDC_EDIT_Wy_Pos2, Pos);
		Pos.Format(_T("%f"), m_pushPoint[0].z);
		SetDlgItemText(IDC_EDIT_Wz_Pos2, Pos);
		Pos.Format(_T("%f"), m_degree);
		SetDlgItemText(IDC_EDIT_Wt_Pos2, Pos);
		//3.pushPoint[1]
		Pos.Format(_T("%f"), m_pushPoint[1].x);
		SetDlgItemText(IDC_EDIT_Wx_Pos3, Pos);
		Pos.Format(_T("%f"), m_pushPoint[1].y);
		SetDlgItemText(IDC_EDIT_Wy_Pos3, Pos);
		Pos.Format(_T("%f"), m_pushPoint[1].z);
		SetDlgItemText(IDC_EDIT_Wz_Pos3, Pos);
		Pos.Format(_T("%f"), m_degree);
		SetDlgItemText(IDC_EDIT_Wt_Pos3, Pos);
	}
	else if (listPriorityMin == 10) //(case4 or case5
	{
		//將標示放在最大優先的地方
		m_list_priority.SetCurSel(numMin);
		m_list_detectNum.SetCurSel(numMin);


		SpecilGrabDecision(numMin, &m_pushPoint[0], &m_degree);

		//顯示pushPoint值
		CString Pos;
		//1.pushPoint[0] 的 x y
		Pos.Format(_T("%f"), m_pushPoint[0].x);
		SetDlgItemText(IDC_EDIT_Wx_Pos1, Pos);
		Pos.Format(_T("%f"), m_pushPoint[0].y);
		SetDlgItemText(IDC_EDIT_Wy_Pos1, Pos);
		Pos.Format(_T("%f"), 100.00);
		SetDlgItemText(IDC_EDIT_Wz_Pos1, Pos);
		Pos.Format(_T("%f"), m_degree);
		SetDlgItemText(IDC_EDIT_Wt_Pos1, Pos);
		//2.pushPoint[0] 的 z
		Pos.Format(_T("%f"), m_pushPoint[0].x);
		SetDlgItemText(IDC_EDIT_Wx_Pos2, Pos);
		Pos.Format(_T("%f"), m_pushPoint[0].y);
		SetDlgItemText(IDC_EDIT_Wy_Pos2, Pos);
		Pos.Format(_T("%f"), m_pushPoint[0].z);
		SetDlgItemText(IDC_EDIT_Wz_Pos2, Pos);
		Pos.Format(_T("%f"), m_degree);
		SetDlgItemText(IDC_EDIT_Wt_Pos2, Pos);
		//3.pushPoint[1]
		Pos.Format(_T("%f"), m_pushPoint[1].x);
		SetDlgItemText(IDC_EDIT_Wx_Pos3, Pos);
		Pos.Format(_T("%f"), m_pushPoint[1].y);
		SetDlgItemText(IDC_EDIT_Wy_Pos3, Pos);
		Pos.Format(_T("%f"), m_pushPoint[1].z);
		SetDlgItemText(IDC_EDIT_Wz_Pos3, Pos);
		Pos.Format(_T("%f"), m_degree);
		SetDlgItemText(IDC_EDIT_Wt_Pos3, Pos);
	}
	
}


void tab2Dlg::OnBnClickedButtongopushpoint1()
{
	CMFC_SystemDlg mainDlg;
	mainDlg.packetCreat_toPoint(m_pushPoint[0].x, m_pushPoint[0].y, 100, m_degree);
}
void tab2Dlg::OnBnClickedButtongopushpoint2()
{
	CMFC_SystemDlg mainDlg;
	mainDlg.packetCreat_toPoint(m_pushPoint[0].x, m_pushPoint[0].y, m_pushPoint[0].z, m_degree);
}
void tab2Dlg::OnBnClickedButtongopushpoint3()
{
	CMFC_SystemDlg mainDlg;
	mainDlg.packetCreat_toPoint(m_pushPoint[1].x, m_pushPoint[1].y, m_pushPoint[1].z, m_degree);
}

bool debugMode = true;
void tab2Dlg::OnBnClickedButtondebugmode()
{
	if (debugMode == true)
	{
	GetDlgItem(IDC_BUTTON_topPriority)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_goPushPoint1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_goPushPoint2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_goPushPoint3)->ShowWindow(SW_HIDE);
	debugMode = false;
	}
	else if(debugMode == false)
	{
		GetDlgItem(IDC_BUTTON_topPriority)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_goPushPoint1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_goPushPoint2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_goPushPoint3)->ShowWindow(SW_SHOW);
		debugMode = true;
	}

}
