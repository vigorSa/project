// Calibration.cpp : 实现文件
//

#include "stdafx.h"
#include "Vison.h"
#include "Calibration.h"
#include "afxdialogex.h"


// CCalibration 对话框

IMPLEMENT_DYNAMIC(CCalibration, CDialogEx)

CCalibration::CCalibration(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCalibration::IDD, pParent)
{
    
}

CCalibration::~CCalibration()
{
}

void CCalibration::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CalList, m_CalList);
	DDX_Control(pDX, IDC_DatList, m_DatList);
	DDX_Control(pDX, IDC_CalList2, m_CalList2);
	DDX_Control(pDX, IDC_DatList2, m_DatList2);
}


BEGIN_MESSAGE_MAP(CCalibration, CDialogEx)
	ON_BN_CLICKED(IDC_cal, &CCalibration::OnBnClickedcal)
	ON_BN_CLICKED(IDC_dat, &CCalibration::OnBnClickeddat)
	ON_BN_CLICKED(IDOK, &CCalibration::OnBnClickedOk)
	ON_BN_CLICKED(IDC_cal2, &CCalibration::OnBnClickedcal2)
	ON_BN_CLICKED(IDC_dat2, &CCalibration::OnBnClickeddat2)
END_MESSAGE_MAP()


// CCalibration 消息处理程序


BOOL CCalibration::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
    //获取编程语言列表视图控件的位置和大小
    m_CalList.GetClientRect(&rect);
    //为列表视图控件添加全行选中和栅格风格
    m_CalList.SetExtendedStyle(m_CalList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
    m_CalList.InsertColumn(0, _T("相机参数"), LVCFMT_CENTER, rect.Width()/2, 0);   
    m_CalList.InsertColumn(1, _T("结果"), LVCFMT_CENTER, rect.Width()/2, 1);    
	CRect rect1;
    //获取编程语言列表视图控件的位置和大小
    m_DatList.GetClientRect(&rect1);
    //为列表视图控件添加全行选中和栅格风格
    m_DatList.SetExtendedStyle(m_DatList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
    m_DatList.InsertColumn(0, _T("相机位资"), LVCFMT_CENTER, rect1.Width()/2, 0);   
	m_DatList.InsertColumn(1, _T("结果"), LVCFMT_CENTER, rect1.Width()/2, 1);  
	CRect rect2;
    //获取编程语言列表视图控件的位置和大小
    m_CalList2.GetClientRect(&rect2);
    //为列表视图控件添加全行选中和栅格风格
    m_CalList2.SetExtendedStyle(m_CalList2.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
    m_CalList2.InsertColumn(0, _T("相机参数"), LVCFMT_CENTER, rect2.Width()/2, 0);   
    m_CalList2.InsertColumn(1, _T("结果"), LVCFMT_CENTER, rect2.Width()/2, 0);    
	CRect rect3;
    //获取编程语言列表视图控件的位置和大小
    m_DatList2.GetClientRect(&rect3);
    //为列表视图控件添加全行选中和栅格风格
    m_DatList2.SetExtendedStyle(m_DatList2.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
    m_DatList2.InsertColumn(0, _T("相机位资"), LVCFMT_CENTER, rect3.Width()/2, 0);   
	m_DatList2.InsertColumn(1, _T("结果"), LVCFMT_CENTER, rect3.Width()/2, 1);  
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CCalibration::OnBnClickedcal()
{
	// TODO: 在此添加控件通知处理程序代码
   ReadCamPar("F:/实验/calibration2/参数.cal", &hv_CameraParameters);
   CString str;
   m_CalList.InsertItem(0,"焦距/m");
   m_CalList.InsertItem(1,"Kappa/1/m²");
   m_CalList.InsertItem(2,"单个像元的宽（Sx）/μm");
   m_CalList.InsertItem(3,"单个像元的高（Sy）/μm");
   m_CalList.InsertItem(4,"中心点x坐标（Cx）/像素");
   m_CalList.InsertItem(5,"中心点y坐标（Cy）/像素");
   for (int i =0; i<=5; i+=1)
   {
	   CameraParameters[i] = hv_CameraParameters[i];
	   str.Format("%f ",CameraParameters[i]);   
	   m_CalList.SetItemText(i,1,str);
   }
}


void CCalibration::OnBnClickeddat()
{
	// TODO: 在此添加控件通知处理程序代码
	ReadPose("F:/实验/calibration2/位资.dat", &hv_CameraPose);
	CString str;
    m_DatList.InsertItem(0,"X方向平移/m");
    m_DatList.InsertItem(1,"Y方向平移/m");
    m_DatList.InsertItem(2,"Z方向平移/m");
    m_DatList.InsertItem(3,"X方向旋转/°");
    m_DatList.InsertItem(4,"Y方向旋转/°");
    m_DatList.InsertItem(5,"Z方向旋转/°");
    for (int i =0; i<=5; i+=1)
    {
	    CameraPose[i] = hv_CameraPose[i];
	    str.Format("%f ",CameraPose[i]);   
	    m_DatList.SetItemText(i,1,str);
    }
}  

void CCalibration::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CCalibration *p=(CCalibration*)AfxGetApp();
	p->hv_CameraParameters=hv_CameraParameters;
	p->hv_CameraPose=hv_CameraPose;
	CCalibration *pV=(CCalibration*)AfxGetApp();
	pV->hv_CameraParametersV=hv_CameraParametersV;
	pV->hv_CameraPoseV=hv_CameraPoseV;
	UpdateData(TRUE);
	CDialogEx::OnOK();
}


void CCalibration::OnBnClickedcal2()
{
	// TODO: 在此添加控件通知处理程序代码
   ReadCamPar("F:/实验/calibration/相机参数.cal", &hv_CameraParametersV);
   CString str;
   m_CalList2.InsertItem(0,"焦距/m");
   m_CalList2.InsertItem(1,"Kappa/1/m²");
   m_CalList2.InsertItem(2,"单个像元的宽（Sx）/μm");
   m_CalList2.InsertItem(3,"单个像元的高（Sy）/μm");
   m_CalList2.InsertItem(4,"中心点x坐标（Cx）/像素");
   m_CalList2.InsertItem(5,"中心点y坐标（Cy）/像素");
   for (int i =0; i<=5; i+=1)
   {
	   CameraParametersV[i] = hv_CameraParametersV[i];
	   str.Format("%f ",CameraParametersV[i]);   
	   m_CalList2.SetItemText(i,1,str);
   }
}


void CCalibration::OnBnClickeddat2()
{
	// TODO: 在此添加控件通知处理程序代码
	ReadPose("F:/实验/calibration/相机位姿.dat", &hv_CameraPoseV);
	CString str;
    m_DatList2.InsertItem(0,"X方向平移/m");
    m_DatList2.InsertItem(1,"Y方向平移/m");
    m_DatList2.InsertItem(2,"Z方向平移/m");
    m_DatList2.InsertItem(3,"X方向旋转/°");
    m_DatList2.InsertItem(4,"Y方向旋转/°");
    m_DatList2.InsertItem(5,"Z方向旋转/°");
    for (int i =0; i<=5; i+=1)
    {
	    CameraPoseV[i] = hv_CameraPoseV[i];
	    str.Format("%f ",CameraPoseV[i]);   
	    m_DatList2.SetItemText(i,1,str);
    }
}


