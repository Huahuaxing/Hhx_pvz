#include<stdio.h>
#include<graphics.h>  //easyxͼ�ο�ͷ�ļ�����Ҫ��װeasyxͼ�ο�
#include "tools.h"

#define WIN_WIDTH 900
#define WIN_HEIGHT 600

enum { WAN_DOU, KUI_HUA, SHI_REN_HUA, ZHI_WU_COUNT };  //ֲ�￨��ö��

IMAGE imgBG;  //���屳��ͼƬ
IMAGE imgBAR;  //����ֲ���̵�
IMAGE imgCards[ZHI_WU_COUNT];  //����ֲ�￨������
IMAGE *imgZhiWu[ZHI_WU_COUNT][20];  //ָ�����飬ÿ��ָ��ָ��gif��ÿһ֡

int curX, curY;  //��ǰѡ�е�ֲ����ƶ������е�����
int curZhiWu = 0;  //0��û��ѡ�У� 1��ѡ���˵�һ��ֲ��

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
	loadimage(&imgBG, "res/bg.jpg");  //���ر���
	loadimage(&imgBAR, "res/bar5.png");  //����ֲ��ѡ���
	memset(imgZhiWu, 0, sizeof(imgZhiWu));  //ָ֡������

	char name[64];  //����ֲ�￨�Ƶ�·��
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//���ж�gif��һ֡�Ƿ����
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

	putimage(0, 0, &imgBG);  //��Ⱦ����

	putimagePNG(230, 0, &imgBAR);  //��Ⱦֲ���̵�

	//��Ⱦֲ�￨��
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		int x = 327 + i * 55  ;
		int y = 12;
		putimage(x, y, &imgCards[i]);
	}

	//��Ⱦ�϶������е�ͼƬ
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