
// StereoCaptureDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "StereoCapture.h"
#include "StereoCaptureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const std::vector<std::pair<BMDVideoConnection, CString>> kInputConnections =
{
	{ bmdVideoConnectionSDI,		_T("SDI") },
	{ bmdVideoConnectionHDMI,		_T("HDMI") },
	{ bmdVideoConnectionOpticalSDI,	_T("Optical SDI") },
	{ bmdVideoConnectionComponent,	_T("Component") },
	{ bmdVideoConnectionComposite,	_T("Composite") },
	{ bmdVideoConnectionSVideo,		_T("S-Video") },
};

static void getAncillaryDataFromFrame(CComPtr<IDeckLinkVideoInputFrame>& videoFrame, BMDTimecodeFormat timecodeFormat, CString& timecodeString, CString& userBitsString);
static void getMetadataFromFrame(CComPtr<IDeckLinkVideoInputFrame>& videoFrame, MetadataStruct& metadata);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ABOUT DIALOG
///////////////////////////////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////////////////////////////


// CStereoCaptureDlg dialog
CStereoCaptureDlg::CStereoCaptureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STEREOCAPTURE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStereoCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPUT_DEVICE_COMBO, m_deviceListCombo);
	DDX_Control(pDX, IDC_INPUT_CONNECTION_COMBO, m_inputConnectionCombo);
	DDX_Control(pDX, IDC_AUTODETECT_FORMAT_CHECK, m_applyDetectedInputModeCheckbox);
	DDX_Control(pDX, IDC_INPUT_MODE_COMBO, m_modeListCombo);
	DDX_Control(pDX, IDC_START_STOP_BUTTON, m_startStopButton);
	DDX_Control(pDX, IDC_INVALID_INPUT_STATIC, m_invalidInputLabel);

	/*
	DDX_Control(pDX, IDC_VITC_TC_F1_STATIC, m_vitcTcF1);
	DDX_Control(pDX, IDC_VITC_UB_F1_STATIC, m_vitcUbF1);
	DDX_Control(pDX, IDC_VITC_TC_F2_STATIC, m_vitcTcF2);
	DDX_Control(pDX, IDC_VITC_UB_F2__STATIC, m_vitcUbF2);

	DDX_Control(pDX, IDC_RP188_VITC1_TC_STATIC, m_rp188Vitc1Tc);
	DDX_Control(pDX, IDC_RP188_VITC1_UB_STATIC, m_rp188Vitc1Ub);
	DDX_Control(pDX, IDC_RP188_VITC2_TC_STATIC, m_rp188Vitc2Tc);
	DDX_Control(pDX, IDC_RP188_VITC2_TC__STATIC, m_rp188Vitc2Ub);
	DDX_Control(pDX, IDC_RP188_LTC_TC_STATIC, m_rp188LtcTc);
	DDX_Control(pDX, IDC_RP188_LTC_UB_STATIC, m_rp188LtcUb);
	DDX_Control(pDX, IDC_RP188_HFRTC_TC_STATIC, m_rp188HfrtcTc);
	DDX_Control(pDX, IDC_RP188_HFRTC_UB_STATIC, m_rp188HfrtcUb);

	DDX_Control(pDX, IDC_HDR_EOTF_STATIC, m_hdrEotf);
	DDX_Control(pDX, IDC_HDR_DP_RED_X_STATIC, m_hdrDpRedX);
	DDX_Control(pDX, IDC_HDR_DP_RED_Y_STATIC, m_hdrDpRedY);
	DDX_Control(pDX, IDC_HDR_DP_GREEN_X_STATIC, m_hdrDpGreenX);
	DDX_Control(pDX, IDC_HDR_DP_GREEN_Y_STATIC, m_hdrDpGreenY);
	DDX_Control(pDX, IDC_HDR_DP_BLUE_X_STATIC, m_hdrDpBlueX);
	DDX_Control(pDX, IDC_HDR_DP_BLUE_Y_STATIC, m_hdrDpBlueY);
	DDX_Control(pDX, IDC_HDR_WHITE_POINT_X_STATIC, m_hdrWhitePointX);
	DDX_Control(pDX, IDC_HDR_WHITE_POINT_Y_STATIC, m_hdrWhitePointY);
	DDX_Control(pDX, IDC_HDR_MAX_DML_STATIC, m_hdrMaxDml);
	DDX_Control(pDX, IDC_HDR_MIN_DML_STATIC, m_hdrMinDml);
	DDX_Control(pDX, IDC_HDR_MAX_CLL_STATIC, m_hdrMaxCll);
	DDX_Control(pDX, IDC_HDR_MAX_FALL_STATIC, m_hdrMaxFall);
	DDX_Control(pDX, IDC_COLORSPACE_STATIC, m_colorspace);
	*/
	DDX_Control(pDX, IDC_IMAGE_FRAME_LEFT, m_imgFrameL);
	DDX_Control(pDX, IDC_IMAGE_FRAME_RIGHT, m_imgFrameR);

	DDX_Control(pDX, IDC_STEREO_CHECK, m_stereoCheck);
}

