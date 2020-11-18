#ifndef __IS_EVENT_HPP__
#define __IS_EVENT_HPP__
#include "pch.hpp"
#include "widget.hpp"

namespace Is
{
    using std::make_shared;
    using std::unordered_map;

    class EventDispatcher : public Object
    {
        using LockCount_t = unordered_map<HWND, LONG>;

    public:
        using Registry_t = unordered_map<HWND, shared_ptr<Widget>>;
        static Registry_t& widget();
        static void addWidget(HWND hWnd, Widget* widget);
        static LockCount_t& lockCount();
        static void addRef(HWND hWnd);
        static void subRef(HWND hWnd);
        static HWND findWinHandler(Widget* widget);

        // 最初にシステムからメッセージが届くウィンドウプロシージャ
        static LRESULT CALLBACK entryProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT subProcedure(shared_ptr<Widget> widget_sp, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };


    class Event : public Object {};
    class CloseEvent : public Event {};
    class HideEvent : public Event {};
    class ShowEvent : public Event {};
    class KeyEvent : public Event {};
    class FocusEvent : public Event {};
    class ResizeEvent : public Event {};
    class MoveEvent : public Event {};
    class PaintEvent : public Event {};
    class MouseEvent : public Event {};
    class WheelEvent : public Event {};
    class DropEvent : public Event {};
    class DragEnterEvent : public Event {};
    class DragMoveEvent : public Event {};
    class DragLeaveEvent : public Event {};
    class TableEvent : public Event {}; 
}

#endif // __IS_EVENT_HPP__