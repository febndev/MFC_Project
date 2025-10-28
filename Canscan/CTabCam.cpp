// CTabCam.cpp: 구현 파일
//

#include "pch.h"
#include "Canscan.h"
#include "afxdialogex.h"
#include "CTabCam.h"


// CTabCam 대화 상자

IMPLEMENT_DYNAMIC(CTabCam, CDialogEx)

CTabCam::CTabCam(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_CAM, pParent)
{

}

CTabCam::~CTabCam()
{
}

void CTabCam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);					// 기본 DDX 호출
	DDX_Control(pDX, IDC_LIST_TABLE, m_ListCam);	// 카메라 목록 컨트롤 바인딩
}


BEGIN_MESSAGE_MAP(CTabCam, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SERCH, &CTabCam::OnBnClickedBtnSerch)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CTabCam::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CTabCam::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CTabCam::OnBnClickedBtnConnect)
END_MESSAGE_MAP()

// CTabCam 메시지 처리기

// 검색 버튼 클릭
void CTabCam::OnBnClickedBtnSerch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

// 열기 버튼 클릭
void CTabCam::OnBnClickedBtnOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

// 닫기 버튼 클릭
void CTabCam::OnBnClickedBtnClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

// 연결 버튼 클릭
void CTabCam::OnBnClickedBtnConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

// List Control 초기화
BOOL CTabCam::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// List Control을 Report View 스타일로 설정
	m_ListCam.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 1. 열(Column) 추가
	m_ListCam.InsertColumn(0, _T("모델명"), LVCFMT_CENTER, 150, -1);
	m_ListCam.InsertColumn(1, _T("Position"), LVCFMT_CENTER, 150, -1);
	m_ListCam.InsertColumn(2, _T("SerialNum"), LVCFMT_CENTER, 150, -1);
	m_ListCam.InsertColumn(3, _T("Stats"), LVCFMT_CENTER, 150, -1);

	// 2. 확장 스타일 설정
	m_ListCam.SetExtendedStyle(
		LVS_EX_FULLROWSELECT |      // 행 전체 선택
		LVS_EX_GRIDLINES |          // 그리드 라인 표시
		LVS_EX_ONECLICKACTIVATE |   // 한 번 클릭으로 선택
		LVS_EX_HEADERDRAGDROP       // 헤더 드래그 가능
	);

	// 테스트 데이터 추가 (선택사항)
	// m_ListCam.InsertItem(0, _T("1"));
	// m_ListCam.SetItemText(0, 1, _T("Camera1"));
	// m_ListCam.SetItemText(0, 2, _T("SN12345"));

	return TRUE;
}
