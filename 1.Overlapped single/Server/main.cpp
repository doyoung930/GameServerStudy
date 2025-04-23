#include "header.h"

MOVEPACKET player_move;

SOCKET c_socket;
WSAOVERLAPPED c_over;
WSABUF c_wsabuf[1];

void do_recv();
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);

int main()
{
    wcout.imbue(std::locale("korean"));

    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
        return false;

    int ret;

    // 접속을 위한 것이 아닌 운영체제 에서 받기 위해 만드는 것임.
    SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN server_addr;
    memset(&server_addr, 0, sizeof(server_addr));


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

    ret = bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    if (ret == SOCKET_ERROR)
    {
        err_display("bind()");
        return false;
    }

    ret = listen(s_socket, SOMAXCONN);
    if (ret == SOCKET_ERROR)
    {
        err_display("listen()");
        return false;
    }

    SOCKADDR_IN cl_addr;
    INT addr_size = sizeof(cl_addr);

    c_socket = accept(s_socket, reinterpret_cast<sockaddr*>(&cl_addr), &addr_size);
    //c_socket = AcceptEx();
    if (c_socket == INVALID_SOCKET)
    {
        err_display("accept()");
        return false;
    }
    closesocket(s_socket);
    do_recv();

    while (true) SleepEx(100, true);
    closesocket(c_socket);
    WSACleanup;
}

void do_recv()
{

    c_wsabuf[0].buf = reinterpret_cast<char*>(&player_move);
    c_wsabuf[0].len = sizeof(player_move);

    DWORD recv_flag = 0;
    memset(&c_over, 0, sizeof(c_over));
    int ret = WSARecv(c_socket, c_wsabuf, 1, 0, &recv_flag, &c_over, recv_callback);
    if (ret == SOCKET_ERROR) err_display("Recv");


}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
    do_recv();
}
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags) {
    if (err != 0) {
        err_display("recv_callback()");
        // 클라이언트 연결 종료 처리
        if (c_socket != INVALID_SOCKET) {
            closesocket(c_socket);
            c_socket = INVALID_SOCKET;
        }
        return;
    }

    if (num_bytes == 0) {
        // 클라이언트가 연결을 종료함
        cout << "클라이언트가 연결을 종료했습니다." << endl;
        if (c_socket != INVALID_SOCKET) {
            closesocket(c_socket);
            c_socket = INVALID_SOCKET;
        }
        return;
    }



    // 소켓의 데이터를 받아 플레이어 이동
    if (player_move.type == static_cast<char>(MOVETYPE::LEFT)) {
        if (0 < player_move.x) {
            player_move.x--;
        }
    }
    if (player_move.type == static_cast<char>(MOVETYPE::RIGHT)) {
        if (7 > player_move.x) {
            player_move.x++;
        }
    }
    if (player_move.type == static_cast<char>(MOVETYPE::UP)) {
        if (0 < player_move.y) {
            player_move.y--;
        }
    }
    if (player_move.type == static_cast<char>(MOVETYPE::DOWN)) {
        if (7 > player_move.y) {
            player_move.y++;
        }
    }
    // 클라이언트로부터 데이터를 받음
    cout << "Player 현재좌표 [" << player_move.x << ", " << player_move.y << " ] " << endl;
    // 클라이언트에게 업데이트된 플레이어 정보를 보냄
    c_wsabuf[0].buf = (char*)&player_move;
    c_wsabuf[0].len = sizeof(player_move);
    memset(&c_over, 0, sizeof(c_over));

    int ret = WSASend(c_socket, c_wsabuf, 1, NULL, 0, &c_over, send_callback);
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        err_display("WSASend");
        // 클라이언트 연결 종료 처리
        if (c_socket != INVALID_SOCKET) {
            closesocket(c_socket);
            c_socket = INVALID_SOCKET;
        }
    }
}
