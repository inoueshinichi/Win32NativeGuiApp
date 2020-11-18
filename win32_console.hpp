// Windows GUIアプリ専用のコンソール出力
#ifndef __WIN32_CONSOLE_HPP__
#define __WIN32_CONSOLE_HPP__

#include "pch.hpp"

#include <cstdio>
    

namespace Is
{
    /**
    * WindowsコンソールのANSI エスケープシーケンス対応のプリプロセッサ.
    * Windows SDKが古いバージョンだと, ENABLE_VIRTUAL_TERMINAL_INPUT / ENABLE_VIRTUAL_TERMINAL_PROCESSING は宣言されていない.
    */
#	ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#		define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#	endif

#	ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#		define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0004
#	endif

#	ifndef ENABLE_LVB_GRID_WORLDWIDE
#		define ENABLE_LVB_GRID_WORLDWIDE 0x0010
#	endif

    // 明示的なキャストが必要になる.
	// enum class 名を明示的なスコープとして指定する必要がある.
    enum class Win32ConsoleStatus : int
	{
		CNSL_OK = 0,
		CNSL_ERR_WINDOW_HANDLE,
		CNSL_ERR_INPUT_MODE,
		CNSL_ERR_SCREEN_MODE,
		CNSL_ERR_NO_STD_INPUT,
		CNSL_ERR_NO_STD_OUTPUT,
		CNSL_ERR_NO_STD_ERROR,
		CNSL_ERR_ANSI_ESCAPE_INPUT,
		CNSL_ERR_ANSI_ESCAPE_SCREEN,
	};

    class Win32Console
	{
		HWND  m_hWnd;  // Windows Handler
		FILE* m_fpIn;  // Input File Pointer
		FILE* m_fpOut; // Output File Pointer
		FILE* m_fpErr; // Error FIle Pointer
		int   m_fdIn;  // Input File Descriptor
		int   m_fdOut; // Output File Descriptor
		int   m_fdErr; // Erro FIle Descriptor
		HANDLE m_hIn;
		HANDLE m_hOut;
		HANDLE m_hErr;

		DWORD m_inputConsoleMode;
		DWORD m_outputConsoleMode;
		int   m_divideX;
		int   m_divideY;
		BOOL  m_isStartUp;
		const static int FILESYSTEM_SOLIDMAXLENGTH = 2048;

		BOOL InitConsole();

	public:
		Win32Console(int divideX, int divideY);
		virtual ~Win32Console();
		static BOOL WINAPI ConsoleSignalHander(DWORD dwCtrlType);

		Win32ConsoleStatus GetMode();
		Win32ConsoleStatus EvableANSIEscapeSequence();
		void ShowConsoleMode();
	};
}
#endif // __WIN32_CONSOLE_HPP__