/*********************程序说明**********************************
程序：终端贪吃蛇游戏 
作者：Memory
2017/1/9 16:00   Version 1.0 
2017/1/9 23:08   Version 1.5	加入暂停功能、限制原地回头、吃到自己会死亡、 
								随着分数增加速度会变快 、改变蛇头形状
2017/1/14 00:31  Version 2.0    用二维数组和链表重构，拓展性更强， 
								加入游戏初始界面，加入两种可选模式， 
								得分可变颜色 
2017/1/14 11:53  Version 2.5    加入关卡
2017/1/14 17:25  Version 2.6
2017/1/14 23:56  Version 3.0    修复更新速度跟不上输入导致死亡的 bug，用系统延时代替自己的函数 
								各种小优化，增加大量的宏定义、常量和注释，增强代码可读性 
*********************头文件************************************/
#include<cstdio>
#include<conio.h>												// getch() 
#include<cstdlib>												// rand()
#include<windows.h>												// system()、HideCursor()、 gotoxy(int x,int y)
#include<unistd.h>												// sleep() 、usleep() 
#include<ctime>													// time()
/*******************常量的定义*********************************/ 

#define WIDTH          35										// 游戏界面的初始宽度 
#define HIGH           25 										// 游戏界面的初始高度 
#define LENGTH         5										// 蛇的初始长度 
#define INITSPEED      1										// 蛇的初始速度 
#define CLASSICALMODE  '1'										// 经典模式 
#define LEVELMODE      '2'										// 闯关模式 
#define SECONDPERSPEED 30000									// 初始延时时间 ，单位是百万分之一秒，即微秒 
#define INITDELAY      300200									// 每增加一个速度减少的延时时间 
#define SCOREPERLEVEL  8										// 过一关需要的得分 
#define SCOREPERSPEED  5										// 增加一次速度需要的得分 

const char left = 'a';											//	蛇的运动方向 
const char right = 'd';											//
const char up = 'w';											//
const char down = 's';											//
const char snake_body = '$';									// 蛇的身体 
const char snake_head = '@';									// 蛇头 
const char food = '*'; 											// 食物 
const char space = ' ';											// 空格 
const char x_line = '_';
const char y_line = '|';

/****************代码开始**************************************/

struct NextMove{												//蛇如何方位改变的数据 
	char move; 
	int x;														// x, y 分别是横向和纵向的改变量 
	int y;
}nextMove[5] = {{up, 0, -1}, {left, -1, 0}, {right, 1, 0}, {down, 0, 1}};

