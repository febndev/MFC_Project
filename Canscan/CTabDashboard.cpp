// CTabDashboard.cpp: 구현 파일
//

#include "pch.h"
#include "Canscan.h"
#include "CanscanDlg.h"
#include "CTabCam.h"
#include "CTabDashboard.h"
#include "afxdialogex.h"


// CTabDashboard 대화 상자

IMPLEMENT_DYNAMIC(CTabDashboard, CDialogEx)

// CTabDashboard 생성자
CTabDashboard::CTabDashboard(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_DASHBOARD, pParent)
{

}

// CTabDashboard 소멸자
CTabDashboard::~CTabDashboard()
{
	if (m_fontResult.GetSafeHandle())   m_fontResult.DeleteObject();        // 폰트 객체 해제
	if (m_brushResultBG.GetSafeHandle()) m_brushResultBG.DeleteObject();    // 브러시 객체 해제
}

// 데이터 교환기
void CTabDashboard::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabDashboard, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CTabDashboard::OnBnClickedBtnInspection)  // 검사 버튼 클릭 핸들러 매핑
	ON_MESSAGE(WM_UPDATE_RESULT_TEXT, &CTabDashboard::OnUpdateResultText)        // 사용자 정의 메시지 핸들러 매핑
	ON_WM_CTLCOLOR()    // 컨트롤 색상 변경 핸들러 매핑
END_MESSAGE_MAP()


// CTabDashboard 메시지 처리기

// 검사 버튼 클릭 핸들러
void CTabDashboard::OnBnClickedBtnInspection()
{
	// 결과 텍스트 초기화
    SetDlgItemText(IDC_RESULT, L"");
    
    // 부모 다이얼로그 가져오기
    CCanscanDlg* pMain = dynamic_cast<CCanscanDlg*>(GetParent()->GetParent());
    if (!pMain) return;

    // 서버 연결 확인
    if (!pMain->IsConnected()) {
        AfxMessageBox(L"서버가 연결되어 있지 않습니다!");
        return;
    }

    // 카메라 탭에서 현재 프레임 가져오기
    const cv::Mat& frame = pMain->GetTabCam().GetFrame();
    if (frame.empty()) {
        AfxMessageBox(L"현재 카메라 프레임이 없습니다!");
        return;
    }

    // jpg로 인코딩 후 서버 전송
    std::vector<uint8_t> buffer;
    cv::imencode(".jpg", frame, buffer);

    pMain->GetPacket().Send(MsgType::IMG_REQ, buffer);
    AfxMessageBox(L"검사 요청(이미지) 전송 완료!");

    // ===== 재전송 대비용 버퍼 저장 =====
    pMain->m_lastSentImage = buffer;   // CCanscanDlg에 멤버로 추가
    pMain->SetWaitingForImgRes(true);  // 상태 표시
}

// 결과 텍스트 업데이트 핸들러
LRESULT CTabDashboard::OnUpdateResultText(WPARAM wParam, LPARAM lParam)
{
    int resultCode = static_cast<int>(wParam); // 0, 1, 기타
    CString text;

    switch (resultCode)
    {
    case 0:  text = _T("PASS"); break;
    case 1:  text = _T("FAIL"); break;
    default:
        text = _T("UNKNOWN"); // 또는 _T("")로 공백 처리
        break;
    }

    m_resultCode = resultCode;                 // 현재 결과 상태 저장
    SetDlgItemText(IDC_RESULT, text);          // 기존에 하시던 텍스트 갱신
    GetDlgItem(IDC_RESULT)->Invalidate();      // 색 반영 위해 다시 그리기

    // resultCode: 0=PASS, 1=FAIL, 기타=UNKNOWN
    if (resultCode == 0) {
        ++m_totalCount;
        ++m_goodCount;
    }
    else if (resultCode == 1) {
        ++m_totalCount;
        ++m_defectCount;
    }
    // 증가분 UI 반영
    UpdateCountersUI();

    return 0;
}

// 폰트 생성 및 배경 브러시 준비 핸들러   
BOOL CTabDashboard::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 2-1) 결과창 큰 볼드 폰트 생성 (예: 28pt, Segoe UI)
    CDC* dc = GetDC();
    const int logPixelsY = dc ? dc->GetDeviceCaps(LOGPIXELSY) : 96;
    if (dc) ReleaseDC(dc);

    const int pointSize = 28; // 원하시는 크기로 조절
    const int height = -MulDiv(pointSize, logPixelsY, 72);

    m_fontResult.CreateFont(
        height, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Malgun Gothic"));

    CWnd* pResult = GetDlgItem(IDC_RESULT);
    if (pResult != nullptr) {
        pResult->SetFont(&m_fontResult);
    }

    // 2-2) 결과 배경 흰색(원하면 다른 색)으로 칠할 브러시
    m_brushResultBG.CreateSolidBrush(RGB(255, 255, 255));

	// 초기 카운터 값 설정
    SetDlgItemInt(IDC_TOTAL, 0, FALSE);
    SetDlgItemInt(IDC_GOOD, 0, FALSE);
    SetDlgItemInt(IDC_DEFECT, 0, FALSE);

    return TRUE;
}

// 컨트롤 색상 변경 핸들러
HBRUSH CTabDashboard::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd != nullptr && pWnd->GetDlgCtrlID() == IDC_RESULT)
    {
        // 배경을 깔끔하게 보이게 하려면 투명 + 직접 브러시 반환
        pDC->SetBkMode(TRANSPARENT);

        // 기본색(UNKNOWN)
        COLORREF color = RGB(80, 80, 80);

        if (m_resultCode == 0)       color = RGB(0, 160, 0);    // PASS: 초록
        else if (m_resultCode == 1)  color = RGB(220, 0, 0);    // FAIL: 빨강

        pDC->SetTextColor(color);

        // 결과 영역 배경색 지정(흰색)
        return (HBRUSH)m_brushResultBG.GetSafeHandle();
    }

    return hbr;
}

// 카운터 값들을 컨트롤에 반영하는 함수
void CTabDashboard::UpdateCountersUI()
{
    SetDlgItemInt(IDC_TOTAL, m_totalCount, FALSE);
    SetDlgItemInt(IDC_GOOD, m_goodCount, FALSE);
    SetDlgItemInt(IDC_DEFECT, m_defectCount, FALSE);
}