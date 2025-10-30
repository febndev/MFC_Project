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

                var cppToPy = PacketRelayLoop("Cpp to Py", cppStream, pyStream);
                var pyToCpp = PacketRelayLoop("Py to Cpp", pyStream, cppStream);

                // 한쪽 종료/에러 감지만 함.
                //await Task.WhenAny(cppToPy, pyToCpp);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"[ClientSession] 오류: {ex.Message}");
                throw;
            }

        }

        // 매개변수 만큼 읽어내는 메서드 
        public static async Task ReadExactlyAsync(NetworkStream stream, byte[] buf, int count)
        {
            int off = 0;
            while (off < count)
            {
                int n = await stream.ReadAsync(buf, off, count - off);
                if (n == 0) throw new EndOfStreamException("remote closed during read");

                off += n;
            }
        }

        // 패킷 분석해서 상대방으로 보내는 것 까지 
        private async Task PacketRelayLoop(string tag, NetworkStream stream, NetworkStream dstStream)
        {
            // 1) 헤더 9바이트
            byte[] header = new byte[9];
            try
            {
                while (true)
                {
                    await ReadExactlyAsync(stream, header, 9);

                    // 2) 파싱 (바이너리 데이터 파싱, 빅엔디안->리틀엔디안으로 변경 )
                    int msgType = header[0];
                    uint bodyLen = BinaryPrimitives.ReadUInt32LittleEndian(header.AsSpan(1, 4));
                    // 바디 상한 + 안전 캐스팅, 이것도 자꾸 강요해서 주석으로 일단 넣음. 
                    //const int MaxBodyLen = 10 * 1024 * 1024;
                    //if (bodyLen > MaxBodyLen) throw new InvalidDataException("Body too large");
                    int bodySize = checked((int)bodyLen);
                    uint imgId = BinaryPrimitives.ReadUInt32LittleEndian(header.AsSpan(5, 4));
                    Console.WriteLine($"[ReceiveOnePacketAsync] msgType={msgType}, bodyLen={bodyLen}, imgId={imgId}");

                    byte[] body = bodySize == 0 ? Array.Empty<byte>() : new byte[bodySize];
                    if (bodySize > 0)
                        await ReadExactlyAsync(stream, body, bodySize);

                    // 3) msgType에 따른 분기처리
                    switch ((MsgType)msgType) // 메시지 열거형으로 캐스팅 후 분기
                    {
                        case MsgType.Image:
                            Console.WriteLine($"{tag} type={msgType} len={bodyLen} id={imgId}");
                        
                            // await SendToDestinationAsync(dstStream, header, body);
                            // 파일로 서버 컴퓨터에 저장 

                            // 1) 목적지로 그대로 중계
                            await SendToDestinationAsync(dstStream, header, body);

                            // 2) 디스크에 저장 (폴더 자동 생성)
                            try
                            {   // 이미지 저장 로직 , 경로생성 
                                var dir = Path.Combine(AppContext.BaseDirectory, "recv_img");
                                Directory.CreateDirectory(dir); // 없으면 생성
                                var ext = ".jpg";               // JPG/PNG라면 ".jpg" 또는 ".png"로 바꿔도 됨, 원래는 ".bin"
                                var path = Path.Combine(dir, $"{imgId}_{DateTime.Now:yyyyMMdd_HHmmssfff}{ext}");

                                await File.WriteAllBytesAsync(path, body);
                                Console.WriteLine($"[SAVE] {path} ({body.Length} bytes)");
                                await Db.Global.InsertImageAsync((int)imgId, path);
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine($"[SAVE] 실패: {e.Message}");
                            }

                            await SendAckAsync(stream, MsgType.ImageReceive, imgId);
                            break;

                        case MsgType.ImageReceive:
                            Console.WriteLine($"{tag} type={msgType} len={bodyLen} id={imgId}");
                            break;

                        case MsgType.Result:
                            Console.WriteLine($"{tag} type={msgType} len={bodyLen} id={imgId}");
                            // C++ 클라로 패킷 중계 
                            await SendToDestinationAsync(dstStream, header, body);
                            // Py 서버로 잘 받았다 보내기 
                            await SendAckAsync(stream, MsgType.ResultReceive, imgId);
                            // DB에 결과 업데이트
                            int intImgId = (int)imgId;
                            int intBody = body[0];
                            bool result = false;
                            if (intBody == 0)

                            {
                                result = true;
                            }
                            else if (intBody == 1)
                            {
                                result = false;
                            }
                                await Db.Global.UpdateResultAsync(intImgId, result);
                            break;

                        case MsgType.ResultReceive:
                            Console.WriteLine($"{tag} type={msgType} len={bodyLen} id={imgId}");
                            break;

                        default:
                            Console.WriteLine($"{tag} type={msgType} len={bodyLen} id={imgId}");
                            // 분류되지 않은 MSGTYPE 도 일단 목적지까지 전달은 함. 
                            await SendToDestinationAsync(dstStream, header, body);
                            break;

                    }
                }
            }
            // 예외처리 

            catch (OperationCanceledException)
            {
                Console.WriteLine($"[{tag}] 취소됨 (앱/세션 종료 요청).");
            }
            catch (InvalidDataException ex)
            {
                Console.WriteLine($"[{tag}] 잘못된 패킷: {ex.Message}");
            }
            catch (IOException ex) // 읽기/쓰기 중 소켓 끊김 등
            {
                // NetworkStream 예외는 대부분 IOException으로 오고,
                // 내부에 SocketException이 들어있는 경우가 많아요.
                if (ex.InnerException is SocketException se)
                {
                    Console.WriteLine($"[{tag}] 소켓 오류({se.SocketErrorCode}): {se.Message}");
                }
                else
                {
                    Console.WriteLine($"[{tag}] I/O 오류: {ex.Message}");
                }
            }
            catch (ObjectDisposedException)
            {
                Console.WriteLine($"[{tag}] 스트림이 이미 닫혔습니다.");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"[{tag}] 예상치 못한 오류: {ex}");
            }

        }


        // 목적지로 중계 
        static async Task SendToDestinationAsync(NetworkStream dstStream, byte[] header, byte[] body)
        {
            // 예외처리 헤더가 9바이트인지 확인 
            if (header == null || header.Length != 9)
                throw new ArgumentException("header must be exactly 9 bytes.");

            byte[] packet = new byte[header.Length + body.Length];
            Buffer.BlockCopy(header, 0, packet, 0, 9);
            if (body.Length > 0)
                Buffer.BlockCopy(body, 0, packet, 9, body.Length);
            Console.WriteLine("서버로 패킷 전송 시도");
            await dstStream.WriteAsync(packet, 0, packet.Length);
        }

        // 응답 보내기 함수
        private static async Task SendAckAsync(NetworkStream stream, MsgType ackType, uint imgId)
        {
            byte[] hdr = new byte[9];
            if (ackType == MsgType.ImageReceive)
            {
                hdr[0] = (byte)2;
            } else if (ackType == MsgType.ResultReceive)
            {
                hdr[0] = (byte)4;
            }
                // hdr[0] = (byte)ackType;
            BinaryPrimitives.WriteUInt32LittleEndian(hdr.AsSpan(1, 4), 0); // bodylen=0
            BinaryPrimitives.WriteUInt32LittleEndian(hdr.AsSpan(5, 4), imgId);
            await stream.WriteAsync(hdr, 0, hdr.Length);
            Console.WriteLine($"잘받았다 응답 {ackType}전송"); // 
        }
    }

    public enum MsgType : byte
    {
        Image = 1, // 이미지 전송
        ImageReceive = 2, // 이미지 잘 받았다
        Result = 3, // 검출결과
        ResultReceive = 4, // 결과 잘 받았다
    }

}
