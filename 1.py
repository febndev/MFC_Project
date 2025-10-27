import asyncio
import struct

clients = set()


async def handle_client(reader, writer):
    addr = writer.get_extra_info('peername')
    print(f"[연결됨] {addr}")
    clients.add(writer)

    try:
        while True:
            data = await reader.read(1024)
            if not data:
                break

            if len(data) < 9:
                print(f"[오류] 데이터 길이 부족: {len(data)}")
                continue

            ty = data[0:1].decode()
            glen = struct.unpack('>I', data[1:5])[0]
            canid = struct.unpack('>I', data[5:9])[0]

            print(f"[수신 {addr}] ty={ty}, len={glen}, id={canid}")

            writer.write(f"서버 수신: {ty}, {glen}, {canid}\n".encode())
            await writer.drain()

    except Exception as e:
        print(f"[오류 {addr}] {e}")
    finally:
        print(f"[연결 종료] {addr}")
        clients.remove(writer)
        writer.close()
        await writer.wait_closed()


async def main():
    server = await asyncio.start_server(handle_client, '10.10.21.117', 8888)
    addr = server.sockets[0].getsockname()
    print(f"서버 시작 {addr}")

    async with server:
        await server.serve_forever()  # Ctrl+C 시 KeyboardInterrupt 발생


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n[서버 종료 완료]")
