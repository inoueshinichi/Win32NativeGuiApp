#ifndef __IS_WIDGET_HPP__
#define __IS_WIDGET_HPP__

#include "pch.hpp"
#include "exception.hpp"
#include "object.hpp"
#include "event.hpp"



namespace Is
{
    using std::size_t;

    class Widget : public Object
    {
        RECT rect_client_;
        //INITCOMMONCONTROLSEX common_control_;

        void createWindow(const CString& window_title, int width, int height);

    protected:
        CString window_title_;
        HMENU handler_menu_;
        HWND handler_statusbar_;
        size_t status_id_;
        CString status_message_;


    public:
        Widget(shared_ptr<Widget> parent);
        virtual ~Widget();

        
        void show();
        RECT windowSize();
        RECT clientSize();
        
        
        // Windows Messages
        void onCreate();
        void onDestroy();
        void onClose(shared_ptr<CloseEvent> event);
        void onHide(shared_ptr<HideEvent> event);
        void onShow(shared_ptr<ShowEvent> event);
        void onChange(shared_ptr<Event> event);
        void onEnter(shared_ptr<Event> event);
        void onLeave(shared_ptr<Event> event);
        void onKeyPress(shared_ptr<KeyEvent> event);
        void onKeyRelease(shared_ptr<KeyEvent> event);
        void onFocusIn(shared_ptr<FocusEvent> event);
        void onFocusOut(shared_ptr<FocusEvent> event);
        void onFocusNextChild(shared_ptr<FocusEvent> event);
        void onFocusPrevChild(shared_ptr<FocusEvent> event);
        void onResize(shared_ptr<ResizeEvent> event);
        void onMove(shared_ptr<MoveEvent> event);
        void onPaint(shared_ptr<PaintEvent> event);
        void onMousePress(shared_ptr<MouseEvent> event);
        void onMouseRelease(shared_ptr<MouseEvent> event);
        void onMouseDbouleClick(shared_ptr<MouseEvent> event);
        void onMouseMove(shared_ptr<MouseEvent> event);
        void onWheel(shared_ptr<WheelEvent> event);
        void onDrop(shared_ptr<DropEvent> event);
        void onDragEnter(shared_ptr<DragEnterEvent> event);
        void onDragMove(shared_ptr<DragMoveEvent> event);
        void onDragLeave(shared_ptr<DragLeaveEvent> event);
        void onTable(shared_ptr<TableEvent> event);

 
    }

}
#endif // __IS_WIDGET_HPP__