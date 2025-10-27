using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Buffers.Binary;

namespace CSServer
{
    public class TCPClientToServer
    {
        private IPAddress? _serverIPAddress = IPAddress.Parse("127.0.0.1"); // 서버 IP 주소 
        private int _serverPort = 23000; // 서버 포트 번호
        private TcpClient? _tcpClient; // 클라 소켓 

        public IPAddress? ServerIPAddress => _serverIPAddress;
        public int ServerPort => _serverPort;
        public TcpClient? TcpClient => _tcpClient;

        public bool SetServerIPAddress(string addressStr)
        {
            if (!IPAddress.TryParse(addressStr, out var parsed))
            {
                Console.WriteLine(" 잘못된 서버 IP입니다.");
                return false;
            }

            _serverIPAddress = parsed;
            return true;
        }

        public bool SetServerPort(string portStr)
        {
            int portNum;
            if (!int.TryParse(portStr, out portNum) || portNum <= 0 || portNum > 65535)
            {
                Console.WriteLine(" 잘못된 서버 포트입니다. (유효범위 : 1 ~ 65535)");
                return false;
            }

            _serverPort = portNum;
            return true;
        }

        public async Task ConnectToServerAsync()
        {
            // 서버가 연결되어 있지 않으면 
            if (_tcpClient == null) _tcpClient = new TcpClient();

            try
            {   // 검증한 IP, Port 로 서버에 연결 시도
                await _tcpClient.ConnectAsync(_serverIPAddress, _serverPort);
                Console.WriteLine($" 서버에 연결되었습니다. {_serverIPAddress}:{_serverPort}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($" 서버 연결 오류: {ex.Message}");
                throw;
            }
        }

        public void Close()
        {
            try { _tcpClient?.Close(); } catch { }
            _tcpClient = null;
        }
    }
}

