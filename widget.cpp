#include "widget.hpp"
#include "exception.hpp"
#include "application.hpp"

namespace Is
{
    Widget::Widget(shared_ptr<Widget> parent)
        : Object(parent)
        , window_title_("")
        , handler_menu_(NULL)
        , handler_statusbar_(NULL)
        , status_id_(0)
        , rect_client_{ 0, 0, 0, 0 }
        , rect_window_{ 0, 0, 0, 0 }
        {}

    
    Widget::~Widget()
    {
        
    }


    void Widget::show()
    {
        HWND hWnd = EventDispatcher::findWinHandler(this);
        if (!hWnd)
        {
            ::ShowWindow(hWnd, SW_SHOW) && ::UpdateWindow(hWnd);
            return;
        }
        IS_ERROR(error_code::value, "Failed to show() widget: %p", this);
    }


    RECT Widget::windowSize()
    {
        HWND hWnd = EventDispatcher::findWinHandler(this);
        if (!hWnd)
        {
            RECT window_rect;
            ::GetWindowRect(hWnd, &window_rect);
            return window_rect;
        }
        IS_ERROR(error_code::value, "Failed to windowSize() widget: %p", this);
    }


    RECT Widget::clientSize()
    {
        HWND hWnd = EventDispatcher::findWinHandler(this);
        if (!hWnd)
        {
            RECT client_rect;
            ::GetClientRect(hWnd, &client_rect);
            return client_rect;
        }
        IS_ERROR(error_code::value, "Failed to clientSize() widget: %p", this);
    }


    void Widget::createWindow(const CString& window_title, int width, int height)
    {
        window_title_ = window_title;
        int width = width ? width : CW_USEDEFAULT;
        int height = height ? height : CW_USEDEFAULT;

        HWND hParentWnd = EventDispatcher::findWinHandler((Widget*)parent_.get()); // ダウンキャスト

        HWND hWnd = CreateWindowEx(
            WS_EX_OVERLAPPEDWINDOW,                              // Window Ex Style
            (LPCTSTR)Application::windowClassName(),             // Window Class Name
            (LPCTSTR)window_title,                               // Window Name
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER, // Window Style
            CW_USEDEFAULT,                                       // left
            CW_USEDEFAULT,                                       // top
            width,                                               // width
            height,                                              // height
            hParentWnd,                                          // HWND
            handler_menu_,                                       // HMENU
            Application::instance(),                             // HINSTANCE
            (LPVOID)this);                                       // プロシージャに転送される

        if (!hWnd)
        {
            MessageBox(NULL, 
                       _T("ウィンドウの生成に失敗しました."), 
                       (LPCTSTR)Application::windowClassName(), 
                       MB_OK | MB_ICONHAND);
            IS_ERROR(error_code::os, "Failed to generate window in %p", this);
        }
    }


    void Widget::onDestroy()
    {
        HWND hWnd = EventDispatcher::findWinHandler(this);
        if (!hWnd)
        {
            ::DestroyWindow(hWnd);  // Window表示の破棄(instanceは残ったまま)

            if (handler_menu_)
                ::DestroyMenu(handler_menu_);
        }
        IS_ERROR(error_code::value, "Failed to clientSize() widget: %p", this); 
    }


    void Widget::onResize(shared_ptr<ResizeEvent> event)
    {
        HWND hWnd = EventDispatcher::findWinHandler(this);
        if (!hWnd)
        {
            ::InvalidateRect(hWnd, NULL, TRUE);
        }
    }

    
    void Widget::onMove(shared_ptr<MoveEvent> event)
    {
        HWND hWnd = EventDispatcher::findWinHandler(this);
        if (!hWnd)
        {
            ::InvalidateRect(hWnd, NULL, TRUE);
        }
    }

    
}