#include "game.h"

/////////////////////////////////////////////////// Window Class ///////////////////////////////////////////////////

Window::Window(const string &title, int width, int height, int fontSize, int ballSize, int speed, float ballSpeed, float pi, int timeEnd, int frameRate)
{
    this->title = title;
    this->width = width;
    this->height = height;
    this->fontSize = fontSize;
    this->ballSize = ballSize;
    this->speed = speed;
    this->ballSpeed = ballSpeed;
    this->ballSpeedCurr = ballSpeed;
    this->pi = pi;
    this->timeEnd = timeEnd;
    this->frameRate = frameRate;
    this->closed = !init();
}

Window::~Window()
{
    SDL_DestroyTexture(this->textureBg);
    SDL_DestroyTexture(this->textureP1_1);
    SDL_DestroyTexture(this->textureP1_2);
    SDL_DestroyTexture(this->textureP2_1);
    SDL_DestroyTexture(this->textureP2_2);
    SDL_DestroyTexture(this->textureBall);
    SDL_DestroyTexture(this->textureTime);
    SDL_DestroyTexture(this->textureScore);
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    Timer::Release();
    this->mTimer = NULL;
}

bool Window::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Failed to initialize SDL\nSDL Error: " << SDL_GetError() << endl;
        return 0;
    };

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        cout << "Failed to initialize SDL_IMG\nSDL Error: " << SDL_GetError() << endl;
        return 0;
    };

    if (TTF_Init() == -1)
    {
        cout << "Failed to initialize SDL_ttf\nSDL Error: " << SDL_GetError() << endl;
        return 0;
    };

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    this->music = Mix_LoadMUS("./src/music/themesong.mp3");
    if (!this->music)
    {
        cout << "Music Error: " << Mix_GetError() << endl;
        return 0;
    }
    this->sound = Mix_LoadWAV("./src/music/shoot.flac");
    if (!this->sound)
    {
        cout << "Sound Error: " << Mix_GetError() << endl;
    };

    Mix_PlayMusic(music, -1);

    this->font = TTF_OpenFont("./src/font/Peepo.ttf", this->fontSize);

    this->window = SDL_CreateWindow(this->title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->width, this->height, SDL_WINDOW_ALLOW_HIGHDPI);

    if (!this->window)
    {
        cout << "Failed to create window\nSDL Error: " << SDL_GetError() << endl;
        return 0;
    };

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface *imageBackground = IMG_Load("./images/Startgame_.png");
    SDL_Surface *imageBall = IMG_Load("./images/SoccerBall_.png");

    this->textureBg = SDL_CreateTextureFromSurface(this->renderer, imageBackground);
    this->textureBall = SDL_CreateTextureFromSurface(this->renderer, imageBall);

    SDL_FreeSurface(imageBackground);
    SDL_FreeSurface(imageBall);

    if (!this->renderer)
    {
        cout << "Failed to create renderer\nSDL Error: " << SDL_GetError() << endl;
    }

    this->start();

    return 1;
}

