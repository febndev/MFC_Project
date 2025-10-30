import asyncio
import struct
import numpy as np
import cv2
from ultralytics import YOLO
import os

# 서버 상태
clients = set()
last_result = {}

# 1. AI 모델 로드
try:
    MODEL = YOLO('last.pt')
    print("[AI 모델 로드 완료] last.pt")
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
# 3. AI 추론 함수 (라벨 이미지 저장 포함)
def run_ai_inference(img, canid: int) -> int:
    if MODEL is None:
        print("[오류] AI 모델 미로드, Fail 처리")
        return 1

    results = MODEL(img, conf=0.7, iou=0.7)
    detected = False
    result = 1

    # 결과 이미지 복사
    labeled_img = img.copy()

    for r in results:
        for box in r.boxes:
            cls_id = int(box.cls[0])
            conf = float(box.conf[0])
            detected = True

            # PASS/FAIL 판단
            if cls_id == 1:
                result = 0
                print(f"[AI 추론] ID {canid}: 클래스 {cls_id}(정상) → PASS, 확률={conf:.2f}")
            elif cls_id == 0:
                result = 1
                print(f"[AI 추론] ID {canid}: 클래스 {cls_id}(불량) → FAIL, 확률={conf:.2f}")

            # 박스 그리기
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            color = (0, 255, 0) if cls_id == 1 else (0, 0, 255)
            label_text = f"{'PASS' if cls_id == 1 else 'FAIL'}({cls_id}) {conf:.2f}"
            cv2.rectangle(labeled_img, (x1, y1), (x2, y2), color, 2)
            cv2.putText(labeled_img, label_text, (x1, y1 - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, color, 2)

        if detected:
            break

    # 탐지 없으면 전체 이미지에 FAIL 표시
    if not detected:
        print(f"[AI 추론] ID {canid}: 검출 없음 → FAIL")
        cv2.putText(labeled_img, "FAIL (No Detection)", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)

    # 이미지 저장
    save_dir = "saved_images"
    os.makedirs(save_dir, exist_ok=True)
    save_path = f"{save_dir}/{canid}_{result}.jpg"
    cv2.imwrite(save_path, labeled_img)
    print(f"[이미지 저장] {save_path}")

    # 결과 저장
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

async def handle_type4(writer, canid: int, body: bytes):
    # body가 비어 있어도 수신 확인으로 간주
    if len(body) == 0:
        print(f"[클라이언트 확인] ID {canid}, 헤더만 수신 → 정상 수신 확인")
        return

    # body가 1바이트 있을 경우는 이전 로직 그대로 유지
    if len(body) == 1:
        client_result = struct.unpack('B', body)[0]
        print(f"[클라이언트 확인] ID {canid}, 결과={client_result}")

        # 실패면 동일 이미지 재전송
        if client_result == 1:
            print(f"[재전송 요청] ID {canid}, Type3 재실행")
            await handle_type3(writer, canid)
    else:
        print(f"[경고] Type4 데이터 이상: 길이={len(body)}")

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
