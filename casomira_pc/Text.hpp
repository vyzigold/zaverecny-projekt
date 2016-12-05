/* 
 * File:   Text.hpp
 * Author: jarda
 *
 * Created on 22. prosinec 2015, 0:13
 */

#ifndef TEXT_HPP
#define	TEXT_HPP
#include <string>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

class Text {
public:
    Text(std::string,int);
    Text(std::string,char,int);
    Text(const Text& orig);
    bool chyba;
    SDL_Surface *getSurface();
    std::string getHodnota();
    SDL_Point getTextSize();
    void setHodnota(std::string);
    virtual ~Text();
private:
    std::string hodnota;
    SDL_Surface *surf;
    TTF_Font *font;
    SDL_Point velikost;
};

#endif	/* TEXT_HPP */