void Window::start()
{
    // Initialize the values
    this->resetInitialState();

    const int frameDelay = 1000 / this->frameRate;
    Uint32 frameStart;
    int frameTime;

    while (!this->isClosed())
    {
        frameStart = SDL_GetTicks();

        frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }

        if (SDL_PollEvent(&this->windowEvent))
        {
            if (SDL_QUIT == this->windowEvent.type)
            {
                this->~Window();
                break;
            }
        }
        if (this->ready)
        {
            if (this->started)
            {
                if (this->over)
                {
                    const Uint8 *keystates = SDL_GetKeyboardState(NULL);

                    if (keystates[SDL_SCANCODE_BACKSPACE])
                    {
                        this->started = false;
                        this->ready = false;
                        this->over = false;
                        this->rePlay = true;
                        SDL_Surface *imageBg = IMG_Load("./images/Startgame_.png");
                        this->textureBg = SDL_CreateTextureFromSurface(this->renderer, imageBg);
                        SDL_FreeSurface(imageBg);

                        this->renderBackground();

                        this->resetInitialState();
                    }
                    else
                    {
                        this->renderOverTime();
                    }
                }
                else
                {
                    this->mTimer->Update();
                    if (this->lastTime != 0)
                    {
                        if (this->lastTime != (15 - floor(this->mTimer->DeltaTime())))
                        {
                            this->lastTime = this->lastTime - 1;
                        };

                        this->time = to_string(this->lastTime);

                        this->update();
                        this->leftInput();
                        if (this->mode)
                            this->rightInput();

                        // Swap player
                        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

                        if (keystates[SDL_SCANCODE_F])
                        {
                            if (this->wLeftPlayer == 0)
                            {
                                this->wLeftPlayer = 1;
                            }
                            else
                            {
                                this->wLeftPlayer = 0;
                            }
                        }
                        if (keystates[SDL_SCANCODE_L])
                        {
                            if (this->wRightPlayer == 0)
                            {
                                this->wRightPlayer = 1;
                            }
                            else
                            {
                                this->wRightPlayer = 0;
                            }
                        };
                    }
                    else
                    {
                        this->over = true;
                    }
                    this->reRender();
                }
            }
            else
            {
                const Uint8 *keystates = SDL_GetKeyboardState(NULL);

                if (keystates[SDL_SCANCODE_SPACE])
                {
                    this->started = true;
                    if (!this->rePlay)
                    {
                        this->mTimer->mStartTicks = SDL_GetTicks();
                        this->mTimer->Reset();
                    }
                    else
                    {
                        this->mTimer->Reset();
                    }
                }
            }
        }
        else
        {
            const Uint8 *keystates = SDL_GetKeyboardState(NULL);

            if (keystates[SDL_SCANCODE_DOWN])
            {
                if (!this->mode)
                {
                    SDL_Surface *imageBg = IMG_Load("./images/Startgame2_.png");
                    this->textureBg = SDL_CreateTextureFromSurface(this->renderer, imageBg);
                    SDL_FreeSurface(imageBg);
                    this->mode = !this->mode;
                }
            }
            if (keystates[SDL_SCANCODE_UP])
            {
                if (this->mode)
                {
                    SDL_Surface *imageBg = IMG_Load("./images/Startgame_.png");
                    this->textureBg = SDL_CreateTextureFromSurface(this->renderer, imageBg);
                    SDL_FreeSurface(imageBg);
                    this->mode = !this->mode;
                }
            }
            if (keystates[SDL_SCANCODE_RETURN])
            {
                if (!this->ready)
                {
                    this->ready = true;
                    this->reRender();
                }
            }

            if (!this->ready)
                this->renderBackground();
        }
    }
    this->~Window();
}

void Window::scoreCounter(string text, int x, int y)
{
    SDL_Surface *surface;
    SDL_Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    if (this->font == NULL)
    {
        cout << "Font not found !" << endl;
        exit(EXIT_FAILURE);
    }
    const char *t = text.c_str();
    surface = TTF_RenderText_Solid(this->font, t, color);
    this->textureScore = SDL_CreateTextureFromSurface(this->renderer, surface);
    this->score_board.w = surface->w;
    this->score_board.h = surface->h;
    this->score_board.x = x - this->score_board.w;
    this->score_board.y = y - this->score_board.h;
    SDL_FreeSurface(surface);
}

void Window::timeCounter(string text, int x, int y)
{
    SDL_Surface *surface;
    SDL_Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    if (this->font == NULL)
    {
        cout << "Font not found !" << endl;
        exit(EXIT_FAILURE);
    }
    const char *t = text.c_str();
    surface = TTF_RenderText_Solid(this->font, t, color);
    this->textureTime = SDL_CreateTextureFromSurface(this->renderer, surface);
    this->time_board.w = surface->w;
    this->time_board.h = surface->h;
    this->time_board.x = x - this->time_board.w;
    this->time_board.y = y - this->time_board.h;
    SDL_FreeSurface(surface);
}

