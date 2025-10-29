#include "pch.h"
#include "Packet.h"
#include <iostream>

// 생성자, 소멸자
Packet::Packet() : sock(INVALID_SOCKET), nextImgId(1) {}
Packet::~Packet() { Disconnect(); }

// 연결 메서드
bool Packet::Connect(const std::string& ip, uint16_t port)
{
#ifdef _WIN32 // 윈도우 전용 초기화
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "[ERROR] 소켓 생성 실패됨\n";
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[ERROR] 연결 실패됨\n";
        Disconnect();
        return false;
    }

    std::cout << "[INFO] 연결됨 => " << ip << ":" << port << "\n";
    return true;
}

// 종료 메서드
void Packet::Disconnect()
{
    if (sock != INVALID_SOCKET) {

#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif

        sock = INVALID_SOCKET;
        std::cout << "[INFO] 연결해제됨\n";
    }
}

// 데이터 수/송신 루프(모든 데이터 받기)
bool Packet::sendAll(const void* data, size_t size)
{
    const char* ptr = reinterpret_cast<const char*>(data);
    size_t total = 0;
    while (total < size)
    {
        int sent = send(sock, ptr + total, static_cast<int>(size - total), 0);
        if (sent <= 0)
            return false;
        total += sent;
    }
    return true;
}
bool Packet::recvAll(void* data, size_t size)
{
    char* ptr = reinterpret_cast<char*>(data);
    size_t total = 0;
    while (total < size)
    {
        int r = recv(sock, ptr + total, static_cast<int>(size - total), 0);
        if (r <= 0)
            return false;
        total += r;
    }
    return true;
}

// 데이터 전송 메서드
bool Packet::Send(MsgType type, const std::vector<uint8_t>& body)
{
    if (sock == INVALID_SOCKET)
        return false;

    PacketHeader header{};
    header.msgType = static_cast<uint8_t>(type);
    header.bodyLen = static_cast<uint32_t>(body.size());
    header.imgId = nextImgId.fetch_add(1); // 자동 1씩 증가

    // ---- Step 1: Header(9Byte)
    if (!sendAll(&header, sizeof(header)))
        return false;

    // ---- Step 2: Body(이미지 or 결과)
    if (header.bodyLen > 0)
        if (!sendAll(body.data(), body.size()))
            return false;

    std::cout << "[SEND] Header: "
        << "MsgType=" << (int)header.msgType
        << ", BodyLen=" << header.bodyLen
        << ", ImgId=" << header.imgId << "\n";
    return true;
}

bool Packet::Receive(MsgType& outType, uint32_t& outImgId, std::vector<uint8_t>& outBody)
{
    outBody.clear();

    // ---- Step 1: Header
    PacketHeader header{};
    if (!recvAll(&header, sizeof(header)))
        return false;
    outType = static_cast<MsgType>(header.msgType);
    outImgId = header.imgId;

    // ---- Step 2: Body
    if (header.bodyLen > 0)
    {
        outBody.resize(header.bodyLen);
        if (!recvAll(outBody.data(), header.bodyLen))
            return false;
    }

    std::cout
        << "[RECV] MsgType=" << (int)header.msgType
        << ", BodyLen=" << header.bodyLen
        << ", ImgId=" << header.imgId << "\n";
    return true;
}
