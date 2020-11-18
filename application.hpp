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
        Application(int argc, char** argv);
        virtual ~Application();
        bool exe();

        
        // static functions
        static size_t widgetCount();
        static HINSTANCE& instance();
        static CString& windowClassName();
        static HICON& icon();
        static HICON& smallIcon();
        static HCURSOR& cursor();
        static HBRUSH& background();
        static HACCEL& accelHandle();
       
    };
}



#endif // __IS_APPLICATION_HPP__