void Window::serve()
{
    this->stepP1[0] = 0;
    this->stepP1[1] = 0;
    this->stepP2[0] = 0;
    this->stepP2[1] = 0;
    this->hitP1[0] = this->hitP1[1] = false;
    this->hitP2[0] = this->hitP2[1] = false;

    this->ballSpeedCurr = this->ballSpeed;

    this->leftPlayer[0].x = 32;
    this->leftPlayer[0].y = (this->height / 4) - (this->leftPlayer[0].h / 2);
    this->leftPlayer[1] = this->leftPlayer[0];
    this->leftPlayer[1].y = (3 * this->height / 4) - (this->leftPlayer[1].h / 2);

    this->rightPlayer[0].y = this->leftPlayer[0].y;
    this->rightPlayer[0].x = this->width - this->rightPlayer[0].w - 32;
    this->rightPlayer[1] = this->rightPlayer[0];
    this->rightPlayer[1].y = this->leftPlayer[1].y;

    this->ball.x = this->width / 2 - this->ballSize / 2 + 5;
    this->ball.y = this->height / 2 - this->ballSize - 5;

    if (this->turn)
    {
        this->veloX = this->ballSpeedCurr / 2;
    }
    else
    {
        this->veloX = -this->ballSpeedCurr / 2;
    }
    this->veloY = 0;
    this->turn = !this->turn;
}

