/*********************����˵��**********************************
�����ն�̰������Ϸ 
���ߣ�Memory
2017/1/9 16:00   Version 1.0 
2017/1/9 23:08   Version 1.5	������ͣ���ܡ�����ԭ�ػ�ͷ���Ե��Լ��������� 
								���ŷ��������ٶȻ��� ���ı���ͷ��״
2017/1/14 00:31  Version 2.0    �ö�ά����������ع�����չ�Ը�ǿ�� 
								������Ϸ��ʼ���棬�������ֿ�ѡģʽ�� 
								�÷ֿɱ���ɫ 
2017/1/14 11:53  Version 2.5    ����ؿ�
2017/1/14 17:25  Version 2.6
2017/1/14 23:56  Version 3.0    �޸������ٶȸ��������뵼�������� bug����ϵͳ��ʱ�����Լ��ĺ��� 
								����С�Ż������Ӵ����ĺ궨�塢������ע�ͣ���ǿ����ɶ��� 
*********************ͷ�ļ�************************************/
#include<cstdio>
#include<conio.h>												// getch() 
#include<cstdlib>												// rand()
#include<windows.h>												// system()��HideCursor()�� gotoxy(int x,int y)
#include<unistd.h>												// sleep() ��usleep() 
#include<ctime>													// time()
/*******************�����Ķ���*********************************/ 

#define WIDTH          35										// ��Ϸ����ĳ�ʼ��� 
#define HIGH           25 										// ��Ϸ����ĳ�ʼ�߶� 
#define LENGTH         5										// �ߵĳ�ʼ���� 
#define INITSPEED      1										// �ߵĳ�ʼ�ٶ� 
#define CLASSICALMODE  '1'										// ����ģʽ 
#define LEVELMODE      '2'										// ����ģʽ 
#define SECONDPERSPEED 30000									// ��ʼ��ʱʱ�� ����λ�ǰ����֮һ�룬��΢�� 
#define INITDELAY      300200									// ÿ����һ���ٶȼ��ٵ���ʱʱ�� 
#define SCOREPERLEVEL  8										// ��һ����Ҫ�ĵ÷� 
#define SCOREPERSPEED  5										// ����һ���ٶ���Ҫ�ĵ÷� 

const char left = 'a';											//	�ߵ��˶����� 
const char right = 'd';											//
const char up = 'w';											//
const char down = 's';											//
const char snake_body = '$';									// �ߵ����� 
const char snake_head = '@';									// ��ͷ 
const char food = '*'; 											// ʳ�� 
const char space = ' ';											// �ո� 
const char x_line = '_';
const char y_line = '|';

/****************���뿪ʼ**************************************/

struct NextMove{												//����η�λ�ı������ 
	char move; 
	int x;														// x, y �ֱ��Ǻ��������ĸı��� 
	int y;
}nextMove[5] = {{up, 0, -1}, {left, -1, 0}, {right, 1, 0}, {down, 0, 1}};

