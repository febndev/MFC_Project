#pragma once
#include "CTabDashboard.h"
#include "CTabCam.h"
#include <opencv2/opencv.hpp>

// CCanscanDlg 대화 상자
class CCanscanDlg : public CDialogEx
{
public:
    CCanscanDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CANSCAN_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

    // ===== 메시지 핸들러 =====
protected:
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult);

    // ===== 내부 유틸 =====
private:
    void InitTabControl();
    void ShowTab(int nTab);

public:
    // 프레임 갱신 & Mat → HBITMAP (분리 유지)
    void UpdateFrame(const cv::Mat& frame);
    HBITMAP MatToHBITMAP(const cv::Mat& mat);

private:
    HICON       m_hIcon;
    CTabCtrl    m_tabMain;
    CTabDashboard m_tabDashboard;
    CTabCam       m_tabCam;
};
