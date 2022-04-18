#include <stdio.h>

#include <time.h>

#include <windows.h>

#include <process.h>

#include <stdlib.h>

#include <conio.h>

#define mWIDTH 50

#define mHEIGHT 50

#define JUMPSPEED -300

#define MOVESPEED 3

#define MAP_WIDTH 1000

#define MAP_HEIGHT 1000

#define g 9.8

#define TOTAL_STAGE 10

#define TOTAL_FLOOR 10

HWND hwnd;

HDC hdc;

typedef struct {

	int x, y;

	int v;

}character;

enum floorType {

	normal = 0, up_down, side_to_side, conveyor_left, conveyor_right, fall, obstacle

};

typedef struct {

	int x, y, width, height;

	enum floorType fType;

	int range;

}position;

position floor_init[TOTAL_STAGE][TOTAL_FLOOR];

//position floor_init[TOTAL_STAGE][TOTAL_FLOOR] = {

//	{ { 0,600,100,30,normal },{ 150,500,100,30,up_down },{ 300,600,100,30,side_to_side },{ 450,500,200,30,conveyor_left },{ 600,400,200,30,conveyor_right },{ 750,600,100,30,fall },{ 900,500,10,100,obstacle } },//stage1

//{ { 0,500,100,30 },{ 150,500,100,30 },{ 300,500,100,30 },{ 450,500,100,30 },{ 650,500,100,30 },{ 850,600,100,30 } }//stage2

//};//�����̴� ���� ��� ��ǥ�� ��� �ٲ� �ʱ� ���� ��Ⱑ ����--> ����ü ��ǥ �����Ѱ� ���������� ����!

int check_touch_something(character ME, position* object)

{

	if ((ME.x + mWIDTH) < object->x || (object->x + object->width) < ME.x)

		return FALSE;

	if ((ME.y + mHEIGHT) < object->y || (object->y + object->height) < ME.y)

		return FALSE;

	return TRUE;

}

void draw_me(int x, int y)

{

	HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));//�귯�� ����

	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush); //�귯�� ����

	Ellipse(hdc, x, y, x + mWIDTH, y + mHEIGHT); //���

	SelectObject(hdc, oldBrush);//���� �귯�� ����

	DeleteObject(myBrush);//�귯�� ����

}

void clear_all_screen(void)

{

	Rectangle(hdc, 0, 0, MAP_WIDTH, MAP_HEIGHT);

}

void clear_before_image(int x, int y, int WIDTH, int HEIGHT)

{

	HPEN myPen = CreatePen(PS_NULL, 0, RGB(255, 255, 255));//���� �׵θ� ���ֱ�

	HGDIOBJ oldPen = SelectObject(hdc, myPen);

	Rectangle(hdc, x - 1, y - 1, x + WIDTH + 1, y + HEIGHT + 1);

	SelectObject(hdc, oldPen);

	DeleteObject(myPen);

}

int check_on_floor(character* ME, position FLOOR)

{

	int xc = (ME->x + (ME->x + mWIDTH)) / 2;

	if ((ME->y + mHEIGHT < FLOOR.y + 10) && (ME->y + mHEIGHT >= FLOOR.y) && (xc < FLOOR.x + FLOOR.width) && (xc > FLOOR.x))

	{

		ME->y = FLOOR.y - mHEIGHT;//���� �ӵ��� ���� �� ������ �Ʒ��� ���ߴ� �� ����. ������ ����ؼ� ���� y��ǥ�� ���� y��ǥ�� �ٲ�.

		return TRUE;

	}

	else

		return FALSE;

}

int all_check_on_floor(character* ME, position FLOOR[][10], int stage, int size)

{

	for (int num = 0;num < size;num++)

	{

		if ((check_on_floor(ME, FLOOR[stage][num]) == TRUE))

		{

			return TRUE;

		}

		else

			continue;

	}

	return FALSE;

}

void up_down_floor(int* f_y, int STAGE, int NUM, int RANGE)

{

	static int flag = 0;

	if (flag == 0)

	{

		*f_y -= 1;

		if (*f_y == floor_init[STAGE][NUM].y - RANGE)

			flag = 1;

	}

	else

	{

		*f_y += 1;

		if (*f_y == floor_init[STAGE][NUM].y + RANGE)

			flag = 0;

	}

}



void side_to_side_floor(position* FLOOR, character* ME, int STAGE, int NUM)//���������ͷ� ���ο��� ���� y��ǥ�� �ٲ�

{

	static int flag = 0;

	if (flag == 0)

	{

		FLOOR->x -= 1;



		if ((check_on_floor(ME, *FLOOR) == TRUE))

		{

			ME->x -= 1;

		}

		if (FLOOR->x == floor_init[STAGE][NUM].x - FLOOR->range)

			flag = 1;

	}

	else

	{

		FLOOR->x += 1;

		if ((check_on_floor(ME, *FLOOR) == TRUE))

		{

			ME->x += 1;

		}

		if (FLOOR->x == floor_init[STAGE][NUM].x + FLOOR->range)

			flag = 0;

	}

}

