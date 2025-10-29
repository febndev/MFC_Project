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
    m_ListCam.InsertColumn(0, _T("번호"), LVCFMT_CENTER, 60);
    m_ListCam.InsertColumn(1, _T("모델명"), LVCFMT_LEFT, 180);
    m_ListCam.InsertColumn(2, _T("시리얼"), LVCFMT_CENTER, 140);
    m_ListCam.InsertColumn(3, _T("상태"), LVCFMT_CENTER, 100);

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
            int row = m_ListCam.InsertItem(i, std::to_wstring(i).c_str());
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
    if (sel < 0) { AfxMessageBox(L"카메라를 선택하세요."); return; }

    if (m_camera.Start())
    {
        UpdateListRow(sel, L"Connect");
        SetTimer(1, 33, nullptr); // ~30fps
    }
    else
        AfxMessageBox(L"연결(그랩 시작) 실패");
}

void CTabCam::OnBnClickedBtnClose()
{
    KillTimer(1);
    m_camera.Close();

    int sel = m_ListCam.GetNextItem(-1, LVNI_SELECTED);
    if (sel >= 0) UpdateListRow(sel, L"Closed");

    // 미리보기 영역을 검정 화면으로 초기화하고 싶다면:
    if (m_pParentDlg)
    {
        cv::Mat black(480, 640, CV_8UC1, cv::Scalar(0));
        m_pParentDlg->UpdateFrame(black);
    }
}

void CTabCam::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1 && m_camera.IsConnected())
    {
        if (m_camera.RetrieveFrame(m_frame) && m_pParentDlg)
            m_pParentDlg->UpdateFrame(m_frame);
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CTabCam::UpdateListRow(int row, const wchar_t* status)
{
    if (row >= 0 && row < m_ListCam.GetItemCount())
        m_ListCam.SetItemText(row, 3, status);
}
