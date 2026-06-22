#pragma once

#include <cstdio>

#include "Constants.h"

/**
 * @brief Utility functions.
 * @namespace Utils
 * @author Kyle Wagler
 * @date 2026-06-06
 */
namespace Utils {
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
         * @brief Specify the file to lock.
         * @author Kyle Wagler
         * @date 2026-06-21
         */
        enum class FileType {
            DEBUG,      /**< Debug file. */
            ERROR,      /**< Error file. */
            INFO,       /**< Info file. */
            PRINT,      /**< Print to console. */
            WARNING,    /**< Warning file. */
        };
    
        /**
         * @brief Locks printing to prevent races.
         * @author Kyle Wagler
         * @date 2026-06-20
         */
        void LockPrint(Utils::Detail::FileType ft);
    
        /**
         * @brief Unlocks printing to allow another thread to print.
         * @author Kyle Wagler
         * @date 2026-06-20
         */
        void UnlockPrint(Utils::Detail::FileType ft);
    }

    #define FilePrintln(whichType, whichFile, initialMessage, ...) Utils::Detail::LockPrint(whichType); \
        std::print(whichFile, initialMessage); \
        std::println(whichFile, __VA_ARGS__); \
        Utils::Detail::UnlockPrint(whichType)
    
    #define    SyncPrintln(...) FilePrintln(Utils::Detail::FileType::PRINT,     stdout,                     "",             __VA_ARGS__)
    #define   DebugPrintln(...) FilePrintln(Utils::Detail::FileType::DEBUG,     Utils::Detail::debugFile,   "DEBUG:   ",    __VA_ARGS__)
    #define   ErrorPrintln(...) FilePrintln(Utils::Detail::FileType::ERROR,     Utils::Detail::errorFile,   "ERROR:   ",    __VA_ARGS__)
    #define    InfoPrintln(...) FilePrintln(Utils::Detail::FileType::INFO,      Utils::Detail::infoFile,    "INFO:    ",    __VA_ARGS__)
    #define WarningPrintln(...) FilePrintln(Utils::Detail::FileType::WARNING,   Utils::Detail::warningFile, "WARNING: ",    __VA_ARGS__)
}

