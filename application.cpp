#include "application.hpp"

#include "exception.hpp"

namespace Is
{
   

    /**
     * アプリケーションの初期化
     * */
    void Application::_init()
    {
        // ショートカットキー定義の読み込み
	    handler_shortcut_key_table_ = LoadAccelerators(handler_instance_, MAKEINTRESOURCE(IDC_WINCVAPP));

        // エントリーウィンドウの生成
        _init_instance();
    }


    /**
     * エントリーウィンドウの生成
     * 本当は例外処理が必要だと思う
     * */
    bool Application::_init_instance()
    {
        bool ret = true;

        window_class_name_ = "MyWindowClass";
        widget_count_ = 0;

        // インスタンスハンドルの取得
        handler_instance_ = ::GetModuleHandle(NULL);
        if (!handler_instance_)
        {
            win32api_error();
            ret = false;
        }
        return ret;
    }


    /**
     * インスタンス終了処理
     * */
    bool Application::_exit_instance()
    {
        // ウィンドウクラスの登録を解除
        return UnregisterClass(window_class_name_.c_str(), handler_instance_);
    }


    /**
     * アプリケーションの起動結果
     * */
    bool Application::exe()
    {
        if (widget_count_ > 0)
            return run();
        else
            return _exit_instance();
    }


    /**
     * Windowsメッセージループ
     * */
    bool Application::run()
    {
        MSG msg;            // Windows Message情報
        size_t loop_count = 0; // メッセージループのカウント回数

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
                    if (Is::Widget::widget_map.size() > 0)
                    {
                        for (auto iter = Is::Widget::widget_map.cbegin(); iter != Is::Widget::widget_map.cend(); ++iter)
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
                        if (!TranslateAccelerator(msg.hwnd, handler_shortcut_key_table_, &msg))
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