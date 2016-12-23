#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <sstream>
#include <ctime>


#include "GameWindow.hpp"
#include "MenuButton.hpp"
#include "Text.hpp"

std::string Convert (float number){
    std::ostringstream buff;
    buff<<number;
    return buff.str();   
}

//vrací nenulovou hodnotu při stisku klávesy

int posliPacket(char *data, UDPpacket *packet, UDPsocket *socket)
{
    packet->len = strlen(data) + 1;
    memcpy(packet->data, data, packet->len);
    return SDLNet_UDP_Send(*socket, -1, packet);
}

void printHelp()
{
    SDL_Log("\n\n\nM           muzi\n");
    SDL_Log("Z           zeny\n");
    SDL_Log("H           help\n");
    SDL_Log("T           zobraz stav tercu\n");
    SDL_Log("V           zapnutí/vypnutí výpisu průběžného času\n");
    SDL_Log("Q           vypnutí programu\n");
    SDL_Log("N           vypis momentalniho nastaveni\n");
    SDL_Log("S           povol/zakaz vystrel\n");
    return;
}

void printNastaveni(bool pohlavi, int spozdeni, bool vypis)
{
    if(pohlavi)
        SDL_Log("\n\n\nbezi muzi\n");
    else
        SDL_Log("bezi zeny\n");
    if(vypis)
        SDL_Log("prubezny vypis casu je povolen\n");
    else
        SDL_Log("prubezny vypis casu je zakazan\n");
    return;
}

std::string getDate()
{
    std::string date;
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    date = Convert(now->tm_mday);
    date += ".";
    date += Convert(now->tm_mon + 1);
    date += ".";
    date += Convert(now->tm_year + 1900);
    return date;
}

char * stringToChar(std::string neco)
{
    
    char *vysledek;
    vysledek = new char[neco.length()+1];
    int i = 0;
    for(; neco.length() > i; i++)
    {
        vysledek[i] = neco[i];
    }
    vysledek[i] = '\0';
    return vysledek;
}

std::string upravCas(float cas)
{
    std::string cislo = Convert(cas);
    int poziceTecky = cislo.find('.');
    if(poziceTecky > 2)
        cislo = "9999";
    else
    {
        cislo.erase(poziceTecky,1);
        cislo.erase(4,cislo.length()-4);
    }
    return cislo;
}

void buttonPress(bool quit)
{
    quit = true;
}

