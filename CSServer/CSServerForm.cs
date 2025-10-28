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
