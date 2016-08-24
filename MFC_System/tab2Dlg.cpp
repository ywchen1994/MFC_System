// tab2Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_System.h"
#include "tab2Dlg.h"
#include "afxdialogex.h"

#include"MFC_SystemDlg.h"
class CMFC_SystemDlg;

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
	DDX_Control(pDX, IDC_COMBO_objList, m_combo_objList);
}


BEGIN_MESSAGE_MAP(tab2Dlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_COMBO_objList, &tab2Dlg::OnCbnSelchangeComboobjlist)
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
	IplImage* CannyRoi_C1 = nullptr;
	while (1)
	{
		CannyRoi_C1 = cvCreateImage(cvGetSize(mainDlg.img_CannyRoiS), IPL_DEPTH_8U, 1);
		cvCopy(mainDlg.img_CannyRoiS, CannyRoi_C1);
		CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x- mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
		cvSetImageROI(CannyRoi_C1, rect);
		hWnd->GetDlgItem(IDC_IMAGE_CannyRoi)->SetWindowPos(NULL, 10, 10, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);
		hWnd->ShowImage(CannyRoi_C1, hWnd->GetDlgItem(IDC_IMAGE_CannyRoi), 1,cvSize(2*(mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2*(mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
		cvReleaseImage(&CannyRoi_C1);
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
		m_combo_objList.ResetContent();
		SetDlgItemText(IDC_EDIT1_pixel_corner1, _T("0"));
		SetDlgItemText(IDC_EDIT1_pixel_corner2, _T("0"));
		SetDlgItemText(IDC_EDIT1_pixel_corner3, _T("0"));
		SetDlgItemText(IDC_EDIT1_pixel_corner4, _T("0"));
		SetDlgItemText(IDC_EDIT1_pixel_cornerCenter, _T("0"));

		system("del .\\ApproxPolyPics\\*.jpg");
		CMFC_SystemDlg mainDlg;
		IplImage*  img_CannyRoi = cvCreateImage(cvGetSize(mainDlg.img_CannyRoiS), IPL_DEPTH_8U, 1);
		cvCopy(mainDlg.img_CannyRoiS, img_CannyRoi);
		//影像處理 閉合破碎
		IplConvKernel *pKernel = NULL;
		pKernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_CROSS, NULL);
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
		m_combo_objList.InsertString(ObjectCounter, objNum);
		m_combo_objList.SetCurSel(ObjectCounter);
		/***************************************/
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

void tab2Dlg::OnCbnSelchangeComboobjlist()
{
	int select = m_combo_objList.GetCurSel();
	CString Pos;

	char path[100];
	sprintf(path, "ApproxPolyPics/inside%d.jpg", select);
	IplImage* ImageLoad = cvLoadImage(path, 0);
	IplImage* ImgApproxPolyLoad = cvCreateImage(cvGetSize(ImageLoad), ImageLoad->depth, 1);
	cvCopy(ImageLoad, ImgApproxPolyLoad);
	cvReleaseImage(&ImageLoad);

	//找中心點
	CMFC_SystemDlg mainDlg;
	mainDlg.Center = GetCentroid(ImgApproxPolyLoad);

	//找角點 跟 角點個數  (CornerPoint 已經放入)
	IplImage* imageCorner = cvCreateImage(cvGetSize(ImgApproxPolyLoad), ImgApproxPolyLoad->depth, 3);
	int cornerNum;
	CornerDetection(ImgApproxPolyLoad, imageCorner, &cornerNum);

	//若角點為4個   //若不是 則單純顯示圖片(不畫出角點)
	if (cornerNum == 4)
	{
		cvCvtColor(ImgApproxPolyLoad, imageCorner, CV_GRAY2RGB);

		//-----排序 CornerPoint x 小到大(左到右)--------------------
		for (int n = 0; n < 3; n++)//做三次
			for (int i = 0; i < 3; i++)//0跟1比......2跟3比
				if (CornerPoint[i].x > CornerPoint[i + 1].x)
				{
					//交換
					CvPoint temp;
					temp = CornerPoint[i];
					CornerPoint[i] = CornerPoint[i+1];
					CornerPoint[i + 1] = temp;
				}
		//-------------------------------------------------------

		//-------------------
		CvPoint2D32f mediumPoint[4];
		mediumPoint[0] = cvPoint2D32f(0.5*(CornerPoint[0].x + CornerPoint[1].x), 0.5*(CornerPoint[0].y + CornerPoint[1].y));
		mediumPoint[1] = cvPoint2D32f(0.5*(CornerPoint[1].x + CornerPoint[3].x), 0.5*(CornerPoint[1].y + CornerPoint[3].y));
		mediumPoint[2] = cvPoint2D32f(0.5*(CornerPoint[3].x + CornerPoint[2].x), 0.5*(CornerPoint[3].y + CornerPoint[2].y));
		mediumPoint[3] = cvPoint2D32f(0.5*(CornerPoint[2].x + CornerPoint[0].x), 0.5*(CornerPoint[2].y + CornerPoint[0].y));
		float length[4];
		length[0] = sqrt(pow((mainDlg.Center.x - mediumPoint[0].x), 2) + pow((mainDlg.Center.y - mediumPoint[0].y), 2));
		length[1] = sqrt(pow((mainDlg.Center.x - mediumPoint[1].x), 2) + pow((mainDlg.Center.y - mediumPoint[1].y), 2));
		length[2] = sqrt(pow((mainDlg.Center.x - mediumPoint[2].x), 2) + pow((mainDlg.Center.y - mediumPoint[2].y), 2));
		length[3] = sqrt(pow((mainDlg.Center.x - mediumPoint[3].x), 2) + pow((mainDlg.Center.y - mediumPoint[3].y), 2));
		const int radius1 = 35;
		CvPoint2D32f detectPoint1[4];
		for (int i = 0; i < 4; i++)
		{
			float x = (mediumPoint[i].x - mainDlg.Center.x);
			float y = (mediumPoint[i].y - mainDlg.Center.y);
			detectPoint1[i] = cvPoint2D32f((x / length[i] * radius1) + mainDlg.Center.x, (y / length[i] * radius1) + mainDlg.Center.y);
		}


		//const int radius2 = 35;
		//CvPoint2D32f detectPoint2[4];
		//for (int i = 0; i < 4; i++)
		//	detectPoint2[i] = cvPoint2D32f((radius2 / length[i])*(mediumPoint[i].x - mainDlg.Center.x) + mainDlg.Center.x, (radius2 / length[i])*(mediumPoint[i].y - mainDlg.Center.y) + mainDlg.Center.y);
		
		CvPoint3D32f outPoint_W[4];
		for (int i = 0; i < 4; i++)
			Img2SCARA(detectPoint1[i].x, detectPoint1[i].y, &outPoint_W[i].x, &outPoint_W[i].y, &outPoint_W[i].z);

		for (int i = 0; i < 4; i++)
			cvCircle(imageCorner, cvPoint (detectPoint1[i].x, detectPoint1[i].y), 2, CV_RGB(0, 0, 255), CV_FILLED);
		//-------------------

		for (int i = 0; i < 4; i++)
			cvCircle(imageCorner, CornerPoint[i], 3, CV_RGB(0, 255, 0), CV_FILLED);

		cvCircle(imageCorner, mainDlg.Center, 3, CV_RGB(255, 0, 0), CV_FILLED);


		Pos.Format(_T("%d,%d"), CornerPoint[0].x, CornerPoint[0].y);
		GetDlgItem(IDC_EDIT1_pixel_corner1)->SetWindowTextW(Pos);

		Pos.Format(_T("%d,%d"), CornerPoint[1].x, CornerPoint[1].y);
		GetDlgItem(IDC_EDIT1_pixel_corner2)->SetWindowTextW(Pos);

		Pos.Format(_T("%d,%d"), CornerPoint[2].x, CornerPoint[2].y);
		GetDlgItem(IDC_EDIT1_pixel_corner3)->SetWindowTextW(Pos);

		Pos.Format(_T("%d,%d"), CornerPoint[3].x, CornerPoint[3].y);
		GetDlgItem(IDC_EDIT1_pixel_corner4)->SetWindowTextW(Pos);

		Pos.Format(_T("%d,%d"), mainDlg.Center.x, mainDlg.Center.y);
		GetDlgItem(IDC_EDIT1_pixel_cornerCenter)->SetWindowTextW(Pos);

		//showimage
		CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
		cvSetImageROI(imageCorner, rect);
		ShowImage(imageCorner, GetDlgItem(IDC_IMAGE_ApproxPoly), 3, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
		m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);

		//內縮角點
		for (int i = 0; i < 4; i++)
		{
			CornerPoint[i].x = (mainDlg.Center.x + 4 * CornerPoint[i].x) / 5;
			CornerPoint[i].y = (mainDlg.Center.y + 4 * CornerPoint[i].y) / 5;
		}

		//5個點 轉成 SCARA座標
		CvPoint3D32f ObjectPoint_World[5];
		Img2SCARA(mainDlg.Center.x, mainDlg.Center.y, &ObjectPoint_World[0].x, &ObjectPoint_World[0].y, &ObjectPoint_World[0].z);
		Img2SCARA(CornerPoint[0].x, CornerPoint[0].y, &ObjectPoint_World[1].x, &ObjectPoint_World[1].y, &ObjectPoint_World[1].z);
		Img2SCARA(CornerPoint[1].x, CornerPoint[1].y, &ObjectPoint_World[2].x, &ObjectPoint_World[2].y, &ObjectPoint_World[2].z);
		Img2SCARA(CornerPoint[2].x, CornerPoint[2].y, &ObjectPoint_World[3].x, &ObjectPoint_World[3].y, &ObjectPoint_World[3].z);
		Img2SCARA(CornerPoint[3].x, CornerPoint[3].y, &ObjectPoint_World[4].x, &ObjectPoint_World[4].y, &ObjectPoint_World[4].z);

		//排序4個角點
		CornerPointSort(&ObjectPoint_World[0]);
		//判斷擺放case
		int ObjCase = caseClassify(&ObjectPoint_World[0]);
		//判斷推的問題

		


		switch (ObjCase)
		{
		case 1:
			mainDlg.SetPos(ObjectPoint_World[0].x);

			break;
		case 2:

	
			break;
		case 3:
			//pushClassify();
			break;
		default:
			break;
		}
	}
	else//若角點不是四個 則單純顯示圖片(不畫出角點)
	{
		//showimage
		CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
		cvSetImageROI(ImgApproxPolyLoad, rect);
		ShowImage(ImgApproxPolyLoad, GetDlgItem(IDC_IMAGE_ApproxPoly), 1, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
		m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);
	}

	cvReleaseImage(&ImgApproxPolyLoad);
	cvReleaseImage(&imageCorner);
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
	cvCornerHarris(src, dst, 5, 5);
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

		CvPoint corner = GetCentroid(Sec);
		cvCircle(dst, corner, 0, CV_RGB(255, 255, 255), CV_FILLED);
		CornerPoint[CornerCounter] = corner;
		CornerCounter++;

		cvCopy(Sec, First);
	}

	*cornerCount = CornerCounter;
	cvReleaseImage(&Sec);
	cvReleaseImage(&First);
}

CvPoint tab2Dlg::GetCentroid(IplImage *src)
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
	*SCARAX = mainDlg.CamRefY  + 280 - mainDlg.kinect.CameraY * 1000;

	*SCARAY = mainDlg.CamRefX + 410 - mainDlg.kinect.CameraX * 1000;

	*SCARAZ = mainDlg.DepthPointsBase[x][y] - mainDlg.kinect.CameraZ * 1000;

	//*SCARAZ = mainDlg.CamRefZ - mainDlg.kinect.CameraZ * 1000;

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
void tab2Dlg::CornerPointSort(CvPoint3D32f* ObjectPoint)
{
	CvPoint3D32f ObjectPointCopy[5] = {};
	memcpy(ObjectPointCopy, ObjectPoint, 5*3*sizeof(ObjectPoint));

	float fitmp1, fitmp2, fitmp3, fitmp4;

	fitmp1 = atan2((ObjectPointCopy[1].y - ObjectPointCopy[0].y), (ObjectPointCopy[1].x - ObjectPointCopy[0].x)) * 180 / CV_PI;
	if (fitmp1 < 0)fitmp1 = fitmp1 + 360;
	phi[0] = fitmp1;
	
	fitmp2 = atan2((ObjectPointCopy[2].y - ObjectPointCopy[0].y), (ObjectPointCopy[2].x - ObjectPointCopy[0].x)) * 180 / CV_PI;
	if (fitmp2 < 0)fitmp2 = fitmp2 + 360;
	phi[1] = fitmp2;

	fitmp3 = atan2((ObjectPointCopy[3].y - ObjectPointCopy[0].y), (ObjectPointCopy[3].x - ObjectPointCopy[0].x)) * 180 / CV_PI;
	if (fitmp3 < 0)fitmp3 = fitmp3 + 360;
	phi[2] = fitmp3;

	fitmp4 = atan2((ObjectPointCopy[4].y - ObjectPointCopy[0].y), (ObjectPointCopy[4].x - ObjectPointCopy[0].x)) * 180 / CV_PI;
	if (fitmp4 < 0)fitmp4 = fitmp4 + 360;
	phi[3] = fitmp4;

	qsort(phi, 4, sizeof(phi[0]), cmp);

	ObjectPoint[FindElement(fitmp1)+1] = ObjectPointCopy[1];

	ObjectPoint[FindElement(fitmp2)+1] = ObjectPointCopy[2];

	ObjectPoint[FindElement(fitmp3)+1] = ObjectPointCopy[3];
	
	ObjectPoint[FindElement(fitmp4)+1] = ObjectPointCopy[4];

}

int tab2Dlg::caseClassify(CvPoint3D32f* objPoint)
{
	const int correctZ = 0;
	float slope12 = (objPoint[2].z - correctZ - objPoint[1].z);
	float slope14 = (objPoint[4].z - objPoint[1].z);

	//float vector12 = (objPoint[2].z + correctZ - objPoint[1].z) / (sqrt(pow(abs(objPoint[2].x - objPoint[1].x), 2) + pow(abs(objPoint[2].y - objPoint[1].y), 2)));
	//float vector14 = (objPoint[4].z - objPoint[1].z) / (sqrt(pow(abs(objPoint[4].x - objPoint[1].x), 2) + pow(abs(objPoint[4].y - objPoint[1].y), 2)));
	//float slope12 = (objPoint[2].z + correctZ - objPoint[1].z) / vector12;
	//float slope14 = (objPoint[4].z - objPoint[1].z) / vector14;

	float flatAllowError = 8;
	if ((slope12 > 0 - flatAllowError && slope12 < 0 + flatAllowError) && (slope14 > 0 - flatAllowError && slope14 < 0 + flatAllowError))
		if (objPoint[0].z > 25)
			return 2;
		else
			return 1;

	if ((slope12 < 0 - flatAllowError || slope12 > 0 + flatAllowError) || (slope14 < 0 - flatAllowError || slope14 > 0 + flatAllowError))
		return 3;

}
int tab2Dlg::pushClassify(CvPoint3D32f* objPoint)
{
	float m12 = (objPoint[2].y - objPoint[1].y) / (objPoint[2].x - objPoint[1].x);
	return 0;
}
void tab2Dlg::findrange(CvPoint3D32f* objPoint,int radius)
{
	//radius 40/2 pixel
	CvPoint2D32f detectPoint[8];
	CvPoint2D32f mediumPoint = cvPoint2D32f(0.5*(objPoint[0].x + objPoint[3].x), 0.5*(objPoint[0].y + objPoint[3].y));
	float length = sqrt(pow((mediumPoint.x - objPoint[0].x), 2) + pow((mediumPoint.y - objPoint[0].y), 2));
	float ratio = radius / length;
	detectPoint[0] = cvPoint2D32f(ratio*(mediumPoint.x- objPoint[0].x)+ objPoint[0].x, ratio*(mediumPoint.y - objPoint[0].y)+ objPoint[0].y);
	for (int i = 1; i < 5; i++)
	{
		length = sqrt(pow((objPoint[i].x - objPoint[0].x), 2) + pow((objPoint[i].y - objPoint[0].y), 2));
		ratio = radius / length;
		detectPoint[i]= cvPoint2D32f(ratio*(objPoint[i].x - objPoint[0].x) + objPoint[0].x, ratio*(objPoint[i].y - objPoint[0].y) + objPoint[0].y);
	}
}
