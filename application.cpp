#include "application.hpp"
#include "exception.hpp"
#include "widget.hpp"
#include "event.hpp"

namespace Is
{
   using std::string;
   using std::size_t;


    /**
     * コンストラクタ
     * */
    Application::Application(int argc, char** argv)
    : argc_(argc), argv_(argv)
    {
        // Windows初期化
        _initialize();
    }


    /**
     * デストラクタ
     * */
    Application::~Application()
    {
        // Windows破棄
        _release();
    }


    // static func
    size_t widgetCount()
    {
        // TODO: スレッドセーフにすること
        EventDispatcher::Registry_t& widget = EventDispatcher::widget();
        return widget.size(); 
    }


    // static func
    HINSTANCE& instance()
    {
        // インスタンスハンドルの取得
        static HINSTANCE handler_process_instance_ = ::GetModuleHandle(NULL);
        if (!handler_process_instance_)
        {
            win32api_error();
        }
        return handler_process_instance_; 
    }


    // static func
    CString& windowClassName()
    {
        static CString window_class_name_ = "MyWindowClass";
        return window_class_name_;
    }


    // static func
    static HICON& icon()
    {
        static HICON handler_icon_ = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDC_WINCVAPP), IMAGE_ICON, 0, 0, LR_SHARED);
        // = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_WINCVAPP)); 
        if (!handler_icon_)
        {
            win32api_error();
        }
        return handler_icon_;
    }


    // static func
    HICON& smallIcon()
    {
        static HICON handler_small_icon_ = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON, 0, 0, LR_SHARED);
        // LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        if (!handler_small_icon_)
        {
            win32api_error();
        }
        return handler_small_icon_;
    }


    // static func
    HCURSOR& cursor()
    {
        static HCURSOR& handler_cursor_ = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED); 
        // = LoadCursor(nullptr, IDC_ARROW); 
        if (!handler_cursor_)
        {
            win32api_error();
        }
        return handler_cursor_;
    }


    // static func
    HBRUSH& background()
    {
        static HBRUSH handler_background_color_ = (HBRUSH)GetStockObject(WHITE_BRUSH); 
        if (!handler_background_color_)
        {
            win32api_error();
        }
        return handler_background_color_;
    }


    // static func
    HACCEL& accelHandle()
    {
        // キーボードアクセラレータ
        HINSTANCE& instance = Application::instance();
        static HACCEL handler_keyboard_accel_ =  LoadAccelerators(instance, MAKEINTRESOURCE(IDC_WINCVAPP));
        if (!handler_keyboard_accel_)
        {
            win32api_error();
        }
        return handler_keyboard_accel_;
    }


   /**
    * インスタンス初期化処理
    * */
   void Application::_initialize()
   {
       CString& win_cls_name = Application::windowClassName();
       HINSTANCE& instance = Application::instance();
       HICON& icon = Application::icon();
       HICON& small_icon = Application::smallIcon();
       HCURSOR& cursor = Application::cursor();
       HBRUSH& bgcolor = Application::background();

        // Window Classの生成
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);                       // Window ClassEx Size
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;      // Window Style
        wcex.cbClsExtra = 0;                                    // Window Class Extra
        wcex.cbWndExtra = 0;// DLGWINDOWEXTRA;                  // Window Handle Extra
        wcex.hInstance = instance;                              // Window Instance
        wcex.hIcon = icon;                                      // Window Icon                
        wcex.hCursor = cursor;                                  // Window Cursor                                
        wcex.hbrBackground = bgcolor;                           // Window Background
        wcex.lpszMenuName = NULL;                               // Window Menu       // = MAKEINTRESOURCEW(IDC_WINCVAPP_MENU); 
        wcex.lpszClassName = (LPCTSTR)win_cls_name;             // Window Class Name
        wcex.hIconSm = small_icon;                              // Window Small Icon      
        wcex.lpfnWndProc = EventDispatcher::entryProcedure;     // Window Procedure
   }

    /**
     * インスタンス終了処理
     * */
    void Application::_release()
    {
        // ウィンドウクラスの登録を解除
        CString& win_cls_name = Application::windowClassName();
        HINSTANCE& instance = Application::instance();
        UnregisterClass((LPCTSTR)win_cls_name, instance);
    }


    /**
     * アプリケーションの起動結果
     * */
    bool Application::exe()
    {
        size_t widget_count = Application::widgetCount();
        if (widget_count > 0)
            return run();
        else
            return false;
    }


    /**
     * Windowsメッセージループ
     * */
    bool Application::run()
    {
        MSG msg;               // Windows Message情報
        size_t loop_count = 0; // メッセージループのカウント回数
        HACCEL& hanlder_keyboard_accel = Application::accelHandle();

        // メッセージループ
        while (true)
        {
            // メッセージが来ているか確認
            if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) > 0)
		    {
                // メッセージを取得
                if (GetMessage(&msg, NULL, 0, 0) > 0)
                {
                    bool window_flag = true;
                    auto& widget_registry = EventDispatcher::widget();

                    // Dialog宛のメッセージかチェック
                    if (widget_registry.size() > 0)
                    {
                        for (auto iter = widget_registry.cbegin(); iter != widget_registry.cend(); ++iter)
                        {
                            if (IsDialogMessage(iter->first, &msg))
                            {
                                // 該当するDialogへのメッセージ
                                window_flag = false;
                                break;
                            }
                        }
                    }

                    // Window宛のメッセージ処理
                    if (window_flag)
                    {
                        // 独自定義のショートカットキーの検出
                        if (!TranslateAccelerator(msg.hwnd, hanlder_keyboard_accel, &msg))
                        {
                            // ウィンドウメッセージの送出
                            DispatchMessage(&msg);    // DispatchMessageでウィンドウプロシージャに送出
                            ::TranslateMessage(&msg); // TranslateMessageで仮想キーメッセージを文字へ変換
                        }
                    }

                } else
                {
                    // WM_QUIT or エラー
                    break;
                }

            } else
            {
                // メッセージが来ていない場合，アイドル処理
                if (onIdle(loop_count))
                {
                    loop_count++;
                }
            }
        }

        return true;
    }


    /**
     * アイドル処理
     * */
    bool Application::onIdle(size_t &message_count)
    {
        // これを入れないと，Runのメッセージループが回り続けて，CPUリソースをどか食いする
        if (message_count > criteria_message_count_)
        {
            message_count = 0;
            Sleep(10); // 10ms
            return false;
        }
        return true;
    }
}