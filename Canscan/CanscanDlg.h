#pragma once
#include "CTabDashboard.h"
#include "CTabCam.h"
#include "Packet.h"
#include <opencv2/opencv.hpp>

// CCanscanDlg 대화 상자
class CCanscanDlg : public CDialogEx
{
public:
    CCanscanDlg(CWnd* pParent = nullptr);
    void OnDestroy();

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
    // 네트워크
    bool IsConnected() const { return m_isConnected; }
    bool m_isConnected = false;
    Packet& GetPacket() { return m_packet; }
    // Camera 탭 접근용 getter
    CTabCam& GetTabCam() { return m_tabCam; }

    std::vector<uint8_t> m_lastSentImage;

    void SetWaitingForImgRes(bool flag) { m_waitingForImgRes = flag; }
    bool IsWaitingForImgRes() const { return m_waitingForImgRes; }

private:
    Packet m_packet;
    HICON       m_hIcon;
    CTabCtrl    m_tabMain;
    CTabDashboard m_tabDashboard;
    CTabCam       m_tabCam;
    bool m_waitingForImgRes = false;
};
