#include "CPlayer.h"
#include"CBoard.h"
CPlayer* CPlayer::m_pInst = NULL;
CPlayer::CPlayer()
{
	
}

CPlayer::~CPlayer()
{
	DestroyAllPlayers();
}



void CPlayer::Render(HDC hdc, float fDeltaTime)
{
	for (auto& player : m_players)
	{
		if (player.second->myid != 0)
		{
			HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 100, 100));

			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
			wsprintf(player.second->str, TEXT("Avatar "));
			
			Ellipse(hdc, player.second->m_size.left, 
				player.second->m_size.top, player.second->m_size.right, player.second->m_size.bottom);
			TextOut(hdc, (player.second->m_size.right + player.second->m_size.left) / 2,
				(player.second->m_size.top+ player.second->m_size.bottom) / 2, player.second->str, lstrlen(player.second->str));
			SelectObject(hdc, oldBrush);
			DeleteObject(myBrush);
		}
		else
		{
			short rc = 100 + (10 * player.first);
			HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(rc, 0, rc));

			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);

			wsprintf(player.second->str, TEXT("pc:%d "), player.second->id);
			
			Ellipse(hdc, player.second->m_size.left,
				player.second->m_size.top, player.second->m_size.right, player.second->m_size.bottom);
			TextOut(hdc, (player.second->m_size.right + player.second->m_size.left) / 2,
				(player.second->m_size.top + player.second->m_size.bottom) / 2, player.second->str, lstrlen(player.second->str));
			SelectObject(hdc, oldBrush);
			DeleteObject(myBrush);
		}
	}
}
