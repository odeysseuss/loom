/*
* Logging utils
*
* USAGE:
*   #define BUILD_DEBUG
*   #include "pool.h"
*
* logTrace(), logDebug() && logInfo() only works when BUILD_DEBUG is defined
* you can disable logWarn() by defining DISABLE_WARN_ON_RELEASE
*
* WARNING: Might not work in some compilers
*/

#ifndef LOG_H
#define LOG_H

#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"

#define COLOR_LIGHT "\x1b[38;2;213;196;161m"
#define COLOR_PINK "\x1b[38;2;211;134;155m"
#define COLOR_BLUE "\x1b[38;2;128;170;158m"
#define COLOR_GREEN "\x1b[38;2;176;184;70m"
#define COLOR_YELLOW "\x1b[38;2;233;177;67m"
#define COLOR_ORANGE "\x1b[38;2;254;128;25m"
#define COLOR_RED "\x1b[38;2;242;89;75m"

#define COLOR_TRACE ANSI_BOLD COLOR_PINK "[TRACE] " ANSI_RESET
#define COLOR_DEBUG ANSI_BOLD COLOR_BLUE "[DEBUG] " ANSI_RESET
#define COLOR_INFO ANSI_BOLD COLOR_GREEN "[INFO ] " ANSI_RESET
#define COLOR_WARN ANSI_BOLD COLOR_YELLOW "[WARN ] " ANSI_RESET
#define COLOR_ERROR ANSI_BOLD COLOR_ORANGE "[ERROR] " ANSI_RESET
#define COLOR_FATAL ANSI_BOLD COLOR_RED "[FATAL] " ANSI_RESET

#ifdef BUILD_DEBUG

#    define logTrace(fmt, ...)                                    \
        do {                                                      \
            printf(COLOR_TRACE ANSI_BOLD COLOR_BLUE               \
                   "%s:%d <%s> " COLOR_LIGHT fmt "\n" ANSI_RESET, \
                   __FILE__,                                      \
                   __LINE__,                                      \
                   __FUNCTION__,                                  \
                   ##__VA_ARGS__);                                \
        } while (0)

#    define logDebug(fmt, ...)                                    \
        do {                                                      \
            printf(COLOR_DEBUG ANSI_BOLD COLOR_BLUE               \
                   "%s:%d <%s> " COLOR_LIGHT fmt "\n" ANSI_RESET, \
                   __FILE__,                                      \
                   __LINE__,                                      \
                   __FUNCTION__,                                  \
                   ##__VA_ARGS__);                                \
        } while (0)

#    define logInfo(fmt, ...)                                     \
        do {                                                      \
            printf(COLOR_INFO ANSI_BOLD COLOR_BLUE                \
                   "%s:%d <%s> " COLOR_LIGHT fmt "\n" ANSI_RESET, \
                   __FILE__,                                      \
                   __LINE__,                                      \
                   __FUNCTION__,                                  \
                   ##__VA_ARGS__);                                \
        } while (0)

#else // BUILD_DEBUG

#    define logTrace(...)
#    define logDebug(...)
#    define logInfo(...)

#endif // BUILD_DEBUG

#ifndef DISABLE_WARN_ON_RELEASE

#    define logWarn(fmt, ...)                                     \
        do {                                                      \
            printf(COLOR_WARN ANSI_BOLD COLOR_BLUE                \
                   "%s:%d <%s> " COLOR_LIGHT fmt "\n" ANSI_RESET, \
                   __FILE__,                                      \
                   __LINE__,                                      \
                   __FUNCTION__,                                  \
                   ##__VA_ARGS__);                                \
        } while (0)
#else // !DISABLE_WARN_ON_RELEASE

#    define logWarn(...)

#endif // !DISABLE_WARN_ON_RELEASE

#define logError(fmt, ...)                                                    \
    do {                                                                      \
        printf(COLOR_ERROR ANSI_BOLD COLOR_BLUE "%s:%d <%s> " COLOR_LIGHT fmt \
                                                "\n" ANSI_RESET,              \
               __FILE__,                                                      \
               __LINE__,                                                      \
               __FUNCTION__,                                                  \
               ##__VA_ARGS__);                                                \
    } while (0)

#define logFatal(fmt, ...)                                                    \
    do {                                                                      \
        printf(COLOR_FATAL ANSI_BOLD COLOR_BLUE "%s:%d <%s> " COLOR_LIGHT fmt \
                                                "\n" ANSI_RESET,              \
               __FILE__,                                                      \
               __LINE__,                                                      \
               __FUNCTION__,                                                  \
               ##__VA_ARGS__);                                                \
    } while (0)

#endif
