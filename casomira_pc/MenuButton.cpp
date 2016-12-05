/* 
 * File:   MenuButton.cpp
 * Author: jarda
 * 
 * Created on 13. prosinec 2015, 17:44
 */

#include "MenuButton.hpp"

/**
 * @param renderer SDL_Renderer
 * @param pathObrazek path k obrázku
 * @param w šířka
 * @param h výška
 * @param posX x-ová pozice
 * @param posY y-ová pozice
 */

MenuButton::MenuButton(SDL_Renderer *renderer, std::string pathObrazek, int w, int h, int posX, int posY) {
    this->rend = renderer;
    obrazek = new Image(pathObrazek, w, h, posX, posY, 180);
}

/**
 * Vyrenderuje button button
 * @return true on success, false on failure
 */

bool MenuButton::render(){
    SDL_Rect rect;
    SDL_RendererFlip flip = (SDL_RendererFlip)(SDL_FLIP_VERTICAL|SDL_FLIP_HORIZONTAL);
    
    SDL_Texture *textura;
    rect.x = obrazek->getPos()->x;
    rect.y = obrazek->getPos()->y;
    rect.w = obrazek->getW();
    rect.h = obrazek->getH();
    textura = SDL_CreateTextureFromSurface(rend, obrazek->getSurface());
    if(textura == NULL)
    {
        std::cout<<"Textura se nemohla vystvorit!"<<SDL_GetError();
        return 0;
    }
    
    if(SDL_RenderCopyEx(rend,textura,NULL,&rect,obrazek->getAngle(),obrazek->getCenter(),flip)!=0)
    {
        std::cout << "Nelze vyrendrovat" << SDL_GetError();
        SDL_DestroyTexture(textura);
        return 0;
    }
    SDL_DestroyTexture(textura);
    return true;
}

/**
 * @param bod bod (kliknutí myši..)
 * @return return true if bod is in the button, otherwise it return false
 */

bool MenuButton::isIn(SDL_Point bod)
{
    SDL_Point *pozice = obrazek->getPos();
    int w = obrazek->getW();
    int h = obrazek->getH();
    if(bod.x>pozice->x&&bod.x<pozice->x+w&&bod.y>pozice->y&&bod.y<pozice->y+h)
        return true;
    return false;
}

void MenuButton::setPosX(int x)
{
    this->obrazek->setPosX(x);
}

void MenuButton::setPosY(int y)
{
    this->obrazek->setPosY(y);
}

void MenuButton::setWidth(int w)
{
    this->obrazek->setW(w);
}

void MenuButton::setHeight(int h)
{
    this->obrazek->setH(h);
}

MenuButton::MenuButton(const MenuButton& orig) {
}

MenuButton::~MenuButton() {
    delete obrazek;
}