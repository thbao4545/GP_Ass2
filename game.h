#include "main.h"

class Timer
{
private:
    static Timer *sInstance;
    unsigned int mElapsedTicks;
    float mDeltaTime;
    float mTimeScale;

public:
    unsigned int mStartTicks;

public:
    static Timer *Instance();
    static void Release();
    void Reset();
    float DeltaTime();
    void TimeScale(float t);
    float TimeScale();
    void Update();

private:
    Timer();
    ~Timer();
};

class Window
{
public:
    Window(const string &title, int width, int height, int fontSize, int ballSize, int speed, float ballSpeed, float pi, int timeEnd, int frameRate);
    ~Window();

    void pollEvents();
    void clear() const;

    inline bool isClosed() const { return closed; }

private:
    bool init();
    void start();
    void scoreCounter(string text, int x, int y);
    void timeCounter(string text, int x, int y);
    void serve();
    void update();
    void rightInput();
    void leftInput();
    void reRender();
    void renderBackground();
    void renderOverTime();
    bool checkPlayersCollision(SDL_Rect P1, SDL_Rect P2);
    void resetInitialState();

    // Window properties
    string title;
    int width;
    int height;
    int fontSize;
    int ballSize;
    int speed;
    float ballSpeed;
    float ballSpeedCurr;
    bool hitP1[2];
    bool hitP2[2];
    bool mode = 0;
    int stepP1[2];
    const char *runningP1[10] = {"./images/Run1P1.png", "./images/Run2P1.png", "./images/Run3P1.png", "./images/Run4P1.png", "./images/Run5P1.png", "./images/Run6P1.png", "./images/Run5P1.png", "./images/Run4P1.png", "./images/Run3P1.png", "./images/Run2P1.png"};
    int wLeftPlayer = 0;
    int stepP2[2];
    const char *runningP2[10] = {"./images/Run1P2_.png", "./images/Run2P2_.png", "./images/Run3P2_.png", "./images/Run4P2_.png", "./images/Run5P2_.png", "./images/Run6P2_.png", "./images/Run5P2_.png", "./images/Run4P2_.png", "./images/Run3P2_.png", "./images/Run2P2_.png"};
    int wRightPlayer = 0;
    float pi;

    // Fps
    int timeEnd;
    int frameRate;

    // Main
    SDL_Rect leftPlayer[2], rightPlayer[2], ball;
    SDL_Rect time_board, score_board;
    float veloX, veloY;
    string score;
    int leftScore, rightScore;
    bool turn;

    // Timer
    Timer *mTimer;
    int lastTime;
    string time;

    // Font
    TTF_Font *font;

    // Status
    bool closed = false;
    bool ready = false;
    bool started = false;
    bool over = false;
    bool rePlay = false;

    // Common variables
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *textureBg = nullptr;
    SDL_Texture *textureP1_1;
    SDL_Texture *textureP1_2;
    SDL_Texture *textureP2_1;
    SDL_Texture *textureP2_2;
    SDL_Texture *textureBall = nullptr;
    SDL_Texture *textureTime = nullptr;
    SDL_Texture *textureScore = nullptr;
    Mix_Music *music;
    Mix_Chunk *sound;

public:
    SDL_Event windowEvent;
};