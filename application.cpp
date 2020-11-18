#include "application.hpp"
#include "exception.hpp"

namespace Is
{
   using std::string;
   using std::size_t;


   /**
    * インスタンス初期化処理
    * */
   void Application::_initialize()
   {
       string& win_cls_name = Application::windowClassName();
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
        wcex.lpszClassName = (LPCTSTR)win_cls_name.c_str();     // Window Class Name
        wcex.hIconSm = small_icon;                              // Window Small Icon      
        wcex.lpfnWndProc = Widget::WindowProc;                  // Window Procedure
   }

    /**
     * インスタンス終了処理
     * */
    void Application::_release()
    {
        // ウィンドウクラスの登録を解除
        string& win_cls_name = Application::windowClassName();
        HINSTANCE& instance = Application::instance();
        UnregisterClass(win_cls_name.c_str(), instance);
    }





    /**
     * アプリケーションの起動結果
     * */
    bool Application::exe()
    {
        size_t& widget_count = Application::widgetCount();
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
        MSG msg;            // Windows Message情報
        size_t loop_count = 0; // メッセージループのカウント回数
        HACCEL& hanlder_keyboard_accel = Applicaiton::accelHandle();

        // メッセージループ
        while (true)
        {
            // メッセージが来ているか確認
            if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) > 0)
		    {
                // メッセージを取得
                if (GetMessage(&msg, NULL, 0, 0) > 0)
                {
                    bool flag = true;

                    // Dialog宛のメッセージかチェック
                    if (Widget::widget_map.size() > 0)
                    {
                        for (auto iter = Widget::widget_map.cbegin(); iter != Is::Widget::widget_map.cend(); ++iter)
                        {
                            HWND handler_widget = iter->first;
                            if (IsDialogMessage(handler_widget, &msg))
                            {
                                // 該当するDialogへのメッセージ
                                flag = false;
                                break;
                            }
                        }
                    }

                    // Window宛のメッセージ処理
                    if (flag)
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
                if (on_idle(loop_count))
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
    bool Application::on_idle(size_t &message_count)
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