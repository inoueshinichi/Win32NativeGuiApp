#include "event.hpp"

namespace Is
{
    // static func
    EventDispatcher::Registry_t& EventDispatcher::widget()
    {
        static Registry_t widget_registry_ = Registry_t();
        return widget_registry_;
    }


    // static func
    void EventDispatcher::addWidget(HWND hWnd, Widget* widget)
    {
        Registry_t& widget_registry = EventDispatcher::widget();
        auto widget_sp = shared_ptr<Widget>(widget);
        widget_registry.emplace(hWnd, widget_sp);
    }


    // static func
    EventDispatcher::LockCount_t& EventDispatcher::lockCount()
    {
        static LockCount_t lock_count_ = LockCount_t();
        return lock_count_;
    }


    // static func
    void EventDispatcher::addRef(HWND hWnd)
    {
        LockCount_t& lock_count = EventDispatcher::lockCount();
        auto iter = lock_count.find(hWnd);
        if (iter == lock_count.end())
        {
            lock_count[hWnd] = (LONG)0;
        }

        // https://stackoverflow.com/questions/41362469/is-it-safe-to-deallocate-oneself-on-wm-ncdestroy
        InterlockedIncrement(&lock_count[hWnd]);
    }


    // static func
    void EventDispatcher::subRef(HWND hWnd)
    {
        LockCount_t& lock_count = EventDispatcher::lockCount();
        InterlockedDecrement(&lock_count[hWnd]);
        if (lock_count[hWnd] == 0)
        {
            lock_count.erase(hWnd);
        }
    }


    // static func
    HWND EventDispatcher::findWinHandler(Widget* widget)
    {
        EventDispatcher::Registry_t& widget_registry = EventDispatcher::widget();
        auto iter =  std::find_if(std::begin(widget_registry), 
                                  std::end(widget_registry),
                                  [&](const auto& kv) {if (kv->second == this) return kv; });

        if (iter != widget_registry.end())
            return iter->first;
        else
            return NULL;
    }


    ///////////////////////////////////////////////////////////////////////////
    // ************************** Window Messages ************************** //
    ///////////////////////////////////////////////////////////////////////////
    // static func
    LRESULT EventDispatcher::entryProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        // TODO: スレッドセーフにすること
        shared_ptr<Widget> widget_sp;
        LRESULT lr = NULL;

        // ウィンドウメッセージ処理
        switch (uMsg)
        {
            // ウィンドウが生成されたとき
            case WM_CREATE:
            {
                // lParamからlpCreateStructを抽出
                LPCREATESTRUCT lpCreateStruct = NULL;
                lpCreateStruct = (LPCREATESTRUCT)lParam;
                if (lpCreateStruct->lpCreateParams != NULL)
			    {
                    // Widget*とHWNDのマッピングを生成
                    Widget* widget = (Widget*)lpCreateStruct->lpCreateParams;
                    EventDispatcher::addWidget(hWnd, widget);
                    EventDispatcher::addRef(hWnd);

                    // 0x0001
			        // ウィンドウの生成時
                    // 注意) CreateWindow()に制御が戻る前に，呼ばれる
                    widget->onCreate();
                    return 0;
                }
            }
            
            // ウィンドウの生成以外はHWNDに紐付いたWidgetポインタを取得
            Registry_t& widget_registry = EventDispatcher::widget();
            if (widget_registry.find(hWnd) != widget_registry.end()) // O(NlogN)
            { 
                widget_sp = widget_registry[hWnd];
            }
            else
            {
                IS_ERROR(error_code::value, "Widget Pointer with HWND %zu is not registered.", hWnd);
            }
            
            case WM_NCDESTROY:
            {
                EventDispatcher::subRef(hWnd);
                widget_registry.erase(hWnd);  // shared_ptr<Widget>が他に参照されていなければ, instanceを削除
                
                if (widget_registry.size() == 0)
                {
                    // ウィンドウハンドルマップが空になった場合，メッセージループ終了
                    ::PostQuitMessage(0);
                }
            }
            break;

            default:
            {
                EventDispatcher::addRef(hWnd);
                lr = EventDispatcher::subProcedure(widget_sp, hWnd, uMsg, wParam, lParam);
                EventDispatcher::subRef(hWnd);
            }
        }

