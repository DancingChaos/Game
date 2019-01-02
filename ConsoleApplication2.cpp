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
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player {
private: 
float  by = 0;
public:
	float Width, Heigth, CurrentPX, CurrentPY, PlayerX, PlayerY, Speed;
	int Direction, playerScore, health;
	bool life;
	String File;
	Image image;
	Texture texture;
	Sprite sprite;
	Player(String F, float X, float Y, float W, float H) {
		Direction = 0; Speed = 0; playerScore = 0; health = 100; CurrentPX = 0; CurrentPY = 0;
		life = true;
		File = F;
		Width = W; Heigth = H;
		image.loadFromFile("img/hero1.png");
		image.createMaskFromColor(Color(41, 33, 59));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		PlayerX = X; PlayerY = Y;
		sprite.setTextureRect(IntRect(0, 0, Width, Heigth));
		sprite.setOrigin(Width / 2, Heigth / 2);
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
		sprite.setPosition(PlayerX, PlayerY);
		interactionWithMap(time);
		if (health <= 0) { life = false; }

	}

	float getplayercoordinateX() {
		return PlayerX;
	}
	float getplayercoordinateY() {
		return PlayerY;
	}
	void interactionWithMap(int time)
	{
		for (int i = PlayerY / 32; i < (PlayerY + Heigth) / 32; i++)
			for (int j = PlayerX / 32; j<(PlayerX + Width) / 32; j++)
			{
				if (TileMap[i][j] == '0')
				{

					if (CurrentPY>0)
					{
						PlayerY = i * 32 - Heigth;
					}
					if (CurrentPY<0)
					{
						PlayerY = i * 32 + 32;
					}
					if (CurrentPX>0)
					{
						PlayerX = j * 32 - Width;
					}
					if (CurrentPX < 0)
					{
						PlayerX = j * 32 +32 ;
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
					if (health + 20 < 100) {
						health += 20;
						TileMap[i][j] = ' ';
					}
				}

			}
	}

};

int main()
{


	RenderWindow window(VideoMode(1366, 768), "Game");
	view.reset(FloatRect(0, 0, 640, 480));
	
	SoundBuffer buffer;
	buffer.loadFromFile("Sound/Punch.flac");
	Sound sound;
	sound.setBuffer(buffer);
	Font font;
	font.loadFromFile("fonts/CyrillicOldBold.ttf");
	Text text("", font, 20);
	Image map_image;
	map_image.loadFromFile("img/map1.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);

	Image quest_image;
	quest_image.loadFromFile("img/missionbg.jpg");
	quest_image.createMaskFromColor(Color(0, 0, 0));
	Texture quest_texture;
	quest_texture.loadFromImage(quest_image);
	Sprite s_quest;
	s_quest.setTexture(quest_texture);
	s_quest.setTextureRect(IntRect(0, 0, 340, 510));
	s_quest.setScale(0.6f, 0.6f);

	Player p("hero1.png", 250, 250, 96.0, 96.0);

	bool showMissionText = true;

	float currentFrame = 0;
	Clock clock;
	Clock gameTimeClock;
	int gameTime = 0;
	int createObjectForMapTimer = 0;//timer for random
	while (window.isOpen())
	{

		float time = clock.getElapsedTime().asMicroseconds();

		if (p.life) gameTime = gameTimeClock.getElapsedTime().asSeconds(); else { view.move(0.8, 0); }
		clock.restart();
		time = time / 800;


		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
				

			if (event.type == Event::KeyPressed)
				if ((event.key.code == Keyboard::Tab)) {


					switch (showMissionText) {

					case true: {
						std::ostringstream playerHealthString;
						playerHealthString << p.health;
						std::ostringstream task;
						task << getTextMission(getCurrentMission(p.getplayercoordinateX()));
						text.setFillColor(Color::Red);
						text.setString("\n Здоровье: " + playerHealthString.str() + "\n" + task.str());
						showMissionText = false;
						break;
					}
					case false: {
						text.setString("");
						showMissionText = true;
						break;
					}
					}
				}
		}

		///////////////////////////////////////////Управление персонажем с анимацией////////////////////////////////////////////////////////////////////////
		if (p.life) {
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				p.Direction = 1; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 110, 96, 96));
			}

			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				p.Direction = 0; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 210, 96, 96));
			}

			if (Keyboard::isKeyPressed(Keyboard::Up)) {
				p.Direction = 3; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 310, 96, 96));
			}
			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				p.Direction = 2; p.Speed = 0.1;
				currentFrame += 0.005*time;
				if (currentFrame > 3) currentFrame -= 3;
				p.sprite.setTextureRect(IntRect(95 * int(currentFrame), 10, 96, 96));
			}
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				sound.play();
			}
			getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
		}



		sf::Vector2i localPosition = Mouse::getPosition(window);



		if (localPosition.x < 3) { view.move(-0.1*time, 0); }
		if (localPosition.x > window.getSize().x - 3) { view.move(0.1*time, 0); }
		if (localPosition.y > window.getSize().y - 3) { view.move(0, 0.1*time); }
		if (localPosition.y < 3) { view.move(0, -0.1*time); }

		createObjectForMapTimer += time;//наращиваем таймер
		if (createObjectForMapTimer>3000) {
			randomMapGenerate();//генерация случ камней
			createObjectForMapTimer = 0;//обнуляем таймер
		}

		p.update(time);

		window.setView(view);
		window.clear();



		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
				if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}


		if (!showMissionText) {
			text.setPosition(view.getCenter().x + 125, view.getCenter().y - 130);
			s_quest.setPosition(view.getCenter().x + 115, view.getCenter().y - 130);
			window.draw(s_quest); window.draw(text);
		}

		window.draw(p.sprite);


		window.display();
	}

	return 0;
}