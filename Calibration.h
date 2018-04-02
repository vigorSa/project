#pragma once
#include "afxcmn.h"
#include "halconcpp.h"
#include "HDevThread.h"

using namespace HalconCpp;


// CCalibration �Ի���

class CCalibration : public CDialogEx
{
	DECLARE_DYNAMIC(CCalibration)

public:
	CCalibration(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCalibration();

// �Ի�������
	enum { IDD = IDD_Calibration };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_CalList;
	CListCtrl m_DatList;
	HTuple hv_CameraParameters,hv_CameraPose;
	double CameraParameters[6],CameraPose[6];
	HTuple hv_CameraParametersV,hv_CameraPoseV;
	double CameraParametersV[6],CameraPoseV[6];
	afx_msg void OnBnClickedcal();
	afx_msg void OnBnClickeddat();
	afx_msg void OnBnClickedOk();
	CListCtrl m_CalList2;
	CListCtrl m_DatList2;
	afx_msg void OnBnClickedcal2();
	afx_msg void OnBnClickeddat2();
	
};
