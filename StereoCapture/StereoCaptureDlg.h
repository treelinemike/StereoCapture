
// StereoCaptureDlg.h : header file
//

#pragma once

#include <map>
#include <mutex>
#include "resource.h"
#include "DeckLinkAPI_h.h"

#include "DeckLinkDeviceDiscovery.h"
#include "DeckLinkDevice.h"
#include "PreviewWindow.h"
#include "ProfileCallback.h"

// Custom messages
#define WM_REFRESH_INPUT_STREAM_DATA_MESSAGE	(WM_APP + 1)
#define WM_DETECT_VIDEO_MODE_MESSAGE			(WM_APP + 2)
#define WM_ADD_DEVICE_MESSAGE					(WM_APP + 3)
#define WM_REMOVE_DEVICE_MESSAGE				(WM_APP + 4)
#define WM_ERROR_RESTARTING_CAPTURE_MESSAGE		(WM_APP + 5)
#define WM_UPDATE_PROFILE_MESSAGE				(WM_APP + 6)

typedef struct {
	// VITC timecodes and user bits for field 1 & 2
	CString	vitcF1Timecode;
	CString	vitcF1UserBits;
	CString	vitcF2Timecode;
	CString	vitcF2UserBits;

	// RP188 timecodes and user bits (VITC1, VITC2, LTC and HFRTC)
	CString	rp188vitc1Timecode;
	CString	rp188vitc1UserBits;
	CString	rp188vitc2Timecode;
	CString	rp188vitc2UserBits;
	CString	rp188ltcTimecode;
	CString	rp188ltcUserBits;
	CString	rp188hfrtcTimecode;
	CString	rp188hfrtcUserBits;
} AncillaryDataStruct;

typedef struct {
	CString electroOpticalTransferFunction;
	CString displayPrimariesRedX;
	CString displayPrimariesRedY;
	CString displayPrimariesGreenX;
	CString displayPrimariesGreenY;
	CString displayPrimariesBlueX;
	CString displayPrimariesBlueY;
	CString whitePointX;
	CString whitePointY;
	CString maxDisplayMasteringLuminance;
	CString minDisplayMasteringLuminance;
	CString maximumContentLightLevel;
	CString maximumFrameAverageLightLevel;
	CString colorspace;
} MetadataStruct;

// CStereoCaptureDlg dialog
class CStereoCaptureDlg : public CDialogEx
{
// Construction
public:
	CStereoCaptureDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STEREOCAPTURE_DIALOG };
#endif

	// UI-related handlers
	afx_msg void			OnNewDeviceSelected();
	afx_msg void			OnInputConnectionSelected();
	afx_msg void			OnStartStopBnClicked();
	afx_msg void			OnAutoDetectCBClicked();
	afx_msg void			OnClose();

	// Custom message handlers
	afx_msg LRESULT			OnRefreshInputStreamData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT			OnDetectVideoMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT			OnAddDevice(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT			OnRemoveDevice(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT			OnErrorRestartingCapture(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT			OnProfileUpdate(WPARAM wParam, LPARAM lParam);

	// DeckLinkDevice delegate methods
	void					ShowErrorMessage(TCHAR* msg, TCHAR* title);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	
	// Internal helper methods
	void					EnableInterface(bool enabled);
	void					RefreshInputDeviceList();
	void					RefreshInputConnectionList();
	void					RefreshVideoModeList();
	void					StartCapture();
	void					StopCapture();
	void					AddDevice(CComPtr<IDeckLink>& deckLink);
	void					RemoveDevice(CComPtr<IDeckLink>& deckLink);
	void					VideoFrameArrived(CComPtr<IDeckLinkVideoInputFrame>& videoFrame);
	void					HaltStreams(CComPtr<IDeckLinkProfile>& newProfile);
	void					HandleDeviceError(DeviceError error);

	// UI elements
	CComboBox				m_deviceListCombo;
	CComboBox				m_inputConnectionCombo;
	CButton					m_applyDetectedInputModeCheckbox;
	CComboBox				m_modeListCombo;
	CButton					m_startStopButton;
	CStatic					m_invalidInputLabel;

	/*
	CStatic					m_vitcTcF1;
	CStatic					m_vitcUbF1;
	CStatic					m_vitcTcF2;
	CStatic					m_vitcUbF2;

	CStatic					m_rp188Vitc1Tc;
	CStatic					m_rp188Vitc1Ub;
	CStatic					m_rp188Vitc2Tc;
	CStatic					m_rp188Vitc2Ub;
	CStatic					m_rp188LtcTc;
	CStatic					m_rp188LtcUb;
	CStatic					m_rp188HfrtcTc;
	CStatic					m_rp188HfrtcUb;

	CStatic					m_hdrEotf;
	CStatic					m_hdrDpRedX;
	CStatic					m_hdrDpRedY;
	CStatic					m_hdrDpGreenX;
	CStatic					m_hdrDpGreenY;
	CStatic					m_hdrDpBlueX;
	CStatic					m_hdrDpBlueY;
	CStatic					m_hdrWhitePointX;
	CStatic					m_hdrWhitePointY;
	CStatic					m_hdrMaxDml;
	CStatic					m_hdrMinDml;
	CStatic					m_hdrMaxCll;
	CStatic					m_hdrMaxFall;
	CStatic					m_colorspace;
	*/

	CStatic					m_imgFrameL;
	CStatic					m_imgFrameR;
	CComPtr<PreviewWindow>	m_imgWinL;
	CComPtr<PreviewWindow>	m_imgWinR;

	CSize					m_minDialogSize;

	//
	AncillaryDataStruct					m_ancillaryData;
	MetadataStruct						m_metadata;
	std::mutex							m_mutex; // to synchronise access to the above structures
	CComPtr<DeckLinkDevice>				m_selectedDevice;
	BMDVideoConnection					m_selectedInputConnection;
	CComPtr<DeckLinkDeviceDiscovery>	m_deckLinkDiscovery;
	CComPtr<ProfileCallback>			m_profileCallback;

	std::map<IDeckLink*, CComPtr<DeckLinkDevice>>		m_inputDevices;
	
	HICON m_hIcon;
	
	// Generated message map functions
	virtual BOOL			OnInitDialog();
	afx_msg void			OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void			OnPaint();
	afx_msg HCURSOR			OnQueryDragIcon();
	afx_msg void			OnGetMinMaxInfo(MINMAXINFO* minMaxInfo);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnStnClickedPreviewBox();
};
