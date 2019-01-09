#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include "mission.h"

using namespace sf;
using namespace std;
//////////////////Player Class////////////////////////
class Player {
public:
	float Width, Heigth, CurrentPX, CurrentPY, PlayerX, PlayerY, Speed;
	float BSpeed, BallCX, BallCY, BallX, BallY;
	int Direction, playerScore, BDir;
	float health, mana;
	//////////////
	bool life;
	String File;
	Image PlayerIm;
	Texture PlayerTex;
	Sprite PlayerSpr;
	Image FBI;
	Texture FBT;
	Sprite FBS;
	/////////////
	Player(String F, float X, float Y, float W, float H) {
		Direction = 0; Speed = 0; playerScore = 0;
		health = 100; CurrentPX = 0; CurrentPY = 0;
		mana = 100;
		life = true;
		File = F;
		Width = W; Heigth = H;
		//////////
		PlayerIm.loadFromFile("img/hero1.png");
		PlayerTex.loadFromImage(PlayerIm);
		PlayerSpr.setTexture(PlayerTex);
		//////////
		PlayerX = X;
		PlayerY = Y;
		PlayerSpr.setTextureRect(IntRect(0, 0, Width, Heigth));
		PlayerSpr.setOrigin(Width / 2, Heigth / 2);
		/////////
		FBI.loadFromFile("img/fire.png");
		FBT.loadFromImage(FBI);
	}

