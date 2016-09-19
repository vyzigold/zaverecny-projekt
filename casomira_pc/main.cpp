#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stropts.h>
#include <string>
#include <sstream>
#include <time.h>


std::string Convert (float number){
    std::ostringstream buff;
    buff<<number;
    return buff.str();   
}


int kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

/**
 * aaa
 * @param data aaa
 * @param packet aaa
 * @param socket aaa
 * @return  aaa
 */
int posliPacket(char *data, UDPpacket *packet, UDPsocket *socket)
{
    packet->len = strlen(data) + 1;
    memcpy(packet->data, data, packet->len);
    return SDLNet_UDP_Send(*socket, -1, packet);
}

void printHelp()
{
    printf("\n\n\nM           muzi\n");
    printf("Z           zeny\n");
    printf("H           help\n");
    printf("D           pro napovedu o nastaveni spozdeni\n");
    printf("T           zobraz stav tercu\n");
    printf("V           zapnutí/vypnutí výpisu průběžného času\n");
    printf("Q           vypnutí programu\n");
    printf("N           vypis momentalniho nastaveni\n");
    printf("S           povol/zakaz vystrel\n");
    printf("cislo       pro nastaveni spozdeni\n");
    return;
}

void printNastaveni(bool pohlavi, int spozdeni, bool vypis)
{
    if(pohlavi)
        printf("\n\n\nbezi muzi\n");
    else
        printf("bezi zeny\n");
    printf("je nastaveno %dms spozdeni\n",spozdeni);
    if(vypis)
        printf("prubezny vypis casu je povolen\n");
    else
        printf("prubezny vypis casu je zakazan\n");
    return;
}

void printDelayHelp()
{
    printf("\n\n\ndelka spozdeni mezi komunikaci s tercama (ovlivnuje presnost a narocnost)\n");
    printf("0           bez spozdeni\n");
    printf("1           1ms\n");
    printf("2           5ms (default)\n");
    printf("3           10ms\n");
    printf("4           25ms\n");
    printf("5           50ms\n");
    printf("6           100ms\n");
    printf("7           200ms\n");
    printf("8           500ms\n");
    printf("9           1000ms\n");
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

int main(int argc, char **argv)
{	
    /* initialize SDL */
    if(SDL_Init(0)==-1)
    {
            printf("SDL_Init: %s\n",SDL_GetError());
            exit(1);
    }

    /* initialize SDL_net */
    if(SDLNet_Init()==-1)
    {
            printf("SDLNet_Init: %s\n",SDLNet_GetError());
            exit(2);
    }
    
    Mix_SetSoundFonts("zvuky/organ.sf2");
    
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    
    Mix_Chunk *prvni = NULL;
    Mix_Chunk *druhy = NULL;
    Mix_Chunk *treti = NULL;
    Mix_Chunk *ctvrty = NULL;
    
    prvni = Mix_LoadWAV( "zvuky/prvni.wav" );
    if( prvni == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    druhy = Mix_LoadWAV( "zvuky/druhy.wav" );
    if( druhy == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    treti = Mix_LoadWAV( "zvuky/treti.wav" );
    if( treti == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    ctvrty = Mix_LoadWAV( "zvuky/ctvrty.wav" );
    if( ctvrty == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    int zvuk;
    
    srand (time(NULL));
    
    IPaddress* espAddress;
    UDPsocket socketOut;
    socketOut = SDLNet_UDP_Open(0);
    if(!socketOut)
    {
        printf("Could not allocate receiving socket\n");
        exit(4);
    }
    
    espAddress = SDLNet_UDP_GetPeerAddress(socketOut, -1);
    // resolve server host
    SDLNet_ResolveHost(espAddress, "192.168.4.1", 1234);
    UDPpacket *packetOut;
    if(!(packetOut = SDLNet_AllocPacket(8)))
    {
        printf("Could not allocate packet\n");
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
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
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
    if(!posliPacket("s",packetOut,&socketOut))
        printf("%s",SDLNet_GetError());
    while(!quit)
    {

        if(SDLNet_UDP_Recv(socketIn, packetIn)) {
            prijato = packetIn->data[0];
            switch(prijato)
            {
                case 'm':
                    pohlavi = true;
                    break;
                case 'z':
                    pohlavi = false;
                    break;
                case 'p':
                    if(pravy == 0)
                        pravy = ((float)(SDL_GetTicks() - start))/(float)1000;
                    break;
                case 'l':
                    if(levy == 0)
                        levy = ((float)(SDL_GetTicks() - start))/(float)1000;
                    break;
                case '1':
                    printf("terce jsou v poradku\n");
                    break;
                case '0':
                    printf("CHYBA! je potreba zvednout terce\n");
                    break;
            }
        }
        else
            printf("%s",SDLNet_GetError());
        if(kbhit())
        {
            znak = getchar();
            printf("\n");
            switch(znak)
            {
                case 'm':
                    if(!posliPacket("m",packetOut,&socketOut))
                        printf("%s",SDLNet_GetError());
                    pohlavi = true;
                    break;
                case 'z':
                    if(posliPacket("z",packetOut,&socketOut))
                        printf("%s",SDLNet_GetError());
                    pohlavi = false;
                    break;
                case 'h':
                    printHelp();
                    break;
                case '0':
                    spozdeni = 0;
                    break;
                case '1':
                    spozdeni = 1;
                    break;
                case '2':
                    spozdeni = 5;
                    break;
                case '3':
                    spozdeni = 10;
                    break;
                case '4':
                    spozdeni = 25;
                    break;
                case '5':
                    spozdeni = 50;
                    break;
                case '6':
                    spozdeni = 100;
                    break;
                case '7':
                    spozdeni = 200;
                    break;
                case '8':
                    spozdeni = 500;
                    break;
                case '9':
                    spozdeni = 1000;
                    break;
                case 'd':
                    printDelayHelp();
                    break;
                case 's':
                    if(vystrel)
                        vystrel = false;
                    else
                        vystrel = true;
                    break;
                case 't':
                    if(posliPacket("t",packetOut,&socketOut))
                        printf("%s",SDLNet_GetError());
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
            printf("%f",levy);
            printf("                      %f\n",pravy);
            cislo = Convert(levy);
            char *retezec = stringToChar(cislo);
            if(posliPacket(retezec,packetOut,&socketOut))
                printf("%s",SDLNet_GetError());
            cislo = Convert(pravy);
            delete retezec;
            retezec = stringToChar(cislo);
            if(posliPacket(retezec,packetOut,&socketOut))
                printf("%s",SDLNet_GetError());
            delete retezec;
            started = false;
        }
        if(vypis && started)
        {
            ted = SDL_GetTicks();
            if(levy == 0)
                printf("%f",((float)(ted - start))/(float)1000);
            else
                printf("%f",levy);
            if(pravy == 0)
                printf("                      %f\n",((float)(ted - start))/(float)1000);
            else
                printf("                      %f\n",pravy);
        }
        if(spozdeni)
            SDL_Delay(spozdeni);
    }
    
    Mix_FreeChunk(prvni);
    Mix_FreeChunk(druhy);
    Mix_FreeChunk(treti);
    Mix_FreeChunk(ctvrty);

    Mix_Quit();
    
    /* shutdown SDL_net */
    SDLNet_Quit();

    /* shutdown SDL */
    SDL_Quit();

    return(0);
}