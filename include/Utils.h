#pragma once

#include <cstdio>

#include "Constants.h"

#define FILES_ALL_CONSOLE

/**
 * @brief Utility functions.
 * @namespace Utils
 * @author Kyle Wagler
 * @date 2026-06-06
 */
namespace Utils {

    /**
     * @brief Specify the level of logging.
     * @enum LogLevel
     * @author Kyle Wagler
     * @date 2026-06-21
     */
    enum class LogLevel {
        INFO,       /**< Info file. */
        DEBUG,      /**< Debug file. */
        WARNING,    /**< Warning file. */
        ERROR,      /**< Error file. */
        PRINT,      /**< Print to console. */
    };

    /**
     * @brief Calculate the minimum of 2 values of type `T`.
     * @param a The first value.
     * @param b The second value.
     * @return The smaller of the 2. Returns value `a` on equivalent.
     * @author Kyle Wagler
     * @date 2026-06-06
     */
    template <typename T>
    T Min(T a, T b) {
        return (b < a) ? b : a;
    }
    
    /**
     * @brief Calculate the maximum of 2 values of type `T`.
     * @param a The first value.
     * @param b The second value.
     * @return The larger of the 2. Returns value `a` on equivalent.
     * @author Kyle Wagler
     * @date 2026-06-06
     */
    template <typename T>
    T Max(T a, T b) {
        return (b > a) ? b : a;
    }
    
    /**
     * @brief Don't touch this.
     * @author Kyle Wagler
     * @date 2026-06-21
     */
    namespace Detail {
#ifdef FILES_ALL_CONSOLE
        inline FILE* debugFile      = stdout;
        inline FILE* errorFile      = stdout;
        inline FILE* infoFile       = stdout;
        inline FILE* warningFile    = stdout;
#else
        inline FILE* debugFile      = fopen("debug.log", "a");
        inline FILE* errorFile      = fopen("error.log", "a");
        inline FILE* infoFile       = fopen("info.log", "a");
        inline FILE* warningFile    = fopen("warning.log", "a");
#endif

        /**
         * @brief Locks printing to prevent races.
         * @param ll The desired output stream to write to.
         * @return `true` on valid to write in this thread.
         * @author Kyle Wagler
         * @date 2026-06-20
         */
        bool LockPrint(Utils::LogLevel ll);
        
        /**
         * @brief Unlocks printing to allow another thread to print.
         * @param ll The desired output stream to stop writing to.
         * @author Kyle Wagler
         * @date 2026-06-20
         */
        void UnlockPrint(Utils::LogLevel ll);
    }

    #define FilePrintln(whichType, whichFile, initialMessage, ...) \
        if (Utils::Detail::LockPrint(whichType)) {  \
            std::print(whichFile, initialMessage);  \
            std::println(whichFile, __VA_ARGS__);   \
            Utils::Detail::UnlockPrint(whichType);  \
        } do {} while (false)
    
    #define    SyncPrintln(...) FilePrintln(Utils::LogLevel::PRINT,     stdout,                     "",             __VA_ARGS__)
    #define   DebugPrintln(...) FilePrintln(Utils::LogLevel::DEBUG,     Utils::Detail::debugFile,   "DEBUG:   ",    __VA_ARGS__)
    #define   ErrorPrintln(...) FilePrintln(Utils::LogLevel::ERROR,     Utils::Detail::errorFile,   "ERROR:   ",    __VA_ARGS__)
    #define    InfoPrintln(...) FilePrintln(Utils::LogLevel::INFO,      Utils::Detail::infoFile,    "INFO:    ",    __VA_ARGS__)
    #define WarningPrintln(...) FilePrintln(Utils::LogLevel::WARNING,   Utils::Detail::warningFile, "WARNING: ",    __VA_ARGS__)

    void SetLogLevel(Utils::LogLevel ll);
}

