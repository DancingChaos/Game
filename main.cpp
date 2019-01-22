
#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <thread> 
#include "map.h"
#include "view.h"
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>
#include "mission.h"
#include <list>

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
		vector<size_t>& al = m_adjacency_list.at(from);
		for (size_t i = 0; i < al.size(); ++i) if (al[i] == to) return;
		al.push_back(to);
	}

private:
	vector<vector<size_t>> m_adjacency_list;
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
	float Width, Heigth, CurrentPX, CurrentPY, CDAttack, CDFireBall,
		PlayerX, PlayerY, Speed, health, mana, stamina, playerScore,
		CrosshairW, CrosshairH;
	int Direction, BDir;
	int mouseX, mouseY;
	const int BarMax = 100;
	graph GraphMap;
	//Init Variables
	bool life, isShoot;
	String File;
	Image PlayerIm;
	Texture PlayerTex;
	Sprite PlayerSpr;
	Image CSI;
	Texture CST;
	Sprite CSS;



	Player(float X, float Y, float W, float H, graph GraphMap, float CSW, float CSH) {
		//Variables
		Direction = 0; Speed = 0; playerScore = 0;
		health = 100; CurrentPX = 0; CurrentPY = 0;
		mana = 100; stamina = 100; life = true;
		isShoot = false;
		this->GraphMap = GraphMap;
		Width = W; Heigth = H;
		CrosshairW = CSW; CrosshairH = CSH;
		//Set Texture
		PlayerIm.loadFromFile("img/Hero.png");
		PlayerIm.createMaskFromColor(Color::White);
		PlayerTex.loadFromImage(PlayerIm);
		PlayerSpr.setTexture(PlayerTex);
		//Set Player Position
		PlayerX = X;
		PlayerY = Y;
		PlayerSpr.setOrigin(Width / 2, Heigth / 2);
		//
		CSI.loadFromFile("img/Crosshair.png");
		CST.loadFromImage(CSI);
		CSS.setTexture(CST);
		CSS.setOrigin(CrosshairW / 2, CrosshairH / 2);
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
		//CoolDowns
		if (CDAttack - 0.005*time >= 0) {
			CDAttack -= 0.005*time;
		}
		else {
			CDAttack = 0;
		}
		if (CDFireBall - 0.0005*time >= 0) {
			CDFireBall -= 0.0005*time;
		}
		else {
			CDFireBall = 0;
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
					if (health >= 0) {
						health -= 0.005*time;
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
		EnemyIm.loadFromFile("img/Enemy.png");
		EnemyIm.createMaskFromColor(Color::White);
		EnemyTex.loadFromImage(EnemyIm);
		EnemySpr.setTexture(EnemyTex);
		//Set Player Position
		eX = X;
		eY = Y;
		EnemySpr.setOrigin(Width / 2, Heigth / 2);
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
		/*needGraphNum = searchPos(enemyGraph.num, playerGraph.num, graphMap);
		needGraph = searchGraphCoords(needGraphNum);
		firstAttempt = false;*/

		distance = sqrt((needGraph.x - eX)*(needGraph.x - eX) + (needGraph.y - eY)*(needGraph.y - eY));
		eX += 0.05*time*(needGraph.x - eX) / distance;
		eY += 0.05*time*(needGraph.y - eY) / distance;


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
class FireBall {
private:
	Texture BulletTex;
	int lifeNum;
	float x;
	float y;

	float plusX;
	float plusY;
	bool firstAttempt = true;

	float directionX;
	float directionY;



public:
	Sprite BulletSpr;
	bool active;

	FireBall(Image &image, float X, float Y, float directionX, float directionY) {
		this->x = X - Block;
		this->y = Y - Block;
		this->directionX = directionX - Block;
		this->directionY = directionY - Block;
		active = true;
		lifeNum = 0;

		BulletTex.loadFromImage(image);
		BulletSpr.setTexture(BulletTex);


	}

	void update(float time) {
		float distance;

		lifeNum++;
		if (lifeNum > 150) {
			active = false;
		}

		if (firstAttempt) {
			distance = sqrt((directionX - x)*(directionX - x) + (directionY - y)*(directionY - y));
			plusX = 0.4*time*(directionX - x) / distance;
			plusY = 0.4*time*(directionY - y) / distance;
			firstAttempt = false;
		}
		x += plusX;
		y += plusY;
		interactionWithMap(time);

		BulletSpr.setPosition(x, y);
	}

	//Collision
	void interactionWithMap(int time)
	{
		for (int i = y / Block; i < (y - 32) / Block; i++)
			for (int j = x / Block; j < (x - 32) / Block; j++)
			{
				if (TileMap[i][j] == '0')
				{
					active = false;
				}
				else { return; }
			}
	}



	~FireBall() {}

};
bool GameStart() {
	//Init Variables
	const int TSizeGiant = 50;
	const int TSizeBig = 20;
	const int TSizeSmall = 15;
	float dX = 0, currentFrame = 0;;
	float dY = 0;
	float loadTime = 3;
	bool Load = false;
	int sizeM = 0, Resolution = 0;;
	int massiveTile[40][160];
	bool Pause = true;

	//Window
	RenderWindow window(VideoMode(1920, 1080), "Game");
	view.reset(FloatRect(0, 0, 640, 480));
	window.setFramerateLimit(60);
	//Sound
	SoundBuffer buffer;
	buffer.loadFromFile("Sound/fb.flac");
	Sound sound;
	sound.setBuffer(buffer);
	//Font
	Font BFont, GUI;
	GUI.loadFromFile("fonts/GUI.otf");
	BFont.loadFromFile("fonts/CyrillicOldBold.ttf");
	Text BarsH("", BFont, TSizeBig);
	Text BarsM("", BFont, TSizeBig);
	Text BarsS("", BFont, TSizeBig);
	Text Score("", BFont, TSizeBig);
	Text Game("", BFont, TSizeGiant);
	Text Restart("", BFont, TSizeGiant);
	Text Play("", GUI, TSizeGiant);
	Text Exit("", GUI, TSizeGiant);
	//Map Elements
	Image map_image;
	map_image.loadFromFile("img/map.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);
	//GameOver&Load Images
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
	Image MenuBg;
	Texture MenuBgT;
	Sprite MenuBgS;
	MenuBg.loadFromFile("img/Menu.jpg");
	MenuBgT.loadFromImage(MenuBg);
	MenuBgS.setTexture(MenuBgT);
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

	// Entities List
	list<FireBall*> bulletList;


	//Player&Obj init
	Player p(205, 1030, 32, 32, GraphMap , 16 , 16);
	Enemy e(215, 1030, 32, 32, GraphMap);

	Image bulletimage;
	bulletimage.loadFromFile("img/Fireball.png");
	bulletimage.createMaskFromColor(Color::White);




	//Window loop
	while (window.isOpen())
	{

		///////Time
		float time = clock.getElapsedTime().asMicroseconds();
		if (p.life == true) { gameTime = gameTimeClock.getElapsedTime().asSeconds(); }
		if (p.life == false) {
			gameTime = 0;
			time = 0;
		}
		clock.restart();
		time = time / 800;
		////////
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) { window.close(); }
			if (Keyboard::isKeyPressed(Keyboard::Enter)) { return true; }
			if (event.type == Event::KeyPressed) {
				if ((event.key.code == Keyboard::Escape)) {
					switch (Pause) {
					case true: {
						Pause = false;
						break;
					}
					case false: {
						Pause = true;
						break;
					}
					}
				}
			}
		}

		//Cursor cords
		Vector2i pixelPos = Mouse::getPosition(window);
		Vector2f pos = window.mapPixelToCoords(pixelPos);
		//Text init variables
		ostringstream playerHealthString;
		ostringstream playerManaString;
		ostringstream playerStaminaString;
		ostringstream task;
		ostringstream score;
		////////String inits
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
		score << (int)p.playerScore;
		Score.setFillColor(Color::White);
		Score.setPosition(view.getCenter().x + 200, view.getCenter().y - 240);
		Score.setString("Score :" + score.str());
		///////////////////////////////////////////Hero Control////////////////////////////////////////////////////////////////////////
		if (p.life == true) { //If alive
			if (Pause) {
				window.setMouseCursorVisible(true);
				gameTime = 0;
				time = 0;
				Play.setString("Play");
				Play.setFillColor(Color::Red);
				Play.setPosition(view.getCenter().x - 40, view.getCenter().y - 100);
				Exit.setString("Exit");
				Exit.setFillColor(Color::Red);
				Exit.setPosition(view.getCenter().x - 40, view.getCenter().y);
				MenuBgS.setPosition(view.getCenter().x - 450, view.getCenter().y - 250);
				window.draw(MenuBgS);
				window.draw(Play);
				window.draw(Exit);
				window.display();
			}
			else {
				
				window.setMouseCursorVisible(false);
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
					if (p.CDAttack == 0) {
						if (p.stamina - 2 >= 0) {
							sound.play();
							p.stamina -= 2;
							bulletList.push_back(new FireBall(bulletimage, p.PlayerX, p.PlayerY, pos.x, pos.y));
							p.CDAttack = 1;
						}
					}

				}
				if (Mouse::isButtonPressed(Mouse::Right)) {
					if (p.CDFireBall == 0) {
						if (p.mana - 1 < 1) {
						}
						else {
							sound.play();
							p.mana -= 20;
							p.CDFireBall = 4;
						}
					}
				}
				//Rotation Funct
				float dX = pos.x - p.PlayerX;
				float dY = pos.y - p.PlayerY;
				float rotation = (atan2(dY, dX)) * 180 / 3.14159265;
				p.PlayerSpr.setRotation(rotation);
				p.CSS.setPosition(pos.x, pos.y);
				///////////////////View
				getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
				//Player&Window funct
				e.getArgs(time, p.getPlayerGraph());
				std::thread(&Enemy::update, &e).detach();
				p.update(time);

				//thread.launch();

				//Random Gen
				//createObjectForMapTimer += time;//наращиваем таймер
				//if (createObjectForMapTimer > 10) {
				//	randomMapGenerate();//генерация случ камней
				//	createObjectForMapTimer = 0;//обнуляем таймер
				//}


				//cout << bulletList.size() << endl;


				if (bulletList.size())
					for (auto it = bulletList.begin(); it != bulletList.end(); it++) {
						if ((*it)->active) {
							(*it)->update(time);
							window.draw((*it)->BulletSpr);
						}
						else {
							delete *it;
							bulletList.erase(it);
							break;
						}
					}

				window.draw(Score);
				window.draw(BarsS);
				window.draw(BarsH);
				window.draw(BarsM);
				window.draw(e.EnemySpr);
				window.draw(p.PlayerSpr);
				window.draw(p.CSS);
				window.display();
				window.clear();
			}
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

		if (!p.life) {
			Restart.setFillColor(Color::Red);
			Game.setFillColor(Color::Red);
			Score.setFillColor(Color::Black);
			Game.setString("Game Over");
			Restart.setString("Press Enter to Restart");
			Game.setPosition(view.getCenter().x - 110, view.getCenter().y - 200);
			Restart.setPosition(view.getCenter().x - 250, view.getCenter().y - 150);
			GraveS.setPosition(view.getCenter().x - 100, view.getCenter().y - 50);
			BGS.setPosition(view.getCenter().x - 400, view.getCenter().y - 300);
			Score.setPosition(view.getCenter().x - 75, view.getCenter().y + 100);
			Score.setString("Yours Score :" + score.str());
			window.draw(BGS);
			window.draw(GraveS);
			window.draw(Game);
			window.draw(Restart);
			window.draw(Score);
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
