#include "win32_console.hpp"
#include "exception.hpp"


namespace Is
{
	/**
	 * コンストラクタ.
	 * 
	 * \param divideX
	 * \param divideY
	 * \return 
	 */
	Win32Console::Win32Console(int divideX, int divideY)
		: m_hWnd(NULL)
		, m_fpIn(nullptr)
		, m_fpOut(nullptr)
		, m_fpErr(nullptr)
		, m_fdIn(NULL)
		, m_fdOut(NULL)
		, m_fdErr(NULL)
		, m_hIn(NULL)
		, m_hOut(NULL)
		, m_hErr(NULL)
		, m_inputConsoleMode(0)
		, m_outputConsoleMode(0)
		, m_isStartUp(FALSE)
		, m_divideX(divideX)
		, m_divideY(divideY)
	{
		// コンソール起動
		if (InitConsole())
		{
			OutputDebugString(CString("\n<<<<< Open Console >>>>>\n"));
			m_isStartUp = TRUE;
		}

		// コンソールモードの取得
		if (!int(GetMode()))
		{
			OutputDebugString(CString("\n***** Console Mode Error *****\n"));
		}

		// ANSI エスケープシーケンスを有効化
		EvableANSIEscapeSequence(); // デフォルトで有効になっている

		// コンソールモードの確認
		ShowConsoleMode();

		OutputDebugString(CString("\n***** Win32Console Constructor! *****\n"));
	}

	/**
	 * デストラクタ.
	 * 
	 */
	Win32Console::~Win32Console()
	{
		if (m_isStartUp)
		{
			// プロセスに割当て済みのWin32コンソールを解放する.
			FreeConsole();
			OutputDebugString(CString("\n<<<<< Close Console >>>>>\n"));
		}
		OutputDebugString(CString("\n***** Win32Console Destructor! *****\n"));
	}


	/**
	 * コンソールへのシグナルハンドラ.
	 * 
	 * \param dwCtrlType
	 * \return 
	 */
	BOOL WINAPI Win32Console::ConsoleSignalHander(DWORD dwCtrlType)
	{
		if (dwCtrlType == CTRL_SHUTDOWN_EVENT)
		{
			// PCをシャットダウンする場合
			// ここに特定の処理を記述
		}
		else if (dwCtrlType == CTRL_LOGOFF_EVENT)
		{
			// PCをログオフする場合
			// ここに特定の処理を記述
		}
		else if (dwCtrlType == CTRL_CLOSE_EVENT)
		{
			// コンソールを消す場合(×ボタン)
			// ここに特定の処理を記述
		}
		else if (dwCtrlType == CTRL_BREAK_EVENT)
		{
			// Ctrl + Breakが押された場合
			// ここに特定の処理を記述
		}
		else if (dwCtrlType == CTRL_C_EVENT)
		{
			// Ctrl + Cが押された場合
			// ここに特定の処理を記述
		}
		return TRUE;
	}


	/**
	 * 入力/スクリーンバッファのモードを取得.
	 * 
	 * \return 
	 */
	Win32ConsoleStatus Win32Console::GetMode()
	{
		if (m_hWnd != NULL)
		{
			// コンソール入力バッファのモードを取得
			if (!GetConsoleMode(m_hIn, &m_inputConsoleMode))
			{
				// 失敗
				win32api_error(m_hWnd, CString("入力バッファモードの取得失敗")); // Win32 API エラーメッセージを取得
				return Win32ConsoleStatus::CNSL_ERR_INPUT_MODE;
			}

			// コンソールスクリーンバッファのモードを取得
			if (!GetConsoleMode(m_hOut, &m_outputConsoleMode))
			{
				// 失敗
				win32api_error(m_hWnd, CString("スクリーンバッファモードの取得失敗")); // Win32 API エラーメッセージを取得
				return Win32ConsoleStatus::CNSL_ERR_SCREEN_MODE;
			}
		}
		else
		{
			return Win32ConsoleStatus::CNSL_ERR_WINDOW_HANDLE;
		}

		return Win32ConsoleStatus::CNSL_OK;
	}


