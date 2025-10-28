// CTabDashboard.cpp: 구현 파일
//

#include "pch.h"
#include "Canscan.h"
#include "afxdialogex.h"
#include "CTabDashboard.h"


// CTabDashboard 대화 상자

IMPLEMENT_DYNAMIC(CTabDashboard, CDialogEx)

CTabDashboard::CTabDashboard(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_DASHBOARD, pParent)
{

}

CTabDashboard::~CTabDashboard()
{
}

void CTabDashboard::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabDashboard, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CTabDashboard::OnBnClickedBtnInspection)
END_MESSAGE_MAP()


// CTabDashboard 메시지 처리기

void CTabDashboard::OnBnClickedBtnInspection()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
