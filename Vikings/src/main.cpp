#include "Game.h"
#include "MemLeaks.h"
#include "Globals.h"

Game* g_game = nullptr;  // ✅ DEFINICIÓN global que otros archivos usan

int main()
{
    ReportMemoryLeaks();

    AppStatus status;
    int main_return = EXIT_SUCCESS;

    LOG("Application start");

    g_game = new Game();  // ✅ Usamos la variable global directamente

    status = g_game->Initialise(GAME_SCALE_FACTOR);
    if (status != AppStatus::OK)
    {
        LOG("Failed to initialise game");
        main_return = EXIT_FAILURE;
    }

    while (status == AppStatus::OK)
    {
        status = g_game->Update();
        if (status != AppStatus::OK)
        {
            if (status == AppStatus::ERROR)      main_return = EXIT_FAILURE;
            else if (status == AppStatus::QUIT)  main_return = EXIT_SUCCESS;
            break;
        }
        g_game->Render();
    }

    LOG("Application finish");

    g_game->Cleanup();
    delete g_game;
    g_game = nullptr;  // 👍 buena práctica

    LOG("Bye :)");
    return main_return;
}