void Window::update()
{
    // Check if the ball collides with player
    if (SDL_HasIntersection(&this->ball, &this->leftPlayer[0]))
    {
        // Check where the ball hits the player
        double collidePoint = (this->leftPlayer[0].y + (this->leftPlayer[0].h / 2)) - (this->ball.y + (this->ballSize / 2));
        // Normalize the value of collidePoint, need to get numbers between -1 and 1.
        collidePoint = collidePoint / (this->leftPlayer[0].h / 2);
        // When the ball hits the top of the player we want the ball, to take a 60degees angle
        // When the ball hits the center of the player we want the ball to take a 0degrees angle
        // When the ball hits the bottom of the player we want the ball to take a -60degrees
        // Math.PI/3 = 60degrees
        double angleRad = collidePoint * (this->pi / 3);
        this->veloX = this->ballSpeedCurr * cos(angleRad);
        this->veloY = this->ballSpeedCurr * -sin(angleRad);
        // Speed up the ball everytime the player hits it.
        this->ballSpeedCurr += 0.2;
        this->hitP1[0] = true;
        Mix_PlayChannel(-1, this->sound, 0);
    }
    if (SDL_HasIntersection(&this->ball, &this->leftPlayer[1]))
    {
        // Check where the ball hits the player
        double collidePoint = (this->leftPlayer[1].y + (this->leftPlayer[1].h / 2)) - (this->ball.y + (this->ballSize / 2));
        // Normalize the value of collidePoint, need to get numbers between -1 and 1.
        collidePoint = collidePoint / (this->leftPlayer[1].h / 2);
        // When the ball hits the top of the player we want the ball, to take a 60degees angle
        // When the ball hits the center of the player we want the ball to take a 0degrees angle
        // When the ball hits the bottom of the player we want the ball to take a -60degrees
        // Math.PI/3 = 60degrees
        double angleRad = collidePoint * (this->pi / 3);
        this->veloX = this->ballSpeedCurr * cos(angleRad);
        this->veloY = this->ballSpeedCurr * -sin(angleRad);
        // Speed up the ball everytime the player hits it.
        this->ballSpeedCurr += 0.2;
        this->hitP1[1] = true;
        Mix_PlayChannel(-1, this->sound, 0);
    }
    if (SDL_HasIntersection(&this->ball, &this->rightPlayer[0]))
    {
        // Check where the ball hits the player
        double collidePoint = (this->rightPlayer[0].y + (this->rightPlayer[0].h / 2)) - (this->ball.y + (this->ballSize / 2));
        // Normalize the value of collidePoint, need to get numbers between -1 and 1.
        collidePoint = collidePoint / (this->rightPlayer[0].h / 2);
        // When the ball hits the top of the player we want the ball, to take a 60degees angle
        // When the ball hits the center of the player we want the ball to take a 0degrees angle
        // When the ball hits the bottom of the player we want the ball to take a -60degrees
        // Math.PI/3 = 60degrees
        double angleRad = collidePoint * (this->pi / 3);
        this->veloX = -this->ballSpeedCurr * cos(angleRad);
        this->veloY = this->ballSpeedCurr * -sin(angleRad);
        // Speed up the ball everytime the player hits it.
        this->ballSpeedCurr += 0.2;
        this->hitP2[0] = true;
        Mix_PlayChannel(-1, this->sound, 0);
    }
    if (SDL_HasIntersection(&this->ball, &this->rightPlayer[1]))
    {
        // Check where the ball hits the player
        double collidePoint = (this->rightPlayer[1].y + (this->rightPlayer[1].h / 2)) - (this->ball.y + (this->ballSize / 2));
        // Normalize the value of collidePoint, need to get numbers between -1 and 1.
        collidePoint = collidePoint / (this->rightPlayer[1].h / 2);
        // When the ball hits the top of the player we want the ball, to take a 60degees angle
        // When the ball hits the center of the player we want the ball to take a 0degrees angle
        // When the ball hits the bottom of the player we want the ball to take a -60degrees
        // Math.PI/3 = 60degrees
        double angleRad = collidePoint * (this->pi / 3);
        this->veloX = -this->ballSpeedCurr * cos(angleRad);
        this->veloY = this->ballSpeedCurr * -sin(angleRad);
        // Speed up the ball everytime the player hits it.
        this->ballSpeedCurr += 0.2;
        this->hitP2[1] = true;
        Mix_PlayChannel(-1, this->sound, 0);
    }

    // AI Mode
    if (!this->mode && this->veloX > 0)
    {
        if (this->ball.y + this->ballSize <= this->height / 2)
        {
            if (this->ball.y > (this->rightPlayer[0].y + (3 * this->rightPlayer[0].h / 4)))
            {
                this->rightPlayer[0].y += this->speed;
                if (this->rightPlayer[0].y + this->rightPlayer[0].h >= this->height / 2)
                {
                    this->rightPlayer[0].y = this->height / 2 - this->rightPlayer[0].h;
                }
                else
                {
                    this->stepP2[0]++;
                }

                if (this->stepP2[0] >= 10)
                {
                    this->stepP2[0] = 0;
                }
            }
            else if (this->ball.y < (this->rightPlayer[0].y + (1 * this->rightPlayer[0].h / 4)))
            {
                this->rightPlayer[0].y -= this->speed;
                if (this->rightPlayer[0].y > 0)
                {
                    this->stepP2[0]--;
                }

                if (this->stepP2[0] < 0)
                {
                    this->stepP2[0] = 9;
                }
            }
        }
        else
        {
            if (this->ball.y > (this->rightPlayer[1].y + (3 * this->rightPlayer[1].h / 4)))
            {
                this->rightPlayer[1].y += this->speed;
                if (this->rightPlayer[1].y + this->rightPlayer[1].h < this->height)
                {
                    this->stepP2[1]++;
                }

                if (this->stepP2[1] >= 10)
                {
                    this->stepP2[1] = 0;
                }
            }
            else if (this->ball.y < (this->rightPlayer[1].y + (this->rightPlayer[1].h / 4)))
            {
                this->rightPlayer[1].y -= this->speed;
                if (this->rightPlayer[1].y <= this->height / 2)
                {
                    this->rightPlayer[1].y = this->height / 2;
                }
                else
                {
                    this->stepP2[1]--;
                }

                if (this->stepP2[1] < 0)
                {
                    this->stepP2[1] = 9;
                }
            }
        }
    }

    // Right player win
    if (this->ball.x < 0)
    {
        this->rightScore++;
        this->serve();
    }

    // Left player win
    if (this->ball.x + this->ballSize > this->width)
    {
        this->leftScore++;
        this->serve();
    }

    // When the ball collides with bottom and top walls, inverse the y velocity.
    if (this->ball.y < 0 || this->ball.y + this->ballSize > this->height)
        this->veloY = -this->veloY;

    this->ball.x += this->veloX;
    this->ball.y += this->veloY;
    this->score = to_string(this->leftScore) + "   " + to_string(this->rightScore);
}

