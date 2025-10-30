#pragma once // 헤더 중복방지

#include "CommonProtocol.h"
#include <string>
#include <vector>
#include <atomic>

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else // 리눅스용
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
typedef int SOCKET;
#endif

// ======================
// 패킷 클래스
// ======================
class Packet
{
public:
    Packet();
    ~Packet();

    // 연결
    bool Connect(const std::string& ip, uint16_t port);
    void Disconnect();

    // 송/수신
    bool Send(MsgType type, const std::vector<uint8_t>& body);
    bool Receive(MsgType& outType, uint32_t& outImgId, std::vector<uint8_t>& outBody);

private:
    SOCKET sock; // 소켓
    std::atomic<uint32_t> nextImgId; // 이미지 ID(자동 증가)

    // 송/수신 루프
    bool sendAll(const void* data, size_t size);
    bool recvAll(void* data, size_t size);
};