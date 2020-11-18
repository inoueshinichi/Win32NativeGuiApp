#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "pch.hpp"

namespace Is
{
    using std::size_t;
    using std::string;

    class Application
    {
        HINSTANCE handler_instance_;     // プロセスのハンドラ
        int argc_;                       // コマンドライン引数の数
        char** argv_;                    // コマンドライン引数
        static string window_class_name_; // Windowsのウィンドウクラスの名前
        const static int criteria_message_count_ = 1000; // メッセージループのアイドル処理用しきい値
        static size_t widget_count_;     // Widget数         
        HACCEL handler_shortcut_key_table_; // ショートカット用(※要調査)

        void _init();
        void _release();
        bool _init_instance();
        bool _exit_instance();

    protected:
        virtual bool run();
        virtual bool exe();
        virtual bool on_idle(size_t &message_count);

    public:
        Application(int argc, char** argv) 
            : argc_(argc)
            , argv_(argv)
            , handler_shortcut_key_table_(nullptr)
        {
           // プロセスの初期化
            _init();
        }

        virtual ~Application();

        // Getter
        size_t widget_count() const { return widget_count_; }
        HINSTANCE instance() const { return handler_instance_; }
       
    };
}



#endif // __APPLICATION_HPP__