void conveyor_left_floor(position* FLOOR, character* ME)

{

	if ((check_on_floor(ME, *FLOOR) == TRUE))

	{

		ME->x -= 1;

	}

}

void conveyor_right_floor(position* FLOOR, character* ME)

{

	if ((check_on_floor(ME, *FLOOR) == TRUE))

	{

		ME->x += 1;

	}

}

void fall_floor(position* FLOOR, character* ME, int STAGE, int NUM)

{

	static int time = 0;

	if ((check_on_floor(ME, *FLOOR) == TRUE))

	{

		time++;

		FLOOR->y += (((floor_init[STAGE][NUM].y) + ((g * (time + 1) * (time + 1)) / 2)) - ((floor_init[STAGE][NUM].y) + ((g * time * time) / 2))) / 50;

	}

	if (ME->y > 2 * MAP_HEIGHT)//ȭ�� ������ �ָ� ��������

	{

		FLOOR->y = floor_init[STAGE][NUM].y;

		time = 0;

	}

}

void obstacle_floor(position* FLOOR, character* ME, int stage)

{

	if (check_touch_something(*ME, FLOOR) == TRUE)

	{

		ME->x = floor_init[stage][0].x;

		ME->y = floor_init[stage][0].y - (mHEIGHT * 2);

	}

}

void draw_floor(position* FLOOR, character* ME, int STAGE, int NUM)

{

	int R = 0, G = 0, B = 255;

	if (FLOOR->fType == up_down)

	{

		R = 0, G = 255, B = 0;

		clear_before_image(FLOOR->x, FLOOR->y, FLOOR->width, FLOOR->height);

		up_down_floor(&FLOOR->y, STAGE, NUM, FLOOR->range);

	}

	else if (FLOOR->fType == side_to_side)

	{

		R = 0, G = 255, B = 255;

		clear_before_image(FLOOR->x, FLOOR->y, FLOOR->width, FLOOR->height);

		side_to_side_floor(FLOOR, ME, STAGE, NUM);

	}

	else if (FLOOR->fType == conveyor_left)

	{

		R = 255, G = 255, B = 0;

		conveyor_left_floor(FLOOR, ME);

	}

	else if (FLOOR->fType == conveyor_right)

	{

		R = 255, G = 0, B = 255;

		conveyor_right_floor(FLOOR, ME);

	}

	else if (FLOOR->fType == fall)

	{

		R = 200, G = 200, B = 200;

		clear_before_image(FLOOR->x, FLOOR->y, FLOOR->width, FLOOR->height);

		fall_floor(FLOOR, ME, STAGE, NUM);

	}

	else if (FLOOR->fType == obstacle)

	{

		R = 0, G = 0, B = 0;

		obstacle_floor(FLOOR, ME, STAGE);

	}

	else

	{

		R = 0, G = 0, B = 255;

	}

	HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(R, G, B));//�귯�� ����

	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush); //�귯�� ����

	Rectangle(hdc, FLOOR->x, FLOOR->y, FLOOR->x + FLOOR->width, FLOOR->y + FLOOR->height);//�� �׸���

	SelectObject(hdc, oldBrush);//���� �귯�� ����

	DeleteObject(myBrush);//�귯�� ����



	SetBkColor(hdc, RGB(R, G, B));//�ؽ�Ʈ ����

	SetTextAlign(hdc, TA_CENTER);//��� ����

	TCHAR str[10][3] = { TEXT(" "),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"), TEXT("!") };//����ȭ�� ���ڿ��� ����� ����

	TextOut(hdc, (((FLOOR->x) + (FLOOR->x + FLOOR->width)) / 2), (((FLOOR->y) + (FLOOR->y + FLOOR->height)) / 2) - 7, str[FLOOR->fType], lstrlen(str[FLOOR->fType]));

	//�� �߰��� ���� �ĺ��� �� �ִ� �ؽ�Ʈ ���

}

void draw_portal(position* PORTAL)

{

	HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 255, 255));//�귯�� ����

	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush); //�귯�� ����

	RoundRect(hdc, PORTAL->x, PORTAL->y, PORTAL->x + PORTAL->width, PORTAL->y + PORTAL->height, 30, 30);

	SelectObject(hdc, oldBrush);//���� �귯�� ����

	DeleteObject(myBrush);//�귯�� ����

}





void gotoxy(int x, int y)

{

	COORD coord = { 0, 0 };

	coord.X = x; coord.Y = y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

}

void print_score(int stage, int x, int y)



