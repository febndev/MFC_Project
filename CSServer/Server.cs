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
        // 클라이언트 리스트
        public List<TcpClient> CppClients { get; } = new(); 

        public bool KeepRunning { get; set; } = false;

        public Server() { }

        public async Task StartServerListeningAsync(
            IPAddress? ipaddr = null, 
            int port = 7000,
            Func<Task>? connectPythonAsync = null //파이썬 연결 콜백
        )
        {

            if (ipaddr == null)
            {
                ipaddr = IPAddress.Any;
                // MFC 클라이언트가 내 컴퓨터에서 실행할때는 IPAddress.Loopback,
                // 다른 PC 에서 실행할때는 IPAddress.Any 로 변경 해야함. 
            }
            if (port <= 0 || port > 65535) 
            { 
                port = 7000;
            }

            mIP = ipaddr;
            mPort = port;

            mTcpListener = new TcpListener(mIP, mPort);

            try
            {
                mTcpListener.Start();
                Console.WriteLine($"Server started on {mIP.ToString()}:{mPort}");

                KeepRunning = true;
                bool pythonConnected = false; // python 연결을 한번만 하도록 스위치

                while (KeepRunning)
                {   // 클라이언트 연결 요청 수락, 리스트 추가 
                    TcpClient client = await mTcpListener.AcceptTcpClientAsync();
                    Console.WriteLine("C++ Client connected.");
                    CppClients.Add(client);

                    if(!pythonConnected) {
                        pythonConnected = true;
                        // python 서버로 연결요청 
                        if (connectPythonAsync != null)
                        {
                            _ = Task.Run(async () =>
                            {
                                try 
                                { 
                                    await connectPythonAsync(); 
                                }
                                catch (Exception ex) { 
                                    Console.WriteLine("[PY] connect FAIL: " + ex.Message);
                                    pythonConnected = false;
                                }
                            });
                        }
                    }

                    // 파싱 로직 
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
