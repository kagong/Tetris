#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	assume_default_colors(COLOR_BLACK,COLOR_WHITE);
	noecho();
	keypad(stdscr, TRUE);	
	start_color();
	curs_set(0);
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,COLOR_GREEN,COLOR_BLACK);
	init_pair(3,COLOR_YELLOW,COLOR_BLACK);
	init_pair(4,COLOR_BLUE,COLOR_BLACK);
	init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(6,COLOR_CYAN,COLOR_BLACK);
	init_pair(7,COLOR_BLACK,COLOR_BLACK);
	init_pair(8,COLOR_RED,COLOR_WHITE);
	init_pair(9,COLOR_WHITE,COLOR_BLUE);
	srand((unsigned int)time(NULL));
	while(!exit){
		clear();
		createRankList();
		switch(menu()){
			case MENU_PLAY: play(); break;
			case MENU_RANK: rank(); break;
			case MENU_RECP: recommendedPlay();break;
			case MENU_EXIT: exit=1; break;
			default: break;
		}
		writeRankFile();
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=-1;
	for(i=0;i<HEIGHT;++i)
		for(j=0;j<WIDTH;++j)
			Cfield[i][j]=field[i][j];

	nextBlock[0]=rand()%7;
	blockColor[0]=rand()%7+1;
	nextBlock[1]=rand()%7;
	blockColor[1]=rand()%7+1;
	nextBlock[2]=rand()%7;
	blockColor[2]=rand()%7+1;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	findRecommend(nextBlock[0],blockRotate);
		DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,2*WIDTH+10);
	printw("SECOND BLOCK");	
	move(2,2*WIDTH+26);
	printw("THIRD BLOCK");
	DrawBox(3,2*WIDTH+10,4,16);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9,2*WIDTH+10);
	printw("SCORE");
	DrawBox(10,2*WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
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
			command = SPACE;
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
	int ret=1,i,j;
	int drawFlag=0;
	switch(command){
		case QUIT:
			ret = QUIT;
			break;
		case SPACE:
			for(i=0;;++i)
			{
				if(CheckToMove(field,nextBlock[0],blockRotate,blockY+i,blockX)==0)
					break;
			}
			blockY+=(i-1);
			DrawChange(field,1,nextBlock[0],blockRotate,blockY,blockX);
			AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
			for(i=0;i<HEIGHT;++i)
				for(j=0;j<WIDTH;++j)
					Cfield[i][j]=field[i][j];
			score+=DeleteLine(field);
			nextBlock[0]=nextBlock[1];
			blockColor[0]=blockColor[1];
			nextBlock[1]=nextBlock[2];
			blockColor[1]=blockColor[2];
			nextBlock[2]=rand()%7;
			blockColor[2]=rand()%7+1;
			blockRotate=0;
			blockY=-1;
			blockX=WIDTH/2-2;
			DrawNextBlock(nextBlock);
			PrintScore(score);
			findRecommend(nextBlock[0],blockRotate);
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

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]!=-1){
				attron(COLOR_PAIR(field[j][i]));
				attron(A_REVERSE);
				printw("  ");
				attroff(A_REVERSE);
				attroff(COLOR_PAIR(field[j][i]));
			}

			else if(i+j==4||j-i==4||i-j==5||i+j==13||j-i==13||i+j==22||i+j==26||j-i==17||j-i==8||i+j==17||i+j==8||j-i==-1){
				attron(COLOR_PAIR(8));
				printw(". ");
				attroff(COLOR_PAIR(8));
			}

			else{
				printw(" .");
			}
		}
	}
}
void PrintScore(int score){
	move(11,2*WIDTH+17);
	printw("%10d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,2*WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(COLOR_PAIR(blockColor[1]));
				attron(A_REVERSE);
				printw("  ");
				attroff(A_REVERSE);
				attroff(COLOR_PAIR(blockColor[1]));
			}
			else{
				printw("  ");
			}
		}
		move(4+i,2*WIDTH+29);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(COLOR_PAIR(blockColor[2]));
				attron(A_REVERSE);
				printw("  ");
				attroff(A_REVERSE);
				attroff(COLOR_PAIR(blockColor[2]));
			}
			else{
				printw("  ");
			}
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,2*j+2*x+1);
				if(tile==' '){
					attron(COLOR_PAIR(blockColor[0]));
					attron(A_REVERSE);
					printw("%c%c",tile,tile);
					attroff(A_REVERSE);
					attroff(COLOR_PAIR(blockColor[0]));
				}
				else
					printw("%c%c",tile,tile);
			}
		}
	move(HEIGHT,2*WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch('O');
	for(i=0;i<2*width;i++)
		addch('-');
	addch('O');
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch('|');
		move(y+j+1,x+2*width+1);
		addch('|');
	}
	move(y+j+1,x);
	addch('O');
	attroff(A_REVERSE);
	for(i=0;i<2*width;i++)
		addch('-');
	addch('O');
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
			DrawBox(HEIGHT/2-1,0,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("     Good-bye!!     ");
			refresh();
			wgetch(stdscr);

			return;
		}
	}while(!gameOver);
	alarm(0);
	wgetch(stdscr);
	DrawBox(HEIGHT/2-1,0,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("     GameOver!!     ");
	refresh();
	wgetch(stdscr);
	newRank(score);
}

