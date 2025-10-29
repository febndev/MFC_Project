#include "pch.h"
#include "Canscan.h"
#include "CanscanDlg.h"
#include "CTabCam.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CTabCam, CDialogEx)

CTabCam::CTabCam(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_TAB_CAM, pParent) {
}

CTabCam::~CTabCam() {}

void CTabCam::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_TABLE, m_ListCam);
}

BEGIN_MESSAGE_MAP(CTabCam, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_SERCH, &CTabCam::OnBnClickedBtnSerch)
    ON_BN_CLICKED(IDC_BTN_OPEN, &CTabCam::OnBnClickedBtnOpen)
    ON_BN_CLICKED(IDC_BTN_CONNECT, &CTabCam::OnBnClickedBtnConnect)
    ON_BN_CLICKED(IDC_BTN_CLOSE, &CTabCam::OnBnClickedBtnClose)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TABLE, &CTabCam::OnListSelChanged)
    ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CTabCam::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 부모 다이얼로그 포인터 (탭 컨트롤 아래라 GetParent()->GetParent()가 실제 부모)
    CWnd* p1 = GetParent();
    CWnd* p2 = p1 ? p1->GetParent() : nullptr;
    m_pParentDlg = dynamic_cast<CCanscanDlg*>(p2);

    m_ListCam.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_ListCam.InsertColumn(0, _T("번호"), LVCFMT_CENTER, 40);
    m_ListCam.InsertColumn(1, _T("모델명"), LVCFMT_LEFT, 120);
    m_ListCam.InsertColumn(2, _T("시리얼"), LVCFMT_CENTER, 100);
    m_ListCam.InsertColumn(3, _T("상태"), LVCFMT_CENTER, 100);

    // ★ 초기 상태/ FPS 표시
    UpdateInfoLabel(false);              // "카메라 종료됨"
    SetTextToControl(IDC_STATIC_FPS, L"- fps");

    return TRUE;
}

void CTabCam::OnBnClickedBtnSerch()
{
    m_ListCam.DeleteAllItems();

    std::vector<CameraInfo> cams;
    if (m_camera.EnumerateDevices(cams))
    {
        for (int i = 0; i < (int)cams.size(); ++i)
        {
            int row = m_ListCam.InsertItem(i, std::to_wstring(i+1).c_str());
            m_ListCam.SetItemText(row, 1, cams[i].model);
            m_ListCam.SetItemText(row, 2, cams[i].serial);
            m_ListCam.SetItemText(row, 3, L"Find");
        }
    }
    else
    {
        AfxMessageBox(L"카메라를 찾지 못했습니다.");
    }
}

void CTabCam::OnBnClickedBtnOpen()
{
    int sel = m_ListCam.GetNextItem(-1, LVNI_SELECTED);
    if (sel < 0) { AfxMessageBox(L"카메라를 선택하세요."); return; }

    if (m_camera.Open(sel))
        UpdateListRow(sel, L"Open");
    else
        AfxMessageBox(L"열기 실패");
}

void CTabCam::OnBnClickedBtnConnect()
{
    int sel = m_ListCam.GetNextItem(-1, LVNI_SELECTED);
    if (sel < 0) { AfxMessageBox(L"카메라 선택"); return; }

    if (m_camera.Start())
    {
        m_ListCam.SetItemText(sel, 3, L"Connect");
        UpdateInfoLabel(true);       // ★ 연결됨
        ResetFpsCounters();          // ★ FPS 리셋
        SetTimer(1, 10, nullptr);    // 30fps 정도
    }
}

