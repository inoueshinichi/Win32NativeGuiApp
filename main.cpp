#include "pch.hpp"
// #include "win32_console.hpp"
#include "application.hpp"

// ::GetModuleHandle(NULL)を呼び出せば, WinMain()を使用する必要はない.
// main()エントリーポイントにすれば, 標準入出力がデフォルトで使用できるので、デバッグ楽ちん.
// int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) 
int main(int argc, char** argv)
{
    // 通常文字の入出力をShift-JISからUTF-8にする
    std::locale::global(std::locale(""));
    
    using namespace Is;
    Application app(argc, argv);
    
    std::cout << "Hello, world!\n";

    Sleep(10000); // 10[s]
    return app.exe();
}