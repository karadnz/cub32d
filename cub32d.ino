
#include <Adafruit_GFX.h>	 // Core graphics library for rendering graphics on displays
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789-based TFT displays
#include <SPI.h>

// ST7789 TFT module connections
#define TFT_MISO 19 // Pin for Master In Slave Out for SPI communication //BOS
#define TFT_MOSI 23 // Pin for Master Out Slave In for SPI communication //SDA
#define TFT_SCLK 18 // Pin for Serial Clock for SPI communication //SCL

#define TFT_CS -1 // Chip Select pin, set to -1 if not used
#define TFT_DC 2  // Data/Command pin to di                                                                                                                                                                                                                                        tiate data and command transmissions
#define TFT_RST 4 // Reset pin, connects to reset the display for proper initialization

// FASTER
#define TFT_CS -1
#define TFT_DC 2
#define TFT_SCLK 18 // SCL
#define TFT_MOSI 23 // SDA
#define TFT_RST 4

// MOVE
#define BUTTON_LEFT 32
#define BUTTON_RIGHT 33

#define BUTTON_FORWARD 26
#define BUTTON_BACKWARD 25
//#define BUTTON_LEFT 27
//#define BUTTON_RIGHT 28

// ST7789 object
// Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ST7789* tft = NULL;

// draw buffer
//#define SCREEN_WIDTH 240
//#define SCREEN_HEIGHT 240
// maybe convert to char and use as enum while drawing
// uint16_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

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
	double angle; // angle of loop
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

	SPIClass* spi = new SPIClass(HSPI);
	spi->begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
	tft = new Adafruit_ST7789(spi, TFT_CS, TFT_DC, TFT_RST);
	// 80MHz should work, but you may need lower speeds
	tft->setSPISpeed(60000000);
	// this will vary depending on your display
	tft->init(240, 240, SPI_MODE2);

	// if the display has CS pin try with SPI_MODE0
	// tft.init(240, 240, SPI_MODE2); // Init ST7789 display 240x240 pixel

	// if the screen is flipped, remove this command
	tft->setRotation(2);

	Serial.println(F("Initialized"));

	game.player.x = 5;
	game.player.y = 5;
	game.player.dir = M_PI / 4;

	// button init
	pinMode(BUTTON_LEFT, INPUT_PULLUP);
	pinMode(BUTTON_RIGHT, INPUT_PULLUP);

	pinMode(BUTTON_FORWARD, INPUT_PULLUP);
	pinMode(BUTTON_BACKWARD, INPUT_PULLUP);
	//pinMode(BUTTON_LEFT, INPUT_PULLUP);
	//pinMode(BUTTON_RIGHT, INPUT_PULLUP);
}

void loop()
{
	unsigned long currentMillis = millis(); // Capture the current time

	// 1000/30 = 33.3ms
	//
  ft_move();

	if (currentMillis - previousMillis >= 33)
	{
		previousMillis = currentMillis;
		// ft_turn(&game.player);
		unsigned long raycastStart = millis();
		raycast(game);
		unsigned long raycastEnd = millis();
		Serial.println("Time spent: "); // 70 - 75 (60 for draw)
		Serial.println(raycastEnd - raycastStart);
	}
}
void ft_move()
{
	if (digitalRead(BUTTON_LEFT) == LOW)
	{
		// Turn player left
		game.player.dir += 0.03;
		if (game.player.dir > 2 * M_PI)
		{
			game.player.dir -= 2 * M_PI;
		}
	}

	// Check if right button is pressed
	if (digitalRead(BUTTON_RIGHT) == LOW)
	{
		// Turn player right
		game.player.dir -= 0.03;
		if (game.player.dir < 0)
		{
			game.player.dir += 2 * M_PI;
		}
	}

	// Moving forward
	if (digitalRead(BUTTON_FORWARD) == LOW)
	{
		game.player.x += cos(game.player.dir) * 0.1;
		game.player.y += sin(game.player.dir) * 0.1;
	}

	// Moving backward
	if (digitalRead(BUTTON_BACKWARD) == LOW)
	{
		game.player.x -= cos(game.player.dir) * 0.1;
		game.player.y -= sin(game.player.dir) * 0.1;
	}

	// Moving left
	if (0 && digitalRead(BUTTON_LEFT) == LOW)
	{
		game.player.x += cos(game.player.dir - M_PI / 2) * 0.1;
		game.player.y += sin(game.player.dir - M_PI / 2) * 0.1;
	}

	// Moving right
	if (0 && digitalRead(BUTTON_RIGHT) == LOW)
	{
		game.player.x += cos(game.player.dir + M_PI / 2) * 0.1;
		game.player.y += sin(game.player.dir + M_PI / 2) * 0.1;
	}
}
// rotate test
void ft_turn(Player* player)
{
	player->dir -= 0.03;
	if (player->dir < 0)
		player->dir += 2 * M_PI;
}
