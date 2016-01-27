// event.cc

#include<iostream>
#include<cstdlib>
#include<string>
#include "SDL.h"
#include "SDL_image.h"
#include <SDL/SDL_ttf.h>
#include <sstream>

using namespace std;

/*------------------------------ CONSTANTES ------------------------------*/

//constante pour la l'écran
const int SCREEN_HEIGHT = 400;
const int SCREEN_WIDTH = 640;
const int SCREEN_BPP = 32;

//constante pour les position pour le mur 1
const int WALL1_POSX = 20;
const int WALL1_POSY = SCREEN_HEIGHT/2;

//constante pour les positions pour le mur 2
const int WALL2_POSX = 600;
const int WALL2_POSY = SCREEN_HEIGHT/2;

//constante hauteur et largeur pour les murs
const int WALL_HEIGHT =50;
const int WALL_WIDTH =10;

// constante pour la taille de la balle
const int TAILLE=20;

int scoreJoeur1 = 0;
int scoreJoeur2 = 0;
ostringstream score1;
ostringstream score2;
ostringstream text1;
ostringstream text2;

/*------------------------------ FONCTION POUR LES IMAGES ------------------------------*/

SDL_Surface *loadImageWithColorKey(string filename, int r, int g, int b)
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old image
        SDL_FreeSurface( loadedImage );

        //If the image was optimized just fine
        if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, r, g, b );

            //Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
    }
    //Return the optimized image
    return optimizedImage;
}

void applySurface(int x, int y, SDL_Surface* source,
                  SDL_Surface* destination, SDL_Rect* clip)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, clip, destination, &offset );
}

struct ball
{
    SDL_Rect rballe;
    int mvt_x; // mouvement sur l'axe des abscisses
    int mvt_y; // mouvement sur l'axe des ordonnées
};

struct wall
{
    SDL_Rect mur;
};

void initBall(ball &b)
{
    b.rballe.x=SCREEN_WIDTH/2-TAILLE/2;
    b.rballe.y=SCREEN_HEIGHT/2-TAILLE/2;
    b.rballe.w=TAILLE;
    b.rballe.h=TAILLE;
    b.mvt_x=2;
    b.mvt_y=2;
}

bool collision(SDL_Rect a, SDL_Rect b)
{
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    if(bottomA <= topB)
        return false;
    if(topA >= bottomB)
        return false;
    if(rightA <= leftB)
        return false;
    if(leftA >= rightB)
        return false;

    return true;
}

int moveBall(ball &b, wall w1, wall w2, int &gagner)
{
    SDL_Rect tmp;

    b.rballe.x+=b.mvt_x;
    b.rballe.y+=b.mvt_y;

    tmp.x=b.rballe.x-TAILLE/2;
    tmp.y=b.rballe.y-TAILLE/2;
    tmp.h=TAILLE;
    tmp.w=TAILLE;

    if(collision(tmp,w1.mur) || collision(tmp,w2.mur))
    {
        b.mvt_x*=-1;
    }

    if(b.rballe.x>SCREEN_WIDTH-40)
    {
        initBall(b);
        scoreJoeur1++;
        score1.flush();
        score1.str("");
        score1<<  scoreJoeur1;

        if(scoreJoeur1 == 2)
        {
            gagner = 1;
        }
    }

    if(b.rballe.x<20)
    {
        initBall(b);
        scoreJoeur2++;
        score2.flush();
        score2.str("");
        score2<<  scoreJoeur2;

        if (scoreJoeur2 == 2)
        {
            gagner = 2;
        }
    }

    if(b.rballe.y>SCREEN_HEIGHT)
    {
        b.rballe.y=SCREEN_HEIGHT-TAILLE;
        b.mvt_y*=-1;
    }

    if(b.rballe.y<0)
    {
        b.rballe.y=0;
        b.mvt_y*=-1;
    }

    SDL_Delay(10);

    return gagner;
}


void showball(ball b, SDL_Surface *ball,SDL_Surface *screen)
{
    SDL_BlitSurface(ball,NULL,screen,&b.rballe);
}
void initWall(wall &w1)
{
    w1.mur.x=WALL1_POSX;
    w1.mur.y=WALL1_POSY;
    w1.mur.w=WALL_WIDTH;
    w1.mur.h=WALL_HEIGHT;
}

