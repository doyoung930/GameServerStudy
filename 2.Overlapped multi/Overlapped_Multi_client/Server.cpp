#include "Server.h"
#include"CPlayer.h"
#include"CBoard.h"

Server* Server::m_pInst = NULL;
bool Server::m_bLoop = true;
Server::Server() :m_recv_flag(0),m_tcp_option(0)
{
	ZeroMemory(&m_recv_over, sizeof(m_recv_over));
	ZeroMemory(&m_send_over, sizeof(m_send_over));
	//움직임 패킷 초기화
	player_move.size = sizeof(player_move);
	player_move.x = 0;
	player_move.y = 0;

	//시작패킷 초기화
	m_start_packet.size = sizeof(m_start_packet);
	m_start_packet.id = 0;
	
}

Server::~Server()
{
	closesocket(m_s_socket);
	WSACleanup();
}

void Server::InitServerAddr()
{
	int ret;
	ZeroMemory(&m_server_addr, sizeof(m_server_addr));
	m_server_addr.sin_family = AF_INET;
	m_server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &m_server_addr.sin_addr);
	ret = connect(m_s_socket, reinterpret_cast<sockaddr*>(&m_server_addr), sizeof(m_server_addr));
	if (ret == SOCKET_ERROR)
	{
	
		err_display("connect()");

	}
	else
		cout << "연결성공" << endl;
	m_tcp_option = 1;
	setsockopt(m_s_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&m_tcp_option), sizeof(m_tcp_option));

	
}

bool Server::Init()
{
	
	wcout.imbue(locale("korean"));
	cout << "IP주소를 입력해주세요 :";
	cin >> SERVER_ADDR;
	if(WSAStartup(MAKEWORD(2, 2), &m_WsaData)!=0)
		return false;
	m_s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	InitServerAddr();
	ALLRecv();
	return true;
}

bool Server::ALLRecv()
{
	m_rWsaBuf.buf = m_temp_buf;
	m_rWsaBuf.len = BUFSIZE;
	ZeroMemory(&m_recv_over, sizeof(m_recv_over));
	int ret = WSARecv(m_s_socket, &m_rWsaBuf, 1, 0, &m_recv_flag, &m_recv_over, Server::recv_callback);
	if (ret == SOCKET_ERROR)
	{
		int err_num = WSAGetLastError();
		if (WSA_IO_PENDING != err_num) {
			err_display("Recv");
			return false;
		}

	
	}
	return false;
}

bool Server::SendMove()
{
	player_move.id = id;
	m_sWsaBuf.buf = reinterpret_cast<char*>(&player_move);
	m_sWsaBuf.len = player_move.size;
	DWORD sent_byte;
	ZeroMemory(&m_send_over, sizeof(m_send_over));
	int ret = WSASend(m_s_socket,&m_sWsaBuf,1,0,0, &m_send_over, Server::send_callback);
	if (ret == SOCKET_ERROR)
	{
		err_display("send()");
		return false;
	}
	return true;
}



bool Server::SendStart(int id)
{
	m_start_packet.id = id;
	m_sWsaBuf.buf = reinterpret_cast<char*>(&m_start_packet);
	m_sWsaBuf.len = m_start_packet.size;
	DWORD sent_byte;
	ZeroMemory(&m_send_over, sizeof(m_send_over));
	cout << "로그인 시도..." << endl;
	int ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &m_send_over, Server::send_callback);
	if (ret == SOCKET_ERROR)
	{
		err_display("SendStart()");
		return false;
	}
	return true;
	
}

bool Server::SendEnd()
{
	m_end_packet.id =id;
	m_sWsaBuf.buf = reinterpret_cast<char*>(&m_end_packet);
	m_sWsaBuf.len = m_end_packet.size;
	DWORD sent_byte;
	ZeroMemory(&m_send_over, sizeof(m_send_over));
	int ret = WSASend(m_s_socket, &m_sWsaBuf, 1, 0, 0, &m_send_over, Server::send_callback);
	if (ret == SOCKET_ERROR)
	{
		err_display("SendStart()");
		return false;
	}
	return true;

}



void Server::recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	cout << "recvcallback에"<<num_byte<<"왔다" << endl;
	if (m_pInst)
	{
		
		char* p = m_pInst->m_temp_buf;
		while (p < m_pInst->m_temp_buf + num_byte) {
			
			switch (static_cast<PACKET_TYPE>(*(p+1)))
			{
			case PACKET_TYPE::START:
			{
				m_pInst->m_start_packet.id = reinterpret_cast<STARTPACKET*>(p)->id;
				cout << "받은 아이디" << (int)m_pInst-> m_start_packet.id << endl;
				//시작한 플레이어 정보 처리
				if (CPlayer::GetInst()->AddPlayer(m_pInst->m_start_packet.id))
				{
					m_pInst->SendStart(m_pInst->m_start_packet.id);
					m_pInst->id = reinterpret_cast<STARTPACKET*>(p)->id;
					
				}
				p = p + m_pInst->m_start_packet.size;
				cout << "player["<< (int)m_pInst->m_start_packet.id<<"] 로그인" << endl;
				break;
			}
			case PACKET_TYPE::MOVE:
			{
				int id = reinterpret_cast<MOVEPACKET*>(p)->id;
				
				CPlayer::GetInst()->AddPlayer(id);
				CPlayer::GetInst()->SetX(id, reinterpret_cast<MOVEPACKET*>(p)->x);
				CPlayer::GetInst()->SetY(id, reinterpret_cast<MOVEPACKET*>(p)->y);
				cout << "Player[" << id << "] 현재좌표" << " x : " << CPlayer::GetInst()->GetX(id) << " y : " << CPlayer::GetInst()->GetY(id) << endl;
				CPlayer::GetInst()->SetSize(id, CBoard::GetInst()->GetSize(CPlayer::GetInst()->GetY(id), CPlayer::GetInst()->GetX(id)));
				if (id == m_pInst->player_move.id)
				{
					m_pInst->player_move.x = reinterpret_cast<MOVEPACKET*>(p)->x;
					m_pInst->player_move.y = reinterpret_cast<MOVEPACKET*>(p)->y;
					
				}
				p = p + m_pInst->player_move.size;
				cout << "move 처리" << (int)*p << endl;
				break;
			}
			case PACKET_TYPE::END:
			{
				m_pInst->m_end_packet.id = reinterpret_cast<ENDPACKET*>(p)->id;
				
				if (m_pInst->m_end_packet.id == m_pInst->id)
				{
					cout << "로그아웃 합니다." << endl;
					closesocket(m_pInst->m_s_socket);
					WSACleanup();
					Server::m_bLoop = false;
					PostQuitMessage(0);
				}
				else
				{
					CPlayer::GetInst()->DeletePlayer(m_pInst->m_end_packet.id);
					cout << "player[" << (int)m_pInst->m_end_packet.id << "] 로그아웃" << endl;
				}
				p = p + m_pInst->m_end_packet.size;
				break;
			}
				
			}
			
		}
		
		cout << "recvcallback 끝" << endl;
		
		m_pInst->ALLRecv();
	}
	else
	{
		cout << "서버 객체가 없습니다." << endl;
	}
}

void Server::send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{
	cout << "[" << num_byte << "] 바이트 전송" << endl;

}
