#pragma once
#include "afxdialogex.h"


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
};
