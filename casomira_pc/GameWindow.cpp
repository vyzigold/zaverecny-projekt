/* 
 * File:   GameWindow.cpp
 * Author: jarda
 * 
 * Created on 12. prosinec 2015, 11:13
 */

#include "GameWindow.hpp"

/**
 * @param nazev Název okna
 * @param width Šířka okna
 * @param height Výška okna
 * @param flag flag
 * @param x pozice x
 * @param y pozice y
 * <b>Flags:</b>
 * SDL_WINDOW_FULLSCREEN fullscreen window
 * SDL_WINDOW_FULLSCREEN_DESKTOP fullscreen window at the current desktop resolution
 * SDL_WINDOW_OPENGL window usable with OpenGL context
 * SDL_WINDOW_SHOWN window is visible
 * SDL_WINDOW_HIDDEN window is not visible
 * SDL_WINDOW_BORDERLESS no window decoration
 * SDL_WINDOW_RESIZABLE window can be resized
 * SDL_WINDOW_MINIMIZED window is minimized
 * SDL_WINDOW_MAXIMIZED window is maximized
 * SDL_WINDOW_INPUT_GRABBED window has grabbed input focus
 * SDL_WINDOW_INPUT_FOCUS window has input focus
 * SDL_WINDOW_MOUSE_FOCUS window has mouse focus
 * SDL_WINDOW_FOREIGN window not created by SDL
 * SDL_WINDOW_ALLOW_HIGHDPI window should be created in high-DPI mode if supported (>= SDL 2.0.1)
 * SDL_WINDOW_MOUSE_CAPTURE window has mouse captured (unrelated to INPUT_GRABBED, >= SDL 2.0.4)
 */
GameWindow::GameWindow(std::string nazev, int width, int height, int flag, int x, int y) {
    //inicializace SDL
    if(!SDL_WasInit(SDL_INIT_VIDEO))
    {
        if(SDL_Init(SDL_INIT_VIDEO))
        {
            std::cout << "\nUnable to initialize SDL:  %s\n" << SDL_GetError();
            konsChyba = true;
        }
    }
    //vytvoreni okna
    window = SDL_CreateWindow(
        nazev.c_str(),                  // window title
        x,           // initial x position
        y,           // initial y position
        width,                               // width, in pixels
        height,                               // height, in pixels
        flag                  // flags - see below
    );
    if(window == NULL)
    {
        std::cout<<"Okno nemohlo byt vytvoreno! SDL_ERROR: %s\n"<< SDL_GetError();
        konsChyba = true;
    }
    //vytvoreni rendereru
    if(!konsChyba&&!createRenderer())
    {
        konsChyba = true;
    }
    SDL_GetWindowSize(window,&w,&h);
}


/**
 * @param nazev Název okna
 * @param width Šířka okna
 * @param height Výška okna
 * @param flag flag
 * <b>Flags:</b>
 * SDL_WINDOW_FULLSCREEN fullscreen window
 * SDL_WINDOW_FULLSCREEN_DESKTOP fullscreen window at the current desktop resolution
 * SDL_WINDOW_OPENGL window usable with OpenGL context
 * SDL_WINDOW_SHOWN window is visible
 * SDL_WINDOW_HIDDEN window is not visible
 * SDL_WINDOW_BORDERLESS no window decoration
 * SDL_WINDOW_RESIZABLE window can be resized
 * SDL_WINDOW_MINIMIZED window is minimized
 * SDL_WINDOW_MAXIMIZED window is maximized
 * SDL_WINDOW_INPUT_GRABBED window has grabbed input focus
 * SDL_WINDOW_INPUT_FOCUS window has input focus
 * SDL_WINDOW_MOUSE_FOCUS window has mouse focus
 * SDL_WINDOW_FOREIGN window not created by SDL
 * SDL_WINDOW_ALLOW_HIGHDPI window should be created in high-DPI mode if supported (>= SDL 2.0.1)
 * SDL_WINDOW_MOUSE_CAPTURE window has mouse captured (unrelated to INPUT_GRABBED, >= SDL 2.0.4)
 */

