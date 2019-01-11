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
	//Init Variables
	float Width, Heigth, CurrentPX, CurrentPY,
		PlayerX, PlayerY, Speed, health, mana, stamina;
	int Direction, playerScore, BDir;
	int mouseX, mouseY;
	const int Block = 32; const int BarMax = 100;
	//Init Variables
	bool life;
	String File;
	Image PlayerIm;
	Texture PlayerTex;
	Sprite PlayerSpr;
	/////////////
	Player(String F, float X, float Y, float W, float H) {
		//Variables
		Direction = 0; Speed = 0; playerScore = 0;
		health = 100; CurrentPX = 0; CurrentPY = 0;
		mana = 100; stamina = 100; life = true;
		File = F; Width = W; Heigth = H;
		//Set Texture
		PlayerIm.loadFromFile("img/hero.png");
		PlayerTex.loadFromImage(PlayerIm);
		PlayerSpr.setTexture(PlayerTex);
		//Set Player Position
		PlayerX = X;
		PlayerY = Y;
		PlayerSpr.setTextureRect(IntRect(0, 0, Width, Heigth));
		PlayerSpr.setOrigin(Width / 5, Heigth / 5);
	}

	void update(float time)
	{
		//Player Direction
			switch (Direction)
			{
			case 0: CurrentPX = Speed; CurrentPY = 0; break;
			case 1: CurrentPX = -Speed; CurrentPY = 0; break;
			case 2: CurrentPX = 0; CurrentPY = Speed; break;
			case 3: CurrentPX = 0; CurrentPY = -Speed; break;
			}
		//Player Speed
		PlayerX += CurrentPX * time;
		PlayerY += CurrentPY * time;
		Speed = 0;
		PlayerSpr.setPosition(PlayerX, PlayerY);
		//Collision
		interactionWithMap(time);
		//Life Check
		if (health <= 0) { life = false; }
		//ManaRegen
		if (mana + 0.0005*time <= BarMax) {
			mana += 0.0005*time;
		}
		else {
			mana = 100;
		}
		//StaminaRegen
		if (stamina + 0.0020*time <= BarMax) {
			stamina += 0.0020*time;
		}
		else {
			stamina = 100;
		}
		//HPRegen
		if (health + 0.0005*time <= BarMax) {
			health += 0.0005*time;
		}
		else {
			health = 100;
		}
	}
	//X Player
	float getplayercoordinateX() {
		return PlayerX;
	}
	//Y Player
	float getplayercoordinateY() {
		return PlayerY;
	}
	//Collision
	bool circleTest(const Sprite &PlayerSpr, const sf::Sprite &s_map )
	{
		sf::Vector2f firstRect(PlayerSpr.getTextureRect().width, PlayerSpr.getTextureRect().height);
		firstRect.x *= PlayerSpr.getScale().x;
		firstRect.y *= PlayerSpr.getScale().y;

		sf::Vector2f secondRect(s_map.getTextureRect().width, s_map.getTextureRect().height);
		secondRect.x *= s_map.getScale().x;
		secondRect.y *= s_map.getScale().y;

		float r1 = (firstRect.x + firstRect.y) / 4;
		float r2 = (secondRect.x + secondRect.y) / 4;
		float xd = PlayerSpr.getPosition().x - s_map.getPosition().x;
		float yd = PlayerSpr.getPosition().y - s_map.getPosition().y;

		return std::sqrt(xd * xd + yd * yd) <= r1 + r2;
	}
	void interactionWithMap(int time)
	{
		for (int i = PlayerY / Block; i < (PlayerY + Heigth / 8) / Block; i++)
			for (int j = PlayerX / Block; j < (PlayerX + Width / 8) / Block; j++)
			{
				if (TileMap[i][j] == '0' || TileMap[i][j] == 'w')
				{
					if (CurrentPY > 0)
					{
						Speed = 0;
						PlayerY = i * Block - Heigth / 8;
					}
					if (CurrentPY < 0)
					{
						Speed = 0;
						PlayerY = i * Block + Block;
					}
					if (CurrentPX > 0)
					{
						Speed = 0;
						PlayerX = j * Block - Width / 8;
					}
					if (CurrentPX < 0)
					{
						Speed = 0;
						PlayerX = j * Block + Block;
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
/*class Object {
public:
	int HP, BoxX, BoxY, Width, Heigth;
	bool life;
	Image BoxI;
	Texture BoxT;
	Sprite BoxS;
	String File;
	Object(String Box, int X, int Y, int W, int H) {
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
*/
int main()
{

	//Init Variables
	const int TSizeBig = 20;
	const int TSizeSmall = 15;
	//Window
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
	Text BarsH("", font, TSizeBig);
	Text BarsM("", font, TSizeBig);
	Text BarsS("", font, TSizeBig);
	Text Quest("", font, TSizeSmall);
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
	Player p("hero.png", 205, 1030, 96.0, 96.0);
//	Object Box("box.png", 200, 1200, 96, 96);
//	Box.BoxS.setPosition(200, 950);
	//Variables init
	bool showMissionText = true;
	float currentFrame = 0;
	int Resolution = 0;
	//Time
	Clock clock;
	Clock gameTimeClock;
	int gameTime = 0;
	int createObjectForMapTimer = 0;
	//
	float dX = 0;
	float dY = 0;
	//Window loop
	while (window.isOpen())
	{
		Vector2i pixelPos = Mouse::getPosition(window);//забираем коорд курсора
		Vector2f pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)
		//Text init variables
		ostringstream playerHealthString;
		ostringstream playerManaString;
		ostringstream playerStaminaString;
		ostringstream task;
		ostringstream score;
		playerHealthString << (int)p.health;
		playerManaString << (int)p.mana;
		playerStaminaString << (int)p.stamina;
		////////HP
		BarsH.setFillColor(Color::Red);
		BarsH.setString("Здоровье:" + playerHealthString.str());
		BarsH.setPosition(view.getCenter().x - 300, view.getCenter().y - 240);
		///////Mana
		BarsM.setFillColor(Color::Blue);
		BarsM.setString("Мана:" + playerManaString.str());
		BarsM.setPosition(view.getCenter().x - 300, view.getCenter().y - 220);
		///////Stamina
		BarsS.setFillColor(Color::Green);
		BarsS.setString("Выносливость:" + playerStaminaString.str());
		BarsS.setPosition(view.getCenter().x - 300, view.getCenter().y - 200);
		///////Time
		float time = clock.getElapsedTime().asMicroseconds();
		if (p.life) { gameTime = gameTimeClock.getElapsedTime().asSeconds(); }
		else { view.move(0.8, 0); }
		clock.restart();
		time = time / 800;
		///////Event Loop
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
		float dX = pos.x - p.PlayerX;//вектор , колинеарный прямой, которая пересекает спрайт и курсор
		float dY = pos.y - p.PlayerY;//он же, координата y
		float rotation = (atan2(dY, dX)) * 180 / 3.14159265;//получаем угол в радианах и переводим его в градусы
		p.PlayerSpr.setRotation(rotation);//поворачиваем спрайт на эти градусы			
		///////////////////////////////////////////Hero Control////////////////////////////////////////////////////////////////////////
		if (p.life) {
			if (Keyboard::isKeyPressed(Keyboard::A)) {
				p.Direction = 1; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
			//	p.PlayerSpr.setTextureRect(IntRect(96 * int(currentFrame), 110, 96, 96));
			}
			if (Keyboard::isKeyPressed(Keyboard::D)) {
				p.Direction = 0; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
			//	p.PlayerSpr.setTextureRect(IntRect(96 * int(currentFrame), 210, 96, 96));
				cout << "X: " << p.getplayercoordinateX() << "Y:" << p.getplayercoordinateY() << endl;
			}
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				p.Direction = 3; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				//p.PlayerSpr.setTextureRect(IntRect(96 * int(currentFrame), 310, 96, 96));
			}
			if (Keyboard::isKeyPressed(Keyboard::S)) {
				p.Direction = 2; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
			//	p.PlayerSpr.setTextureRect(IntRect(95 * int(currentFrame), 10, 96, 96));
			}
			if (Mouse::isButtonPressed(Mouse::Left)) {
				if (p.stamina - 1 < 1) {
				}
				else {
					sound.play();
					p.stamina--;
				}
			}
			if (Mouse::isButtonPressed(Mouse::Right)) {
				if (p.mana - 1 < 1) {
				}
				else {
					sound.play();
					p.mana--;
				}

			}
			///////////////////
			getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
			/*
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
			*/
		}
		//////Random Generate
		createObjectForMapTimer += time;//наращиваем таймер
		if (createObjectForMapTimer > 3000) {
			randomMapGenerate();//
			createObjectForMapTimer = 0;//обнуляем таймер
		}
		//Updating 
		p.update(time);
		window.setView(view);
		window.clear();
		//Map objects
		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((TileMap[i][j] == 'w')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
				if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
				if ((TileMap[i][j] == 'g'))s_map.setTextureRect(IntRect(192, 0, 31.9, 32));
				if ((TileMap[i][j] == 'o'))s_map.setTextureRect(IntRect(160, 0, 31.9, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(224, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}
		//Mission Check
		if (!showMissionText) {
			Quest.setPosition(view.getCenter().x + 120, view.getCenter().y - 130);
			s_quest.setPosition(view.getCenter().x + 115,
				view.getCenter().y - 130);
			window.draw(s_quest);   window.draw(Quest);
		}
		//Drawing Textures and objects
		window.draw(BarsS);
		window.draw(BarsH);
		window.draw(BarsM);
		window.draw(p.PlayerSpr);
//		window.draw(Box.BoxS);
		window.display();
	}

	return 0;
}