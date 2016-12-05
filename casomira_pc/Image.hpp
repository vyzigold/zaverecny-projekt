/* 
 * File:   Image.hpp
 * Author: jarda
 *
 * Created on 12. prosinec 2015, 16:59
 */

#ifndef IMAGE_HPP
#define	IMAGE_HPP
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <iostream>

class Image {
public:
    //nazev,width,height,posX,posY,angle
    Image(std::string,int,int,int,int,double);
    //nazev,pozice a rozměry,angle
    Image(std::string,SDL_Rect,double);
    Image(const Image& orig);
    virtual ~Image();
    bool chyba = false;
    
    SDL_Surface* getSurface();
    SDL_Point* getCenter();
    int getW();
    int getH();
    double getAngle();
    SDL_Point* getPos();
    void setW(int);
    void setH(int);
    void setAngle(double);
    void setPos(SDL_Point);
    void setPosX(int);
    void setPosY(int);
    
private:
    SDL_Texture *texture;
    int w, h;
    SDL_Point pos;
    SDL_Surface *image;
    SDL_RWops *rwop;
    SDL_Point center;
    double angle;
};

#endif	/* IMAGE_HPP */