void Window::rightInput()
{
    SDL_Event event;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event))
        if (event.type == SDL_QUIT)
            this->closed = true;

    if (keystates[SDL_SCANCODE_ESCAPE])
        this->closed = true;
    if (keystates[SDL_SCANCODE_L])
    {
        if (this->wRightPlayer == 0)
        {
            this->wRightPlayer = 1;
        }
        else
        {
            this->wRightPlayer = 0;
        }
    };
    if (keystates[SDL_SCANCODE_UP])
    {
        this->rightPlayer[this->wRightPlayer].y -= this->speed;
        if (this->checkPlayersCollision(this->rightPlayer[0], this->rightPlayer[1]))
        {
            this->rightPlayer[this->wRightPlayer].y += this->speed;
        }
        else
        {
            if (this->rightPlayer[this->wRightPlayer].y > 0)
            {
                this->stepP2[this->wRightPlayer]++;
            }
        }
    }
    if (keystates[SDL_SCANCODE_DOWN])
    {
        this->rightPlayer[this->wRightPlayer].y += this->speed;
        if (this->checkPlayersCollision(this->rightPlayer[0], this->rightPlayer[1]))
        {
            this->rightPlayer[this->wRightPlayer].y -= this->speed;
        }
        else
        {
            if (this->rightPlayer[this->wRightPlayer].y + this->rightPlayer[this->wRightPlayer].h < this->height)
            {
                this->stepP2[this->wRightPlayer]--;
            }
        }
    }
    if (keystates[SDL_SCANCODE_LEFT])
    {
        this->rightPlayer[this->wRightPlayer].x -= this->speed;
        if (this->checkPlayersCollision(this->rightPlayer[0], this->rightPlayer[1]))
        {
            this->rightPlayer[this->wRightPlayer].x += this->speed;
        }
        else
        {
            this->stepP2[this->wRightPlayer]++;
        }
    }
    if (keystates[SDL_SCANCODE_RIGHT])
    {
        this->rightPlayer[this->wRightPlayer].x += this->speed;
        if (this->checkPlayersCollision(this->rightPlayer[0], this->rightPlayer[1]))
        {
            this->rightPlayer[this->wRightPlayer].x -= this->speed;
        }
        else
        {
            if (this->rightPlayer[this->wRightPlayer].x + this->rightPlayer[this->wRightPlayer].w < this->width)
            {
                this->stepP2[this->wRightPlayer]--;
            }
        }
    }
}

void Window::leftInput()
{
    SDL_Event event;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event))
        if (event.type == SDL_QUIT)
            this->closed = true;

    if (keystates[SDL_SCANCODE_ESCAPE])
        this->closed = true;
    if (keystates[SDL_SCANCODE_F])
    {
        if (this->wLeftPlayer == 0)
        {
            this->wLeftPlayer = 1;
        }
        else
        {
            this->wLeftPlayer = 0;
        }
    }
    if (keystates[SDL_SCANCODE_W])
    {
        this->leftPlayer[this->wLeftPlayer].y -= this->speed;
        if (this->checkPlayersCollision(this->leftPlayer[0], this->leftPlayer[1]))
        {
            this->leftPlayer[this->wLeftPlayer].y += this->speed;
        }
        else
        {
            if (this->leftPlayer[this->wLeftPlayer].y > 0)
            {
                this->stepP1[this->wLeftPlayer]++;
            }
        }
    }
    if (keystates[SDL_SCANCODE_S])
    {
        this->leftPlayer[this->wLeftPlayer].y += this->speed;
        if (this->checkPlayersCollision(this->leftPlayer[0], this->leftPlayer[1]))
        {
            this->leftPlayer[this->wLeftPlayer].y -= this->speed;
        }
        else
        {
            if (this->leftPlayer[this->wLeftPlayer].y + this->leftPlayer[this->wLeftPlayer].h < this->height)
            {
                this->stepP1[this->wLeftPlayer]--;
            }
        }
    }
    if (keystates[SDL_SCANCODE_A])
    {
        this->leftPlayer[this->wLeftPlayer].x -= this->speed;
        if (this->checkPlayersCollision(this->leftPlayer[0], this->leftPlayer[1]))
        {
            this->leftPlayer[this->wLeftPlayer].x += this->speed;
        }
        else
        {
            if (this->leftPlayer[this->wLeftPlayer].x > 0)
            {
                this->stepP1[this->wLeftPlayer]--;
            }
        }
    }
    if (keystates[SDL_SCANCODE_D])
    {
        this->leftPlayer[this->wLeftPlayer].x += this->speed;
        if (this->checkPlayersCollision(this->leftPlayer[0], this->leftPlayer[1]))
        {
            this->leftPlayer[this->wLeftPlayer].x -= this->speed;
        }
        else
        {
            this->stepP1[this->wLeftPlayer]++;
        }
    }
}

