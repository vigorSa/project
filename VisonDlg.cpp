
// VisonDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Vison.h"
#include "VisonDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///< Bayer ��ʽͼ������ת��ΪRGB24λ��ʽ���㷨����
const HV_BAYER_CONVERT_TYPE ConvertType = BAYER2RGB_NEIGHBOUR1; 
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVisonDlg �Ի���

CVisonDlg::CVisonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVisonDlg::IDD, pParent)
	, m_strSendMsg(_T(""))
	, m_strReceiveMsg(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hhv			= NULL;
	m_SnapMode		= CONTINUATION;	
	m_Resolution	= RES_MODE0;
	m_nADCLevel		= ADC_LEVEL2;

	for (int i = 0; i < 4; i++)
	{
		m_lGain[i]	= 10;
	}
	
	m_rcOutputWindow.SetRect(0, 0, 0, 0);
	
	m_ImageMode		= HV_COLOR;
	m_Layout	= BAYER_GB;
	
	m_nMaxWidth		= 0;
	m_nMaxHeight	= 0;

	m_pBmpInfo		= (BITMAPINFO *)m_chBmpBuf;

	m_dwCount		= 0;
	m_dwStart		= 0;

	m_bSnapping    =FALSE;
	
	//ӳ���ļ����
	m_pLutMapping = NULL;;

	m_ExposureMode = SIMULTANEITY;
	m_lTintUpper =60;
	m_lTintLower = 1000;
	m_ConversionType = BAYER2RGB_NEIGHBOUR1;

	m_bIsToGetBadPixel = FALSE;
	m_bIsToRemoveBadPixel = FALSE;
	m_bIsToGetTemplate = FALSE;
	m_bIsTemplateCorrect = FALSE;
	m_nTempTimes =0;

	m_SnapSpeed = HIGH_SPEED;
	m_nHBlanking = 0;
	m_nVBlanking = 0;

	m_bIsSnapSpeedSprted = FALSE;		//��������Ƿ�֧�����òɼ��ٶ�
	m_bIsBlkSizeSprted = FALSE;			//��������Ƿ�֧������ȡ������ȡֵ��Χ	
	m_bIsExposureModeSprted = FALSE;	//��������Ƿ�֧�������ع�ģʽ

	// ��ɫ���ұ��ʼ��
	for(int i=0; i<256; i++)
	{
		m_pLutR[i] = i;
		m_pLutG[i] = i;
		m_pLutB[i] = i;
	}

	// ��ƽ�����ϵ����ʼ��
	m_wbRatio.dRatioB = 1;
	m_wbRatio.dRatioR = 1;
	m_nShutterValue	  = 10000;	
	m_nShutterUnit	  = SHUTTER_US; 
	m_hDeviceType     = UNKNOWN_TYPE;

	m_hIcon_ON=LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_ON));
    m_hIcon_OFF=LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_OFF));
    m_hIcon_WAIT=LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_WAIT));

}

void CVisonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_SNAP, m_btnSnap);
	DDX_Control(pDX, IDC_STATIC_Frame, m_staticFrame);
	DDX_Control(pDX, IDC_STATIC_Preview, m_staticPreview);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
	DDX_Text(pDX, IDC_EDIT_SendMsg, m_strSendMsg);
	DDX_Text(pDX, IDC_EDIT_ReceiveMsg2, m_strReceiveMsg);
	DDX_Control(pDX, IDC_MSCOMM1, m_ctrlComm);
	DDX_Control(pDX, IDC_CONTROL, m_bitmap);
}

BEGIN_MESSAGE_MAP(CVisonDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_32793, &CVisonDlg::OnFileOpen)
	ON_COMMAND(ID_32794, &CVisonDlg::OnFileSave)
	ON_COMMAND(ID_RESOLUTION_MODE, &CVisonDlg::OnResolutionMode)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CVisonDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CVisonDlg::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_SNAP, &CVisonDlg::OnBnClickedBtnSnap)
	ON_BN_CLICKED(IDC_BTN_STOP, &CVisonDlg::OnBnClickedBtnStop)
	ON_COMMAND(ID_RESOLUTION_MODE32773, &CVisonDlg::OnResolutionMode1)
	ON_UPDATE_COMMAND_UI(ID_RESOLUTION_MODE, &CVisonDlg::OnUpdateResolutionMode)
	ON_UPDATE_COMMAND_UI(ID_RESOLUTION_MODE32773, &CVisonDlg::OnUpdateResolutionMode1)
	ON_UPDATE_COMMAND_UI(ID_RESOLUTION_MODE32774, &CVisonDlg::OnUpdateResolutionMode2)
	ON_COMMAND(ID_RESOLUTION_MODE32774, &CVisonDlg::OnResolutionMode2)
	ON_COMMAND(ID_IMAGEMDOE_COLOR, &CVisonDlg::OnImageModeColor)
	ON_COMMAND(ID_IMAGEMDOE_BW, &CVisonDlg::OnImageModeBw)
	ON_UPDATE_COMMAND_UI(ID_IMAGEMDOE_COLOR, &CVisonDlg::OnUpdateImageModeColor)
	ON_UPDATE_COMMAND_UI(ID_IMAGEMDOE_BW, &CVisonDlg::OnUpdateImageModeBw)
	ON_COMMAND(ID_CONVERSIONTYOE_BEST,&CVisonDlg:: OnConversionBest)
	ON_UPDATE_COMMAND_UI(ID_CONVERSIONTYOE_BEST, &CVisonDlg::OnUpdateConversionBest)
	ON_COMMAND(ID_CONVERSIONTYOE_FAST,&CVisonDlg:: OnConversionFast)
	ON_UPDATE_COMMAND_UI(ID_CONVERSIONTYOE_FAST,&CVisonDlg:: OnUpdateConversionFast)
	ON_COMMAND(ID_CONVERSIONTYOE_NORMAL, &CVisonDlg::OnConversionNormal)
	ON_UPDATE_COMMAND_UI(ID_CONVERSIONTYOE_NORMAL,&CVisonDlg:: OnUpdateConversionNormal)
	ON_COMMAND(ID_SNAPSPEED_HIGHSPEED, &CVisonDlg::OnSnapspeedHigh)
	ON_UPDATE_COMMAND_UI(ID_SNAPSPEED_HIGHSPEED, &CVisonDlg::OnUpdateSnapspeedHigh)
	ON_COMMAND(ID_SNAPSPEED_NORMALSPEED, &CVisonDlg::OnSnapspeedNormal)
	ON_UPDATE_COMMAND_UI(ID_SNAPSPEED_NORMALSPEED, &CVisonDlg::OnUpdateSnapspeedNormal)
	ON_COMMAND(ID_BAYERDATALAYOUT_GBRG, &CVisonDlg::OnBayerdatalayoutGb)
	ON_UPDATE_COMMAND_UI(ID_BAYERDATALAYOUT_GBRG, &CVisonDlg::OnUpdateBayerdatalayoutGb)
	ON_COMMAND(ID_BAYERDATALAYOUT_GRBG, &CVisonDlg::OnBayerdatalayoutGr)
	ON_UPDATE_COMMAND_UI(ID_BAYERDATALAYOUT_GRBG, &CVisonDlg::OnUpdateBayerdatalayoutGr)
	ON_COMMAND(ID_BAYERDATALAYOUT_BGGR, &CVisonDlg::OnBayerdatalayoutBg)
	ON_UPDATE_COMMAND_UI(ID_BAYERDATALAYOUT_BGGR, &CVisonDlg::OnUpdateBayerdatalayoutBg)
	ON_COMMAND(ID_BAYERDATALAYOUT_RGGB, &CVisonDlg::OnBayerdatalayoutRg)
	ON_UPDATE_COMMAND_UI(ID_BAYERDATALAYOUT_RGGB, &CVisonDlg::OnUpdateBayerdatalayoutRg)
	ON_COMMAND(ID_SNAPMODE_CONTINUATION, &CVisonDlg::OnSnapmodeContinuation)
	ON_UPDATE_COMMAND_UI(ID_SNAPMODE_CONTINUATION, &CVisonDlg::OnUpdateSnapmodeContinuation)
	ON_COMMAND(ID_SNAPMODE_TRIGGER, &CVisonDlg::OnSnapmodeTrigger)
	ON_UPDATE_COMMAND_UI(ID_SNAPMODE_TRIGGER, &CVisonDlg::OnUpdateSnapmodeTrigger)
	ON_BN_CLICKED(IDC_BTN_ENQ, &CVisonDlg::OnBnClickedBtnEnq)
	ON_BN_CLICKED(IDC_BTN_SEND, &CVisonDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_TEST, &CVisonDlg::OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_BTN_Calibration, &CVisonDlg::OnBnClickedBtnCalibration)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CVisonDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_PictureCorrection, &CVisonDlg::OnBnClickedPicturecorrection)
	ON_BN_CLICKED(IDC_PictureProcessing, &CVisonDlg::OnBnClickedPictureprocessing)
	ON_BN_CLICKED(IDC_Result, &CVisonDlg::OnClickedResult)
	ON_BN_CLICKED(IDC_yes, &CVisonDlg::OnBnClickedyes)
	ON_BN_CLICKED(IDC_no, &CVisonDlg::OnBnClickedno)
END_MESSAGE_MAP()


// CVisonDlg ��Ϣ�������

BOOL CVisonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�
	m_Menu.LoadMenu(IDR_MENU1); //��Ӳ˵�
	SetMenu(&m_Menu);

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// ���豸
	HVSTATUS status = BeginHVDevice(1, &m_hhv);
	if (!HV_SUCCESS(status))
	{
		UpdateUI();
		HV_MESSAGE(status);
		return FALSE;
	}

	//Get current device type
	int nLen    = sizeof(HVTYPE);
	status      = HVGetDeviceInfo(m_hhv, DESC_DEVICE_TYPE, &m_hDeviceType, &nLen);
	if (!HV_SUCCESS(status))
	{
		UpdateUI();
		return FALSE;
	}
	if (HV_SUCCESS(status))
	{
		m_bIsSnapSpeedSprted = TRUE;		//��������Ƿ�֧�����òɼ��ٶ�
		m_bIsBlkSizeSprted = TRUE;			//��������Ƿ�֧������ȡ������ȡֵ��Χ	
	}
	
	// ��ʼ��HVExtend �ӿ���Ҫ����Դ
	status = HVInitialize(m_hhv);
	if (!HV_SUCCESS(status))
	{
		m_bInitFailed = TRUE;
		HV_MESSAGE(status);
		return FALSE;
	}
	if (m_hDeviceType == GV400UMTYPE || m_hDeviceType == GV400UCTYPE)
	{
		m_bIsExposureModeSprted = TRUE;//��������Ƿ�֧�������ع�ģʽ

		CMenu*  pmenu = GetMenu()->GetSubMenu(1); 
		pmenu->EnableMenuItem(ID_RESOLUTION_MODE32774, MF_DISABLED|MF_GRAYED); //GV400Ux��֧��mode2
	}

	/*
	 *	��ʼ�����������Ӳ��״̬���û�Ҳ����������λ�ó�ʼ�������������
	 *	��Ӧ��֤����������Ѿ��򿪣������û���Ӧ�ó����ʼ��ʱ��
	 *	ͬʱ��ʼ�����������Ӳ����
	 */

	// ��������������ֱ���
	status = HVSetResolution(m_hhv,  m_Resolution);
	HV_VERIFY(status);

	// ��ȡͼ���С
	GetImageSize();

	// ��Ƶ������ڣ�����Ƶ�����Χ���������ȡֵ��Χ������sensor���ͼ��Χ���ڣ�
	// ��Ƶ�������Ͻ�X����ʹ��ڿ��ӦΪ4�ı��������Ͻ�Y����ʹ��ڸ߶�ӦΪ2�ı���
	// ������ڵ���ʼλ��һ������Ϊ(0, 0)���ɡ�
	status = HVSetOutputWindow(m_hhv, 0, 0, m_nMaxWidth, m_nMaxHeight);
	HV_VERIFY(status);
	
	// ���òɼ��ٶ�
	if(m_bIsSnapSpeedSprted)
	status = HVSetSnapSpeed(m_hhv, HIGH_SPEED);
	HV_VERIFY(status);
	
	// �ɼ�ģʽ������ CONTINUATION(����)��TRIGGER(�ⴥ��)
	status = HVSetSnapMode(m_hhv, m_SnapMode);
	HV_VERIFY(status);
	
	HVSetBlanking(m_hhv,m_nHBlanking,m_nVBlanking);
	AdjustHBlanking();
	
	// ���ø�������������
	for (int i = 0; i < 4; i++)
	{
		status = HVAGCControl(m_hhv, RED_CHANNEL + i, m_lGain[i]);
		HV_VERIFY(status);
	}
	//�����ع�ģʽ
	if(m_bIsExposureModeSprted)
	{
		int nlength = sizeof(m_ExposureMode);
		HVCommand(m_hhv,CMD_SET_EXPOSURE_MODE, &m_ExposureMode, &nlength);
	}
	// �����ع�ʱ��
	status = HVAECControl(m_hhv, AEC_SHUTTER_UNIT, m_nShutterUnit);
	HV_VERIFY(status);
	status = HVAECControl(m_hhv, AEC_SHUTTER_SPEED, m_nShutterValue);
	HV_VERIFY(status);
	
	// ����ADC�ļ���
	status = HVADCControl(m_hhv, ADC_BITS, m_nADCLevel);
	HV_VERIFY(status);
	
	// m_pBmpInfo��ָ��m_chBmpBuf���������û������Լ�����BTIMAPINFO������	
	m_pBmpInfo								= (BITMAPINFO *)m_chBmpBuf;
	// ��ʼ��BITMAPINFO �ṹ���˽ṹ�ڱ���bmp�ļ�����ʾ�ɼ�ͼ��ʱʹ��
	m_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	// ͼ���ȣ�һ��Ϊ������ڿ��
	m_pBmpInfo->bmiHeader.biWidth			= m_nMaxWidth;
	// ͼ���ȣ�һ��Ϊ������ڸ߶�
	m_pBmpInfo->bmiHeader.biHeight			= m_nMaxHeight;
	
	// ��������һ����ͬ��
	// ���ڵ���8λ��λͼ����Ӧ������Ӧ��λͼ��ɫ��
	m_pBmpInfo->bmiHeader.biPlanes			= 1;
	m_pBmpInfo->bmiHeader.biBitCount		= 24;
	m_pBmpInfo->bmiHeader.biCompression		= BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage		= 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biClrUsed			= 0;
	m_pBmpInfo->bmiHeader.biClrImportant	= 0;
	
	// ����ԭʼͼ�񻺳�����һ�������洢�ɼ�ͼ��ԭʼ����
	// һ��ͼ�񻺳�����С��������ڴ�С����Ƶ��ʽȷ����
	m_pRawBuffer = new BYTE[m_nMaxWidth * m_nMaxHeight * 2];
	ASSERT(m_pRawBuffer);

	// ����Bayerת����ͼ�����ݻ���
	m_pImageBuffer = new BYTE[m_nMaxWidth * m_nMaxHeight * 3];
	ASSERT(m_pImageBuffer);

	InitParamFromINI();
	InitMSComm();
	m_bitmap.SetBitmap(m_hIcon_WAIT);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
