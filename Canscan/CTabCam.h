#pragma once
#include "afxdialogex.h"
#include "afxcmn.h"
#include "CameraManager.h"
#include <opencv2/opencv.hpp>

class CCanscanDlg; // forward

class CTabCam : public CDialogEx
{
    DECLARE_DYNAMIC(CTabCam)

public:
    CTabCam(CWnd* pParent = nullptr);
    virtual ~CTabCam();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TAB_CAM };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedBtnSerch();
    afx_msg void OnBnClickedBtnOpen();
    afx_msg void OnBnClickedBtnClose();
    afx_msg void OnBnClickedBtnConnect();
    afx_msg void OnListSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    CListCtrl m_ListCam;

private:
    CameraManager m_camera;
    cv::Mat       m_frame;
    CCanscanDlg* m_pParentDlg = nullptr;

    void UpdateListRow(int row, const wchar_t* status);
    void SetTextToControl(UINT id, const CString& text);

    // ★ FPS 계산용 멤버
    ULONGLONG m_lastFpsTick = 0;
    int       m_frameCount = 0;
    int       m_currentFps = 0;

    // ★ 상태 라벨 갱신 헬퍼
    void UpdateInfoLabel(bool connected);
    void ResetFpsCounters();

public:
    const cv::Mat& GetFrame() const { return m_frame; }
};