void Window::reRender()
{
    // Reset coordinates
    if (this->leftPlayer[0].y < 0)
        this->leftPlayer[0].y = 0;
    if (this->leftPlayer[0].x < 0)
        this->leftPlayer[0].x = 0;
    if (this->leftPlayer[0].y + this->leftPlayer[0].h > this->height)
        this->leftPlayer[0].y = this->height - this->leftPlayer[0].h;

    if (this->leftPlayer[1].y < 0)
        this->leftPlayer[1].y = 0;
    if (this->leftPlayer[1].x < 0)
        this->leftPlayer[1].x = 0;
    if (this->leftPlayer[1].y + this->leftPlayer[1].h > this->height)
        this->leftPlayer[1].y = this->height - this->leftPlayer[1].h;

    if (this->rightPlayer[0].y < 0)
        this->rightPlayer[0].y = 0;
    if (this->rightPlayer[0].x + this->rightPlayer[0].w >= this->width)
        this->rightPlayer[0].x = this->width - this->rightPlayer[0].w;
    if (this->rightPlayer[0].y + this->rightPlayer[0].h > this->height)
        this->rightPlayer[0].y = this->height - this->rightPlayer[0].h;

    if (this->rightPlayer[1].y < 0)
        this->rightPlayer[1].y = 0;
    if (this->rightPlayer[1].x + this->rightPlayer[1].w >= this->width)
        this->rightPlayer[1].x = this->width - this->rightPlayer[1].w;
    if (this->rightPlayer[1].y + this->rightPlayer[1].h > this->height)
        this->rightPlayer[1].y = this->height - this->rightPlayer[1].h;

    // Check step
    if (this->stepP1[0] >= 10)
    {
        this->stepP1[0] = 0;
    }
    else if (this->stepP1[0] < 0)
    {
        this->stepP1[0] = 9;
    }

    if (this->stepP1[1] >= 10)
    {
        this->stepP1[1] = 0;
    }
    else if (this->stepP1[1] < 0)
    {
        this->stepP1[1] = 9;
    }

    if (this->stepP2[0] >= 10)
    {
        this->stepP2[0] = 0;
    }
    else if (this->stepP2[0] < 0)
    {
        this->stepP2[0] = 9;
    }

    if (this->stepP2[1] >= 10)
    {
        this->stepP2[1] = 0;
    }
    else if (this->stepP2[1] < 0)
    {
        this->stepP2[1] = 9;
    }

    if (this->hitP1[0])
    {
        SDL_Surface *imagePlayerShoot = IMG_Load("./images/Run6P1_.png");
        SDL_Surface *imagePlayer_1_2 = IMG_Load(this->runningP1[this->stepP1[1]]);
        this->textureP1_1 = SDL_CreateTextureFromSurface(this->renderer, imagePlayerShoot);
        this->textureP1_2 = SDL_CreateTextureFromSurface(this->renderer, imagePlayer_1_2);
        SDL_FreeSurface(imagePlayerShoot);
        SDL_FreeSurface(imagePlayer_1_2);
        this->hitP1[0] = !this->hitP1[0];
    }
    else if (this->hitP1[1])
    {
        SDL_Surface *imagePlayerShoot = IMG_Load("./images/Run6P1_.png");
        SDL_Surface *imagePlayer_1_1 = IMG_Load(this->runningP1[this->stepP1[0]]);
        this->textureP1_2 = SDL_CreateTextureFromSurface(this->renderer, imagePlayerShoot);
        this->textureP1_1 = SDL_CreateTextureFromSurface(this->renderer, imagePlayer_1_1);
        SDL_FreeSurface(imagePlayerShoot);
        SDL_FreeSurface(imagePlayer_1_1);
        this->hitP1[1] = !this->hitP1[1];
    }
    else
    {
        SDL_Surface *imagePlayer_1_1 = IMG_Load(this->runningP1[this->stepP1[0]]);
        SDL_Surface *imagePlayer_1_2 = IMG_Load(this->runningP1[this->stepP1[1]]);
        this->textureP1_1 = SDL_CreateTextureFromSurface(this->renderer, imagePlayer_1_1);
        this->textureP1_2 = SDL_CreateTextureFromSurface(this->renderer, imagePlayer_1_2);
        SDL_FreeSurface(imagePlayer_1_1);
        SDL_FreeSurface(imagePlayer_1_2);
    }

    if (this->hitP2[0])
    {
        SDL_Surface *imagePlayerShoot = IMG_Load("./images/Run1P2_.png");
        SDL_Surface *imagePlayer_2_2 = IMG_Load(this->runningP2[this->stepP2[1]]);
        this->textureP2_1 = SDL_CreateTextureFromSurface(this->renderer, imagePlayerShoot);
        this->textureP2_2 = SDL_CreateTextureFromSurface(this->renderer, imagePlayer_2_2);
        SDL_FreeSurface(imagePlayerShoot);
        SDL_FreeSurface(imagePlayer_2_2);
        this->hitP2[0] = !this->hitP2[0];
    }
    else if (this->hitP2[1])
    {
        SDL_Surface *imagePlayerShoot = IMG_Load("./images/Run1P2_.png");
        SDL_Surface *imagePlayer_2_1 = IMG_Load(this->runningP2[this->stepP2[0]]);
        this->textureP2_2 = SDL_CreateTextureFromSurface(this->renderer, imagePlayerShoot);
        this->textureP2_1 = SDL_CreateTextureFromSurface(this->renderer, imagePlayer_2_1);
        SDL_FreeSurface(imagePlayer_2_1);
        SDL_FreeSurface(imagePlayerShoot);
        this->hitP2[1] = !this->hitP2[1];
    }
    else
    {
        SDL_Surface *imagePlayer_2_1 = IMG_Load(this->runningP2[this->stepP2[0]]);
        SDL_Surface *imagePlayer_2_2 = IMG_Load(this->runningP2[this->stepP2[1]]);
        this->textureP2_1 = SDL_CreateTextureFromSurface(this->renderer, imagePlayer_2_1);
        this->textureP2_2 = SDL_CreateTextureFromSurface(this->renderer, imagePlayer_2_2);
        SDL_FreeSurface(imagePlayer_2_1);
        SDL_FreeSurface(imagePlayer_2_2);
    }

    SDL_Surface *imageBackground = IMG_Load("./images/Background_.png");
    this->textureBg = SDL_CreateTextureFromSurface(this->renderer, imageBackground);
    SDL_FreeSurface(imageBackground);

    if (this->started)
    {
        this->scoreCounter(this->score, this->width / 2 + 50, this->fontSize * 2);

        if (this->lastTime != 0)
        {
            this->timeCounter(this->time, this->width / 2 + 20, this->height);
        }
    }
    else if (!this->started)
    {
        this->scoreCounter("Press SpaceBar to start", this->width / 2 + 260, this->fontSize * 2);
        this->timeCounter("15", this->width / 2 + 20, this->height);
    }

    SDL_RenderCopy(this->renderer, this->textureBg, nullptr, nullptr);
    SDL_RenderCopy(this->renderer, this->textureP1_1, nullptr, &this->leftPlayer[0]);
    SDL_RenderCopy(this->renderer, this->textureP1_2, nullptr, &this->leftPlayer[1]);
    SDL_RenderCopy(this->renderer, this->textureP2_1, nullptr, &this->rightPlayer[0]);
    SDL_RenderCopy(this->renderer, this->textureP2_2, nullptr, &this->rightPlayer[1]);
    SDL_RenderCopy(this->renderer, this->textureBall, nullptr, &this->ball);
    SDL_RenderCopy(this->renderer, this->textureScore, nullptr, &this->score_board);
    SDL_RenderCopy(this->renderer, this->textureTime, nullptr, &this->time_board);

    // Destroy texture
    SDL_DestroyTexture(this->textureBg);
    SDL_DestroyTexture(this->textureP1_1);
    SDL_DestroyTexture(this->textureP1_2);
    SDL_DestroyTexture(this->textureP2_1);
    SDL_DestroyTexture(this->textureP2_2);
    SDL_DestroyTexture(this->textureScore);
    SDL_DestroyTexture(this->textureTime);

    SDL_RenderPresent(this->renderer);
}

