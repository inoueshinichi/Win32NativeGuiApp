cmake_minimum_required(VERSION 3.14.0)

# ---------- MSVC セッティング ---------- #

# ソースコードでUTF-8を使う(MSVC++)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# C++11以降を利用した場合にマクロ変数__cplusplusの値を利用中のC++バージョンに合わせる(つけないとC++98相当の値になる)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>")


# Encoding Manifest File UTF-8
set(ENC_MANIFEST_FILE "encoding_utf8.exe.manifest")

# WindowsのWinMainエントリーポイントを使用可能にする
set(CMAKE_WIN32_EXECUTABLE ON)

# ---------- MSVC セッティング ---------- #

# C++ 言語専用
project(Win32VSCodeCMake LANGUAGES CXX VERSION 0.1.0)

# 詳細情報の表示
set(CMAKE_VERBOSE_MAKEFILE ON)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++標準に準拠しない拡張機能を無効化
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--pedantic-errors")






# Header Files
file(GLOB INCLUDE_FILES "*.hpp *.h")
message("Header Files: ${INCLUDE_FILES}")

# Source Files
file(GLOB SRC_FILES "*.cpp")
message("Source Files: ${SRC_FILES}")



include(CTest)
enable_testing()

# 実行ファイル
add_executable(Win32VSCodeCMake
                "${ENC_MANIFEST_FILE}" # Encoding UTF-8 マニフェスト
                "${INCLUDE_FILES}"     # ヘッダーファイル
                "${SRC_FILES}"         # ソースファイル
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
