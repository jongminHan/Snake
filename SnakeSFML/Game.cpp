#include "Game.h"

Game::Game()
	: mDeltaTime(0.f)
	, mRadius(5.f)
	, mState(nullptr)
	, mGameMenuState(nullptr)
	, mGameOverState(nullptr)
	, mGamePlayState(nullptr)
{
	mSnake.reserve(100);
	mFood.reserve(100);
	mPoison.reserve(100);
}

Game::~Game()
{
	delete mGameMenuState;
	delete mGamePlayState;
	delete mGameOverState;

	for (auto iter = mSnake.begin(); iter != mSnake.end(); iter++)  // Destroy snake
	{
		delete *iter;
	}

	for (auto iter = mFood.begin(); iter != mFood.end(); iter++) // Destroy foods
	{
		if (*iter != nullptr)
		{
			delete *iter;
		}
	}

	for (auto iter = mPoison.begin(); iter != mPoison.end(); iter++) // Destroy poisoned foods
	{
		if (*iter != nullptr)
		{
			delete *iter;
		}
	}
}

bool Game::Init()
{
	mWindow.create(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT), "Snake", sf::Style::Titlebar | sf::Style::Close);
	mWindow.setFramerateLimit(60);

	mGameMenuState = new GameMenuState();
	mState = mGameMenuState;

	// Load the text font
	if (!mFont.loadFromFile("C:/Users/Jongmin/source/repos/SnakeSFML/SnakeSFML/resources/sansation.ttf"))
	{
		std::cout << "Failed to initialize the game\n";
		return false;
	}

	// Initialize the pause message
	mPauseMessage.setFont(mFont);
	mPauseMessage.setCharacterSize(40);
	mPauseMessage.setPosition(170.f, 150.f);
	mPauseMessage.setFillColor(sf::Color::White);
	mPauseMessage.setString("Welcome to Snake!\nPress A to start the game");

	
	return true;
}
void Game::Run()
{
	srand(static_cast<unsigned int>(time(0)));

	while (mWindow.isOpen())
	{
		sf::Event event;

		while (mWindow.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				mWindow.close();
				break;
			}

			/*
			// Press button A to start the game
			if (event.type == sf::Event::JoystickButtonPressed && sf::Joystick::isButtonPressed(0, 0))
			{
				std::cout << "Button A is pressed.\n";
				if (mState == &mGameOverState) // If the current state is Game Over, then initialize the game.
				{
					Init();
				}
				mState = &mGamePlayState;
			}
			*/
		}

		mState->Run(*this);
	}
}

void Game::MoveSnake(float x, float y)
{
	int speed = 4;
	int size = static_cast<int>(mSnake.size());

	for (int i = 0; i < size; i++)
	{
		if (i == 0) // Move head
		{
			if (mSnake[i]->getPosition().x < 0 && x <= 0)
			{
				mSnake[i]->move(0, y * mDeltaTime * speed);
			}
			else if (mSnake[i]->getPosition().x + 2 * mSnake[i]->getRadius() > GAME_WIDTH && x >= 0)
			{
				mSnake[i]->move(0, y * mDeltaTime * speed);
			}
			else if (mSnake[i]->getPosition().y < 0 && y <= 0)
			{
				mSnake[i]->move(x * mDeltaTime * speed, 0);
			}
			else if (mSnake[i]->getPosition().y + 2 * mSnake[i]->getRadius() > GAME_HEIGHT && y >= 0)
			{
				mSnake[i]->move(x * mDeltaTime * speed, 0);
			}
			else
			{
				mSnake[i]->move(x * mDeltaTime * speed, y * mDeltaTime * speed);
			}
		}
		else  // Move body
		{
			// (x - 2rcos@, y + 2rsin@)
			float x = mSnake[i - 1]->getPosition().x;
			float y = mSnake[i - 1]->getPosition().y;
			float r = mRadius;
			float cos = GetCOS(mSnake[i - 1]->getPosition(), mSnake[i]->getPosition());
			float sin = GetSIN(mSnake[i - 1]->getPosition(), mSnake[i]->getPosition());

			mSnake[i]->setPosition(x - 2 * r * cos, y + 2 * r * sin);
		}
	}
}

float Game::GetDistance(sf::Vector2f point1, sf::Vector2f point2)
{
	return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));  // Pythagorean theorem
}

float Game::GetCOS(sf::Vector2f point1, sf::Vector2f point2)
{
	return (point1.x - point2.x) / GetDistance(point1, point2);
}

float Game::GetSIN(sf::Vector2f point1, sf::Vector2f point2)
{
	return (point2.y - point1.y) / GetDistance(point1, point2);
}

void Game::DetectFoodCollision()
{
	for (size_t i = 0; i < mFood.size(); i++)
	{
		if (mFood[i] != nullptr)
		{
			if (GetDistance(mFood[i]->GetCenterPosition(), mSnake[0]->GetCenterPosition()) <= 2 * mRadius)
			{
				std::cout << "Food(" << mFood[i] << ") destroyed\n";
				delete mFood[i];
				mFood[i] = nullptr;

				Circle* body = new Circle(mRadius);
				body->setFillColor(sf::Color(0, 255, 0));
				body->setPosition(-10, -10);
				mSnake.push_back(body);
				break;
			}
		}
	}
}

void Game::DetectPoisonCollision()
{
	for (size_t i = 0; i < mPoison.size(); i++)
	{
		if (mPoison[i] != nullptr)
		{
			if (GetDistance(mPoison[i]->GetCenterPosition(), mSnake[0]->GetCenterPosition()) <= 2 * mRadius)
			{
				std::cout << "Poisoned food(" << mPoison[i] << ") destroyed\n";
				delete mPoison[i];
				mPoison[i] = nullptr;

				mState = mGameOverState;
				break;
			}
		}
	}
}