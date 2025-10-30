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
        bool _serverStarted = false; // �� �� ���� ����


        public CSServerForm()
        {
            InitializeComponent();

            // �ܼ� ��� ��θ� logBox�� ����
            Console.SetOut(new RichTextBoxWriter(logBox));
            Console.SetError(new RichTextBoxWriter(logBox)); // ������ ���� ������ ����

            mServer = new Server();
            //pyClient = new ClientToPyServer();
        }


        //private void CSServerForm_Shown(object sender, EventArgs e)
        //{
        //    base.OnShown(e); // Form �⺻ ����(Shown �̺�Ʈ �߻� ��)�� ���� ����
        //                     // ������� �� �߰� �۾� (���� ���� ��)

        //    if (_serverStarted) return;
        //    _serverStarted = true;

        //    var pyIp = "10.10.21.117";
        //    var pyPort = 8888;

        //    // UI �ȸ��� , ��׶��忡�� ���� ����
        //    _ = mServer.StartServerListeningAsync(
        //        ipaddr: IPAddress.Any,
        //        port: 7000,
        //        pyIp: pyIp,
        //        pyPort: pyPort
        //    );
        //    Console.WriteLine("C# ���� ����");
        //}

        //private void CSServerForm_FormClosing(object sender, FormClosingEventArgs e)
        //{
        //    base.OnFormClosing(e);
        //    // �� �� ���� �� ����
        //    mServer.Stop();
        //}

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            if (_serverStarted) return;
            _serverStarted = true;

            // (�ϵ��ڵ�) �Ǵ� TextBox���� �о����
            var pyIp = "10.10.21.117";
            var pyPort = 8888;

            // �� await ���� ���� ��׶���� ���� (UI �ȸ���)
            _ = mServer.StartServerListeningAsync(
                ipaddr: IPAddress.Any,
                port: 7000,
                pyIp: pyIp,
                pyPort: pyPort
            );

            // ���ϸ� UI�� �佺Ʈ/�޽���
            // MessageBox.Show("���� ��� ����");
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            base.OnFormClosing(e);
            // �� �� ���� �� ����
            mServer.Stop();
        }
    }
}
