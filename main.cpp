#include "game.h"

#define TITLE "Soccer"
#define SCREEN_WIDTH 1192
#define SCREEN_HEIGHT 689
#define BALL_SIZE 32
#define FONT_SIZE 48
#define SPEED 10
#define BALL_SPEED 18
#define TIME_END 1 // seconds	
#define FRAME_RATE 144
#define PI 3.14159265358979323846

int main(int argc, char **argv)
{
	Window *window_game = new Window(TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, FONT_SIZE, BALL_SIZE, SPEED, BALL_SPEED, PI, TIME_END, FRAME_RATE);

	return EXIT_SUCCESS;
}