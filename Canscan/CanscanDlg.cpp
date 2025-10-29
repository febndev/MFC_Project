#include "pch.h"
#include "Canscan.h"
#include "CanscanDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ===== AboutDlg (필요시) =====
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}
protected:
    virtual void DoDataExchange(CDataExchange* pDX) { CDialogEx::DoDataExchange(pDX); }
    DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ===== CCanscanDlg =====
CCanscanDlg::CCanscanDlg(CWnd* pParent)
    : CDialogEx(IDD_CANSCAN_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
void CCanscanDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    if (m_isConnected) {
        m_packet.Disconnect();
        m_isConnected = false;
    }
}


void CCanscanDlg::DoDataExchange(CDataExchange* pDX)
{
    // 디버그용 콘솔, 모두 구현 후 삭제할 것
#ifdef _DEBUG
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
#endif

    CDialogEx::DoDataExchange(pDX);
    // 탭 컨트롤 ID는 반드시 "IDC_TAB_MAIN" 이어야 함 (리소스 확인!)
    DDX_Control(pDX, IDC_TAB_MAIN, m_tabMain);
}

BEGIN_MESSAGE_MAP(CCanscanDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CCanscanDlg::OnTabSelChange)
END_MESSAGE_MAP()

BOOL CCanscanDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 시스템 메뉴에 "정보..." 추가 (선택)
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        CString strAboutMenu; strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 아이콘
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // 탭 초기화
    InitTabControl();

    // 미리보기 초기 화면(검정)
    // 리소스에 Static(Bitmap) 컨트롤: IDC_PICTURE_CAM 있어야 함
    {
        cv::Mat black(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
        HBITMAP hBmp = MatToHBITMAP(black);
        CStatic* pic = (CStatic*)GetDlgItem(IDC_PICTURE_CAM);
        if (pic && hBmp) pic->SetBitmap(hBmp);
    }

    if (m_packet.Connect("10.10.21.101", 7000)) {
        m_isConnected = true;
        AfxMessageBox(L"서버 연결 성공");
    }
    else {
        m_isConnected = false;
        AfxMessageBox(L"서버 연결 실패");
    }

    // ===================== 수신 스레드 =====================
    std::thread([this] {
        MsgType type;
        uint32_t imgId;
        std::vector<uint8_t> body;
        ULONGLONG lastSendTick = GetTickCount64();
        const DWORD TIMEOUT_MS = 10000; // 10초
        bool waitingForImgRes = false;

        while (m_isConnected)
        {
            // 1) 수신 시도 (blocking)
            if (m_packet.Receive(type, imgId, body))
            {
                switch (type)
                {
                case MsgType::IMG_RES:
                    std::cout << "[RECV][IMG_RES] 이미지 결과 수신 완료 (ID=" << imgId << ")\n";
                    waitingForImgRes = false;
                    break;

                case MsgType::RESULT_REQ:
                {
                    if (!body.empty()) {
                        uint8_t val = body[0];
                        std::cout << "[RECV][RESULT_REQ] 서버 결과 값: " << (int)val << std::endl;

                        if (val == 1)
                            std::cout << "검사 결과: FAIL" << std::endl;
                        else if (val == 0)
                            std::cout << "검사 결과: PASS" << std::endl;
                        else
                            std::cout << "알 수 없는 결과 코드" << std::endl;
                    }
                    else {
                        std::cout << "[RECV][RESULT_REQ] Body가 비어 있음!" << std::endl;
                    }

                    // 2) 잘 받았다고 응답 (RESULT_RES)
                    std::string ackMsg = "ACK:" + std::to_string(imgId);
                    std::vector<uint8_t> ackBody(ackMsg.begin(), ackMsg.end());
                    m_packet.Send(MsgType::RESULT_RES, ackBody);
                    std::cout << "[SEND][RESULT_RES] ACK 전송 완료\n";
                    break;
                }

                default:
                    std::cout << "[RECV] Unknown MsgType=" << (int)type << " (" << body.size() << " bytes)" << std::endl;
                    break;
                }
            }
            else
            {
                // recv 실패 시, 소켓이 닫혔거나 일시적 오류일 수 있음
                Sleep(10);
            }

            // 3) IMG_RES 10초 타임아웃 체크
            if (waitingForImgRes && GetTickCount64() - lastSendTick >= TIMEOUT_MS)
            {
                std::cout << "[WARN] 10초 내 IMG_RES 미수신 → 재전송\n";

                // 이 부분은 다시 보내고 싶은 이미지 버퍼를 저장해둔 곳에서 재전송해야 함
                if (!m_lastSentImage.empty())
                {
                    m_packet.Send(MsgType::IMG_REQ, m_lastSentImage);
                    lastSendTick = GetTickCount64();
                }
            }
        }

        std::cout << "[INFO] 수신 스레드 종료됨." << std::endl;
        }).detach();

    return TRUE;
}

void CCanscanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout; dlgAbout.DoModal();
    }
    else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CCanscanDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect; GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CCanscanDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// ===== 탭 =====
void CCanscanDlg::InitTabControl()
{
    // 탭 추가
    m_tabMain.InsertItem(0, _T("대시보드"));
    m_tabMain.InsertItem(1, _T("카메라"));

    // 각 탭 다이얼로그 생성(부모=탭 컨트롤)
    m_tabDashboard.Create(IDD_TAB_DASHBOARD, &m_tabMain);
    m_tabCam.Create(IDD_TAB_CAM, &m_tabMain);

    // 탭 클라이언트 영역으로 맞춤
    CRect rc; m_tabMain.GetClientRect(&rc);
    m_tabMain.AdjustRect(FALSE, &rc);
    m_tabDashboard.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
    m_tabCam.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);

    // 기본 탭 표시
    ShowTab(0);
}

void CCanscanDlg::ShowTab(int nTab)
{
    m_tabDashboard.ShowWindow(SW_HIDE);
    m_tabCam.ShowWindow(SW_HIDE);

    switch (nTab)
    {
    case 0: m_tabDashboard.ShowWindow(SW_SHOW); break;
    case 1: m_tabCam.ShowWindow(SW_SHOW); break;
    }
}

void CCanscanDlg::OnTabSelChange(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
    int sel = m_tabMain.GetCurSel();
    ShowTab(sel);
    *pResult = 0;
}

// ===== 프레임 갱신 & 변환 =====
void CCanscanDlg::UpdateFrame(const cv::Mat& frame)
{
    if (frame.empty()) return;

    cv::Mat resized;
    cv::resize(frame, resized, cv::Size(640, 480), 0, 0, cv::INTER_AREA);

    HBITMAP hBmp = MatToHBITMAP(resized);
    CStatic* pic = (CStatic*)GetDlgItem(IDC_PICTURE_CAM);
    if (pic && hBmp) pic->SetBitmap(hBmp);
}

HBITMAP CCanscanDlg::MatToHBITMAP(const cv::Mat& mat)
{
    if (mat.empty()) return nullptr;

    cv::Mat rgb;
    if (mat.channels() == 1) cv::cvtColor(mat, rgb, cv::COLOR_GRAY2BGR);
    else                     rgb = mat;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = rgb.cols;
    bi.bmiHeader.biHeight = -rgb.rows; // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 24;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp) return nullptr;

    memcpy(bits, rgb.data, rgb.total() * 3);
    return hBmp;
}