BEGIN_MESSAGE_MAP(CStereoCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()

	// UI element messages
	ON_BN_CLICKED(IDC_START_STOP_BUTTON, &CStereoCaptureDlg::OnStartStopBnClicked)
	ON_CBN_SELCHANGE(IDC_INPUT_DEVICE_COMBO, &CStereoCaptureDlg::OnNewDeviceSelected)
	ON_CBN_SELCHANGE(IDC_INPUT_CONNECTION_COMBO, &CStereoCaptureDlg::OnInputConnectionSelected)
	ON_BN_CLICKED(IDC_AUTODETECT_FORMAT_CHECK, &CStereoCaptureDlg::OnAutoDetectCBClicked)

	// Custom messages
	ON_MESSAGE(WM_REFRESH_INPUT_STREAM_DATA_MESSAGE, &CStereoCaptureDlg::OnRefreshInputStreamData)
	ON_MESSAGE(WM_DETECT_VIDEO_MODE_MESSAGE, &CStereoCaptureDlg::OnDetectVideoMode)
	ON_MESSAGE(WM_ADD_DEVICE_MESSAGE, &CStereoCaptureDlg::OnAddDevice)
	ON_MESSAGE(WM_REMOVE_DEVICE_MESSAGE, &CStereoCaptureDlg::OnRemoveDevice)
	ON_MESSAGE(WM_ERROR_RESTARTING_CAPTURE_MESSAGE, &CStereoCaptureDlg::OnErrorRestartingCapture)
	ON_MESSAGE(WM_UPDATE_PROFILE_MESSAGE, &CStereoCaptureDlg::OnProfileUpdate)

END_MESSAGE_MAP()


// CStereoCaptureDlg message handlers

BOOL CStereoCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Set initial dialog size as minimum size
	CRect rectWindow;
	GetWindowRect(rectWindow);
	m_minDialogSize = rectWindow.Size();

	// Empty popup menus
	m_deviceListCombo.ResetContent();
	m_modeListCombo.ResetContent();

	// Disable the interface
	m_startStopButton.EnableWindow(FALSE);
	EnableInterface(false);

	// Create and initialise preview, profile callback and DeckLink device discovery objects 
	m_imgWinL.Attach(new PreviewWindow());
	if (m_imgWinL->init(&m_imgFrameL) == false)
	{
		ShowErrorMessage(_T("This application was unable to initialise the preview window"), _T("Error"));
		goto bail;
	}

	m_profileCallback.Attach(new ProfileCallback());

	m_profileCallback->onHaltStreams(std::bind(&CStereoCaptureDlg::HaltStreams, this, std::placeholders::_1));
	m_profileCallback->onProfileActivated([this](CComPtr<IDeckLinkProfile>& /* unused */) {
		// Update UI with new profile
		PostMessage(WM_UPDATE_PROFILE_MESSAGE, 0, 0);
		});

	try
	{
		m_deckLinkDiscovery.Attach(new DeckLinkDeviceDiscovery());

		if (!m_deckLinkDiscovery->enable())
			throw std::runtime_error("This application requires the DeckLink drivers installed.\nPlease install the Blackmagic DeckLink drivers to use the features of this application.");
	}
	catch (const std::exception& e)
	{
		CStringW errorString(e.what());
		MessageBox(errorString, _T("Error"));
		return FALSE;
	}

	m_deckLinkDiscovery->onDeviceArrival([this](CComPtr<IDeckLink>& dl) {
		// Update UI (add new device to menu) from main thread
		PostMessage(WM_ADD_DEVICE_MESSAGE, (WPARAM)dl.Detach(), 0);
		});

	m_deckLinkDiscovery->onDeviceRemoval([this](CComPtr<IDeckLink>& dl) {
		// Update UI (remove device from menu) from main thread
		PostMessage(WM_REMOVE_DEVICE_MESSAGE, (WPARAM)dl.Detach(), 0);
		dl.Release();
		});

bail:
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CStereoCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CStereoCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStereoCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Required to ensure minimum size of dialog
void CStereoCaptureDlg::OnGetMinMaxInfo(MINMAXINFO* minMaxInfo)
{
	CDialog::OnGetMinMaxInfo(minMaxInfo);

	minMaxInfo->ptMinTrackSize.x = max(minMaxInfo->ptMinTrackSize.x, m_minDialogSize.cx);
	minMaxInfo->ptMinTrackSize.y = max(minMaxInfo->ptMinTrackSize.y, m_minDialogSize.cy);
}

void getAncillaryDataFromFrame(CComPtr<IDeckLinkVideoInputFrame>& videoFrame, BMDTimecodeFormat timecodeFormat, CString& timecodeString, CString& userBitsString)
{
	CComPtr<IDeckLinkTimecode>		timecode;
	CComBSTR						timecodeBstr;
	BMDTimecodeUserBits				userBits;

	timecodeString = _T("");
	userBitsString = _T("");

	if (videoFrame && (videoFrame->GetTimecode(timecodeFormat, &timecode) == S_OK))
	{
		if (timecode->GetString(&timecodeBstr) == S_OK)
			timecodeString = timecodeBstr;

		if (timecode->GetTimecodeUserBits(&userBits) == S_OK)
			userBitsString.Format(_T("0x%08X"), userBits);
	}
}

void getMetadataFromFrame(CComPtr<IDeckLinkVideoInputFrame>& videoFrame, MetadataStruct& metadata)
{
	metadata.electroOpticalTransferFunction = _T("");
	metadata.displayPrimariesRedX = _T("");
	metadata.displayPrimariesRedY = _T("");
	metadata.displayPrimariesGreenX = _T("");
	metadata.displayPrimariesGreenY = _T("");
	metadata.displayPrimariesBlueX = _T("");
	metadata.displayPrimariesBlueY = _T("");
	metadata.whitePointX = _T("");
	metadata.whitePointY = _T("");
	metadata.maxDisplayMasteringLuminance = _T("");
	metadata.minDisplayMasteringLuminance = _T("");
	metadata.maximumContentLightLevel = _T("");
	metadata.maximumFrameAverageLightLevel = _T("");
	metadata.colorspace = _T("");

	CComQIPtr<IDeckLinkVideoFrameMetadataExtensions> metadataExtensions(videoFrame);

	if (metadataExtensions)
	{
		double doubleValue = 0.0;
		int64_t intValue = 0;

		if (metadataExtensions->GetInt(bmdDeckLinkFrameMetadataHDRElectroOpticalTransferFunc, &intValue) == S_OK)
		{
			switch (intValue)
			{
			case 0:
				metadata.electroOpticalTransferFunction = _T("SDR");
				break;
			case 1:
				metadata.electroOpticalTransferFunction = _T("HDR");
				break;
			case 2:
				metadata.electroOpticalTransferFunction = _T("PQ (ST2084)");
				break;
			case 3:
				metadata.electroOpticalTransferFunction = _T("HLG");
				break;
			default:
				metadata.electroOpticalTransferFunction.Format(_T("Unknown EOTF: %d"), (int32_t)intValue);
				break;
			}
		}

		if (videoFrame->GetFlags() & bmdFrameContainsHDRMetadata)
		{
			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRDisplayPrimariesRedX, &doubleValue) == S_OK)
				metadata.displayPrimariesRedX.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRDisplayPrimariesRedY, &doubleValue) == S_OK)
				metadata.displayPrimariesRedY.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRDisplayPrimariesGreenX, &doubleValue) == S_OK)
				metadata.displayPrimariesGreenX.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRDisplayPrimariesGreenY, &doubleValue) == S_OK)
				metadata.displayPrimariesGreenY.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRDisplayPrimariesBlueX, &doubleValue) == S_OK)
				metadata.displayPrimariesBlueX.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRDisplayPrimariesBlueY, &doubleValue) == S_OK)
				metadata.displayPrimariesBlueY.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRWhitePointX, &doubleValue) == S_OK)
				metadata.whitePointX.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRWhitePointY, &doubleValue) == S_OK)
				metadata.whitePointY.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRMaxDisplayMasteringLuminance, &doubleValue) == S_OK)
				metadata.maxDisplayMasteringLuminance.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRMinDisplayMasteringLuminance, &doubleValue) == S_OK)
				metadata.minDisplayMasteringLuminance.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRMaximumContentLightLevel, &doubleValue) == S_OK)
				metadata.maximumContentLightLevel.Format(_T("%.04f"), doubleValue);

			if (metadataExtensions->GetFloat(bmdDeckLinkFrameMetadataHDRMaximumFrameAverageLightLevel, &doubleValue) == S_OK)
				metadata.maximumFrameAverageLightLevel.Format(_T("%.04f"), doubleValue);
		}

		if (metadataExtensions->GetInt(bmdDeckLinkFrameMetadataColorspace, &intValue) == S_OK)
		{
			switch (intValue)
			{
			case bmdColorspaceRec601:
				metadata.colorspace = _T("Rec.601");
				break;
			case bmdColorspaceRec709:
				metadata.colorspace = _T("Rec.709");
				break;
			case bmdColorspaceRec2020:
				metadata.colorspace = _T("Rec.2020");
				break;
			}
		}
	}
}