char menu(){
	int x,y;
	getmaxyx(stdscr,y,x);
	if(x<130)
	{
		move(0,x/2-24);
		printw("  ,--.           ,--.         ,--. \n");        
		move(1,x/2-24);
		printw(",-'  '-. ,---. ,-'  '-.,--.--.`--' ,---.  \n");
		move(2,x/2-24);
		printw("'-.  .-'| .-. :'-.  .-'|  .--',--.(  .-' \n");
		move(3,x/2-24);
		printw("  |  |  |  ---.  |  |  |  |   |  |.-'  `) \n");
		move(4,x/2-24);
		printw("   `--   `----'  `--'  `--'   `--'`----'  \n");

		move(7,x/2-10);
		printw("1. play\n");
		move(8,x/2-10);
		printw("2. rank\n");
		move(9,x/2-10);
		printw("3. recommended play\n");
		move(10,x/2-10);
		printw("4. exit\n");

		DrawBox(6,x/2-11,4,10);
	}
	else
	{
		move(0,x/2-62);
		printw("  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------. \n");
		move(1,x/2-62);
		printw(" | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n");
		move(2,x/2-62);
		printw(" | |  _________   | || |  _________   | || |  _________   | || |  _______     | || |     _____    | || |    _______   | |\n");
		move(3,x/2-62);
		printw(" | | |  _   _  |  | || | |_   ___  |  | || | |  _   _  |  | || | |_   __ \\    | || |    |_   _|   | || |   /  ___  |  | |\n");
		move(4,x/2-62);
		printw(" | | |_/ | | \\_|  | || |   | |_  \\_|  | || | |_/ | | \\_|  | || |   | |__) |   | || |      | |     | || |  |  (__ \\_|  | |\n");
		move(5,x/2-62);
		printw(" | |     | |      | || |   |  _|  _   | || |     | |      | || |   |  __ /    | || |      | |     | || |   '.___`-.   | |\n");
		move(6,x/2-62);
		printw(" | |    _| |_     | || |  _| |___/ |  | || |    _| |_     | || |  _| |  \\ \\_  | || |     _| |_    | || |  |`\\____) |  | |\n");
		move(7,x/2-62);
		printw(" | |   |_____|    | || | |_________|  | || |   |_____|    | || | |____| |___| | || |    |_____|   | || |  |_______.'  | |\n");
		move(8,x/2-62);
		printw(" | |              | || |              | || |              | || |              | || |              | || |              | |\n");
		move(9,x/2-62);
		printw(" | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n");
		move(10,x/2-62);
		printw("  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' \n");

		move(y/2,x/2-5);
		printw("1. play\n");
		move(y/2+1,x/2-5);
		printw("2. rank\n");
		move(y/2+2,x/2-5);
		printw("3. recommended play\n");
		move(y/2+3,x/2-5);
		printw("4. exit\n");

		DrawBox(y/2-1,x/2-6,4,10);
	}
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int Y, int X){
	int i,j;
	int Cbx,Cby;
	for(i=0;i<BLOCK_HEIGHT;++i)
	{
		for(j=0;j<BLOCK_WIDTH;++j)
		{
			if(block[currentBlock][blockRotate][i][j]==1)
			{
				Cbx=j+X;
				Cby=i+Y;
				if(Cbx>=0&&Cbx<WIDTH&&Cby>=0&&Cby<HEIGHT)
				{
					if(field[Cby][Cbx]!=-1)
						return 0;
				}
				else
				{
					return 0;
				}
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int i;
	DrawField();

	for(i=0;;++i)
	{
		if(CheckToMove(field,nextBlock[0],blockRotate,blockY+i,blockX)==0)
			break;
	}
	DrawBlock(recommendY,recommendX, nextBlock[0],recommendR,'R');
	DrawBlock(blockY+i-1, blockX, nextBlock[0],blockRotate,'/');

	DrawBlock(blockY, blockX, nextBlock[0],blockRotate,' ');
}
void BlockDown(int sig){
	int i,j;
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX))
		DrawChange(field,1,nextBlock[0],blockRotate,++blockY,blockX);
	else if(blockY==-1)	
		gameOver=1;
	else
	{
		AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		for(i=0;i<HEIGHT;++i)
			for(j=0;j<WIDTH;++j)
				Cfield[i][j]=field[i][j];
		score+=DeleteLine(field);
		nextBlock[0]=nextBlock[1];
		blockColor[0]=blockColor[1];
		nextBlock[1]=nextBlock[2];
		blockColor[1]=blockColor[2];
		nextBlock[2]=rand()%7;
		blockColor[2]=rand()%7;
		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;
		DrawOutline();
		DrawNextBlock(nextBlock);
		PrintScore(score);
		findRecommend(nextBlock[0],blockRotate);
	}
	timed_out=0;
}

void AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int Y, int X){
	int i,j;
	for(i=0;i<BLOCK_HEIGHT;++i)
	{
		for(j=0;j<BLOCK_WIDTH;++j)
		{
			if(block[currentBlock][blockRotate][i][j]==1)
			{
				if	(Y+i==HEIGHT-1)
					score+=10;
				else if(f[Y+i+1][X+j]!=-1)
					score+=10;
				f[Y+i][X+j]=blockColor[0];
			}
		}
	}

}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i,j,k,l;
	int line=0;
	for(i=0;i<HEIGHT;++i)
	{
		for(j=0;j<WIDTH;++j)
		{
			if(f[i][j]==-1)
				break;
		}
		if(j==WIDTH)
		{
			++line;
			if(i==0)
			{
				for(l=0;l<WIDTH;++l)
					f[0][l]=-1; 
			}
			else
			{
				for(k=i;k>0;--k)
				{
					for(l=0;l<WIDTH;++l)
						f[k][l]=f[k-1][l]; 
				}
				for(l=0;l<WIDTH;++l)
					f[0][l]=-1; 
			}
		}
	}
	return line*line*100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
}

