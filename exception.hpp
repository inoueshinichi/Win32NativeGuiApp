#ifndef __IS_EXCEPTION_HPP__
#define __IS_EXCEPTION_HPP__

#include "pch.hpp"

#if defined(_MSC_VER)
#define __func__ __FUNCTION__
#endif


namespace Is
{
    using std::string;
    using std::vector;
    using std::size_t;

    
#define IS_ERROR(code, msg, ...)                                                       \
    throw Exception(code, format_string(msg, ##__VA_ARGS__),                           \
                    __func__, __FILE__, __LINE__);

#define IS_CHECK(condition, code, msg, ...)                                            \
    if (!(condition))                                                                  \
    {                                                                                  \
        IS_ERROR(code, string("Failed `" #condition "`: ") + msg, ##__VA_ARGS__);      \
    }

#define IS_FORCE_ASSERT(condition, msg, ...)                                           \
    if (!(condition))                                                                  \
    {                                                                                  \
        std::cerr << "Aborting: " << format_string(msg, ##__VA__ARGS__)                \
        << " at " << __func__ << " in " << __FILE__ << ":" << __LINE__ << std::endl;   \
        std::abort();                                                                  \
    }


    enum class error_code
    {
        unclassified,
        not_implemenated,
        value,
        type,
        memory,
        io,
        os,
        target_specific,
        target_specific_async,
        runtime
    };

    
    string get_error_string(error_code code)
    {
        switch (code)
        {
#define CASE_ERROR_STRING(code_name)  \
    case error_code::code_name:       \
        return #code_name

    CASE_ERROR_STRING(unclassified);
    CASE_ERROR_STRING(not_implemenated);
    CASE_ERROR_STRING(value);
    CASE_ERROR_STRING(type);
    CASE_ERROR_STRING(memory);
    CASE_ERROR_STRING(io);
    CASE_ERROR_STRING(os);
    CASE_ERROR_STRING(target_specific);
    CASE_ERROR_STRING(target_specific_async);
    CASE_ERROR_STRING(runtime);

#undef CASE_ERROR_STRING
        }
        return string();
    }


    class Exception : public std::exception
    {
    protected:
        error_code code_;
        string full_msg_;
        string msg_;
        string func_;
        string file_;
        size_t line_;
    
    public:
        Exception(error_code code, const string& msg, const string& func, 
                  const string& file, size_t line)
            : code_(code), msg_(msg), func_(func), file_(file), line_(line)
        {
            std::ostringstream ss;
            ss << get_error_string(code_) << " error in " << func_ << std::endl
               << file_ << ":" << line_ << std::endl;
            full_msg_ = ss.str();
        }

        virtual ~Exception() throw() {}

        virtual const char* what() const throw()
        {
            return full_msg_.c_str();
        }
    }


    /**
     * Win32 API のエラーを捕捉してメッセージダイアログで通知.
     */
    void win32api_error(HWND hWnd = NULL, const CString& title = CString())
    {
        DWORD errCode = ::GetLastError();
        LPTSTR lpMsgBuff = nullptr; // エラーメッセージ用バッファポインタ

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | // テキストのメモリ割り当てを要求
            FORMAT_MESSAGE_FROM_SYSTEM |     // エラーメッセージはWindowsが用意しているものを使用
            FORMAT_MESSAGE_IGNORE_INSERTS,   // 次の引数を無視してエラーコードに対するエラーメッセージを作成する
            NULL,
            errCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 言語(日本語)を設定
            lpMsgBuff,                        // メッセージテキストが保存されるバッファへのポインタ
            0,
            NULL
        );

        if (hWnd)
        {
            MessageBox(hWnd, (LPCTSTR)lpMsgBuff, (LPCTSTR)title, MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            IS_ERROR(error_code::os, "Win32 API Error: %s", lpMsgBuff);
        }
        
        
        LocalFree(lpMsgBuff);
    }


    /**
     * フォーマット付き文字列の作成関数
     * */
    template <typename T, typename... Args>
    string format_string(const string& format, T first, Args... rest)
    {
        int size = std::snprintf(nullptr, 0, format.c_str(), first, rest...);
        if (size < 0)
        {
            std::printf("Faital error in format_string function: std::sprintf failed.\n");
            std::abort();
        }
        vector<char> buffer(size + 1); // null文字追加
        std::snprintf(buffer.data(), size + 1, format.c_str(), first, rest...);
        return string(buffer.data(), buffer.data() + size);
    }


    inline string format_string(const string& format)
    {
        for (auto iter = format.begin(); iter != format.end(); ++iter)
        {
            if (*iter == '%')
            {
                if (*(iter + 1) == '%')
                {
                    iter++;
                }
                else
                {
                    IS_ERROR(error_code::unclassified, "Invalid format string %s", format.c_str());
                }
            }
        }
        return format;
    }
}


#endif // __IS_EXCEPTION_HPP__