void CStereoCaptureDlg::OnStartStopBnClicked()
{
	if (m_selectedDevice == NULL)
		return;

	if (m_selectedDevice->isCapturing())
		StopCapture();
	else
		StartCapture();
}

void CStereoCaptureDlg::OnNewDeviceSelected()
{
	int			selectedDeviceIndex;

	m_selectedDevice = nullptr;

	selectedDeviceIndex = m_deviceListCombo.GetCurSel();
	if (selectedDeviceIndex < 0)
		return;

	if (m_selectedDevice)
	{
		// Unsubscribe to input device callbacks
		m_selectedDevice->setErrorListener(nullptr);
		m_selectedDevice->onVideoFormatChange(nullptr);
		m_selectedDevice->onVideoFrameArrival(nullptr);

		m_selectedDevice.Release();
	}

	// Find input device based on IDeckLink* object
	auto iter = m_inputDevices.find((IDeckLink*)m_deviceListCombo.GetItemDataPtr(selectedDeviceIndex));
	if (iter == m_inputDevices.end())
		return;

	m_selectedDevice = iter->second;

	if (m_selectedDevice)
	{
		// Subscribe to input device callbacks
		m_selectedDevice->setErrorListener(std::bind(&CStereoCaptureDlg::HandleDeviceError, this, std::placeholders::_1));
		m_selectedDevice->onVideoFormatChange([this](BMDDisplayMode displayMode) { PostMessage(WM_DETECT_VIDEO_MODE_MESSAGE, 0, (LPARAM)displayMode); });
		m_selectedDevice->onVideoFrameArrival(std::bind(&CStereoCaptureDlg::VideoFrameArrived, this, std::placeholders::_1));

		// Update the input video connections combo
		RefreshInputConnectionList();

		EnableInterface(true);
	}
}


