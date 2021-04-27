#ifndef _TETRIS_H_
#define _TETRIS_H_


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <string.h>

#define WIDTH	10
#define HEIGHT	22
#define NOTHING	0
#define QUIT	'q'
#define NUM_OF_SHAPE	7
#define NUM_OF_ROTATE	4
#define BLOCK_HEIGHT	4
#define BLOCK_WIDTH	4
#define BLOCK_NUM	3

// menu number
#define MENU_PLAY '1'
#define MENU_RANK '2'
#define MENU_EXIT '4'

#define NAMELEN 16

#define CHILDREN_MAX 36

#define INF 2000000000

typedef struct _RecNode{
	int lv,score;
	char (*f)[WIDTH];
	struct _RecNode *c[CHILDREN_MAX];
} RecNode;

typedef struct _ListNode {
	char name[NAMELEN];
	int score;
	struct _ListNode* next;
} ListNode;

typedef struct {
	int size;
	ListNode* head;
} List;

List* rankList;
void InsertNode(List* curList, ListNode* curNode, ListNode* nodeToInsert) {
	curList->size++;
	ListNode* temp = curNode->next;
	curNode->next = nodeToInsert;
	nodeToInsert->next = temp;
}

void initList(List* curList) {
	rankList = malloc(sizeof(List));
	rankList->head = malloc(sizeof(ListNode));
	rankList->head->score = INF;
	rankList->head->next = NULL;
}

const char block[NUM_OF_SHAPE][NUM_OF_ROTATE][BLOCK_HEIGHT][BLOCK_WIDTH] ={
	{
		{
			{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}			
		},
		{
			{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}			
		},
		{
			{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}
		}
	},
	{
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}
		},
		{
			{0, 0, 0, 0}, {0, 0 ,1, 1}, {0, 0, 1, 0}, {0, 0, 1, 0}
		},
		{
			{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}
		}
	},
	{
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}
		},
		{
			{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 1}
		},
		{
			{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}
		}
	},
	{
		{
			{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}
		},
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}
		},
		{
			{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}
		}
	},
	{
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}
		},
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}
		},
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}
		},
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}
		}
	},
	{
		{
			{0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}			
		},
		{
			{0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}			
		},
		{
			{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}			
		}
	},
	{
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}
		},
		{
			{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}
		},
		{
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}
		},
		{
			{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}
		}
	}
};

char field[HEIGHT][WIDTH];
int nextBlock[BLOCK_NUM];
int blockRotate,blockY,blockX;
int score;
int gameOver=0;
int timed_out;
int recommendR,recommendY,recommendX;
RecNode *recRoot;
int scorePrintPos;
int nextBlockDrawPos[BLOCK_NUM];

void InitTetris();
void DrawOutline();
int GetCommand();
int ProcessCommand(int command);
void BlockDown(int sig);
int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX);
void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX);
void DrawField();
int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX);
int DeleteLine(char f[HEIGHT][WIDTH]);
void gotoyx(int y, int x);
void DrawNextBlock(int *nextBlock);
void PrintScore(int score);
void DrawBox(int y,int x, int height, int width);
void DrawBlock(int y, int x, int blockID,int blockRotate,char tile, int reverse);
void DrawShadow(int y, int x, int blockID,int blockRotate);
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate);
void play();
char menu();
void createRankList();
void rank();
void printNameScore(char* name, int score);
void printNameScoreTitle();
void writeRankFile();
void newRank(int score);
void deleteNextNode(ListNode* cur);
int recommend(RecNode *root);
void recommendedPlay();
//내가 만든 함수.
//field 안에 있는 좌표인지 확인
int inField(int y, int x) {
	if (!(0 <= y && y < HEIGHT)) return 0;
	if (!(0 <= x && x < WIDTH)) return 0;

	return 1;
}
//랜덤하게 블록 종류 결정
int randomShape() {
	return rand() % NUM_OF_SHAPE;
}

#endif
