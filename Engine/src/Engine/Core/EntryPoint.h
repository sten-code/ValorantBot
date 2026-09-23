#pragma once

#include <Engine/Core/Application.h>
#include <Engine/Core/Log.h>

extern Application* CreateApplication();

int main(int argc, char** argv)
{
    Logger::Init();

    Application* app = CreateApplication();
    app->Run();
    delete app;
}