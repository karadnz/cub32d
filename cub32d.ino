/**************************************************************************
 *
 * Interfacing ESP8266 NodeMCU with ST7789 TFT display (240x240 pixel).
 * Graphics test example.
 * This is a free software with NO WARRANTY.
 * https://simple-circuit.com/
 *
 *************************************************************************/

#include <Adafruit_GFX.h>	 // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

// ST7789 TFT module connections
#define TFT_DC 2  // TFT DC  pin is connected to NodeMCU pin D1 (GPIO5)
#define TFT_RST 4 // TFT RST pin is connected to NodeMCU pin D2 (GPIO4)
#define TFT_CS D8 // TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)
// initialize ST7789 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS -1 // Not connected
#define TFT_DC 2
#define TFT_RST 4 // Connect reset to ensure display initialises
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

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
		raycast(&game);
	}
}

void	ft_turn(Player *player)
{
	player->dir -= 0.03;
		if (player->dir < 0)
			player->dir += 2 * M_PI;
}
