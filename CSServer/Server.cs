using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CSServer
{
    internal class Server
    {
        IPAddress? mIP;
        int mPort;
        TcpListener? mTcpListener;

        public bool KeepRunning { get; set; } = false;

        public Server() { }

        public async Task StartServerListeningAsync(IPAddress? ipaddr = null, int port = 7000)
        {
            if (ipaddr == null)
            {
                ipaddr = IPAddress.Any;
                // MFC 클라이언트가 내 컴퓨터에서 실행할때는 IPAddress.Loopback,
                // 다른 PC 에서 실행할때는 IPAddress.Any 로 변경 해야함. 
            }
                if (port <= 0 || port > 65535)
                port = 7000; 

            mIP = ipaddr;
            mPort = port;

            mTcpListener = new TcpListener(mIP, mPort);

            try
            {
                mTcpListener.Start();
                Console.WriteLine($"Server started on {mIP.ToString()}:{mPort}");

                KeepRunning = true;

                while (KeepRunning)
                {
                    TcpClient client = await mTcpListener.AcceptTcpClientAsync();
                    Console.WriteLine("Client connected.");

                    _ = Task.Run(() => new ClientSession(client).ManageClientAsync());
                }
            }
            catch (IOException)
            {
                throw; // 타임아웃, 연결문제 발생시 종료 
            }
            catch (SocketException) when (!KeepRunning)
            {
                throw; // Stop() 으로 Server 닫히면 여기로 들어오면서 정상 종료. 
            }
            catch (Exception ex)
            {
                Console.WriteLine($"AcceptLoop 오류 : {ex.Message}");
            }
        }


    }
}
