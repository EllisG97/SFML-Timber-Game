// HelloSFML.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>

using namespace sf;

//Function Declaration 
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

//Where is the player and the branch?
//Left or Right
enum class side {LEFT, RIGHT, NONE};
side branchPositions[NUM_BRANCHES];



int main()
{
	//create a video mode object 
	VideoMode vm(1920, 1080);

	//create and open a renderwindow 
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	//Create a texture to hold the background sprite
	Texture textureBackground; 

	//Load the graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");

	//Create the sprite
	Sprite spriteBackground;

	//attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	//Set the background to cover the screen
	spriteBackground.setPosition(0, 0);

	//Make the tree sprite
	Texture textureTree;
	Sprite spriteTree;
	textureTree.loadFromFile("graphics/tree.png");
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	//Prepare a Bee Sprite
	Texture textureBee;
	Sprite spriteBee;
	textureBee.loadFromFile("graphics/bee.png");
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);

	//Is the Bee currently Moving?
	bool beeActive = false;

	//how fast cant he bee move?
	float beeSpeed = 0.0f;

	//make 3 cloud sprites from 1 texture
	Texture textureCloud;

	//load the texture
	textureCloud.loadFromFile("graphics/cloud.png");

	//3 sprites sharing the same texture
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);

	//Position the clouds offscreen
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);

	//Are the clouds on the screen now?
	bool cloudActive1 = false;
	bool cloudActive2 = false;
	bool cloudActive3 = false;

	//How fast is each cloud?
	float cloudSpeed1 = 0.0f;
	float cloudSpeed2 = 0.0f;
	float cloudSpeed3 = 0.0f;

	//variables relating to time and it's manipulation
	Clock clock;

	//Time Bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;



	//Track to see if the game is paused or not
	bool paused = true;

	//Set the score
	int score = 0;

	//Set the text up
	sf::Text messageText;
	sf::Text scoreText;


	//Choose a font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	//Set the correct font for the message
	messageText.setFont(font);
	scoreText.setFont(font);

	//Assign an actual message
	messageText.setString("Press Enter to Start!");
	scoreText.setString("Score = 0");

	//Increase the size aptly
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	//Choose a font colour
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	//Position the Text correctly
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);

	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

	scoreText.setPosition(20, 20);

	//Prepare 6 Branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	//Set the textures for each branch
	for (int i = 0; i < NUM_BRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);

		//Set the sprites origin to dead center
		//Allowing us to spin branches without changing its position
		branches[i].setOrigin(220, 20);

	}

	// Prepare the Player Sprite
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	//The player starts on the left of the tree
	side playerSide = side::LEFT;

	//prepare the gravestone
	Texture textureGrave;
	textureGrave.loadFromFile("graphics/rip.png ");
	Sprite spriteGrave;
	spriteGrave.setTexture(textureGrave);
	spriteGrave.setPosition(600, 860);

	//Prepare the Axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	//Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	//prepare the log texture
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	//Log related variables for movement
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	//Control the player input
	bool acceptInput = false;

	//Prepare the sounds
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	//Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);




	while (window.isOpen())
	{
		/*
		****************************************
				Handle the players input
		****************************************
		*/

		Event event;

		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				//Listen for key pressed
				acceptInput = true;

				// hide the axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		//start the game
		if (Keyboard::isKeyPressed(Keyboard::Enter))
		{
			paused = false;

			//Reset the time and the score
			score = 0;
			timeRemaining = 6;

			// Make all the branches disappear 
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				branchPositions[i] = side::NONE;
			}

			//Make sure grave is hidden at the start
			spriteGrave.setPosition(675, 2000);

			//Place the player into the right position
			spritePlayer.setPosition(580, 720);

			acceptInput = true;

		}

		//Wrap the player controls
		//to allow for inputs
		if (acceptInput)
		{
			//First handle: pressing the right key ->
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				//Make sure player is on the right
				playerSide = side::RIGHT;
				score++;

				//add to the overall time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);

				spritePlayer.setPosition(1200, 720);

				//update the branches
				updateBranches(score);

				//Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				//play the chop sound
				chop.play();

			}

			//Second Handle: pressing the left key <-
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{

				//Make sure player is on the left
				playerSide = side::LEFT;
				score++;

				//add to the overall time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);

				spritePlayer.setPosition(580, 720);

				//update the banches
				updateBranches(score);

				//Set the log flying to the right
				spriteLog.setPosition(580, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				//play the chop sound
				chop.play();

			}

		}

		/*
		****************************************
					Update the scene
		****************************************
		*/
		if (!paused)
		{

			//measure time
			Time dt = clock.restart();

			//Subtract from amount of time remaining 
			timeRemaining -= dt.asSeconds();

			//size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f)
			{
				//Pause the game
				paused = true;

				//Change the message the player sees
				messageText.setString("Out of Time!!! ");

				//Reposition the text based on its size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				//play OOT sound
				outOfTime.play();

			}


			//Setup the bee's movement
			if (!beeActive)
			{
				//how fast is the bee?
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;

				//How high up is the bee?
				srand((int)time(0));
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;

			}

			else
				//move the bee

			{
				spriteBee.setPosition(
					spriteBee.getPosition().x -
					(beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y);

				//has the bee reached the right side of the screen?
				if (spriteBee.getPosition().x < -100)
				{
					//set it up to be a whole new bee sprite next frame
					beeActive = false;
				}


			}


			//Manage the cloud sprites
			//Cloud 1
			if (!cloudActive1)
			{
				//how fast is the cloud?
				srand((int)time(0) * 10);
				cloudSpeed1 = (rand() % 200);

				//How high is the cloud?
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloudActive1 = true;

			}

			else
				//move the cloud

			{
				spriteCloud1.setPosition(
					spriteCloud1.getPosition().x +
					(cloudSpeed1 * dt.asSeconds()),
					spriteCloud1.getPosition().y);

				//Has the cloud reached the side?
				if (spriteCloud1.getPosition().x > 1920)
				{
					//setup a new cloud for the next frame
					cloudActive1 = false;

				}


			}


			//Manage the cloud sprites
			//Cloud 2
			if (!cloudActive2)
			{
				//how fast is the cloud?
				srand((int)time(0) * 20);
				cloudSpeed2 = (rand() % 200);

				//How high is the cloud?
				srand((int)time(0) * 20);
				float height = (rand() % 150);
				spriteCloud2.setPosition(-200, height);
				cloudActive2 = true;

			}

			else
				//move the cloud

			{
				spriteCloud2.setPosition(
					spriteCloud2.getPosition().x +
					(cloudSpeed2 * dt.asSeconds()),
					spriteCloud2.getPosition().y);

				//Has the cloud reached the side?
				if (spriteCloud2.getPosition().x > 1920)
				{
					//setup a new cloud for the next frame
					cloudActive2 = false;

				}


			}


			//Manage the cloud sprites
			//Cloud 3
			if (!cloudActive3)
			{
				//how fast is the cloud?
				srand((int)time(0) * 30);
				cloudSpeed3 = (rand() % 200);

				//How high is the cloud?
				srand((int)time(0) * 30);
				float height = (rand() % 150);
				spriteCloud3.setPosition(-200, height);
				cloudActive3 = true;

			}

			else
				//move the cloud

			{
				spriteCloud3.setPosition(
					spriteCloud3.getPosition().x +
					(cloudSpeed3 * dt.asSeconds()),
					spriteCloud3.getPosition().y);

				//Has the cloud reached the side?
				if (spriteCloud3.getPosition().x > 1920)
				{
					//setup a new cloud for the next frame
					cloudActive3 = false;

				}


			}
			//update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			//update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;
				if (branchPositions[i] == side::LEFT)
				{
					//Move the sprite to the left
					branches[i].setPosition(610, height);

					//flip the sprite
					branches[i].setRotation(180);

				}
				else if(branchPositions[i] == side::RIGHT)
				{
					//Move the sprite to the right
					branches[i].setPosition(1330, height);

					//rotate the sprite to normal position
					branches[i].setRotation(0);
				}
				else
				{
					//Hide the branch from view
					branches[i].setPosition(3000, height);
				}
			}

			//Handle the flying log when chopping
			if (logActive)
			{
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y +
					(logSpeedY * dt.asSeconds()));

				//has the log reached the right hand side?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x > 2000)
				{
					//Set up a new log for the next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			//Has the player been crushed?
			if (branchPositions[5] == playerSide)
			{
				//DEATH
				paused = true;
				acceptInput = false;

				//Draw the gravestone
				spriteGrave.setPosition(525, 760);

				//hide the player
				spritePlayer.setPosition(2000, 660);

				//Set apt message text
				messageText.setString("You've Been Crushed!!!");

				//Center the text on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);

				//play death sound
				death.play();


			}


		}  //End if (!paused)

		/*
		****************************************
					Draw the scene
		****************************************
		*/

		//Clear everything from the last scene 
		window.clear();

		//Draw the game scene here
		window.draw(spriteBackground);

		//Draw the clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		//Draw the Branches
		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			window.draw(branches[i]);
		}

		//Draw the tree
		window.draw(spriteTree);

		//Draw the player
		window.draw(spritePlayer);

		//Draw the axe
		window.draw(spriteAxe);

		//Draw the flying log
		window.draw(spriteLog);

		//Draw the gravestone
		window.draw(spriteGrave);


		//Draw the bee
		window.draw(spriteBee);

		//Draw the score
		window.draw(scoreText);

		//Draw the time bar
		window.draw(timeBar);


		if (paused)
		{
			//Draw the pause message 
			window.draw(messageText);	
		}

		//Show everything that has been drawn
		window.display();
	}

	return 0;
}

//Function Definition
void updateBranches(int seed)
{
	//Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--)
	{
		branchPositions[j] = branchPositions[j - 1];
	}

	//Spawn a new branch at the top of the tree
	//LEFT, RIGHT or NONE
	srand((int)time(0)+seed);
	int r = (rand() % 5);
	switch (r)
	{
	case 0:
		branchPositions[0] = side::LEFT;
		break;

	case 1: 
		branchPositions[0] = side::RIGHT;
		break;

	case 2:
		branchPositions[0] = side::NONE;
		break;

	}

}
