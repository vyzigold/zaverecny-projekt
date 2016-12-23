/* 
 * File:   GameWindow.hpp
 * Author: jarda
 *
 * Created on 12. prosinec 2015, 11:13
 */

#ifndef GAMEWINDOW_HPP
#define	GAMEWINDOW_HPP
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include "Image.hpp"

class GameWindow {
public:
    //nazev,width,height,flag
    GameWindow(std::string,int,int,int);
    //nazev,width,height,flag,umisteniX,umisteniY
    GameWindow(std::string,int,int,int,int,int);
    GameWindow();
    bool update(Image*);
    bool konsChyba = false;
    SDL_Renderer* getRenderer(); 
    int getWidth();
    int getHeight();
    void clear();
    GameWindow(const GameWindow& orig);
    virtual ~GameWindow();
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    int w;
    int h;
    bool createRenderer();
};

#endif	/* GAME;WINDOW_HPP */