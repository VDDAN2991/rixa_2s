#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER

#include "Play.h"
#include "Camera.h"

int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;

//what is the state of the game
struct GameState
{
	float timer = 0;
	int spriteId = 0;
	// Player attributes
	int speed = 10;
	float angle; // Angle is the speed of bidirectional movement
};
//what are the types of game objects
enum GameObjectType
{
	TYPE_NULL = -1,
	angel,
	projectile,
	enemy,
	background,
	TYPE_ENEMY,
	TYPE_ENEMY2,
	TYPE_ENEMY3,
	TYPE_ENEMY4,
	TYPE_TOOL,
};
enum AngelState
{
	STATE_APPEAR = 0,
	STATE_HALT,
	STATE_PLAY,
	STATE_DEAD,
};
AngelState angelState = STATE_APPEAR;

// DVD CLASSES 
class Enemy1 {
public:
	// constructors 
	Enemy1() {};
	Enemy1(std::string name, std::string description) {
		name_ = name;
		description_ = description;
	}
	// getters 
	std::string getName() {
		return name_;
	}
	std::string getDescription() {
		return description_;
	}
	// setters 
	void setName(const std::string name) {
		name_ = name;
	}
	void setDescription(const std::string description) {
		description_ = description;
	}

	// methods
	void CreateEnemy(enum GameObjectType ENEMY_TYPE, int velocity_x) {
		int id_enemy1 = Play::CreateGameObject(ENEMY_TYPE,{ 0, 70}, 50, "coins_2");
		Play::GetGameObject(id_enemy1).velocity = { velocity_x,0 };
		Play::GetGameObject(id_enemy1).animSpeed = 3.0f;
	}
	void UpdateEnemy(enum GameObjectType TYPE, std::string enemy_name, int rotSpeed1) {

		GameObject& obj_enemy = Play::GetGameObjectByType(TYPE);
		Play::SetSprite(obj_enemy, "coins_2", 0.25f);
		if (Play::RandomRoll(100) == 100) {
			int id = Play::CreateGameObject(TYPE_TOOL, { obj_enemy.pos.x, obj_enemy.pos.y }, 90, "bomb");
			GameObject& obj_tool = Play::GetGameObject(id);
			obj_tool.velocity = Point2f(Play::RandomRollRange(-1, 1) * 4, 4);
			obj_tool.rotSpeed = rotSpeed1;
			Play::PlayAudio("tool");
		}
		Play::UpdateGameObject(obj_enemy);
		if (Play::IsLeavingDisplayArea(obj_enemy)) {
			obj_enemy.pos = obj_enemy.oldPos;
			obj_enemy.velocity.x = obj_enemy.velocity.x * -1;
		}
		Play::DrawObjectRotated(obj_enemy);
	}
	void UpdateTools(enum GameObjectType TYPE_TL)
	{
		GameObject& obj_angel = Play::GetGameObjectByType(angel);
		std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TL);
		for (int id : vTools) {
			GameObject& obj_tool = Play::GetGameObject(id);
			Play::UpdateGameObject(obj_tool);
			if (angelState != STATE_DEAD && Play::IsColliding(obj_tool, obj_angel)) {
				angelState = STATE_DEAD;
				Play::StopAudioLoop("music");
				Play::PlayAudio("die");
				obj_angel.pos = { -100,-100 };
			}
			if (Play::IsLeavingDisplayArea(obj_tool, Play::VERTICAL)) {
				obj_tool.pos = -obj_tool.oldPos;
				obj_tool.velocity.x *= -1;
			}
			Play::DrawObjectRotated(obj_tool);
			if (!Play::IsVisible(obj_tool))
				Play::DestroyGameObject(id);
		}
	}