int main(int argc, char **argv)
{	
    /* initialize SDL */
    if(SDL_Init(0)==-1)
    {
            SDL_Log("SDL_Init: %s\n",SDL_GetError());
            exit(1);
    }

    /* initialize SDL_net */
    if(SDLNet_Init()==-1)
    {
            SDL_Log("SDLNet_Init: %s\n",SDLNet_GetError());
            exit(2);
    }
    
    Mix_SetSoundFonts("zvuky/organ.sf2");
    
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        SDL_Log( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    
    Mix_Chunk *prvni = NULL;
    Mix_Chunk *druhy = NULL;
    Mix_Chunk *treti = NULL;
    Mix_Chunk *ctvrty = NULL;
    
    //nacteni zvuku
    prvni = Mix_LoadWAV( "zvuky/prvni.wav" );
    if( prvni == NULL )
    {
        SDL_Log( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    druhy = Mix_LoadWAV( "zvuky/druhy.wav" );
    if( druhy == NULL )
    {
        SDL_Log( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    treti = Mix_LoadWAV( "zvuky/treti.wav" );
    if( treti == NULL )
    {
        SDL_Log( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    ctvrty = Mix_LoadWAV( "zvuky/ctvrty.wav" );
    if( ctvrty == NULL )
    {
        SDL_Log( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    int zvuk;
    
    srand (time(NULL));
    
    IPaddress* espAddress;
    UDPsocket socketOut;
    socketOut = SDLNet_UDP_Open(0);
    if(!socketOut)
    {
        SDL_Log("Could not allocate receiving socket\n");
        exit(4);
    }
    
    espAddress = SDLNet_UDP_GetPeerAddress(socketOut, -1);
    // resolve server host
    SDLNet_ResolveHost(espAddress, "192.168.4.1", 1234);
    UDPpacket *packetOut;
    if(!(packetOut = SDLNet_AllocPacket(8)))
    {
        SDL_Log("Could not allocate packet\n");
        SDLNet_Quit();
        SDL_Quit();
        exit(2);
    }

    packetOut->address.host = espAddress->host;
    packetOut->address.port = espAddress->port;

    
    // create a UDPsocket on port 6666 (server)
    UDPsocket socketIn;

    socketIn=SDLNet_UDP_Open(6666);
    if(!socketIn) {
        SDL_Log("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }

    UDPpacket *packetIn  = SDLNet_AllocPacket(2);

        
    bool quit = false;
    bool vypis = true;
    bool pohlavi = true;
    bool started = false;
    bool vystrel = true;
    std::string cislo;
    int ted;
    float pravy = 0;
    float levy = 0;
    unsigned int start = 0;
    char znak;
    char prijato;
    int spozdeni = 5;
    int poziceTecky = 0;
    if(!posliPacket("s",packetOut,&socketOut))
        SDL_Log("%s",SDLNet_GetError());
    
    GameWindow *okno = new GameWindow(std::string("Casomira"),500,500,
            SDL_WINDOW_RESIZABLE|SDL_WINDOW_INPUT_FOCUS|SDL_WINDOW_MOUSE_FOCUS|SDL_WINDOW_SHOWN,
            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED);
    if(okno->konsChyba)
    {
        std::cout << "Chyba pri vytvareni okna";
    }
    Text *levyText = new Text("0",400);
    Text *pravyText = new Text("0",400);
    Text *muziZenyText = new Text("Muzi",400);
    Text *startText = new Text("START",400);
    MenuButton levyButton = MenuButton(okno->getRenderer(),
            std::string("img/terc.png"),                                  //obrázek pozadí
            (okno->getWidth() - 3*((int)(((float)okno->getWidth()/100)*2)))/2,                                           //šířka
            okno->getHeight()/3 - (((int)(((float)okno->getHeight()/100)*2))+((int)(((float)okno->getHeight()/100)*1))),                                            //výška
            (int)(((float)okno->getWidth()/100)*2),                                                              //pozice x
            (int)(((float)okno->getHeight()/100)*2),                                                              //pozice y
            levyText);                                                      //ukazatel na text
    levyButton.render();
    MenuButton pravyButton = MenuButton(okno->getRenderer(),
            std::string("img/terc.png"),                                  //obrázek pozadí
            (okno->getWidth() - 3*((int)(((float)okno->getWidth()/100)*2)))/2,                                           //šířka
            okno->getHeight()/3 - (((int)(((float)okno->getHeight()/100)*2))+((int)(((float)okno->getHeight()/100)*1))),                                            //výška
            (okno->getWidth()/2) + (int)(((float)okno->getWidth()/100)*1),                                             //pozice x
            (int)(((float)okno->getHeight()/100)*2),                                                              //pozice y
            pravyText);                                                     //ukazatel na text
    pravyButton.render();
    MenuButton muziZenyButton = MenuButton(okno->getRenderer(),
            std::string("img/muzi.png"),                                  //obrázek pozadí
            okno->getWidth() - 2*((int)(((float)okno->getWidth()/100)*2)),                                               //šířka
            (okno->getHeight() - 4*((int)(((float)okno->getHeight()/100)*2)))/10  - (int)(((float)okno->getHeight()/100)*1),                                           //výška
            (int)(((float)okno->getWidth()/100)*2),                                                              //pozice x
            okno->getHeight()/3 + ((int)(((float)okno->getHeight()/100)*1)),                                            //pozice y
            muziZenyText);                                                  //ukazatel na text
    muziZenyButton.render();
    MenuButton startButton = MenuButton(okno->getRenderer(),
            std::string("img/start.png"),                                  //obrázek pozadí
            okno->getWidth() - 2*((int)(((float)okno->getWidth()/100)*2)),                                               //šířka
            (okno->getHeight() - ((int)(((float)okno->getHeight()/100)*3)))-(okno->getHeight()/3+okno->getHeight()/10),   //výška
            (int)(((float)okno->getWidth()/100)*2),                                                              //pozice x
            okno->getHeight()/3+okno->getHeight()/10 + (int)(((float)okno->getHeight()/100)*1),                       //pozice y
            startText);                                                     //ukazatel na text
    startButton.render();
    SDL_Point poziceKliknuti;
    SDL_Event e;
    
    
    while(!quit)
    {
        
        if(SDLNet_UDP_Recv(socketIn, packetIn)) {
            prijato = packetIn->data[0];
            switch(prijato)
            {
                case 'm':
                    pohlavi = true;
                    muziZenyButton.setImage(std::string("img/muzi.png"));
                    muziZenyText->setHodnota("Muzi");
                    muziZenyButton.render();
                    break;
                case 'z':
                    pohlavi = false;
                    muziZenyButton.setImage(std::string("img/zeny.png"));
                    muziZenyText->setHodnota("Zeny");
                    muziZenyButton.render();
                    break;
                case 'p':
                    if(pravy == 0)
                    {
                        pravy = ((float)(SDL_GetTicks() - start))/(float)1000;
                        pravyText->setHodnota(Convert(pravy));
                        pravyButton.render();
                    }
                    break;
                case 'l':
                    if(levy == 0)
                    {
                        levy = ((float)(SDL_GetTicks() - start))/(float)1000;
                        levyText->setHodnota(Convert(levy));
                        levyButton.render();
                    }
                    break;
                case '1':
                    SDL_Log("terce jsou v poradku\n");
                    levyButton.setImage(std::string("img/start.png"));
                    pravyButton.setImage(std::string("img/start.png"));
                    levyButton.render();
                    pravyButton.render();
                    break;
                case '0':
                    SDL_Log("CHYBA! je potreba zvednout terce\n");
                    levyButton.setImage(std::string("img/stop.png"));
                    pravyButton.setImage(std::string("img/stop.png"));
                    levyButton.render();
                    pravyButton.render();
                    break;
            }
        }
        if(SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT)
                quit=true;
            else if (e.type==SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_SPACE:
                        start = SDL_GetTicks();
                        if(started)
                        {
                            started = false;
                            startButton.setImage(std::string("img/start.png"));
                            startText->setHodnota(std::string("START"));
                            startButton.render();
                        }
                        else
                        {
                            started = true;
                            startButton.setImage(std::string("img/stop.png"));
                            startText->setHodnota(std::string("STOP"));
                            startButton.render();
                            if(vystrel)
                            {
                                zvuk = rand() % 4;
                                switch(zvuk)
                                {
                                    case 0:
                                        Mix_PlayChannel( -1, prvni, 0 );
                                        break;
                                    case 1:
                                        Mix_PlayChannel( -1, druhy, 0 );
                                        break;
                                    case 2:
                                        Mix_PlayChannel( -1, treti, 0 );
                                        break;
                                    case 3:
                                        Mix_PlayChannel( -1, ctvrty, 0 );
                                        break;
                                }
                            }
                            pravy = 0;
                            levy = 0;
                        }
                        zvuk = rand() % 4;
                        break;
                    case SDLK_m:
                        if(!posliPacket("m",packetOut,&socketOut))
                            SDL_Log("%s",SDLNet_GetError());
                        pohlavi = true;
                        muziZenyButton.setImage(std::string("img/muzi.png"));
                        muziZenyText->setHodnota("Muzi");
                        muziZenyButton.render();
                        break;
                    case SDLK_z:
                        if(!posliPacket("z",packetOut,&socketOut))
                            SDL_Log("%s",SDLNet_GetError());
                        pohlavi = false;
                        muziZenyButton.setImage(std::string("img/zeny.png"));
                        muziZenyText->setHodnota("Zeny");
                        muziZenyButton.render();
                        break;
                    case SDLK_t:
                        if(!posliPacket("t",packetOut,&socketOut))
                            SDL_Log("%s",SDLNet_GetError());
                        break;
                    case SDLK_s:
                        if(vystrel)
                            vystrel = false;
                        else
                            vystrel = true;
                        break;
                    case 'q':
                        quit = true;
                        break;
                }
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                poziceKliknuti.x = e.button.x;
                poziceKliknuti.y = e.button.y;
                if(levyButton.isIn(poziceKliknuti) || pravyButton.isIn(poziceKliknuti))
                {
                    if(!posliPacket("t",packetOut,&socketOut))
                        SDL_Log("%s",SDLNet_GetError());
                }
                if(muziZenyButton.isIn(poziceKliknuti))
                {
                    if(pohlavi)
                    {
                        if(!posliPacket("z",packetOut,&socketOut))
                            SDL_Log("%s",SDLNet_GetError());
                        pohlavi = false;
                        muziZenyButton.setImage(std::string("img/zeny.png"));
                        muziZenyText->setHodnota("Zeny");
                        muziZenyButton.render();
                    }
                    else
                    {
                        if(!posliPacket("m",packetOut,&socketOut))
                            SDL_Log("%s",SDLNet_GetError());
                        pohlavi = true;
                        muziZenyButton.setImage(std::string("img/muzi.png"));
                        muziZenyText->setHodnota("Muzi");
                        muziZenyButton.render();
                    }
                            
                }
                if(startButton.isIn(poziceKliknuti))
                {
                    start = SDL_GetTicks();
                        if(started)
                        {
                            started = false;
                            startButton.setImage(std::string("img/start.png"));
                            startText->setHodnota(std::string("START"));
                            startButton.render();
                        }
                        else
                        {
                            started = true;
                            startButton.setImage(std::string("img/stop.png"));
                            startText->setHodnota(std::string("STOP"));
                            startButton.render();
                            if(vystrel)
                            {
                                zvuk = rand() % 4;
                                switch(zvuk)
                                {
                                    case 0:
                                        Mix_PlayChannel( -1, prvni, 0 );
                                        break;
                                    case 1:
                                        Mix_PlayChannel( -1, druhy, 0 );
                                        break;
                                    case 2:
                                        Mix_PlayChannel( -1, treti, 0 );
                                        break;
                                    case 3:
                                        Mix_PlayChannel( -1, ctvrty, 0 );
                                        break;
                                }
                            }
                            pravy = 0;
                            levy = 0;
                        }
                        zvuk = rand() % 4;
                }
            }
            else if (e.type == SDL_WINDOWEVENT) 
            {
                switch (e.window.event) 
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        okno->clear();
                        levyButton.setWidth((okno->getWidth() - 3*((int)(((float)okno->getWidth()/100)*2)))/2);
                        levyButton.setHeight(okno->getHeight()/3 - (((int)(((float)okno->getHeight()/100)*2))+((int)(((float)okno->getHeight()/100)*1))));
                        levyButton.setPosX((int)(((float)okno->getWidth()/100)*2));
                        levyButton.render();
                        pravyButton.setWidth((okno->getWidth() - 3*((int)(((float)okno->getWidth()/100)*2)))/2);
                        pravyButton.setHeight(okno->getHeight()/3 - (((int)(((float)okno->getHeight()/100)*2))+((int)(((float)okno->getHeight()/100)*1))));
                        pravyButton.setPosX((okno->getWidth()/2) + (int)(((float)okno->getWidth()/100)*1));
                        pravyButton.render();
                        muziZenyButton.setWidth(okno->getWidth() - 2*((int)(((float)okno->getWidth()/100)*2)));
                        muziZenyButton.setHeight((okno->getHeight() - 4*((int)(((float)okno->getHeight()/100)*2)))/10  - (int)(((float)okno->getHeight()/100)*1));
                        muziZenyButton.setPosX((int)(((float)okno->getWidth()/100)*2));
                        muziZenyButton.setPosY(okno->getHeight()/3 + ((int)(((float)okno->getHeight()/100)*1)));
                        muziZenyButton.render();
                        startButton.setWidth(okno->getWidth() - 2*((int)(((float)okno->getWidth()/100)*2)));
                        startButton.setHeight((okno->getHeight() - ((int)(((float)okno->getHeight()/100)*3)))-(okno->getHeight()/3+okno->getHeight()/10));
                        startButton.setPosX((int)(((float)okno->getWidth()/100)*2));
                        startButton.setPosY(okno->getHeight()/3+okno->getHeight()/10 + (int)(((float)okno->getHeight()/100)*1));
                        startButton.render();
                        break;
                }
            }
        }
        
        if(false)
        {
            znak = getchar();
            SDL_Log("\n");
            switch(znak)
            {
                case 'm':
                    if(!posliPacket("m",packetOut,&socketOut))
                        SDL_Log("%s",SDLNet_GetError());
                    pohlavi = true;
                    break;
                case 'z':
                    if(posliPacket("z",packetOut,&socketOut))
                        SDL_Log("%s",SDLNet_GetError());
                    pohlavi = false;
                    break;
                case 'h':
                    printHelp();
                    break;
                case 's':
                    if(vystrel)
                        vystrel = false;
                    else
                        vystrel = true;
                    break;
                case 't':
                    if(!posliPacket("t",packetOut,&socketOut))
                        SDL_Log("%s",SDLNet_GetError());
                    break;
                case 'n':
                    printNastaveni(pohlavi,spozdeni,vypis);
                    break;
                case 'v':
                    if(vypis)
                        vypis = false;
                    else
                        vypis = true;
                    break;
                case 'q':
                    quit = true;
                    break;
                case ' ':
                    //posliPacket("t",packetOut,&socketOut);
                    start = SDL_GetTicks();
                    if(started)
                        started = false;
                    else
                    {
                        started = true;
                        if(vystrel)
                        {
                            zvuk = rand() % 4;
                            switch(zvuk)
                            {
                                case 0:
                                    Mix_PlayChannel( -1, prvni, 0 );
                                    break;
                                case 1:
                                    Mix_PlayChannel( -1, druhy, 0 );
                                    break;
                                case 2:
                                    Mix_PlayChannel( -1, treti, 0 );
                                    break;
                                case 3:
                                    Mix_PlayChannel( -1, ctvrty, 0 );
                                    break;
                            }
                        }
                        pravy = 0;
                        levy = 0;
                    }
                    zvuk = rand() % 4;
                    break;
            }
        }
        if(levy != 0 && pravy != 0&& started)
        {
            SDL_Log("%f",levy);
            SDL_Log("                      %f\n",pravy);
            levyText->setHodnota(Convert(levy));
            pravyText->setHodnota(Convert(pravy));
            levyButton.render();
            pravyButton.render();
            cislo = upravCas(levy);
            cislo += upravCas(pravy);
            cislo += getDate();
            if(posliPacket(&cislo[0],packetOut,&socketOut))
                SDL_Log("%s",SDLNet_GetError());
            started = false;
        }
        if(vypis && started)
        {
            ted = SDL_GetTicks();
            if(levy == 0)
            {
                SDL_Log("%f",((float)(ted - start))/(float)1000);
                levyText->setHodnota(Convert(((float)(ted - start))/(float)1000));
            }
            else
            {
                SDL_Log("%f",levy);
                levyText->setHodnota(Convert(levy));
            }
            if(pravy == 0)
            {
                SDL_Log("                      %f\n",((float)(ted - start))/(float)1000);
                pravyText->setHodnota(Convert(((float)(ted - start))/(float)1000));
            }
            else
            {
                SDL_Log("                      %f\n",pravy);
                pravyText->setHodnota(Convert(pravy));
            }
            levyButton.render();
            pravyButton.render();
        }
        levyButton.render();
        pravyButton.render();
        startButton.render();
        muziZenyButton.render();
        SDL_Delay(5);
        SDL_RenderPresent(okno->getRenderer());
    }
    
    //uvolneni pameti
    Mix_FreeChunk(prvni);
    Mix_FreeChunk(druhy);
    Mix_FreeChunk(treti);
    Mix_FreeChunk(ctvrty);
    
    delete okno;
    delete muziZenyText;
    delete startText;
    delete levyText;
    delete pravyText;

    Mix_Quit();
    
    /* shutdown SDL_net */
    SDLNet_Quit();

    /* shutdown SDL */
    SDL_Quit();

    return(0);
}