void CStereoCaptureDlg::OnInputConnectionSelected()
{
	int selectedConnectionIndex;

	selectedConnectionIndex = m_inputConnectionCombo.GetCurSel();
	if (selectedConnectionIndex < 0)
		return;

	m_selectedInputConnection = (BMDVideoConnection)m_inputConnectionCombo.GetItemData(selectedConnectionIndex);

	// Configure input connection for selected device
	if (m_selectedDevice->getDeckLinkConfiguration()->SetInt(bmdDeckLinkConfigVideoInputConnection, (int64_t)m_selectedInputConnection) != S_OK)
		return;

	// Updated video mode combo for selected input connection
	RefreshVideoModeList();
}

void CStereoCaptureDlg::OnAutoDetectCBClicked()
{
	m_modeListCombo.EnableWindow(m_selectedDevice && (m_applyDetectedInputModeCheckbox.GetCheck() == BST_UNCHECKED) ? TRUE : FALSE);
}

void CStereoCaptureDlg::OnClose()
{
	// Stop the capture
	StopCapture();

	// Disable profile callback
	if (m_selectedDevice)
	{
		m_selectedDevice.Release();
	}

	// Release all DeckLinkDevice instances
	for (auto& device : m_inputDevices)
	{
		CComQIPtr<IDeckLinkProfileManager> profileManager(device.second->getDeckLinkInstance());
		if (profileManager)
			profileManager->SetCallback(nullptr);

		device.second.Release();
	}

	// Release profile callback
	m_profileCallback.Release();

	// Release preview window
	m_imgWinL.Release();

	// Release DeckLink discovery instance
	m_deckLinkDiscovery->disable();
	m_deckLinkDiscovery.Release();

	CDialog::OnClose();
}


