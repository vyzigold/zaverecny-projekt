/* 
 * File:   Text.cpp
 * Author: jarda
 * 
 * Created on 22. prosinec 2015, 0:13
 */

#include "Text.hpp"

/**
 * Konstruktor textu
 * @param hodnota String, který se má zobrazit
 */

Text::Text(std::string hodnota,int height) {
    chyba = false;
    surf = 0;
    if(!TTF_WasInit())
        if(TTF_Init()==-1)
            chyba = true;
    if(!chyba)
    {
        if((font = TTF_OpenFont("font/AmaticSC-Regular.ttf",height)) == NULL)
            chyba = true;
        else
        {
            this->hodnota = hodnota;
        }
    }
}

/**
 * Konstruktor textu
 * @param hodnota String, který se má zobrazit
 * @param fontStyle styl textu (b||i||u)
 */

Text::Text(std::string hodnota,char fontStyle,int height) {
    chyba = false;
    surf = 0;
    if(!TTF_WasInit())
        if(TTF_Init()==-1)
            chyba = true;
    if(!chyba)
    {
        if((font = TTF_OpenFont("font/AmaticSC-Regular.ttf",height)) == NULL)
            chyba = true;
        else
        {
            this->hodnota = hodnota;
            if(fontStyle == 'b'||fontStyle == 'B')
                TTF_SetFontStyle(font, TTF_STYLE_BOLD);
            if(fontStyle == 'i'||fontStyle == 'I')
                TTF_SetFontStyle(font, TTF_STYLE_ITALIC);
            if(fontStyle == 'u'||fontStyle == 'U')
                TTF_SetFontStyle(font, TTF_STYLE_UNDERLINE);
        }
    }
}

/**
 * Vrací obrázek textu jako surface
 * @return SDL_Surface, který obsahuje text
 */

SDL_Surface * Text::getSurface()
{
    SDL_Color color={0,0,0};
    surf = TTF_RenderText_Solid(font, hodnota.c_str(),color);
    return surf;
}

/**
 * @return Vrací právě nastavený text jako string
 */

std::string Text::getHodnota()
{
    return hodnota;
}

/**
 * Nastaví text na předanou hodnotu
 * @param hodnota String, na který se má text nastavit
 */

void Text::setHodnota(std::string hodnota)
{
    this->hodnota = hodnota;
}

/**
 * @return Vrací velikost textu jako SDL_Point, (x - šířka, y - výška)
 * 
 * Doporučuji použít vždy při renderování textu
 */

SDL_Point Text:: getTextSize()
{
    if(TTF_SizeText(font,hodnota.c_str(),&velikost.x,&velikost.y)) 
    {
        std::cout << "Nelze nastavit velikost textu: " << TTF_GetError(); 
    } 
    else 
    {
        return velikost;
    }
}


SDL_Point* Text::getCenter()
{
    SDL_Point *center;
    
    center->x = ceil((float)this->getTextSize().x/2);
    center->y = ceil((float)this->getTextSize().y/2);
    return center;
}

Text::Text(const Text& orig) {
}

Text::~Text() {
    SDL_FreeSurface(surf);
    TTF_CloseFont(font);
}

