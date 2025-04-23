#include "CCore.h"
#include"CBoard.h"
#include"CPlayer.h"
#include"CTimer.h"
#include"Server.h"
CCore* CCore::m_pInst = NULL;
bool CCore::m_bLoop = true;
CCore::CCore()
{
}

CCore::~CCore()
{
    CPlayer:: DestroyInst();
    CBoard::DestroyInst();
    Server::DestroyInst();
    ReleaseDC(m_hWnd, m_hDC);
}

bool CCore::Init(HINSTANCE hInst)
{
	m_hInst = hInst;
    MyRegisterClass();
    m_tRS.iW = 1280;
    m_tRS.iH = 720;
    Create();
    m_hDC = GetDC(m_hWnd);
    if (!Server::GetInst()->Init())
        return false;
    if (!CTimer::GetInst()->Init())
        return false;
	return true;
}

int CCore::Run()
{
    MSG msg;

    // 기본 메시지 루프입니다:
    while (Server::m_bLoop)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //윈도우 데드타임일 경우 
        //여기서 기능구현
        else
        {
            CTimer::GetInst()->Update();
            float fDeltaTime = CTimer::GetInst()->GetDeltaTime();
            
            Update(fDeltaTime);
            //Render(fDeltaTime);
        }
        SleepEx(100, true);
    }
    
    return (int)msg.wParam;
	
}

ATOM CCore::MyRegisterClass()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = CCore::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInst;
    wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDB_PNG1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"ChessBoard";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDB_PNG1));

    return RegisterClassExW(&wcex);
}

BOOL CCore::Create()
{
   m_hWnd = CreateWindowW(L"ChessBoard", L"ChessBoard", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInst, nullptr);

    if (!m_hWnd)
    {
        return FALSE;
    }
    
   
    RECT rc = { 0,0,600,600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    //윈도우 영역 크기정하기
    SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left,
        rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return TRUE;
}

void CCore::Input(float fDeltaTime)
{
}

void CCore::Update(float fDeltaTime)
{

    
}

void CCore::Render(float fDeltaTime)
{
    CBoard::GetInst()->Render(m_hDC, 0.f);
    CPlayer::GetInst()->Render(m_hDC, fDeltaTime);
    if (bOnce)
    {
        TCHAR str[100];
        
        if (CPlayer::GetInst()->GetAvatar() != NULL)
        {
            wsprintf(str, TEXT("Avatar x : %d, y : %d | 종료: ESC"), CPlayer::GetInst()->GetAvatar()->x, CPlayer::GetInst()->GetAvatar()->y);
            TextOut(m_hDC, 10, 20, str, lstrlen(str));
        }
        CBoard::GetInst()->Render(m_hDC, 0.f);
        
        bOnce = false;
    }
   
    
}

LRESULT CCore::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        SetTimer(hWnd, 1, 10, NULL);
        break;
    }
    case WM_TIMER:
    {
        InvalidateRect(hWnd, NULL, FALSE);
    }
    case WM_KEYDOWN:
        switch (wParam)
        {
         
        case VK_ESCAPE:
            Server::GetInst()->SendEnd();
            break;
        case VK_RIGHT:
            Server::GetInst()->SetMove( static_cast<char>(MOVETYPE::RIGHT));
            Server::GetInst()->SendMove();
           
         
            
            break;
        case VK_LEFT:
            Server::GetInst()->SetMove(static_cast<char>(MOVETYPE::LEFT));
            Server::GetInst()->SendMove();
         
            break;
        case VK_UP:
            Server::GetInst()->SetMove(static_cast<char>(MOVETYPE::UP));
            Server::GetInst()->SendMove();
           
            break;
        case VK_DOWN:
            Server::GetInst()->SetMove(static_cast<char>(MOVETYPE::DOWN));
            Server::GetInst()->SendMove();
         
            break;
        }
       
        CCore::GetInst()->bOnce = true;
        
       
        break;
   
    case WM_PAINT://그리기
    {
        PAINTSTRUCT ps;
 
        
        //
        //
       // EndPaint(hWnd, &ps);
        /** 더블버퍼링 시작처리입니다. **/
        static HDC hdc, MemDC, tmpDC;
        static HBITMAP BackBit, oldBackBit;
        static RECT bufferRT;
        hdc = BeginPaint(hWnd, &ps);

        GetClientRect(hWnd, &bufferRT);
        MemDC = CreateCompatibleDC(hdc);
        BackBit = CreateCompatibleBitmap(hdc, bufferRT.right, bufferRT.bottom);
        oldBackBit = (HBITMAP)SelectObject(MemDC, BackBit);
        PatBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
        tmpDC = hdc;
        hdc = MemDC;
        MemDC = tmpDC;

        // TODO: 여기에 그리기 코드를 추가합니다.
        CBoard::GetInst()->Render(hdc, 0.f);
        CPlayer::GetInst()->Render(hdc, 0.f);

        /** 더블버퍼링 끝처리 입니다. **/
        tmpDC = hdc;
        hdc = MemDC;
        MemDC = tmpDC;
        GetClientRect(hWnd, &bufferRT);
        BitBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, MemDC, 0, 0, SRCCOPY);
        SelectObject(MemDC, oldBackBit);
        DeleteObject(BackBit);
        DeleteDC(MemDC);
        EndPaint(hWnd, &ps);
        break;

    }
    break;
    case WM_DESTROY:
        
        Server::m_bLoop = false;
        
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
    return LRESULT();
}
