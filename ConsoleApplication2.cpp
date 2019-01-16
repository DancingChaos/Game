#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "map.h"
#include "view.h"
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>
#include "mission.h"
#include <windows.h>
using namespace sf;
using namespace std;


//Constants
const int Block = 32;

//////////////////Point///////////////////////////////
struct point { int x; int y; int num; };


//////////////////Player Class////////////////////////
class Player {
public:
	//Init Variables
	float Width, Heigth, CurrentPX, CurrentPY,
		PlayerX, PlayerY, Speed, health, mana, stamina;
	int Direction, playerScore, BDir;
	int mouseX, mouseY;
	const int BarMax = 100;
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
		PlayerIm.loadFromFile("img/gera.png");
		PlayerIm.createMaskFromColor(Color::White);
		PlayerTex.loadFromImage(PlayerIm);
		PlayerSpr.setTexture(PlayerTex);
		//Set Player Position
		PlayerX = X;
		PlayerY = Y;
		PlayerSpr.setOrigin(Width / 4, Heigth / 4);
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
	void interactionWithMap(int time)
	{
		for (int i = PlayerY / Block; i < (PlayerY + Heigth / 4) / Block; i++)
			for (int j = PlayerX / Block; j < (PlayerX + Width / 4) / Block; j++)
			{
				if (TileMap[i][j] == '0' || TileMap[i][j] == 'w')
				{
					if (CurrentPY > 0)
					{
						Speed = 0;
						PlayerY = i * Block - Heigth / 4;
					}
					if (CurrentPY < 0)
					{
						Speed = 0;
						PlayerY = i * Block + Block;
					}
					if (CurrentPX > 0)
					{
						Speed = 0;
						PlayerX = j * Block - Width / 4;
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
class Entity {
public:
	vector<Sprite> obj;
	float dx, dy, x, y, speed, moveTimer;
	int w, h, health;
	bool life, onGround;
	Texture texture;
	Sprite sprite;
	String name;
	Entity(Image &image, String Name, float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}

	virtual void update(float time) = 0;//все потомки переопределяют эту ф-цию

};



/////////////////////Graphs/////////////////////////////
struct graph {

	graph(size_t nodes)
		: m_adjacency_list(nodes) {
	}

	size_t number_of_nodes() const {
		return m_adjacency_list.size();
	}

	std::vector<size_t> const& neighbours_of(size_t node) const {
		return m_adjacency_list.at(node);
	}

	void add_edge(size_t from, size_t to) {
		std::vector<size_t>& al = m_adjacency_list.at(from);
		if (to >= m_adjacency_list.size())
			throw std::runtime_error("Tried to add edge to non-existant node");
		for (size_t i = 0; i < al.size(); ++i) if (al[i] == to) return;
		al.push_back(to);
	}

private:
	std::vector<std::vector<size_t>> m_adjacency_list;
};
void GoToPos(Player& p, float X, float Y, float time) {
	bool isMove = true;
	p.PlayerX += (p.PlayerX - X) * time;
	p.PlayerY += (p.PlayerX - Y) * time;

	//cout << distance << endl;
}



/////////////////////Graphs/////////////////////////////
struct graph {

	graph(size_t nodes)
		: m_adjacency_list(nodes) {
	}

	size_t number_of_nodes() const {
		return m_adjacency_list.size();
	}

	std::vector<size_t> const& neighbours_of(size_t node) const {
		return m_adjacency_list.at(node);
	}

	void add_edge(size_t from, size_t to) {
		std::vector<size_t>& al = m_adjacency_list.at(from);
		if (to >= m_adjacency_list.size())
			throw std::runtime_error("Tried to add edge to non-existant node");
		for (size_t i = 0; i < al.size(); ++i) if (al[i] == to) return;
		al.push_back(to);
	}

private:
	std::vector<std::vector<size_t>> m_adjacency_list;
};
size_t searchPos(size_t startT, size_t targeT, graph GraphMap) {
	vector<size_t> reached_by(GraphMap.number_of_nodes(), GraphMap.number_of_nodes());
	queue<size_t> q;
	size_t start = startT;
	size_t target = targeT;
	reached_by[start] = start;
	q.push(start);
	while (!q.empty()) {
		size_t node = q.front();
		q.pop();
		for (size_t i = 0; i < GraphMap.neighbours_of(node).size(); ++i) {
			size_t candidate = GraphMap.neighbours_of(node)[i];
			if (reached_by[candidate] == GraphMap.number_of_nodes()) {
				reached_by[candidate] = node;
				if (candidate == target) break;
				q.push(candidate);
			}
		}
	}
	bool first_el = true;
	size_t need_num = 0;

	if (reached_by[target] == GraphMap.number_of_nodes())
		cout << "No path to " << target << " found!" << endl;
	// TODO: Сделать что-то в случае не нахождения маршрута
	else {
		//cout << "Path to " << target << ": ";
		for (size_t node = target; node != start; node = reached_by[node]) {
			//cout << node << " <- ";
			need_num = node;
		}

		return need_num;
	}

}
point searchGraphCoords(int needNum) {
	int freeNum = 0;
	for (int i = 0; i < HEIGHT_MAP; i++)
		for (int j = 0; j < WIDTH_MAP; j++)
		{
			if (TileMap[i][j] == ' ') {
				freeNum++;
				if (needNum == freeNum) {
					point coords;
					coords.x = j * Block;
					coords.y = i * Block;
					coords.num = freeNum;
					//cout << coords.x << " " << coords.y << endl;
					return coords;

				}
			}


		}

}
point getPlayerGraph(Player p) {
	int freeNum = 0;
	for (int i = 0; i < HEIGHT_MAP; i++)
		for (int j = 0; j < WIDTH_MAP; j++)
		{
			if (TileMap[i][j] == ' ') {
				freeNum++;
				if (p.PlayerX - 32 < j * 32 && p.PlayerX + 32 > j * 32 &&
					p.PlayerY - 32 < i * 32 && p.PlayerY + 32 > i * 32) {
					point coords;
					coords.x = i * Block;
					coords.y = j * Block;
					coords.num = freeNum;
					/*cout << coords.num << endl;
					cout << p.PlayerX << " " << p.PlayerY << endl;*/
					return coords;

				}
			}


		}

}

int main()
{

	//Init Variables
	const int TSizeGiant = 50;
	const int TSizeBig = 20;
	const int TSizeSmall = 15;
	float dX = 0;
	float dY = 0;
	int sizeM = 0;
	int massiveTile[40][160];
	//////////GUI
	GUI bg("img/bg.jpg");

	//Window

	RenderWindow window(VideoMode(1920, 1080), "Game");
	view.reset(FloatRect(0, 0, 640, 480));
	//Sound
	SoundBuffer buffer;
	buffer.loadFromFile("Sound/Punch.flac");
	Sound sound;
	sound.setBuffer(buffer);
	//Font
	Font BFont, GUI;
	BFont.loadFromFile("fonts/CyrillicOldBold.ttf");
	GUI.loadFromFile("fonts/GUI.otf");
	Text Play("", GUI, TSizeGiant);
	Text BarsH("", BFont, TSizeBig);
	Text BarsM("", BFont, TSizeBig);
	Text BarsS("", BFont, TSizeBig);
	Text Quest("", BFont, TSizeSmall);
	//Map Elements
	Image map_image;
	map_image.loadFromFile("img/map.png");
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
	Player p("gera.png", 205, 1030, 50, 50);
	//Variables init
	bool showMissionText = true;
	float currentFrame = 0;
	int Resolution = 0;
	//Time Variables
	Clock clock;
	Clock gameTimeClock;
	int gameTime = 0;
	int createObjectForMapTimer = 0;
	// Creating Graph Map
	for (int i = 0; i < HEIGHT_MAP; i++) {
		for (int j = 0; j < WIDTH_MAP; j++) {
			if (TileMap[i][j] == ' ') {
				sizeM++;
				massiveTile[i][j] = sizeM;
			}
		}
	}

	graph GraphMap(sizeM + 1 * 4);

	for (int i = 0; i < 40; i++) {
		for (int j = 0; j < 160; j++) {
			if (massiveTile[i][j] < 0) continue;

			if (massiveTile[i][j + 1] > 0) {
				GraphMap.add_edge(massiveTile[i][j], massiveTile[i][j + 1]);
			}
			if (massiveTile[i + 1][j] > 0) {
				GraphMap.add_edge(massiveTile[i][j], massiveTile[i + 1][j]);
			}
			if (massiveTile[i][j - 1] > 0) {
				GraphMap.add_edge(massiveTile[i][j], massiveTile[i][j - 1]);
			}
			if (massiveTile[i - 1][j] > 0) {
				GraphMap.add_edge(massiveTile[i][j], massiveTile[i - 1][j]);
			}
		}
	}
	bool life;
	int tempX = 0;//TMP for X
	int tempY = 0;//TMP for Y
	float distance = 0;//Distance from hero to click

	//Window loop
	while (window.isOpen())
	{
		Vector2i pixelPos = Mouse::getPosition(window);//Cursor cords
		Vector2f pos = window.mapPixelToCoords(pixelPos);
		//Text init variables
		ostringstream playerHealthString;
		ostringstream playerManaString;
		ostringstream playerStaminaString;
		ostringstream task;
		ostringstream score;
		playerHealthString << (int)p.health;
		playerManaString << (int)p.mana;
		playerStaminaString << (int)p.stamina;
		////////
		Play.setFillColor(Color::Yellow);
		Play.setString("Play");
		Play.setPosition(view.getCenter().x - 50, view.getCenter().y - 150);
		////////HP
		BarsH.setFillColor(Color::Red);
		BarsH.setString("HP:" + playerHealthString.str());
		BarsH.setPosition(view.getCenter().x - 300, view.getCenter().y - 240);
		///////Mana
		BarsM.setFillColor(Color::Blue);
		BarsM.setString("Mana:" + playerManaString.str());
		BarsM.setPosition(view.getCenter().x - 300, view.getCenter().y - 220);
		///////Stamina
		BarsS.setFillColor(Color::Green);
		BarsS.setString("Stamina:" + playerStaminaString.str());
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
			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Escape) {
					window.close();
				}
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
		//Rotation Funct
		float dX = pos.x - p.PlayerX;
		float dY = pos.y - p.PlayerY;
		float rotation = (atan2(dY, dX)) * 180 / 3.14159265;
		p.PlayerSpr.setRotation(rotation);
		///////////////////////////////////////////Hero Control////////////////////////////////////////////////////////////////////////
		if (p.life) { //If alive
			if (Keyboard::isKeyPressed(Keyboard::A)) {
				p.Direction = 1; p.Speed = 0.1;
			}
			if (Keyboard::isKeyPressed(Keyboard::D)) {
				p.Direction = 0; p.Speed = 0.1;
			}
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				p.Direction = 3; p.Speed = 0.1;
			}
			if (Keyboard::isKeyPressed(Keyboard::S)) {
				p.Direction = 2; p.Speed = 0.1;
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
			///////////////////View
			getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
		}
		//////Random Generate
		createObjectForMapTimer += time;//Timer +
		if (createObjectForMapTimer > 3000) {
			randomMapGenerate();
			createObjectForMapTimer = 0;//Timer Null
		}
		//Player&Window funct
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
				if ((TileMap[i][j] == 'g'))s_map.setTextureRect(IntRect(192, 0, 32, 32));
				if ((TileMap[i][j] == 'o'))s_map.setTextureRect(IntRect(160, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(224, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}
		//Mission Check
		if (!showMissionText) {
			Quest.setPosition(view.getCenter().x + 120, view.getCenter().y - 130);
			s_quest.setPosition(view.getCenter().x + 115, view.getCenter().y - 130);
			bg.BGs.setPosition(view.getCenter().x -320, view.getCenter().y - 250);
			window.draw(s_quest);
			window.draw(Quest);
			window.draw(bg.BGs);
			window.draw(Play);
		}
		//Drawing Textures and objects
		window.draw(BarsS);
		window.draw(BarsH);
		window.draw(BarsM);
		window.draw(p.PlayerSpr);
		window.display();
	}
	return 0;
}