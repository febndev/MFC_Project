import socket
import struct
import threading
import time

HOST = "127.0.0.1"   # C#에서 접속하는 IP (같은 PC면 127.0.0.1)
PORT = 7100          # C#과 반드시 동일 포트로 맞춰주세요

def recvall(conn, n):
    """정확히 n바이트를 받을 때까지 반복해서 recv."""
    data = bytearray()
    while len(data) < n:
        chunk = conn.recv(n - len(data))
        if not chunk:
            return None
        data.extend(chunk)
    return bytes(data)

def handle_client(conn, addr):
    print(f"[+] Connected from {addr}")
    try:
        while True:
            # 1) 4바이트 길이 읽기 (big-endian, unsigned int)
            header = recvall(conn, 4)
            if not header:
                print("[-] Client closed (no header).")
                break

            (length,) = struct.unpack("!I", header)
            if length == 0:
                # 빈 패킷(헬스체크 등)도 허용 — ACK만 응답
                resp = b"ACK"
                conn.sendall(struct.pack("!I", len(resp)) + resp)
                continue

            # 2) 본문 payload 읽기
            payload = recvall(conn, length)
            if payload is None:
                print("[-] Client disconnected mid-payload.")
                break

            # 간단 로그 + 파일로 저장(확인용)
            print(f"[{addr}] received {length} bytes. preview={payload[:32]!r}")
            filename = f"recv_{int(time.time()*1000)}.bin"
            with open(filename, "wb") as f:
                f.write(payload)
            print(f"    saved to {filename}")

            # 3) 길이-프리픽스 형태로 ACK 응답
            resp = b"ACK"
            conn.sendall(struct.pack("!I", len(resp)) + resp)

    except Exception as e:
        print(f"[!] Exception: {e}")
    finally:
        conn.close()
        print(f"[x] Closed {addr}")

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen()
        print(f"Python AI server listening on {HOST}:{PORT}")
        while True:
            conn, addr = s.accept()
            threading.Thread(target=handle_client, args=(conn, addr), daemon=True).start()

if __name__ == "__main__":
    main()