void CStereoCaptureDlg::ShowErrorMessage(TCHAR* msg, TCHAR* title)
{
	MessageBox(msg, title);
}

void CStereoCaptureDlg::RefreshInputDeviceList()
{
	int index;
	BOOL hasActiveInputDevices;

	m_deviceListCombo.ResetContent();

	for (auto& device : m_inputDevices)
	{
		CComQIPtr<IDeckLinkProfileAttributes> deckLinkAttributes(device.second->getDeckLinkInstance());

		if (deckLinkAttributes)
		{
			int64_t intAttribute;
			if ((deckLinkAttributes->GetInt(BMDDeckLinkDuplex, &intAttribute) == S_OK) &&
				(((BMDDuplexMode)intAttribute) != bmdDuplexInactive))
			{
				// Input device is active, add to combobox
				index = m_deviceListCombo.AddString(device.second->getDeviceName());
				m_deviceListCombo.SetItemDataPtr(index, (void*)device.first);

				// Retain selected device even if combo box position has changed
				if (device.second == m_selectedDevice)
					m_deviceListCombo.SetCurSel(index);
			}
		}
	}

	hasActiveInputDevices = m_deviceListCombo.GetCount() > 0;

	// If there is at least 1 active device, enable start/stop button
	m_startStopButton.EnableWindow(hasActiveInputDevices);

	if (hasActiveInputDevices)
	{
		// If device has been removed or becomes inactive due to profile change, then select first device in combobox
		index = m_deviceListCombo.GetCurSel();
		if (index == CB_ERR)
		{
			m_deviceListCombo.SetCurSel(0);
			OnNewDeviceSelected();
		}
	}
	else
		m_selectedDevice = nullptr;

	// If a device is selected and not capturing then enable interface
	EnableInterface(m_selectedDevice && !m_selectedDevice->isCapturing());
}

void CStereoCaptureDlg::RefreshInputConnectionList()
{
	CComQIPtr<IDeckLinkProfileAttributes>	deckLinkAttributes(m_selectedDevice->getDeckLinkInstance());
	LONGLONG								availableInputConnections;
	LONGLONG								currentInputConnection;
	int										index;

	m_inputConnectionCombo.ResetContent();

	if (!deckLinkAttributes)
		return;

	// Get the available input video connections for the device
	if (deckLinkAttributes->GetInt(BMDDeckLinkVideoInputConnections, &availableInputConnections) != S_OK)
		availableInputConnections = bmdVideoConnectionUnspecified;

	// Get the current selected input connection
	if (m_selectedDevice->getDeckLinkConfiguration()->GetInt(bmdDeckLinkConfigVideoInputConnection, &currentInputConnection) != S_OK)
	{
		currentInputConnection = bmdVideoConnectionUnspecified;
	}

	for (auto connection : kInputConnections)
	{
		if ((connection.first & (BMDVideoConnection)availableInputConnections) != 0)
		{
			// Input video connection is supported by device, add to combo
			index = m_inputConnectionCombo.AddString(connection.second);
			m_inputConnectionCombo.SetItemData(index, connection.first);

			// If input connection is the active connection set combo to this item
			if (connection.first == (BMDVideoConnection)currentInputConnection)
			{
				m_inputConnectionCombo.SetCurSel(index);
				OnInputConnectionSelected();
			}
		}
	}

	// If no input connection has been selected, select first index
	index = m_inputConnectionCombo.GetCurSel();
	if ((index == CB_ERR) && (m_inputConnectionCombo.GetCount() > 0))
	{
		m_inputConnectionCombo.SetCurSel(0);
		OnInputConnectionSelected();
	}
}

