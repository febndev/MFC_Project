#pragma once
#include "afxdialogex.h"

// 사용자 정의 메시지: 검사 결과 텍스트 갱신
#define WM_UPDATE_RESULT_TEXT (WM_APP + 101)

// CTabDashboard 대화 상자

class CTabDashboard : public CDialogEx
{
	DECLARE_DYNAMIC(CTabDashboard)

public:
	CTabDashboard(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabDashboard();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_DASHBOARD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnInspection();
	afx_msg LRESULT OnUpdateResultText(WPARAM wParam, LPARAM lParam); // 사용자 정의 메시지 핸들러

	// 결과 집계용 카운터
	int m_totalCount = 0;  // IDC_TOTAL
	int m_goodCount = 0;  // IDC_GOOD
	int m_defectCount = 0;  // IDC_DEFECT

	// 카운트 값들을 컨트롤에 반영
	void UpdateCountersUI();

	// 1-1) 현재 결과 코드 저장용
	int   m_resultCode = -1;   // 0=PASS, 1=FAIL, 기타=UNKNOWN

	// 1-2) 결과 표시용 큰 폰트와 배경 브러시
	CFont m_fontResult;
	CBrush m_brushResultBG;

	// 1-3) 초기화/색상 변경 핸들러 선언
protected:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
