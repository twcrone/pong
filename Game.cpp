// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mLeftPaddleDir(0)
,mRightPaddleDir(0)
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
	mLeftPaddlePos.x = 10.0f;
	mLeftPaddlePos.y = 768.0f/2.0f;
    mRightPaddlePos.x = 1000.0f;
    mRightPaddlePos.y = 768.0f/2.0f;
    mBall.position.x = 1024.0f/2.0f;
	mBall.position.y = 768.0f/2.0f;
	mBall.velocity.x = -200.0f;
	mBall.velocity.y = 235.0f;
	return true;
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
	mLeftPaddleDir = 0;
    mRightPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mLeftPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mLeftPaddleDir += 1;
	}
    if (state[SDL_SCANCODE_I])
    {
        mRightPaddleDir -= 1;
    }
    if (state[SDL_SCANCODE_K])
    {
        mRightPaddleDir += 1;
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
	if (mLeftPaddleDir != 0)
	{
        UpdatePaddle(mLeftPaddlePos, mLeftPaddleDir, deltaTime);
	}
    if (mRightPaddleDir != 0)
    {
        UpdatePaddle(mRightPaddlePos, mRightPaddleDir, deltaTime);
    }

	// Update ball position based on ball velocity
	mBall.position.x += mBall.velocity.x * deltaTime;
	mBall.position.y += mBall.velocity.y * deltaTime;
	
	// Bounce if needed
	// Did we intersect with the paddle?
	float diff = mLeftPaddlePos.y - mBall.position.y;
	// Take absolute value of difference
	diff = (diff > 0.0f) ? diff : -diff;
    float rdiff = mRightPaddlePos.y - mBall.position.y;
    // Take absolute value of difference
    rdiff = (rdiff > 0.0f) ? rdiff : -rdiff;
	if (
		// Our y-difference is small enough
		diff <= paddleH / 2.0f &&
		// We are in the correct x-position
		mBall.position.x <= 25.0f && mBall.position.x >= 20.0f &&
		// The ball is moving to the left
		mBall.velocity.x < 0.0f)
	{
		mBall.velocity.x *= -1.0f;
	}
    if (
        // Our y-difference is small enough
        rdiff <= paddleH / 2.0f &&
        // We are in the correct x-position
        mBall.position.x >= 1000.0f && mBall.position.x <= 1024.0f &&
        // The ball is moving to the left
        mBall.velocity.x > 0.0f)
    {
        mBall.velocity.x *= -1.0f;
    }
	// Did the ball go off the screen? (if so, end game)
	else if (mBall.position.x <= 0.0f || mBall.position.x >= 1024)
	{
		mIsRunning = false;
	}
	// Did the ball collide with the right wall?
//	else if (mBall.position.x >= (1024.0f - thickness) && mBall.velocity.x > 0.0f)
//	{
//		mBall.velocity.x *= -1.0f;
//	}
	
	// Did the ball collide with the top wall?
	if (mBall.position.y <= thickness && mBall.velocity.y < 0.0f)
	{
		mBall.velocity.y *= -1;
	}
	// Did the ball collide with the bottom wall?
	else if (mBall.position.y >= (768 - thickness) &&
		mBall.velocity.y > 0.0f)
	{
		mBall.velocity.y *= -1;
	}
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
		static_cast<int>(mLeftPaddlePos.x),
		static_cast<int>(mLeftPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &leftPaddle);
    SDL_Rect rightPaddle{
        static_cast<int>(mRightPaddlePos.x),
        static_cast<int>(mRightPaddlePos.y - paddleH/2),
        thickness,
        static_cast<int>(paddleH)
    };
    SDL_RenderFillRect(mRenderer, &rightPaddle);
    
	// Draw ball
	SDL_Rect ball{	
		static_cast<int>(mBall.position.x - thickness/2),
		static_cast<int>(mBall.position.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