	void update(float time)
	{
		switch (Direction)
		{
		case 0: CurrentPX = Speed; CurrentPY = 0; break;
		case 1: CurrentPX = -Speed; CurrentPY = 0; break;
		case 2: CurrentPX = 0; CurrentPY = Speed; break;
		case 3: CurrentPX = 0; CurrentPY = -Speed; break;
		}
		PlayerX += CurrentPX * time;
		PlayerY += CurrentPY * time;
		Speed = 0;
		BSpeed = 0.5;
		PlayerSpr.setPosition(PlayerX, PlayerY);
		interactionWithMap(time);
		if (health <= 0) { life = false; }
		if (mana + 1 <= 100)
			mana += 0.0005*time;
	}
	float getplayercoordinateX() {
		return PlayerX;
	}
	float getplayercoordinateY() {
		return PlayerY;
	}
	void interactionWithMap(int time)
	{
		for (int i = PlayerY / 32; i < (PlayerY + Heigth / 4) / 32; i++)
			for (int j = PlayerX / 32; j < (PlayerX + Width / 4) / 32; j++)
			{
				if (TileMap[i][j] == '0' || TileMap[i][j] == 'w')
				{
					if (CurrentPY > 0)
					{
						PlayerY = i * 32 - Heigth / 4;
					}
					if (CurrentPY < 0)
					{
						PlayerY = i * 32 + 32;
					}
					if (CurrentPX > 0)
					{
						PlayerX = j * 32 - Width / 4;
					}
					if (CurrentPX < 0)
					{
						PlayerX = j * 32 + 32;
					}
				}

				if (TileMap[i][j] == 's') {
					playerScore++;
					TileMap[i][j] = ' ';
				}
				if (TileMap[i][j] == 'f') {
					health -= 40;
					TileMap[i][j] = ' ';
				}
				if (TileMap[i][j] == 'h') {
					if (health + 20 <= 100) {
						health += 20;
						TileMap[i][j] = ' ';
					}
				}

			}
	}
};
/////////////////Entity Class/////////////////////////
class Object {
public:
	int HP, BoxX, BoxY , Width , Heigth;
	bool life;
	Image BoxI;
	Texture BoxT;
	Sprite BoxS;
	String File;
	Object(String Box, int X, int Y , int W , int H) {
		HP = 50; life = true;
		////////////////
		BoxI.loadFromFile("img/box.png");
		BoxT.loadFromImage(BoxI);
		BoxS.setTexture(BoxT);
		///////////////
		Width = W;
		Heigth = H;
		File = Box;
		BoxX = X;
		BoxY = Y;
	   BoxS.setTextureRect(IntRect(0, 0, Width, Heigth));
		BoxS.setOrigin(Width / 2, Heigth / 2);
	}


};
int main()
{
	RenderWindow window(VideoMode(1366, 768), "Game");
	view.reset(FloatRect(0, 0, 640, 480));
	//Sound
	SoundBuffer buffer;
	buffer.loadFromFile("Sound/Punch.flac");
	Sound sound;
	sound.setBuffer(buffer);
	//Font
	Font font;
	font.loadFromFile("fonts/CyrillicOldBold.ttf");
	Text BarsH("", font, 20);
	Text BarsM("", font, 20);
	Text Quest("", font, 15);
	//Map Elements
	Image map_image;
	map_image.loadFromFile("img/map1.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);
	//QuestList
	Image quest_image;
	quest_image.loadFromFile("img/missionbg.jpg");
	quest_image.createMaskFromColor(Color(0, 0, 0));
	Texture quest_texture;
	quest_texture.loadFromImage(quest_image);
	Sprite s_quest;
	s_quest.setTexture(quest_texture);
	s_quest.setTextureRect(IntRect(0, 0, 340, 510));
	s_quest.setScale(0.5f, 0.5f);
	//Player&Obj init
	Player p("hero1.png", 200, 1150, 96.0, 96.0);
	Object Box("box.png", 200, 1200, 96, 96);
	Box.BoxS.setPosition(200, 950);
	//Variables init
	bool showMissionText = true;
	float currentFrame = 0;
	int Resolution = 0;
	//Time
	Clock clock;
	Clock gameTimeClock;
	int gameTime = 0;
	int createObjectForMapTimer = 0;//timer for random
	//
	while (window.isOpen())
	{
		ostringstream playerHealthString;
		ostringstream playerManaString;
		ostringstream task;
		ostringstream score;
		playerHealthString << p.health;
		playerManaString << p.mana;
		////////HP
		BarsH.setFillColor(Color::Red);
		BarsH.setString("Здоровье:" + playerHealthString.str());
		BarsH.setPosition(view.getCenter().x - 300, view.getCenter().y - 240);
		///////Mana
		BarsM.setFillColor(Color::Blue);
		BarsM.setString("Мана:" + playerManaString.str());
		BarsM.setPosition(view.getCenter().x - 300, view.getCenter().y - 220);
		///////
		float time = clock.getElapsedTime().asMicroseconds();
		if (p.life) { gameTime = gameTimeClock.getElapsedTime().asSeconds(); }
		else { view.move(0.8, 0); }
		clock.restart();
		time = time / 800;
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed) {

				if ((event.key.code == Keyboard::Tab)) {
					switch (showMissionText) {
					case true: {
						task << getTextMission(getCurrentMission(p.getplayercoordinateX(), p.getplayercoordinateY()));
						score << p.playerScore;
						Quest.setFillColor(Color::Black);
						Quest.setString("\n Миссия : " + task.str() + "\n Очки :" + score.str());
						showMissionText = false;
						break;
					}
					case false: {
						Quest.setString("");
						showMissionText = true;
						break;
					}
					}
				}
			}
		}

		///////////////////////////////////////////Hero Control////////////////////////////////////////////////////////////////////////
		if (p.life) {
			if (Keyboard::isKeyPressed(Keyboard::A)) {
				p.Direction = 1; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				p.PlayerSpr.setTextureRect(IntRect(96 * int(currentFrame), 110, 96, 96));
			}

			if (Keyboard::isKeyPressed(Keyboard::D)) {
				p.Direction = 0; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				p.PlayerSpr.setTextureRect(IntRect(96 * int(currentFrame), 210, 96, 96));
				cout << "X: " << p.getplayercoordinateX() << "Y:" << p.getplayercoordinateY() << endl;
			}

			if (Keyboard::isKeyPressed(Keyboard::W)) {
				p.Direction = 3; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				p.PlayerSpr.setTextureRect(IntRect(96 * int(currentFrame), 310, 96, 96));
			}
			if (Keyboard::isKeyPressed(Keyboard::S)) {
				p.Direction = 2; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				p.PlayerSpr.setTextureRect(IntRect(95 * int(currentFrame), 10, 96, 96));
			}

			if (Mouse::isButtonPressed(Mouse::Left)) {
				if (p.mana - 1 < 1) {

				}
				else {

					sound.play();
					p.mana--;
				}

			}
			getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
			if (p.Speed == 0 && p.Direction == 1) {
				p.PlayerSpr.setTextureRect(IntRect(96, 110, 96, 96));
			}
			else if (p.Speed == 0 && p.Direction == 0) {
				p.PlayerSpr.setTextureRect(IntRect(96, 210, 96, 96));
			}
			else if (p.Speed == 0 && p.Direction == 3) {
				p.PlayerSpr.setTextureRect(IntRect(96, 310, 96, 96));
			}
			else if (p.Speed == 0 && p.Direction == 2) {
				p.PlayerSpr.setTextureRect(IntRect(95, 10, 96, 96));
			}
		}

		/*createObjectForMapTimer += time;//наращиваем таймер
		if (createObjectForMapTimer > 3000) {
			randomMapGenerate();//
			createObjectForMapTimer = 0;//обнуляем таймер
		}
		*/
		p.update(time);

		window.setView(view);
		window.clear();



		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((TileMap[i][j] == 'w')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
				if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
				if ((TileMap[i][j] == 'g'))s_map.setTextureRect(IntRect(160, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(224, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}


		if (!showMissionText) {
			Quest.setPosition(view.getCenter().x + 120, view.getCenter().y - 130);
			s_quest.setPosition(view.getCenter().x + 115, view.getCenter().y - 130);
			window.draw(s_quest);   window.draw(Quest);
		}
		window.draw(BarsH);
		window.draw(BarsM);
		window.draw(p.PlayerSpr);
		window.draw(p.FBS);
		window.draw(Box.BoxS);
		window.display();
	}

	return 0;
}