//�����������ͳ�����,
//��Ϊ���ʱ���Ӱ���ع�ʱ��,����ͬʱҪ�����ع�ʱ��
void CVisonDlg::SetBlanking(int nHBlanking,int nVBlanking)
{

	DWORD pBlankSize[4];
	int nBufSize = 0;
	int nHmin = -9;
	int nVmin = -9;
	int nHmax = 1930;
	int nVmax = 1930;		

	if(m_bIsBlkSizeSprted)
	{
		HVGetDeviceInfo(m_hhv,DESC_DEVICE_BLANKSIZE,NULL,&nBufSize);	
		HVGetDeviceInfo(m_hhv,DESC_DEVICE_BLANKSIZE,pBlankSize,&nBufSize);	
		
		//�õ������ı߽�ֵ
		nHmin = (int)pBlankSize[0];
		nVmin = (int)pBlankSize[1];
		nHmax = (int)pBlankSize[2];
		nVmax = (int)pBlankSize[3];	
	}

	nHBlanking = max(min(nHmax,nHBlanking),nHmin);
	nVBlanking = max(min(nVmax,nVBlanking),nVmin);

	m_nHBlanking = nHBlanking;
	m_nVBlanking = nVBlanking;

	HVSetBlanking(m_hhv,nHBlanking,nVBlanking);
	SetExposureTime(GetOutputWinWdith(),m_lTintUpper,m_lTintLower);	
}

void CVisonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CVisonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CVisonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CVisonDlg::~ CVisonDlg()
{
	if (m_pImageBuffer) {
		delete []m_pImageBuffer;
	}
	if (m_pRawBuffer) {
		delete []m_pRawBuffer;
	}
	if(m_pLutMapping)
	{
		// �ӽ��̵ĵ�ַ�ռ䳷���ļ�����ӳ��
		UnmapViewOfFile(m_pLutMapping);		
	}
	if(m_pPatternBuf)
	{
		delete[] m_pPatternBuf;
	}
	if(m_pBadPixelBuf)
	{
		delete[] m_pBadPixelBuf;
	}
}

void CVisonDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{

   // TODO: Add your message handler code here
    ASSERT(pPopupMenu != NULL);
    // Check the enabled state of various menu items.
    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);
    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
        CWnd* pParent = this;
        // Child windows don't have menus--need to go to the top!
        if (pParent != NULL &&
            (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
        {
            int nIndexMax = ::GetMenuItemCount(hParentMenu);
            for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
            {
                if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
                {
                    // When popup is found, m_pParentMenu is containing menu.
                    state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                    break;
                }
            }
        }
    }
    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
        state.m_nIndex++)
    {
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
        if (state.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.
        ASSERT(state.m_pOther == NULL);
        ASSERT(state.m_pMenu != NULL);
        if (state.m_nID == (UINT)-1)
        {
            // Possibly a popup menu, route to first item of that popup.
            state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
            if (state.m_pSubMenu == NULL ||
                (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
                state.m_nID == (UINT)-1)
            {
                continue;       // First item of popup can't be routed to.
            }
            state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
        }
        else
        {
            // Normal menu item.
            // Auto enable/disable if frame window has m_bAutoMenuEnable
            // set and command is _not_ a system command.
            state.m_pSubMenu = NULL;
            state.DoUpdate(this, FALSE);
        }
        // Adjust for menu deletions and additions.
        UINT nCount = pPopupMenu->GetMenuItemCount();
        if (nCount < state.m_nIndexMax)
        {
            state.m_nIndex -= (state.m_nIndexMax - nCount);
            while (state.m_nIndex < nCount &&
                pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
            {
                state.m_nIndex++;
            }
        }
        state.m_nIndexMax = nCount;
    }
}

void CVisonDlg::InitParamFromINI()
{
	int n = 0;

	m_Resolution	= (HV_RESOLUTION)GetPrivateProfileInt("Video Parameter", "Resolution", RES_MODE0, "HVViewer.ini" );

	m_SnapMode	= (HV_SNAP_MODE)GetPrivateProfileInt("Video Parameter", "SnapMode", CONTINUATION, "HVViewer.ini" );
	
	m_ImageMode		= (HV_IMAGE_MODE)GetPrivateProfileInt("Video Parameter", "ImageMode", HV_COLOR, "HVViewer.ini" );
	
	m_Layout	= (HV_BAYER_LAYOUT)GetPrivateProfileInt("Video Parameter", "BayerLayout", BAYER_GR, "HVViewer.ini" );

	m_lTintUpper	= (long)GetPrivateProfileInt("Video Parameter", "TintUpper", 60, "HVViewer.ini" );
	m_lTintLower	= (long)GetPrivateProfileInt("Video Parameter", "TintLower", 1000, "HVViewer.ini" );

	m_nADCLevel	= (long)GetPrivateProfileInt("Video Parameter", "ADCLevel", ADC_LEVEL1, "HVViewer.ini" );
	
	m_lGain[0]	= (long)GetPrivateProfileInt("Video Parameter", "GainRCh", 10, "HVViewer.ini" );
	m_lGain[1]	= (long)GetPrivateProfileInt("Video Parameter", "GainRChG", 10, "HVViewer.ini" );
	m_lGain[2]	= (long)GetPrivateProfileInt("Video Parameter", "GainBChG", 10, "HVViewer.ini" );
	m_lGain[3]	= (long)GetPrivateProfileInt("Video Parameter", "GainBCh", 10, "HVViewer.ini" );
	
	m_nHBlanking	= (long)GetPrivateProfileInt("Video Parameter", "HBlanking", 0, "HVViewer.ini" );
	m_nVBlanking	= (long)GetPrivateProfileInt("Video Parameter", "VBlanking", 0, "HVViewer.ini" );
	m_SnapSpeed =  (HV_SNAP_SPEED )GetPrivateProfileInt("Video Parameter", "SnapSpeed", HIGH_SPEED, "HVViewer.ini" );
	m_ExposureMode  = (HV_EXPOSURE_MODE)GetPrivateProfileInt("Video Parameter", "ExposureMode", SIMULTANEITY, "HVViewer.ini" );
}

void CVisonDlg::SaveParamToINI()
{
	CString strValue;

	strValue.Format("%d", m_Resolution);
	WritePrivateProfileString("Video Parameter", "Resolution", strValue, "HVViewer.ini");

	strValue.Format("%d", m_SnapMode);
	WritePrivateProfileString("Video Parameter", "SnapMode", strValue, "HVViewer.ini");

	strValue.Format("%d", m_ImageMode);
	WritePrivateProfileString("Video Parameter", "ImageMode", strValue, "HVViewer.ini");
	strValue.Format("%d", m_Layout);
	WritePrivateProfileString("Video Parameter", "BayerLayout", strValue, "HVViewer.ini");
	
	strValue.Format("%d", m_lTintLower);
	WritePrivateProfileString("Video Parameter", "TintLower", strValue, "HVViewer.ini");

	strValue.Format("%d", m_lTintUpper);
	WritePrivateProfileString("Video Parameter", "TintUpper", strValue, "HVViewer.ini");

	strValue.Format("%d", m_nADCLevel);
	WritePrivateProfileString("Video Parameter", "ADCLevel", strValue, "HVViewer.ini");

	strValue.Format("%d", m_lGain[0]);
	WritePrivateProfileString("Video Parameter", "GainRCh", strValue, "HVViewer.ini");
	strValue.Format("%d", m_lGain[1]);
	WritePrivateProfileString("Video Parameter", "GainRChG", strValue, "HVViewer.ini");
	strValue.Format("%d", m_lGain[2]);
	WritePrivateProfileString("Video Parameter", "GainBChG", strValue, "HVViewer.ini");
	strValue.Format("%d", m_lGain[3]);
	WritePrivateProfileString("Video Parameter", "GainBCh", strValue, "HVViewer.ini");

	strValue.Format("%d", m_nHBlanking);
	WritePrivateProfileString("Video Parameter", "HBlanking", strValue, "HVViewer.ini");
	strValue.Format("%d", m_nVBlanking);
	WritePrivateProfileString("Video Parameter", "VBlanking", strValue, "HVViewer.ini");

	strValue.Format("%d", m_SnapSpeed);
	WritePrivateProfileString("Video Parameter", "SnapSpeed", strValue, "HVViewer.ini");	

	strValue.Format("%d", m_ExposureMode);
	WritePrivateProfileString("Video Parameter", "ExposureMode", strValue, "HVViewer.ini" );
}




void CVisonDlg::AdjustHBlanking()
{
	int size = sizeof(HVTYPE);
	HVTYPE type;
	HVGetDeviceInfo(m_hhv,DESC_DEVICE_TYPE, &type, &size);	
	if(type == GV400UMTYPE || type == GV400UCTYPE)
	{
		int rate = 0;
		switch(m_Resolution)
		{
		case RES_MODE0:
			rate = 1;
			break;
		case RES_MODE1:
			rate = 2;
			break;
		default:
			break;
		}
		if (m_nHBlanking + m_rcOutputWindow.Width()*rate <= 0x236)
		{
			//ȡ�������ı߽�ֵ
			DWORD pBlankSize[4];
			int nBufSize = 0;
			int nHmin = 0;
			int nHmax = 0;
			HVGetDeviceInfo(m_hhv,DESC_DEVICE_BLANKSIZE,NULL,&nBufSize);	
			HVGetDeviceInfo(m_hhv,DESC_DEVICE_BLANKSIZE,pBlankSize,&nBufSize);	
			nHmin = (int)pBlankSize[0];
			nHmax = (int)pBlankSize[2];
			int tHor = 0x236-m_rcOutputWindow.Width()*rate+1;
			m_nHBlanking = max(nHmin,min(tHor, nHmax));
			HVSetBlanking(m_hhv,m_nHBlanking,m_nVBlanking);
		}			
	}
}


void CVisonDlg::OnFileOpen()
{
	// TODO: �ڴ���������������
    CFileDialog dlg(TRUE , "*.bmp", NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, "Bitmap Files(*.bmp)|*.bmp|JPEG Files(*.jpg)|*.jpg|DH Raw Files(*.raw)|*.raw||", this);	
	
	if (dlg.DoModal() == IDOK) {
		if (m_pImageBuffer && m_pRawBuffer) {
			int nFileExt = GetFileExt(dlg.GetPathName());
			switch(nFileExt) {
			case 0:
				OpenBMPFile(dlg.GetPathName(), m_pBmpInfo, m_pImageBuffer);
				break;
			case 1:
				HVLoadJPEG((char *)(LPCSTR)dlg.GetPathName(), m_pImageBuffer, (int *)&(m_pBmpInfo->bmiHeader.biWidth), 
					(int *)&(m_pBmpInfo->bmiHeader.biHeight), (int *)&(m_pBmpInfo->bmiHeader.biBitCount), TRUE);
				break;
			case 2:
				OpenRawFile(dlg.GetPathName(), m_pBmpInfo, m_pRawBuffer);
				DecodeImage();
				break;
			default:
				break;
			}		
			Invalidate(TRUE);
		} 
	}
	
	
}


void CVisonDlg::OnFileSave()
{
	// TODO: �ڴ���������������
	CFileDialog dlg(FALSE , "*.bmp", NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, "Bitmap Files(*.bmp)|*.bmp|JPEG Files(*.jpg)|*.jpg|DH Raw Files(*.raw)|*.raw||", this);	

	if (dlg.DoModal() == IDOK) {
		if (m_pImageBuffer && m_pRawBuffer) {
			int nFileExt = GetFileExt(dlg.GetPathName());
			switch(nFileExt) {
			case 0:
				SaveBMPFile(dlg.GetPathName(), m_pBmpInfo, m_pImageBuffer);
				break;
			case 1:
				HVSaveJPEG((char *)(LPCSTR)dlg.GetPathName(), m_pImageBuffer, (int)(m_pBmpInfo->bmiHeader.biWidth), 
					(int)(m_pBmpInfo->bmiHeader.biHeight), (int)(m_pBmpInfo->bmiHeader.biBitCount), TRUE, 100);
				break;
			case 2:
				SaveRawFile(dlg.GetPathName(), m_pBmpInfo, m_pRawBuffer);
				break;
			default:
				break;
			}	
		} 
	}
}


void CVisonDlg:: Format24To8(BYTE *pDestBit8,BYTE *pSrceBit24, int nWid,int nHei)
{
	if(pDestBit8 == NULL)
		return;
	if(pSrceBit24 == NULL)
		return;
	if(nWid <=0 || nHei <=0)
		return;
	if(m_pLutMapping == NULL)
		return;


	int i=0,j=0;
	int nPos=0;
	PBYTE pYUVLut;
	for(i=0;i<nHei;i++)
	{
		nPos = i*nWid*3;	
		for(j=0;j<nWid;j++)
		{
			//��������Ϊ GBR
			pYUVLut = m_pLutMapping + 256*256*pSrceBit24[nPos+2] + 256*pSrceBit24[nPos+1] + pSrceBit24[nPos];
			*pDestBit8 = *pYUVLut;
			pDestBit8 ++;
			nPos +=3;
		}
	}


}

void CVisonDlg::DecodeImage()
{
	if (m_ImageMode	== HV_COLOR) {
		m_pBmpInfo->bmiHeader.biBitCount = 24;
		CSize Size;
		Size.cx  = m_pBmpInfo->bmiHeader.biWidth;
		Size.cy = m_pBmpInfo->bmiHeader.biHeight;
		
		
		//ÿ��֡����һ��
		if(m_ConversionType != BAYER2RGB_NEIGHBOUR)
		{
			static int nTemp = 1;
			if(nTemp%2 ==0 )
			{
				nTemp =1;
				return;
			}
			else
				nTemp = 2;
		}
		
						
		if(m_bIsToGetBadPixel)
		{
			DetectDeadPixel(m_pRawBuffer,m_pBadPixelBuf,m_nMaxWidth,m_nMaxHeight);
			m_bIsToGetBadPixel = FALSE;

			/*�õ�����λ�ú�,�ָ�ԭ���ɼ�����*/

			//�������������ǰ��ֹͣ�ɼ�
			if (m_bSnapping)
			{
				HVStopSnap(m_hhv);
			}	
			AdjustHBlanking();
			HVSTATUS status = HVSetOutputWindow(m_hhv, 
				m_rcOutputWindow.left,
				m_rcOutputWindow.top,
				m_rcOutputWindow.Width(),
				m_rcOutputWindow.Height()			
				);   
			
			//������������ں󣬿�ʼ�ɼ�
			if (m_bSnapping)
			{
				//ֻ����һ��������,
				BYTE * ppBuffer[1];
				ppBuffer[0] = m_pRawBuffer;
				status = HVStartSnap(m_hhv,ppBuffer,1);	
			}
		}

		if(m_bIsToRemoveBadPixel) //ȥ������
		{
			EraseDeadPixel(m_pRawBuffer,m_pBadPixelBuf,m_rcOutputWindow.left,m_rcOutputWindow.top,Size.cx,Size.cy,m_nMaxWidth,m_nMaxHeight);
		}

		if(m_bIsToGetTemplate)
		{
			if(m_nTempTimes ==0)  //��һ������ʱ���������
				memset(m_pPatternBuf,'\0',sizeof(int)*m_nMaxWidth * m_nMaxHeight);
			GetPatternNoise(m_pRawBuffer,m_pPatternBuf,m_nMaxWidth,m_nMaxHeight);
			m_nTempTimes ++ ;
			if(m_nTempTimes >= 10)
			{
				AvgPatternNoise(m_pPatternBuf,m_nMaxWidth,m_nMaxHeight,m_nTempTimes);
				m_bIsToGetTemplate = FALSE;
				m_nTempTimes =0;

				
				/*�õ�ģ���,�ָ�ԭ���ɼ�����*/				
				//�������������ǰ��ֹͣ�ɼ�
				if (m_bSnapping)
				{
					HVStopSnap(m_hhv);
				}		
				AdjustHBlanking();
				HVSTATUS status = HVSetOutputWindow(m_hhv, 
					m_rcOutputWindow.left,
					m_rcOutputWindow.top,
					m_rcOutputWindow.Width(),
					m_rcOutputWindow.Height()			
					);   
				
				//������������ں󣬿�ʼ�ɼ�
				if (m_bSnapping)
				{
					//ֻ����һ��������,
					BYTE * ppBuffer[1];
					ppBuffer[0] = m_pRawBuffer;
					status = HVStartSnap(m_hhv,ppBuffer,1);	
				}
				

			}
		}
		if(m_bIsTemplateCorrect)
		{
			FixPatternNoise(m_pRawBuffer,m_pPatternBuf,m_rcOutputWindow.left,m_rcOutputWindow.top,Size.cx,Size.cy,m_nMaxWidth,m_nMaxHeight);
		}

		ConvertBayer2Rgb(m_pImageBuffer, m_pRawBuffer,Size.cx,Size.cy, m_ConversionType,m_pLutR,m_pLutG,m_pLutB,true,m_Layout);

		
	} 

	else
	{
		m_pBmpInfo->bmiHeader.biBitCount = 8;
		//CopyMemory(m_pImageBuffer, m_pRawBuffer, m_pBmpInfo->bmiHeader.biWidth * m_pBmpInfo->bmiHeader.biHeight);
		
		CSize Size;
		Size.cx  = m_pBmpInfo->bmiHeader.biWidth;
		Size.cy = m_pBmpInfo->bmiHeader.biHeight;	
		ConvertBayer2Rgb(m_pImageBuffer, m_pRawBuffer,Size.cx,Size.cy, m_ConversionType,m_pLutR,m_pLutG,m_pLutB,true,m_Layout);
	
		long lImageSize = m_pBmpInfo->bmiHeader.biWidth * m_pBmpInfo->bmiHeader.biHeight;
		BYTE *pTmpBit8 = new BYTE[lImageSize];
		if(pTmpBit8 == NULL)
			return ;

		//ת����8λ�ڰ�ͼ��
		Format24To8(pTmpBit8,m_pImageBuffer, m_pBmpInfo->bmiHeader.biWidth,m_pBmpInfo->bmiHeader.biHeight);	
		memcpy(m_pImageBuffer,pTmpBit8,lImageSize);

		if(pTmpBit8)
		{
			delete []pTmpBit8;
			pTmpBit8 = NULL;
		}
		
	}
}

BOOL CVisonDlg::OpenBMPFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pImageBuffer)
{
	BOOL bRVal = TRUE;
	DWORD dwBytesRead = 0;
	DWORD dwSize = 0;
	BITMAPFILEHEADER bfh;//�ļ�ͷ

	HANDLE hFile = ::CreateFile(lpFileName,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL
								);
	if (hFile == INVALID_HANDLE_VALUE) {
		bRVal = FALSE;
	}
	else{
		dwSize = sizeof(BITMAPFILEHEADER);
		::ReadFile(hFile, &bfh, dwSize, &dwBytesRead, NULL );
		if (bfh.bfType != ((WORD)'M' << 8 | 'B')) {
			bRVal = FALSE;
		}
		else{
			dwSize = sizeof(BITMAPINFOHEADER);
			::ReadFile(hFile, pBmpInfo, dwSize, &dwBytesRead, NULL);
			if (pBmpInfo->bmiHeader.biBitCount <= 8) {
				dwSize = bfh.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
				ReadFile(hFile,
					(char *)pBmpInfo + sizeof(BITMAPINFOHEADER),
					dwSize,
					&dwBytesRead,
					NULL
					);
			}
			
			dwSize = WIDTH_BYTES(pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biBitCount) *
					pBmpInfo->bmiHeader.biHeight;

			SetFilePointer(hFile, bfh.bfOffBits, 0, FILE_BEGIN);	//���õ�ͼ�����ݵ�λ��(2009.08.27)

			ReadFile(hFile, pImageBuffer, dwSize, &dwBytesRead, NULL);
		}
		CloseHandle(hFile);
		
	}
	
	return bRVal;
}




BOOL CVisonDlg::SaveBMPFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pImageBuffer)
{
	BOOL bRVal				= TRUE;
	DWORD dwBytesRead		= 0;
	DWORD dwSize			= 0;
	BITMAPFILEHEADER bfh	= {0};
	int nTable				= 0;
	DWORD dwImageSize		= 0;

	if (pBmpInfo->bmiHeader.biBitCount > 8) {
		nTable = 0;
	}
	else{
		nTable = 256;
	}

	dwImageSize =  WIDTH_BYTES(pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biBitCount) * 
					pBmpInfo->bmiHeader.biHeight;
	
	if (dwImageSize <= 0) {
		bRVal = FALSE;
	}
	else{
		bfh.bfType		= (WORD)'M' << 8 | 'B';
		bfh.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nTable * sizeof(RGBQUAD);
		bfh.bfSize		= bfh.bfOffBits + dwImageSize;			
		
		HANDLE hFile = ::CreateFile(lpFileName,
			GENERIC_WRITE ,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (hFile == INVALID_HANDLE_VALUE) {
			bRVal = FALSE;
		}
		else{
			dwSize = sizeof(BITMAPFILEHEADER);
			::WriteFile(hFile, &bfh, dwSize, &dwBytesRead, NULL );
			
			dwSize = sizeof(BITMAPINFOHEADER) + nTable * sizeof(RGBQUAD);
			::WriteFile(hFile, pBmpInfo, dwSize, &dwBytesRead, NULL );
			
			dwSize = dwImageSize;
			WriteFile(hFile, pImageBuffer, dwSize, &dwBytesRead, NULL );
			
			CloseHandle(hFile);
		}
	}
	
	return bRVal;
}



BOOL CVisonDlg::OpenRawFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pRawBuffer)
{
	BOOL bRVal = TRUE;
	DWORD dwBytesRead = 0;

	HANDLE hFile = ::CreateFile(lpFileName,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL
								);
	if (hFile == INVALID_HANDLE_VALUE) {
		bRVal = FALSE;
	}
	else{
		::ReadFile(hFile, &(pBmpInfo->bmiHeader.biWidth), sizeof(DWORD), &dwBytesRead, NULL);
		::ReadFile(hFile, &(pBmpInfo->bmiHeader.biHeight), sizeof(DWORD), &dwBytesRead, NULL);
			
		::ReadFile(hFile, pRawBuffer, pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biHeight, &dwBytesRead, NULL);
		
		CloseHandle(hFile);
	}
	
	return bRVal;
}




BOOL CVisonDlg::SaveRawFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pRawBuffer)	
{
	BOOL bRVal				= TRUE;
	DWORD dwBytesRead		= 0;
	DWORD dwImageSize		= 0;

	dwImageSize =  pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biHeight;
	
	if (dwImageSize <= 0) {
		bRVal = FALSE;
	}
	else{
		HANDLE hFile = ::CreateFile(lpFileName,
			GENERIC_WRITE ,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (hFile == INVALID_HANDLE_VALUE) {
			bRVal = FALSE;
		}
		else{	
			::WriteFile(hFile, &(pBmpInfo->bmiHeader.biWidth), sizeof(DWORD), &dwBytesRead, NULL );
			::WriteFile(hFile, &(pBmpInfo->bmiHeader.biHeight), sizeof(DWORD), &dwBytesRead, NULL );
			::WriteFile(hFile, pRawBuffer, dwImageSize, &dwBytesRead, NULL );
			
			CloseHandle(hFile);
		}
	}
	
	return bRVal;
}


int CVisonDlg::GetFileExt(LPCSTR lpFileName)
{
	int nRVal = 0;

	int nLength			= 0;
	char cFileExt[5]	= {0};

	nLength = strlen(lpFileName);
	CopyMemory(cFileExt, (lpFileName + nLength - 4), 4);
	_strupr_s(cFileExt);
	if (strcmp(cFileExt, ".BMP") == 0) {
		nRVal = 0;
	}
	else if(strcmp(cFileExt, ".JPG") == 0){
		nRVal = 1;
	}
	else if(strcmp(cFileExt, ".RAW") == 0){
		nRVal = 2;
	}

	return nRVal;
}

void CVisonDlg::OnBnClickedBtnOpen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		if (HV_SUCCESS(Open())) 
	{
		m_bOpen = TRUE;
        UpdateUI();
	}
}


void CVisonDlg::OnBnClickedBtnClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (HV_SUCCESS(Close())) 
	{
		m_bOpen = FALSE;
		UpdateUI();
	}

}

void CVisonDlg::OnBnClickedBtnSnap()
{
		// TODO: �ڴ���ӿؼ�֪ͨ����������
  	if (HV_SUCCESS(Start())) 
	{
		m_bSnapping = TRUE;
		SetTimer(1, 100, NULL);
		UpdateUI();
	}
}


void CVisonDlg::OnBnClickedBtnStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (HV_SUCCESS(Stop())) 
	{
		m_bSnapping = FALSE;
		KillTimer(1);
		UpdateUI();
	}
}


void CVisonDlg::UpdateUI()
{
	bool bIsHV51Device = false;
	if ((m_hDeviceType == HV5051UCTYPE) || (m_hDeviceType == HV5051UMTYPE))
	{
		bIsHV51Device = true;
	}

	m_btnOpen.EnableWindow((!m_bOpen) && !m_bSnapping && bIsHV51Device);
	m_btnSnap.EnableWindow(m_bOpen && !m_bSnapping && bIsHV51Device);
	m_btnStop.EnableWindow(m_bOpen && m_bSnapping && bIsHV51Device);
	m_btnClose.EnableWindow(m_bOpen && !m_bSnapping && bIsHV51Device);
}

HVSTATUS CVisonDlg::Open()
{
	HVSTATUS status = HVOpenSnap(m_hhv, SnapCallback, this);					
	HV_VERIFY(status);
	
	return status;
}
HVSTATUS CVisonDlg::Close()
{
	HVSTATUS status = HVCloseSnap(m_hhv);
	HV_VERIFY(status);
	
	return status;
}
HVSTATUS  CVisonDlg::Start()
{
	BYTE *ppBuf[1];
	ppBuf[0] = m_pRawBuffer;
	HVSTATUS status = HVStartSnap(m_hhv, ppBuf, 1);
	HV_VERIFY(status);
	
	return status;
}

