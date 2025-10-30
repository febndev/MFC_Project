#pragma once
#include <cstdint>

// ======================
// 공통 프로토콜 정의
// ======================

// 메시지 타입(1Byte)
enum class MsgType : uint8_t
{
    IMG_REQ = 1, // 이미지 전송 요청
    IMG_RES = 2, // 이미지 전송 응답
    RESULT_REQ = 3, // 결과 요청
    RESULT_RES = 4, // 결과 응답
    HEARTBEAT = 9  // 연결 유지용
};

// 고정값
constexpr int MAX_JSON_SIZE = 4096;   // 메타 정보 최대 크기
constexpr int MAX_IMAGE_SIZE = 1 * 1024 * 1024; // 최대 1MB 이미지
constexpr int HEADER_SIZE = 9;             // 1 + 4 + 4 = 9바이트

#pragma pack(push, 1)
struct PacketHeader
{
    uint8_t msgType; // 1Byte
    uint32_t bodyLen; // 4Byte
    uint32_t imgId; // 4Byte
};
#pragma pack(pop)