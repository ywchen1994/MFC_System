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

	m_img_CannyRoi.SetWindowPos(NULL, 10, 10, 320, 240, SWP_SHOWWINDOW);
	m_img_approxPoly.SetWindowPos(NULL, 10, 10+240, 480,360, SWP_SHOWWINDOW);
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
		CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,320,240 };
		cvSetImageROI(CannyRoi_C1, rect);
		hWnd->ShowImage(CannyRoi_C1, hWnd->GetDlgItem(IDC_IMAGE_CannyRoi), 1,cvSize(480,360));
		cvReleaseImage(&CannyRoi_C1);
	}
}


void tab2Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.x > (10) && point.x < (10 + 320) && point.y > 10 && point.y < (10 + 240))
	{
		m_threadPara.m_case = 0;
		m_threadPara.hWnd = m_hWnd;
		m_lpThread = AfxBeginThread(&tab2Dlg::MythreadFun, (LPVOID)&m_threadPara);
	}
	if (point.x > (10) && point.x < (10 + 320) && point.y > 10+240 && point.y < (10 + 240+240))
	{
		ObjectCounter = 0;
		system("del .\\ApproxPolyPics\\*.jpg");
		CMFC_SystemDlg mainDlg;
		IplImage*  img_CannyRoi = cvCreateImage(cvGetSize(mainDlg.img_CannyRoiS), IPL_DEPTH_8U, 1);
		cvCopy(mainDlg.img_CannyRoiS, img_CannyRoi);
		ImageProcessing(img_CannyRoi);
		ApproxPoly(img_CannyRoi);
		
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void tab2Dlg::ImageProcessing(IplImage *img_roi)//edge為Canny
{
	IplConvKernel *pKernel = NULL;
	pKernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, NULL);

	cvSmooth(img_roi, img_roi, CV_BLUR_NO_SCALE, 3, 3);
	cvSmooth(img_roi, img_roi, CV_BLUR_NO_SCALE, 3, 3);
	cvThreshold(img_roi, img_roi, 200, 255, CV_THRESH_BINARY);

	cvErode(img_roi, img_roi, pKernel, 1);
	cvSmooth(img_roi, img_roi, CV_BLUR_NO_SCALE, 3, 3);
	cvErode(img_roi, img_roi, pKernel, 2);

	cvSmooth(img_roi, img_roi, CV_BLUR_NO_SCALE, 3, 3);
	cvErode(img_roi, img_roi, pKernel, 1);
	cvSmooth(img_roi, img_roi, CV_BLUR_NO_SCALE, 3, 3);
	cvErode(img_roi, img_roi, pKernel, 1);
	cvSmooth(img_roi, img_roi, CV_BLUR_NO_SCALE, 3, 3);
	cvErode(img_roi, img_roi, pKernel, 1);
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
			mcont = cvApproxPoly(cont, sizeof(CvContour), storage1, CV_POLY_APPROX_DP, cvContourPerimeter(cont)*0.02, 0);
			cvDrawContours(Sec, mcont, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), 1, 1, 8, cvPoint(0, 0));

			cvAbsDiff(Sec, First, Sec);

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
	// TODO: 在此加入控制項告知處理常式程式碼
	int select = m_combo_objList.GetCurSel();
	CString Pos;

	char path[100];
	sprintf(path, "ApproxPolyPics/inside%d.jpg", select);
	IplImage* ImageLoad= cvLoadImage(path, 0);
	IplImage* ImgApproxPolyLoad = cvCreateImage(cvGetSize(ImageLoad), ImageLoad->depth,1);
	cvCopy(ImageLoad, ImgApproxPolyLoad);
	cvReleaseImage(&ImageLoad);
	
	IplImage* imageCorner = cvCreateImage(cvGetSize(ImgApproxPolyLoad), ImgApproxPolyLoad->depth, 3);
	CvPoint Center = GetCenterPoint(ImgApproxPolyLoad);
	CornerDetection(ImgApproxPolyLoad, imageCorner,0);
	if (CornerCounter==4) {
		cvCvtColor(ImgApproxPolyLoad,imageCorner,CV_GRAY2RGB);
		for (int i = 0; i < 4;i++){
			cvCircle(imageCorner, CornerPoint[i], 3, CV_RGB(0, 255, 255), CV_FILLED);
		}
		Pos.Format(_T("%d,%d"), CornerPoint[0].x, CornerPoint[0].y);
		GetDlgItem(IDC_EDIT1_pixel_corner1)->SetWindowTextW(Pos);

		Pos.Format(_T("%d,%d"), CornerPoint[1].x, CornerPoint[1].y);
		GetDlgItem(IDC_EDIT1_pixel_corner2)->SetWindowTextW(Pos);

		Pos.Format(_T("%d,%d"), CornerPoint[2].x, CornerPoint[2].y);
		GetDlgItem(IDC_EDIT1_pixel_corner3)->SetWindowTextW(Pos);
		
		Pos.Format(_T("%d,%d"), CornerPoint[3].x, CornerPoint[3].y);
		GetDlgItem(IDC_EDIT1_pixel_corner4)->SetWindowTextW(Pos);

		Pos.Format(_T("%d,%d"), Center.x, Center.y);
		GetDlgItem(IDC_EDIT1_pixel_cornerCenter)->SetWindowTextW(Pos);

		ShowImage(imageCorner, GetDlgItem(IDC_IMAGE_ApproxPoly),3, cvSize(640, 480));
	}
	else
	{
		ShowImage(ImgApproxPolyLoad, GetDlgItem(IDC_IMAGE_ApproxPoly), 1, cvSize(640, 480));
	}
  
	cvReleaseImage(&ImgApproxPolyLoad);
	cvReleaseImage(&imageCorner);
}
void tab2Dlg::CornerDetection(IplImage* edge_roi, IplImage *CornerImg_Modified, int radius)//這裡只做Harris由於Harris的角點是範圍的所以才要有CornerFind
{
	IplImage* src = nullptr;
	src = cvCreateImage(cvGetSize(edge_roi), edge_roi->depth, 1);
	cvCopy(edge_roi, src);


	IplImage *dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
	IplImage *dst_8U = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);


	cvSetZero(dst);
	cvCornerHarris(src, dst, 7, 3);
	cvConvertScale(dst, dst_8U, 1000);
	cvThreshold(dst_8U, dst_8U, 10, 255, CV_THRESH_BINARY);


	CornerFind(dst_8U, CornerImg_Modified, radius);




	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst_8U);

}
void tab2Dlg::CornerFind(IplImage* Cornerimage, IplImage *dst, int radius)
{
	IplImage *Sec = cvCreateImage(cvGetSize(Cornerimage), Cornerimage->depth, 3);
	IplImage *First = cvCreateImage(cvGetSize(Cornerimage), Cornerimage->depth, 3);

	cvSetZero(First);
	cvSetZero(Sec);

	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq*contours;
	cvFindContours(Cornerimage, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP,
		CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));


	for (; contours != 0; contours = contours->h_next)
	{
		cvDrawContours(Sec, contours, CV_RGB(255, 255, 255), CV_RGB(0, 0, 0), -1, CV_FILLED, 8, cvPoint(0, 0));
		cvAbsDiff(Sec, First, Sec);
		GetCornerPoint(Sec, dst, radius);
		cvCopy(Sec, First);
	}
	cvReleaseImage(&Sec);
	cvReleaseImage(&First);

}

