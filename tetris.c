#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	createRankList();

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	for(int j=0;j<HEIGHT;j++)
		for(int i=0;i<WIDTH;i++)
			field[j][i]=0;

	for (int i = 0; i < BLOCK_NUM; i++) nextBlock[0] = randomShape();

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	/* 블럭이 떨어지는 공간의 테두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 테두리를 그린다.*/
	int printHere = 2;
	move(printHere++,WIDTH+10);
	printw("NEXT BLOCK");

	for (int i = 1; i < BLOCK_NUM; i++, printHere += (BLOCK_HEIGHT + 3)) {
		DrawBox(printHere, WIDTH + 10, BLOCK_HEIGHT, BLOCK_WIDTH * 2);
		nextBlockDrawPos[i] = printHere;
	}

	/* score를 보여주는 공간의 테두리를 그린다.*/
	printHere++;
	move(printHere++, WIDTH + 10);
	printw("SCORE");
	DrawBox(printHere,WIDTH+10,1,8);
	scorePrintPos = printHere + 1;
}

int GetCommand(){
	int command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField() {
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}

void PrintScore(int score){
	move(scorePrintPos, WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	for (int k = 1; k < BLOCK_NUM; k++) {
		DrawBlock(nextBlockDrawPos[k], WIDTH + 13, nextBlock[k - 1], 0, ' ', 0);
		DrawBlock(nextBlockDrawPos[k], WIDTH + 13, nextBlock[k], 0, ' ', 1);
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile, int reverse){
	int i,j;
	for(i=0;i<BLOCK_HEIGHT;i++)
		for(j=0;j< BLOCK_WIDTH;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				if (reverse) {
					attron(A_REVERSE);
					printw("%c", tile);
					attroff(A_REVERSE);
				}
				else {
					printw("%c", tile);
				}
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			newRank(score);
			return;
		}
	}while(!gameOver);
	
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[currentBlock][blockRotate % 4][i][j]) {
				int fieldY = blockY + i, fieldX = blockX + j;
				if (inField(fieldY, fieldX) == 0 || f[fieldY][fieldX] == 1) return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {
	int prevY = blockY, prevX = blockX, prevRotate = blockRotate;

	if (command == KEY_UP) prevRotate = (blockRotate + 3) % 4;
	else if (command == KEY_DOWN) prevY--;
	else if (command == KEY_LEFT) prevX++;
	else if (command == KEY_RIGHT) prevX--;

	DrawBlock(prevY, prevX, currentBlock, prevRotate, '.', 0);
	
	//그림자 지우기
	for (int i = prevY; i < HEIGHT; i++) {
		if (CheckToMove(field, currentBlock, prevRotate, i + 1, prevX) == 0) {
			DrawBlock(i, prevX, currentBlock, prevRotate, '.', 0);
			break;
		}
	}

	DrawShadow(blockY, blockX, currentBlock, blockRotate);
	DrawBlock(blockY, blockX, currentBlock, blockRotate, ' ', 1);
	move(HEIGHT, WIDTH + 10);
}

void BlockDown(int sig) {
	//이동할 수 있다면 이동하고 끝
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}

	//이동할 수 없다
	else {
		//게임 종료
		if (blockY == -1) {
			gameOver = 1;
			return;
		}

		//블록 쌓기
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);

		//줄 지우기
		score += DeleteLine(field);

		//블록 설정
		for (int i = 0; i < BLOCK_NUM - 1; i++) nextBlock[i] = nextBlock[i + 1];
		nextBlock[BLOCK_NUM - 1] = randomShape();
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;

		//게임판 재구성
		DrawField();
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
		DrawNextBlock(nextBlock);
		PrintScore(score);
		move(HEIGHT + 3, 0);
	}
	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	int addScore = 0;
	for (int i = 0; i < BLOCK_WIDTH; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			int fieldY = blockY + i, fieldX = blockX + j;
			if (block[currentBlock][blockRotate][i][j]) {
				f[fieldY][fieldX] = 1;
				if (fieldY == HEIGHT - 1 || f[fieldY + 1][fieldX] == 1) addScore++;
			}
		}
	}
	return addScore * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
	int cnt = 0;
	int notEmpty[HEIGHT] = { 0 };

	for (int i = 0; i < HEIGHT; i++) {
		int flag = 1;
		for (int j = 0; j < WIDTH; j++) {
			if (f[i][j] != 1) {
				flag = 0; break;
			}
		}

		if (flag) {
			cnt++;
		}
		else {
			notEmpty[i] = 1;
		}
	}

	int fill = HEIGHT - 1;
	for (int i = HEIGHT - 1; i >= 0; i--) {
		if (notEmpty[i]) {
			if (i != fill) {
				for (int j = 0; j < WIDTH; j++) {
					field[fill][j] = field[i][j];
				}
			}
			fill--;
		}
	}
	for (int i = fill; i >= 0; i--) {
		for (int j = 0; j < WIDTH; j++) {
			field[i][j] = 0;
		}
	}

	return cnt * cnt * 100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	for (int i = y; i < HEIGHT; i++) {
		if (CheckToMove(field, blockID, blockRotate, i + 1, x) == 0) {
			DrawBlock(i, x, blockID, blockRotate, '/', 1);
			break;
		}
	}

	move(HEIGHT, WIDTH + 10);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ', 1);
}

