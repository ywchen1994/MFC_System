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
					CornerPoint[i] = CornerPoint[i + 1];
					CornerPoint[i + 1] = temp;
				}
		//-------------------------------------------------------


		//算出外圈的點 供推方塊使用
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
		CvPoint2D32f outPoint1[4];
		for (int i = 0; i < 4; i++)
		{
			float x = (mediumPoint[i].x - mainDlg.Center.x);
			float y = (mediumPoint[i].y - mainDlg.Center.y);
			outPoint1[i] = cvPoint2D32f((x / length[i] * radius1) + mainDlg.Center.x, (y / length[i] * radius1) + mainDlg.Center.y);
		}

		//2 3 對調 使 點 順時針照順序
		CvPoint temp;
		temp = CornerPoint[2];
		CornerPoint[2] = CornerPoint[3];
		CornerPoint[3] = temp;

		//0 1 要是長邊
		float distance1, distance2;
		distance1 = pow((CornerPoint[1].x - CornerPoint[0].x), 2) + pow((CornerPoint[1].y - CornerPoint[0].y), 2);
		distance2 = pow((CornerPoint[3].x - CornerPoint[0].x), 2) + pow((CornerPoint[3].y - CornerPoint[0].y), 2);
		if (distance1 < distance2)
		{
			CvPoint temp;
			temp = CornerPoint[1];
			CornerPoint[1] = CornerPoint[2];
			CornerPoint[2] = CornerPoint[3];
			CornerPoint[3] = CornerPoint[0];
			CornerPoint[0] = temp;

			CvPoint2D32f tempOut;
			tempOut = outPoint1[1];
			outPoint1[1] = outPoint1[2];
			outPoint1[2] = outPoint1[3];
			outPoint1[3] = outPoint1[0];
			outPoint1[0] = tempOut;
		}

		for (int i = 0; i < 4; i++)
			cvCircle(imageCorner, CornerPoint[i], 3, CV_RGB(0, 255, 0), CV_FILLED);

		//轉成順時針
		int Array1[] = { CornerPoint[1].x - CornerPoint[0].x ,CornerPoint[1].y - CornerPoint[0].y };
		int Array2[] = { CornerPoint[3].x - CornerPoint[0].x ,CornerPoint[3].y - CornerPoint[0].y };
		int cross = Array1[0] * Array2[1] - Array1[1] * Array2[0];

		if (cross < 0)
		{
			CvPoint temp;
			temp = CornerPoint[0];
			CornerPoint[0] = CornerPoint[1];
			CornerPoint[1] = temp;

			temp = CornerPoint[2];
			CornerPoint[2] = CornerPoint[3];
			CornerPoint[3] = temp;

			CvPoint2D32f tempOut;
			tempOut = outPoint1[1];
			outPoint1[1] = outPoint1[3];
			outPoint1[3] = tempOut;
		}

		//將外點 換成世界座標
		CvPoint3D32f outPoint_World[4];
		for (int i = 0; i < 4; i++)
			Img2SCARA(outPoint1[i].x, outPoint1[i].y, &outPoint_World[i].x, &outPoint_World[i].y, &outPoint_World[i].z);

		//內縮角點
		for (int i = 0; i < 4; i++)
		{
			CornerPoint[i].x = (mainDlg.Center.x + 4 * CornerPoint[i].x) / 5;
			CornerPoint[i].y = (mainDlg.Center.y + 4 * CornerPoint[i].y) / 5;
		}

		//5個點 轉成 世界座標
		CvPoint3D32f ObjectPoint_World[5];
		Img2SCARA(mainDlg.Center.x, mainDlg.Center.y, &ObjectPoint_World[0].x, &ObjectPoint_World[0].y, &ObjectPoint_World[0].z);
		Img2SCARA(CornerPoint[0].x, CornerPoint[0].y, &ObjectPoint_World[1].x, &ObjectPoint_World[1].y, &ObjectPoint_World[1].z);
		Img2SCARA(CornerPoint[1].x, CornerPoint[1].y, &ObjectPoint_World[2].x, &ObjectPoint_World[2].y, &ObjectPoint_World[2].z);
		Img2SCARA(CornerPoint[2].x, CornerPoint[2].y, &ObjectPoint_World[3].x, &ObjectPoint_World[3].y, &ObjectPoint_World[3].z);
		Img2SCARA(CornerPoint[3].x, CornerPoint[3].y, &ObjectPoint_World[4].x, &ObjectPoint_World[4].y, &ObjectPoint_World[4].z);
		//-----------------------------------------------------

			//畫出外點
		for (int i = 0; i < 4; i++)
			cvCircle(imageCorner, cvPoint(outPoint1[i].x, outPoint1[i].y), 2, CV_RGB(0, 0, 255), CV_FILLED);
		//畫出內縮角點
		for (int i = 0; i < 4; i++)
			cvCircle(imageCorner, CornerPoint[i], 1, CV_RGB(0, 255, 0), CV_FILLED);
		//畫出中點
		cvCircle(imageCorner, mainDlg.Center, 3, CV_RGB(255, 0, 0), CV_FILLED);

		//畫上座標
		Text(imageCorner, "0", outPoint1[0].x, outPoint1[0].y);
		Text(imageCorner, "1", outPoint1[1].x, outPoint1[1].y);
		Text(imageCorner, "2", outPoint1[2].x, outPoint1[2].y);
		Text(imageCorner, "3", outPoint1[3].x, outPoint1[3].y);

		Text(imageCorner, "0", CornerPoint[0].x, CornerPoint[0].y);
		Text(imageCorner, "1", CornerPoint[1].x, CornerPoint[1].y);
		Text(imageCorner, "2", CornerPoint[2].x, CornerPoint[2].y);
		Text(imageCorner, "3", CornerPoint[3].x, CornerPoint[3].y);

		//showimage	 
		CvRect rect = { mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[0].y,mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x,mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y };
		cvSetImageROI(imageCorner, rect);
		ShowImage(imageCorner, GetDlgItem(IDC_IMAGE_ApproxPoly), 3, cvSize(2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y)));
		m_img_approxPoly.SetWindowPos(NULL, 10, 10 + 400, 2 * (mainDlg.RoiPoint[1].x - mainDlg.RoiPoint[0].x), 2 * (mainDlg.RoiPoint[1].y - mainDlg.RoiPoint[0].y), SWP_SHOWWINDOW);

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

		//判斷擺放case
		int ObjCase = caseClassify(&ObjectPoint_World[0]);
		//判斷推的問題

		//判斷有幾個東西在四周
		int blockCount = 0;
		for (int i = 0; i < 4; i++)
		{
			if (outPoint_World[i].z > BlockStep)
			{
				blockCount++;
			}
		}


		CvPoint3D32f pushPoint[2];
		CvPoint3D32f grabPoint;
		float deg;

		switch (ObjCase)
		{
		case 1:  //平放
		{
			switch (blockCount)
			{
			case 0://平面旁邊無東西
			{
				//夾取位置 
				grabPoint = cvPoint3D32f(ObjectPoint_World[0].x, ObjectPoint_World[0].y, 18);
				deg = getDegree(ObjectPoint_World[0], outPoint_World[0]);
			}
			break;
			case 1://平面 旁邊一個block
			{
				if ((outPoint_World[1].z < BlockStep) && (outPoint_World[3].z < BlockStep))//block在長邊
				{
					//從短邊推
					pushPoint[1] = ObjectPoint_World[0];//=中心
					pushPoint[0] = outPoint_World[1];//短邊外面
					deg = getDegree(pushPoint[0], pushPoint[1]);
				}
				else // block 在短邊
				{
					//直接夾
					grabPoint = cvPoint3D32f(ObjectPoint_World[0].x, ObjectPoint_World[0].y, 18);
					deg = getDegree(ObjectPoint_World[0], outPoint_World[0]);
				}
			}
			break;
			}
		}
		break;
		case 2:  //平放堆疊
		{

		}
		break;
		case 3: //斜面
		{
			switch (blockCount)
			{
			case 0:
			{

			}
			break;
			case 1: //斜面 正常狀況
			{
				if ((outPoint_World[1].z > BlockStep) || (outPoint_World[3].z > BlockStep))//block在短邊
				{
					//夾取
					grabPoint = cvPoint3D32f(ObjectPoint_World[0].x, ObjectPoint_World[0].y, 36);
					deg = getDegree(ObjectPoint_World[0], outPoint_World[0]);
				}
				if ((outPoint_World[0].z > BlockStep) || (outPoint_World[2].z > BlockStep))//block在長邊
				{
					//找高邊
					if (outPoint_World[4].z > outPoint_World[1].z)//表示 block在outPoint 2
					{
						//從 outPoint 2 往 center 推
						pushPoint[0] = outPoint_World[2];
						pushPoint[1] = ObjectPoint_World[0];
						deg = getDegree(pushPoint[0], pushPoint[1]);
					}
					else if (outPoint_World[4].z < outPoint_World[1].z)//表示 block在outPoint 0
					{
						//從 outPoint 0 往 center 推
						pushPoint[0] = outPoint_World[0];
						pushPoint[1] = ObjectPoint_World[0];
						deg = getDegree(pushPoint[0], pushPoint[1]);
					}
				}
			}
			break;
			case 2: //斜面 旁邊有一個障礙物
			{
				//從高邊 往 center推
				pushPoint[0] = outPoint_World[findHighestSide(ObjectPoint_World)];
				pushPoint[1] = ObjectPoint_World[0];
				deg = getDegree(pushPoint[0], pushPoint[1]);
			}
			break;
			case 3:
			{
				//從高邊 往 center推
				pushPoint[0] = outPoint_World[findHighestSide(ObjectPoint_World)];
				pushPoint[1] = ObjectPoint_World[0];
				deg = getDegree(pushPoint[0], pushPoint[1]);
			}
			break;
			case 4:
			{
				//從高邊 往 center推
				pushPoint[0] = outPoint_World[findHighestSide(ObjectPoint_World)];
				pushPoint[1] = ObjectPoint_World[0];
				deg = getDegree(pushPoint[0], pushPoint[1]);
			}
			break;
			}//switch (blockCount)
		}
		break;//斜面
		}//switch ObjCase
	}//if (cornerNum == 4)
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
	*SCARAX = mainDlg.CamRefY - (mainDlg.kinect.CameraY * 1000) + 280 ;

	*SCARAY = mainDlg.CamRefX - (mainDlg.kinect.CameraX * 1000) - 410;

	*SCARAZ = (mainDlg.DepthPointsBase[x][y] - mainDlg.kinect.CameraZ * 1000);

	//*SCARAZ = 183-(mainDlg.DepthPointsBase[x][y] - mainDlg.kinect.CameraZ * 1000);
	//上面才是SCARA座標

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
	float y = first.y - second.y;
	float m = x / -y;
	float degree;

	degree = -atan(m);

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