void Window::renderBackground()
{
    SDL_RenderCopy(this->renderer, this->textureBg, NULL, NULL);
    SDL_DestroyTexture(this->textureBg);

    SDL_RenderPresent(this->renderer);
}

void Window::renderOverTime()
{

    if (this->leftScore > this->rightScore)
    {
        
    }
    else if (this->leftScore < this->rightScore)
    {
        
    }
    else
    {
        
    }

    SDL_Surface *imageBackground = IMG_Load("./images/Background.png");

    this->textureBg = SDL_CreateTextureFromSurface(this->renderer, imageBackground);

    this->scoreCounter(this->score, this->width / 2 + 50, this->fontSize * 2);
    this->timeCounter("Press Backspace to return home!", this->width / 2 + 360, this->height - this->fontSize);

    SDL_FreeSurface(imageBackground);

    SDL_RenderCopy(this->renderer, this->textureBg, nullptr, nullptr);
    SDL_RenderCopy(this->renderer, this->textureP1_1, nullptr, &this->leftPlayer[0]);
    SDL_RenderCopy(this->renderer, this->textureP1_2, nullptr, &this->leftPlayer[1]);
    SDL_RenderCopy(this->renderer, this->textureP2_1, nullptr, &this->rightPlayer[0]);
    SDL_RenderCopy(this->renderer, this->textureP2_2, nullptr, &this->rightPlayer[1]);
    SDL_RenderCopy(this->renderer, this->textureScore, nullptr, &this->score_board);
    SDL_RenderCopy(this->renderer, this->textureTime, nullptr, &this->time_board);

    // Destroy texture
    SDL_DestroyTexture(this->textureBg);
    SDL_DestroyTexture(this->textureP1_1);
    SDL_DestroyTexture(this->textureP1_2);
    SDL_DestroyTexture(this->textureP2_1);
    SDL_DestroyTexture(this->textureP2_2);
    SDL_DestroyTexture(this->textureScore);
    SDL_DestroyTexture(this->textureTime);

    SDL_RenderPresent(this->renderer);
}



