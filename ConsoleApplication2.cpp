#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

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
//////////////////Point and graphs///////////////////////////////
struct point { int x; int y; int num; };
struct graph {
	graph() = default;
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
point searchGraphCoords(int needNum) {
	int freeNum = 0;
	for (int i = 0; i < HEIGHT_MAP; i++)
		for (int j = 0; j < WIDTH_MAP; j++)
		{
			if (TileMap[i][j] == ' ' || TileMap[i][j] == 'f' || TileMap[i][j] == 'g' ||
				TileMap[i][j] == 'o' || TileMap[i][j] == 'w' || TileMap[i][j] == 's' ||
				TileMap[i][j] == 'h') {
				freeNum++;
				if (needNum == freeNum) {
					point coords;
					coords.x = j * Block;
					coords.y = i * Block;
					coords.num = freeNum;
					return coords;

				}
			}
		}
}
//////////////////Player Class////////////////////////
class Player {
public:
	//Init Variables
	float Width, Heigth, CurrentPX, CurrentPY,
		PlayerX, PlayerY, Speed, health, mana, stamina , playerScore;
	int Direction,  BDir;
	int mouseX, mouseY;
	const int BarMax = 100;
	graph GraphMap;
	//Init Variables
	bool life = true;
	String File;
	Image PlayerIm;
	Texture PlayerTex;
	Sprite PlayerSpr;

	/////////////
	Player() {
		//Variables
		Direction = 0; Speed = 0; playerScore = 0;
		health = 100; CurrentPX = 0; CurrentPY = 0;
		mana = 100; stamina = 100; life = true;
		this->GraphMap = GraphMap;
		Width = 0; Heigth = 0;
		//Set Texture
		PlayerIm.loadFromFile("img/gera1.png");
		PlayerIm.createMaskFromColor(Color::White);
		PlayerTex.loadFromImage(PlayerIm);
		PlayerSpr.setTexture(PlayerTex);
		//Set Player Position
		PlayerX = 0;
		PlayerY = 0;
		PlayerSpr.setOrigin(Width / 2, Heigth / 2);
	}
	Player(float X, float Y, float W, float H, graph GraphMap) {
		//Variables
		Direction = 0; Speed = 0; playerScore = 0;
		health = 100; CurrentPX = 0; CurrentPY = 0;
		mana = 100; stamina = 100; life = true;
		this->GraphMap = GraphMap;
		Width = W; Heigth = H;
		//Set Texture
		PlayerIm.loadFromFile("img/gera1.png");
		PlayerIm.createMaskFromColor(Color::White);
		PlayerTex.loadFromImage(PlayerIm);
		PlayerSpr.setTexture(PlayerTex);
		//Set Player Position
		PlayerX = X;
		PlayerY = Y;
		PlayerSpr.setOrigin(Width / 2, Heigth / 2);
	}
	void update(float time)
	{
		cout << time << endl;
		//Player Direction
		switch (Direction)
		{
		case 0: CurrentPX = Speed; CurrentPY = 0; break;
		case 1: CurrentPX = -Speed; CurrentPY = 0; break;
		case 2: CurrentPX = 0; CurrentPY = Speed; break;
		case 3: CurrentPX = 0; CurrentPY = -Speed; break;
		}
		//scoreTimer
		playerScore += 0.0002*time;
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

	//

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
					if (health >= 0) {
						health -= 0.5*time;
					}
					else {
						health = 0;
					}

				}
				if (TileMap[i][j] == 'h') {
					if (health + 20 <= 100) {
						health += 20;
						TileMap[i][j] = ' ';
					}
				}

			}
	}

	// Graph Works
	point getPlayerGraph() {
		int freeNum = 0;
		int size = 16;
		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ' || TileMap[i][j] == 'f' || TileMap[i][j] == 'g' ||
					TileMap[i][j] == 'o' || TileMap[i][j] == 'w' || TileMap[i][j] == 's' ||
					TileMap[i][j] == 'h') {
					freeNum++;
					if (PlayerX - size < j * 32 && PlayerX + size > j * 32 &&
						PlayerY - size < i * 32 && PlayerY + size > i * 32) {
						point coords;
						coords.x = i * Block;
						coords.y = j * Block;
						coords.num = freeNum;

						return coords;

					}
				}
			}
		point tempcoord;
		tempcoord.x = 1;
		tempcoord.y = 1;
		tempcoord.num = 1;
		return tempcoord;

	}

	// Operator reloading
	Player operator =(Player p) {
		return p;
	}



};
/////////////////Enemy Class///////////////////////////
class Enemy {
public:
	float Width, Heigth, CurrentPX, CurrentPY,
		eX, eY, health;
	bool life;
	bool firstAttempt = true;
	int needGraphNum;
	Image EnemyIm;
	Texture EnemyTex;
	Sprite EnemySpr;
	graph graphMap;
	point needGraph;
	float time;
	point playerGraph;
	//Player p;