HVSTATUS CVisonDlg::Stop()
{
	HVSTATUS status = HVStopSnap(m_hhv);
	HV_VERIFY(status);
	
	return status;
}
int CALLBACK  CVisonDlg::SnapCallback(HV_SNAP_INFO *pInfo)
{
	 CVisonDlg *pDlg = ( CVisonDlg*)(pInfo->pParam);
	
	 CDC    *pDC= pDlg->m_staticPreview.GetDC();
	
	// Bayerͼ��ת��ΪRGBͼ��
	HVBayerToRGB(pDlg->m_hhv,                 // �豸���
		pDlg->m_pRawBuffer,                   // ԭʼ���ݻ�����(����)
		pDlg->m_nMaxWidth,                    // ͼ����
		pDlg->m_nMaxHeight,                   // ͼ��߶�
		true,                                 // ��תͼ��
		(HV_ADC_LEVEL)pDlg->m_nADCLevel,      // ģ��/����ת������
		ConvertType,                          // Raw2RGB�㷨
		pDlg->m_pImageBuffer,                 // Bayerת���󻺳���(���)
		&(pDlg->m_wbRatio)                    // ��ƽ�����ϵ��
		);

	
	// �����豸�����е�λͼ����ģʽ
	SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);

	// ��ȡ��ʾ����Ĵ�С
	CRect rc;
	pDlg->m_staticFrame.GetClientRect(&rc);

	// ����ʾ������ʾͼ��
	StretchDIBits(pDC->GetSafeHdc(),
		0,						
		0,
		rc.Width(),				//��ʾ���ڿ��
		rc.Height(),			//��ʾ���ڸ߶�
		0,
		0,
		pDlg->m_nMaxWidth,		//ͼ����
		pDlg->m_nMaxHeight,		//ͼ��߶�
		pDlg->m_pImageBuffer,	//ͼ�񻺳���
		pDlg->m_pBmpInfo,		//BMPͼ��������Ϣ
		DIB_RGB_COLORS,
		SRCCOPY
		);
	
	pDlg->m_staticFrame.ReleaseDC(pDC);
	
	return 1;//������
}

void CVisonDlg::GetImageSize()
{
	HVSTATUS  status = STATUS_OK;
	
	int nResolutionSize = 0; // ���DESC_RESOLUTION����ռ�Ĵ�С
	
	status = HVGetDeviceInfo(m_hhv, DESC_RESOLUTION, NULL, &nResolutionSize);	          
	if (STATUS_OK != status)
	{
		AfxMessageBox(HVGetErrorString(status));
	}
	else
	{
		BYTE  *pbContext = NULL;
		pbContext = new BYTE[nResolutionSize];
		DWORD *pdContext = (DWORD *)pbContext;
		status = HVGetDeviceInfo(m_hhv, DESC_RESOLUTION, pdContext, &nResolutionSize);
		if (STATUS_OK != status)
		{
			if (NULL !=pbContext) 
			{
				delete []pbContext;
				pbContext=NULL;
			}			
			AfxMessageBox(HVGetErrorString(status));
		}
		else
		{		
			int nWid = 0;
			int nHei = 0;
			int nCon = 0;
			
			for(int i=0; i<(nResolutionSize/8); i++)    // (nResolutionSize/8)��ʾ������ֱ��ʵ�����
			{                                           // CCD�����ֻ��һ�ֱַ��� 
				nWid = *(pdContext + 2*nCon + 0);
				nHei = *(pdContext + 2*nCon + 1);
				
				m_nMaxWidth  = (m_nMaxWidth>nWid)? m_nMaxWidth:nWid;    //  ��÷ֱ������� 
				m_nMaxHeight = (m_nMaxHeight>nHei)? m_nMaxHeight:nHei;  //  ��÷ֱ�������
				
				nCon+=1;
			}
		}

		if (NULL !=pbContext) 
		{
			delete []pbContext;
			pbContext=NULL;
		}
	}
}

void CVisonDlg::OnConversionBest() 
{
	m_ConversionType = BAYER2RGB_ADAPTIVE;
	
}

void CVisonDlg::OnUpdateConversionBest(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ConversionType == BAYER2RGB_ADAPTIVE);
	
}

void CVisonDlg::OnConversionFast() 
{
	m_ConversionType = BAYER2RGB_NEIGHBOUR;
	
}

void CVisonDlg::OnUpdateConversionFast(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ConversionType == BAYER2RGB_NEIGHBOUR);
	
}

void CVisonDlg::OnConversionNormal() 
{
	m_ConversionType = BAYER2RGB_NEIGHBOUR1;
}

void CVisonDlg::OnUpdateConversionNormal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ConversionType == BAYER2RGB_NEIGHBOUR1);
	
}

void CVisonDlg::OnResolutionMode()
{
	// TODO: �ڴ���������������
	HVSTATUS status = STATUS_OK;
	m_Resolution = RES_MODE0;
	//�������������ǰ��ֹͣ�ɼ�
	if (m_bSnapping)
	{
		HVStopSnap(m_hhv);
	}

	status = HVSetResolution(m_hhv, RES_MODE0);
	AdjustHBlanking();
	status = HVSetOutputWindow(m_hhv, 
		m_rcOutputWindow.left,
		m_rcOutputWindow.top,
		m_rcOutputWindow.Width(),
		m_rcOutputWindow.Height()			
		);  

	//������������ں󣬿�ʼ�ɼ�
	if (m_bSnapping)
	{
		//ֻ����һ��������,
		BYTE * ppBuffer[1];
		ppBuffer[0] = m_pRawBuffer;
		status = HVStartSnap(m_hhv,ppBuffer,1);	
	}

}
void CVisonDlg::OnResolutionMode1()
{
	// TODO: �ڴ���������������
	HVSTATUS status = STATUS_OK;
	m_Resolution = RES_MODE1;
	//�������������ǰ��ֹͣ�ɼ�
	if (m_bSnapping)
	{
		HVStopSnap(m_hhv);
	}

	status = HVSetResolution(m_hhv, RES_MODE1);
	AdjustHBlanking();
	status = HVSetOutputWindow(m_hhv, 
		m_rcOutputWindow.left,
		m_rcOutputWindow.top,
		m_rcOutputWindow.Width(),
		m_rcOutputWindow.Height()			
		);  

	//������������ں󣬿�ʼ�ɼ�
	if (m_bSnapping)
	{
		//ֻ����һ��������,
		BYTE * ppBuffer[1];
		ppBuffer[0] = m_pRawBuffer;
		status = HVStartSnap(m_hhv,ppBuffer,1);	
	}
	
}
void CVisonDlg::OnResolutionMode2()
{
	// TODO: �ڴ���������������
	HVSTATUS status = STATUS_OK;
	m_Resolution = RES_MODE2;
	//�������������ǰ��ֹͣ�ɼ�
	if (m_bSnapping)
	{
		HVStopSnap(m_hhv);
	}

	status = HVSetResolution(m_hhv, RES_MODE2);
	AdjustHBlanking();
	status = HVSetOutputWindow(m_hhv, 
		m_rcOutputWindow.left,
		m_rcOutputWindow.top,
		m_rcOutputWindow.Width(),
		m_rcOutputWindow.Height()			
		);  

	//������������ں󣬿�ʼ�ɼ�
	if (m_bSnapping)
	{
		//ֻ����һ��������,
		BYTE * ppBuffer[1];
		ppBuffer[0] = m_pRawBuffer;
		status = HVStartSnap(m_hhv,ppBuffer,1);	
	}
}
void CVisonDlg::OnUpdateResolutionMode(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_Resolution == RES_MODE0);	
}
void CVisonDlg::OnUpdateResolutionMode1(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_Resolution == RES_MODE1);
}
void CVisonDlg::OnUpdateResolutionMode2(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_Resolution == RES_MODE2);
}
void CVisonDlg::OnImageModeColor()
{
	// TODO: �ڴ���������������
	m_ImageMode	= HV_COLOR;
}
void CVisonDlg::OnImageModeBw()
{
	// TODO: �ڴ���������������
	m_ImageMode	= HV_BW;
}
void CVisonDlg::OnUpdateImageModeColor(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck( m_ImageMode == HV_COLOR);
}
void CVisonDlg::OnUpdateImageModeBw(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck( m_ImageMode == HV_BW);
}
void CVisonDlg::OnSnapspeedHigh()
{
	// TODO: �ڴ���������������
	if(m_bIsSnapSpeedSprted)
	{
		m_SnapSpeed = HIGH_SPEED;
		HVSTATUS status = HVSetSnapSpeed(m_hhv,m_SnapSpeed);
		SetExposureTime(GetOutputWinWdith(),m_lTintUpper,m_lTintLower);

		InterlockedExchange(&m_dwCount, 0);
		m_dwStart = GetTickCount();
	}
}
void CVisonDlg::OnUpdateSnapspeedHigh(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_SnapSpeed == HIGH_SPEED);	
	pCmdUI->Enable(m_bIsSnapSpeedSprted);	
}
void CVisonDlg::OnSnapspeedNormal()
{
	// TODO: �ڴ���������������
		m_SnapSpeed = NORMAL_SPEED;
		HVSTATUS status = HVSetSnapSpeed(m_hhv,m_SnapSpeed);
		SetExposureTime(GetOutputWinWdith(),m_lTintUpper,m_lTintLower);

		InterlockedExchange(&m_dwCount, 0);
		m_dwStart = GetTickCount();
}
void CVisonDlg::OnUpdateSnapspeedNormal(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_SnapSpeed == NORMAL_SPEED);	
	pCmdUI->Enable(m_bIsSnapSpeedSprted);
}
void CVisonDlg::OnBayerdatalayoutGb()
{
	// TODO: �ڴ���������������
	m_Layout = BAYER_GB;
}
void CVisonDlg::OnUpdateBayerdatalayoutGb(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_Layout == BAYER_GB);
}
void CVisonDlg::OnBayerdatalayoutGr()
{
	// TODO: �ڴ���������������
	m_Layout = BAYER_GR;
}
void CVisonDlg::OnUpdateBayerdatalayoutGr(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_Layout == BAYER_GR);
}
void CVisonDlg::OnBayerdatalayoutBg()
{
	// TODO: �ڴ���������������
	m_Layout = BAYER_BG;
}
void CVisonDlg::OnUpdateBayerdatalayoutBg(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck( m_Layout == BAYER_BG);
}
void CVisonDlg::OnBayerdatalayoutRg()
{
	// TODO: �ڴ���������������
	m_Layout = BAYER_RG;
}
void CVisonDlg::OnUpdateBayerdatalayoutRg(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_Layout == BAYER_RG);
}
void CVisonDlg::OnSnapmodeContinuation()
{
	// TODO: �ڴ���������������
		HVSTATUS status = HVSetSnapMode(m_hhv, CONTINUATION);
	if (HV_SUCCESS(status)) {
		m_SnapMode = CONTINUATION;
	}
}
void CVisonDlg::OnUpdateSnapmodeContinuation(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_SnapMode == CONTINUATION);
	pCmdUI->Enable( ! m_bSnapping);
}
void CVisonDlg::OnSnapmodeTrigger()
{
	// TODO: �ڴ���������������
	HVSTATUS status = HVSetSnapMode(m_hhv, TRIGGER);
	if (HV_SUCCESS(status)){
		m_SnapMode = TRIGGER;
	}
}
void CVisonDlg::OnUpdateSnapmodeTrigger(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_SnapMode == TRIGGER);
	pCmdUI->Enable( ! m_bSnapping);
}