void CStereoCaptureDlg::RefreshVideoModeList()
{
	m_modeListCombo.ResetContent();

	m_selectedDevice->queryDisplayModes([this](CComPtr<IDeckLinkDisplayMode>& deckLinkDisplayMode)
		{
			CComBSTR	modeName;
			int			newIndex;
			BOOL		supported;

			// Check that display mode is supported with the active profile


			// TODO: change video mode to bmdSupportedVideoModeDualStream3D
			// first add a stereo checkbox to the UI and read it's status here






			if ((m_selectedDevice->getDeckLinkInput()->DoesSupportVideoMode(m_selectedInputConnection, deckLinkDisplayMode->GetDisplayMode(), bmdFormatUnspecified,
				bmdNoVideoInputConversion, bmdSupportedVideoModeDefault, nullptr, &supported) != S_OK) ||
				!supported)
			{
				return;
			}

			if (deckLinkDisplayMode->GetName(&modeName) != S_OK)
				return;

			// Add this item to the video format popup menu
			newIndex = m_modeListCombo.AddString(modeName);

			// Save the BMDDisplayMode in the menu item's tag
			m_modeListCombo.SetItemData(newIndex, deckLinkDisplayMode->GetDisplayMode());
		});

	if (m_modeListCombo.GetCount() > 0)
	{
		// Select first item in list
		m_modeListCombo.SetCurSel(0);
	}

	m_startStopButton.EnableWindow(m_modeListCombo.GetCount() > 0);
}

void CStereoCaptureDlg::StartCapture()
{
	bool	applyDetectedInputMode = (m_applyDetectedInputModeCheckbox.GetCheck() == BST_CHECKED);
	int		selectedVideoFormatIndex = m_modeListCombo.GetCurSel();

	if (selectedVideoFormatIndex < 0)
		return;

	if (m_selectedDevice)
	{
		if (m_selectedDevice->startCapture((BMDDisplayMode)m_modeListCombo.GetItemData(selectedVideoFormatIndex), m_imgWinL, applyDetectedInputMode))
		{
			// Update UI
			m_startStopButton.SetWindowText(_T("Stop capture"));
			EnableInterface(false);
		}
		else
		{
			m_selectedDevice->stopCapture();
		}
	}
}

void CStereoCaptureDlg::StopCapture()
{
	if (m_selectedDevice)
		m_selectedDevice->stopCapture();

	// Update UI
	m_startStopButton.SetWindowText(_T("Start capture"));
	EnableInterface(true);
	m_invalidInputLabel.ShowWindow(SW_HIDE);
}

void CStereoCaptureDlg::EnableInterface(bool enabled)
{
	m_deviceListCombo.EnableWindow((enabled) ? TRUE : FALSE);
	m_inputConnectionCombo.EnableWindow((enabled) ? TRUE : FALSE);

	if (enabled)
	{
		if (m_selectedDevice && m_selectedDevice->doesSupportFormatDetection())
		{
			m_applyDetectedInputModeCheckbox.EnableWindow(TRUE);
			m_applyDetectedInputModeCheckbox.SetCheck(BST_CHECKED);
			m_modeListCombo.EnableWindow(FALSE);
		}
		else
		{
			m_applyDetectedInputModeCheckbox.EnableWindow(FALSE);
			m_applyDetectedInputModeCheckbox.SetCheck(BST_UNCHECKED);
			m_modeListCombo.EnableWindow(TRUE);
		}
	}
	else
	{
		m_applyDetectedInputModeCheckbox.EnableWindow(FALSE);
		m_modeListCombo.EnableWindow(FALSE);
	}
}