void createRankList(){
	FILE* fp=fopen("rank.txt","r");
	int n,i;
	char Name[NAMELEN];
	if(!fp)
	{
		fp=fopen("rank.txt","w+");
		fclose(fp);
		fp=fopen("rank.txt","r");
	}
	nptr temp=NULL;
	head=(nptr)malloc(sizeof(node));
	head->link=NULL;
	if(fscanf(fp,"%d",&n)==EOF)
		head->score=0;
	else
	{
		head->score=n;
		temp=head;
		for(i=0;i<n;++i)
		{
			temp->link=(nptr)malloc(sizeof(node));
			temp=temp->link;
			fscanf(fp,"%s %d",temp->name,&(temp->score));
			temp->link=NULL;
		}
	}
	fclose(fp);
	// user code
}
int inputXY()
{
	char x;
	int xy=0,flag=0;//flag안에 아무것도 입력되지않았다
	while(1)
	{
		x=wgetch(stdscr);
		switch(x)
		{
			case '0':case '1':			
			case '2':case '3':
			case '4':case '5':	
			case '6':case '7':
			case '8':case '9':
				xy*=10;
				xy+=x-'0';
				flag=1;
				break;
			case ' ':
				if(flag==0)
					return -1;
				else
				{
					return -2;
				}
			case '\n':
				if(flag==0)
					return -1;
				else
					return xy;
			default :
				return -2;
				//return 0;
		}
	}
}
void rank(){
	int x,y;
	int cnt=0;
	nptr temp=NULL;
	clear();
	curs_set(2);
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	switch(wgetch(stdscr)){
		case '1': break;
		case '2': findrank();return;
		case '3': deleterank();return;
	}
	echo();
	move(3,0);
	printw("X: ");
	x=inputXY();
	if(x==-2||x==0)
	{
		curs_set(0);
		noecho();
		printw("input error\n");
		getch();
		return ;
	}
	move(4,0);
	printw("Y: ");
	y=inputXY();
	curs_set(0);
	move(5,0);
	if(y==-2||y==0)
	{
		noecho();
		printw("input error\n");
		getch();
		return ;
	}
	noecho();
	printw("%-20s|%-20s\n","name","score");
	printw("---------------------------------\n");
	if(x==-1)
		x=1;
	if(y==-1)
		y=head->score;
	if(x==0||x>y)
		printw("search failure:no rank in the list\n");
	temp=head;
	if(x<=head->score)
	{
		while(1)
		{
			++cnt;
			if(cnt==x)
				break;
			temp=temp->link;
		}
		for(cnt=x;cnt<=y;++cnt)
		{
			if(temp->link==NULL)
				break;
			else
			{
				temp=temp->link;
				printw("%-20s|%-20d\n",temp->name,temp->score);
			}
		}
	}	
	wgetch(stdscr);

}
void findrank(){
	int i,flag=0;
	nptr temp=head->link;
	char name[NAMELEN];
	echo();
	move(3,0);
	printw("input the name: ");
	scanw("%s",name);
	printw("%-20s|%-20s\n","name","score");
	printw("---------------------------------\n");
	for(i=0;i<head->score;++i){
		if(strcmp(temp->name,name)==0)
		{
			flag=1;
			printw("%-20s|%-20d\n",temp->name,temp->score);
		}
		temp=temp->link;
	}
	if(flag==0)
		printw("search failure:no name in the list\n");
	curs_set(0);
	noecho();
	getch();
}
void deleterank(){
	int n,i;
	nptr temp=head,temp2;
	echo();
	move(3,0);
	printw("input the rank: ");
	scanw("%d",&n);
	if(n>0&&n<=head->score){
		for(i=0;i<n-1;++i){
			temp=temp->link;
		}
		temp2=temp->link->link;
		free(temp->link);
		temp->link=temp2;
		--head->score;
		printw("result: the rank deleted\n");
	}
	else
		printw("search failure:no rank not in the list\n");
	noecho();
	getch();
}
void writeRankFile(){
	nptr temp=NULL;
	FILE *fp=fopen("rank.txt","w");
	fprintf(fp,"%d\n",head->score);
	while(1)
	{
		if(head->link==NULL)
			break;
		else
		{
			fprintf(fp,"%s %d\n",head->link->name,head->link->score);
			temp=head->link->link;
			free(head->link);
			head->link=temp;
		}
	}
	fclose(fp);
	free(head);
	head=NULL;
}

