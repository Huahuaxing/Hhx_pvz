#include<stdio.h>
#include<stdlib.h>
#include<graphics.h>  //easyx图形库头文件，需要安装easyx图形库
#include<time.h>
#include "tools.h"

#define WIN_WIDTH 900
#define WIN_HEIGHT 600

enum { WAN_DOU, KUI_HUA, SHI_REN_HUA, ZHI_WU_COUNT };  //植物卡牌枚举

IMAGE imgBG;  //定义背景图片
IMAGE imgBAR;  //定义植物商店
IMAGE imgCards[ZHI_WU_COUNT];  //定义植物卡牌数组
IMAGE *imgZhiWu[ZHI_WU_COUNT][20];  //指针数组，每个指针保存gif每一帧的地址

int curX, curY;  //当前选中的植物，在移动过程中的坐标
int curZhiWu = 0;  //0：没有选中， 1：选择了第一种植物， 应用于拖动植物

struct zhiwu {
	int type;		 //0：没有植物， 1：第一种植物
	int frameIndex;  //序列帧的序号
};

struct zhiwu map[3][9];  //应用于草地植物

struct sunshineBall {
	int x, y;  //阳光球在飘落过程中的坐标位置（x不变）
	int frameIndex;  //当前显示的图片帧序号
	int destY;  //飘落的目标位置的y坐标
	bool usageIndex; //是否在使用
};

struct sunshineBall balls[10];  //阳光池
IMAGE imgSunshineBall[29];

bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL)
		return false;
	else {
		fclose(fp);
		return true;
	}
}


void InitGame() {
	loadimage(&imgBG, "res/bg.jpg");  //加载背景
	loadimage(&imgBAR, "res/bar5.png");  //加载植物选择框

	memset(imgZhiWu, 0, sizeof(imgZhiWu));  //初始化帧指针数组

	memset(map, 0, sizeof(map));  //初始化草地植物数组

	char name[64];  //保存植物卡牌的路径
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//先判断gif的一帧是否存在
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else {
				break;
			}
		}
	}

	curZhiWu = 0;

	memset(balls, 0, sizeof(balls));  //初始化阳光池

	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}

	//配置随机种子


	initgraph(WIN_WIDTH, WIN_HEIGHT, 1);
}

void updateWindows() {

	BeginBatchDraw();

	putimage(0, 0, &imgBG);  //渲染背景

	putimagePNG(230, 0, &imgBAR);  //渲染植物商店

	//渲染植物卡牌
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		int x = 327 + i * 55  ;
		int y = 12;
		putimage(x, y, &imgCards[i]);
	}

	//渲染草地上种植的植物
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				int x = 256 + j * 81;
				int y = 179 + i * 102;
				int type = map[i][j].type - 1;
				int frameIndex = map[i][j].frameIndex;
				putimagePNG(x, y, imgZhiWu[type][frameIndex]);
			}
		}
	}

	//渲染拖动过程中的图片
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, imgZhiWu[curZhiWu - 1][0]);
	}

	EndBatchDraw();
}

void userClick() {
	ExMessage msg;
	static int status = 0;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {
			if (msg.x > 327 && msg.x < 327 + 55 * ZHI_WU_COUNT && msg.y > 12 && msg.y < 89) {
				int index = (msg.x - 327) / 55;
				status = 1;
				curZhiWu = index + 1;
				curX = msg.x;
				curY = msg.y;
			}

		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP) {
			if (msg.x > 256 && msg.y > 179 && msg.y < 480) {
				int row = (msg.y - 179) / 102;
				int col = (msg.x - 256) / 81;
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
				}
			}
			

			curZhiWu = 0;
			status = 0;
		}
	}




}

void createSunshine() {
	static int count = 0;
	static int fre = 400;
	count++;
	if (count >= fre) {
		fre = 200 + rand() % 200;
		count = 0;

		//从阳光池中取一个可以使用的
		int ballMax = sizeof(balls) / sizeof(balls[0]);

		int i;
		for (i = 0; i < ballMax && balls[i].usageIndex; i++)
			if (i >= ballMax) return;

		balls[i].usageIndex = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % (900 - 260);  //260 . 900
		balls[i].y = 60;
		balls[i].destY = 200 + (rand() % 4) * 90;//
	}
}

void updataGame() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int zhiWuType = map[i][j].type - 1;
				int zhiWuFrame = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][zhiWuFrame] == NULL)
					map[i][j].frameIndex = 0;
			}
		}
	}
}

void startMenu() {
	IMAGE MenuBG, MenuOP1, MenuOP11;
	loadimage(&MenuBG, "res/menu.png");
	loadimage(&MenuOP1, "res/menu1.png");
	loadimage(&MenuOP11, "res/menu2.png");

	bool flag = false;

	
	while (1) {
		BeginBatchDraw();

		putimage(0, 0, &MenuBG);
		putimagePNG(474, 75, flag ? &MenuOP11 : &MenuOP1);

		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN && msg.x > 475 && msg.x < 475 + 331 && msg.y > 75 && msg.y < 75 + 140) {
				flag = true;
				
			}
			if (msg.message == WM_LBUTTONUP && msg.x > 475 && msg.x < 475 + 331 && msg.y > 75 && msg.y < 75 + 140) {
				return;
			}
			else if (msg.message == WM_LBUTTONUP) {
				flag = false;
			}
		}
		EndBatchDraw();
	}

}


int main(void) {

	InitGame();

	int timer = 0;
	bool flag = true;

	//启动菜单
	//startMenu();

	//游戏内循环
	while (1) {

		userClick();

		timer += getDelay();

		if (timer > 50) {
			flag = true;
			timer = 0;
		}

		if (flag) {
			flag = false;
			updateWindows();
			updataGame();
		}

	}
	return 0;
}