HVSTATUS CVisonDlg::SetExposureTime(int nWindWidth, long lTintUpper, long lTintLower)
{
	HVTYPE type = UNKNOWN_TYPE;
	int size    = sizeof(HVTYPE);
	HVGetDeviceInfo(m_hhv,DESC_DEVICE_TYPE, &type, &size);	

	int nOutputWid = nWindWidth;

	double dExposure = 0.0;
	double dTint = max((double)lTintUpper/(double)lTintLower,MY_ZERO);

	double lClockFreq = 0.0;  
	if (IS_USB_GV400(type))
	{
		long lTb = m_nHBlanking;
		lTb += 0x5e;	
		lClockFreq = (m_SnapSpeed == HIGH_SPEED)? 26600000.0:13300000.0; 		
		long rate = 0;

		switch(m_Resolution)
		{
		case RES_MODE0:
			rate = 1;
			break;
		case RES_MODE1:
			rate = 2;
			break;

		default:
			return STATUS_PARAMETER_OUT_OF_BOUND;
		}
		
		nOutputWid = nOutputWid * rate;	//width*rate
		if ((dTint * lClockFreq) <= (nOutputWid+ lTb - 255))
		{
			dExposure = 1;
		}
		else
			dExposure = ((dTint * lClockFreq) - (nOutputWid + lTb - 255) ) / (nOutputWid + lTb);				

		if (dExposure < 3) 
			dExposure = 3;
		else if (dExposure > 32766)
			dExposure = 32766;

	}
	else if( IS_USB_HV300(type))
	{
		lClockFreq = (m_SnapSpeed == HIGH_SPEED)? 24000000:12000000;
		long lTb = m_nHBlanking;
		lTb += 142;
		if(lTb<21)
			lTb = 21;
		long lP1 = 331, lP2 = 38;
		long lp4 = 316;
		if(m_Resolution == RES_MODE1)
		{
			lP1 = 673;
			lP2 = 22;
			lp4 = 316*2;
		}
		long lAQ = (long)(nOutputWid + lP1 + lP2 + lTb);
		long lTrow = max(lAQ,(lP1+lp4));
		dExposure = ((dTint* lClockFreq)+ lP1 - 132.0)/lTrow;	
		
		if((dExposure-(int)dExposure) >0.5)
			dExposure += 1.0;		
		if(dExposure <= 0)
			dExposure = 1;
		else if(dExposure > 1048575) 
			dExposure = 1048575;
	}
	else if(IS_USB_HV200(type))
	{
		lClockFreq = (m_SnapSpeed == HIGH_SPEED)? 24000000:12000000;
		long lTb = m_nHBlanking;
		lTb += 53;
		if(lTb<19)
			lTb = 19;
		long lAQ = (long)(nOutputWid + 305.0 + lTb);
		long lTrow = max(617,lAQ);
		dExposure = (dTint* lClockFreq + 180.0)/lTrow + 1;
		
		if((dExposure-(int)dExposure) >0.5)//��������
			dExposure += 1.0;
		if(dExposure <= 0)
			dExposure = 1;
		else if(dExposure > 16383) 
			dExposure = 16383;
	}
	else if(IS_USB_HV5051(type))
	{
		SHUTTER_UNIT_VALUE nUnit = SHUTTER_MS;
		HVSTATUS status          = STATUS_OK;

		if (lTintLower == 1000000)
		{
			nUnit = SHUTTER_US;
		}
		
		//�����ع�ʱ�䵥λ
		status = HVAECControl(m_hhv, AEC_SHUTTER_UNIT, nUnit);
		if (!HV_SUCCESS(status))
		{
			return status;
		}
		
		//�����ع�ʱ�䡣
		return HVAECControl(m_hhv, AEC_SHUTTER_SPEED, lTintUpper);			
	}
	else
	{		
		lClockFreq = (m_SnapSpeed == HIGH_SPEED)? 24000000:12000000;
		long lTb = m_nHBlanking;
		lTb += 9;
		lTb -= 19;	
		if(lTb <= 0) lTb =0;
		if(((double)nOutputWid + 244.0 + lTb ) > 552)	
			dExposure = (dTint* lClockFreq + 180.0)/((double)nOutputWid + 244.0 + lTb);
		else
			dExposure = ((dTint * lClockFreq)+ 180.0) / 552 ;

		if((dExposure-(int)dExposure) >0.5)
			dExposure += 1.0;
		if(dExposure <= 0)
			dExposure = 1;
		else if(dExposure > 16383) 
			dExposure = 16383;
	}

	return HVAECControl(m_hhv, AEC_EXPOSURE_TIME, (long)dExposure);
}

//�õ�������ڴ�С
int CVisonDlg::GetOutputWinWdith()
{
	return m_rcOutputWindow.Width();
}

////PLC����
void CVisonDlg::InitMSComm()
{
	m_ctrlComm.put_CommPort(5);//ѡ�񴮿�
	m_ctrlComm.put_InputMode(1);//������������Ϊ�����Ʒ�ʽ
	m_ctrlComm.put_InBufferSize(1024);//�������뻺������С
	m_ctrlComm.put_OutBufferSize(512);//���������������С
	m_ctrlComm.put_Settings("9600,e,7,1");//������9600��żУ�飬7������λ��1��ֹͣλ
	if(!m_ctrlComm.get_PortOpen())
		m_ctrlComm.put_PortOpen(TRUE);//���ڴ�
	else
		AfxMessageBox(_T("���ڳ���"));
	m_ctrlComm.put_RThreshold(1);//����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_ctrlComm.put_InputLen(0);//���õ�ǰ���������ݳ���Ϊ0
	m_ctrlComm.get_Input();//��Ԥ���������������������

}

void CVisonDlg::OnBnClickedBtnEnq()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CByteArray hexdata;
	int len =String2Hex("05",hexdata);//�˴����ص�len�������ڼ��㷢���˶��ٸ�ʮ��������
	m_ctrlComm.put_Output(COleVariant(hexdata));//����ʮ����������
}


void CVisonDlg::OnBnClickedBtnSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);//��ȡ�༭������
	CByteArray hexdata;
	int len =String2Hex(m_strSendMsg,hexdata);//�˴����ص�len�������ڼ��㷢���˶��ٸ�ʮ��������
	m_ctrlComm.put_Output(COleVariant(hexdata));//����ʮ����������
}
BEGIN_EVENTSINK_MAP(CVisonDlg, CDialogEx)
	ON_EVENT(CVisonDlg, IDC_MSCOMM1, 1, CVisonDlg::OnCommMscomm, VTS_NONE)
END_EVENTSINK_MAP()


void CVisonDlg::OnCommMscomm()
{
	// TODO: �ڴ˴������Ϣ����������
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048];//����BYTE����
	CString strtemp;
	if (m_ctrlComm.get_CommEvent()==2)//�¼�ֵΪ2��ʾ���ջ����������ַ�
	{
	   //���¿��Ը���ͨѶЭ����봦�����
	   variant_inp = m_ctrlComm.get_Input();//��ȡ���ջ�����
	   safearray_inp = variant_inp;//variant�ͱ���ת��ΪColeSafeArray�ͱ���
	   len = safearray_inp.GetOneDimSize();//�õ���Ч���ݳ���
	   for(k=0;k<len;k++)        //������ת��ΪCstring�ͱ���
	   safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
	   for(k=0;k<len;k++)//������ת��ΪCstring�ͱ���
	   {
		   BYTE bt=*(char*)(rxdata+k);//�ַ���
	   strtemp.Format("%02X ",bt);//X��ʾ���ַ���ʮ�����Ʒ�ʽ������ʱ����strtemp��ţ�02X��Ҫ��һ���ո�
	   m_strReceiveMsg+=strtemp;//������ձ༭���ڶ�Ӧ�ַ���
	   }
	}
	UpdateData(FALSE);//���±༭������

}

//���ַ���תʮ�����ƣ���������senddata�����У�ͬʱ�����ַ����ĳ���ֵ
int  CVisonDlg::String2Hex(CString str, CByteArray &senddata)
{
	int   hexdata,lowhexdata;   
	int   hexdatalen=0;   
    int   len=str.GetLength();   
	senddata.SetSize(len/2);   
	for(int   i=0;i<len;)   
	{   
		char lstr,hstr=str[i];  
		if(hstr ==' ')  
		{   
			i++;   
			continue;   
		}   
		i++;   
		if(i>=len)   
			break;   
		lstr=str[i];   
		hexdata=ConvertHexChar(hstr);   
       lowhexdata=ConvertHexChar(lstr);   
	   if((hexdata==16)||(lowhexdata==16))   
		   break;   
	   else     
		   hexdata=hexdata*16+lowhexdata;   
	   i++;   
	   senddata[hexdatalen]=(char)hexdata;   
	   hexdatalen++;   
	}   
	senddata.SetSize(hexdatalen);   
	return   hexdatalen; 
}

char CVisonDlg::ConvertHexChar(char ch)
{
  if((ch>='0')&&(ch<='9'))   
	  return   ch-0x30;   
  else   if((ch>='A')&&(ch<='F'))   
	  return   ch-'A'+10;   
  else   if((ch>='a')&&(ch<='f'))   
	  return   ch-'a'+10;   
  else 
	  return   0x10;   
}

void CVisonDlg::OnBnClickedBtnTest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CByteArray Y0_ON;
	CByteArray Y0_OFF;
	m_bitmap.SetBitmap(m_hIcon_ON);
	String2Hex("02 37 30 30 30 35 03 46 46",Y0_ON);//Y0��ON,���ʹ�����
	m_ctrlComm.put_Output(COleVariant(Y0_ON));//����ʮ����������
	Sleep(1000);
	String2Hex("02 38 30 30 30 35 03 30 30",Y0_OFF);//Y0��OFF�����ʹ�ֹͣ
	m_ctrlComm.put_Output(COleVariant(Y0_OFF));//����ʮ����������
}






void CVisonDlg::OnBnClickedBtnCalibration()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CCalibration Dlg;
	Dlg.DoModal();
}

void CVisonDlg::disp_continue_message (HTuple hv_WindowHandle, HTuple hv_Color, HTuple hv_Box)
{

  // Local iconic variables

  // Local control variables
  HTuple  hv_ContinueMessage, hv_Row, hv_Column;
  HTuple  hv_Width, hv_Height, hv_Ascent, hv_Descent, hv_TextWidth;
  HTuple  hv_TextHeight;

  //This procedure displays 'Press Run (F5) to continue' in the
  //lower right corner of the screen.
  //It uses the procedure disp_message.
  //
  //Input parameters:
  //WindowHandle: The window, where the text shall be displayed
  //Color: defines the text color.
  //   If set to '' or 'auto', the currently set color is used.
  //Box: If set to 'true', the text is displayed in a box.
  //
  hv_ContinueMessage = "���������ʾ";
  GetWindowExtents(hv_WindowHandle, &hv_Row, &hv_Column, &hv_Width, &hv_Height);
  GetStringExtents(hv_WindowHandle, (" "+hv_ContinueMessage)+" ", &hv_Ascent, &hv_Descent, 
      &hv_TextWidth, &hv_TextHeight);
  disp_message(hv_WindowHandle, hv_ContinueMessage, "window", (hv_Height-hv_TextHeight)-12, 
      (hv_Width-hv_TextWidth)-12, hv_Color, hv_Box);
  return;
}

// Chapter: Graphics / Text
// Short Description: This procedure writes a text message. 
void CVisonDlg::disp_message (HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, 
    HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box)
{

  // Local iconic variables

  // Local control variables
  HTuple  hv_Red, hv_Green, hv_Blue, hv_Row1Part;
  HTuple  hv_Column1Part, hv_Row2Part, hv_Column2Part, hv_RowWin;
  HTuple  hv_ColumnWin, hv_WidthWin, hv_HeightWin, hv_MaxAscent;
  HTuple  hv_MaxDescent, hv_MaxWidth, hv_MaxHeight, hv_R1;
  HTuple  hv_C1, hv_FactorRow, hv_FactorColumn, hv_UseShadow;
  HTuple  hv_ShadowColor, hv_Exception, hv_Width, hv_Index;
  HTuple  hv_Ascent, hv_Descent, hv_W, hv_H, hv_FrameHeight;
  HTuple  hv_FrameWidth, hv_R2, hv_C2, hv_DrawMode, hv_CurrentColor;

  //This procedure displays text in a graphics window.
  //
  //Input parameters:
  //WindowHandle: The WindowHandle of the graphics window, where
  //   the message should be displayed
  //String: A tuple of strings containing the text message to be displayed
  //CoordSystem: If set to 'window', the text position is given
  //   with respect to the window coordinate system.
  //   If set to 'image', image coordinates are used.
  //   (This may be useful in zoomed images.)
  //Row: The row coordinate of the desired text position
  //   If set to -1, a default value of 12 is used.
  //Column: The column coordinate of the desired text position
  //   If set to -1, a default value of 12 is used.
  //Color: defines the color of the text as string.
  //   If set to [], '' or 'auto' the currently set color is used.
  //   If a tuple of strings is passed, the colors are used cyclically
  //   for each new textline.
  //Box: If Box[0] is set to 'true', the text is written within an orange box.
  //     If set to' false', no box is displayed.
  //     If set to a color string (e.g. 'white', '#FF00CC', etc.),
  //       the text is written in a box of that color.
  //     An optional second value for Box (Box[1]) controls if a shadow is displayed:
  //       'true' -> display a shadow in a default color
  //       'false' -> display no shadow (same as if no second value is given)
  //       otherwise -> use given string as color string for the shadow color
  //
  //Prepare window
  GetRgb(hv_WindowHandle, &hv_Red, &hv_Green, &hv_Blue);
  GetPart(hv_WindowHandle, &hv_Row1Part, &hv_Column1Part, &hv_Row2Part, &hv_Column2Part);
  GetWindowExtents(hv_WindowHandle, &hv_RowWin, &hv_ColumnWin, &hv_WidthWin, &hv_HeightWin);
  SetPart(hv_WindowHandle, 0, 0, hv_HeightWin-1, hv_WidthWin-1);
  //
  //default settings
  if (0 != (hv_Row==-1))
  {
    hv_Row = 12;
  }
  if (0 != (hv_Column==-1))
  {
    hv_Column = 12;
  }
  if (0 != (hv_Color==HTuple()))
  {
    hv_Color = "";
  }
  //
  hv_String = ((""+hv_String)+"").TupleSplit("\n");
  //
  //Estimate extentions of text depending on font size.
  GetFontExtents(hv_WindowHandle, &hv_MaxAscent, &hv_MaxDescent, &hv_MaxWidth, &hv_MaxHeight);
  if (0 != (hv_CoordSystem==HTuple("window")))
  {
    hv_R1 = hv_Row;
    hv_C1 = hv_Column;
  }
  else
  {
    //Transform image to window coordinates
    hv_FactorRow = (1.*hv_HeightWin)/((hv_Row2Part-hv_Row1Part)+1);
    hv_FactorColumn = (1.*hv_WidthWin)/((hv_Column2Part-hv_Column1Part)+1);
    hv_R1 = ((hv_Row-hv_Row1Part)+0.5)*hv_FactorRow;
    hv_C1 = ((hv_Column-hv_Column1Part)+0.5)*hv_FactorColumn;
  }
  //
  //Display text box depending on text size
  hv_UseShadow = 1;
  hv_ShadowColor = "gray";
  if (0 != (HTuple(hv_Box[0])==HTuple("true")))
  {
    hv_Box[0] = "#fce9d4";
    hv_ShadowColor = "#f28d26";
  }
  if (0 != ((hv_Box.TupleLength())>1))
  {
    if (0 != (HTuple(hv_Box[1])==HTuple("true")))
    {
      //Use default ShadowColor set above
    }
    else if (0 != (HTuple(hv_Box[1])==HTuple("false")))
    {
      hv_UseShadow = 0;
    }
    else
    {
      hv_ShadowColor = ((const HTuple&)hv_Box)[1];
      //Valid color?
      try
      {
        SetColor(hv_WindowHandle, HTuple(hv_Box[1]));
      }
      // catch (Exception) 
      catch (HalconCpp::HException &HDevExpDefaultException)
      {
        HDevExpDefaultException.ToHTuple(&hv_Exception);
        hv_Exception = "Wrong value of control parameter Box[1] (must be a 'true', 'false', or a valid color string)";
        throw HalconCpp::HException(hv_Exception);
      }
    }
  }
  if (0 != (HTuple(hv_Box[0])!=HTuple("false")))
  {
    //Valid color?
    try
    {
      SetColor(hv_WindowHandle, HTuple(hv_Box[0]));
    }
    // catch (Exception) 
    catch (HalconCpp::HException &HDevExpDefaultException)
    {
      HDevExpDefaultException.ToHTuple(&hv_Exception);
      hv_Exception = "Wrong value of control parameter Box[0] (must be a 'true', 'false', or a valid color string)";
      throw HalconCpp::HException(hv_Exception);
    }
    //Calculate box extents
    hv_String = (" "+hv_String)+" ";
    hv_Width = HTuple();
    {
    HTuple end_val93 = (hv_String.TupleLength())-1;
    HTuple step_val93 = 1;
    for (hv_Index=0; hv_Index.Continue(end_val93, step_val93); hv_Index += step_val93)
    {
      GetStringExtents(hv_WindowHandle, HTuple(hv_String[hv_Index]), &hv_Ascent, 
          &hv_Descent, &hv_W, &hv_H);
      hv_Width = hv_Width.TupleConcat(hv_W);
    }
    }
    hv_FrameHeight = hv_MaxHeight*(hv_String.TupleLength());
    hv_FrameWidth = (HTuple(0).TupleConcat(hv_Width)).TupleMax();
    hv_R2 = hv_R1+hv_FrameHeight;
    hv_C2 = hv_C1+hv_FrameWidth;
    //Display rectangles
    GetDraw(hv_WindowHandle, &hv_DrawMode);
    SetDraw(hv_WindowHandle, "fill");
    //Set shadow color
    SetColor(hv_WindowHandle, hv_ShadowColor);
    if (0 != hv_UseShadow)
    {
      DispRectangle1(hv_WindowHandle, hv_R1+1, hv_C1+1, hv_R2+1, hv_C2+1);
    }
    //Set box color
    SetColor(hv_WindowHandle, HTuple(hv_Box[0]));
    DispRectangle1(hv_WindowHandle, hv_R1, hv_C1, hv_R2, hv_C2);
    SetDraw(hv_WindowHandle, hv_DrawMode);
  }
  //Write text.
  {
  HTuple end_val115 = (hv_String.TupleLength())-1;
  HTuple step_val115 = 1;
  for (hv_Index=0; hv_Index.Continue(end_val115, step_val115); hv_Index += step_val115)
  {
    hv_CurrentColor = ((const HTuple&)hv_Color)[hv_Index%(hv_Color.TupleLength())];
    if (0 != (HTuple(hv_CurrentColor!=HTuple("")).TupleAnd(hv_CurrentColor!=HTuple("auto"))))
    {
      SetColor(hv_WindowHandle, hv_CurrentColor);
    }
    else
    {
      SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
    }
    hv_Row = hv_R1+(hv_MaxHeight*hv_Index);
    SetTposition(hv_WindowHandle, hv_Row, hv_C1);
    WriteString(hv_WindowHandle, HTuple(hv_String[hv_Index]));
  }
  }
  //Reset changed window settings
  SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
  SetPart(hv_WindowHandle, hv_Row1Part, hv_Column1Part, hv_Row2Part, hv_Column2Part);
  return;
}


