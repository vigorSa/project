
// VisonDlg.h : 头文件

#if !defined(AFX_MAINFRM_H__4B30C2A7_6EF2_4E39_8536_4A56F405A510__INCLUDED_)
#define AFX_MAINFRM_H__4B30C2A7_6EF2_4E39_8536_4A56F405A510__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvdailt.h"
#include "Raw2Rgb.h"
#include "hvextend.h"
#include "halconcpp.h"
#include "afxwin.h"
#include "hvutil.h"
#include "mscomm1.h"
#include "afxcmn.h"
#include "Calibration.h"

using namespace HalconCpp;


//定义自己的0
#define  MY_ZERO 0.000000001

//定义彩色图像还是黑白图像
typedef enum  tagHV_IMAGE_MODE
{
		HV_COLOR = 0,
		HV_BW = 1
}HV_IMAGE_MODE;

// CVisonDlg 对话框
class CVisonDlg : public CDialogEx
{
// 构造
public:
	CVisonDlg(CWnd* pParent = NULL);	// 标准构造函数
	CMenu m_Menu;                      //添加菜单变量

// 对话框数据
	enum { IDD = IDD_VISON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
public:
	virtual ~ CVisonDlg();

protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();

public:

	/// 打开设备
	HVSTATUS Open();
	/// 开始采集
	HVSTATUS Start();
	/// 关闭设备
	HVSTATUS Close();
	/// 停止采集
	HVSTATUS Stop();
	/// 刷新UI
	void UpdateUI();
    /// Snap 回调函数，用户也可以定义为全局函数， 
    /// 如果作为类的成员函数，必须为静态成员函数。
	static int CALLBACK SnapCallback(HV_SNAP_INFO *pInfo);
	///转换格式
	void Format24To8(BYTE *pDestBit8,BYTE *pSrceBit24, int nWid,int nHei);
	///调节窗口
	void AdjustHBlanking();
	/// 获取图像大小
	void GetImageSize();
	void InitParamFromINI();
	void SaveParamToINI();
	void DecodeImage();
	void SetBlanking(int nHBlanking,int nVBlanking);
	HVSTATUS SetExposureTime(int nWindWidth,long lTintUpper,long lTintLower);

private:

	HHV	m_hhv;			        ///< 数字摄像机句柄
	BOOL m_bOpen;               ///< 初始化标	        
	BOOL m_bSnapping;          ///< 启动标志
	
	BOOL m_bIsToGetBadPixel;
	BOOL m_bIsToRemoveBadPixel;	
	BOOL m_bIsToGetTemplate;
	BOOL m_bIsTemplateCorrect;

	BOOL m_bIsSnapSpeedSprted;    //本摄像机是否支持设置采集速度
	BOOL m_bIsBlkSizeSprted;      //本摄像机是否支持设置取得消隐取值范围
	BOOL m_bIsExposureModeSprted; //本摄像机是否支持设置曝光模式

	int *m_pPatternBuf;    //用来进行模板测试的缓冲区
	BYTE *m_pBadPixelBuf;	//保存坏点位置的内存
	int m_nTempTimes;       //取模板的次数

	BITMAPINFO *m_pBmpInfo;		///< BITMAPINFO 结构指针，显示图像时使用
	BYTE *m_pRawBuffer;			///< 采集图像原始数据缓冲区
	BYTE *m_pImageBuffer;		///< Bayer转换后缓冲区
    char m_chBmpBuf[2048];		///< BIMTAPINFO 存储缓冲区，m_pBmpInfo即指向此缓冲区

	BYTE m_pLutR[256] ;			///< 颜色查找表
	BYTE m_pLutG[256] ;
	BYTE m_pLutB[256] ;
	
	HV_EXPOSURE_MODE m_ExposureMode; //曝光模式
	HV_SNAP_SPEED m_SnapSpeed;
	HV_IMAGE_MODE m_ImageMode;
	HV_BAYER_LAYOUT m_Layout;
	HV_RESOLUTION m_Resolution;
	HV_SNAP_MODE m_SnapMode;
	HV_BAYER_CONVERT_TYPE m_ConversionType; //the conversion method of raw to rgb
	HV_WB_RATIO m_wbRatio;      ///< 白平衡比例系数
	HVTYPE m_hDeviceType;       ///< 设备类型
	int    m_nMaxWidth;         ///< 图像宽度
	int    m_nMaxHeight;        ///< 图像高度
	BOOL   m_bInitFailed;       ///< 初始化是否成功标识
	long   m_nADCLevel;         ///< 模拟/数字转换级别
	long   m_nShutterValue;	  	///< 曝光大小
	long   m_nShutterUnit;		///< 曝光单位
	long m_lTintUpper;          //曝光时间的分子和分母
	long m_lTintLower;
	int m_nHBlanking;
	int m_nVBlanking;
	PBYTE m_pLutMapping;    //映射内存指针
    long m_lGain[4];
	int GetOutputWinWdith();
	CRect m_rcOutputWindow;
	LONG m_dwCount;
	DWORD m_dwStart;