CvPoint tab2Dlg::GetCornerPoint(IplImage *src, IplImage *dst, int radius)
{
	int x0 = 0, y0 = 0, sum = 0;
	CvPoint corner;
	CvScalar pixel;
	for (int i = 0; i < src->width; i++) {
		for (int j = 0; j < src->height; j++) {
			pixel = cvGet2D(src, j, i);
			if (pixel.val[0] > 0)
			{
				x0 = x0 + i;
				y0 = y0 + j;
				sum = sum + 1;
			}
		}
	}
	if (sum == 0) {
		corner.x = 0;
		corner.y = 0;
		return corner;
	}
	corner.x = x0 / sum;
	corner.y = y0 / sum;
	cvCircle(dst, corner, radius, CV_RGB(255, 255, 255), CV_FILLED);
	if (radius == 0)
	{
		CornerPoint[CornerCounter] = corner;

		CornerCounter++;
	}
	return corner;
}
CvPoint tab2Dlg::GetCenterPoint(IplImage *src)
{
	int x0 = 0, y0 = 0, sum = 0;
	CvPoint center;
	CvScalar pixel;
	for (int i = 0; i < src->width; i++) {
		for (int j = 0; j < src->height; j++) {
			pixel = cvGet2D(src, j, i);
			if (pixel.val[0] > 0)
			{
				x0 = x0 + i;
				y0 = y0 + j;
				sum = sum + 1;
			}
		}
	}
	if (sum == 0) {
		center.x = 0;
		center.y = 0;
		return center;
	}
	center.x = x0 / sum;
	center.y = y0 / sum;
	return center;
}
