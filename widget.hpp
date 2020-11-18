#ifndef __IS_WIDGET_HPP__
#define __IS_WIDGET_HPP__

#include "pch.hpp"
#include "object.hpp"



namespace Is
{
    using std::string;
    using std::size_t;

    class Widget : public Object
    {
        using Registry_t = map<HWND, pair<HWND, pair<shared_ptr<Widget>, void()>>>;

        RECT rect_client_;
        RECT rect_window_;
        SIZE size_window_;
        //INITCOMMONCONTROLSEX common_control_;

    protected:

        HWND handler_menu_;
        HWND handler_statusbar_;
        WPARAM wparam_;
        LPARAM lparam_;
        size_t status_id_;
        string status_message_;


        // Windows Messages
        void onCreateEvent();
        void onDestroyEvent();
        void onCloseEvent(shared_ptr<CloseEvent> event);
        void onHideEvent(shared_ptr<HideEvent> event);
        void onShowEvent(shared_ptr<ShowEvent> event);
        void onChangeEvent(shared_ptr<Event> event);
        void onEnterEvent(shared_ptr<Event> event);
        void onLeaveEvent(shared_ptr<Event> event);
        void onKeyPressEvent(shared_ptr<KeyEvent> event);
        void onKeyReleaseEvent(shared_ptr<KeyEvent> event);
        void onFocusInEvent(shared_ptr<FocusEvent> event);
        void onFocusOutEvent(shared_ptr<FocusEvent> event);
        void onFocusNextChildEvent(shared_ptr<FocusEvent> event);
        void onFocusPrevChildEvent(shared_ptr<FocusEvent> event);
        void onResizeEvent(shared_ptr<ResizeEvent> event);
        void onMoveEvent(shared_ptr<MoveEvent> event);
        void onPaintEvent(shared_ptr<PaintEvent> event);
        void onMousePressEvent(shared_ptr<MouseEvent> event);
        void onMouseReleaseEvent(shared_ptr<MouseEvent> event);
        void onMouseDbouleClickEvent(shared_ptr<MouseEvent> event);
        void onMouseMoveEvent(shared_ptr<MouseEvent> event);
        void onWheelEvent(shared_ptr<WheelEvent> event);
        void onDropEvent(shared_ptr<DropEvent> event);
        void onDragEnterEvent(shared_ptr<DragEnterEvent> event);
        void onDragMoveEvent(shared_ptr<DragMoveEvent> event);
        void onDragLeaveEvent(shared_ptr<DragLeaveEvent> event);
        void onTableEvent(shared_ptr<TableEvent> event);

        
    public:
        

        // マルチスレッドの各スレッドでStaticWindow(Dialog)Procを使うために必要
        void add_ref(HWND handler_widget);
        void sub_ref(HWND handler_widget);

        void show(UINT style = SW_SHOW);

    public:
        Widget(shared_ptr<Widget> parent);
        virtual ~Widget();


        static Registry_t& widgetRegistry()
        {
            static Registry_t widget_registry = Registry_t();
            return widget_registry;
        }

        
    }

}
#endif // __IS_WIDGET_HPP__