void HideCursor()												//隐藏光标函数 ,百度上找的 
{
	CONSOLE_CURSOR_INFO cursor_info = {1, 0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void gotoxy(int x,int y)										//类似于清屏函数，光标移动到 x, y位置进行重画
{																// x 是列， y 是行 
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(handle,pos);
}

class Snake{
	private:
		char **maze;											//游戏界面的二维数组指针
		struct snakeNode{
			int x;												//蛇的位置的 x 坐标 
			int y;												//蛇的位置的 y 坐标
			char mov;											//蛇当前的移动方向 
			snakeNode *next;										
		}snake, *head;											//表头指向蛇尾， head 指向蛇头 
		int width;												//游戏界面的宽度
		int high;												//游戏界面的高度
		int length;												//蛇的长度 
		int food_x, food_y;										//食物的位置
		int last_food_x, last_food_y;							//上一个食物的位置  
		int score;												//获得的分数 
		char next;												//蛇下一步的方向
		int delay;												//模拟时钟 
		int speed;												//蛇的速度
		int level;												//目前的关卡数 
		bool pause;												//是否暂停 
		bool end;												//是否结束 
		char mode;												//游戏模式：1为无尽模式，2为闯关模式 
		FILE *fpScore, *fpSpeed;								//存储最高记录的文件 ："score.txt" 和 "speed.txt" 
	public:
		Snake();												//初始化
		void creatFile();										//创建防止最高纪录的文件 
		void creatSnake();										//生成一条蛇 
		void creatMap();										//创建空地图 
		void showGame();										//显示游戏界面
		void showBegin();										//显示选择初始界面
		void showRecord();										//显示最高记录
		void showEnd();											//显示游戏结束的闪烁界面
		void changePosition();									//根据方向改变蛇的位置 
		void changeLevel(); 									//改变当前关卡
		void changeColor();										//改变界面颜色 
		void updateByScore();									//分数引起的改变
		void updateInput();										//和输入相关的数据更新
		void updateRecord();									//更新游戏的最高记录
		void growUp();											//蛇消化完食物长度加一 
		void addFood();											//随机生成一个食物，进行一些数据更新 
		void gameRun();											//运行的主要函数  
		bool isEnd();											//游戏是否结束  
};

//函数的构造函数，在这里进行一些数据的初始化
//包括第一次运行时创建两个最高记录的文件
//和创建一条初始长度为 length 的蛇(用链表表示） 
Snake::Snake()													
{
	width = WIDTH;												// 游戏界面的大小 
	high = HIGH;												// 
	length = LENGTH;											//蛇的初始长度 
	speed = INITSPEED;											//初始速度 
	delay = INITDELAY - speed * SECONDPERSPEED;					//延迟的设置，与速度成反比 
	score = 0;													// 
	level = 1;													// 
	end = false;												// 
	pause= false; 												// 
	food_x = width >> 1;	  									//食物的初始位置固定 
	food_y = high >> 2;
	next = right; 	 											//蛇的初始移动方向固定为向右 
	creatMap();													// 
	creatSnake();												// 
	creatFile();												// 
	maze[food_y][food_x] = food;								//设置食物 
}
//创建初始地图 
void Snake::creatMap()
{
	maze = new char*[high+1];									//创建二维数组表示游戏界面
	for(int i = 0; i <= high; i++)								//空白界面设置 
	{
		maze[i] = new char[width+1];
		for(int j = 0; j <= width; j++)
		{
			if((i == 0) || (i == high ) || (j == width) || (j == 0))
				maze[i][j] = '+';
			else
				maze[i][j] = space;
		}
	}
}

//检查是否有存储最高记录的文件，若没有则创建 
void Snake::creatFile() 
{
	if((fpScore = fopen("score.txt", "r")) == NULL)
	{
		fpScore = fopen("score.txt", "w");
		fprintf(fpScore,"%d",0);
		fclose(fpScore);
	}
	if((fpSpeed = fopen("speed.txt", "r")) == NULL)
	{
		fpSpeed = fopen("speed.txt", "w");
		fprintf(fpSpeed,"%d", 0);
		fclose(fpSpeed);
	}
}

//用链表初始化一条长度为 length 的蛇，表头是蛇尾，存储蛇的坐标信息  
void Snake::creatSnake()
{
	snakeNode *temp;
	snakeNode *last = &snake;
	for(int i = length; i < length << 1; i++) 
	{									
		temp = new snakeNode;				
		temp->x = i;
		temp->y = high >> 1;
		temp->mov = right;
		temp->next = NULL;
		last->next = temp;
		last = temp;
		if(i < length + 4)
			maze[temp->y][temp->x] = snake_body;
		else
		{
			maze[temp->y][temp->x] = snake_head;
			head = temp;										//蛇头指向表尾			
		}
	}
}
//显示初始界面，让玩家选择游戏模式
void Snake::showBegin()											 
{
	system("cls");
	gotoxy(10, 5);
	printf("欢迎来到贪食蛇游戏!\n");
	printf("\n          按w,a,s,d控制移动方向,空格键暂停。\n");
	printf("\n          请选择游戏模式(按数字): \n");
	printf("\n            1、无限模式\n");
	printf("\n            2、闯关模式\n");
	char m = getch();											
	while( (m != CLASSICALMODE) && (m != LEVELMODE))			//重复输入直到正确 
		m = getch();
	mode  = m;
	
	if(mode == LEVELMODE)										//若选择无尽模式则继续选择速度 
	{
		printf("\n          请输入速度(1~10): ");
		int s;
		scanf("%d", &s);
		if(s < 1) s = 1;										//速度最低为 1  
		else if(s > 10) s = 10;									//最高为 10
		speed = s;
		delay = INITDELAY - speed * SECONDPERSPEED;
	} 
	system("cls");
}
//显示主游戏界面 
void Snake::showGame()						
{
	gotoxy(0,1);												//每次移动光标回初始位置 

	for(int i = 0; i <= high; i++)								//显示界面 
	{
		for(int j = 0; j <= width; j++)
		{
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
	printf("Score: %d",score);									//显示当前分数 
	for(int i = 0; i < width - 18; i++)
		printf(" ");
	if(mode == CLASSICALMODE)									//根据模式选择显示关卡还是速度 
	{
		printf("Speed: %d\n",speed);
	}
	else if(mode == LEVELMODE)
	{
		printf("Level: %d\n",level);
	}
}
//显示最高记录，从文件中获取数据 
void Snake::showRecord()
{
	if(mode == CLASSICALMODE)
	{
		int s;
		fpScore = fopen("score.txt", "r");
		fscanf(fpScore,"%d", &s);
		printf(" Highest Score : %d ",s); 
	}
	else if(mode == LEVELMODE)
	{
		int s;
		fpSpeed = fopen("speed.txt", "r");
		fscanf(fpSpeed, "%d", &s);
		printf(" Highest Speed to Pass: %d ", s);
	}
}
//显示游戏结束时的画面 
void Snake::showEnd()
{
	updateRecord();
	if(end)														//若是撞到障碍结束，则闪烁三下 
	{
		for(int i = 0; i <= 5; i++)
		{
			usleep(500000); 
			snakeNode *t = snake.next;
			while(t)
			{
				if(maze[t->y][t->x] != space)
					maze[t->y][t->x] = space;
				else if(t->next)
					maze[t->y][t->x] = snake_body;
				else
					maze[t->y][t->x] = snake_head;
				t = t->next;
			}
			showGame();
		}
	}
	else														//若是通关结束，则显示结束语 
	{
		pause = true;
		gotoxy(10, 7);
		printf("你竟然玩通关了!\n");
		if(speed < 10)
			printf("\n       有本事把速度调高了再玩一次!\n"); 
		else
			printf("\n          你真他妈的是个天才\n");
	}
	
	bool x = true;
	while(!kbhit())												//闪烁显示”按任意键继续“ 
	{
		gotoxy(10, 13);
		usleep(500000);
		if(x)
			printf(" 按任意键继续 ");
		else
			printf("               ");
		x = !x;
	}	
}
//游戏结束时更新最高记录，更新到文件中 
void Snake::updateRecord()
{
	int s, x;
	fpScore = fopen("score.txt", "r");
	fpSpeed = fopen("speed.txt", "r");
	fscanf(fpScore,"%d", &s);
	fscanf(fpSpeed, "%d", &x);
	if(mode == CLASSICALMODE && score > s)
	{
		fpScore = fopen("score.txt", "w");
		fprintf(fpScore,"%d",score);
		fclose(fpScore);
	}
	if(mode == LEVELMODE && speed > x && !end)
	{
		fpSpeed = fopen("speed.txt", "w");
		fprintf(fpSpeed,"%d",speed);
		fclose(fpSpeed);
	}
}
//改变字体颜色，底色为黑色不变，字体颜色随机改变
//system("color xy") 中，x 为底色， y 为字体颜色，范围是16进制 0 ~ e 
void Snake::changeColor()											 
{
	char color[20] = "color 00";
	char x = rand() % 15 + 1;
	if(x <= 9)	x += '0';
	else 		x += left - 10;
	color[7] = x;
	system(color);
} 

//闯关模式下改变关卡，不同关卡显示不同障碍 
void Snake::changeLevel()
{
	for(int i = 0; i <= high; i++)								//每次都重置边框以及清除上一关的障碍 
	{
		for(int j = 0; j <= width; j++)
		{
			if(((j == width) || (j == 0)) && (i != 0))
				maze[i][j] = y_line; 
			else if((i == 0) || (i == high ))
				maze[i][j] = x_line;
			else if(i != 0 && i != high && j != 0 && j != width)
			{
				if(maze[i][j] == x_line || maze[i][j] == y_line)
					maze[i][j] = space;
			}
		}
	}
	
	if(3 == level)												 
	{
		int x1 = high / 3;
		int x2 = x1 << 1;
		int y1 = width / 5;
		int y2 = 4 * y1;
		for(int i = 0; i <= high; i++) 
		{
			for(int j = 0; j <= width; j++)
			{
				if((j >= y1) && (j <= y2))
				{
					if(i == x1 || i == x2)
						maze[i][j] = x_line;
				}
			}
		}	
	}
	else if(4 == level)
	{
		int x1 = high / 5;
		int x2 = 4 * x1;
		int y1 = width / 5;
		int y2 = 4 * y1;
		for(int i = 0; i <= high; i++) 
		{
			for(int j = 0; j <= width; j++)
			{
				if((j == width >> 1) && (i >= x1) && (i <= x2))
				{
					maze[i][j] = y_line;
				}
				else if((i == high >> 1) && (j >= y1) && (j <= y2))
				{
					maze[i][j] = x_line;
				}
			}
		}
	}
	else if(5 == level)
	{
		int x1 = high / 3;
		int x2 = x1 << 1;
		int y1 = width / 3;
		int y2 = y1 << 1;
		for(int i = 1; i <= high; i++) 
		{
			for(int j = 1; j < width; j++)
			{
				if((i == high >> 1) && ((j <= y1) || (j >= y2)))
				{
					maze[i][j] = x_line;
				}
				else if((j == width >> 1) && ((i <= x1) || (i >= x2)))
				{
					maze[i][j] = y_line;
				}
			}
		}
	}
}
//得分引起的改变： 
//包括加分、加速度、进入下一关、以及结束 
void Snake::updateByScore()				 
{
	score++;
	if(mode == CLASSICALMODE)									//经典模式下得分引起的速度增加 
	{
		if(score && score % SCOREPERSPEED == 0 && speed <= 10)
		{
			speed++;
			delay = INITDELAY - speed * SECONDPERSPEED;
		}
	}
	else if(mode == LEVELMODE)									//闯关模式下得够一定分进入下一关卡 
	{
		if(score % SCOREPERLEVEL == 0 && score)
		{
			level++;
			if(level <= 5)
			{
				changeLevel();
				showGame();
				sleep(1);
			}
			else
			{
				showEnd();										//通关，游戏结束 
				end = true;
			}
		}
	}
}
//添加一个食物，会引起变色 
void Snake::addFood()					
{
	changeColor();												//吃一个变一次色 
	printf("\a");												//吃到时音效题型 
	updateByScore();											//加分及相关 
//	maze[food_y][food_x] == space;								//当前食物位置为空 
	last_food_x = food_x;										//将最近被吃的食物存储起来用于判断何时长度加一
	last_food_y = food_y;		 
	while(1)													//找一个不冲突的位置生成新的食物 
	{
		food_x = rand() % (width -1) + 1;
		food_y = rand() % (high - 1) + 1;
		if( maze[food_y][food_x] == space) break;
	}
	maze[food_y][food_x] = food;								//放置食物 
}
//蛇的长度加一 
void Snake::growUp()
{
	snakeNode *temp = new snakeNode;
	for(int i = 0; i < 4; i++)
	{
		if(next == nextMove[i].move)							//由最后一个节点的位置和移动方向计算出新节点位置 
		{
			temp->x = snake.next->x - nextMove[i].x;
			temp->y = snake.next->y - nextMove[i].y;
			temp->mov = snake.next->mov;
			temp->next = snake.next;
			snake.next = temp;
			maze[temp->y][temp->x] = snake_body;
			return;
		}	
	}
}
//改变蛇的位置，通过把尾巴移动到蛇头下一步要走的节点实现 
void Snake::changePosition()			
{
	for(int i = 0; i < 4; i++)
	{
		if(next == nextMove[i].move)
		{
			int x, y;
			if(level == 1)										//若是经典模式或者闯关第一关 ，则可穿墙 
			{
				x = (head->x + nextMove[i].x + width) % width;
				if(!x)	x = (x + nextMove[i].x + width) % width;
				y = (head->y + nextMove[i].y + high) % high;
				if(!y)	y = (y + nextMove[i].y + high) % high;
			}
			else												//反之不能 
			{
				x = head->x + nextMove[i].x;
				y = head->y + nextMove[i].y;
			}
																
			if(maze[y][x] == snake_body || maze[y][x] == x_line || maze[y][x] == y_line)		
			{													//咬到自己或撞到障碍就算死 
				end = true;
				showEnd();										//游戏结束闪烁 
				return;
			}
			maze[snake.next->y][snake.next->x] = space;
			snake.next->x = x;
			snake.next->y = y;
			maze[snake.next->y][snake.next->x] = snake_head;
			maze[head->y][head->x] = snake_body;
			head->next = snake.next;
			head = head->next;
			snake.next = head->next;
			head->next = NULL;
			head->mov = next;
			return;
		}
	}
}
//处理输入 
void Snake::updateInput()			
{
	if(kbhit())													//判断用户是否用户输入 
	{
		char input; 
		input = getch();
		if(input == space)										//输入空格键暂停 
		{
			pause = !pause;
		}
		if(pause) return;
		bool flag = false;
		for(int i = 0; i < 4; i++)								//下一个运动方向不能为当前方向的反方向 
		{
			if(input == nextMove[i].move && next != nextMove[3-i].move)
			{
				next = input;
				flag = true;
				return;
			}
			else if(input + 32 == nextMove[i].move && next != nextMove[3-i].move)
			{													//支持大小写 
				next = input + 32;
				flag = true;
				return;
			}
		}
	}
}
//游戏的主运行函数， 各种数据的判断和更新
void Snake::gameRun()		 
{
	showGame();													//显示游戏界面 
	usleep(delay);												//延时以控制蛇的速度 
	updateInput();												//输入
	if(!pause)													//判断是否暂停 
	{
		 
		if((head->x == food_x) && (head->y == food_y))			//判断是否吃到食物 
		{
			addFood();											//生成食物 
		}
		if((snake.next->x == last_food_x) && (snake.next->y == last_food_y))  
		{														//是否消化完成，即上一个食物已到尾巴处 
			growUp();											//消化完成长度加一 
			if(level > 1)										//修复可能被破坏的游戏界面 
				changeLevel();
			last_food_x = -1;									//消化完置为 -1 防止重复判断 
			last_food_y = -1;
		}
		changePosition();										//改变蛇的位置 
	}
}
//是否结束
bool Snake::isEnd()						 
{
	return end;
}

int main()
{
	srand(unsigned(time(0)));									//生成随机数的种子 
	HideCursor();												//隐藏光标避免屏幕闪烁 
	
	while(1)
	{
		Snake snake;
		snake.showBegin();
		snake.showRecord(); 
		while(!snake.isEnd())
		{
			snake.gameRun();
		}
	}
	
	return 0;
}
/**************END OF FILE*************************************/

