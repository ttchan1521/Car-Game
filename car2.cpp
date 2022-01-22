#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include<ctime>
#include<cmath>
#include<SDL_ttf.h>
#include<sstream>
#include<SDL_mixer.h>
using namespace std;

const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 340;

class LTexture{
public:
    LTexture();
    ~LTexture();

    bool loadFromFile(string path);
    void free();
    void render(int x, int  y);
    void handleEvent(SDL_Event &e);
    void Move();
    bool loadFromRenderText(string textureText, SDL_Color textColor);

    int getWidth();
    int getHeight();
    double  mPosX;
    double mPosY;
private:
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
    int mVelX;
};


bool init();
bool loadMedia();
void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
LTexture gCarTexture;
LTexture gBGTexture;
LTexture gImpediment[20];
LTexture gFire;
TTF_Font *gFont = NULL;
LTexture gStart;
LTexture gScore;
Mix_Music *gMusic = NULL;
Mix_Chunk *gTing = NULL;

LTexture::LTexture()
{
    mTexture = NULL;
    mWidth =0;
    mHeight = 0;
    mPosX =0;
    mPosY =0;
    mVelX =0;
}

LTexture::~LTexture()
{
    free();
}

bool LTexture::loadFromFile(string path)
{
    free();
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        cout << "Unable to load image" << IMG_GetError();
    }
    else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            cout << "Unable to create texture from " << SDL_GetError();
        }
        else{
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        SDL_FreeSurface(loadedSurface);
    }
    mTexture = newTexture;
    return mTexture!=NULL;
}

void LTexture::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mWidth = 0;
        mHeight =0;
    }
}

void LTexture::render(int x, int y)
{
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad);
}

void LTexture::handleEvent(SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch(e.key.keysym.sym)
        {
            case SDLK_LEFT: mVelX -= 100; break;
            case SDLK_RIGHT: mVelX +=100; break;
        }

    }
    else if (e.type == SDL_KEYUP && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
            case SDLK_LEFT: mVelX += 100; break;
            case SDLK_RIGHT: mVelX -= 100; break;
        }
    }
    mPosX += mVelX;
    if ((mPosX<0) || (mPosX+ mWidth > SCREEN_WIDTH))
        mPosX -=mVelX;
}
bool LTexture::loadFromRenderText(string textureText, SDL_Color textColor)
{
    free();
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
    if (textSurface == NULL)
        cout << "Unable to render text surface ";
    else
    {
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if (mTexture == NULL)
            cout << "Unable to create texture from rendered text";
        else
        {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        SDL_FreeSurface(textSurface);
    }
    return mTexture!=NULL;
}
int LTexture::getHeight()
{
    return mHeight;
}

int LTexture::getWidth()
{
    return mWidth;
}

bool init()
{
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) <0)
    {
        cout << "SDL could not initialize" << SDL_GetError();
        success = false;
    }
    else{
            gWindow = SDL_CreateWindow("TT", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    {
        gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED );
        if (gRenderer == NULL)
        {
            cout << "Renderer could not be created " << SDL_GetError();
            success = false;

        }
        else
        {
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags))
            {
                cout << "SDL_image could not initialize" << IMG_GetError();
                success = false;
            }
            if (TTF_Init() == -1)
            {
                cout << "SDL_ttf could not initialize";
                success = false;
            }
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) <0)
            {
                cout << "SDL_mixer could not initialize";
                success = false;
            }
        }}

    }
    return success;
}

bool loadMedia()
{
    bool success = true;
    if (!gCarTexture.loadFromFile("oto.png"))
    {
        cout << "Failed to load car texture";
        success = false;
    }
    if (!(gBGTexture.loadFromFile("road.png")))
    {
        cout << "Failed to load background texture";
        success = false;
    }
    for (int i=0; i<10; i++)
        if (!gImpediment[i].loadFromFile("da.png"))
    {
        cout << "Failed to load impediment texture";
        success = false;
    }
    if (!gFire.loadFromFile("fire.png"))
    {
        cout << "Failed to load fire texture";
        success = false;
    }

    gFont = TTF_OpenFont("16_true_type_fonts/lazy.ttf", 34);

    if (gFont == NULL)
    {
        cout << "Failed to load lazy font";
        success = false;
    }
    else
    {
        SDL_Color textColor = {0xFF, 0, 0};
        if (!gStart.loadFromRenderText("PRESS SPACE TO START", textColor))
        {
            cout << "Failed to render text texture";
            success= false;
        }
    }
    gMusic = Mix_LoadMUS("gMusic.wav");
    if (gMusic == NULL)
    {
        cout << "Failed to load music";
        success = false;
    }
    gTing = Mix_LoadWAV("gTing.wav");
    if (gTing == NULL)
    {
        cout << "Failed to load sound effect";
        success = false;
    }

    return success;
}