void CVisonDlg::OnBnClickedBtnLoad()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ReadImage(&ho_Image, "F:/ʵ��/Image009.bmp");
	GetImagePointer1(ho_Image, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);
    SetWindowAttr("background_color","black");
	HWND hwnd1;
	CRect rect;
	GetDlgItem(IDC_STATIC_Frame)->GetWindowRect(&rect);
	hwnd1=GetDlgItem(IDC_STATIC_Frame)->m_hWnd;
    OpenWindow(0,0,rect.Width(),rect.Height(),(LONG)hwnd1,"","",&hv_WindowHandle);
    HDevWindowStack::Push(hv_WindowHandle);
    if (HDevWindowStack::IsOpen())
		DispObj(ho_Image, HDevWindowStack::GetActive());
	
	ReadImage(&ho_ImageV, "F:/ʵ��/Image003.bmp");
	 GetImagePointer1(ho_ImageV, &hv_PointerV, &hv_TypeV, &hv_WidthV, &hv_HeightV);
    SetWindowAttr("background_color","black");
	HWND hwnd2;
	CRect rect2;
	GetDlgItem(IDC_STATIC_Preview)->GetWindowRect(&rect2);
	hwnd2=GetDlgItem(IDC_STATIC_Preview)->m_hWnd;
	m_width2[0]=rect2.Width();
	m_height2[0]=rect2.Height();
    OpenWindow(0,0,m_width2,m_height2,(LONG)hwnd2,"","",&hv_WindowHandleV);
    hv_ROI_0.Clear();
    hv_ROI_0[0] = 100;
    hv_ROI_0[1] = 300;
    hv_ROI_0[2] = 900;
    hv_ROI_0[3] = 1000;
    if (HDevWindowStack::IsOpen())
		SetPart(hv_WindowHandleV,HTuple(hv_ROI_0[0]).TupleRound(), HTuple(hv_ROI_0[1]).TupleRound(), 
        HTuple(hv_ROI_0[2]).TupleRound(), HTuple(hv_ROI_0[3]).TupleRound());
    if (HDevWindowStack::IsOpen())
		DispObj(ho_ImageV, hv_WindowHandleV);


}


void CVisonDlg::OnBnClickedPicturecorrection()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CCalibration *p=(CCalibration*)AfxGetApp();
	hv_CameraParameters=p->hv_CameraParameters;
	hv_CameraPose=p->hv_CameraPose;
	// ReadCamPar("F:/ʵ��/calibration2/����.cal", &hv_CameraParameters);
    // ReadPose("F:/ʵ��/calibration2/λ��.dat", &hv_CameraPose);
	GenRectangle1(&ho_Rectangle, 0, 0, hv_Height-1, hv_Width-1);
    GenContourRegionXld(ho_Rectangle, &ho_ImageBorder, "border");
    ContourToWorldPlaneXld(ho_ImageBorder, &ho_ContoursTrans, hv_CameraParameters, hv_CameraPose, 1);
    SmallestRectangle1Xld(ho_ContoursTrans, &hv_MinY, &hv_MinX, &hv_MaxY, &hv_MaxX);
    SetOriginPose(hv_CameraPose, hv_MinX, hv_MinY, 0.01, &hv_PoseForEntireImage);
    ImagePointsToWorldPlane(hv_CameraParameters, hv_PoseForEntireImage, ((hv_Height/2).TupleConcat(hv_Height/2)).TupleConcat((hv_Height/2)+1), 
      ((hv_Width/2).TupleConcat((hv_Width/2)+1)).TupleConcat(hv_Width/2), 1, &hv_WorldPixelX, &hv_WorldPixelY);
    DistancePp(HTuple(hv_WorldPixelY[0]), HTuple(hv_WorldPixelX[0]), HTuple(hv_WorldPixelY[1]), 
      HTuple(hv_WorldPixelX[1]), &hv_WorldLength1);
    DistancePp(HTuple(hv_WorldPixelY[0]), HTuple(hv_WorldPixelX[0]), HTuple(hv_WorldPixelY[2]), 
      HTuple(hv_WorldPixelX[2]), &hv_WorldLength2);
    hv_ScaleForSimilarPixelSize = (hv_WorldLength1+hv_WorldLength2)/2;
    hv_ExtentX = hv_MaxX-hv_MinX;
    hv_ExtentY = hv_MaxY-hv_MinY;
    hv_WidthRectifiedImage = hv_ExtentX/hv_ScaleForSimilarPixelSize;
    hv_HeightRectifiedImage = hv_ExtentY/hv_ScaleForSimilarPixelSize;
    GenImageToWorldPlaneMap(&ho_Map, hv_CameraParameters, hv_PoseForEntireImage, hv_Width, 
    hv_Height, hv_WidthRectifiedImage, hv_HeightRectifiedImage, hv_ScaleForSimilarPixelSize, "bilinear");
    MapImage(ho_Image, ho_Map, &ho_ImageMapped);
    if (HDevWindowStack::IsOpen())
		DispObj(ho_ImageMapped, HDevWindowStack::GetActive());
	
	CCalibration *pV=(CCalibration*)AfxGetApp();
	hv_CameraParametersV=pV->hv_CameraParametersV;
	hv_CameraPoseV=pV->hv_CameraPoseV;
    GetImagePointer1(ho_ImageV, &hv_PointerV, &hv_TypeV, &hv_WidthV, &hv_HeightV);
    GenRectangle1(&ho_RectangleV, 0, 0, hv_HeightV-1, hv_WidthV-1);
    GenContourRegionXld(ho_RectangleV, &ho_ImageBorderV, "border");
    ContourToWorldPlaneXld(ho_ImageBorderV, &ho_ContoursTransV, hv_CameraParametersV, hv_CameraPoseV, 1);
    SmallestRectangle1Xld(ho_ContoursTransV, &hv_MinYV, &hv_MinXV, &hv_MaxYV, &hv_MaxXV);
  SetOriginPose(hv_CameraPoseV, hv_MinXV, hv_MinYV, 0.01, &hv_PoseForEntireImageV);
  ImagePointsToWorldPlane(hv_CameraParametersV, hv_PoseForEntireImageV, ((hv_HeightV/2).TupleConcat(hv_HeightV/2)).TupleConcat((hv_HeightV/2)+1), 
      ((hv_WidthV/2).TupleConcat((hv_WidthV/2)+1)).TupleConcat(hv_WidthV/2), 1, &hv_WorldPixelXV, 
      &hv_WorldPixelYV);
  DistancePp(HTuple(hv_WorldPixelYV[0]), HTuple(hv_WorldPixelXV[0]), HTuple(hv_WorldPixelYV[1]), 
      HTuple(hv_WorldPixelXV[1]), &hv_WorldLength1V);
  DistancePp(HTuple(hv_WorldPixelYV[0]), HTuple(hv_WorldPixelXV[0]), HTuple(hv_WorldPixelYV[2]), 
      HTuple(hv_WorldPixelXV[2]), &hv_WorldLength2V);
  hv_ScaleForSimilarPixelSizeV = (hv_WorldLength1V+hv_WorldLength2V)/2;
  hv_ExtentXV = hv_MaxXV-hv_MinXV;
  hv_ExtentYV = hv_MaxYV-hv_MinYV;
  hv_WidthRectifiedImageV = hv_ExtentXV/hv_ScaleForSimilarPixelSizeV;
  hv_HeightRectifiedImageV = hv_ExtentYV/hv_ScaleForSimilarPixelSizeV;
  GenImageToWorldPlaneMap(&ho_MapV, hv_CameraParametersV, hv_PoseForEntireImageV, 
      hv_WidthV, hv_HeightV, hv_WidthRectifiedImageV, hv_HeightRectifiedImageV, hv_ScaleForSimilarPixelSizeV, 
      "bilinear");
  MapImage(ho_ImageV, ho_MapV, &ho_ImageMappedV);
   if (HDevWindowStack::IsOpen())
		SetPart(hv_WindowHandleV,HTuple(hv_ROI_0[0]).TupleRound(), HTuple(hv_ROI_0[1]).TupleRound(), 
        HTuple(hv_ROI_0[2]).TupleRound(), HTuple(hv_ROI_0[3]).TupleRound());
    if (HDevWindowStack::IsOpen())
		DispObj(ho_ImageMappedV, hv_WindowHandleV);
}


void CVisonDlg::OnBnClickedPictureprocessing()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	 Rgb1ToGray(ho_ImageMapped, &ho_GrayImage);
  Threshold(ho_GrayImage, &ho_Regions, 90, 255);
  Connection(ho_Regions, &ho_ConnectedRegions);
  SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, (HTuple("area").Append("row")), 
      "and", (HTuple(5522.94).Append(159.63)), (HTuple(20000).Append(2000)));
  DilationCircle(ho_SelectedRegions, &ho_RoiOuter, 5);
  ErosionCircle(ho_SelectedRegions, &ho_RoiInner, 3);
  Difference(ho_RoiOuter, ho_RoiInner, &ho_RegionDifference);
  Union1(ho_RegionDifference, &ho_RegionEdges);
  ReduceDomain(ho_GrayImage, ho_RegionEdges, &ho_ImageReduced);
