using System;
using System.Net;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CSServer
{
    public partial class CSServerForm : Form
    {
        Server mServer;
        ClientToPyServer pyClient;

        public CSServerForm()
        {
            InitializeComponent();

            // 콘솔 출력 경로를 logBox로 변경
            Console.SetOut(new RichTextBoxWriter(logBox));
            Console.SetError(new RichTextBoxWriter(logBox)); // 오류도 같이 보려면 선택

            mServer = new Server();
            pyClient = new ClientToPyServer();
        }

        private async void btnAcceptClient_Click(object sender, EventArgs e)
        {
            await mServer.StartServerListeningAsync(
                ipaddr: IPAddress.Any,
                port : 7000,
                pyServer : pyClient
            );
        }
    }
}