void initWall2(wall &w2)
{
    w2.mur.x=WALL2_POSX;
    w2.mur.y=WALL2_POSY;
    w2.mur.w=WALL_WIDTH;
    w2.mur.h=WALL_HEIGHT;
}

void showwall1(wall w1, SDL_Surface *player01, SDL_Surface *screen)
{
    SDL_BlitSurface(player01,NULL,screen,&w1.mur);
}

void showwall2(wall w2,SDL_Surface *player02, SDL_Surface *screen)
{
    SDL_BlitSurface(player02,NULL,screen,&w2.mur);
}

void bouger(wall &w1, wall &w2)
{
    Uint8 *keystates = SDL_GetKeyState( NULL );

    //If up is pressed
    if( keystates[ SDLK_e ] )
    {
        if(w1.mur.y>0)
            w1.mur.y-=6;
    }

    //If down is pressed
    if( keystates[ SDLK_d ] )
    {
        if(w1.mur.y+w1.mur.h<SCREEN_HEIGHT)
            w1.mur.y+=6;
    }

    if( keystates[ SDLK_UP ] )
    {
        if(w2.mur.y>0)
            w2.mur.y-=6;
    }
    //If down is pressed
    if( keystates[ SDLK_DOWN ] )
    {
        if(w2.mur.y+w2.mur.h<SCREEN_HEIGHT)
            w2.mur.y+=6;
    }
}

void showMessageScreen(string message,int x,int y,
                       TTF_Font *font,int fontSize,SDL_Color textColor,SDL_Surface* &screen)
{
    string mot="";
    string space=" ";
    int i=0,j;
    SDL_Surface *mes=NULL;

    j = message.find(space);
    while( j != string::npos )
    {
        mot=message.substr(i,j-i);
        if(mot != "")
        {
            mes=TTF_RenderText_Solid(font,mot.c_str(),textColor);
            applySurface(x,y,mes,screen,NULL);
            x+=mes->w;
            SDL_FreeSurface(mes);
        }
        x+=fontSize;
        i=j+1;
        j = message.find(space,i);
    }

    mot=message.substr(i);
    mes=TTF_RenderText_Solid(font,mot.c_str(),textColor);
    applySurface(x,y,mes,screen,NULL);
    SDL_FreeSurface(mes);
}


