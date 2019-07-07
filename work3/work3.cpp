#include "pch.h"
#include <iostream>
#include <cstdlib>
#include<ctime>
#include<string>
using namespace std;
//地图大小可通过size快速改变
#define size 8
//一些地图图标宏定义
#define lobby 4
#define near_mp -4
#define near_p -3
#define near_m -2
#define princess 3
#define monster 2
#define wall 1
#define road 0
#define known 0
#define unknown 1
int find_p = 0;//是否找到了公主，若为1则回到lobby则胜利
int map[size][size];//地图 
int wall_map[size][size];//生成地图时，用来记录固定不拆的墙 
int view_map[size][size];//视野 
int man_x, man_y;//人物坐标
int onemove[4][2] = { 1,0,0,1,-1,0,0,-1 };//用来在循环中简化前进时坐标变换
void initial_all(void);//初始化地图，并调用make_maze生成一张地图
void make_maze(int x, int y);//创建迷宫，即地图雏形
void print_map(int view_all);//打印地图
int place_p_m(void);//放置公主和怪物，并检测是否可以在不经过怪物的情况下找到公主，不符的话就重新再放置，或者重生成地图
int find_way(int x, int y, int pass[][size]);//找路函数
void room_detail(void); //生成房间提示，即靠近公主或怪物的提示
int  move(int x, int y);//移动函数
int main(void)
{
	int i;
	int flag;
	srand(time(0));
	while (1)
	{
		initial_all();
		//放置公主，怪物，若连续失败5次，就重新生成地图
		for (i = 1; i <= 5; i++)
		{
			if (place_p_m())break;
		}
		if (i <= 5)break;
	}
	room_detail();
	//移动
	flag = move(size - 1, size / 2);
	//游戏结束时打印全部地图
	print_map(1);
	if (flag)cout << endl << "You have take the princess to the lobby successfully." << endl << "You win.";
	else cout << endl << "You meet the monster.Game over. ";
	return 0;
}
void make_maze(int x, int y)
{
	map[x][y] = road;
	int i;
	int dir, turn;//控制前进方向
	int x1, y1, x2, y2, x3, y3;
	//对dir和turn进行随机，保证每次转弯随机，增强地图随机性
	dir = rand() % 4;
	turn = rand() % 2 * 2 + 1;
	//以回溯的思想来造路
	for (i = 0; i < 4; i++)
	{
		dir = (dir + turn) % 4;
		x1 = x + onemove[dir][0];//向当前方向前进 1格 
		y1 = y + onemove[dir][1];
		x2 = x + 2 * onemove[dir][0];//向当前方向前进 2格 
		y2 = y + 2 * onemove[dir][1];
		x3 = x1 + onemove[(dir + 1) % 4][0];//垂直方向 
		y3 = y1 + onemove[(dir + 1) % 4][1];
		if (x2 > size - 1 || y2 > size - 1 || x2 < 0 || y2 < 0)
		{
			continue;//防止越界及拆掉边界
		}
		//判断，如果下一格墙被拆会造成2条路联通，就被固定为墙，不能被拆
		if (map[x3][y3] == road || map[2 * x1 - x3][2 * y1 - y3] == road) {//防止田字型路段 
			wall_map[x1][y1] = 1;
		}
		//如果此格已经固定为墙，就不能再被拆
		if (wall_map[x1][y1] == 1)continue;
		//若所有条件满足，就进入下一格
		if (map[x2][y2] == wall) {
			make_maze(x1, y1);
		}
	}
}
void initial_all(void)
{
	int i, j;
	int x, y;
	//初始把所有格子都填满墙
	man_x = size - 1;
	man_y = size / 2;
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			map[i][j] = wall;
			wall_map[i][j] = 0;
			view_map[i][j] = unknown;
		}
	}
	map[size - 1][size / 2 - 1] = map[size - 1][size / 2 + 1] = map[size - 1][size / 2] = road;
	//随机化生成地图起始点
	y = rand() % 3 + size / 2 - 1;
	x = size - 1 - rand() % 2;
	make_maze(x, y);
	map[size - 2][size / 2 - 1] = map[size - 2][size / 2] = map[size - 2][size / 2 + 1] = road;
}
int place_p_m(void)
{
	int i, j;
	int count = 0;//记录路的数量，以便后面随机公主、怪物坐标
	int p_x, p_y, m_x, m_y;//公主、怪物坐标
	int recode[size*size];//记录路
	int pass[size][size];//记录这段路有没有走过
	int p_site, m_site;//公主和怪物位置
	int flag;
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			pass[i][j] = 0;
		}
	}
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			if (map[i][j] == road)
			{
				recode[count] = i * size + j;
				count++;
			}
		}
	}
	//随机生成公主、怪物坐标，并作初步检查
	//初步检查包括不离起点太近，且2者坐标不同
	p_site = rand() % count;
	p_x = recode[p_site] / size;
	p_y = recode[p_site] % size;
	while (p_x >= size - 3 && p_x <= size - 1 && p_y <= size / 2 + 2 && p_y >= size / 2 - 1)
	{
		p_site = rand() % count;
		p_x = recode[p_site] / size;
		p_y = recode[p_site] % size;
	}
	do
	{
		m_site = rand() % count;
		m_x = recode[m_site] / size;
		m_y = recode[m_site] % size;
	} while (m_x >= size - 3 && m_x <= size - 1 && m_y <= size / 2 + 1 && m_y >= size / 2 - 1 || m_site == p_site);
	p_x = recode[p_site] / size;
	p_y = recode[p_site] % size;
	map[p_x][p_y] = princess;
	m_x = recode[m_site] / size;
	m_y = recode[m_site] % size;
	map[m_x][m_y] = monster;
	//调用寻路函数，看看是否能在不经过怪物的情况下找到公主，若能返回1，不能就撤销地图上的变化，返回0
	flag = find_way(size - 1, size / 2, pass);
	if (flag)return 1;
	map[m_x][m_y] = map[p_x][p_y] = road;
	return 0;
}
//回溯，自动寻路，主要用来辅助放置place_p_m函数
int find_way(int x, int y, int pass[][size])
{
	int i;
	int newx, newy;
	int flag;
	//找到公主则返回1
	if (map[x][y] == princess)return 1;
	//经过一点就在pass上做出变化，防止后面绕圈圈陷入死循环
	pass[x][y] = 1;
	for (i = 0; i <= 3; i++)
	{
		newx = x + onemove[i][0];
		newy = y + onemove[i][1];
		//防止越界
		if (newx < 0 || newy < 0 || newx >= size || newy >= size)continue;
		//不走墙以及怪物
		else if (map[newx][newy] == wall || map[newx][newy] == monster)continue;
		//已经经过则不再走
		else if (pass[newx][newy] == 1)continue;
		else flag = find_way(newx, newy, pass);
		if (flag)return 1;
	}
	return 0;
}
void print_map(int view_all)
{
	system("cls");
	int i, j;
	for (j = 0; j != size; j++) {
		for (i = 0; i != size; i++) {
			cout << " --- ";
		}
		cout << endl;
		//根据地图信息、人物坐标、视野打印出地图
		for (i = 0; i != size; i++) {
			if (map[j][i] == wall)cout << "| ■|";
			else if (j == man_x && i == man_y && view_all == 0)cout << "| ♂|";
			else if (view_map[j][i] == unknown && view_all == 0)cout << "| ? |";
			else if (map[j][i] == princess)cout << "| ♀|";
			else if (map[j][i] == monster)cout << "| $ |";
			else if (map[j][i] == lobby)cout << "| ▲|";
			else  cout << "|   |";
		}
		if (j == 0)cout << "       - 空房间                              说明";
		if (j == 1)cout << "    ■ - 墙壁                1.你的坐标为最优先显示级;";
		if (j == 2)cout << "    ?  - 未探索房间          2.经过一个房间后,房间会变成已探索状态(问号消失);";
		if (j == 3)cout << "    $  - 怪物所处房间        3.所在房间靠近公主或者怪物时，会有相应提示;";
		if (j == 4)cout << "    ♀ - 囚禁公主的房间      4.遇到怪物、或带着公主回到lobby时游戏结束;";
		if (j == 5)cout << "    ♂ - 你所在房间          5.游戏结束时,会以全视野输出整张地图;";
		if (j == 6)cout << "    ▲ - lobby";
		cout << endl;
	}
	for (i = 0; i != size; i++) {
		cout << " --- ";
	}
}
//交互，接收用户指令的移动函数
int  move(int x, int y)
{
	int i, j;
	int newx, newy;
	//用来记录可以前进的方向的个数
	int ChoiceCount = 0;
	//用来记录四个方向是否能走通
	int all_dir[4] = { 0,0,0,0 };
	//2个字符串用来接收用户指令
	string choice;
	string go;
	int flag;
	string dir[4] = { "down","east","up","west" };
	string st[4];
	int ptr = 0;
	//将当前所在房间的视野打开
	view_map[x][y] = known;
	print_map(0);
	for (i = 0; i <= 3; i++)
	{
		newx = x + onemove[i][0];
		newy = y + onemove[i][1];
		//若以某方向前进1格越界、撞墙则此方向不通
		if (newx >= size || newy >= size || newx < 0 || newy < 0)continue;
		else if (map[newx][newy] == wall)continue;
		else
		{
			ChoiceCount++;
			all_dir[i] = 1;
		}
	}
	cout << endl;
	//根据房间类型打印相应信息
	if (map[x][y] == road)cout << "You enter a room. Nothing happen. " << endl;
	else if (map[x][y] == near_m )cout << "You enter a room. You hear the roar of the monster. " << endl;
	else if (map[x][y] == near_p )cout << "You enter a room. You found the princess's footprints. " << endl;
	else if (map[x][y] == near_mp)
	{
		cout << "You enter a room. " << endl << "You hear the roar of the monster. " << endl;
		cout << "And you found the princess's footprints. " << endl;
	}
	//到达lobby的时候，若已经找到过公主，则返回1
	else if (map[x][y] == lobby)
	{
		cout << "Welcome to the lobby. \n";
		if (find_p)return 1;
	}
	//遇到怪物则直接返回0
	else if (map[x][y] == monster)
	{
		cout << "You meet the monster.Game over. ";
		return 0;
	}
	//遇到公主，置find_p=1
	else if (map[x][y] == princess)
	{
		cout << "You find the princess.Please the castle soon. \n";
		find_p = 1;
	}
	cout << "There are " << ChoiceCount << " exits:";
	for (i = 0; i < 4; i++)
	{
		if (all_dir[i])st[ptr++] = dir[i];
	}
	if (ChoiceCount == 4)cout << " " << st[0] << ", " << st[1] << ", " << st[2] << " and " << st[3];
	else if (ChoiceCount == 3)cout << " " << st[0] << ", " << st[1] << " and " << st[2];
	else if (ChoiceCount == 2)cout << " " << st[0] << " and " << st[1];
	else if (ChoiceCount == 1)cout << " " << st[0];
	cout << endl;
	//接受用户指令并对不相符的指令做出回绝
	//符合要求的指令，则调用move函数继续前进
	while (1)
	{
		cout << "Enter your command:";
		cin >> go;
		cin >> choice;
		for (i = 0; i < 4; i++)
		{
			if (dir[i] == choice)break;
		}
		if (i == 4)cout << "Your command does not match the format." << endl;
		else if (all_dir[i] == 0)cout << "No way in this direction." << endl;
		else {
			man_x = newx = x + onemove[i][0];
			man_y = newy = y + onemove[i][1];
			flag = 0;
			flag = move(newx, newy);
			if (flag == 0)return 0;
			return 1;
		}
	}
	return 1;
}
//生成房间更多的细节
void room_detail(void)
{
	int m_x, m_y, p_x, p_y;
	int i, j;
	//找到公主和怪物的坐标
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			if (map[i][j] == monster)
			{
				m_x = i; m_y = j;
			}
			else if (map[i][j] == princess)
			{
				p_x = i; p_y = j;
			}
		}
	}
	//以公主、怪物为中心的九宫格内的房间，分别标记为near_p,near_m
	//特别注意的是若出现刚好有房间同时临近2者，就标记为near_mp
	for (i = -1; i <= 1; i++)
	{
		for (j = -1; j <= 1; j++)
		{
			if (map[m_x + i][m_y + j] == road)map[m_x + i][m_y + j] = near_m;
			else if (map[m_x + i][m_y + j] == near_p)map[m_x + i][m_y + j] = near_mp;
			if (map[p_x + i][p_y + j] == road)map[p_x + i][p_y + j] = near_p;
			else if (map[p_x + i][p_y + j] == near_m)map[p_x + i][p_y + j] = near_mp;
		}
	}
	map[size - 1][size / 2] = lobby;
}
