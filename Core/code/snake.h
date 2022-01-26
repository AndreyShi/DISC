/*
 * snake.h
 *
 * Created: 04.12.2017 9:36:58
 *  Author: Andrew
 */ 


#ifndef SNAKE_H_
#define SNAKE_H_
#include "display.h"
#include <avr/pgmspace.h>

Cdisplay320240 dispsnake;


struct snakeBlock {
	float x;
	int y;
};

struct snake
{
	int size;
	struct snakeBlock block[20];
}snake1;

short dire[2] = { 0,1};
short cash;
short speedX;
short speedY;
//snakeBlock snakeLir[5];
//char *map=(char*)malloc();
 char map[][32]  ={"###############################",
				  "#                         $   #",
				  "#        $                    #",
				  "#                   $         #",
				  "#    $                        #",
				  "#             $               #",
				  "#                             #",
				  "#    $                  $     #",
				  "#             $               #",
				  "###############################"};



void initsnakegame();
void snakeLirInit(int x, int y, snake );
bool snakeMoveLir(snake, short *);
bool checkloose(int x, int y,snake );
void drawmap(snake); 
bool checkmap(int x,int y);

bool drawon;



bool checkmap(int x,int y)
{
 
  if (map[y][x] == '#' || map[y][x] == 0x0A)
  return true;
  else
  return false;
}

void drawmap(snake)
{
char *num = 0;


for (int i = 0; map[i][0]; i++)
{
   
	for (int j = 0; map[i][j]; j++)
	{
	   
		if (map[i][j] != ' ')
		{
			dispsnake.gotoxy(j, i*16);
			*num = map[i][j];
			dispsnake.print_ram(num,0);
		}
	}	
}
for (int i = 0; i<snake1.size; i++)
{
display.gotoxy(snake1.block[i].x/speedX, snake1.block[i].y/speedY);
dispsnake.put_char(0x0A,0);
//dispsnake.print_ram((char*)map,0);
}

}

bool checkloose(int x, int y,snake)
{

     if (map[y/16][x] == '#')
	 return true;
	 if (snake1.size>3)
	 {
		 for (int i = 3; i<snake1.size; i++)
		 if (snake1.block[i].x == x&&snake1.block[i].y == y)
		 return true;
	 }



	 if(map[y/16][x] == '$')
	 {
	 map[y/16][x] = ' ';
	 if(drawon)
	      {
	      //dispsnake.printf_flash2(PSTR("Ok"),0,0,230);
		  snakeBlock newblock;
		  newblock.x = snake1.block[snake1.size - 1].x;
		  newblock.y = snake1.block[snake1.size - 1].y;
		  snake1.block[snake1.size]=newblock;
		  snake1.size++;
	      int rx,ry;
		  do{
	      rx = rand() % 0x1E;//map x = 30
	      ry = rand() % 0x8;//map y = 5
		  }while(checkmap(rx,ry));
	      map[ry][rx] = '$';
	      drawmap(snake1);
		  drawon = false;
		  cash++;
		  dispsnake.gotoxy(27,220);
		  dispsnake.print_debug_one(cash,0);
		  }
	 }
	 if(map[y/16][x] == ' ')
	 {
	 drawon = true;
	 }
    return false;
}

void snakeLirInit(int x, int y, snake)
{

	snakeBlock newblock;
	newblock.x = x;
	newblock.y = y;
	snake1.block[0] = newblock;
	//snake1.block[1] = newblock;
	//snake1.block[2] = newblock;
	//snake1.block[3] = newblock;
	//snake1.size = 4;
	snake1.size = 1;
}



bool snakeMoveLir(snake, short *dire)
{
   float x;
   float oldx;
   int y;
   int oldy;


dispsnake.gotoxy(snake1.block[snake1.size - 1].x/speedX, snake1.block[snake1.size - 1].y/speedY);

dispsnake.print_flash(PSTR(" "),0);

oldx = snake1.block[0].x;
oldy = snake1.block[0].y;


     snake1.block[0].x += dire[0];

     snake1.block[0].y += dire[1]*8;

dispsnake.gotoxy(snake1.block[0].x/speedX, snake1.block[0].y/speedY);
dispsnake.put_char(0x0A,0);

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

if(checkloose(snake1.block[0].x/speedX,snake1.block[0].y/speedY,snake1))
{
return false;
}

return true;
}


void initsnakegame()
{
speedX = 1;
speedY = 1;
cash = 0;
dispsnake.printf_flash2(PSTR("Cash: $"),0,20,210);
//map[3][3] = '$';
//snakeLir[snakeLir->size-1].x = 1;
  snakeLirInit(4, 30, snake1);
  drawmap(snake1);
}



#endif /* SNAKE_H_ */