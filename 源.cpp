#include<stdio.h>
#include<graphics.h>  //easyx图形库头文件，需要安装easyx图形库
#include "tools.h"

#define WIN_WIDTH 900
#define WIN_HEIGHT 600

enum { WAN_DOU, KUI_HUA, SHI_REN_HUA, ZHI_WU_COUNT };  //植物卡牌枚举

IMAGE imgBG;  //定义背景图片
IMAGE imgBAR;  //定义植物商店
IMAGE imgCards[ZHI_WU_COUNT];  //定义植物卡牌数组
IMAGE *imgZhiWu[ZHI_WU_COUNT][20];  //指针数组，每个指针指向gif的每一帧

int curX, curY;  //当前选中的植物，在移动过程中的坐标
int curZhiWu = 0;  //0：没有选中， 1：选择了第一种植物

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
	memset(imgZhiWu, 0, sizeof(imgZhiWu));  //帧指针数组

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

	//渲染拖动过程中的图片
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth()/2, curY - img->getheight()/2, imgZhiWu[curZhiWu - 1][0]);
		//printf("%d\n", curZhiWu);
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
				//printf("%d\n", index);
			}

		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP) {

		}
	}




}


int main(void) {

	InitGame();

	while (1) {

		userClick();

		updateWindows();

	}

	system("pause");
	return 0;
}