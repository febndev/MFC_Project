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

    // PNG로 인코딩 후 서버 전송
    std::vector<uint8_t> buffer;
    cv::imencode(".png", frame, buffer);

    pMain->GetPacket().Send(MsgType::IMG_REQ, buffer);
    AfxMessageBox(L"검사 요청(이미지) 전송 완료!");

    // ===== 재전송 대비용 버퍼 저장 =====
    pMain->m_lastSentImage = buffer;   // CCanscanDlg에 멤버로 추가
    pMain->SetWaitingForImgRes(true);  // 상태 표시
}