if (HDevWindowStack::IsOpen())
    ClearWindow(HDevWindowStack::GetActive());
  EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 20, 40);
  if (HDevWindowStack::IsOpen())
    DispObj(ho_Edges, HDevWindowStack::GetActive());
  SelectShapeXld(ho_Edges, &ho_SelectedXLD, "contlength", "and", 140, 1000);
  if (HDevWindowStack::IsOpen())
    ClearWindow(HDevWindowStack::GetActive());
  if (HDevWindowStack::IsOpen())
    SetColored(HDevWindowStack::GetActive(),12);
  if (HDevWindowStack::IsOpen())
    DispObj(ho_SelectedXLD, HDevWindowStack::GetActive());
  SegmentContoursXld(ho_SelectedXLD, &ho_ContoursSplit, "lines_circles", 0, 3, 2);
  CountObj(ho_ContoursSplit, &hv_Number);
  GenEmptyObj(&ho_Lines);
  GenEmptyObj(&ho_Circles);
  hv_NumCircles = 0;
  hv_NumLines = 0;
  {
  HTuple end_val58 = hv_Number;
  HTuple step_val58 = 1;
  for (hv_i=1; hv_i.Continue(end_val58, step_val58); hv_i += step_val58)
  {
    SelectObj(ho_ContoursSplit, &ho_ObjectSelected, hv_i);
    GetContourGlobalAttribXld(ho_ObjectSelected, "cont_approx", &hv_Attrib);
    if (0 != (hv_Attrib>0))
    {
      ConcatObj(ho_Circles, ho_ObjectSelected, &ho_Circles);
      hv_NumCircles += 1;
    }
    else
    {
      ConcatObj(ho_Lines, ho_ObjectSelected, &ho_Lines);
      hv_NumLines += 1;
    }
  }
  }
  UnionCollinearContoursXld(ho_Lines, &ho_Lines, 10, 1, 2, 0.1, "attr_keep");
  SelectShapeXld(ho_Lines, &ho_SelectedLines, "contlength", "and", 65.5, 200);
  FitLineContourXld(ho_SelectedLines, "regression", -1, 0, 5, 2, &hv_RowBegin, &hv_ColBegin, 
      &hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);
 if (HDevWindowStack::IsOpen())
    SetColor(HDevWindowStack::GetActive(),"magenta");
  if (HDevWindowStack::IsOpen())
    SetLineWidth(HDevWindowStack::GetActive(),2);
  if (HDevWindowStack::IsOpen())
    DispObj(ho_ImageMapped, HDevWindowStack::GetActive());
  if (HDevWindowStack::IsOpen())
    DispObj(ho_SelectedLines, HDevWindowStack::GetActive());
  hv_Message = "����ֱ�ߣ�����нǣ�";
  disp_message(hv_WindowHandle, hv_Message, "window", 150, 12, "black", "true");
  disp_continue_message(hv_WindowHandle, "black", "true");


  //��ֱͼ�������
  Rgb1ToGray(ho_ImageMappedV, &ho_ImageGrayV);
  //GetImageSize(ho_ImageGrayV, &hv_WidthV, &hv_HeightV);
  if (HDevWindowStack::IsOpen())
    DispObj(ho_ImageGrayV, hv_WindowHandleV);
  disp_message(hv_WindowHandleV, "Draw with the mouse the region of interest", "window", -1, 
      -1, "black", "true");
  //*ѡ�����Ȥ������ROI
  DrawRegion(&ho_ROIV, hv_WindowHandleV);
  ReduceDomain(ho_ImageGrayV, ho_ROIV, &ho_ImageReducedV);
  Threshold(ho_ImageReducedV, &ho_RegionsV, 171, 255);
  OrientationRegion(ho_RegionsV, &hv_PhiV);
  AreaCenter(ho_RegionsV, &hv_AreaV, &hv_Row1V, &hv_Column1V);
  VectorAngleToRigid(hv_Row1V, hv_Column1V, hv_PhiV, hv_Row1V, hv_Column1V, 0, &hv_HomMat2D);
  AffineTransImage(ho_ImageReducedV, &ho_ImageAffinTransV, hv_HomMat2D, "constant", 
      "false");
  Threshold(ho_ImageAffinTransV, &ho_Regions1V, 180, 255);
  Connection(ho_Regions1V, &ho_ConnectedRegionsV);
  SelectShape(ho_ConnectedRegionsV, &ho_CirclesV, (HTuple("area").Append("circularity")), 
      "and", (HTuple(3348.62).Append(0)), (HTuple(21238.5).Append(0.56055)));
  CountObj(ho_CirclesV, &hv_NumberV);
  SelectObj(ho_CirclesV, &ho_PillSelectedV, 1);
  DilationCircle(ho_CirclesV, &ho_ROIOuterV, 5);
  ErosionCircle(ho_CirclesV, &ho_ROIInnerV, 3);
  Difference(ho_ROIOuterV, ho_ROIInnerV, &ho_ROI_difV);
  Union1(ho_ROI_difV, &ho_ROIEdgesV);
  ReduceDomain(ho_ImageAffinTransV, ho_ROIEdgesV, &ho_ImageAffinTrans1V);
  EdgesSubPix(ho_ImageAffinTrans1V, &ho_EdgesV, "canny", 1, 65, 120);
  SegmentContoursXld(ho_EdgesV, &ho_ContoursSplitV, "lines_circles", 0.5, 10, 5);
  CountObj(ho_ContoursSplitV, &hv_NumberV);
  GenEmptyObj(&ho_CirclesV);
  hv_NumCircles = 0;
  {
  HTuple end_val56 = hv_NumberV;
  HTuple step_val56 = 1;
  for (hv_v=1; hv_v.Continue(end_val56, step_val56); hv_v += step_val56)
  {
    SelectObj(ho_ContoursSplitV, &ho_ObjectSelectedV, hv_v);
    GetContourGlobalAttribXld(ho_ObjectSelectedV, "cont_approx", &hv_Attrib);
    if (0 != (hv_Attrib==1))
    {
      ConcatObj(ho_CirclesV, ho_ObjectSelectedV, &ho_CirclesV);
      hv_NumCircles += 1;
    }
  }
  }
  if (HDevWindowStack::IsOpen())
    DispObj(ho_CirclesV, hv_WindowHandleV);
  //*�ϲ�����ͬһ��Բ�ܵ�����
  UnionCocircularContoursXld(ho_CirclesV, &ho_UnionContoursV, 0.1, 0.2, 0.1, 5, 10, 
      10, "true", 1);
  if (HDevWindowStack::IsOpen())
    ClearWindow(hv_WindowHandleV);
  if (HDevWindowStack::IsOpen())
    SetColored(hv_WindowHandleV,12);
  if (HDevWindowStack::IsOpen())
    SetLineWidth(hv_WindowHandleV,2);
  if (HDevWindowStack::IsOpen())
    SetPart(hv_WindowHandleV,HTuple(hv_ROI_0[0]).TupleRound(), HTuple(hv_ROI_0[1]).TupleRound(), 
        HTuple(hv_ROI_0[2]).TupleRound(), HTuple(hv_ROI_0[3]).TupleRound());
  if (HDevWindowStack::IsOpen())
    DispObj(ho_ImageAffinTransV, hv_WindowHandleV);
  if (HDevWindowStack::IsOpen())
    DispObj(ho_CirclesV, hv_WindowHandleV);
  hv_Message = "����Բ��������뾶�Ϳ�ȣ�";
  disp_message(hv_WindowHandleV, hv_Message, "window", 20, 12, "black", "true");
  disp_continue_message(hv_WindowHandleV, "black", "true");
 

}



