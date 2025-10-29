import asyncio
import struct
import numpy as np
import cv2
from ultralytics import YOLO

# 서버 상태
clients = set()
last_result = {}

# 1. AI 모델 로드
try:
    MODEL = YOLO('best.pt')
    print("[AI 모델 로드 완료] best.pt")
except Exception as e:
    print(f"[ AI 모델 로드 오류] {e}")
    MODEL = None

# 2. 메시지 전송 함수
async def send_message(writer, ty: int, canid: int, body: bytes):
    header = struct.pack('<BII', ty, len(body), canid)
    writer.write(header + body)
    await writer.drain()
    print(f"[송신] Type {ty}, ID {canid}, BodyLen {len(body)}")

# 3. AI 추론 함수
def run_ai_inference(img, canid: int) -> int:
    if MODEL is None:
        print("[오류] AI 모델 미로드, Fail 처리")
        return 1

    results = MODEL(img, conf=0.25, iou=0.7)
    detected = False
    result = 1

    for r in results:
        for box in r.boxes:
            cls_id = int(box.cls[0])
            detected = True

            if cls_id == 1:
                result = 0  # 클래스 1이면 PASS
                print(f"[AI 추론] ID {canid}: 클래스 {cls_id}(정상) 검출 → PASS")
                break

            elif cls_id == 0:
                result = 1  # 클래스 0이면 FAIL
                print(f"[AI 추론] ID {canid}: 클래스 {cls_id}(불량) 검출 → FAIL")
                break

        if detected:
            break

    if not detected:
        print(f"[AI 추론] ID {canid}: 검출 없음 → FAIL")

    last_result[canid] = result
    return result

# 4. Type1 처리: 이미지 수신
async def handle_type1(reader, writer, canid: int, body: bytes):
    # 1) 이미지 디코딩
    np_arr = np.frombuffer(body, np.uint8)
    img = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)

    if img is None:
        print(f"[오류] ID {canid}: 이미지 디코딩 실패")
        result = 1
    else:
        result = run_ai_inference(img, canid)

    # 2) Type2 응답 전송 (수신 확인, 결과 없음)
    await send_message(writer, ty=2, canid=canid, body=b'')
    return result

# 5. Type3 처리: 결과 전송
async def handle_type3(writer, canid: int):
    # AI 결과 존재 여부 확인
    result = last_result.get(canid, 1)  # 없으면 Fail
    await send_message(writer, ty=3, canid=canid, body=struct.pack('B', result))
    print(f"[결과 전송] ID {canid}, 결과={result}")

# 6. Type4 처리: 클라이언트 확인 결과
async def handle_type4(writer, canid: int, body: bytes):
    if len(body) != 1:
        print(f"[오류] Type4 바디 길이 오류: {len(body)}")
        return

    client_result = struct.unpack('B', body)[0]
    print(f"[클라이언트 확인] ID {canid}, 결과={client_result}")

    # 실패면 동일 이미지 재전송
    if client_result == 1:
        print(f"[재전송 요청] ID {canid}, Type3 재실행")
        await handle_type3(writer, canid)

# 7. 클라이언트 처리
async def handle_client(reader, writer):
    addr = writer.get_extra_info('peername')
    print(f"[연결됨] {addr}")
    # canid = 1
    # last_result[canid] = 0
    # await handle_type3(writer, canid)
    try:
        while True:
            # 1) 헤더 읽기
            try:
                header = await reader.readexactly(9)
            except asyncio.IncompleteReadError:
                print(f"[클라이언트 종료] {addr}")
                break

            ty, glen, canid = struct.unpack('<BII', header)
            body = b''
            if glen > 0:
                try:
                    body = await reader.readexactly(glen)
                except asyncio.IncompleteReadError:
                    print(f"[오류] ID {canid}, Body 읽기 실패")
                    break

            # 2) Type별 처리
            if ty == 1:
                await handle_type1(reader, writer, canid, body)
                # 추론 끝나면 Type3 실행
                await handle_type3(writer, canid)

            elif ty == 4:
                await handle_type4(writer, canid, body)

            else:
                print(f"[경고] 알 수 없는 Type {ty} 수신")

    except (ConnectionResetError, OSError) as e:
        print(f"[연결 오류] {addr}: {e}")

    finally:
        if writer in clients:
            clients.remove(writer)
        try:
            writer.close()
            await writer.wait_closed()
        except Exception as e:
            print(f"[writer 종료 예외] {addr}: {e}")
        print(f"[연결 종료] {addr}")

# 8. 서버 메인
async def main():
    host = '10.10.21.117'
    port = 8888
    server = await asyncio.start_server(handle_client, host, port)
    addr = server.sockets[0].getsockname()
    print(f"[서버 시작] {addr}")
    async with server:
        await server.serve_forever()

# 9. 실행
if __name__ == "__main__":
    try:
        if MODEL is None:
            print("[ 경고] AI 모델 로드 실패")
        asyncio.run(main())
    except KeyboardInterrupt:
        print("[서버 수동 종료 완료]")
