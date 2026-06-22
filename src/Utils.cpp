#include "Utils.h"

#include <cstdio>
#include <mutex>
#include <print>

static std::mutex mtxPrint, mtxDebug, mtxError, mtxInfo, mtxWarning;

void Utils::Detail::LockPrint(Utils::Detail::FileType ft)
{
    switch (ft) {
    case Utils::Detail::FileType::DEBUG:
        mtxDebug.lock();
        break;
    case Utils::Detail::FileType::ERROR:
        mtxError.lock();
        break;
    case Utils::Detail::FileType::INFO:
        mtxInfo.lock();
        break;
    case Utils::Detail::FileType::PRINT:
        mtxPrint.lock();
        break;
    case Utils::Detail::FileType::WARNING:
        mtxWarning.lock();
        break;
    default:
        ErrorPrintln("Invalid filetype lock: {}", (int)ft);
        break;
    }
}
void Utils::Detail::UnlockPrint(Utils::Detail::FileType ft)
{
    switch (ft) {
    case Utils::Detail::FileType::DEBUG:
        mtxDebug.unlock();
        break;
    case Utils::Detail::FileType::ERROR:
        mtxError.unlock();
        break;
    case Utils::Detail::FileType::INFO:
        mtxInfo.unlock();
        break;
    case Utils::Detail::FileType::PRINT:
        mtxPrint.unlock();
        break;
    case Utils::Detail::FileType::WARNING:
        mtxWarning.unlock();
        break;
    default:
        ErrorPrintln("Invalid filetype unlock: {}", (int)ft);
        break;
    }
}

