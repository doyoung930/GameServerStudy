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

    // ������ ���� ���� �ƴ� �ü�� ���� �ޱ� ���� ����� ����.
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
        // Ŭ���̾�Ʈ ���� ���� ó��
        if (c_socket != INVALID_SOCKET) {
            closesocket(c_socket);
            c_socket = INVALID_SOCKET;
        }
        return;
    }

    if (num_bytes == 0) {
        // Ŭ���̾�Ʈ�� ������ ������
        cout << "Ŭ���̾�Ʈ�� ������ �����߽��ϴ�." << endl;
        if (c_socket != INVALID_SOCKET) {
            closesocket(c_socket);
            c_socket = INVALID_SOCKET;
        }
        return;
    }



    // ������ �����͸� �޾� �÷��̾� �̵�
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
    // Ŭ���̾�Ʈ�κ��� �����͸� ����
    cout << "Player ������ǥ [" << player_move.x << ", " << player_move.y << " ] " << endl;
    // Ŭ���̾�Ʈ���� ������Ʈ�� �÷��̾� ������ ����
    c_wsabuf[0].buf = (char*)&player_move;
    c_wsabuf[0].len = sizeof(player_move);
    memset(&c_over, 0, sizeof(c_over));

    int ret = WSASend(c_socket, c_wsabuf, 1, NULL, 0, &c_over, send_callback);
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        err_display("WSASend");
        // Ŭ���̾�Ʈ ���� ���� ó��
        if (c_socket != INVALID_SOCKET) {
            closesocket(c_socket);
            c_socket = INVALID_SOCKET;
        }
    }
}