private:
	std::string name_;
	std::string description_;
	std::string enemy_name_; 
};
void UpdateEnemy();
void UpdateTools();
void HandlePlayerControls();
void UpdateGameObjects();
void DrawOffset(GameObject* go);
GameState gameState;
std::string message = "";
Camera camera(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	//approximate directional movement
	gameState.angle = gameState.speed * 0.7;
	//must be done before creating game objects	
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::CentreAllSpriteOrigins();
	// Set default game objects
	Play::CreateGameObject(angel, { DISPLAY_WIDTH / 2,DISPLAY_HEIGHT / 2 }, 100, "angelwalknorth_3");
	Play::CreateGameObject(background, { DISPLAY_WIDTH / 2,DISPLAY_HEIGHT / 2 }, 100, "MarsBG");
	//Play::LoadBackground("Data\\Sprites\\MarsBG2.png");
	//does file exist, read file
	//std::ifstream afile = std::ifstream("config.txt");
	//std::getline(afile, message);
	//afile.close();
	// DVD: enemy
	int id_enemy1 = Play::CreateGameObject(TYPE_ENEMY, { 0, 50 }, 50, "coins_2");
	Play::GetGameObject(id_enemy1).velocity = { 1,0 };
	Play::GetGameObject(id_enemy1).animSpeed = 3.0f;

	Enemy1 v_cershinsky; 
	v_cershinsky.CreateEnemy(TYPE_ENEMY2,2);

	Enemy1 j_bidet;
	j_bidet.CreateEnemy(TYPE_ENEMY3, 3);

	Enemy1 Ursula_von_der_Leyen;
	Ursula_von_der_Leyen.CreateEnemy(TYPE_ENEMY4, 1); 
}
// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	// Delta time
	gameState.timer += elapsedTime;
	HandlePlayerControls();
	UpdateGameObjects();
	//DVD: update to handle the enemy and the bombs;
	UpdateEnemy();
	UpdateTools();

	Enemy1 v_cershinsky = Enemy1("The Beggar", "the communist with the democratic face"); 
	v_cershinsky.UpdateEnemy(TYPE_ENEMY2, "coins_2", 2);
	v_cershinsky.UpdateTools(TYPE_TOOL);

	Enemy1 j_bidet = Enemy1("The BIDET", "The Geriatric Corporation Puppet");
	j_bidet.UpdateEnemy(TYPE_ENEMY3, "coins_2", 3);
	j_bidet.UpdateTools(TYPE_TOOL);

	Enemy1 Ursula_Leyen = Enemy1("Madame Betrayal", "An old lady with a Hustler's Guts");
	Ursula_Leyen.UpdateEnemy(TYPE_ENEMY3, "coins_2", 3);
	Ursula_Leyen.UpdateTools(TYPE_TOOL);

	// Draw Font Text 
	Play::DrawFontText("64px", v_cershinsky.getName(), {DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 30}, Play::CENTRE);
	//draw everything
	Play::PresentDrawingBuffer();
	return Play::KeyDown(VK_ESCAPE);
}
// Gets called once when the player quits the game
int MainGameExit(void)
{
	//write to the config file
	//std::ofstream afile = std::ofstream("config.txt");
	//afile << "Hello World";
	//afile.close();
	Play::DestroyManager();
	return PLAY_OK;
}
void HandlePlayerControls()
{
	GameObject& player = Play::GetGameObjectByType(angel);
	player.velocity = { 0, 0 };
	if (Play::KeyDown(0x57)) // W
	{
		player.velocity = { 0, -gameState.speed };
		if (Play::KeyDown(0x41)) // A
			player.velocity = { -gameState.angle, -gameState.angle };
		if (Play::KeyDown(0x44)) // D
			player.velocity = { gameState.angle, -gameState.angle };
	}
	if (Play::KeyDown(0x53)) // S
	{
		player.velocity = { 0, gameState.speed };
		if (Play::KeyDown(0x41)) // A
			player.velocity = { -gameState.angle, gameState.angle };
		if (Play::KeyDown(0x44)) // D
			player.velocity = { gameState.angle, gameState.angle };
	}
	if (Play::KeyDown(0x41)) // A
	{
		player.velocity = { -gameState.speed, 0 };
		if (Play::KeyDown(0x57)) // W
			player.velocity = { -gameState.angle, -gameState.angle };
		if (Play::KeyDown(0x53)) // S
			player.velocity = { -gameState.angle, gameState.angle };
	}
	if (Play::KeyDown(0x44)) // D
	{
		player.velocity = { gameState.speed, 0 };
		if (Play::KeyDown(0x57)) // W
			player.velocity = { gameState.angle, -gameState.angle };
		if (Play::KeyDown(0x53)) // S
			player.velocity = { gameState.angle, gameState.angle };
	}
	// FIRE WEAPON
	if (Play::KeyPressed(0x45)) // E
	{
		int p = Play::CreateGameObject(projectile, player.pos, 30, "star");
		//Play::GetGameObject(p).velocity = Vector2D( 10, 10 );
		GameObject& nya = Play::GetGameObject(p);
		// Find x and y of mouse relative to position
		Point2D mousePos = Play::GetMousePos();
		int x = floor((mousePos.x - player.pos.x));
		int y = floor((mousePos.y - player.pos.y));
		std::cout << x << std::endl;
		std::cout << y << std::endl;
		int length = sqrt(x * x + y * y) / 10;
		nya.velocity = Vector2D(x / length, y / length);
	}
	// Player bounds checking
	// Issue -- with camera implementation, it may not look like it is leaving display area...
	//if (Play::IsLeavingDisplayArea(player))
	//	if ((player.pos.y > DISPLAY_HEIGHT && player.velocity.y > 0)
	//		|| (player.velocity.y < 0 && player.pos.y < 0))
	//		player.velocity.y = 0;
	//if ((player.pos.x > DISPLAY_WIDTH && player.velocity.x > 0)
	//	|| (player.velocity.x < 0 && player.pos.x < 0))
	//	player.velocity.x = 0;
	player.pos = Point2D(floor(player.pos.x), floor(player.pos.y));
}
void UpdateGameObjects()
{
	// Camera must state all the offsets required to draw each sprite
	// Fixed offset
		// playeroff = player.pos - offset pos
	// if is not
	GameObject& player = Play::GetGameObjectByType(angel);
	camera.Follow(player.pos.x, player.pos.y);
	// BACKGROUND MUST BE UPDATED FIRST
	GameObject& bg = Play::GetGameObjectByType(background);
	DrawOffset(&bg);
	// Update projectiles
	std::vector<int> pv = Play::CollectGameObjectIDsByType(projectile);
	for (int id : pv)
	{
		GameObject& p = Play::GetGameObject(id);
		DrawOffset(&p);
	}
	//Update player
	//DrawOffset(&player);
	//GameObject& player = Play::GetGameObjectByType(angel);
	//Play::UpdateGameObject(player);
	//Play::DrawObject(player);
	DrawOffset(&player);
}
// Draw offset presented by the camera placement
void DrawOffset(GameObject* go)
{
	float oldPosx = go->pos.x;
	float oldPosy = go->pos.y;
	go->pos = { oldPosx - camera.GetXOffset(),oldPosy - camera.GetYOffset() };
	Play::UpdateGameObject(*go);
	Play::DrawObject(*go);
	go->pos = { oldPosx,oldPosy };
	Play::UpdateGameObject(*go);
}
// DVD functions
void UpdateEnemy() {
	GameObject& obj_enemy = Play::GetGameObjectByType(TYPE_ENEMY);
	Play::SetSprite(obj_enemy, "coins_2", 0.25f);
	if (Play::RandomRoll(100) == 100) {
		int id = Play::CreateGameObject(TYPE_TOOL, { obj_enemy.pos.x, obj_enemy.pos.y }, 90, "bomb");
		GameObject& obj_tool = Play::GetGameObject(id);
		obj_tool.velocity = Point2f(Play::RandomRollRange(-1, 1) * 4, 4);
		obj_tool.rotSpeed = 0.2f;
		Play::PlayAudio("tool");
	}
	Play::UpdateGameObject(obj_enemy);
	if (Play::IsLeavingDisplayArea(obj_enemy)) {
		obj_enemy.pos = obj_enemy.oldPos;
		obj_enemy.velocity.x = obj_enemy.velocity.x * -1;
	}
	Play::DrawObjectRotated(obj_enemy);
}
void UpdateTools()
{
	GameObject& obj_angel = Play::GetGameObjectByType(angel);
	std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);
	for (int id : vTools) {
		GameObject& obj_tool = Play::GetGameObject(id);
		Play::UpdateGameObject(obj_tool);
		if (angelState != STATE_DEAD && Play::IsColliding(obj_tool, obj_angel)) {
			angelState = STATE_DEAD;
			Play::StopAudioLoop("music");
			Play::PlayAudio("die");
			obj_angel.pos = { -100,-100 };
		}
		if (Play::IsLeavingDisplayArea(obj_tool, Play::VERTICAL)) {
			obj_tool.pos = -obj_tool.oldPos;
			obj_tool.velocity.x *= -1;
		}
		Play::DrawObjectRotated(obj_tool);
		if (!Play::IsVisible(obj_tool))
			Play::DestroyGameObject(id);
	}
}

