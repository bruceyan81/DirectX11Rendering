#include "Application/Application.h"

#include <Windows.h>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    ApplicationSpace::Application application{};

    return application.run();
}
