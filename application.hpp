#ifndef __IS_APPLICATION_HPP__
#define __IS_APPLICATION_HPP__

#include "pch.hpp"

namespace Is
{
    using std::size_t;
    using std::string;

    class Application
    {
        int argc_;                       // コマンドライン引数の数
        char** argv_;                    // コマンドライン引数
        static const int criteria_message_count_ = 1000; // メッセージループのアイドル処理用しきい値       

        void _initialize();
        void _release();

    protected:
        virtual bool run();
        virtual bool onIdle(size_t &message_count);

    public:
        bool exe();


        Application(int argc, char** argv) 
            : argc_(argc)
            , argv_(argv)
        {
            // Windows初期化
            _initialize();
        }

        virtual ~Application()
        {
            // Windows破棄
            _release();
        }

        
        static size_t& widgetCount()
        {
            // TODO: スレッドセーフにすること
            static size_t widget_count_ = 0;
             return widget_count_; 
        }


        static HINSTANCE& instance() 
        {
            // インスタンスハンドルの取得
            static HINSTANCE handler_process_instance_ = ::GetModuleHandle(NULL);
            if (!handler_process_instance_)
            {
                win32api_error();
            }
            return handler_process_instance_; 
        }


        static string& windowClassName()
        {
            static string window_class_name_ = "MyWindowClass";
            return window_class_name_;
        }


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


        static HICON& smallIcon()
        {
            static HICON handler_small_icon_ = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON, 0, 0, LR_SHARED);
            // LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
            if (!handler_small_icon_)
            {
                win32api_error();
            }
            return handler_small_icon_;
        }


        static HCURSOR& cursor()
        {
            static HCURSOR& handler_cursor_ = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED); 
            // = LoadCursor(nullptr, IDC_ARROW); 
            if (!handler_cursor_)
            {
                win32api_error();
            }
            return handler_cursor_;
        }


        static HBRUSH& background()
        {
            static HBRUSH handler_background_color_ = (HBRUSH)GetStockObject(WHITE_BRUSH); 
            if (!handler_background_color_)
            {
                win32api_error();
            }
            return handler_background_color_;
        }


        static HINSTANCE& accelHandle()
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
       
    };
}



#endif // __IS_APPLICATION_HPP__