	int GetFileExt(LPCSTR lpFileName);	
	BOOL OpenBMPFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pImageBuffer);
	BOOL SaveBMPFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pImageBuffer);
	BOOL OpenRawFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pRawBuffer);
	BOOL SaveRawFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pRawBuffer);



public:
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnSnap();
	afx_msg void OnBnClickedBtnStop();
	CButton m_btnOpen;
	CButton m_btnStop;
	CButton m_btnClose;
	CButton m_btnSnap;
	CStatic m_staticFrame;
	CStatic m_staticPreview;
	afx_msg void OnResolutionMode();
	afx_msg void OnResolutionMode1();
	afx_msg void OnResolutionMode2();
	afx_msg void OnUpdateResolutionMode(CCmdUI *pCmdUI);
	afx_msg void OnUpdateResolutionMode1(CCmdUI *pCmdUI);
	afx_msg void OnUpdateResolutionMode2(CCmdUI *pCmdUI);
	afx_msg void OnImageModeColor();
	afx_msg void OnImageModeBw();
	afx_msg void OnUpdateImageModeColor(CCmdUI *pCmdUI);
	afx_msg void OnUpdateImageModeBw(CCmdUI *pCmdUI);
	afx_msg void OnSnapspeedHigh();
	afx_msg void OnUpdateSnapspeedHigh(CCmdUI *pCmdUI);
	afx_msg void OnSnapspeedNormal();
	afx_msg void OnUpdateSnapspeedNormal(CCmdUI *pCmdUI);
	afx_msg void OnBayerdatalayoutGb();
	afx_msg void OnUpdateBayerdatalayoutGb(CCmdUI *pCmdUI);
	afx_msg void OnBayerdatalayoutGr();
	afx_msg void OnUpdateBayerdatalayoutGr(CCmdUI *pCmdUI);
	afx_msg void OnBayerdatalayoutBg();
	afx_msg void OnUpdateBayerdatalayoutBg(CCmdUI *pCmdUI);
	afx_msg void OnBayerdatalayoutRg();
	afx_msg void OnUpdateBayerdatalayoutRg(CCmdUI *pCmdUI);
	afx_msg void OnSnapmodeContinuation();
	afx_msg void OnUpdateSnapmodeContinuation(CCmdUI *pCmdUI);
	afx_msg void OnSnapmodeTrigger();
	afx_msg void OnUpdateSnapmodeTrigger(CCmdUI *pCmdUI);
    afx_msg void CVisonDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnConversionBest();
	afx_msg void OnUpdateConversionBest(CCmdUI* pCmdUI);
	afx_msg void OnConversionFast();
	afx_msg void OnUpdateConversionFast(CCmdUI* pCmdUI);
	afx_msg void OnConversionNormal();
	afx_msg void OnUpdateConversionNormal(CCmdUI* pCmdUI);
public:
	void InitMSComm();
	int CVisonDlg::String2Hex(CString str, CByteArray &senddata);
	char CVisonDlg::ConvertHexChar(char ch);
	CString m_strSendMsg;
	CString m_strReceiveMsg;
	CMscomm m_ctrlComm;
	afx_msg void OnBnClickedBtnEnq();
	afx_msg void OnBnClickedBtnSend();
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm();

