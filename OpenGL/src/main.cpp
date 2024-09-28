#include "Application.h"

int main()
{
    Application application;
    application.InitApplication();
    application.UpdateLoop();
    application.ShutDown();
    return 0;
}