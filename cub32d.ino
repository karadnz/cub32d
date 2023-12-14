
#include <Adafruit_GFX.h>    // Core graphics library for rendering graphics on displays
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789-based TFT displays

// ST7789 TFT module connections
#define TFT_MISO 19 // Pin for Master In Slave Out for SPI communication BOS
#define TFT_MOSI 23 // Pin for Master Out Slave In for SPI communication
#define TFT_SCLK 18 // Pin for Serial Clock for SPI communication 

#define TFT_CS   -1 // Chip Select pin, set to -1 if not used
#define TFT_DC    2 // Data/Command pin to differentiate data and command transmissions
#define TFT_RST   4 // Reset pin, connects to reset the display for proper initialization

// ST7789 object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


//draw buffer
//#define SCREEN_WIDTH 240
//#define SCREEN_HEIGHT 240
//maybe convert to char and use as enum while drawing
//uint16_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];


float p = 3.1415926;

#include <math.h>

#define WIDTH 240
#define HEIGHT 240
#define MAP_SIZE 10
#define FOV M_PI / 3
#define RAY_STEP 0.01

int game_map[MAP_SIZE][MAP_SIZE] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

typedef struct
{
	double x;
	double y;
} Point;

typedef struct
{
	Point dir;
	Point side_dist;
	Point delta_dist;
	Point step;
	Point p_map;

	int side;
	int hit;
  double angle; //angle of loop
	double perp_wall_dist;
} Ray;

typedef struct
{
	double x;
	double y;
	double dir;
} Player;

typedef struct
{
	Player player;

} Game;

Game game;

unsigned long previousMillis = 0;

void setup(void)
{
	Serial.begin(9600);
	Serial.print(F("Hello! ST77xx TFT Test"));

	// if the display has CS pin try with SPI_MODE0
	tft.init(240, 240, SPI_MODE2); // Init ST7789 display 240x240 pixel

	// if the screen is flipped, remove this command
	tft.setRotation(2);

	Serial.println(F("Initialized"));

	game.player.x = 5;
	game.player.y = 5;
	game.player.dir = M_PI / 4;
}

void loop()
{
	unsigned long currentMillis = millis(); // Capture the current time

  // 1000/30 = 33.3ms
  //
	if (currentMillis - previousMillis >= 33)
	{
		previousMillis = currentMillis;
    ft_turn(&game.player);
    unsigned long raycastStart = millis();
		raycast(game);
    unsigned long raycastEnd = millis();
    Serial.println("Time spent: "); //70 - 75 (60 for draw)
    Serial.println(raycastEnd - raycastStart);
	}
}

//rotate test
void	ft_turn(Player *player)
{
	player->dir -= 0.03;
		if (player->dir < 0)
			player->dir += 2 * M_PI;
}
