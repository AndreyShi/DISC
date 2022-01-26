/*
 * snake.cpp
 *
 * Created: 26.01.2018 10:25:40
 *  Author: Andrew
 */ 
 
#include "MenuLir.h"
#include <stdlib.h>

#if( configUSE_SnakeGame == 1)
void Menu::initsnakegame()
{
	cash = 1;
	dire[0] = 1;
	dire[1] = 0;
	speed =35; // old 25 000
	snakeLirInit(4, 8 *16);   //for xmega y =30 //for pc 3

	if(!sn_win){
		draw_skbis(' ');
		map[4][13]= '*';
		}
	else
		draw_skbis('*');

	drawmap();

}

void Menu::snakeLirInit(int x_, int y_)
{
	snake1.block[0].x = x_;
	snake1.block[0].y = y_;
	snake1.size = 1;
}

bool Menu::snakeMoveLir(short *dire)
{
	int x;
	int oldx;
	int y;
	int oldy;

	display.print_flash_local(PSTR(" "),0,snake1.block[snake1.size - 1].x, snake1.block[snake1.size - 1].y);
	oldx = snake1.block[0].x;
	oldy = snake1.block[0].y;


	snake1.block[0].x += dire[0];

	snake1.block[0].y += dire[1]*16; //*8;  // for xmega *8

	display.put_char_local_independent(0x0A, 0,snake1.block[0].x, snake1.block[0].y);

	if (snake1.size>1)
	{
		for (int i = 1; i<snake1.size; i++)
		{
			x = snake1.block[i].x;
			y = snake1.block[i].y;
			snake1.block[i].x = oldx;
			snake1.block[i].y = oldy;
			oldx = x;
			oldy = y;
		}
	}

	if (checkloose(snake1.block[0].x, snake1.block[0].y))
		return false;

	return true;	
}

bool Menu::checkloose(int x, int y)
{
	if (map[y/16][x] == '#')   
		return true;
	if (snake1.size>3)
	{
		for (int i = 3; i<snake1.size; i++)
			if (snake1.block[i].x == x && snake1.block[i].y == y)
				return true;
	}
	if (map[y/16][x] == '*') 
	{
		map[y/16][x] = ' '; 
		
			snakeBlock newblock;
			newblock.x = snake1.block[snake1.size - 1].x;
			newblock.y = snake1.block[snake1.size - 1].y;
			snake1.block[snake1.size] = newblock;
			snake1.size++;
			int rx, ry;
			do {
				rx = rand() % X_SNAKE_MAP;
				ry = rand() % Y_SNAKE_MAP;
			} while (checkmap(rx, ry*16));

			if(!sn_win)
				map[ry][rx] = '*';

			drawmap();
			cash++;
			speed = speed-1;
			if(speed < 20)
				speed = 20;
			//display.print_debug_one_local(cash,0,0,16 *12);
			print_long_format(cash,CPrg::FORMAT::FORMAT_END,0,1,16*12,RIGHT_ALIGN,1);
			display.print_flash_local(PSTR("/50"),0,2,16*12);
			if(cash == 50)
			{
			   return true;
			}
	}

	return false;
}

void Menu::drawmap()
{
	for (int i = 0; i<Y_SNAKE_MAP; i++)
	{
		for (int j = 0; j<X_SNAKE_MAP; j++)
			display.put_char_local_independent(map[i][j],0,j, i*16);
	}
	for (int i = 0; i<snake1.size; i++)
		display.put_char_local_independent(0x0A,0,snake1.block[i].x, snake1.block[i].y);
}

bool Menu::checkmap(int x, int y)
{
	if (map[y/16][x] == '#' || map[y/16][x] == 0x0A)
		return true;

	if (snake1.size>0)
	{
		for (int i = 0; i<snake1.size; i++)
			if (snake1.block[i].x == x && snake1.block[i].y == y)
				return true;
	}	

    return false;
}
#endif