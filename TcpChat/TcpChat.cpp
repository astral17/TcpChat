// TcpChat.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "TcpChat.h"

#define MAX_LOADSTRING 100
#define WM_SOCKET WM_USER + 1

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TCPCHAT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TCPCHAT));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TCPCHAT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TCPCHAT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 515, 380, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HWND hList;
void AddLog(const TCHAR* text)
{
    //SendMessage(hList, LB_RESETCONTENT, 0, 0);
    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)text);
    InvalidateRect(hList, NULL, TRUE);
}

std::vector<SOCKET> clients;
void CreateServer(HWND hWnd)
{
    char buffer[128];
    unsigned short port = 5002;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // TCP socket
    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(port);

    bind(listen_socket, (sockaddr*)&local, sizeof(local));
    WSAAsyncSelect(listen_socket, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
    listen(listen_socket, SOMAXCONN);
    AddLog(TEXT("Server created"));
}

void SendToClients(void* text, size_t size)
{
    for (SOCKET& client : clients)
        send(client, (char*)text, size, 0);
}

SOCKET conn_socket;
void ConnectServer(HWND hWnd)
{
    const char* server_name = "127.0.0.1";
    char buffer[128];
    unsigned short port = 5002;
    WSADATA wsaData;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //fgets(server_name, sizeof(server_name), stdin);

    ULONG addr = inet_addr(server_name);
    HOSTENT* host = addr != INADDR_NONE
        ? gethostbyaddr((char*)&addr, sizeof(ULONG), AF_INET)
        : gethostbyname(server_name);

    // Create a socket
    conn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (conn_socket == INVALID_SOCKET)
    {
        AddLog(TEXT("Client: Error Opening socket: Error!"));
        WSACleanup();
        return;
    }

    // Copy into the sockaddr_in structure
    SOCKADDR_IN server;
    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = *(ULONG*)host->h_addr_list[0];
    server.sin_family = host->h_addrtype;
    server.sin_port = htons(port);

    AddLog(TEXT("connecting 127.0.0.1:5002..."));
    if (connect(conn_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        AddLog(TEXT("Connect failed"));
        WSACleanup();
        return;
    }
    WSAAsyncSelect(conn_socket, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
    AddLog(TEXT("connected!"));
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit, hSend;
    constexpr int kServer = 1;
    constexpr int kClient = 2;
    static int server = 0;
    switch (message)
    {
    case WM_CREATE:
        {
            hList = CreateWindow(TEXT("LISTBOX"), NULL,
                WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
                0, 0, 500, 300,
                hWnd, (HMENU)IDL_LIST, hInst, NULL);
            hEdit = CreateWindow(TEXT("EDIT"), TEXT(""),
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
                0, 300, 420, 20,
                hWnd, (HMENU)IDE_TEXT, hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("send"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                420, 300, 80, 20,
                hWnd, (HMENU)IDB_SEND, hInst, NULL);
        }
        break;
    case WM_SOCKET:
        if (WSAGETSELECTERROR(lParam))
        {
            // Display the error and close the socket
            closesocket((SOCKET)wParam);
            break;
        }
        // Determine what event occurred on the socket
        switch (WSAGETSELECTEVENT(lParam))
        {
        case FD_ACCEPT:
            {
                AddLog(TEXT("Accept!"));
                // Accept an incoming connection
                SOCKET accept_socket = accept(wParam, NULL, NULL);
                // Prepare accepted socket for read, write, and close notification
                WSAAsyncSelect(accept_socket, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
                clients.push_back(accept_socket);
            }
            break;
        case FD_READ:
            {
                // Receive data from the socket in wParam
                //AddLog(TEXT("read!"));
                TCHAR buffer[1024];
                recv(wParam, (char*)buffer, sizeof(buffer), 0);
                AddLog(buffer);
                if (server == kServer)
                    SendToClients(buffer, sizeof(buffer));
            }
            break;
        case FD_WRITE:
            {
                // The socket in wParam is ready for sending data
                AddLog(TEXT("write!"));
            }
            break;
        case FD_CLOSE:
            // The connection is now closed
            AddLog(TEXT("close!"));
            closesocket((SOCKET)wParam);
            break;
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_CREATE:
                if (!server)
                {
                    server = kServer;
                    CreateServer(hWnd);
                }
                else
                    MessageBox(NULL, TEXT("Already created or connected!"), TEXT("Error!"), MB_OK | MB_ICONERROR);
                break;
            case IDM_CONNECT:
                if (!server)
                {
                    server = kClient;
                    ConnectServer(hWnd);
                }
                else
                    MessageBox(NULL, TEXT("Already created or connected!"), TEXT("Error!"), MB_OK | MB_ICONERROR);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDB_SEND:
                {
                    TCHAR buffer[128];
                    GetWindowText(hEdit, buffer, 128);
                    SetWindowText(hEdit, TEXT(""));
                    if (server == kServer)
                    {
                        AddLog(buffer);
                        SendToClients(buffer, sizeof(buffer));
                    }
                    else if (server == kClient)
                    {
                        send(conn_socket, (char*)buffer, sizeof(buffer), 0);
                    }
                    else
                        MessageBox(NULL, TEXT("No connection..."), TEXT("Error!"), MB_OK | MB_ICONERROR);
                }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