bool Window::checkPlayersCollision(SDL_Rect P1, SDL_Rect P2)
{
    if (SDL_HasIntersection(&P1, &P2))
        return 1;
    return 0;
}

void Window::resetInitialState()
{
    this->leftScore = this->rightScore = 0;
    this->score = to_string(this->leftScore) + "   " + to_string(this->rightScore);

    this->lastTime = 15;
    this->time = "15";

    if (!this->rePlay)
    {
        this->mTimer = Timer::Instance();
    }
    else
    {
        this->mTimer->Reset();
    }

    this->mode = 0;

    this->leftPlayer[0].x = 32;
    this->leftPlayer[0].h = this->height / 4;
    this->leftPlayer[0].y = (this->height / 4) - (this->leftPlayer[0].h / 2);
    this->leftPlayer[0].w = 130;
    this->leftPlayer[1] = this->leftPlayer[0];
    this->leftPlayer[1].y = this->height - this->leftPlayer[1].h;

    this->rightPlayer[0] = this->leftPlayer[this->wLeftPlayer];
    this->rightPlayer[0].x = this->width - this->rightPlayer[0].w - 32;
    this->rightPlayer[1] = this->rightPlayer[0];
    this->rightPlayer[1].y = this->leftPlayer[1].y;

    this->ball.w = this->ball.h = this->ballSize;

    this->serve();
};

/////////////////////////////////////////////////// Timer Class ///////////////////////////////////////////////////

Timer *Timer::sInstance = NULL;

Timer *Timer::Instance()
{
    if (sInstance == NULL)
    {
        sInstance = new Timer();
    };

    return sInstance;
};

void Timer::Release()
{
    delete sInstance;
    sInstance = NULL;
};

Timer::Timer()
{
    Reset();
    mTimeScale = 1.0f;
};

Timer::~Timer()
{
    this->Reset();
    this->Release();
};

void Timer::Reset()
{
    mStartTicks = SDL_GetTicks();
    mElapsedTicks = 0;
    mDeltaTime = 0;
};

float Timer::DeltaTime()
{
    return mDeltaTime;
};

void Timer::TimeScale(float t)
{
    mTimeScale = t;
};

float Timer::TimeScale()
{
    return mTimeScale;
};

void Timer::Update()
{
    mElapsedTicks = SDL_GetTicks() - mStartTicks;
    mDeltaTime = mElapsedTicks * 0.001f;
};