void newRank(int score){
	int Ncnt=0;
	nptr temp=NULL,temp2=NULL;
	char c,n[NAMELEN];
	clear();
	echo();
	printw("your name:");
	while(1)
	{
		c=wgetch(stdscr);
		if(c=='\n')
		{
			n[Ncnt]='\0';
			break;
		}
		else
			n[Ncnt++]=c;

	}
	noecho();
	temp=(nptr)malloc(sizeof(node));
	strcpy(temp->name,n);
	temp->score=score;
	temp->link=NULL;
	temp2=head;
	while(1)
	{
		if(temp2->link==NULL)
		{
			temp2->link=temp;

			head->score+=1;
			break;
		}
		else if(temp->score	> temp2->link->score)
		{
			head->score+=1;
			temp->link=temp2->link;
			temp2->link=temp;
			break;
		}
		else 
			temp2=temp2->link;
	}
}
//최대값을 반환 최대값의 좌표에 해당하는 rx,ry 정함 rotate는 변경하지않으므로 2번호출해야함
void findRecommend(int blockID,int blockRotate){
	// user code
	int i,j,k,n=1,Tscore;
	int r,x,y=-1,max=-111111111;

	r=blockRotate;
	for(j=0;j<4;++j)
	{
		switch(blockID)
		{
			case 0:
				if(r/2==0)
					n=4;
				else
					n=1;
				break;
			case 1:case 2:
			case 3:case 5:
			case 6:
				if(r/2==0)
					n=3;
				else
					n=2;
				break;
			case 4:
				n=2;
				break;

		}
		if(CheckToMove(field,nextBlock[0],r,1,-2)==1)
			x=-2;
		else if(CheckToMove(field,nextBlock[0],r,1,-1)==1)
			x=-1;
		else
			x=0;
		//for(;x<=10-n;++x)
		while(CheckToMove(field,nextBlock[0],r,1,x)!=0)
		{
			y= -1;
			while(CheckToMove(field,nextBlock[0],r,y+1,x))
					y++;

			Tscore=score;
			AddBlockToField(Cfield,nextBlock[0],r,y,x);
			score=Tscore;
			Tscore=checkRecommend(x,y,blockID,r);
			if(max<Tscore)
			{
				max=Tscore;
				recommendX=x;
				recommendY=y;
				recommendR=r;
			}
			for(i=0;i<HEIGHT;++i)
				for(k=0;k<WIDTH;++k)
					Cfield[i][k]=field[i][k];
			++x;
		}
		++r;
		r=r%4;
	}
}

