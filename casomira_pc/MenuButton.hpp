/* 
 * File:   MenuButton.hpp
 * Author: jarda
 *
 * Created on 13. prosinec 2015, 17:44
 */

#ifndef MENUBUTTON_HPP
#define	MENUBUTTON_HPP
#include "Image.hpp"
#include "Text.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <iostream>

class MenuButton {
public:
    //path obrazek, width, height,posX,posY
    MenuButton(SDL_Renderer*,std::string,int,int,int,int);
    MenuButton(SDL_Renderer*,std::string,int,int,int,int, Text*);
    void setPosX(int);
    void setPosY(int);
    void setWidth(int);
    void setHeight(int);
    bool render();
    bool isIn(SDL_Point);
    MenuButton(const MenuButton& orig);
    virtual ~MenuButton();
private:
    Text *text;
    Image* obrazek;
    SDL_Renderer *rend;
    int poradi;
};

#endif	/* MENUBUTTON_HPP */