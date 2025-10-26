using System;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CSServer
{
    public partial class CSServerForm : Form
    {
        Server mServer;

        public CSServerForm()
        {
            InitializeComponent();
            mServer = new Server();
        }

        private async void btnAcceptClient_Click(object sender, EventArgs e)
        {
            await mServer.StartServerListeningAsync();
        }
    }
}