	Enemy(float X, float Y, float W, float H, graph graphMap) {
		//Variables
		health = 100; CurrentPX = 0; CurrentPY = 0; life = true;
		Width = W; Heigth = H;
		this->graphMap = graphMap;
		
		//Set Texture
		EnemyIm.loadFromFile("img/gera.png");
		EnemyIm.createMaskFromColor(Color::White);
		EnemyTex.loadFromImage(EnemyIm);
		EnemySpr.setTexture(EnemyTex);
		//Set Player Position
		eX = X;
		eY = Y;
		EnemySpr.setOrigin(Width / 4, Heigth / 4);
		needGraph.num = 0;
		needGraph.x = 0;
		needGraph.y = 0;


	}

	void getArgs(float time, point playerGraph) {
		this->time = time;
		this->playerGraph = playerGraph;
		//this->p = p;
	}
	void update() {
		point enemyGraph = getEnemyGraph();

		if (enemyGraph.num != playerGraph.num) {
			goToPlayer(enemyGraph, playerGraph, time);
		}

		////Rotation Funct
		/*float dX = p.PlayerX - eX;
		float dY = p.PlayerY - eY;
		float rotation = (atan2(dY, dX)) * 180 / 3.14159265;
		EnemySpr.setRotation(rotation);*/

		EnemySpr.setPosition(eX, eY);

	}

	void goToPlayer(point enemyGraph, point playerGraph, float time) {
		float distance;
		if (enemyGraph.num == needGraphNum || firstAttempt) {
			needGraphNum = searchPos(enemyGraph.num, playerGraph.num, graphMap);
			needGraph = searchGraphCoords(needGraphNum);
			firstAttempt = false;
		}

		distance = sqrt((needGraph.x - eX)*(needGraph.x - eX) + (needGraph.y - eY)*(needGraph.y - eY));
		eX += 0.03*time*(needGraph.x - eX) / distance;
		eY += 0.03*time*(needGraph.y - eY) / distance;


	}