void CTabCam::OnBnClickedBtnClose()
{
    KillTimer(1);
    m_camera.Close();

    // 🔹 검은 화면 초기화 (CanscanDlg의 MatToHBITMAP 활용)
    {
        cv::Mat black(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));

        // 부모 다이얼로그(CCanscanDlg) 가져오기
        CCanscanDlg* pParent = (CCanscanDlg*)GetParent();
        if (pParent)
        {
            HBITMAP hBmp = pParent->MatToHBITMAP(black);
            CStatic* pic = (CStatic*)GetDlgItem(IDC_PICTURE_CAM);
            if (pic && hBmp)
                pic->SetBitmap(hBmp);
        }
    }

    // 🔹 리스트 상태 "Closed"로 변경
    int sel = m_ListCam.GetNextItem(-1, LVNI_SELECTED);
    if (sel >= 0)
        m_ListCam.SetItemText(sel, 3, L"Closed");

    // 🔹 상태 표시 갱신
    UpdateInfoLabel(false);
    SetTextToControl(IDC_STATIC_FPS, L"- fps");
}

void CTabCam::OnListSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    // 선택 상태 변화를 가진 경우만 처리
    if ((pNMLV->uChanged & LVIF_STATE) &&
        ((pNMLV->uNewState ^ pNMLV->uOldState) & LVIS_SELECTED))
    {
        int sel = m_ListCam.GetNextItem(-1, LVNI_SELECTED);
        if (sel >= 0)
        {
            CString model = m_ListCam.GetItemText(sel, 1);  // "모델명" 컬럼
            CString serial = m_ListCam.GetItemText(sel, 2); // "시리얼" 컬럼

            // ★ 여기에 원하는 리소스 ID로 텍스트 설정
            SetTextToControl(IDC_STATIC_SELECTED, model);
            SetTextToControl(IDC_STATIC_SERIALNUM, serial);
        }
        else
        {
            // 선택 해제 시 초기화
            SetTextToControl(IDC_STATIC_SELECTED, L"-");
            SetTextToControl(IDC_STATIC_SERIALNUM, L"-");
        }
    }

    if (pResult) *pResult = 0;
}

void CTabCam::SetTextToControl(UINT id, const CString& text)
{
    // 1) 먼저 탭(자기 자신)에서 찾기
    if (CWnd* here = GetDlgItem(id)) { here->SetWindowText(text); return; }

    // 2) 탭의 부모(=탭컨트롤), 그 부모(=메인 다이얼로그) 순으로 탐색
    CWnd* p = GetParent();               // 탭컨트롤
    if (p) p = p->GetParent();           // 메인 다이얼로그 (대부분 여기)
    if (p) {
        if (CWnd* there = p->GetDlgItem(id)) {
            there->SetWindowText(text);
            return;
        }
    }
    // 못 찾을 경우는 조용히 무시 (필요하면 TRACE 등)
}
void CTabCam::UpdateInfoLabel(bool connected)
{
    SetTextToControl(IDC_STATIC_INFO, connected ? L"카메라 연결됨" : L"카메라 종료됨");
}

void CTabCam::ResetFpsCounters()
{
    m_lastFpsTick = GetTickCount64();
    m_frameCount = 0;
    m_currentFps = 0;
    SetTextToControl(IDC_STATIC_FPS, L"0 fps");
}


void CTabCam::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        // 연결 체크: IsConnected()가 없으면 IsGrabbing()으로 대체
        if (m_camera.IsConnected() && m_camera.RetrieveFrame(m_frame))
        {
            // 부모 다이얼로그로 영상 갱신 전달 (이미 구현됨)
            if (m_pParentDlg)
                m_pParentDlg->UpdateFrame(m_frame);

            // ★ FPS 계산
            ++m_frameCount;
            ULONGLONG now = GetTickCount64();
            if (now - m_lastFpsTick >= 1000) // 1초마다
            {
                m_currentFps = m_frameCount;
                m_frameCount = 0;
                m_lastFpsTick = now;

                CString s; s.Format(L"%d fps", m_currentFps);
                SetTextToControl(IDC_STATIC_FPS, s);
            }

            // 안전빵으로 연결 라벨도 유지
            UpdateInfoLabel(true);
        }
        else
        {
            // 프레임을 못 가져오면 연결 상태 재확인
            UpdateInfoLabel(false);
        }
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CTabCam::UpdateListRow(int row, const wchar_t* status)
{
    if (row >= 0 && row < m_ListCam.GetItemCount())
        m_ListCam.SetItemText(row, 3, status);
}