void createRankList(){
	FILE *fi = fopen("rank.txt", "r");
	rankList = NULL;

	if (fi == NULL) {
		return;
	}

	initList(rankList);
	fscanf(fi, "%d", &rankList->size);
	
	ListNode* cur = rankList->head;
	for (int i = 0; i < rankList->size; i++) {
		cur->next = malloc(sizeof(ListNode));
		cur = cur->next;

		fscanf(fi, "%s", cur->name);
		fscanf(fi, "%d", &cur->score);
		cur->next = NULL;
	}

	fclose(fi);
}

void rank(){
	clear();
	move(0, 0);

	if (rankList == NULL) {
		printw("No Records\n");
		getch();
		return;
	}

	//options
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	int mode = wgetch(stdscr);

	if (mode == '1') { //1. list ranks from X to Y
		printw("enter X: ");
		int x; echo(); scanw("%d", &x); noecho();
		printw("enter Y: ");
		int y; echo(); scanw("%d", &y); noecho();
		x = (1 <= x ? x : 1);
		y = (y <= rankList->size ? y : rankList->size);
		
		if (x > y) {
			printw("search failure : no rank in the list\n");
			getch();
			return;
		}

		printNameScoreTitle();

		ListNode* cur = rankList->head;
		for (int i = 0; i < x; i++) cur = cur->next;
		for (int i = x; i <= y; i++, cur = cur->next) {
			printNameScore(cur->name, cur->score);
		}

		getch();
		clear();
		return;
	}

	else if (mode == '2') { //2. list ranks by a specific name
		printw("name you are searching for : ");
		char keyName[NAMELEN];
		echo(); scanw("%s", keyName); noecho();

		printNameScoreTitle();
		ListNode* cur = rankList->head;
		int flag = 0;
		while (cur->next != NULL) {
			cur = cur->next;
			if (strcmp(cur->name, keyName) == 0) {
				printNameScore(cur->name, cur->score);
				flag = 1;
			}
		}

		if (flag == 0) {
			printw("search failure: no name in the list\n");
		}

		getch();
		clear();
		return;
	}

	else if (mode == '3') { //3. delete a specific rank
		printw("rank to delete : ");

		int rankInput;
		echo(); scanw("%d", &rankInput); noecho();

		if(!(1 <= rankInput && rankInput <= rankList->size)){
			printw("\nsearch failure: enter rank between 1 to %d\n", rankList->size);
			getch(); clear(); return;
		}

		else {
			int count = 0;
			ListNode* cur = rankList->head;
			while (cur != NULL) {
				if (count + 1 == rankInput) {
					deleteNextNode(cur);
					printw("\nsuccessfully deleted\n");
					break;
				}
				cur = cur->next;
				count++;
			}

			getch(); clear(); return;
		}
	}
	
	else {
		printw("Enter a number between 1 to 3\n");
		getch();
		clear();
	}
}

void printNameScoreTitle() {
	printw("\n");
	printw("\tname\t|\tscore\n");
	printw("--------------------------------\n");	
}

void printNameScore(char* name, int score) {
	int flag = 1; //간격 맞게 이름을 출력하기 위함
	for (int k = 0; k < NAMELEN; k++) {
		if (flag && name[k] == 0) flag = 0;

		if (flag) printw("%c", name[k]);
		else printw(" ");
	}
	printw("|\t%d\n", score);
}

void deleteNextNode(ListNode* cur)
{
	ListNode* temp = cur->next;
	cur->next = cur->next->next;
	free(temp);
	rankList->size--;

	writeRankFile();
}

void writeRankFile(){
	FILE*file = fopen("rank.txt", "w");

	ListNode* cur = rankList->head;

	fprintf(file, "%d\n", rankList->size);
	while (cur->next != NULL) {
		cur = cur->next;
		fprintf(file, "%s %d\n", cur->name, cur->score);
	}
}

void newRank(int score){
	clear();
	printw("your name: ");

	ListNode* newNode = malloc(sizeof(ListNode));
	echo();
	scanw("%s", newNode->name);
	newNode->score = score;
	noecho();

	if (rankList == NULL) initList(rankList);
	ListNode* cur = rankList->head;
	int flag = 0;
	while (cur != NULL) {
		if (cur->next == NULL || (cur->score >= score && score >= cur->next->score)) {
			InsertNode(rankList, cur, newNode);
			flag = 1;
			break;
		}
		cur = cur->next;
	}
	if (flag == 0) InsertNode(rankList, cur, newNode);

	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
