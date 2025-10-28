#pragma once
#include "afxdialogex.h"
#include "afxcmn.h"  // CListCtrl을 사용하기 위한 헤더

// CTabCam 대화 상자

class CTabCam : public CDialogEx
{
	DECLARE_DYNAMIC(CTabCam)

public:
	CTabCam(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabCam();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_CAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog(); // 대화 상자 초기화
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnSerch();
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnConnect();
	
	CListCtrl m_ListCam;  // 카메라 목록 컨트롤
};
