// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"

// Vector2 struct just stores x/y coordinates
// (for now)
struct Vector2
{
	float x;
	float y;
};

struct Ball
{
    Vector2 position;
    Vector2 velocity;
    bool onScreen = true;
};

struct Paddle
{
    int direction;
    Vector2 position;
    bool left;
};

// Game class
class Game
{
public:
	Game();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
    void UpdatePaddle(Vector2 &paddlePos, int paddleDir, float deltaTime);
    Ball InitializeBall(float posX, float posY, float velX, float velY);
    bool HasCollision(Ball ball, Paddle paddle);
    void UpdateBall(Ball &ball, float deltaTime);

	// Window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;
	
	// Pong specific
    
    // Left Paddle
    Paddle mLeftPaddle;
    
    // Right Paddle
    Paddle mRightPaddle;

    // the ball
    Ball mBall1;
    Ball mBall2;
};
