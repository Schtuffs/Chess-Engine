#pragma once

#include <cstdio>
#include <print>

#include "Types/Types.h"

#define UTILS_LOG_CONSOLE

/**
 * @brief Utility functions.
 * @namespace Utils
 * @date 2026-06-06
 */
namespace Utils {

constexpr u8 CalculateIndex(Colour col, PieceType type) { return u8(type + (col * 8)); }

/**
 * @brief Checks if an `Square` is valid.
 * @param sq The `Square` to check.
 * @return `true` on valid.
 * @date 2026-07-22
 */
constexpr bool IsValidSquare(Square sq) { return (int)sq < SQ_TOTAL; }

/**
 * @brief Specify the level of logging.
 * @enum LogLevel
 * @date 2026-06-21
 */
enum class LogLevel {
    INFO,    /**< Info file. */
    DEBUG,   /**< Debug file. */
    WARNING, /**< Warning file. */
    ERROR,   /**< Error file. */
    PRINT,   /**< Print to console. */
};

/**
 * @brief Don't touch this.
 * @date 2026-06-21
 */
namespace Detail {
#ifdef UTILS_LOG_CONSOLE
inline FILE* debugFile   = stdout;
inline FILE* errorFile   = stdout;
inline FILE* infoFile    = stdout;
inline FILE* warningFile = stdout;
#else
inline FILE* debugFile   = fopen("debug.log", "a");
inline FILE* errorFile   = fopen("error.log", "a");
inline FILE* infoFile    = fopen("info.log", "a");
inline FILE* warningFile = fopen("warning.log", "a");
#endif

/**
 * @brief Locks printing to prevent races.
 * @param ll The desired output stream to write to.
 * @return `true` on valid to write in this thread.
 * @date 2026-06-20
 */
bool LockPrint(Utils::LogLevel ll);

/**
 * @brief Unlocks printing to allow another thread to print.
 * @param ll The desired output stream to stop writing to.
 * @date 2026-06-20
 */
void UnlockPrint(Utils::LogLevel ll);
} // namespace Detail

#define FilePrintln(whichType, whichFile, initialMessage, ...)                                     \
    if (Utils::Detail::LockPrint(whichType)) {                                                     \
        std::print(whichFile, initialMessage);                                                     \
        std::println(whichFile, __VA_ARGS__);                                                      \
        Utils::Detail::UnlockPrint(whichType);                                                     \
    }                                                                                              \
    do {                                                                                           \
    } while (false)
#ifdef UTILS_LOG_NONE
#define DebugPrintln(...) /* __VA_ARGS__ */                                                        \
    do {                                                                                           \
    } while (false)
#define ErrorPrintln(...) /* __VA_ARGS__ */                                                        \
    do {                                                                                           \
    } while (false)
#define InfoPrintln(...) /* __VA_ARGS__ */                                                         \
    do {                                                                                           \
    } while (false)
#define WarningPrintln(...) /* __VA_ARGS__ */                                                      \
    do {                                                                                           \
    } while (false)
#else
// clang-format off
#define   DebugPrintln(...) FilePrintln(Utils::LogLevel::DEBUG,   Utils::Detail::debugFile,   "DEBUG:   ", __VA_ARGS__)
#define   ErrorPrintln(...) FilePrintln(Utils::LogLevel::ERROR,   Utils::Detail::errorFile,   "ERROR:   ", __VA_ARGS__)
#define    InfoPrintln(...) FilePrintln(Utils::LogLevel::INFO,    Utils::Detail::infoFile,    "INFO:    ", __VA_ARGS__)
#define WarningPrintln(...) FilePrintln(Utils::LogLevel::WARNING, Utils::Detail::warningFile, "WARNING: ", __VA_ARGS__)
// clang-format on
#endif
#define SyncPrintln(...) FilePrintln(Utils::LogLevel::PRINT, stdout, "", __VA_ARGS__)

/**
 * @brief Sets the `Utils::LogLevel` for the program.
 * @param ll The desired `Utils::LogLevel` for the program.
 * @date 2026-06-20
 */
void SetLogLevel(Utils::LogLevel ll);
} // namespace Utils