	/**
	 * コンソールでANSIエスケープを有効にする.
	 * Windows 10（TH2/1511以降）では条件付きでANSIエスケープシーケンスに対応.
	 * \return 
	 */
	Win32ConsoleStatus Win32Console::EvableANSIEscapeSequence()
	{
		if (m_hWnd != NULL)
		{
			/**
			 * https://qiita.com/mod_poppo/items/2ff384530c6f3215c635
			 * ANSIエスケープを有効にするために
			 * GetConsoleMode()のmodeに定数ENABLE_VIRTUAL_TERMINAL_PROCESSING / ENABLE_VIRTUALTERMINAL_INPUTを指定する
			 * 新しいWindowsコンソールではANSIエスケープシーケンスが有効になる.
			 * 古いWindowsコンソールではAPI呼び出しが失敗し、GetLastError()はERROR_INVALID_PARAMETER(0x57)を返す.
			 * GetConsoleMode()の戻り値 成功:0以外の数値, 失敗:0
			 */ 

			// // 標準入力がコマンドプロンプトに繋がっているかチェック
			// if (_isatty(m_fdIn))
			// {
			// 	if (SetConsoleMode(m_hIn, m_inputConsoleMode | ENABLE_VIRTUAL_TERMINAL_INPUT))
			// 	{
			// 		// 成功
			// 		OutputDebugString(CString("\n***** Input: Enable ANSI-Escape Sequence *****\n"));
			// 	}
			// 	else
			// 	{
			// 		// 失敗
			// 		OutputDebugString(CString("\n***** Input: Disable ANSI-Escape Sequence *****\n"));
			// 		ShowWin32APIError(m_hWnd, CString("入力バッファモードのANSI ESCAPEの有効化失敗")); // Win32 API エラーメッセージを取得
			// 		return Win32ConsoleStatus::CNSL_ERR_ANSI_ESCAPE_INPUT;
			// 	}
			// }
			// else
			// {
			// 	// 標準入力につながっていない
			// 	ShowWin32APIError(m_hWnd, CString("標準入力に未接続")); // Win32 API エラーメッセージを取得
			// 	return Win32ConsoleStatus::CNSL_ERR_NO_STD_INPUT;
			// }


			// 標準出力がコマンドプロンプトに繋がっているかチェック
			if (_isatty(m_fdOut))
			{
				// ANSI Escape Sequenceを有効化
				if (SetConsoleMode(m_hOut, m_outputConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
				{
					// 成功
					OutputDebugString(CString("\n***** Output: Enable ANSI-Escape Sequence *****\n"));
				}
				else
				{
					// 失敗
					OutputDebugString(CString("\n***** Output: Disable ANSI-Escape Sequence *****\n"));
					win32api_error(m_hWnd, CString("スクリーンバッファモードのANSI ESCAPEの有効化失敗.")); // Win32 API エラーメッセージを取得
					return Win32ConsoleStatus::CNSL_ERR_ANSI_ESCAPE_SCREEN;
				}
			}
			else
			{
				// 標準出力につながっていない
				win32api_error(m_hWnd, "標準出力に未接続"); // Win32 API エラーメッセージを取得
				return Win32ConsoleStatus::CNSL_ERR_NO_STD_OUTPUT;
			}

		}
		else
		{
			return Win32ConsoleStatus::CNSL_ERR_WINDOW_HANDLE;
		}

		return Win32ConsoleStatus::CNSL_OK;
	}


	/**
	 * 現在のコンソールモードの状態を表示.
	 * 
	 */
	void Win32Console::ShowConsoleMode()
	{
		if (m_hWnd != NULL)
		{
			/*
			BOOL GetConsoleMode(HANDLE hConsoleHandle, LPDWORD lpMode)関数

			[hConsoleHandleに入力バファfのハンドルを指定した場合，lpModeが指す変数に以下の値の組み合わせが格納される]
			ENABLE_LINE_INPUT				0x0002 : ReadFile関数またはReadConsole関数はキャリッジリターン文字を読み取ったときだけ制御を戻す。
			このモードが無効な場合，これらの関数は１つ以上の文字を読取ると制御を戻す。

			ENABLE_EXTENDED_FLAGS			0x0080 :


			ENABLE_INSERT_MODE				0x0020 :


			ENABLE_ECHO_INPUT				0x0004 : ReadFile関数またはReadConsole関数で文字を読取るたびに，その文字がアクティブなスクリーンバッファに書き込まれる。
			このモードを有効にできるのは, ENABLE_LINE_INPUTモードが有効な場合だけ。

			ENABLE_PROCESSED_INPUT			0x0001 : CTRL+Cがシステムによって処理され，入力バッファに格納されない。ReadFile関数またはReadConsole関数で入力バッファの読み取りを行う場合，
			他の制御キーはシステムによって処理され，ReadFile関数またはReadConsole関数のバッファに返されない。
			ENABLE_LINE_INPUTモードも有効な場合，バックスペース文字，キャリッジ・リターン文字，ラインフィード文字がシステムによって処理される。

			ENABLE_WINDOW_INPUT				0x0008 : コンソールスクリーンバッファのサイズを変更するためのユーザーとの対話がコンソールの入力バッファに報告される。これらのイベントに関する情報は，
			ReadConsoleInput関数を使って入力バッファから読み取ることができる。ReadFile関数とReadConsole関数は読み取ることができない。

			ENABLE_MOUSE_INPUT				0x0010 : マウスポインタがコンソールウィンドウの境界内にあり，ウィンドウがキーボードフォーカスを持っている場合，マウスを移動したりマウスボタンを押すことに
			　　　　　　　　　　　　　　　				 よって生成されるマウスイベントが入力バッファに置かれる。これらのイベントは，このモードが有効な場合でも，ReadFile関数やReadConsole関数によって破棄される。

			ENABLE_QUICK_EDIT_MODE			0x0040 :


			ENABLE_VIRTUALC_TERMINAL_INPUT	0x0200 :
			*/

			if (m_hIn != NULL)
			{
				OutputDebugString(CString("\n***** [ InputConsoleBuffer Mode ] *****\n"));
				CString isFlags;
				isFlags.Format(CString("\n***** ENABLE_LINE_INPUT             -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_LINE_INPUT) == ENABLE_LINE_INPUT) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_EXTEDNED_FLAGS         -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_EXTENDED_FLAGS) == ENABLE_EXTENDED_FLAGS) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_INSERT_MODE            -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_INSERT_MODE) == ENABLE_INSERT_MODE) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_ECHO_INPUT             -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_ECHO_INPUT) == ENABLE_ECHO_INPUT) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_PROCESSED_INPUT        -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_PROCESSED_INPUT) == ENABLE_PROCESSED_INPUT) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_MOUSE_INPUT            -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_MOUSE_INPUT) == ENABLE_MOUSE_INPUT) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_QUICK_EDIT_MODE        -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_QUICK_EDIT_MODE) == ENABLE_QUICK_EDIT_MODE) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_VIRTUALCStringERMINAL_INPUT -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_VIRTUAL_TERMINAL_INPUT) == ENABLE_VIRTUAL_TERMINAL_INPUT) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
			}

			/*
			[hConsoleHandleにスクリーンバッファのハンドルを指定した場合，lpModeが指す変数に以下の値の組み合わせが格納される]
			ENABLE_PROCESSED_OUTPUT				0x0001 : WriteFile関数またはWriteConsole関数で書き込む文字，ReadFile関数またはReadConsole関数でエコーする文字がASCII文字か解析され，
			正しい動作が実行される。バックスペース文字、タブ文字、ベル文字、キャリッジリターン文字、ラインフィード文字が処理される。
			ENABLE_WRAP_AT_EOL_OUTPUT			0x0002 : WriteFile関数またはWriteConsole関数で書き込みをする時，あるいはReadFile関数またはReadConsole関数でエコーする時，カーソルが
			　　　　　　　　　　　　　　　　　				 現在の行の終端に到達すると次の行の先頭に移動する。カーソルがウィンドウの最後の行から次の行に進む時，コンソールウィンドウに表示されている
											 行が自動的に上にスクロールする。また，カーソルがスクリーンバッファの最後の行から次の行に進む時，スクリーンバッファの内容が上にスクロールする。
											 (スクリーンバッファの一番上の行が破棄される)。このモードが無効になっていると，業の最後の文字にそれ以降の文字が上書きされる

											 ENABLE_VIRTUALCStringERMINAL_PROCESSING	0x0004 :

											 DISABLE_NEWLINE_AUTO_RETURN			0x0008 :

											 ENABLE_LVB_GRID_WORLDWIDE			0x0010 :
											 */

			if (m_hOut != NULL)
			{
				OutputDebugString(CString("\n***** [ ScreenConsoleBuffer Mode ] *****\n"));
				CString isFlags;
				isFlags.Format(CString("\n***** ENABLE_PROCESSED_OUTPUT            -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_PROCESSED_OUTPUT) == ENABLE_PROCESSED_OUTPUT) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_WRAP_AT_EOL_OUTPUT          -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_WRAP_AT_EOL_OUTPUT) == ENABLE_WRAP_AT_EOL_OUTPUT) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_VIRTUALCStringERMINAL_PROCESSING -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) == ENABLE_VIRTUAL_TERMINAL_PROCESSING) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
				isFlags.Format(CString("\n***** ENABLE_LVB_GRID_WORLDWIDE          -> %s *****\n"),
					((m_inputConsoleMode & ENABLE_LVB_GRID_WORLDWIDE) == ENABLE_LVB_GRID_WORLDWIDE) ? CString("Enable") : CString("Didable"));
				OutputDebugString(isFlags);
			}
		}

		return;
	}

	/**
	 * コンソールの初期化.
	 * 
	 * \return 
	 */
	BOOL Win32Console::InitConsole()
	{
		// 呼び出しプログラム(プロセス)にWin32コンソールを割り当てる
		if (!AllocConsole())
		{
			return FALSE;
		}

		// (C/C++) FILE Pointerの取得
		// FILEポインタに標準入力・標準出力・標準エラー出力を割り当てる
		freopen_s(&m_fpIn,  "CONIN$", "r", stdin);
		freopen_s(&m_fpOut, "CONOUT$", "w+", stdout); // "w+": 読み取りと書き込みの両方のモードでオープン
		freopen_s(&m_fpErr, "CONOUT$", "w", stderr);

		// (Unix) File Descriptorの取得
		m_fdIn  = _fileno(m_fpIn);
		m_fdOut = _fileno(m_fpOut);
		m_fdErr = _fileno(m_fpErr);

		// 標準入力・標準出力・標準エラー出力の(Windows) HANDLEを取得
		m_hIn  = (HANDLE)_get_osfhandle(m_fdIn);
		m_hOut = (HANDLE)_get_osfhandle(m_fdOut);
		m_hErr = (HANDLE)_get_osfhandle(m_fdErr);
		/**
		 * HANDLE WINAPI GetStdHandle(DWORD nStdHandle)の代替
		 * nStdHandleは、取得するハンドルのタイプを指定.
		 * STD_INPUT_HANDLEならば標準入力ハンドル
		 * STD_OUTPUT_HANDLEならば標準出力ハンドル
		 * STD_ERROR_HANDLEは標準エラーハンドル
		 * が返ってくる.
		 * 既にAllocConsoleが呼び出されている場合は、標準入力ハンドルが入力バッファのハンドルとなり、 
		 * 標準出力ハンドルがスクリーンバッファのハンドルになります。 取得したハンドルを閉じる必要はない.
		 * */


		// コンソールへのシグナルハンドラを設定
		::SetConsoleCtrlHandler(&Win32Console::ConsoleSignalHander, TRUE);

		// コンソールウィンドウのタイトルを取得
		TCHAR cnslTitle[FILESYSTEM_SOLIDMAXLENGTH];
		::GetConsoleTitle(cnslTitle, sizeof(cnslTitle));


		// タイトルからウィンドウを検索してウィンドウハンドルを取得
		m_hWnd = ::FindWindow(NULL, cnslTitle);

		// 現在のウィンドウ位置を取得
		RECT cnslWinRect;
		::GetWindowRect(m_hWnd, &cnslWinRect);

		// ウィンドウ位置を変更
		int cnslLeft = ::GetSystemMetrics(SM_CXSCREEN) / m_divideX;
		int cnslTop = ::GetSystemMetrics(SM_CYSCREEN) / m_divideY;
		int cnslWidth = (cnslWinRect.right - cnslWinRect.left);
		int cnslHeight = (cnslWinRect.bottom - cnslWinRect.top);
		::MoveWindow(m_hWnd, cnslLeft, cnslTop, cnslWidth, cnslHeight, TRUE);

		return TRUE;
	}

} // end namespace Is