{

	TCHAR str[100];//����ȭ�� ���ڿ��� ����� ����

	wsprintf(str, TEXT("stage : %d x��ǥ : %d y��ǥ : %d"), stage, x, y);//����ȭ

	TextOut(hdc, 50, 50, str, lstrlen(str));

}

int main(void)

{

	hwnd = GetForegroundWindow();

	hdc = GetWindowDC(hwnd);

	//HFONT hFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("�ü�"));





	character me = { 0 , 100, 0 };//x��ǥ,y��ǥ,�ӵ�



	position floor[TOTAL_STAGE][TOTAL_FLOOR] = {

		{ { 0,600,100,30,normal },{ 150,500,100,30,up_down,50 },{ 300,600,100,30,side_to_side,50 },{ 450,500,200,30,conveyor_left },{ 600,400,200,30,conveyor_right },{ 750,600,100,30,fall },{ 900,500,10,100,obstacle } },//stage1

	{ { 0,500,100,30 },{ 150,500,100,30 },{ 300,500,100,30 },{ 450,500,100,30 },{ 650,500,100,30 },{ 850,600,100,30 } }//stage2

	};//������� x��ǥ,y��ǥ,�ʺ�,����,Ÿ��



	memcpy(floor_init, floor, sizeof(position[TOTAL_STAGE][TOTAL_FLOOR]));//floor����ü �迭�� �ʱ� ���� ���������� floor_init ����ü �迭�� ����!

	position portal[TOTAL_STAGE] = {

		{ 775, 150, 50, 50 },{ 875, 550, 50, 50 }

	};

	int time = 0;

	int vi = 0;

	int yi = 0;

	int stage = 0;

	for (int stage = 0;stage < TOTAL_STAGE;stage++)

	{

		clear_all_screen();

		/*TextOut(hdc, 300, 300, "zzz", strlen("zzz"));

		SelectObject(hdc, oldFont);

		DeleteObject(myFont);*/

		me.x = floor_init[stage][0].x;

		me.y = floor_init[stage][0].y - (mHEIGHT * 2);

		draw_me(me.x, me.y);

		while (1)

		{

			print_score(stage + 1, me.x, me.y);

			clear_before_image(me.x, me.y, mWIDTH, mHEIGHT);

			for (int num = 0;num < TOTAL_STAGE; num++)

			{

				draw_floor(&floor[stage][num], &me, stage, num);

			}

			if (me.y > 2 * MAP_HEIGHT)//ȭ�� ������ �ָ� ��������

			{

				me.x = floor[stage][0].x;

				me.y = floor[stage][0].y - (mHEIGHT * 2);

				me.v = 0;

				time = 0;

				continue;

			}

			if (all_check_on_floor(&me, floor, stage, TOTAL_FLOOR) == FALSE)

			{

				clear_before_image(me.x, me.y, mWIDTH, mHEIGHT);

				time++;

				me.y += ((yi + (vi * (time + 1)) + ((g * (time + 1) * (time + 1)) / 2)) - (yi + (vi * time) + ((g * time * time) / 2))) / 50;

				//50���� �ȳ����� �ε巴�� �ʰ� �Ҷ� ����

				//me.y = ((yi+(vi*time) + ((g*time*time) / 2)))/50; ���� �ϸ� 30���� �����°� �ȿ� yi�� ���Եż� ���� �̻�����->�������� ��� ��������.

				me.v = vi + g * time;

			}

			else //if (check_over_floor(me.x, me.y, floor, stage, 10) == yes)

			{

				time = 0, me.v = 0, vi = 0, yi = me.y;

				if (GetAsyncKeyState(VK_SPACE) & 0x8000)

				{

					clear_before_image(me.x, me.y, mWIDTH, mHEIGHT);

					vi = JUMPSPEED;

					me.v = vi;

					me.y -= 3;//���� ���� �� �����̽��ٸ� ������ ��� ���� ���� ���� �ϰ� �߷��� �ް� �ϱ� ����

				}

			}

			if (GetAsyncKeyState(VK_LEFT))

			{

				clear_before_image(me.x, me.y, mWIDTH, mHEIGHT);

				me.x -= MOVESPEED;

			}

			else if (GetAsyncKeyState(VK_RIGHT))

			{

				clear_before_image(me.x, me.y, mWIDTH, mHEIGHT);

				me.x += MOVESPEED;

			}



			if (check_touch_something(me, &portal[stage]) == TRUE)

			{

				me.v = 0;

				time = 0;

				break;

			}

			draw_me(me.x, me.y);

			draw_portal(portal);

			//GetAsyncKeyState�Լ��� Ű �Է��� ��ٸ��� ����! ->_getch�Լ� �������� ���߿� ���� �� �¿� �̵��� �ȵ��� 

			Sleep(10);//1000clock=1�� , �ð������� 10clock 

		}

	}

	return 0;

}