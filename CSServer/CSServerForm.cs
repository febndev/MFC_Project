using System;
using System.Net;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CSServer
{
    public partial class CSServerForm : Form
    {
        Server mServer;
        //ClientToPyServer pyClient;
        bool _serverStarted = false; // 두 번 시작 방지


        public CSServerForm()
        {
            InitializeComponent();

            // 콘솔 출력 경로를 logBox로 변경
            Console.SetOut(new RichTextBoxWriter(logBox));
            Console.SetError(new RichTextBoxWriter(logBox)); // 오류도 같이 보려면 선택

            mServer = new Server();
            //pyClient = new ClientToPyServer();
        }


        //private void CSServerForm_Shown(object sender, EventArgs e)
        //{
        //    base.OnShown(e); // Form 기본 동작(Shown 이벤트 발생 등)을 먼저 수행
        //                     // 여기부터 내 추가 작업 (서버 시작 등)

        //    if (_serverStarted) return;
        //    _serverStarted = true;

        //    var pyIp = "10.10.21.117";
        //    var pyPort = 8888;

        //    // UI 안막힘 , 백그라운드에서 서버 시작
        //    _ = mServer.StartServerListeningAsync(
        //        ipaddr: IPAddress.Any,
        //        port: 7000,
        //        pyIp: pyIp,
        //        pyPort: pyPort
        //    );
        //    Console.WriteLine("C# 서버 시작");
        //}

        //private void CSServerForm_FormClosing(object sender, FormClosingEventArgs e)
        //{
        //    base.OnFormClosing(e);
        //    // ★ 폼 닫을 때 정리
        //    mServer.Stop();
        //}

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            if (_serverStarted) return;
            _serverStarted = true;

            // (하드코딩) 또는 TextBox에서 읽어오기
            var pyIp = "10.10.21.117";
            var pyPort = 8888;

            // ★ await 하지 말고 백그라운드로 시작 (UI 안막힘)
            _ = mServer.StartServerListeningAsync(
                ipaddr: IPAddress.Any,
                port: 7000,
                pyIp: pyIp,
                pyPort: pyPort
            );

            // 원하면 UI에 토스트/메시지
            // MessageBox.Show("서버 대기 시작");
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            base.OnFormClosing(e);
            // ★ 폼 닫을 때 정리
            mServer.Stop();
        }
    }
}
