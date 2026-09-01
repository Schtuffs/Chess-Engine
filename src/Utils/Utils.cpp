#include "Utils/Utils.h"

#include <atomic>
#include <cstdio>
#include <mutex>

#include "Utils/Constants.h"

static std::mutex                   mtxPrint, mtxDebug, mtxError, mtxInfo, mtxWarning;
static std::atomic<Utils::LogLevel> s_logLevel = Utils::LogLevel::ERROR;

bool Utils::Detail::LockPrint(Utils::LogLevel ll)
{
    if (ll < s_logLevel) {
        return false;
    }

#ifdef FILES_ALL_CONSOLE
    mtxPrint.lock();
    return true;
#else
    switch (ll) {
    case Utils::LogLevel::INFO:
        mtxInfo.lock();
        return true;
    case Utils::LogLevel::DEBUG:
        mtxDebug.lock();
        return true;
    case Utils::LogLevel::WARNING:
        mtxWarning.lock();
        return true;
    case Utils::LogLevel::ERROR:
        mtxError.lock();
        return true;
    case Utils::LogLevel::PRINT:
        mtxPrint.lock();
        return true;
    default:
        ErrorPrintln("Utils::LockPrint: Invalid filetype lock: {}", (int)ll);
        return false;
    }
#endif
}

void Utils::Detail::UnlockPrint(Utils::LogLevel ll)
{
#ifdef FILES_ALL_CONSOLE
    (void)ll;
    mtxPrint.unlock();
#else
    switch (ll) {
    case Utils::LogLevel::DEBUG:
        mtxDebug.unlock();
        break;
    case Utils::LogLevel::ERROR:
        mtxError.unlock();
        break;
    case Utils::LogLevel::INFO:
        mtxInfo.unlock();
        break;
    case Utils::LogLevel::PRINT:
        mtxPrint.unlock();
        break;
    case Utils::LogLevel::WARNING:
        mtxWarning.unlock();
        break;
    default:
        ErrorPrintln("Utils::UnlockPrint: Invalid filetype unlock: {}", (int)ll);
        break;
    }
#endif
}

void Utils::SetLogLevel(Utils::LogLevel ll) { s_logLevel = ll; }
