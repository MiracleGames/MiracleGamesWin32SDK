// CppApp.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "CppApp.h"
#include <nlohmann/json.hpp>
#include <objbase.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//MG SDK相关的全局变量
HINSTANCE g_hDLL = NULL;
HWND g_hwndLog = NULL;
HWND g_hwndMain = NULL;
bool g_comInitialized = false;

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.
    // 初始化 COM 为 STA
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        g_comInitialized = true;
    }
    else {
        MessageBox(nullptr, L"COM初始化失败", L"错误", MB_ICONERROR);
        return FALSE;
    }

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CPPAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 清理 COM
    if (g_comInitialized) {
        CoUninitialize();
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPAPP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CPPAPP);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    g_hwndMain = hWnd;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

#pragma region //参数定义
// App Log
void AppendLog(const wchar_t* format, ...) {
    if (!g_hwndLog) return;

    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, format, args);
    va_end(args);

    int len = GetWindowTextLength(g_hwndLog);
    SendMessage(g_hwndLog, EM_SETSEL, len, len);
    SendMessage(g_hwndLog, EM_REPLACESEL, FALSE, (LPARAM)buffer);
    SendMessage(g_hwndLog, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}

HWND g_hPnlCmp = NULL;//CMP控件，启动时在程序的最上面，宽度铺满，高度50
HWND g_hPnlSplashScreen = NULL;//开屏(全屏)
HWND g_hPnlBanner = NULL;
HWND g_hPnlCoupletLeft = NULL;
HWND g_hPnlCoupletRight = NULL;
HWND g_hPnlInterstitial = NULL;//插屏  
HWND g_hPnlReward = NULL;//激励视频  
HWND g_hBtnPreloadAdShow = NULL;

// 定义自定义消息
#define WM_SHOW_ADVERT (WM_USER + 300)
#define WM_SHOW_OPENSCREEN_ADVERT (WM_USER + 301)
#define WM_DESTROY_CMP (WM_USER + 310)
#define WM_DESTROY_ADVERT (WM_USER + 311) 

int g_cmpChangedWidth = 0;
int g_cmpChangedHeight = 0;
int g_cmpSdkControlHandle = 0;

const char* YourAppKey = "8647026706";
const char* MgGoodsKey = "952D6F5C23C9065";                        //MG商品标识
const char* MgCallbackId = "14df40e9-f566-11ed-8b41-263ab2cdee42"; //支付回调Id
const char* SplashAdKey = "E4F8E422D3";                      //开屏:1920 x 1080
const char* ExitAdKey = "6A1FD1D202";                              //退屏:1920 x 1080
const char* BannerAdKey = "0B2D9717A1";                            //横幅:728 x 90
const char* InterstitialAdKey = "C6E76462AF";                      //插屏:640 x 640
const char* FullScreenInterstitialAdKey = "72F76D95C0";            //全屏插播:768 x 432 
const char* CoupletAdKey = "3427B3ED71";                           //对联:300 x 600
const char* RewardedAdKey = "B6B030D76C";                            //激励广告:768 x 432 
#pragma endregion

#pragma region //创建广告控件
void CreateSplashScreenAdPanel(HWND hWnd) {
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = rect.right - rect.left;
        int panelHeight = rect.bottom - rect.top;
        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlSplashScreen = CreateWindowW(L"STATIC", L"This is a panel for Open Screen advertising", WS_CHILD | WS_VISIBLE, 0, 0, panelWidth, panelHeight, hWnd, (HMENU)2011, minstance, NULL);
        BringWindowToTop(g_hPnlSplashScreen);
    }
}

void CreateBannerAdPanel(HWND hWnd) {
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 728;
        int panelHeight = 90; // Banner广告尺寸 728x90
        int topMargin = (rect.bottom - rect.top - panelHeight - 20);
        int leftMargin = (rect.right - rect.left - panelWidth) / 2;

        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        // 创建Panel的窗口句柄
        g_hPnlBanner = CreateWindowW(L"STATIC", L"This is a panel for banner advertising", WS_CHILD | WS_VISIBLE, leftMargin, topMargin, 728, 90, hWnd, (HMENU)2031, minstance, NULL);
    }
}