void CVisonDlg::OnClickedResult()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
  hv_AngleJunction = HTuple();
  hv_Row1Junction = HTuple();
  hv_Column1Junction = HTuple();
  hv_PhiJunction = HTuple();
  hv_Phi1Junction = HTuple();
  GenEmptyObj(&ho_lines);
  CountObj(ho_SelectedLines, &hv_i);
  CountObj(ho_SelectedLines, &hv_j);
  for (hv_i=1; hv_i<=14; hv_i+=1)
  {
    GenContourPolygonXld(&ho_Contour, HTuple(hv_RowBegin[hv_i-1]).TupleConcat(HTuple(hv_RowEnd[hv_i-1])), 
        HTuple(hv_ColBegin[hv_i-1]).TupleConcat(HTuple(hv_ColEnd[hv_i-1])));
    ConcatObj(ho_lines, ho_Contour, &ho_lines);
  }
  if (HDevWindowStack::IsOpen())
    SetColor(HDevWindowStack::GetActive(),"magenta");
  if (HDevWindowStack::IsOpen())
    SetLineWidth(HDevWindowStack::GetActive(),3);
  if (HDevWindowStack::IsOpen())
    DispObj(ho_lines, HDevWindowStack::GetActive());
  // stop(); only in hdevelop
  //0
  AngleLl(HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]), HTuple(hv_ColEnd[0]), 
      HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]), HTuple(hv_ColEnd[1]), 
      &hv_Angle);
  hv_Angle = hv_Angle.TupleDeg();
  hv_AngleJunction = hv_AngleJunction.TupleConcat(hv_Angle);
  IntersectionLines(HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]), 
      HTuple(hv_ColEnd[0]), HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]), 
      HTuple(hv_ColEnd[1]), &hv_Row1, &hv_Column1, &hv_IsOverlapping);
  hv_Row1Junction = hv_Row1Junction.TupleConcat(hv_Row1);
  hv_Column1Junction = hv_Column1Junction.TupleConcat(hv_Column1);
  LineOrientation(HTuple(hv_RowBegin[0]), HTuple(hv_ColBegin[0]), HTuple(hv_RowEnd[0]), 
      HTuple(hv_ColEnd[0]), &hv_Phi);
  hv_PhiJunction = hv_PhiJunction.TupleConcat(hv_Phi);
  LineOrientation(HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]), 
      HTuple(hv_ColEnd[1]), &hv_Phi1);
  hv_Phi1Junction = hv_Phi1Junction.TupleConcat(hv_Phi1);
  //1
  AngleLl(HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]), HTuple(hv_ColEnd[3]), 
      HTuple(hv_RowBegin[4]), HTuple(hv_ColBegin[4]), HTuple(hv_RowEnd[4]), HTuple(hv_ColEnd[4]), 
      &hv_Angle);
  hv_Angle = hv_Angle.TupleDeg();
  hv_AngleJunction = hv_AngleJunction.TupleConcat(hv_Angle);
  IntersectionLines(HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]), 
      HTuple(hv_ColEnd[3]), HTuple(hv_RowBegin[4]), HTuple(hv_ColBegin[4]), HTuple(hv_RowEnd[4]), 
      HTuple(hv_ColEnd[4]), &hv_Row1, &hv_Column1, &hv_IsOverlapping);
  hv_Row1Junction = hv_Row1Junction.TupleConcat(hv_Row1);
  hv_Column1Junction = hv_Column1Junction.TupleConcat(hv_Column1);
  LineOrientation(HTuple(hv_RowBegin[3]), HTuple(hv_ColBegin[3]), HTuple(hv_RowEnd[3]), 
      HTuple(hv_ColEnd[3]), &hv_Phi);
  hv_PhiJunction = hv_PhiJunction.TupleConcat(hv_Phi);
  LineOrientation(HTuple(hv_RowBegin[4]), HTuple(hv_ColBegin[4]), HTuple(hv_RowEnd[4]), 
      HTuple(hv_ColEnd[4]), &hv_Phi1);
  hv_Phi1Junction = hv_Phi1Junction.TupleConcat(hv_Phi1);
  //2
  AngleLl(HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]), HTuple(hv_ColEnd[1]), 
      HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]), HTuple(hv_ColEnd[2]), 
      &hv_Angle);
  hv_Angle = hv_Angle.TupleDeg();
  hv_AngleJunction = hv_AngleJunction.TupleConcat(hv_Angle);
  IntersectionLines(HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]), 
      HTuple(hv_ColEnd[1]), HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]), 
      HTuple(hv_ColEnd[2]), &hv_Row1, &hv_Column1, &hv_IsOverlapping);
  hv_Row1Junction = hv_Row1Junction.TupleConcat(hv_Row1);
  hv_Column1Junction = hv_Column1Junction.TupleConcat(hv_Column1);
  LineOrientation(HTuple(hv_RowBegin[2]), HTuple(hv_ColBegin[2]), HTuple(hv_RowEnd[2]), 
      HTuple(hv_ColEnd[2]), &hv_Phi);
  hv_Phi = hv_Phi-(HTuple(180).TupleRad());
  hv_PhiJunction = hv_PhiJunction.TupleConcat(hv_Phi);
  LineOrientation(HTuple(hv_RowBegin[1]), HTuple(hv_ColBegin[1]), HTuple(hv_RowEnd[1]), 
      HTuple(hv_ColEnd[1]), &hv_Phi1);
  hv_Phi1 = hv_Phi1-(HTuple(180).TupleRad());
  hv_Phi1Junction = hv_Phi1Junction.TupleConcat(hv_Phi1);
  //3
  AngleLl(HTuple(hv_RowBegin[4]), HTuple(hv_ColBegin[4]), HTuple(hv_RowEnd[4]), HTuple(hv_ColEnd[4]), 
      HTuple(hv_RowBegin[5]), HTuple(hv_ColBegin[5]), HTuple(hv_RowEnd[5]), HTuple(hv_ColEnd[5]), 
      &hv_Angle);
  hv_Angle = hv_Angle.TupleDeg();
  hv_AngleJunction = hv_AngleJunction.TupleConcat(hv_Angle);
  IntersectionLines(HTuple(hv_RowBegin[4]), HTuple(hv_ColBegin[4]), HTuple(hv_RowEnd[4]), 
      HTuple(hv_ColEnd[4]), HTuple(hv_RowBegin[5]), HTuple(hv_ColBegin[5]), HTuple(hv_RowEnd[5]), 
      HTuple(hv_ColEnd[5]), &hv_Row1, &hv_Column1, &hv_IsOverlapping);
  hv_Row1Junction = hv_Row1Junction.TupleConcat(hv_Row1);
  hv_Column1Junction = hv_Column1Junction.TupleConcat(hv_Column1);
  LineOrientation(HTuple(hv_RowBegin[5]), HTuple(hv_ColBegin[5]), HTuple(hv_RowEnd[5]), 
      HTuple(hv_ColEnd[5]), &hv_Phi);
  hv_Phi = hv_Phi-(HTuple(180).TupleRad());
  hv_PhiJunction = hv_PhiJunction.TupleConcat(hv_Phi);
  LineOrientation(HTuple(hv_RowBegin[4]), HTuple(hv_ColBegin[4]), HTuple(hv_RowEnd[4]), 
      HTuple(hv_ColEnd[4]), &hv_Phi1);
  hv_Phi1 = hv_Phi1-(HTuple(180).TupleRad());
  hv_Phi1Junction = hv_Phi1Junction.TupleConcat(hv_Phi1);
  //4
  AngleLl(HTuple(hv_RowBegin[11]), HTuple(hv_ColBegin[11]), HTuple(hv_RowEnd[11]), 
      HTuple(hv_ColEnd[11]), HTuple(hv_RowBegin[10]), HTuple(hv_ColBegin[10]), HTuple(hv_RowEnd[10]), 
      HTuple(hv_ColEnd[10]), &hv_Angle);
  hv_Angle = hv_Angle.TupleDeg();
  hv_AngleJunction = hv_AngleJunction.TupleConcat(hv_Angle);
  IntersectionLines(HTuple(hv_RowBegin[10]), HTuple(hv_ColBegin[10]), HTuple(hv_RowEnd[10]), 
      HTuple(hv_ColEnd[10]), HTuple(hv_RowBegin[11]), HTuple(hv_ColBegin[11]), HTuple(hv_RowEnd[11]), 
      HTuple(hv_ColEnd[11]), &hv_Row1, &hv_Column1, &hv_IsOverlapping);
  hv_Row1Junction = hv_Row1Junction.TupleConcat(hv_Row1);
  hv_Column1Junction = hv_Column1Junction.TupleConcat(hv_Column1);
  LineOrientation(HTuple(hv_RowBegin[11]), HTuple(hv_ColBegin[11]), HTuple(hv_RowEnd[11]), 
      HTuple(hv_ColEnd[11]), &hv_Phi);
  hv_PhiJunction = hv_PhiJunction.TupleConcat(hv_Phi);
  LineOrientation(HTuple(hv_RowBegin[10]), HTuple(hv_ColBegin[10]), HTuple(hv_RowEnd[10]), 
      HTuple(hv_ColEnd[10]), &hv_Phi1);
  hv_Phi1Junction = hv_Phi1Junction.TupleConcat(hv_Phi1);
  //5
  AngleLl(HTuple(hv_RowBegin[6]), HTuple(hv_ColBegin[6]), HTuple(hv_RowEnd[6]), HTuple(hv_ColEnd[6]), 
      HTuple(hv_RowBegin[7]), HTuple(hv_ColBegin[7]), HTuple(hv_RowEnd[7]), HTuple(hv_ColEnd[7]), 
      &hv_Angle);
  hv_Angle = hv_Angle.TupleDeg();
  hv_AngleJunction = hv_AngleJunction.TupleConcat(hv_Angle);
  IntersectionLines(HTuple(hv_RowBegin[6]), HTuple(hv_ColBegin[6]), HTuple(hv_RowEnd[6]), 
      HTuple(hv_ColEnd[6]), HTuple(hv_RowBegin[7]), HTuple(hv_ColBegin[7]), HTuple(hv_RowEnd[7]), 
      HTuple(hv_ColEnd[7]), &hv_Row1, &hv_Column1, &hv_IsOverlapping);
  hv_Row1Junction = hv_Row1Junction.TupleConcat(hv_Row1);
  hv_Column1Junction = hv_Column1Junction.TupleConcat(hv_Column1);
  LineOrientation(HTuple(hv_RowBegin[6]), HTuple(hv_ColBegin[6]), HTuple(hv_RowEnd[6]), 
      HTuple(hv_ColEnd[6]), &hv_Phi);
  hv_PhiJunction = hv_PhiJunction.TupleConcat(hv_Phi);
  LineOrientation(HTuple(hv_RowBegin[7]), HTuple(hv_ColBegin[7]), HTuple(hv_RowEnd[7]), 
      HTuple(hv_ColEnd[7]), &hv_Phi1);
  hv_Phi1Junction = hv_Phi1Junction.TupleConcat(hv_Phi1);
  //6
  AngleLl(HTuple(hv_RowBegin[8]), HTuple(hv_ColBegin[8]), HTuple(hv_RowEnd[8]), HTuple(hv_ColEnd[8]), 
      HTuple(hv_RowBegin[9]), HTuple(hv_ColBegin[9]), HTuple(hv_RowEnd[9]), HTuple(hv_ColEnd[9]), 
      &hv_Angle);
  hv_Angle = hv_Angle.TupleDeg();
  hv_AngleJunction = hv_AngleJunction.TupleConcat(hv_Angle);
  IntersectionLines(HTuple(hv_RowBegin[8]), HTuple(hv_ColBegin[8]), HTuple(hv_RowEnd[8]), 
      HTuple(hv_ColEnd[8]), HTuple(hv_RowBegin[9]), HTuple(hv_ColBegin[9]), HTuple(hv_RowEnd[9]), 
      HTuple(hv_ColEnd[9]), &hv_Row1, &hv_Column1, &hv_IsOverlapping);
  hv_Row1Junction = hv_Row1Junction.TupleConcat(hv_Row1);
  hv_Column1Junction = hv_Column1Junction.TupleConcat(hv_Column1);
  LineOrientation(HTuple(hv_RowBegin[8]), HTuple(hv_ColBegin[8]), HTuple(hv_RowEnd[8]), 
      HTuple(hv_ColEnd[8]), &hv_Phi);
  hv_Phi = hv_Phi-(HTuple(180).TupleRad());
  hv_PhiJunction = hv_PhiJunction.TupleConcat(hv_Phi);
  LineOrientation(HTuple(hv_RowBegin[9]), HTuple(hv_ColBegin[9]), HTuple(hv_RowEnd[9]), 
      HTuple(hv_ColEnd[9]), &hv_Phi1);
  hv_Phi1Junction = hv_Phi1Junction.TupleConcat(hv_Phi1);
  //7
  AngleLl(HTuple(hv_RowBegin[13]), HTuple(hv_ColBegin[13]), HTuple(hv_RowEnd[13]), 
      HTuple(hv_ColEnd[13]), HTuple(hv_RowBegin[12]), HTuple(hv_ColBegin[12]), HTuple(hv_RowEnd[12]), 
      HTuple(hv_ColEnd[12]), &hv_Angle);
  hv_Angle = hv_Angle.TupleDeg();
  hv_AngleJunction = hv_AngleJunction.TupleConcat(hv_Angle);
  IntersectionLines(HTuple(hv_RowBegin[12]), HTuple(hv_ColBegin[12]), HTuple(hv_RowEnd[12]), 
      HTuple(hv_ColEnd[12]), HTuple(hv_RowBegin[13]), HTuple(hv_ColBegin[13]), HTuple(hv_RowEnd[13]), 
      HTuple(hv_ColEnd[13]), &hv_Row1, &hv_Column1, &hv_IsOverlapping);
  hv_Row1Junction = hv_Row1Junction.TupleConcat(hv_Row1);
  hv_Column1Junction = hv_Column1Junction.TupleConcat(hv_Column1);
  LineOrientation(HTuple(hv_RowBegin[13]), HTuple(hv_ColBegin[13]), HTuple(hv_RowEnd[13]), 
      HTuple(hv_ColEnd[13]), &hv_Phi);
  hv_Phi = hv_Phi-(HTuple(180).TupleRad());
  hv_PhiJunction = hv_PhiJunction.TupleConcat(hv_Phi);
  LineOrientation(HTuple(hv_RowBegin[12]), HTuple(hv_ColBegin[12]), HTuple(hv_RowEnd[12]), 
      HTuple(hv_ColEnd[12]), &hv_Phi1);
  hv_Phi1Junction = hv_Phi1Junction.TupleConcat(hv_Phi1);
  GenEmptyObj(&ho_circles);
  for (hv_s=1; hv_s<=8; hv_s+=1)
  {
    GenCircleContourXld(&ho_ContCircle, HTuple(hv_Row1Junction[hv_s-1]), HTuple(hv_Column1Junction[hv_s-1]), 
        80, HTuple(hv_PhiJunction[hv_s-1]), HTuple(hv_Phi1Junction[hv_s-1]), "negative", 
        1);
    ConcatObj(ho_circles, ho_ContCircle, &ho_circles);
    SetTposition(hv_WindowHandle, HTuple(hv_Row1Junction[hv_s-1]), HTuple(hv_Column1Junction[hv_s-1])-50);
    WriteString(hv_WindowHandle, "A"+hv_s);
  }
  if (HDevWindowStack::IsOpen())
    SetColor(HDevWindowStack::GetActive(),"yellow");
  if (HDevWindowStack::IsOpen())
    SetLineWidth(HDevWindowStack::GetActive(),2);
  if (HDevWindowStack::IsOpen())
    DispObj(ho_circles, HDevWindowStack::GetActive());
  for (hv_s=1; hv_s<=8; hv_s+=2)
  {
    SetTposition(hv_WindowHandle, 580+hv_s *60, 35);
    WriteString(hv_WindowHandle, (((("avg_Angle"+hv_s)+(hv_s+1))+" = ")+(((HTuple(hv_AngleJunction[hv_s-1])+HTuple(hv_AngleJunction[hv_s]))/2).TupleString("5.2f")))+"��");
  }

  //��ֱͼ������ʾ
 
  if (HDevWindowStack::IsOpen())
    SetLineWidth(hv_WindowHandleV,2);
  FitCircleContourXld(ho_UnionContoursV, "geometric", -1, 0, 0, 3, 2, &hv_RowV, &hv_ColumnV, 
      &hv_RadiusV, &hv_StartPhiV, &hv_EndPhiV, &hv_PointOrderV);
  ImagePointsToWorldPlane(hv_CameraParametersV, hv_CameraPoseV, hv_RowV, hv_ColumnV, 
      "mm", &hv_X1V, &hv_Y1V);
  GenCircleContourXld(&ho_ContCircle1V, HTuple(hv_RowV[0]), HTuple(hv_ColumnV[0]), 
      HTuple(hv_RadiusV[0]), 0, 6.28318, "positive", 1);
  if (HDevWindowStack::IsOpen())
  DispObj(ho_ContCircle1V, hv_WindowHandleV);
  SetTposition(hv_WindowHandleV, HTuple(hv_RowV[0])-HTuple(hv_RadiusV[0]), HTuple(hv_ColumnV[0]));
  WriteString(hv_WindowHandleV, "C1");
  SetTposition(hv_WindowHandleV, 700, 310);
  WriteString(hv_WindowHandleV, "C1 = "+(HTuple(hv_RadiusV[0]).TupleString("5.2f")));
  if (HDevWindowStack::IsOpen())
    SetColor(hv_WindowHandleV,"spring green");
  GenCircleContourXld(&ho_ContCircle2V, HTuple(hv_RowV[1]), HTuple(hv_ColumnV[1]), 
      HTuple(hv_RadiusV[1]), 0, 6.28318, "positive", 1);
  if (HDevWindowStack::IsOpen())
  DispObj(ho_ContCircle2V, hv_WindowHandleV);
  SetTposition(hv_WindowHandleV, HTuple(hv_RowV[1])-HTuple(hv_RadiusV[1]), HTuple(hv_ColumnV[1]));
  WriteString(hv_WindowHandleV, "C2");
  SetTposition(hv_WindowHandleV, 750, 310);
  WriteString(hv_WindowHandleV, "C2 = "+(HTuple(hv_RadiusV[1]).TupleString("5.2f")));
  if (HDevWindowStack::IsOpen())
    SetColor(hv_WindowHandleV,"magenta");
  GenCircleContourXld(&ho_ContCircle3V, HTuple(hv_RowV[3]), HTuple(hv_ColumnV[3]), 
      HTuple(hv_RadiusV[3]), 0, 6.28318, "positive", 1);
  if (HDevWindowStack::IsOpen())
  DispObj(ho_ContCircle3V, hv_WindowHandleV);
  SetTposition(hv_WindowHandleV, HTuple(hv_RowV[3])+HTuple(hv_RadiusV[3]), HTuple(hv_ColumnV[3]));
  WriteString(hv_WindowHandleV, "C3");
  SetTposition(hv_WindowHandleV, 800, 310);
  WriteString(hv_WindowHandleV, "C3 = "+(HTuple(hv_RadiusV[3]).TupleString("5.2f")));



}


void CVisonDlg::OnBnClickedyes()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CByteArray Y0_ON;
	CByteArray Y0_OFF;
	m_bitmap.SetBitmap(m_hIcon_ON);
	String2Hex("02 37 30 30 30 35 03 46 46",Y0_ON);//Y0��ON,���ʹ�����
	m_ctrlComm.put_Output(COleVariant(Y0_ON));//����ʮ����������
	Sleep(1000);
	String2Hex("02 38 30 30 30 35 03 30 30",Y0_OFF);//Y0��OFF�����ʹ�ֹͣ
	m_ctrlComm.put_Output(COleVariant(Y0_OFF));//����ʮ����������
}


void CVisonDlg::OnBnClickedno()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CByteArray Y2_ON;
	CByteArray Y2_OFF;
	m_bitmap.SetBitmap(m_hIcon_ON);
	String2Hex("02 37 30 32 30 35 03 30 31",Y2_ON);//Y2��ON,�޳�װ������
	m_ctrlComm.put_Output(COleVariant(Y2_ON));//����ʮ����������
	Sleep(1000);
	String2Hex("02 38 30 32 30 35 03 30 32",Y2_OFF);//Y2��OFF���޳�װ�ø�λ
	m_ctrlComm.put_Output(COleVariant(Y2_OFF));//����ʮ����������
}
