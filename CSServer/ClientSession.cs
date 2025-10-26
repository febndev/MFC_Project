using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CSServer
{
    internal class ClientSession
    {
        private readonly TcpClient mClient;

        public ClientSession(TcpClient client)
        {
            mClient = client;
        }

        public void ManageClientAsync()
        {
            NetworkStream? stream = null;
            StreamReader? reader = null;
            try
            {
                stream = mClient.GetStream();
                reader = new StreamReader(stream);

                // buffer 정의 
                // 그 밑에 읽어내는 로직 작성 필요. 
            }
            catch (Exception ex)
            {
                throw;
            }

        }
        private static async Task<byte[]?> ReadExactAsync(NetworkStream stream, int count)
        {
            var buf = new byte[count];
            int off = 0;
            while (off < count)
            {
                int n = await stream.ReadAsync(buf, off, count - off);
                if (n == 0)
                {
                    return null;
                }
                off += n;
            }
            return buf;
        }
    }
}