public:
	HBITMAP  m_hIcon_WAIT;
	HBITMAP  m_hIcon_OFF;
    HBITMAP  m_hIcon_ON;
	CStatic  m_bitmap;
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedBtnCalibration();
	afx_msg void OnBnClickedBtnLoad();
   void disp_continue_message (HTuple hv_WindowHandle, HTuple hv_Color, HTuple hv_Box);
   // Chapter: Graphics / Text
   // Short Description: This procedure writes a text message. 
  void disp_message (HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, 
    HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);
  // Local iconic variables
  HObject  ho_Image,ho_Rectangle, ho_ImageBorder;
  HObject  ho_ContoursTrans, ho_Map, ho_ImageMapped, ho_GrayImage;
  HObject  ho_Regions, ho_ConnectedRegions, ho_SelectedRegions;
  HObject  ho_RoiOuter, ho_RoiInner, ho_RegionDifference, ho_RegionEdges;
  HObject  ho_ImageReduced, ho_Edges, ho_SelectedXLD, ho_ContoursSplit;
  HObject  ho_Lines, ho_Circles, ho_ObjectSelected, ho_SelectedLines;
  HObject  ho_lines, ho_Contour, ho_circles, ho_ContCircle;

  // Local control variables
  HTuple  hv_CameraParameters, hv_CameraPose,hv_Pointer;
  HTuple  hv_Type, hv_Width, hv_Height, hv_WindowHandle,hv_MinY,m_width2,m_height2;
  HTuple  hv_MinX, hv_MaxY, hv_MaxX, hv_PoseForEntireImage;
  HTuple  hv_WorldPixelX, hv_WorldPixelY, hv_WorldLength1;
  HTuple  hv_WorldLength2, hv_ScaleForSimilarPixelSize, hv_ExtentX;
  HTuple  hv_ExtentY, hv_WidthRectifiedImage, hv_HeightRectifiedImage;
  HTuple  hv_Number, hv_NumLines, hv_i, hv_Attrib;
  HTuple  hv_RowBegin, hv_ColBegin, hv_RowEnd, hv_ColEnd;
  HTuple  hv_Nr, hv_Nc, hv_Dist, hv_Message;
  HTuple  hv_AngleJunction, hv_Row1Junction, hv_Column1Junction;
  HTuple  hv_PhiJunction, hv_Phi1Junction, hv_j, hv_Angle;
  HTuple  hv_Row1, hv_Column1, hv_IsOverlapping, hv_Phi, hv_Phi1;
  HTuple  hv_s;

  // Local iconic variables
  HObject  ho_ImageV, ho_RectangleV, ho_ImageBorderV;
  HObject  ho_ContoursTransV, ho_MapV, ho_ImageMappedV, ho_ImageGrayV;
  HObject  ho_ROIV, ho_ImageReducedV, ho_RegionsV, ho_ImageAffinTransV;
  HObject  ho_Regions1V, ho_ConnectedRegionsV, ho_CirclesV;
  HObject  ho_PillSelectedV, ho_ROIOuterV, ho_ROIInnerV, ho_ROI_difV;
  HObject  ho_ROIEdgesV, ho_ImageAffinTrans1V, ho_EdgesV, ho_ContoursSplitV;
  HObject  ho_ObjectSelectedV, ho_UnionContoursV, ho_ContCircle1V;
  HObject  ho_ContCircle2V, ho_ContCircle3V, ho_Rectangle1V;
  HObject  ho_EdgeFirstV, ho_EdgeSecondV;

  // Local control variables
  HTuple  hv_CameraParametersV, hv_CameraPoseV;
  HTuple  hv_ROI_0, hv_PointerV, hv_TypeV, hv_WidthV, hv_HeightV;
  HTuple  hv_MinYV, hv_MinXV, hv_MaxYV, hv_MaxXV, hv_PoseForEntireImageV;
  HTuple  hv_WorldPixelXV, hv_WorldPixelYV, hv_WorldLength1V;
  HTuple  hv_WorldLength2V, hv_ScaleForSimilarPixelSizeV;
  HTuple  hv_ExtentXV, hv_ExtentYV, hv_WidthRectifiedImageV;
  HTuple  hv_HeightRectifiedImageV, hv_PhiV, hv_AreaV, hv_Row1V;
  HTuple  hv_Column1V, hv_HomMat2D, hv_NumberV, hv_WindowHandleV;
  HTuple  hv_NumCircles, hv_v, hv_RowV, hv_ColumnV;
  HTuple  hv_RadiusV, hv_StartPhiV, hv_EndPhiV, hv_PointOrderV;
  HTuple  hv_X1V, hv_Y1V, hv_angle, hv_length1V, hv_length2V;
  HTuple  hv_MeasureHandleV, hv_RowEdgeFirst, hv_ColumnEdgeFirst;
  HTuple  hv_AmplitudeFirst, hv_RowEdgeSecond, hv_ColumnEdgeSecond;
  HTuple  hv_AmplitudeSecond, hv_IntraDistance, hv_InterDistance;
  HTuple  hv_X2, hv_Y2, hv_X3, hv_Y3, hv_Distance;

	afx_msg void OnBnClickedPicturecorrection();
	afx_msg void OnBnClickedPictureprocessing();
	afx_msg void OnClickedResult();

	afx_msg void OnBnClickedyes();
	afx_msg void OnBnClickedno();
};

#endif