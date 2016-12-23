/* 
 * File:   Image.cpp
 * Author: jarda
 * 
 * Created on 12. prosinec 2015, 16:59
 */

#include "Image.hpp"

/**
 * @param path path k obrázku
 * @param w šířka obrázku
 * @param h výška obrázku
 * @param x pozice x
 * @param y pozice y
 * @param angle uhel ve stupnich
 */

Image::Image(std::string path,int w, int h,int x, int y, double angle) {
    //nacteni raw pixelu
    rwop=SDL_RWFromFile(path.c_str(), "rb");
    if(rwop == NULL)
    {
        SDL_Log("Nelze nacist obrazek %s\n", IMG_GetError());
        chyba = true;
    }
    //prevedeni raw pixelu na surface
    image=IMG_LoadPNG_RW(rwop);
    if(!image) 
    {
        SDL_Log("Nelze nacist obrazek %s\n", IMG_GetError());
        chyba = true;
    }
    //zinicializovani promennych
    this->w = w;
    this->h = h;
    pos.x = x;
    pos.y = y;
    center.x = ceil((float)w/2);
    center.y = ceil((float)h/2);
    
    this->angle = angle;
}

/**
 * @param path path k obrázku
 * @param area rectangle s informacemi o ploše obrázku
 * @param angle uhel ve stupnich
 */

Image::Image(std::string path,SDL_Rect area, double angle) {
    //nacteni raw pixelu
    rwop=SDL_RWFromFile(path.c_str(), "rb");
    if(rwop == NULL)
    {
        SDL_Log("Nelze nacist obrazek %s\n", IMG_GetError());
        chyba = true;
    }
    //prevedeni raw pixelu na surface
    image=IMG_LoadPNG_RW(rwop);
    if(!image) 
    {
        SDL_Log("Nelze nacist obrazek %s\n", IMG_GetError());
        chyba = true;
    }
    //zinicializovani promennych
    this->w = area.w;
    this->h = area.h;
    pos.x = area.x;
    pos.y = area.y;
    center.x = ceil((float)area.w/2);
    center.y = ceil((float)area.h/2);
    
    this->angle = angle;
}

/**
 * @return Vrátí ukazatel na obrázek jako sruface
 */

SDL_Surface* Image::getSurface()
{
    return image;
}

/**
 * @return Vrátí šířku
 */

int Image::getW()
{
    return w;
}

/**
 * @return Vrátí výšku
 */

int Image::getH()
{
    return h;
}

/**
 * @return vrátí úhel ve stupních
 */

double Image::getAngle()
{
    return angle;
}

/**
 * @return vrátí ukazatel na centerový bod
 */

SDL_Point* Image::getCenter()
{
    return &center;
}

/**
 * @return vrátí ukazatel na levý horní roh (pozice obrázku)
 */

SDL_Point* Image::getPos()
{
    return &pos;
}

/**
 * @param w nastaví šířku
 */

void Image::setW(int w)
{
    this->w = w;
    center.x = ceil((float)w/2);
}

/**
 * @param h nastaví výšku
 */

void Image::setH(int h)
{
    this->h = h;
    center.y = ceil((float)h/2);
}

/**
 * @param angle nastaví úhel ve stupních
 */

void Image::setAngle(double angle)
{
    this->angle = angle;
}

/**
 * @param pos nastaví pozici podle bodu
 */

void Image::setPos(SDL_Point pos)
{
    this->pos = pos;
}

/**
 * @param x nastaví pozici na x-ové ose
 */

void Image::setPosX(int x)
{
    this->pos.x = x;
}

/**
 * @param y nastaví pozici na y-ové ose
 */

void Image::setPosY(int y)
{
    this->pos.y = y;
}

bool Image::setImage(std::string path)
{
    SDL_RWops *rwop_pom=SDL_RWFromFile(path.c_str(), "rb");
    if(rwop_pom == NULL)
    {
        SDL_Log("Nelze nacist obrazek %s\n", IMG_GetError());
        return false;
    }
    //prevedeni raw pixelu na surface
    SDL_Surface *image_pom=IMG_LoadPNG_RW(rwop_pom);
    if(!image_pom) 
    {
        SDL_Log("Nelze nacist obrazek %s\n", IMG_GetError());
        return false;
    }
    
    SDL_FreeSurface(image);
    SDL_FreeRW(rwop);
    
    image = image_pom;
    rwop = rwop_pom;
            
    return true;
}

Image::Image(const Image& orig) {
}

Image::~Image() {
    SDL_FreeSurface(image);
    SDL_FreeRW(rwop);
}

