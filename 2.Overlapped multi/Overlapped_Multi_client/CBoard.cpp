#include "CBoard.h"
CBoard* CBoard::m_pInst = NULL;
CBoard::CBoard():x(0),y(0)
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            m_Tile[i][j] = { 100 + j * 50, 100 + i * 50, 150 + j * 50, 150 + i * 50 };
        }
    }
}

CBoard::~CBoard()
{
}

void CBoard::Render(HDC hdc,float fDeltaTime)
{
    //HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
    //
    //HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (i % 2 == 0 )
            {
                if (j % 2 == 0)
                {
                    HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));

                    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
                    Rectangle(hdc, m_Tile[i][j].left, m_Tile[i][j].top, m_Tile[i][j].right, m_Tile[i][j].bottom);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(myBrush);
                }
                else
                    Rectangle(hdc, m_Tile[i][j].left, m_Tile[i][j].top, m_Tile[i][j].right, m_Tile[i][j].bottom);
            }
            else
            {
                if (j % 2 == 1)
                {
                    HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));

                    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
                    Rectangle(hdc, m_Tile[i][j].left, m_Tile[i][j].top, m_Tile[i][j].right, m_Tile[i][j].bottom);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(myBrush);
                }
                else
                    Rectangle(hdc, m_Tile[i][j].left, m_Tile[i][j].top, m_Tile[i][j].right, m_Tile[i][j].bottom);
            }
        }
    }
}