void HideCursor()												//���ع�꺯�� ,�ٶ����ҵ� 
{
	CONSOLE_CURSOR_INFO cursor_info = {1, 0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void gotoxy(int x,int y)										//��������������������ƶ��� x, yλ�ý����ػ�
{																// x ���У� y ���� 
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(handle,pos);
}

class Snake{
	private:
		char **maze;											//��Ϸ����Ķ�ά����ָ��
		struct snakeNode{
			int x;												//�ߵ�λ�õ� x ���� 
			int y;												//�ߵ�λ�õ� y ����
			char mov;											//�ߵ�ǰ���ƶ����� 
			snakeNode *next;										
		}snake, *head;											//��ͷָ����β�� head ָ����ͷ 
		int width;												//��Ϸ����Ŀ��
		int high;												//��Ϸ����ĸ߶�
		int length;												//�ߵĳ��� 
		int food_x, food_y;										//ʳ���λ��
		int last_food_x, last_food_y;							//��һ��ʳ���λ��  
		int score;												//��õķ��� 
		char next;												//����һ���ķ���
		int delay;												//ģ��ʱ�� 
		int speed;												//�ߵ��ٶ�
		int level;												//Ŀǰ�Ĺؿ��� 
		bool pause;												//�Ƿ���ͣ 
		bool end;												//�Ƿ���� 
		char mode;												//��Ϸģʽ��1Ϊ�޾�ģʽ��2Ϊ����ģʽ 
		FILE *fpScore, *fpSpeed;								//�洢��߼�¼���ļ� ��"score.txt" �� "speed.txt" 
	public:
		Snake();												//��ʼ��
		void creatFile();										//������ֹ��߼�¼���ļ� 
		void creatSnake();										//����һ���� 
		void creatMap();										//�����յ�ͼ 
		void showGame();										//��ʾ��Ϸ����
		void showBegin();										//��ʾѡ���ʼ����
		void showRecord();										//��ʾ��߼�¼
		void showEnd();											//��ʾ��Ϸ��������˸����
		void changePosition();									//���ݷ���ı��ߵ�λ�� 
		void changeLevel(); 									//�ı䵱ǰ�ؿ�
		void changeColor();										//�ı������ɫ 
		void updateByScore();									//��������ĸı�
		void updateInput();										//��������ص����ݸ���
		void updateRecord();									//������Ϸ����߼�¼
		void growUp();											//��������ʳ�ﳤ�ȼ�һ 
		void addFood();											//�������һ��ʳ�����һЩ���ݸ��� 
		void gameRun();											//���е���Ҫ����  
		bool isEnd();											//��Ϸ�Ƿ����  
};

//�����Ĺ��캯�������������һЩ���ݵĳ�ʼ��
//������һ������ʱ����������߼�¼���ļ�
//�ʹ���һ����ʼ����Ϊ length ����(�������ʾ�� 
Snake::Snake()													
{
	width = WIDTH;												// ��Ϸ����Ĵ�С 
	high = HIGH;												// 
	length = LENGTH;											//�ߵĳ�ʼ���� 
	speed = INITSPEED;											//��ʼ�ٶ� 
	delay = INITDELAY - speed * SECONDPERSPEED;					//�ӳٵ����ã����ٶȳɷ��� 
	score = 0;													// 
	level = 1;													// 
	end = false;												// 
	pause= false; 												// 
	food_x = width >> 1;	  									//ʳ��ĳ�ʼλ�ù̶� 
	food_y = high >> 2;
	next = right; 	 											//�ߵĳ�ʼ�ƶ�����̶�Ϊ���� 
	creatMap();													// 
	creatSnake();												// 
	creatFile();												// 
	maze[food_y][food_x] = food;								//����ʳ�� 
}
//������ʼ��ͼ 
void Snake::creatMap()
{
	maze = new char*[high+1];									//������ά�����ʾ��Ϸ����
	for(int i = 0; i <= high; i++)								//�հ׽������� 
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

//����Ƿ��д洢��߼�¼���ļ�����û���򴴽� 
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

//�������ʼ��һ������Ϊ length ���ߣ���ͷ����β���洢�ߵ�������Ϣ  
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
			head = temp;										//��ͷָ���β			
		}
	}
}
//��ʾ��ʼ���棬�����ѡ����Ϸģʽ
void Snake::showBegin()											 
{
	system("cls");
	gotoxy(10, 5);
	printf("��ӭ����̰ʳ����Ϸ!\n");
	printf("\n          ��w,a,s,d�����ƶ�����,�ո����ͣ��\n");
	printf("\n          ��ѡ����Ϸģʽ(������): \n");
	printf("\n            1������ģʽ\n");
	printf("\n            2������ģʽ\n");
	char m = getch();											
	while( (m != CLASSICALMODE) && (m != LEVELMODE))			//�ظ�����ֱ����ȷ 
		m = getch();
	mode  = m;
	
	if(mode == LEVELMODE)										//��ѡ���޾�ģʽ�����ѡ���ٶ� 
	{
		printf("\n          �������ٶ�(1~10): ");
		int s;
		scanf("%d", &s);
		if(s < 1) s = 1;										//�ٶ����Ϊ 1  
		else if(s > 10) s = 10;									//���Ϊ 10
		speed = s;
		delay = INITDELAY - speed * SECONDPERSPEED;
	} 
	system("cls");
}
//��ʾ����Ϸ���� 
void Snake::showGame()						
{
	gotoxy(0,1);												//ÿ���ƶ����س�ʼλ�� 

	for(int i = 0; i <= high; i++)								//��ʾ���� 
	{
		for(int j = 0; j <= width; j++)
		{
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
	printf("Score: %d",score);									//��ʾ��ǰ���� 
	for(int i = 0; i < width - 18; i++)
		printf(" ");
	if(mode == CLASSICALMODE)									//����ģʽѡ����ʾ�ؿ������ٶ� 
	{
		printf("Speed: %d\n",speed);
	}
	else if(mode == LEVELMODE)
	{
		printf("Level: %d\n",level);
	}
}
//��ʾ��߼�¼�����ļ��л�ȡ���� 
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
//��ʾ��Ϸ����ʱ�Ļ��� 
void Snake::showEnd()
{
	updateRecord();
	if(end)														//����ײ���ϰ�����������˸���� 
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
	else														//����ͨ�ؽ���������ʾ������ 
	{
		pause = true;
		gotoxy(10, 7);
		printf("�㾹Ȼ��ͨ����!\n");
		if(speed < 10)
			printf("\n       �б��°��ٶȵ���������һ��!\n"); 
		else
			printf("\n          ����������Ǹ����\n");
	}
	
	bool x = true;
	while(!kbhit())												//��˸��ʾ��������������� 
	{
		gotoxy(10, 13);
		usleep(500000);
		if(x)
			printf(" ����������� ");
		else
			printf("               ");
		x = !x;
	}	
}
//��Ϸ����ʱ������߼�¼�����µ��ļ��� 
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
//�ı�������ɫ����ɫΪ��ɫ���䣬������ɫ����ı�
//system("color xy") �У�x Ϊ��ɫ�� y Ϊ������ɫ����Χ��16���� 0 ~ e 
void Snake::changeColor()											 
{
	char color[20] = "color 00";
	char x = rand() % 15 + 1;
	if(x <= 9)	x += '0';
	else 		x += left - 10;
	color[7] = x;
	system(color);
} 

//����ģʽ�¸ı�ؿ�����ͬ�ؿ���ʾ��ͬ�ϰ� 
void Snake::changeLevel()
{
	for(int i = 0; i <= high; i++)								//ÿ�ζ����ñ߿��Լ������һ�ص��ϰ� 
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
//�÷�����ĸı䣺 
//�����ӷ֡����ٶȡ�������һ�ء��Լ����� 
void Snake::updateByScore()				 
{
	score++;
	if(mode == CLASSICALMODE)									//����ģʽ�µ÷�������ٶ����� 
	{
		if(score && score % SCOREPERSPEED == 0 && speed <= 10)
		{
			speed++;
			delay = INITDELAY - speed * SECONDPERSPEED;
		}
	}
	else if(mode == LEVELMODE)									//����ģʽ�µù�һ���ֽ�����һ�ؿ� 
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
				showEnd();										//ͨ�أ���Ϸ���� 
				end = true;
			}
		}
	}
}
//���һ��ʳ��������ɫ 
void Snake::addFood()					
{
	changeColor();												//��һ����һ��ɫ 
	printf("\a");												//�Ե�ʱ��Ч���� 
	updateByScore();											//�ӷּ���� 
//	maze[food_y][food_x] == space;								//��ǰʳ��λ��Ϊ�� 
	last_food_x = food_x;										//��������Ե�ʳ��洢���������жϺ�ʱ���ȼ�һ
	last_food_y = food_y;		 
	while(1)													//��һ������ͻ��λ�������µ�ʳ�� 
	{
		food_x = rand() % (width -1) + 1;
		food_y = rand() % (high - 1) + 1;
		if( maze[food_y][food_x] == space) break;
	}
	maze[food_y][food_x] = food;								//����ʳ�� 
}
//�ߵĳ��ȼ�һ 
void Snake::growUp()
{
	snakeNode *temp = new snakeNode;
	for(int i = 0; i < 4; i++)
	{
		if(next == nextMove[i].move)							//�����һ���ڵ��λ�ú��ƶ����������½ڵ�λ�� 
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
//�ı��ߵ�λ�ã�ͨ����β���ƶ�����ͷ��һ��Ҫ�ߵĽڵ�ʵ�� 
void Snake::changePosition()			
{
	for(int i = 0; i < 4; i++)
	{
		if(next == nextMove[i].move)
		{
			int x, y;
			if(level == 1)										//���Ǿ���ģʽ���ߴ��ص�һ�� ����ɴ�ǽ 
			{
				x = (head->x + nextMove[i].x + width) % width;
				if(!x)	x = (x + nextMove[i].x + width) % width;
				y = (head->y + nextMove[i].y + high) % high;
				if(!y)	y = (y + nextMove[i].y + high) % high;
			}
			else												//��֮���� 
			{
				x = head->x + nextMove[i].x;
				y = head->y + nextMove[i].y;
			}
																
			if(maze[y][x] == snake_body || maze[y][x] == x_line || maze[y][x] == y_line)		
			{													//ҧ���Լ���ײ���ϰ������� 
				end = true;
				showEnd();										//��Ϸ������˸ 
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
//�������� 
void Snake::updateInput()			
{
	if(kbhit())													//�ж��û��Ƿ��û����� 
	{
		char input; 
		input = getch();
		if(input == space)										//����ո����ͣ 
		{
			pause = !pause;
		}
		if(pause) return;
		bool flag = false;
		for(int i = 0; i < 4; i++)								//��һ���˶�������Ϊ��ǰ����ķ����� 
		{
			if(input == nextMove[i].move && next != nextMove[3-i].move)
			{
				next = input;
				flag = true;
				return;
			}
			else if(input + 32 == nextMove[i].move && next != nextMove[3-i].move)
			{													//֧�ִ�Сд 
				next = input + 32;
				flag = true;
				return;
			}
		}
	}
}
//��Ϸ�������к����� �������ݵ��жϺ͸���
void Snake::gameRun()		 
{
	showGame();													//��ʾ��Ϸ���� 
	usleep(delay);												//��ʱ�Կ����ߵ��ٶ� 
	updateInput();												//����
	if(!pause)													//�ж��Ƿ���ͣ 
	{
		 
		if((head->x == food_x) && (head->y == food_y))			//�ж��Ƿ�Ե�ʳ�� 
		{
			addFood();											//����ʳ�� 
		}
		if((snake.next->x == last_food_x) && (snake.next->y == last_food_y))  
		{														//�Ƿ�������ɣ�����һ��ʳ���ѵ�β�ʹ� 
			growUp();											//������ɳ��ȼ�һ 
			if(level > 1)										//�޸����ܱ��ƻ�����Ϸ���� 
				changeLevel();
			last_food_x = -1;									//��������Ϊ -1 ��ֹ�ظ��ж� 
			last_food_y = -1;
		}
		changePosition();										//�ı��ߵ�λ�� 
	}
}
//�Ƿ����
bool Snake::isEnd()						 
{
	return end;
}

int main()
{
	srand(unsigned(time(0)));									//��������������� 
	HideCursor();												//���ع�������Ļ��˸ 
	
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

