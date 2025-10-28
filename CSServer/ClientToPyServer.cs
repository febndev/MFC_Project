using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Buffers.Binary;

namespace CSServer
{   // py서버에 연결하기
    public class ClientToPyServer
    {
        private IPAddress? _pyServerIPAddress = IPAddress.Parse("10.10.21.117"); // py 서버 IP 주소 (현재는 종찬이 IP)
        private int _pyServerPort = 8888; // 서버 포트 번호
        private TcpClient? _csToPyClient; // 클라 소켓 

        public IPAddress? PyServerIPAddress => _pyServerIPAddress;
        public int PyServerPort => _pyServerPort;
        public TcpClient? CsToPyClient => _csToPyClient;
        // 파이썬 서버로 쓰는건 한 번에 하나씩만 가능하도록 세마포어 사용
        public System.Threading.SemaphoreSlim WriteLock { get; } = new(1, 1);


        public bool SetPyServerIPAddress(string addressStr)
        {
            if (!IPAddress.TryParse(addressStr, out var parsed))
            {
                Console.WriteLine(" 잘못된 서버 IP입니다.");
                return false;
            }

            _pyServerIPAddress = parsed;
            return true;
        }

        public bool SetPyServerPort(string portStr)
        {
            int portNum;
            if (!int.TryParse(portStr, out portNum) || portNum <= 0 || portNum > 65535)
            {
                Console.WriteLine(" 잘못된 서버 포트입니다. (유효범위 : 1 ~ 65535)");
                return false;
            }

            _pyServerPort = portNum;
            return true;
        }

        public async Task<NetworkStream> ConnectToPyServerAsync()
        {
            // 우선 연결된 py 서버가 있는지 확인 후 연결. 있으면 그 스트림 반환
            try
            {
                if (_csToPyClient == null || !_csToPyClient.Connected)
                {
                    _csToPyClient = new TcpClient();
                    // 검증한 IP, Port 로 서버에 연결 시도
                    await _csToPyClient.ConnectAsync(_pyServerIPAddress, _pyServerPort);
                    Console.WriteLine($" py 서버에 연결되었습니다. {_pyServerIPAddress}:{_pyServerPort}"); 
                }
                return _csToPyClient.GetStream();
            }
            catch (Exception ex)
            {
                Console.WriteLine($" py 서버 연결 오류: {ex.Message}");
                throw;
            }
        }

        public void Close()
        {
            try { _csToPyClient?.Close(); } catch { }
            _csToPyClient = null;
        }
    }
}