int main(int argc, char* argv[])
{

    TTF_Init();
    TTF_Font *fonts;
    SDL_Color textColor= {255,255,255};
    int fontSize=26;
    string police="Space_age.ttf";
    fonts = TTF_OpenFont(police.c_str(),fontSize);

    score1.flush();
    score1.str("");
    score1 <<  scoreJoeur1;

    score2.flush();
    score2.str("");
    score2 <<  scoreJoeur2;

    text1.flush();
    text1.str("");
    text1 <<  "Score ";

    text2.flush();
    text2.str("");
    text2 <<  "-";

    int gagner =0;

    bool quit=false;
    bool play=false;

    bool menuPlaySurvol=false;
    bool menuQuitSurvol=false;

    ball b;

    wall w1;
    wall w2;

    initWall(w1);
    initWall2(w2);
    initBall(b);
    SDL_Event event;
    SDL_Surface *screen;
    SDL_Surface *menu;
    SDL_Surface *player01;
    SDL_Surface *player02;
    SDL_Surface *background;
    SDL_Surface *title;
    SDL_Surface *ball;
    SDL_Surface *victoire1;
    SDL_Surface *victoire2;

    SDL_Rect fondScore;

    fondScore.x=0;
    fondScore.y=0;
    fondScore.w=640;
    fondScore.h=30;

    SDL_Rect positionBackground;

    positionBackground.x=0;
    positionBackground.y=0;

    SDL_Rect positionTitle;
    positionTitle.x=0;
    positionTitle.y=0;

    SDL_Rect positionVictoire;
    positionVictoire.x=0;
    positionVictoire.y=0;


    SDL_Rect lecturePlay;
    lecturePlay.x=0;
    lecturePlay.y=100;
    lecturePlay.w=150;
    lecturePlay.h=100;

    SDL_Rect lectureQuit;
    lectureQuit.x=0;
    lectureQuit.y=0;
    lectureQuit.w=150;
    lectureQuit.h=100;

    SDL_Rect lecturePlaySurvol;
    lecturePlaySurvol.x=200;
    lecturePlaySurvol.y=100;
    lecturePlaySurvol.w=150;
    lecturePlaySurvol.h=100;

    SDL_Rect lectureQuitSurvol;
    lectureQuitSurvol.x=200;
    lectureQuitSurvol.y=0;
    lectureQuitSurvol.w=150;
    lectureQuitSurvol.h=100;

    SDL_Init(SDL_INIT_EVERYTHING);

    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);

    title = SDL_LoadBMP("title.bmp");
    background = SDL_LoadBMP("fond.bmp");

    menu = loadImageWithColorKey("button.bmp",0,0,0);

    player01 = loadImageWithColorKey("player01.bmp",0,255,255);
    player02 = loadImageWithColorKey("player02.bmp",0,255,255);

    ball = loadImageWithColorKey("ball.bmp",0,255,255);

    victoire1 = SDL_LoadBMP("victoire1.bmp");
    victoire2 = SDL_LoadBMP("victoire2.bmp");

    while(quit==false)
    {
        SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,255,255,255));


        SDL_BlitSurface(title,NULL,screen,&positionTitle);

        applySurface(240,220,menu,screen,&lecturePlay);
        applySurface(240,320,menu,screen,&lectureQuit);


        while(SDL_PollEvent(&event))
        {
            if( event.type == SDL_MOUSEMOTION  )
            {
                //les coordonnées de la souris
                int x;
                int y;

                SDL_GetMouseState(&x,&y);

                if( (( x > 240) && ( x < 390) && ( y > 220 ) && ( y < 280 )) )
                {
                    menuPlaySurvol=true;
                }

                else
                {
                    menuPlaySurvol=false;
                }

                if( (( x > 240) && ( x < 390) && ( y > 320 ) && ( y < 380 )) )
                {
                    menuQuitSurvol=true;
                }
                else
                {
                    menuQuitSurvol=false;
                }
            }

            if( event.type == SDL_MOUSEBUTTONDOWN )
            {
                //les coordonnées de la souris
                int x;
                int y;
                SDL_GetMouseState(&x,&y);

                if( (( x > 240) && ( x < 390) && ( y > 220 ) && ( y < 280 )) )
                {
                    play=true;
                }

                if( (( x > 240) && ( x < 390) && ( y > 320 ) && ( y < 380 )) )
                {
                    quit=true;
                }
            }

            if( event.type == SDL_QUIT )
            {
                quit = true;
            }
        }

        if (menuPlaySurvol == true)
        {
            applySurface(240,220,menu,screen,&lecturePlaySurvol);
        }

        if (menuQuitSurvol == true)
        {
            applySurface(240,320,menu,screen,&lectureQuitSurvol);
        }

        if (play == true)
        {
            SDL_BlitSurface(background,NULL,screen,&positionBackground);
            SDL_FillRect(screen,&fondScore,SDL_MapRGB(screen->format,0,0,0));

            showMessageScreen(score1.str(),320,0,fonts,fontSize,textColor,screen);
            showMessageScreen(score2.str(),390,0,fonts,fontSize,textColor,screen);
            showMessageScreen(text1.str(),180,0,fonts,fontSize,textColor,screen);
            showMessageScreen(text2.str(),360,0,fonts,fontSize,textColor,screen);

            showball(b,ball,screen);
            moveBall(b,w1,w2,gagner);

            showwall1(w1,player01,screen);
            showwall2(w2,player02,screen);
            bouger(w1,w2);
            if (gagner == 1)
            {
                SDL_BlitSurface(victoire1,NULL,screen,&positionVictoire);
            }

            if (gagner == 2)
            {
                SDL_BlitSurface(victoire2,NULL,screen,&positionVictoire);
            }

        }

        SDL_Flip(screen);
    }
    SDL_FreeSurface(screen);//Free the image
    SDL_Quit();//Quit SDL#include <SDL/SDL_ttf.h>
    return EXIT_SUCCESS;
}

// A FAIRE : COLLISION MUR