GameWindow::GameWindow(std::string nazev, int width, int height, int flag) {
    //inicializace SDL
    if(!SDL_WasInit(SDL_INIT_VIDEO))
    {
        if(SDL_Init(SDL_INIT_VIDEO))
        {
            std::cout << "\nUnable to initialize SDL:  %s\n" << SDL_GetError();
            konsChyba = true;
        }
    }
    //vytvoreni okna
    window = SDL_CreateWindow(
        nazev.c_str(),                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        width,                               // width, in pixels
        height,                               // height, in pixels
        flag                  // flags - see below
    );
    if(window == NULL)
    {
        std::cout<<"Okno nemohlo byt vytvoreno! SDL_ERROR: %s\n"<< SDL_GetError();
        konsChyba = true;
    }
    //vytvoreni rendereru
    if(!konsChyba&&!createRenderer())
    {
        konsChyba = true;
    }
    SDL_GetWindowSize(window,&w,&h);
}

GameWindow::GameWindow() {
    //inicializace SDL
    if(!SDL_WasInit(SDL_INIT_VIDEO))
    {
        if(SDL_Init(SDL_INIT_VIDEO))
        {
            std::cout << "\nUnable to initialize SDL:  %s\n" << SDL_GetError();
            konsChyba = true;
        }
    }
    //vytvoreni okna
    window = SDL_CreateWindow(
        "Bez nazvu",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        500,                               // width, in pixels
        500,                               // height, in pixels
        0                  // flags - see below
    );
    if(window == NULL)
    {
        std::cout<<"Okno nemohlo byt vytvoreno! SDL_ERROR: %s\n"<< SDL_GetError();
        konsChyba = true;
    }
    //vytvoreni rendereru
    if(!konsChyba&&!createRenderer())
    {
        konsChyba = true;
    }
    SDL_GetWindowSize(window,&w,&h);
}

/**
 * @return returns true on succes and false on failure
 */

bool GameWindow::createRenderer()
{
    //vytvoreni rendereru
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        std::cout<<"Renderer nemohl byt vytvoren! SDL_ERROR: %s\n"<< SDL_GetError();
        return 0;
    }
    //prebarveni obrazovky
    SDL_SetRenderDrawColor(renderer, 68, 65, 65, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    return 1;
}

/**
 * @param obrazek obrazek pro zobrazení
 * Zobrazí předaný obrázek na jeho souřadnicích
 * @return returns true on succes and false on failure
 */

bool GameWindow::update(Image *obrazek)
{
    //vytvoreni textury
    SDL_Texture *textura;
    textura = SDL_CreateTextureFromSurface(renderer, obrazek->getSurface());
    if(textura == NULL)
    {
        std::cout<<"Textura se nemohla vystvorit!"<<SDL_GetError();
        return 0;
    }
    //zjisteni pozice a velikosti
    SDL_Rect rect;
    rect.x = obrazek->getPos()->x;
    rect.y = obrazek->getPos()->y;
    rect.w = obrazek->getW();
    rect.h = obrazek->getH();
    SDL_RendererFlip flip = (SDL_RendererFlip)(SDL_FLIP_VERTICAL|SDL_FLIP_HORIZONTAL);
    //vykresleni
    int i = SDL_RenderCopyEx(renderer,textura,NULL,&rect,obrazek->getAngle(),obrazek->getCenter(),flip);
    if(i!=0)
    {
        std::cout << "Nelze vyrendrovat" << SDL_GetError();
        SDL_DestroyTexture(textura);
    }
    else
    {
        SDL_RenderPresent(renderer);
    }
    std::cout << obrazek->getPos()->x << std::endl << obrazek->getPos()->y << std::endl << obrazek->getW() << std::endl << obrazek->getH();
    SDL_DestroyTexture(textura);
    return true;
}

/**
 * @return returns pointer to a window renderer
 */

SDL_Renderer* GameWindow::getRenderer()
{
    return renderer;
}

/**
 * @return Vrací výšku okna
 */

int GameWindow::getHeight()
{
    SDL_GetWindowSize(window,&w,&h);
    return h;
}

/**
 * @return Vrací šířku okna
 */

int GameWindow::getWidth()
{
    SDL_GetWindowSize(window,&w,&h);
    return w;
}

void GameWindow::clear()
{
    SDL_RenderClear(this->renderer);
}

GameWindow::GameWindow(const GameWindow& orig) {
}

GameWindow::~GameWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