void close()
{
    gCarTexture.free();
    gBGTexture.free();
    gStart.free();
    TTF_CloseFont(gFont);
    gFont = NULL;

    Mix_FreeChunk(gTing);
    gTing = NULL;
    Mix_FreeMusic(gMusic);
    gMusic = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void getPos()
{
    gBGTexture.mPosX = 0;
    gBGTexture.mPosY = -(gBGTexture.getHeight()-SCREEN_HEIGHT);
    gCarTexture.mPosX = (SCREEN_WIDTH-gCarTexture.getWidth())/2;
    gCarTexture.mPosY = SCREEN_HEIGHT - gCarTexture.getHeight();
    for (int i=0; i<3; i++)
    {
        int t = rand() % 5;
        gImpediment[i].mPosX = 20;
        gImpediment[i].mPosY = -gBGTexture.getHeight()+SCREEN_HEIGHT+ 800*t;
    }
    for (int i=3; i<7; i++)
    {
        int t = rand() % 7;
        gImpediment[i].mPosX = 120;

        gImpediment[i].mPosY = -gBGTexture.getHeight()+SCREEN_HEIGHT+120+ 500*t;
    }
    for (int i=7; i<10; i++)
    {
        int t = rand() % 7;
        gImpediment[i].mPosX = 220;
        for (int j=0; j<7; j++)
            if (fabs(gImpediment[i].mPosY-(250+300*t))<400) t++;
        gImpediment[i].mPosY = -gBGTexture.getHeight()+SCREEN_HEIGHT+250+ 300*t;
    }
}

bool Collision(LTexture& a,  LTexture& b)
{
    double leftA, leftB;
    double rightA, rightB;
    double topA, topB;
    double bottomA, bottomB;
    leftA = a.mPosX;
    rightA = a.mPosX+a.getWidth();
    topA = a.mPosY ;
    bottomA = a.mPosY + a.getHeight();

    leftB = b.mPosX;
    rightB = b.mPosX + b.getWidth();
    topB = b.mPosY;
    bottomB = b.mPosY + b.getHeight();

    if (bottomA <= topB) return false;
    if (topA >= bottomB) return false;
    if (rightA <= leftB) return false;
    if (leftA >= rightB) return false;
    return true;
}

bool checkCollision()
{
    for (int i=0; i<10; i++)
        if (Collision(gCarTexture, gImpediment[i]))
            return true;
    return false;
}
int main(int argc, char* args[])
{
    srand(time(0));
    if (!init())
        cout << "Failed to initialize";
    else
    {
        if (!loadMedia())
            cout << "Failed to load media";
        else
        {
            bool quit = false;
            SDL_Event e;
            bool kt = false;
            int score =0;
            stringstream scoreText;
            double scrollingOffset = 0.2;
            getPos();
            while (!quit)
            {
                if (kt == false)
                    gStart.render((SCREEN_WIDTH - gStart.getWidth())/2, (SCREEN_HEIGHT - gStart.getHeight())/2);
                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT)
                        quit = true;
                    if ((e.type == SDL_KEYDOWN) && (e.key.keysym.sym == SDLK_SPACE)){
                        kt = true;
                        getPos();
                        scrollingOffset = 0.2;
                        score= 0;
                    }
                    gCarTexture.handleEvent(e);
                }
                if (kt == true)
                {
                Mix_PlayMusic(gMusic, -1);
                if (checkCollision())
                    scrollingOffset = 0;

                if (gBGTexture.mPosY >=0)
                    gCarTexture.mPosY -=scrollingOffset;
                else
                {
                    gBGTexture.mPosY += scrollingOffset;
                    for (int i=0; i<10; i++)
                    gImpediment[i].mPosY += scrollingOffset;
                }
                if (gCarTexture.mPosY<= 0){
                    getPos();
                    score++;
                    Mix_PlayChannel(-1, gTing, 0);
                    scrollingOffset +=0.05;
                }

                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);
                gBGTexture.render(gBGTexture.mPosX, gBGTexture.mPosY);
                gCarTexture.render(gCarTexture.mPosX, gCarTexture.mPosY);
                for (int i=0; i<10; i++)
                    gImpediment[i].render(gImpediment[i].mPosX, gImpediment[i].mPosY);
                if (scrollingOffset ==0) {
                    gFire.render(gCarTexture.mPosX, gCarTexture.mPosY);
                    scoreText.str("");
                    scoreText << "Your score: " << score;
                    SDL_Color textColor = {0xFF, 0, 0};
                    if (!gScore.loadFromRenderText(scoreText.str().c_str(), textColor))
                        cout << "Unable to render score texture";
                    gScore.render((SCREEN_WIDTH-gScore.getWidth())/2, (SCREEN_HEIGHT-gScore.getHeight())/2-100);
                    kt = false;
                    Mix_HaltMusic();
                }}
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();

    return 0;
}

