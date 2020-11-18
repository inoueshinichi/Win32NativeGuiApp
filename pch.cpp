// プリコンパイル済みヘッダーのソースコード
#include "pch.hpp"

/**
 * 現在日時を文字列で返す.
 * \return 
 */
std::string return_current_datetime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    std::tm jit;
#ifdef WIN32
    errno_t error = localtime_s(&jit, &in_time_t);
#elif
    jit = *std::localtime(&in_time_t);
#endif
    ss << std::put_time(&jit, "%c");
    return ss.str();
}