        return lr;
    }


    LRESULT EventDispatcher::subProcedure(shared_ptr<Widget> widget_sp, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    // Event
    #define GEN_EVENT(variable, CLASS)        \
        auto variable = make_shared<CLASS>();

        
        // GEN_EVENT(hide_event, HideEvent);
        // GEN_EVENT(event, Event);
        // GEN_EVENT(drop_event, DropEvent);
        // GEN_EVENT(drag_enter_event, DragEnterEvent);
        // GEN_EVENT(drag_move_event, DragMoveEvent);
        // GEN_EVENT(drag_leave_event, DragLeaveEvent);
        // GEN_EVENT(table_event, TableEvent);

   

        LRESULT lr;

        switch (uMsg)
        {
            //////////////////////////////////////////////////////////////////
		    // ************************** Window ************************** //
		    //////////////////////////////////////////////////////////////////
            case WM_CLOSE:
            {
                // 0x0010
			    // ウィンドウのクローズ時
                GEN_EVENT(close_event, CloseEvent);
                widget_sp->onClose(close_event);
            }
            break;

            case WM_DESTROY:
            {
                // 0x0002
			    // ウィンドウの破棄時
                widget_sp->onDestroy(); // WM_DESTROYの送信後にWM_NCDESTROYが来る
            }

            case WM_MOVE:
            {
                // 0x0003
                // ウィンドウの移動時
                GEN_EVENT(move_event, MoveEvent);
                widget_sp->onMove(move_event);
            }
            break;

            case WM_SIZE:
            {
                // 0x0005
                // ウィンドウのサイズの変更時
                GEN_EVENT(resize_event, ResizeEvent);
                widget_sp->onResize(resize_event);
            }
            break;

            case WM_ACTIVATE:
            {
                // 0x0006
                // ウィンドウのアクティブ状態の変更時
                GEN_EVENT(event, Event);
                widget_sp->onChange(event);
            }
            break;

            case WM_ENABLE:
            {
                // 0x000A
                // ウィンドウの有効/無効状態の変更時
                GEN_EVENT(event, Event);
                widget_sp->onChange(event);
            }
            break;

            case WM_SHOWWINDOW:
        	{
        		// 0x0018	
        		// ウィンドウの表示/非表示状態の変更時
                GEN_EVENT(show_event, ShowEvent);
                widget_sp->onShow(show_event);
        	}
        	break;

            case WM_SETFOCUS:
            {
                // 0x0007	
                // ウィンドウがフォーカス取得時
                GEN_EVENT(focus_event, FocusEvent);
                widget_sp->onFocusIn(focus_event);
            }
            break;

            case WM_KILLFOCUS:
            {
                // 0x0008	
                // ウィンドウがフォーカス喪失時
                GEN_EVENT(focus_event, FocusEvent);
                widget_sp->onFocusOut(focus_event);
            }
            break;

            case WM_GETMINMAXINFO:
            {
                // 0x0024	
                // ウィンドウの最大・最小サイズ、最大時の位置、最小トラッキングサイズなどを取得
            }
            break;

            case WM_SETTEXT:
            {
                // 0x000C	
                // ウィンドウの文字列(画面タイトルやコントロール値など)を設定
            }
            break;

            case WM_GETTEXT:
            {
                // 0x000D	
                // ウィンドウの文字列(画面タイトルやコントロール値など)を取得
            }
            break;

            case WM_GETTEXTLENGTH:
            {
                // 0x000E	
                // ウィンドウの文字列(画面タイトルやコントロール値など)のバイト数を取得
            }
            break;

            case WM_SETREDRAW:
            {
                // 0x000B	
                // ウィンドウの描画フラグの変更時
            }
            break;

            case WM_PAINT:
            {
                // 0x000F	
                // ウィンドウの描画時
                GEN_EVENT(paint_event, PaintEvent);
                widget_sp->onPaint(paint_event);
            }
            break;

            ///////////////////////////////////////////////////////////////////////////////
	        // ****************************** Mouse & Cusor ****************************** //
	        /////////////////////////////////////////////////////////////////////////////////
            case WM_SETCURSOR:
            {
                // 0x0020	
                // ウィンドウ内でカーソル移動時 (マウス入力がキャプチャされていない場合)
            }
            break;

            //case WM_MOUSEFIRST:
            //{
            //	// 0x0200	
            //	// マウスイベントの最初
            //}
            //break;

            case WM_MOUSEMOVE:
            {
                // 0x0200	
                // マウス移動時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMouseMove(mouse_event);
            }
            break;

            case WM_LBUTTONDOWN:
            {
                // 0x0201	
                // マウス左クリック開始時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMousePress(mouse_event);
            }
            break;

            case WM_LBUTTONUP:
            {
                // 0x0202
                // マウス左クリック終了時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMouseRelease(mouse_event);
            }
            break;

            case WM_LBUTTONDBLCLK:
            {
                // 0x0203	
                // マウス左ダブルクリック時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMouseDbouleClick(mouse_event);
            }
            break;

            case WM_RBUTTONDOWN:
            {
                // 0x0204	
                // マウス右クリック開始時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMousePress(mouse_event);
            }
            break;

            case WM_RBUTTONUP:
            {
                // 0x0205	
                // マウス右クリック終了時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMouseRelease(mouse_event);
            }
            break;

            case WM_RBUTTONDBLCLK:
            {
                // 0x0206	
                // マウス右ダブルクリック時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMouseDbouleClick(mouse_event);
            }
            break;

            case WM_MBUTTONDOWN:
            {
                // 0x0207	
                // マウス中クリック開始時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMousePress(mouse_event);
            }
            break;

            case WM_MBUTTONUP:
            {
                // 0x0208	
                // マウス中クリック終了時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMouseRelease(mouse_event);
            }
            break;

            case WM_MBUTTONDBLCLK:
            {
                // 0x0209	
                // マウス中ダブルクリック時
                GEN_EVENT(mouse_event, MouseEvent);
                widget_sp->onMouseDbouleClick(mouse_event);
            }
            break;

            case WM_XBUTTONDOWN:
            {
                // 0x020B	
                // マウス拡張ボタンクリック開始時 ※Win2000(ver5.0)～
            }
            break;

            case WM_XBUTTONUP:
            {
                // 0x020C	
                // マウス拡張ボタンクリック終了時 ※Win2000(ver5.0)～
            }
            break;

            case WM_XBUTTONDBLCLK:
            {
                // 0x020D
                // マウス拡張ボタンダブルクリック時 ※Win2000(ver5.0)～
            }
            break;

            case WM_MOUSEWHEEL:
            {
                // 0x020A	
                // 縦マウスホイール時 ※Win95(ver4.0)～
                GEN_EVENT(wheel_event, WheelEvent);
                widget_sp->onWheel(wheel_event);
            }
            break;

            case WM_MOUSEHWHEEL:
            {
                // 0x020E	
                // 横マウスホイール時 ※WinVista(ver6.0)～
                GEN_EVENT(wheel_event, WheelEvent);
                widget_sp->onWheel(wheel_event);
            }
            break;

            //case WM_MOUSELAST:
            //{
            //	// 0x020D	
            //	// マウスイベントの最後 ※Win2000(ver5.0)～
            //}
            //break;

            case WM_MOUSEHOVER:
            {
                // 0x02A1	
                // クライアント領域内にカーソルが一定時間ある時 ※Win2000(ver5.0)～
            }
            break;

            case WM_MOUSELEAVE:
            {
                // 0x02A3	
                // クライアント領域外にカーソルが移動した時 ※Win2000(ver5.0)～
            }
            break;

            case WM_NCMOUSEMOVE:
            {
                // 0x00A0	
                // 非クライアント領域(タイトルバー等)でのカーソル移動時
            }
            break;

            case WM_NCLBUTTONDOWN:
            {
                // 0x00A1	
                // 非クライアント領域(タイトルバー等)でのマウス左クリック開始時
            }
            break;

            case WM_NCLBUTTONUP:
            {
                // 0x00A2	
                // 非クライアント領域(タイトルバー等)でのマウス左クリック終了時
            }
            break;

            case WM_NCLBUTTONDBLCLK:
            {
                // 0x00A3	
                // 非クライアント領域(タイトルバー等)でのマウス左ダブルクリック時
            }
            break;

            case WM_NCRBUTTONDOWN:
            {
                // 0x00A4	
                // 非クライアント領域(タイトルバー等)でのマウス右クリック開始時
            }
            break;

            case WM_NCRBUTTONUP:
            {
                // 0x00A5	
                // 非クライアント領域(タイトルバー等)でのマウス右クリック終了時
            }
            break;

            case WM_NCRBUTTONDBLCLK:
            {
                // 0x00A6
                // 非クライアント領域(タイトルバー等)でのマウス右ダブルクリック時
            }
            break;

            case WM_NCMBUTTONDOWN:
            {
                // 0x00A7	
                // 非クライアント領域(タイトルバー等)でのマウス中クリック開始時
            }
            break;

            case WM_NCMBUTTONUP:
            {
                // 0x00A8	
                // 非クライアント領域(タイトルバー等)でのマウス中クリック終了時
            }
            break;

            case WM_NCMBUTTONDBLCLK:
            {
                // 0x00A9	
                // 非クライアント領域(タイトルバー等)でのマウス中ダブルクリック時
            }
            break;

            case WM_NCXBUTTONDOWN:
            {
                // 0x00AB	
                // 非クライアント領域(タイトルバー等)でのマウス拡張ボタンクリック開始時 ※Win2000(ver5.0)～
            }
            break;

            case WM_NCXBUTTONUP:
            {
                // 0x00AC	
                // 非クライアント領域(タイトルバー等)でのマウス拡張ボタンクリック終了時 ※Win2000(ver5.0)～
            }
            break;

            case WM_NCXBUTTONDBLCLK:
            {
                // 0x00AD	
                // 非クライアント領域(タイトルバー等)でのマウス拡張ボタンダブルクリック時 ※Win2000(ver5.0)～
            }
            break;

            case WM_NCMOUSEHOVER:
            {
                // 0x02A0
                // 非クライアント領域(タイトルバー等)内にカーソルが一定時間ある時 ※Win2000(ver5.0)～
            }
            break;

            case WM_NCMOUSELEAVE:
            {
                // 0x02A2	
                // 非クライアント領域(タイトルバー等)外にカーソルが移動した時 ※Win2000(ver5.0)～
            }
            break;

            ///////////////////////////////////////////////////////////////////////
            // ****************************** Key ****************************** //
            ///////////////////////////////////////////////////////////////////////
            //case WM_KEYFIRST:
            //{
            //	// 0x0100	
            //	// キーイベントの最初
            //}
            //break;

            case WM_KEYDOWN:
            {
                // 0x0100
                // キー押下開始時
                GEN_EVENT(key_event, KeyEvent);
                widget_sp->onKeyPress(key_event);
            }
            break;

            case WM_KEYUP:
            {
                // 0x0101	
                // キー押下終了時
                GEN_EVENT(key_event, KeyEvent);
                widget_sp->onKeyRelease(key_event);
            }
            break;

            case WM_CHAR:
            {
                // 0x0102	
                // 仮想キーコード(UTF-16)を文字変換時
            }
            break;

            case WM_DEADCHAR:
            {
                // 0x0103	
                // 仮想キーコード(デッドキー)を文字変換時
            }
            break;

            case WM_SYSKEYDOWN:
            {
                // 0x0104	
                // システムキー押下開始時
                GEN_EVENT(key_event, KeyEvent);
                widget_sp->onKeyPress(key_event);
            }
            break;

            case WM_SYSKEYUP:
            {
                // 0x0105	
                // システムキー押下終了時
                GEN_EVENT(key_event, KeyEvent);
                widget_sp->onKeyRelease(key_event);
            }
            break;

            case WM_SYSCHAR:
            {
                // 0x0106	
                // 仮想キーコード(システムキー)を文字変換時
            }
            break;

            case WM_SYSDEADCHAR:
            {
                // 0x0107	
                // 仮想キーコード(システムキー＋デッドキー)を文字変換時
            }
            break;

            case WM_UNICHAR:
            {
                // 0x0109	
                // 仮想キーコード(UTF-32)を文字変換時 ※WinXP(ver5.1)～
            }
            break;

            //case WM_KEYLAST:
            //{
            //	// 0x0109	
            //	// キーイベントの最後 ※WinXP(ver5.1)～
            //}
            //break;

            ////////////////////////////////////////////////////////////////////////////////////////////
            // ****************************** Command & Timer & HotKey ****************************** //
            ////////////////////////////////////////////////////////////////////////////////////////////
            case WM_COMMAND:
            {
                // 0x0111	
                // メニュー操作やアクセラレータキー操作、コントロールから親ウィンドウへ通知時
            }
            break;

            case WM_SYSCOMMAND:
            {
                // 0x0112	
                // ウィンドウやメニューの制御時
            }
            break;

            case WM_APPCOMMAND:
            {
                // 0x0319	
                // ※Win2000(ver5.0)～
            }
            break;

            case WM_TIMER:
            {
                // 0x0113	
                // 設定された間隔でのタイマー処理の実行時
            }
            break;

            case WM_SETHOTKEY:
            {
                // 0x0032	
                // ウィンドウにホットキーを設定
            }
            break;

            case WM_GETHOTKEY:
            {
                // 0x0033	
                // ウィンドウかホットキーを取得
            }
            break;

            case WM_HOTKEY:
            {
                // 0x0312
                // 設定されたホットキーを押下時
            }
            break;

            //////////////////////////////////////////////////////////////////////////////////
            // ****************************** Control & Menu ****************************** //
            //////////////////////////////////////////////////////////////////////////////////
            case WM_CUT:
            {
                // 0x0300	
                // テキストボックス等で「切り取り」の実行時
            }
            break;

            case WM_COPY:
            {
                // 0x0301
                // テキストボックス等で「コピー」の実行時
            }
            break;

            case WM_PASTE:
            {
                // 0x0302	
                // テキストボックス等で「貼り付け」の実行時
            }
            break;

            case WM_CLEAR:
            {
                // 0x0303	
                // テキストボックス等で「削除」の実行時
            }
            break;

            case WM_UNDO:
            {
                // 0x0304
                // テキストボックス等で「元に戻す」の実行時
            }
            break;

            case WM_HSCROLL:
            {
                // 0x0114	
                // 横スクロール時
            }
            break;

            case WM_VSCROLL:
            {
                // 0x0115	
                // 縦スクロール時
            }
            break;

            case WM_INITMENU:
            {
                // 0x0116
                // メニューがアクティブとなる時
            }
            break;

            case WM_INITMENUPOPUP:
            {
                // 0x0117	
                // ポップアップメニューがアクティブとなる時
            }
            break;

            case WM_MENUSELECT:
            {
                // 0x011F	
                // メニューの項目を選択時
            }
            break;

            case WM_MENUCHAR:
            {
                // 0x0120	
                // メニューでキー入力時
            }
            break;

            case WM_MENURBUTTONUP:
            {
                // 0x0122	
                // メニュー項目でマウス右クリック終了時 ※Win2000(ver5.0)～ / WinCE未対応
            }
            break;

            case WM_MENUDRAG:
            {
                // 0x0123	
                // メニュー項目をドラッグ開始時 メニュースタイル(dwStyle)にMNS_DRAGDROPを指定時のみ ※Win2000(ver5.0)～ / WinCE未対応
            }
            break;

            case WM_CONTEXTMENU:
            {
                // 0x007B	
                // ウィンドウでマウス右クリック ここでコンテキストメニュー表示処理を実装 ※Win95(ver4.0)～
            }
            break;

            case WM_MENUGETOBJECT:
            {
                // 0x0124	
                // ※Win2000(ver5.0)～ / WinCE未対応
            }
            break;

            case WM_UNINITMENUPOPUP:
            {
                // 0x0125	
                // ドロップダウンメニューまたはサブメニュー破棄時 ※Win2000(ver5.0)～ / WinCE未対応
            }
            break;

            case WM_MENUCOMMAND:
            {
                //0x0126	
                // メニュー操作から親ウィンドウへ通知時 メニュースタイル(dwStyle)にMNS_NOTIFYBYPOSを指定時のみ ※Win2000(ver5.0)～ / WinCE未対応
            }
            break;

            case MN_GETHMENU:
            {
                // 0x01E1	
                // メニューハンドル取得
            }
            break;

            case WM_ENTERMENULOOP:
            {
                // 0x0211	
                // メニューループ開始時(メニュー表示時)
            }
            break;

            case WM_EXITMENULOOP:
            {
                // 0x0212	
                // メニューループ終了時(選択せずにメニュー終了時)
            }
            break;

            case WM_NEXTMENU:
            {
                // 0x0213
                // ←または→キーでメニューバーとシステムメニューが切り替わり時
                // (例えばメニューバーの最左メニュー項目で←キー押下すると画面最大化・最小化などシステムメニューが表示される時など)
                // ※Win95(ver4.0)～
            }
            break;

            case WM_DRAWITEM:
            {
                // 0x002B	
                // コントロールまたはメニューの項目の描画時
            }
            break;

            case WM_MEASUREITEM:
            {
                // 0x002C	
                // コントロールまたはメニューの項目のサイズの計算時
            }
            break;

            case WM_DELETEITEM:
            {
                // 0x002D	
                // コントロールまたはメニューの項目の削除時
            }
            break;

            case WM_VKEYTOITEM:
            {
                // 0x002E
                // LBS_WANTKEYBOARDINPUTのスタイルを設定されたリストボックスの項目に仮想キーを設定時
            }
            break;

            case WM_CHARTOITEM:
            {
                // 0x002F
                // LBS_WANTKEYBOARDINPUTのスタイルを設定されたリストボックスでキー入力時
            }
            break;

            ///////////////////////////////////////////////////////////////////////////////
            // ****************************** Dialg & MDI ****************************** //
            ///////////////////////////////////////////////////////////////////////////////
            case WM_INITDIALOG:
            {
                // 0x0110	
                // ダイアログボックスの初期化時
                MessageBox(hWnd, _T("ダイアログ"), _T("生成 in WindowProc"), MB_OK);
            }
            break;
        
            case WM_NEXTDLGCTL:
            {
                // 0x0028	
                // ダイアログ画面で次のコントロールにキーボードフォーカスを設定
            }
            break;
        
            case WM_ENTERIDLE:
            {
                // 0x0121
                // モーダルダイヤログ画面またはメニューがアイドル状態となる時
            }
            break;
        
            case WM_CHILDACTIVATE:
            {
                // 0x0022	
                // MDIの子ウィンドウがアクティブとなる時
            }
            break;
        
            case WM_MDICREATE:
            {
                // 0x0220
            }
            break;
        
            case WM_MDIDESTROY:
            {
                // 0x0221
            }
            break;
        
            case WM_MDIACTIVATE:
            {
                // 0x0222
            }
            break;
        
            case WM_MDIRESTORE:
            {
                // 0x0223
            }
            break;
        
            case WM_MDINEXT:
            {
                // 0x0224
            }
            break;
        
            case WM_MDIMAXIMIZE:
            {
                // 0x0225
            }
            break;
        
            case WM_MDITILE:
            {
                // 0x0226
            }
            break;
        
            case WM_MDICASCADE:
            {
                // 0x0227
            }
            break;
        
            case WM_MDIICONARRANGE:
            {
                // 0x0228
            }
            break;
        
            case WM_MDIGETACTIVE:
            {
                // 0x0229
            }
            break;
        
            case WM_MDISETMENU:
            {
                // 0x0230
            }
            break;
        
            case WM_MDIREFRESHMENU:
            {
                // 0x0234
            }
            break;

            /////////////////////////////////////////////////////////////////////////////////////////////
            // ****************************** Font & Icon & Print & IME ****************************** //
            /////////////////////////////////////////////////////////////////////////////////////////////
            case WM_SETFONT:
            {
                // 0x0030	
                // ウィンドウにフォントを設定
            }
            break;

            case WM_GETFONT:
            {
                // 0x0031	
                // ウィンドウの現在のフォントを取得
            }
            break;

            case WM_PAINTICON:
            {
                // 0x0026	
                // アイコンの描画時
            }
            break;

            case WM_ICONERASEBKGND:
            {
                // 0x0027	
                // アイコンの描画のクリア時 (背景用のブラシで塗りつぶし)
            }
            break;

            case WM_GETICON:
            {
                // 0x007F	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_SETICON:
            {
                //0x0080	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_SPOOLERSTATUS:
            {
                // 0x002A	
                // 印刷のキューにジョブが追加または削除時
            }
            break;

            case WM_PRINT:
            {
                // 0x0317	
                // プリンタ印刷での描画時 ※Win95(ver4.0)～
            }
            break;

            case WM_IME_STARTCOMPOSITION:
            {
                // 0x010D	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_ENDCOMPOSITION:
            {
                // 0x010E	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_COMPOSITION:
            {
                // 0x010F
                // ※Win95(ver4.0)～
            }
            break;

            //case WM_IME_KEYLAST:
            //{
            //	// 0x010F	
            //	// ※Win95(ver4.0)～
            //}
            //break;

            case WM_IME_SETCONTEXT:
            {
                // 0x0281	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_NOTIFY:
            {
                // 0x0282	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_CONTROL:
            {
                // 0x0283
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_COMPOSITIONFULL:
            {
                // 0x0284	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_SELECT:
            {
                // 0x0285	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_CHAR:
            {
                // 0x0286	
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_REQUEST:
            {
                // 0x0288	
                // ※Win2000(ver5.0)～
            }
            break;

            case WM_IME_KEYDOWN:
            {
                // 0x0290
                // ※Win95(ver4.0)～
            }
            break;

            case WM_IME_KEYUP:
            {
                // 0x0291	
                // ※Win95(ver4.0)～
            }
            break;

            ////////////////////////////////////////////////////////////////////////////////////////////
            // ****************************** Application & Windows OS ****************************** //
            ////////////////////////////////////////////////////////////////////////////////////////////
            case WM_QUIT:
            {
                // 0x0012	
                // アプリケーションの終了(メッセージループ終了)
                // PostQuitMessage関数を実行時に発生
                // ※メッセージループが終了するのでウィンドウプロシージャまで来ない
            }
            break;

            case WM_ACTIVATEAPP:
            {
                // 0x001C	
                // アプリケーションのアクティブ状態の変更時
            }
            break;

            case WM_FONTCHANGE:
            {
                // 0x001D	
                // アプリケーションのフォントリソース変更時
            }
            break;

            case WM_QUERYENDSESSION:
            {
                // 0x0011	
                // Windowsセッションの終了時(シャットダウンやログオフ時)
                // Windowsセッションの終了の中止処理をここで行う
                // ※WinCE未対応
            }
            break;

            case WM_ENDSESSION:
            {
                // 0x0016	
                // WM_QUERYENDSESSIONでWindowsセッションの終了を中止しなかった時
                // ※WinCE未対応
            }
            break;

            case WM_SYSCOLORCHANGE:
            {
                // 0x0015	
                // システムカラーの設定が変更時
            }
            break;

            //case WM_WININICHANGE:
            //{
            //	// 0x001A	
            //	// WIN.INIファイルの変更時
            //}
            //break;

            case WM_SETTINGCHANGE:
            {
                // 0x001A	コントロールパネルで設定変更時
                // ※Win95(ver4.0)～
            }
            break;

            case WM_DEVMODECHANGE:
            {
                // 0x001B	
                // デバイスモード変更時
            }
            break;

            case WM_TIMECHANGE:
            {
                // 0x001E
                // システム時刻の変更時
            }
            break;

            /////////////////////////////////////////////////////////////////////////////////
            // ****************************** Vista & 7 & 8 ****************************** //
            /////////////////////////////////////////////////////////////////////////////////
            case WM_DWMCOMPOSITIONCHANGED:
            {
                // 0x031E	
                // ※WinVista(ver6.0)～
            }
            break;

            default:
            {
                // Windows APIのDefWindowProcに処理を任せる
                lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        } // end switch(uMsg)

        return lr;
    #undef GEN_EVENT
    }

}