using System;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;

namespace CSServer
{
    internal static class MainApp
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static async Task Main()
        {
            // To customize application configuration such as set high DPI settings or default font,
            // see https://aka.ms/applicationconfiguration.
            ApplicationConfiguration.Initialize();
            await Db.Global.EnsureTableAsync();
            Application.Run(new CSServerForm());
        }
    }
}