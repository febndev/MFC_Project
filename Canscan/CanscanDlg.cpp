
// CanscanDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Canscan.h"
#include "CanscanDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCanscanDlg 대화 상자



CCanscanDlg::CCanscanDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CANSCAN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// 데이터 교환 지원입니다.
void CCanscanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDD_TAB_MAIN, m_tabMain);
}

// 메시지 맵입니다.
BEGIN_MESSAGE_MAP(CCanscanDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDD_TAB_MAIN, &CCanscanDlg::OnTabSelChange)	// 탭 선택 변경 이벤트
END_MESSAGE_MAP()


// CCanscanDlg 메시지 처리기

BOOL CCanscanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	InitTabControl();				// 탭 컨트롤 초기화

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CCanscanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCanscanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCanscanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 탭 컨트롤 초기화 함수
void CCanscanDlg::InitTabControl()
{
	// 탭 아이템 추가
	m_tabMain.InsertItem(0, _T("대시보드"));		// 첫 번째 탭 (Dashboard)
	m_tabMain.InsertItem(1, _T("카메라"));		// 두 번째 탭 (Cam)

	// 탭 다이얼로그 생성
	m_tabDashboard.Create(IDD_TAB_DASHBOARD, &m_tabMain);
	m_tabCam.Create(IDD_TAB_CAM, &m_tabMain);

	// 탭 컨트롤의 클라이언트 영역 계산
	CRect rect;
	m_tabMain.GetClientRect(&rect);
	m_tabMain.AdjustRect(FALSE, &rect);

	// 탭 다이얼로그 위치 및 크기 설정
	m_tabDashboard.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
	m_tabCam.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);

	// 첫 번째 탭을 기본으로 표시
	ShowTab(0);
}

// 탭 전환 함수
void CCanscanDlg::ShowTab(int nTab)
{
	// 모든 탭 숨기기
	m_tabDashboard.ShowWindow(SW_HIDE);
	m_tabCam.ShowWindow(SW_HIDE);

	// 선택된 탭만 표시
	switch (nTab)
	{
	case 0:		// 탭1 - Dashboard
		m_tabDashboard.ShowWindow(SW_SHOW);
		break;
	case 1:		// 탭2 - Cam
		m_tabCam.ShowWindow(SW_SHOW);
		break;
	}
}

// 탭 선택 변경 이벤트 핸들러
void CCanscanDlg::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nTab = m_tabMain.GetCurSel();	// 현재 선택된 탭 인덱스 가져오기
	ShowTab(nTab);						// 해당 탭 표시
	*pResult = 0;
}