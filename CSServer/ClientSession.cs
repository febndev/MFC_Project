using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CSServer
{
    internal class ClientSession
    {
        private readonly TcpClient cppClient;
        private readonly ClientToPyServer pyServer;

        public ClientSession(TcpClient client, ClientToPyServer pyServerClient)
        {
            cppClient = client;
            pyServer = pyServerClient;
        }

        public async Task ManageClientAsync()
        {
            NetworkStream? cppStream = null;
            // StreamReader? reader = null;
            NetworkStream? pyStream = null;
            try
            {
                cppStream = cppClient.GetStream();
                pyStream = await pyServer.ConnectToPyServerAsync();
                //reader = new StreamReader(stream);
                while (true)
                {
                    var (msgType, imgId, body) = await ReceiveOnePacketAsync(cppStream, pyStream);
                    //Console.WriteLine($"MsgType={msgType}, ImgId={imgId}, BodyLen={body.Length}"); 
                
                
                
                
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"[ClientSession] 오류: {ex.Message}");
                throw;
            }

        }

        // 매개변수 만큼 읽어내는 메서드 
        public static async Task ReadExactlyAsync(NetworkStream cppStream, byte[] buf, int count)
        {
            int off = 0;
            while (off < count)
            {
                int n = await cppStream.ReadAsync(buf, off, count - off);
                if (n == 0) throw new EndOfStreamException("remote closed during read");

                off += n;
            }
            // void 로 하던지 아니면 Task 로 해야되나
        }

        // 패킷 파싱하고 복사 후 파이썬 서버로 전송
        static async Task<(int msgType, uint imgId, byte[] body)> ReceiveOnePacketAsync(NetworkStream cppStream, NetworkStream pyStream)
        {
            // 1) 헤더 9바이트
            byte[] header = new byte[9];
            await ReadExactlyAsync(cppStream, header, 9);
            // Console.WriteLine($"HDR={BitConverter.ToString(header)}");

            // 2) 파싱 (바이너리 데이터 파싱, 리틀엔디안으로 변경시 ReadUInt32LittleEndian으로 변경 )
            int msgType = header[0];
            uint bodyLen = BinaryPrimitives.ReadUInt32LittleEndian(header.AsSpan(1, 4));
            uint imgId = BinaryPrimitives.ReadUInt32LittleEndian(header.AsSpan(5, 4));
            Console.WriteLine($"[ReceiveOnePacketAsync] msgType={msgType}, bodyLen={bodyLen}, imgId={imgId}");
            // 3) 바디 버퍼를 "딱 그 크기"로 만들고 채우기
            byte[] body = new byte[bodyLen];
            await ReadExactlyAsync(cppStream, body, (int)bodyLen);

            //// 4) "헤더+바디"를 담을 1개짜리 연속 버퍼 만들기
            //byte[] packet = new byte[9 + bodyLen];
            //Buffer.BlockCopy(header, 0, packet, 0, 9);
            //Buffer.BlockCopy(body, 0, packet, 9, (int)bodyLen);

            // 6) 단 한 번에 전송
            await SendToPythonSingleAsync(pyStream, header, body);

            return (msgType, imgId, body);
        }

        // 파이썬 서버로 중계 
        static async Task SendToPythonSingleAsync(NetworkStream pyStream, byte[] header, byte[] body)
        {
            // 예외처리 헤더가 9바이트인지 확인 
            if (header == null || header.Length != 9)
                throw new ArgumentException("header must be exactly 9 bytes.");

            byte[] packet = new byte[header.Length + body.Length];
            Buffer.BlockCopy(header, 0, packet, 0, 9);
            if (body.Length > 0)
                Buffer.BlockCopy(body, 0, packet, 9, body.Length);
            Console.WriteLine("서버로 패킷 전송 시도");
            await pyStream.WriteAsync(packet, 0, packet.Length);
        }

    }
}
