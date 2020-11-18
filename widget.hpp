#ifndef __WIDGET_HPP__
#define __WIDGET_HPP__

#include "pch.hpp"
#include "object.hpp"



namespace Is
{
    using std::string;

    class Widget : public Object
    {
        static map<HWND, pair<ObjectPtr, size_t>> widget_map_; // <HWND, pair<shared_ptr<Object> inter_lock_count>>
        HWND handler_widget_;
        HWND handler_menu_;
        HWND handler_statusbar_;
        INITCOMMONCONTROLSEX common_control_;
        UINT status_id_;
        string status_message_;
        WPARAM wparam_;
        LPARAM lparam_;
        RECT rect_client_;
        RECT rect_window_;
        SIZE size_window_;
        bool is_heap_location_;


        static bool register_window_class()
        {
            if (!Object::instance())
                return false;
            
            WNDCLASSEX wcex;
            wcex.cbSize = sizeof(WNDCLASSEX);                       // Window ClassEx Size
            wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;      // Window Style
            wcex.cbClsExtra = 0;                                    // Window Class Extra
            wcex.cbWndExtra = 0;// DLGWINDOWEXTRA;                  // Window Handle Extra
            wcex.hInstance = Object::instance();                    // Window Instance
            wcex.hIcon = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDC_WINCVAPP), IMAGE_ICON, 0, 0, LR_SHARED);    // Window Icon       // = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_WINCVAPP));          
            wcex.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);                  // Window Cursor     // = LoadCursor(nullptr, IDC_ARROW);                            
            wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);                                           // Window Background
            wcex.lpszMenuName = NULL;                                                                           // Window Menu       // = MAKEINTRESOURCEW(IDC_WINCVAPP_MENU); 
            wcex.lpszClassName = (LPCTSTR)Object::window_class_name().c_str();                                  // Window Class Name
            wcex.hIconSm = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDC_WINCVAPP), IMAGE_ICON, 0, 0, LR_SHARED);  // Window Small Icon // = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));     
            wcex.lpfnWndProc = StaticWindowProc;                                                                // Window Procedure
        }

    protected:
        // Virtual Windows Messages
        virtual void on_destroy() = 0;
        virtual int on_create(HWND handler_widget) = 0;
        virtual void on_close() = 0;
        virtual void on_size() = 0;
        virtual void on_move() = 0;
        virtual void on_paint() = 0;

        // マルチスレッドの各スレッドでStaticWindow(Dialog)Procを使うために必要
        void add_ref(HWND handler_widget);
        void sub_ref(HWND handler_widget);


    public:
        Widget(shared_ptr<Widget> parent);
        virtual ~Widget();

        void show(UINT style = SW_SHOW);
    }

}
#endif // __WIDGET_HPP__