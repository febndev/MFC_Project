using System;
using System.Net;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CSServer
{
    public partial class CSServerForm : Form
    {
        Server mServer;
        TCPClientToServer pyClient;

        public CSServerForm()
        {
            InitializeComponent();
            mServer = new Server();
            pyClient = new TCPClientToServer();
        }

        private async void btnAcceptClient_Click(object sender, EventArgs e)
        {
            await mServer.StartServerListeningAsync(
                ipaddr: IPAddress.Any,
                port : 7000,
                connectPythonAsync: () => pyClient.ConnectToServerAsync()
            );
        }
    }
}
