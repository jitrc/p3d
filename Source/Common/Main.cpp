#include "Includes.h"

#include "Common/Config.h"
#include "Common/Logger.h"

using namespace P3D;

extern int P2DMain();

int SDL_main(int argc, char** argv)
{
    // initialize system-wide services
    AtExitManager atExitManager;
    Config::GetInstance();
    LoggingSystem::GetInstance();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        Logger log(L"System.SDL");
        log.error() << L"Couldn't initialize SDL: " << ToUTF16(SDL_GetError());
        return -1;
    }

    int res = -1;
    try
    {
        res = P2DMain();
    }
    catch (Exception& exc)
    {
        Logger logger(L"System.Thread.MainThread");
        logger.error() << L"Unhandled exception in the main thread: " << exc;
    }
    catch (std::exception& exc)
    {
        Logger logger(L"System.Thread.MainThread");
        logger.error() << L"Unhandled exception in the main thread: " << exc.what();
    }
    catch (...)
    {
        Logger logger(L"System.Thread.MainThread");
        logger.error() << L"Unknown unhandled exception in the main thread.";
    }

    SDL_Quit();

    return res;
}