void CStereoCaptureDlg::VideoFrameArrived(CComPtr<IDeckLinkVideoInputFrame>& videoFrame)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		// Get the various timecodes and userbits attached to this frame
		getAncillaryDataFromFrame(videoFrame, bmdTimecodeVITC, m_ancillaryData.vitcF1Timecode, m_ancillaryData.vitcF1UserBits);
		getAncillaryDataFromFrame(videoFrame, bmdTimecodeVITCField2, m_ancillaryData.vitcF2Timecode, m_ancillaryData.vitcF2UserBits);
		getAncillaryDataFromFrame(videoFrame, bmdTimecodeRP188VITC1, m_ancillaryData.rp188vitc1Timecode, m_ancillaryData.rp188vitc1UserBits);
		getAncillaryDataFromFrame(videoFrame, bmdTimecodeRP188LTC, m_ancillaryData.rp188ltcTimecode, m_ancillaryData.rp188ltcUserBits);
		getAncillaryDataFromFrame(videoFrame, bmdTimecodeRP188VITC2, m_ancillaryData.rp188vitc2Timecode, m_ancillaryData.rp188vitc2UserBits);
		getAncillaryDataFromFrame(videoFrame, bmdTimecodeRP188HighFrameRate, m_ancillaryData.rp188hfrtcTimecode, m_ancillaryData.rp188hfrtcUserBits);
		getMetadataFromFrame(videoFrame, m_metadata);
	}

	// Update the UI
	PostMessage(WM_REFRESH_INPUT_STREAM_DATA_MESSAGE, (videoFrame->GetFlags() & bmdFrameHasNoInputSource), 0);
}

void CStereoCaptureDlg::HandleDeviceError(DeviceError error)
{
	switch (error)
	{
	case DeviceError::EnableVideoInputFailed:
		ShowErrorMessage(_T("This application was unable to select the chosen video mode. Perhaps, the selected device is currently in-use."), _T("Error starting the capture"));
		break;

	case DeviceError::StartStreamsFailed:
		ShowErrorMessage(_T("This application was unable to start the capture. Perhaps, the selected device is currently in-use."), _T("Error starting the capture"));
		break;

	case DeviceError::ReenableVideoInputFailed:
		PostMessage(WM_ERROR_RESTARTING_CAPTURE_MESSAGE, 0, 0);
		break;
	}
}

void CStereoCaptureDlg::AddDevice(CComPtr<IDeckLink>& deckLink)
{
	CComPtr<DeckLinkDevice>		newDevice;

	try
	{
		newDevice.Attach(new DeckLinkDevice(deckLink));
	}
	catch (...)
	{
		// Device does not have IDeckLinkInput interface, eg it is a DeckLink Mini Monitor
		return;
	}

	// Initialise new DeckLinkDevice object
	if (!newDevice->init())
		return;

	// Register profile callback with newly added device's profile manager
	CComQIPtr<IDeckLinkProfileManager> profileManager(newDevice->getDeckLinkInstance());
	if (profileManager)
		profileManager->SetCallback(m_profileCallback);

	// Store input device to map to maintain reference
	m_inputDevices[deckLink] = std::move(newDevice);
	RefreshInputDeviceList();
}

void CStereoCaptureDlg::RemoveDevice(CComPtr<IDeckLink>& deckLink)
{
	// Remove input device from list 
	auto iter = m_inputDevices.find(deckLink);
	if (iter != m_inputDevices.end())
	{
		CComPtr<DeckLinkDevice> deviceToRemove = iter->second;
		CComQIPtr<IDeckLinkProfileManager> profileManager(deviceToRemove->getDeckLinkInstance());

		if (deviceToRemove->isCapturing())
			deviceToRemove->stopCapture();

		// Release profile callback from device to remove
		if (profileManager)
			profileManager->SetCallback(nullptr);

		// Release DeckLinkDevice instance
		deviceToRemove.Release();

		m_inputDevices.erase(iter);

		// Update input device combo box
		RefreshInputDeviceList();
	}
}

void    CStereoCaptureDlg::HaltStreams(CComPtr<IDeckLinkProfile>& newProfile)
{
	CComPtr<IDeckLink> deckLink;

	// Profile is changing, stop capture if running
	if ((newProfile->GetDevice(&deckLink) == S_OK) &&
		(m_selectedDevice->getDeckLinkInstance() == deckLink) &&
		(m_selectedDevice->isCapturing()))
	{
		StopCapture();
	}
}