	point getEnemyGraph() {
		int freeNum = 0;
		int size = 16;
		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ' || TileMap[i][j] == 'f' || TileMap[i][j] == 'g' ||
					TileMap[i][j] == 'o' || TileMap[i][j] == 'w' || TileMap[i][j] == 's' ||
					TileMap[i][j] == 'h') {
					freeNum++;
					if (eX - size < j * 32 && eX + size > j * 32 &&
						eY - size < i * 32 && eY + size > i * 32) {
						point coords;
						coords.x = i * Block;
						coords.y = j * Block;
						coords.num = freeNum;
						return coords;
					}
				}

			}

	}



	size_t searchPos(size_t startT, size_t targeT, graph GraphMap) 
	{
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

		if (reached_by[target] == GraphMap.number_of_nodes()) {
			cout << "No path to " << target << " found!" << endl;
			return 1;
		}
		
		else {
			for (size_t node = target; node != start; node = reached_by[node]) {
				need_num = node;
			}

			return need_num;
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
bool GameStart() {
	//Init Variables
	const int TSizeGiant = 50;
	const int TSizeBig = 20;
	const int TSizeSmall = 15;
	float dX = 0;
	float dY = 0;
	int sizeM = 0;
	int massiveTile[40][160];
	//////////GUI
	//GUI bg("img/bg.jpg");

	//Window

	RenderWindow window(VideoMode(1920, 1080), "Game");
	view.reset(FloatRect(0, 0, 640, 480));
	window.setFramerateLimit(60);

	//Sound
	SoundBuffer buffer;
	buffer.loadFromFile("Sound/Punch.flac");
	Sound sound;
	sound.setBuffer(buffer);
	//Font
	Font BFont, GUI;
	BFont.loadFromFile("fonts/CyrillicOldBold.ttf");
	GUI.loadFromFile("fonts/GUI.otf");
	Text BarsH("", BFont, TSizeBig);
	Text BarsM("", BFont, TSizeBig);
	Text BarsS("", BFont, TSizeBig);
	Text Quest("", BFont, TSizeBig);
	Text Game("", BFont, TSizeGiant);
	Text Restart("", BFont, TSizeGiant);
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
	//GameOver Images
	Image GraveI;
	GraveI.loadFromFile("img/Grave.png");
	Texture GraveT;
	GraveT.loadFromImage(GraveI);
	Sprite GraveS;
	GraveS.setTexture(GraveT);
	Image BGI;
	BGI.loadFromFile("img/BG.jpg");
	Texture BGT;
	BGT.loadFromImage(BGI);
	Sprite BGS;
	BGS.setTexture(BGT);

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
			if (TileMap[i][j] == ' ' || TileMap[i][j] == 'f' || TileMap[i][j] == 'g' ||
				TileMap[i][j] == 'o' || TileMap[i][j] == 'w' || TileMap[i][j] == 's' ||
				TileMap[i][j] == 'h') {
				sizeM++;
				massiveTile[i][j] = sizeM;
			}
		}
	}

	graph GraphMap(sizeM + 1 * 4);

	for (int i = 0; i < HEIGHT_MAP; i++) {
		for (int j = 0; j < WIDTH_MAP; j++) {
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

	//Player&Obj init
	Player p(205, 1030, 32, 32, GraphMap);
	Enemy e(215, 1030, 32, 32, GraphMap);

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
		//////Score
		score <<(int)p.playerScore;
		Quest.setFillColor(Color::White);
		Quest.setPosition(view.getCenter().x + 200, view.getCenter().y - 240);
		Quest.setString("Score :" + score.str());
		///////Time
		float time = clock.getElapsedTime().asMicroseconds();
		if (p.life == true) { gameTime = gameTimeClock.getElapsedTime().asSeconds(); }
		if (p.life == false) {
			gameTime = 0;
			time = 0;
		}
		else { view.move(0.8, 0); }
		clock.restart();
		time = time / 800;
		///////Event Loop
		Event event;
		while (window.pollEvent(event))
		{
			if (Keyboard::isKeyPressed(Keyboard::Enter)) { return true; }//если таб, то перезагружаем игру
			if (Keyboard::isKeyPressed(Keyboard::Escape)) { return false; }//если эскейп, то выходим из игры
		}
		///////////////////////////////////////////Hero Control////////////////////////////////////////////////////////////////////////
		if (p.life == true) { //If alive
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
			//Rotation Funct
			float dX = pos.x - p.PlayerX;
			float dY = pos.y - p.PlayerY;
			float rotation = (atan2(dY, dX)) * 180 / 3.14159265;
			p.PlayerSpr.setRotation(rotation);
			///////////////////View
			getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());

			//Player&Window funct
			p.update(time);
			e.getArgs(time, p.getPlayerGraph());


			Thread thread(&Enemy::update, &e);
			thread.launch();
			
			
			window.draw(Quest);
			window.draw(BarsS);
			window.draw(BarsH);
			window.draw(BarsM);
			window.draw(e.EnemySpr);
			
			window.draw(p.PlayerSpr);
			window.display();
		}

	
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
				if ((TileMap[i][j] == 'o'))s_map.setTextureRect(IntRect(160, 0, 32, 32));
				if ((TileMap[i][j] == 'g'))s_map.setTextureRect(IntRect(192, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(224, 0, 32, 32));
				if ((TileMap[i][j] == '1')) s_map.setTextureRect(IntRect(256, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}
	

		if(p.life == false){
			Restart.setFillColor(Color::Red);
			Game.setFillColor(Color::Red);
			Quest.setFillColor(Color::Black);
			Game.setString("Game Over");
			Restart.setString("Press Enter to Restart");
			Game.setPosition(view.getCenter().x - 110, view.getCenter().y - 200);
			Restart.setPosition(view.getCenter().x - 250, view.getCenter().y - 150);
			GraveS.setPosition(view.getCenter().x - 100 , view.getCenter().y - 50 );
			BGS.setPosition(view.getCenter().x - 400, view.getCenter().y - 300);
			Quest.setPosition(view.getCenter().x - 75, view.getCenter().y + 100);
			Quest.setString("Yours Score :" + score.str());
			window.draw(BGS);
			window.draw(GraveS);
			window.draw(Game);
			window.draw(Restart);
			window.draw(Quest);
			window.display();
		}
		
	}
}
void GameRunning() {
	if (GameStart()) { GameRunning(); }
}

int main()
{
	GameRunning();
	return 0;
}