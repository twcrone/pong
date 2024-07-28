// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <time.h>

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Pong 2", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
    srand(time(NULL));
	mLeftPaddle.position.x = 10.0f;
	mLeftPaddle.position.y = 768.0f/2.0f;
    mLeftPaddle.left = true;
    mRightPaddle.position.x = 1000.0f;
    mRightPaddle.position.y = 768.0f/2.0f;
    mRightPaddle.left = false;
    mBall1 = InitializeBall(1024.0f/2.0f, rand() % 768, -200.0f, rand() % 235 + 100);
    mBall2 = InitializeBall(1024.0f/2.0f, rand() % 768, 200.0f, rand() % -100 + -235);
	return true;
}

Ball Game::InitializeBall(float posX, float posY, float velX, float velY) {
    Ball b;
    b.position.x = posX;
    b.position.y = posY;
    b.velocity.x = velX;
    b.velocity.y = velY;
    return b;
}


void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys
	mLeftPaddle.direction = 0;
    mRightPaddle.direction = 0;
	if (state[SDL_SCANCODE_W])
	{
		mLeftPaddle.direction -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mLeftPaddle.direction += 1;
	}
    if (state[SDL_SCANCODE_I])
    {
        mRightPaddle.direction -= 1;
    }
    if (state[SDL_SCANCODE_K])
    {
        mRightPaddle.direction += 1;
    }
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update paddle position based on direction
	if (mLeftPaddle.direction != 0)
	{
        UpdatePaddle(mLeftPaddle.position, mLeftPaddle.direction, deltaTime);
	}
    if (mRightPaddle.direction != 0)
    {
        UpdatePaddle(mRightPaddle.position, mRightPaddle.direction, deltaTime);
    }

	// Update ball position based on ball velocity
//	mBall1.position.x += mBall1.velocity.x * deltaTime;
//	mBall1.position.y += mBall1.velocity.y * deltaTime;
    UpdateBall(mBall1, deltaTime);
    UpdateBall(mBall2, deltaTime);
	
	// Bounce if needed
	// Did we intersect with the paddle?
	// Did the ball go off the screen? (if so, end game)
	if (mBall1.position.x <= 0.0f || mBall1.position.x >= 1024)
	{
		mIsRunning = false;
	}
}

void Game::UpdateBall(Ball &ball, float deltaTime) {
    ball.position.x += ball.velocity.x * deltaTime;
    ball.position.y += ball.velocity.y * deltaTime;
    if (HasCollision(ball, mLeftPaddle) || HasCollision(ball, mRightPaddle))
    {
        ball.velocity.x *= -1.0f;
    }
    
    // Did the ball collide with the top wall?
    if (ball.position.y <= thickness && ball.velocity.y < 0.0f)
    {
        ball.velocity.y *= -1;
    }
    // Did the ball collide with the bottom wall?
    else if (ball.position.y >= (768 - thickness) &&
        ball.velocity.y > 0.0f)
    {
        ball.velocity.y *= -1;
    }
}

bool Game::HasCollision(Ball ball, Paddle paddle) {
    float diff = paddle.position.y - ball.position.y;
    // Take absolute value of difference
    diff = (diff > 0.0f) ? diff : -diff;
    float max = paddle.left ? 25.0f : 1024.0f;
    float min = paddle.left ? 20.0f : 1000.0f;
    bool ballMovingLeft = ball.velocity.x < 0.0f;
    return (
        // Our y-difference is small enough
        diff <= paddleH / 2.0f &&
        // We are in the correct x-position
        ball.position.x <= max && ball.position.x >= min &&
        // The ball is moving to the left
            ballMovingLeft == paddle.left);

}

void Game::UpdatePaddle(Vector2 &paddlePos, int paddleDir, float deltaTime)
{
    SDL_Log("Bob");
    paddlePos.y += paddleDir * 300.0f * deltaTime;
    // Make sure paddle doesn't move off screen!
    if (paddlePos.y < (paddleH/2.0f + thickness))
    {
        paddlePos.y = paddleH/2.0f + thickness;
    }
    else if (paddlePos.y > (768.0f - paddleH/2.0f - thickness))
    {
        paddlePos.y = 768.0f - paddleH/2.0f - thickness;
    }
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
//	wall.x = 1024 - thickness;
//	wall.y = 0;
//	wall.w = thickness;
//	wall.h = 1024;
//	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw paddle
	SDL_Rect leftPaddle{
		static_cast<int>(mLeftPaddle.position.x),
		static_cast<int>(mLeftPaddle.position.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &leftPaddle);
    SDL_Rect rightPaddle{
        static_cast<int>(mRightPaddle.position.x),
        static_cast<int>(mRightPaddle.position.y - paddleH/2),
        thickness,
        static_cast<int>(paddleH)
    };
    SDL_RenderFillRect(mRenderer, &rightPaddle);
    
	// Draw balls
	SDL_Rect ball1{
		static_cast<int>(mBall1.position.x - thickness/2),
		static_cast<int>(mBall1.position.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball1);

    SDL_Rect ball2{
        static_cast<int>(mBall2.position.x - thickness/2),
        static_cast<int>(mBall2.position.y - thickness/2),
        thickness,
        thickness
    };
    SDL_RenderFillRect(mRenderer, &ball2);

	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