float checkRecommend(int x,int y,int blockID,int r)
{
	int k,i,j,temp=0,flag=0,blanktemp=0,zero=0;//1쌓이기 시작 0 안쌓음
	int blank=0,blankBlock=0,lowline=0,RLline=0,box=0,line=0;
	int blockHigh[WIDTH];
	for(i=0;i<WIDTH;++i)//전체검사로 블럭높이의합 블럭사이에빈공간 빈공간을 막고있는블럭의갯수를조사
	{
		blockHigh[i]=0;
		for(j=0;j<HEIGHT;++j)
		{
			if(Cfield[j][i]!=-1)
			{
				if(flag==1)//temp에 저장후 만약  빈공간 만나면 빈공간막블럭수에저장
				{
					++temp;
				}
				else//높이저장
				{
					flag=1;
					blockHigh[i]=22-j;
					temp=1;
				}
			}
			else
			{
				if(flag==1)//빈공간 막 블럭수에 저장 temp를 따로 증가시키지는않고 빈공간수도증가
				{
					++blank;
					blanktemp=temp;
				}
			}
		}

		blankBlock+=blanktemp;
		blanktemp=0;
		flag=0;
	}
	//현재블럭의 위치를 조사하여 완성된줄의개수 현재블럭이 닾는면의 갯수등을 조사
	for(i=0;i<4;++i)
	{
		for(j=0;j<4;++j)
		{
			if( block[blockID][r][j][i] == 1 )
			{
				if(y+j==HEIGHT-1)//끝자리
				{
					++lowline;
				}
				else if(field[y+j+1][x+i]!=-1)
				{
					++box;
				}
				if(x+i==0)//왼쪽
				{
					++RLline;
				}
				else if(field[y+j][x+i-1]!=-1)
				{
					++box;
				}
				if(x+i==WIDTH-1)//오른쪽
				{
					++RLline;
				}
				else if(field[y+j][x+i+1]!=-1)
				{
					++box;
				}
			}
		}
	}
	for(i=0;i<HEIGHT;++i)
	{
		for(j=0;j<WIDTH;++j)
		{
			if(Cfield[i][j]==-1)
				break;
		}
		if(j==WIDTH)
		{
			++line;	
		}
	}

	for(i=1;i<WIDTH;++i)
		blockHigh[0]+=blockHigh[i];

	if((blockHigh[0]/10)>=10)
		line*=2;
	return 10*y-2*zero+line*(8.2)+RLline*(2.5)+box*(3.7)+lowline*(4.0)-blank*(8.8)-blankBlock*(0.59)-blockHigh[0]*(3.78);
}
int recommned(RecNode *root)
{
}
void recommendedPlay(){
	int command;
	clear();
	act.sa_handler = RecDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(command==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,0,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("     Good-bye!!     ");
			refresh();
			wgetch(stdscr);

			return;
		}
	}while(!gameOver);
	alarm(0);
	wgetch(stdscr);
	DrawBox(HEIGHT/2-1,0,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("     GameOver!!     ");
	refresh();
	wgetch(stdscr);
	// user code
}
void RecDown(int isg)
{
	int i,j;
		//xyxy=0;

	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)==0)
		gameOver=1;
	else
	{
		AddBlockToField(field,nextBlock[0],recommendR,recommendY,recommendX);
		recommendY=-1;
		for(i=0;i<HEIGHT;++i)
			for(j=0;j<WIDTH;++j)
				Cfield[i][j]=field[i][j];
		clear();
		score+=DeleteLine(field);
		nextBlock[0]=nextBlock[1];
		blockColor[0]=blockColor[1];
		nextBlock[1]=nextBlock[2];
		blockColor[1]=blockColor[2];
		nextBlock[2]=rand()%7;
		blockColor[2]=rand()%7;
		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;
		DrawField();
		DrawOutline();
		DrawNextBlock(nextBlock);
		PrintScore(score);
		findRecommend(nextBlock[0],blockRotate);
		DrawBlock(recommendY,recommendX, nextBlock[0],recommendR,'R');
		if(recommendY==-1)
			gameOver=1;
	}
	timed_out=0;
}