void CreateInterstitialAdPannel(HWND hWnd) {
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 640;
        int panelHeight = 640;
        int topMargin = (rect.bottom - rect.top - panelHeight) / 2;
        int leftMargin = (rect.right - rect.left - panelWidth) / 2;

        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlInterstitial = CreateWindowW(L"STATIC", L"This is a panel for Interstitial advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2041, minstance, NULL);
        BringWindowToTop(g_hPnlInterstitial);
    }
}

void CreateCoupletAdPannel(HWND hWnd) {
    HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 300;
        int panelHeight = 600; // 对联广告尺寸 300x600
        int topMargin = (rect.bottom - rect.top - panelHeight - 50);
        int leftMargin = (rect.right - rect.left - panelWidth);

        g_hPnlCoupletLeft = CreateWindowW(L"STATIC", L"This is a panel for CoupletLeft advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 0, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2051, minstance, NULL);
        //SetWindowPos(g_hPnlCoupletLeft, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        BringWindowToTop(g_hPnlCoupletLeft);
        g_hPnlCoupletRight = CreateWindowW(L"STATIC", L"This is a panel for CoupletRight advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2052, minstance, NULL);
        //SetWindowPos(g_hPnlCoupletRight, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        BringWindowToTop(g_hPnlCoupletRight);
    }
}

void CreateRewardAdPannel(HWND hWnd) {
    RECT rect;
    if (GetClientRect(hWnd, &rect))
    {
        int panelWidth = 768;
        int panelHeight = 432;
        int topMargin = (rect.bottom - rect.top - panelHeight) / 2;
        int leftMargin = (rect.right - rect.left - panelWidth) / 2;

        HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        g_hPnlReward = CreateWindowW(L"STATIC", L"This is a panel for Reward advertising", WS_TABSTOP | WS_CHILD | WS_VISIBLE, leftMargin, topMargin, panelWidth, panelHeight, hWnd, (HMENU)2061, minstance, NULL);
        BringWindowToTop(g_hPnlReward);
    }
}

void CreateControls(HWND hWnd) {
    int y = 10;
    CreateWindowW(L"BUTTON", L"登录", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y, 100, 30, hWnd, (HMENU)ID_BTN_LOGIN, hInst, NULL);
    CreateWindowW(L"BUTTON", L"MG支付", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_MGPAY, hInst, NULL);
    CreateWindowW(L"BUTTON", L"微软支付", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_MSPAY, hInst, NULL);
    CreateWindowW(L"BUTTON", L"微软好评", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_RATING, hInst, NULL);

    CreateWindowW(L"BUTTON", L"插屏", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD1, hInst, NULL);
    CreateWindowW(L"BUTTON", L"Banner", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD2, hInst, NULL);
    CreateWindowW(L"BUTTON", L"对联", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD3, hInst, NULL);
    CreateWindowW(L"BUTTON", L"全屏插播", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD4, hInst, NULL);
    CreateWindowW(L"BUTTON", L"激励视频", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_AD7, hInst, NULL);

    CreateWindowW(L"BUTTON", L"关闭CMP", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_CLOSECMP, hInst, NULL);

    CreateWindowW(L"BUTTON", L"Ad Preload", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_ADPRELOAD10, hInst, NULL);
    g_hBtnPreloadAdShow = CreateWindowW(L"BUTTON", L"Ad Show", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, y += 35, 100, 30, hWnd, (HMENU)ID_BTN_ADPRELOAD11, hInst, NULL);
    EnableWindow(g_hBtnPreloadAdShow, FALSE);

    // Log
    g_hwndLog = CreateWindow(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        320, 10, 700, 270, hWnd, (HMENU)ID_TXT_LOG, hInst, NULL);
}
#pragma endregion

// 注册回调函数
void RegisterCallbackEvent(HINSTANCE hdll) {
    // Register the CMP callback event
    //if (auto func = (CmpSizeChangedEvent)GetProcAddress(hdll, "CmpSizeChangedEvent"))
    //    func(onCmpSizeChangedEvent);
    if (auto func = (CmpClosedEvent)GetProcAddress(hdll, "CmpClosedEvent")) //CMP已关闭
        func(onCmpClosedEvent);
    // Register the initialisation completion callback event
    if (auto func = (InitCompleteEvent)GetProcAddress(hdll, "InitCompleteEvents")) // 初始化完成后的回调函数
        func(onInitCompleteEvent);
    if (auto func = (FireCloseAdvertEvents)GetProcAddress(hdll, "FireCloseAdvertEvents")) //广告关闭事件的回调函数
        func(onFireCloseAdvertEvent);
    if (auto func = (PreloadAdvertEvents)GetProcAddress(hdll, "PreloadAdvertEvents")) //广告预缓存的回调函数
        func(onPreloadAdvertEvent);
    if (auto func = (LoginCompleteEvent)GetProcAddress(hdll, "LoginCompleteEvents")) // 登录成功后的回调函数
        func(onLoginCompleteEvent);
    if (auto func = (AssetChangedJsonEvent)GetProcAddress(hdll, "AssetChangedJsonEvent")) // 客户端资产变更事件回调函数
        func(onAssetsChangedEvent);
    if (auto func = (PaymentCompleteEvent)GetProcAddress(hdll, "PaymentCompleteEvent")) // 支付完成事件的回调函数
        func(onPaymentCompleteEvent);
    if (auto func = (IsRatingReviewEvents)GetProcAddress(hdll, "IsRatingReviewEvents")) // 判断用户是否进行过评论的回调函数
        func(onIsRatingReviewEvent);
    if (auto func = (ShowRatingReviewEvents)GetProcAddress(hdll, "ShowRatingReviewEvents")) // 接收评论结果的回调函数
        func(onShowRatingReviewEvent);

    if (auto func = (PushUriEvent)GetProcAddress(hdll, "PushUriEvent")) // 获取推送Uri的回调函数
        func(onPushUriEvent);
    if (auto func = (AntiAddictionCloseGameEvent)GetProcAddress(hdll, "AntiAddictionCloseGameEvent")) // 防沉迷认证失败，需要由客户端负责关闭程序
        func(onAntiAddictionCloseGameEvent);
}

void TryOpenCmp(HWND hWnd) {
    //首先判断是否需要弹出CMP
    if (isOpenCmp(g_hDLL))
    {
        RECT clientRect;
        if (GetClientRect(hWnd, &clientRect)) {
            int panelWidth = clientRect.right - clientRect.left;
            int panelHeight = clientRect.bottom - clientRect.top;

            //1.创建CMP控件
            HINSTANCE minstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
            g_hPnlCmp = CreateWindowW(L"STATIC", L"This is a panel for CMP", WS_CHILD | WS_VISIBLE, 0, 0, panelWidth, 50, hWnd, (HMENU)3011, minstance, NULL);
            BringWindowToTop(g_hPnlCmp);

            //2.弹出CMP
            nlohmann::json json_obj = {
             {"appKey", YourAppKey},
             {"handle", reinterpret_cast<int>(g_hPnlCmp)},
             {"parentWidth", panelWidth},
             {"parentHeight", panelHeight}
            };
            std::string jsonStr = json_obj.dump();
            openCmp(g_hDLL, jsonStr.c_str());
        }
    }
}

void InitMgSdk(HWND hWnd) {
    if (g_hDLL) return;

    g_hDLL = LoadLibrary(L"MgSDKCSharpDLL.dll");
    if (g_hDLL) {
        RegisterCallbackEvent(g_hDLL);

        //若 App 集成了广告模块，必须在 App 启动后，首先调用 CMP 接口
        TryOpenCmp(hWnd);

        //SDK 初始化
        setupAsync(g_hDLL);
    }
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateControls(hWnd);
        InitMgSdk(hWnd);//初始化SDK
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case ID_BTN_LOGIN:
            openMgLogin();
            break;
        case ID_BTN_MGPAY:
            openMgStore(0);//0.MG支付 1.微软支付
            break;
        case ID_BTN_MSPAY:
            openMgStore(1);
            break;
        case ID_BTN_AD1:
        {//插屏
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
              {"advertplaceKey", InterstitialAdKey},
              {"appType", 1},
              {"adType", 1},//1.插屏 2.横幅 3.对联 4.全屏插播 5.开屏 6.退屏 7.激励视频 
              {"handle",reinterpret_cast<int>(g_hPnlInterstitial)}
            };
            std::string jsonStr = json_obj.dump();
            openMGAdvert(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD2:
        {//Banner
            CreateBannerAdPanel(hWnd);
            nlohmann::json json_obj = {
                {"handle",reinterpret_cast<int>(g_hPnlBanner)},
                {"advertplaceKey", BannerAdKey},
                {"appType", 1},
                {"adType", 2}//Banner 
            };
            std::string jsonStr = json_obj.dump();
            openMGAdvert(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD3:
        {//对联
            CreateCoupletAdPannel(hWnd);
            nlohmann::json json_obj = {
                {"handle",reinterpret_cast<int>(g_hPnlCoupletLeft)},
                {"advertplaceKey", CoupletAdKey},
                {"appType", 1},
                {"adType", 3},
                {"handle2", reinterpret_cast<int>(g_hPnlCoupletRight)}
            };
            std::string jsonStr = json_obj.dump();
            openMGAdvert(jsonStr.c_str());
            break;
        }
        case ID_BTN_AD4:
        {//全屏插播
            RECT clientRect;
            if (GetClientRect(hWnd, &clientRect)) {
                int clientWidth = clientRect.right - clientRect.left;
                int clientHeight = clientRect.bottom - clientRect.top;
                nlohmann::json json_obj = {
                 {"handle",reinterpret_cast<int>(g_hwndMain)},
                 {"advertplaceKey", FullScreenInterstitialAdKey},
                 {"appType", 1},
                 {"adType", 4},
                 {"parentWidth", clientWidth},
                 {"parentHeight", clientHeight}
                };
                std::string jsonStr = json_obj.dump();
                openMGAdvert(jsonStr.c_str());
            }
            break;
        }
        case ID_BTN_AD7:
        {//激励视频 
            CreateRewardAdPannel(hWnd);
            nlohmann::json json_obj = {
                 {"handle",reinterpret_cast<int>(g_hPnlReward)},
                 {"advertplaceKey", RewardedAdKey},
                 {"comment", "abc123"},//透传参数，前端需要进行urlEncode；在广告关闭回调事件中会原封不动的返回
                 {"appType", 1},
                 {"adType", 7},
                 /* {"width", 768},
                  {"height", 432}*/
            };
            std::string jsonStr = json_obj.dump();
            openMGAdvert(jsonStr.c_str());
            break;
        }
        case ID_BTN_ADPRELOAD10:
        {//插屏预缓存
            nlohmann::json json_obj = {
              {"advertplaceKey", InterstitialAdKey},
              {"appType", 1},
              {"adType", 1},//1.插屏 2.横幅 3.对联 4.全屏插播 5.开屏 6.退屏 7.激励视频 
            };
            std::string jsonStr = json_obj.dump();
            preloadAdvert(jsonStr.c_str());
            break;
        }
        case ID_BTN_ADPRELOAD11:
        {//插屏预缓存
            CreateInterstitialAdPannel(hWnd);
            nlohmann::json json_obj = {
              {"advertplaceKey", InterstitialAdKey},
              {"appType", 1},
              {"adType", 1},//1.插屏 2.横幅 3.对联 4.全屏插播 5.开屏 6.退屏 7.激励视频 
              {"handle",reinterpret_cast<int>(g_hPnlInterstitial)}
            };
            std::string jsonStr = json_obj.dump();
            showAdvert(jsonStr.c_str());
            break;
        }

        case ID_BTN_RATING://判断并进行好评
            isRatingReview(g_hDLL);
            break;
        case ID_BTN_CLOSECMP://关闭CMP，客户端将控件销毁即可
        {
            g_cmpChangedWidth = 0;
            g_cmpChangedHeight = 0;
            DestroyWindow(g_hPnlCmp);
            g_hPnlCmp = NULL;
            break;
        }

        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SHOW_OPENSCREEN_ADVERT:
    {
        CreateSplashScreenAdPanel(g_hwndMain);//创建开屏广告控件
        RECT clientRect;
        if (GetClientRect(g_hwndMain, &clientRect)) {
            int clientWidth = clientRect.right - clientRect.left;
            int clientHeight = clientRect.bottom - clientRect.top;
            nlohmann::json json_obj = {
             {"handle",reinterpret_cast<int>(g_hPnlSplashScreen)},
             {"advertplaceKey", SplashAdKey},
             {"appType", 1},
             {"adType", 1},
             {"parentWidth", clientWidth},//开屏广告，需要传入程序的宽高
             {"parentHeight", clientHeight}
            };
            std::string jsonStr = json_obj.dump();
            openMGAdvert(jsonStr.c_str());
        }
        return 0;
    }
    case WM_DESTROY_CMP: {
        try
        {
            DestroyWindow(g_hPnlCmp);
            g_hPnlCmp = NULL;
        }
        catch (const std::exception& ex)
        {
        }
        return 0;
    }
    case WM_DESTROY_ADVERT: {
        const char* json = reinterpret_cast<const char*>(lParam);
        if (json) {
            try
            {
                nlohmann::json json_obj = nlohmann::json::parse(json);
                std::string unitId = json_obj["advertplaceKey"];
                if (unitId == SplashAdKey)
                {//删除开屏广告控件
                    DestroyWindow(g_hPnlSplashScreen);
                    g_hPnlSplashScreen = NULL;
                }
                else if (unitId == InterstitialAdKey)
                {//删除插屏广告控件
                    DestroyWindow(g_hPnlInterstitial);
                    g_hPnlInterstitial = NULL;
                }
                else if (unitId == BannerAdKey)
                {//删除Banner广告控件
                    DestroyWindow(g_hPnlBanner);
                    g_hPnlBanner = NULL;
                }
                else if (unitId == CoupletAdKey)
                {//删除对联广告控件
                    int advertStatus = json_obj["AdvertStatus"];
                    if (advertStatus == 1)//广告已显示，由用户关闭
                    {
                        int coupletType = json_obj["CoupletType"];
                        if (coupletType == 1)//删除左侧控件
                        {
                            BOOL result = DestroyWindow(g_hPnlCoupletLeft);
                            g_hPnlCoupletLeft = NULL;
                        }
                        else
                        {
                            BOOL result = DestroyWindow(g_hPnlCoupletRight);
                            g_hPnlCoupletRight = NULL;
                        }
                    }
                    else
                    {   //没有广告素材，删除两个控件
                        BOOL result = DestroyWindow(g_hPnlCoupletLeft);
                        g_hPnlCoupletLeft = NULL;

                        BOOL result2 = DestroyWindow(g_hPnlCoupletRight);
                        g_hPnlCoupletRight = NULL;
                    }
                }
                else if (unitId == RewardedAdKey)
                {//激励视频
                    DestroyWindow(g_hPnlReward);
                    g_hPnlReward = NULL;

                    int completeStatus = json_obj["CompleteStatus"];
                    if (completeStatus == 1)
                    {
                        std::string comment = json_obj["Comment"];
                        std::string rewardId = json_obj["RewardId"];

                        //视频播放完毕，下发奖励道具 
                        //... 

                        //向MG核销订单
                        reportMgRewardFulfillment(rewardId.c_str());
                        AppendLog(L"reportAdRewardFulfillment Async: %hs", rewardId.c_str());
                    }
                }
            }
            catch (const std::exception& ex)
            {
            }
            free((void*)json);
        }
        return 0;
    }
    case WM_SIZE:
    {
        if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
        {
            int parentWidth = LOWORD(lParam);
            int parentHeight = HIWORD(lParam);
            if (g_hPnlCmp != NULL)
            {
                RECT cmpRect;
                GetClientRect(g_hPnlCmp, &cmpRect);
                int cmpWidth = cmpRect.right - cmpRect.left;
                int cmpHeight = cmpRect.bottom - cmpRect.top;
                if (cmpHeight > 50)
                {
                    int x = (parentWidth - cmpWidth) / 2;
                    int y = (parentHeight - cmpHeight) / 2;
                    SetWindowPos(g_hPnlCmp, NULL, x, y, cmpWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                }
                else
                {
                    if (cmpWidth != parentWidth)
                    {
                        SetWindowPos(g_hPnlCmp, NULL, 0, 0, parentWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                        if (g_cmpSdkControlHandle > 0)
                        {
                            HWND hWndSdkControl = (HWND)(INT_PTR)g_cmpSdkControlHandle;
                            SetWindowPos(hWndSdkControl, NULL, 0, 0, parentWidth, cmpHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
                        }
                    }
                }
            }

            if (g_hPnlBanner != NULL) {
                int x = (parentWidth - 728) / 2;
                int y = parentHeight - 90 - 20;
                SetWindowPos(g_hPnlBanner, NULL, x, y, 728, 90, SWP_NOZORDER | SWP_SHOWWINDOW);
            }
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
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

// Message handler for about box.
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
            openExitAdvert(g_hDLL);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

#pragma region //1.CMP
/*
    为实现CMP功能，开发者需按以下步骤创建并管理控件：
    1.初始化创建：开发者需在应用启动时，创建CMP控件并将其置于视图顶层。控件初始尺寸应设为：宽度与App等宽，高度为50 px。
    2.响应应用布局变化：当应用窗口尺寸改变时，开发者需手动更新CMP控件的宽度，确保其始终与新的可用宽度保持一致。
    3.完成与清理：当用户完成授权(即CMP关闭)后，开发者必须在 CmpClosedEvent 回调事件中，销毁CMP控件实例，以释放资源。
*/

bool isOpenCmp(HINSTANCE hdll) {
    bool result = false;
    try
    {
        IsOpenCmp func = (IsOpenCmp)GetProcAddress(hdll, "IsOpenCmp");
        result = func();
    }
    catch (const std::exception&)
    {
    }
    AppendLog(L"Cmp IsOpen Flag  %hs", result ? "true" : "false");
    return result;
}
void openCmp(HINSTANCE hdll, const char* jsonParam) {
    try
    {
        OpenCmp func = (OpenCmp)GetProcAddress(hdll, "OpenCmp");
        g_cmpSdkControlHandle = func(jsonParam);
    }
    catch (const std::exception&)
    {
    }
}

void onCmpClosedEvent(char* s) {
    try
    {
        nlohmann::json json_obj = nlohmann::json::parse(s);
        int cmpOrigin = (int)json_obj["cmpOrigin"];//1.From CMP control
        if (cmpOrigin == 1)
        {
            g_cmpChangedWidth = 0;
            g_cmpChangedHeight = 0;

            //Remove CMP control on the UI thread
            /* DestroyWindow(g_hPnlCmp);
            g_hPnlCmp = NULL;*/
            PostMessage(g_hwndMain, WM_DESTROY_CMP, 0, NULL);

            AppendLog(L"CMP has been removed.");
        }
    }
    catch (const std::exception&)
    {
    }
}
#pragma endregion

#pragma region //2.SDK Initialisation
void setupAsync(HINSTANCE hdll) {
    try
    {
        SetupAsync func = (SetupAsync)GetProcAddress(hdll, "SetupAsync");
        func(YourAppKey);
    }
    catch (const std::exception&)
    {
    }
}
// Initialise callback functions
void onInitCompleteEvent(char* s) {
    try
    {
        AppendLog(L"SDK Initialization: %hs", s);
        if (s[0] == '1') {
            loadMgExitAdvert(g_hDLL); // After initialization, load the content of the back screen advertisement

            PostMessage(g_hwndMain, WM_SHOW_OPENSCREEN_ADVERT, 0, NULL);//Open screen advertisement 
        }
    }
    catch (const std::exception&)
    {
    }
}
#pragma endregion

#pragma region  //3.Login / MG Community
// Open Login
void openMgLogin() {
    OpenMgLogin func = (OpenMgLogin)GetProcAddress(g_hDLL, "OpenMgLogin");
    func();
}
// Login completion callback function
void onLoginCompleteEvent(char* s) {
    if (s[0] == '1') {
        AppendLog(L"onLoginComplete: %hs", s);

        //1,Login Successful
        //... 

        char* userId = strstr(s, "userId=");
        char* userName = strstr(s, "userName=");
        char* userToken = strstr(s, "userToken=");

        //isOpenMgBubble(hDLL);

       /* if (userId && userName && userToken) {
            AppendLog(L"User ID: %hs", userId + 7);
            AppendLog(L"User Name: %hs", userName + 9);
            AppendLog(L"Token: %hs", userToken + 10);
        }  */
    }
}
bool isOpenMgBubble(HINSTANCE hdll) {
    IsOpenMgBubble func = (IsOpenMgBubble)GetProcAddress(hdll, "IsOpenMgBubble");
    bool res = func();
    if (res)
    {
        AppendLog(L"MG bubble permission has been opened in the background");

        //Show MG Bubble button
        //EnableWindow(GetDlgItem(g_hwndMain, ID_BTN_MGBUBBLE), TRUE);
    }
    return res;
}
//Open MG Community
void openMgBubble(HINSTANCE hdll) {
    OpenMgBubble func = (OpenMgBubble)GetProcAddress(hdll, "OpenMgBubble");
    func();
}
#pragma endregion

#pragma region  //4.Payment
// Open Mg Store 
void openMgStore(int isOnlyMsPay) {
    if (auto func = (OpenMgStore)GetProcAddress(g_hDLL, "OpenMgStore")) {
        nlohmann::json json_obj = {
                 {"goodsKey",MgGoodsKey},
                 { "callback", MgCallbackId },
                 {"comment", "abc132"},
                 {"isOnlyMsPay", isOnlyMsPay}
        };
        std::string jsonStr = json_obj.dump();
        func(jsonStr.c_str());
    }
}

// After successful payment, the client receives the asset change callback function
void onAssetsChangedEvent(char* s) {
    //onAssetsChange: {"Assets":[{"Id":"7B3E88A55A20241223180728","Comment":"ProductId123-OrderKey123","DigitalGoods":{"Name":"%E5%95%86%E5%93%811","Tag":"product_1Yuan_tag%E5%8F%82%E6%95%B0%3Aa%3D1%7Cb%3D2%24c%3D3","Price":"0.01","Count":1}}],"EventType":1}
    AppendLog(L"onAssetsChangedEvent: %hs", s);
    nlohmann::json json_obj = nlohmann::json::parse(s);
    nlohmann::json assets = json_obj["Assets"];
    for (size_t i = 0; i < assets.size(); ++i) {// JSON parsing here should loop through values
        //game props

        std::string orderId = assets[i]["Id"];
        reportFulfillment(orderId.c_str()); // Write-off of assets
    }
}
// Write-off of assets
void reportFulfillment(const char* orderId) {
    ReportFulfillment func = (ReportFulfillment)GetProcAddress(g_hDLL, "ReportFulfillment");
    bool res = func(orderId);
    if (res)
    {
        //success
        AppendLog(L"orderId %hs ,assets have been written off", orderId);
    }
}

// Payment Complete Callback Function
void onPaymentCompleteEvent(char* s) {
    //example：{"Comment":"adcd-efg123","OrderKey":"7B3E88A55A20241127173952","GoodsKey":"","Amount":"0.01","Result":"success","TransactionId":""}
    nlohmann::json json_obj = nlohmann::json::parse(s);
    std::string strResult = json_obj["Result"];
    if (strResult == "success")
    {
        std::string strPrice = json_obj["Amount"];
        double price = std::stod(strPrice);

        std::string orderId = json_obj["OrderKey"];
        AppendLog(L"onPaymentCompleteEvent,orderKey is %hs", orderId.c_str());
    }
}
#pragma endregion

#pragma region //5.Ad
// 1.LoadMgExitAdvert
void loadMgExitAdvert(HINSTANCE hdll) {
    LoadMgExitAdvert func = (LoadMgExitAdvert)GetProcAddress(hdll, "LoadMgExitAdvert");
    func(ExitAdKey);
}

// 2.openExitAdvert
void openExitAdvert(HINSTANCE hdll) {
    OpenExitAdvert func = (OpenExitAdvert)GetProcAddress(hdll, "OpenExitAdvert");
    func();
}

// 3.Register Callback event to close the advert
void onFireCloseAdvertEvent(char* s) {
    AppendLog(L"onAdCloseEvent: %hs", s);
    //...
    // Destroy Ad pannel 

    // 发送到主UI线程
    char* jsonCopy = _strdup(s);
    PostMessage(g_hwndMain, WM_DESTROY_ADVERT, 0, reinterpret_cast<LPARAM>(jsonCopy));
}

// 4.4.Open MG Advert window
void openMGAdvert(const char* json) {
    try
    {
        // 确保在调用前COM已初始化
        if (!g_comInitialized) {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                g_comInitialized = true;
            }
        }

        OpenAdvert func = (OpenAdvert)GetProcAddress(g_hDLL, "OpenAdvert");
        if (func) {
            int result = func(json);
            if (result == 1) {
                // success 
                AppendLog(L"open mg ad: success");
            }
        }
    }
    catch (const std::exception& ex)
    {
    }
}
//5.Report to MG
void reportMgRewardFulfillment(const char* rewardId) {
    ReportMgRewardFulfillment func = (ReportMgRewardFulfillment)GetProcAddress(g_hDLL, "ReportMgRewardFulfillment");
    if (func) {
        func(rewardId);
    }
}


void preloadAdvert(const char* json)
{
    try
    {
        // 确保在调用前COM已初始化
        if (!g_comInitialized) {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                g_comInitialized = true;
            }
        }
        PreloadAdvert func = (PreloadAdvert)GetProcAddress(g_hDLL, "PreloadAdvert");
        if (func) {
            func(json);
        }
    }
    catch (const std::exception& ex)
    {
    }
}

void onPreloadAdvertEvent(char* s) {
    //s = {"advertplaceKey":"C6E76462AF","AdvertStatus":1,"DisplayStatus":0}
    AppendLog(L"onPreloadAdvertEvent: %hs", s);
    //...

    nlohmann::json json_obj = nlohmann::json::parse(s);
    std::string unitId = json_obj["advertplaceKey"];
    std::int32_t adStatus = json_obj["AdvertStatus"];
    if (adStatus == 1)
    {//缓存成功，可以显示广告
        if (unitId == SplashAdKey)
        {//开屏广告 
            CreateSplashScreenAdPanel(g_hwndMain);
            RECT clientRect;
            if (GetClientRect(g_hwndMain, &clientRect)) {
                int clientWidth = clientRect.right - clientRect.left;
                int clientHeight = clientRect.bottom - clientRect.top;
                nlohmann::json json_obj = {
                 {"handle",reinterpret_cast<int>(g_hPnlSplashScreen)},
                 {"advertplaceKey", SplashAdKey},
                 {"appType", 1},
                 {"adType", 1},
                 {"parentWidth", clientWidth},//开屏广告，需要传入程序的宽高
                 {"parentHeight", clientHeight}
                };
                std::string jsonStr = json_obj.dump();
                showAdvert(jsonStr.c_str());
            }
        }
        else
        {
            // The button show the advert can be clicked.
            EnableWindow(g_hBtnPreloadAdShow, TRUE);
        }
    }
}

int showAdvert(const char* json)
{
    int result = 0;
    try
    {
        if (!g_comInitialized) {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (SUCCEEDED(hr)) {
                g_comInitialized = true;
            }
        }
        ShowAdvert func = (ShowAdvert)GetProcAddress(g_hDLL, "ShowAdvert");
        if (func) {
            result = func(json);
            if (result == 1) {
                // success 
                AppendLog(L"show mg ad: success");
                EnableWindow(g_hBtnPreloadAdShow, FALSE);
            }
        }
    }
    catch (const std::exception& ex)
    {
    }
    return result;
}
#pragma endregion

#pragma region //6.Praise
//Whether the user has commented on the application interface
void isRatingReview(HINSTANCE hdll) {
    IsRatingReview func = (IsRatingReview)GetProcAddress(hdll, "IsRatingReview");
    func();
}
void onIsRatingReviewEvent(char* s) {
    AppendLog(L"onIsRatingReviewEvent: %hs", s);
    if (s[0] == '1') {
        // s:
        // 1,User has commented
        // 0,The user hasn't commented yet
    }
    else if (s[0] == '0') {
        //..
        showRatingReview(g_hDLL);//调用好评接口
    }
}

//Callback event to determine if the user has commented on the app
void showRatingReview(HINSTANCE hdll) {
    ShowRatingReview func = (ShowRatingReview)GetProcAddress(hdll, "ShowRatingReview");
    func();
}
void onShowRatingReviewEvent(char* s) {
    AppendLog(L"onShowRatingReviewEvent: %hs", s);
    // s:
    // 1,User comments succeeded
    // 0,User comment failed, close the ratingReview window directly and so on  
}
#pragma endregion

#pragma region  //7.Push
void onPushUriEvent(char* s) {
    AppendLog(L"Push URI is: %hs", s);

    //将推动uri传给游戏服务器
}
#pragma endregion

#pragma region //8.Anti-Addiction
//Close the game event after failing the anti-addiction authentication, the callback returns the string "1".
void onAntiAddictionCloseGameEvent(char* s) {
    AppendLog(L"onAntiAddictionCloseGame:%s\n", s);
    //Close game logic implemented by the game client
    //...
}
#pragma endregion