LRESULT  CStereoCaptureDlg::OnRefreshInputStreamData(WPARAM wParam, LPARAM lParam)
{
	{
		// Update the UI under protection of mutex lock
		std::lock_guard<std::mutex> lock(m_mutex);

		/*
		m_vitcTcF1.SetWindowText(m_ancillaryData.vitcF1Timecode);
		m_vitcUbF1.SetWindowText(m_ancillaryData.vitcF1UserBits);
		m_vitcTcF2.SetWindowText(m_ancillaryData.vitcF2Timecode);
		m_vitcUbF2.SetWindowText(m_ancillaryData.vitcF2UserBits);

		m_rp188Vitc1Tc.SetWindowText(m_ancillaryData.rp188vitc1Timecode);
		m_rp188Vitc1Ub.SetWindowText(m_ancillaryData.rp188vitc1UserBits);
		m_rp188Vitc2Tc.SetWindowText(m_ancillaryData.rp188vitc2Timecode);
		m_rp188Vitc2Ub.SetWindowText(m_ancillaryData.rp188vitc2UserBits);
		m_rp188LtcTc.SetWindowText(m_ancillaryData.rp188ltcTimecode);
		m_rp188LtcUb.SetWindowText(m_ancillaryData.rp188ltcUserBits);
		m_rp188HfrtcTc.SetWindowText(m_ancillaryData.rp188hfrtcTimecode);
		m_rp188HfrtcUb.SetWindowText(m_ancillaryData.rp188hfrtcUserBits);

		m_hdrEotf.SetWindowText(m_metadata.electroOpticalTransferFunction);
		m_hdrDpRedX.SetWindowText(m_metadata.displayPrimariesRedX);
		m_hdrDpRedY.SetWindowText(m_metadata.displayPrimariesRedY);
		m_hdrDpGreenX.SetWindowText(m_metadata.displayPrimariesGreenX);
		m_hdrDpGreenY.SetWindowText(m_metadata.displayPrimariesGreenY);
		m_hdrDpBlueX.SetWindowText(m_metadata.displayPrimariesBlueX);
		m_hdrDpBlueY.SetWindowText(m_metadata.displayPrimariesBlueY);
		m_hdrWhitePointX.SetWindowText(m_metadata.whitePointX);
		m_hdrWhitePointY.SetWindowText(m_metadata.whitePointY);
		m_hdrMaxDml.SetWindowText(m_metadata.maxDisplayMasteringLuminance);
		m_hdrMinDml.SetWindowText(m_metadata.minDisplayMasteringLuminance);
		m_hdrMaxCll.SetWindowText(m_metadata.maximumContentLightLevel);
		m_hdrMaxFall.SetWindowText(m_metadata.maximumFrameAverageLightLevel);
		m_colorspace.SetWindowText(m_metadata.colorspace);
		*/
	}

	m_invalidInputLabel.ShowWindow((wParam) ? SW_SHOW : SW_HIDE);

	return 0;
}

LRESULT CStereoCaptureDlg::OnDetectVideoMode(WPARAM wParam, LPARAM lParam)
{
	BMDDisplayMode	detectedVideoMode;
	int				modeIndex;

	// A new video mode was auto-detected, update the video mode combo box
	detectedVideoMode = (BMDDisplayMode)lParam;

	for (modeIndex = 0; modeIndex < m_modeListCombo.GetCount(); modeIndex++)
	{
		if (m_modeListCombo.GetItemData(modeIndex) == detectedVideoMode)
		{
			m_modeListCombo.SetCurSel(modeIndex);
			break;
		}
	}

	return 0;
}


LRESULT CStereoCaptureDlg::OnAddDevice(WPARAM wParam, LPARAM lParam)
{
	// A new device has been connected
	CComPtr<IDeckLink> deckLink;
	deckLink.Attach((IDeckLink*)wParam);
	AddDevice(deckLink);
	return 0;
}

LRESULT	CStereoCaptureDlg::OnRemoveDevice(WPARAM wParam, LPARAM lParam)
{
	// An existing device has been disconnected
	CComPtr<IDeckLink> deckLink;
	deckLink.Attach((IDeckLink*)wParam);
	RemoveDevice(deckLink);
	return 0;
}

LRESULT	CStereoCaptureDlg::OnErrorRestartingCapture(WPARAM wParam, LPARAM lParam)
{
	// A change in the input video mode was detected, but the capture could not be restarted.
	StopCapture();
	ShowErrorMessage(_T("This application was unable to apply the detected input video mode."), _T("Error restarting the capture."));
	return 0;
}

LRESULT CStereoCaptureDlg::OnProfileUpdate(WPARAM wParam, LPARAM lParam)
{
	// Check whether device is active/inactive and update input device combobox, 
	// following the same steps as if the device was added/removed. 
	RefreshInputDeviceList();
	return 0;
}
