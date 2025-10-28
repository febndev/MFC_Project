
// CanscanDlg.h: 헤더 파일
//

#pragma once
#include "CTabDashboard.h"	// 대시보드 탭 헤더 포함
#include "CTabCam.h"		// 카메라 탭 헤더 포함


// CCanscanDlg 대화 상자
class CCanscanDlg : public CDialogEx
{
// 생성입니다.
public:
	CCanscanDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CANSCAN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CTabCtrl m_tabMain;					// 탭 컨트롤 선언
	CTabDashboard m_tabDashboard;		// 대시보드 탭 선언		
	CTabCam m_tabCam;					// 카메라 탭 선언
	void InitTabControl();				// 탭 컨트롤 초기화 함수
	void ShowTab(int nTab);				// 탭 전환 함수
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult);	// 탭 선택 변경 이벤트
};
