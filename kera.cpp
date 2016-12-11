#include <cstdio>
#include <map>
#include <random>
#include <iostream>
#include <time.h>
#include <string>
#define PACK_SIZE 3
#define EMPTY 0

using namespace std;
typedef struct{
	int point[40];
    int chain;
}FireScore_t;

typedef struct{
	int obstacleNum;
    int board[10][19];
}Board_t;
typedef struct{
	Board_t *b;
	double point;
	int jamaStock;
	int beforePotePoint;
	long long allCommand;
	long long beforeAllCommand;
	string allCommandStr;
	string beforeAllCommandStr;
	long long allCommand2;
	long long beforeAllCommand2;
	double treeScore;
	int fire_;
	int oldPotePoint;
	int oldPoint;
	double oldJamaPenalty;
	double oldRandScore;
	int oldChain ;
	int semiFire;
}State_t;

int turn = -1;
int allPack[500][3][3] = {};
int height;
int width;
int packSize;
int summation;
int obstacle;
int maxTurn;
long long millitime;
int opOldJama;
int oldJama;
int attack=0;
int oldCol = -2;
int oldRot = -1;
long long remainTime = 0;
int attackedCount = 0;
int attacked = 0;
int gi = 0;
int delay = 0;
Board_t my;
Board_t op;
Board_t tmpB;
clock_t start;
long long beforeTurnAllCommand[20];
long long beforeTurnFireAllCommand[20];
long long beforeTurnAllCommand2[20];
long long beforeTurnFireAllCommand2[20];
random_device rnd;
mt19937 mt(rnd());

double getRand(){
	return mt()/4294967295.0;
}
void boardCopyAtoB(Board_t *ba, Board_t *bb){
	bb->obstacleNum = ba->obstacleNum;
	for(int i=0;i<width;i++){
		for(int j=0;j<height;j++){

			bb->board[i][j] = ba->board[i][j];
		}
	}
}
void stateCopyAtoB(State_t *sa,State_t *sb, Board_t *ab){
	sb->point = sa->point;
	sb->jamaStock = sa->jamaStock;
	sb->beforePotePoint = sa->beforePotePoint;
	sb->allCommand = sa->allCommand;
	sb->beforeAllCommand = sa->beforeAllCommand;
	sb->allCommand2 = sa->allCommand2;
	sb->beforeAllCommand2 = sa->beforeAllCommand2;
	sb->treeScore = sa->treeScore;
	sb->fire_ = sa->fire_;
	sb->oldPotePoint = sa->oldPotePoint;
	sb->oldPoint  = sa->oldPoint;
	sb->oldJamaPenalty = sa->oldJamaPenalty;
	sb->oldRandScore = sa->oldRandScore;
	sb->oldChain = sa->oldChain;
	sb->semiFire = sa->semiFire;
	sb->b = ab;
	for(int i=0; i<width;i++){
		for(int j=0; j<height;j++){
			sb->b->board[i][j] = sa->b->board[i][j];
		}
	}
}
void stateInit(State_t *st, Board_t *ab, double p,State_t *s, int c){
	st->b = ab;
	st->point = p;
	st->jamaStock = c;
	st->beforePotePoint = 0;
}
void boardInit(Board_t *b){
	b->obstacleNum = 0;
	for(int i=0 ;i<10;i++){
		for(int j=0; j<19;j++){
			b->board[i][j] = 0;
		}
	}
}
void stateInit(State_t *st){
	st->point=0;
	st->jamaStock = 0;
	st->beforePotePoint = 0;
	st->allCommand = 0;
	st->beforeAllCommand = 0;
	st->allCommand2 = 0;
	st->beforeAllCommand2 = 0;
	st->treeScore =0;
	st->fire_ = 0;
	st->oldPotePoint = 0;
	st->oldPoint  =0;
	st->oldJamaPenalty = 0;
	st->oldRandScore = 0;
	st->oldChain = 0;
	st->semiFire = 0;
}
void fsInit(FireScore_t *fs){
	for(int i=0; i<40;i++){
		fs->point[i] = 0;
	}
    fs->chain = 0;
}
double pow(double a, int b){
	double ans = 1;
	for(int i=0; i<b;i++){
		ans *= a;
	}
	return ans;
}
int toScore(FireScore_t *fs){
	int ans = 0;
	for(int i=1;i<=fs->chain;i++){

		ans += (int)(pow(1.3,i)) * (int)(fs->point[i-1]/2);

	}
	return ans;
}
//パックを落とす処理
void fall(Board_t *b, int pack[PACK_SIZE][PACK_SIZE], int col){
	for(int i=0; i<3;i++){
		int j=0;
		int k=0;
		for(k=0;k<3;k++){
			if(pack[k][i]!=EMPTY)break;
		}
		if(k==3)continue;

    	while(j<height && b->board[col+i][j] == EMPTY){
    		j++;
    	}
    	j--;
    	if(j==-1)continue;
    	for(k=2;k>=0;k--){
    		if(pack[k][i] != EMPTY && j >= 0){
    			b->board[col+i][j] = pack[k][i];
    			j--;
    		}
    	}
	}
}
//全体を落として盤面中に開いている穴を埋める処理
void fall(Board_t *b,int maxHeight,int fallList[]){
	for(int i=0;i<width;i++){
		int k=1;
		for(int j=fallList[i]/*height-1*/;j-k>=height-maxHeight;j--){
			if(b->board[i][j] == EMPTY){
				for(;j-k>=height-maxHeight;k++){
					if(b->board[i][j-k] != EMPTY){
						b->board[i][j] = b->board[i][j-k];
						b->board[i][j-k] = EMPTY;
						break;
					}
				}
			}
		}
	}
}
//パックを落としたときの発火シミュ
void fire(Board_t *b,FireScore_t *fs, int maxHeight,int argj){

    //fprintf(stderr, "b0:\n");fflush(stderr);
	int chain = 0;
	int fallList[10];;
	int breakList[110];;
	while(1){
    	int point = 0;
    	int bListCount = 0;
    	//int ii=0;
    	/*while(breakList[ii]!=0){
    		breakList[ii] = 0;ii++;
    	}*/
		for(int i=0; i<width;i++){
			//if(i<start || i > end)continue;
			for(int j=height-1/*height-maxHeight*/; j>=height-maxHeight;j--){
				if(b->board[i][j] == EMPTY){break;}
				if( b->board[i][j] != 11){
			    	for(int q=0;q<4;q++){
    					int k = 1,ah =0,aw = 0;
			    		switch(q){
			    		case 0:aw = 1;break;
			    		case 1:aw=1;ah = 1;break;
			    		case 2:ah=1;break;
			    		case 3:ah = 1	;aw = -1;break;
			    		}
    					int tmp = b->board[i][j];
						while(tmp<10){
							int h = j+ah*k, w=i+aw*k;
							if(h>=height||w>=width ||w<0 || b->board[w][h]%11==EMPTY)break;
							tmp += b->board[w][h];
							k++;
						}
						if(tmp ==10){
							point+= k;
    						for(int l=0;l<k;l++){
    							//b.board[i+aw*l][j+ah*l] += 20;
    							breakList[bListCount] =(i+aw*l)*20+ j+ah*l;
    							bListCount++;
    						}
						}
			    	}

				}
			}
		}
		if(point==0)break;
    	for(int i=0;i<10;i++){
    		fallList[i] = -1;
    		//fireList[i] = 0;
    	}
    	for(int a=0; a<bListCount;a++){
    		int i = breakList[a]/20;
    		int j = breakList[a]%20;
    		b->board[i][j] = EMPTY;
    		if(fallList[i] < j)fallList[i] = j;
    		//fireList[i] = 1;
    	}
		fs->point[chain] = point;
		chain++;
		fall(b,maxHeight,fallList);
	}
    //fprintf(stderr, "b1:\n");fflush(stderr);
	fs->chain =chain;
   // fprintf(stderr, "b2:\n");fflush(stderr);;
	//cerr << "a";
	//cerr.flush();
}

//潜在的連鎖スコアを計測するときの発火シミュ（上に比べ省略されていてより高速）
void fire(Board_t *b,FireScore_t *fs, int maxHeight,int x, int y, int hoc){
	int chain = 0;
	int fallList[10];
	int breakList[110];
	//int fireList[] = new int[10];
	int start = x,end = x;
	int fireHeight = y;
	int qNum = 13;
	int qStart = hoc + 1;
	while(1){
    	int point = 0;
    	int bListCount = 0;
    	//int ii= 0;
    	/*while(breakList[ii]!=0){
    		breakList[ii] = 0;ii++;
    	}*/
		for(int i=start; i<=end;i++){
			//if(i<start || i > end)continue;//これいらない
			for(int j=fireHeight/*height-maxHeight*/; b->board[i][j] != EMPTY/*j>=height-maxHeight*/;j--){
				int dx=0,dy=0;
				/*if(chain == 0){
					i=x;j=y;qNum = 10;//頑張れば消せる
				}*/
				if( b->board[i][j] != 11){
			    	for(int q=qStart;q<qNum;q++){//q9
    					int k = 0;
			    		int ah =0,aw = 0;
			    		switch(q){
			    		case 0:ah=1;break;
			    		case 1:aw = 1;ah = -1;break;
			    		case 2:aw = 1;break;
			    		case 3:aw=1;ah = 1;break;
			    		case 4:
			    			aw = -1	;ah = +1;
			    			break;
			    		case 5:
			    			aw=-1;
			    			break;
			    		case 6:
			    			aw = -1;ah=-1;
			    			break;
			    		case 7:
			    			aw =1;ah=-1;
			    			dx--;dy++;
			    			break;
			    		case 8:
			    			aw = 1;
			    			dy--;
			    			break;
			    		case 9:
			    			aw=1;ah=1;
			    			dy--;
			    			break;
			    		case 10:
			    			aw=1;ah=1;
			    			dx--;dy--;
			    			break;
			    		case 11:
			    			aw = 1;
			    			dy+=2;
			    			break;
			    		case 12:
			    			aw = 1;ah=-1;
			    			dy+=2;
			    			break;
			    		}
    					//int tmp = b.board[i][j];//いらなくね
			    		int tmp = 0;
						while(tmp<10){
							int h = j+dy+ah*k, w=i+dx+aw*k;
							if(h<0 || h>=height||w>=width ||w<0||b->board[w][h]%11==EMPTY)break;//11を図る必要なくね？
							tmp += b->board[w][h];
							k++;
						}
						if(tmp ==10){
							point+= k;
    						for(int l=0;l<k;l++){
    							breakList[bListCount] =(i+dx+aw*l)*20+ j+dy+ah*l;
    							bListCount++;
    						}
						}
			    	}
				}/*
				if(chain == 0){
					i=width;
					j=height;
					break;
				}*/
			}
		}
    	if(point == 0)break;
		qNum = 4;
		qStart = 0;
    	for(int i=0;i<10;i++){
    		fallList[i] = -1;
    		//fireList[i] = 0;
    	}
    	fireHeight = 0;
    	for(int a=0; a<bListCount;a++){
    		int i = breakList[a]/20;
    		int j = breakList[a]%20;
    		//System.err.println("i:"+i+" j:"+j);
    		b->board[i][j] = EMPTY;
    		if(fallList[i] < j){
    			if(fireHeight < j)fireHeight = j;
    			fallList[i] = j;
    		}
    		//fireList[i] = 1;//fire開始高さもっとたかくできない？
    	}
    	fireHeight += 4;//４でやったら？
    	if(fireHeight >height-1)fireHeight = height-1;
    	int i=0;
    	while(fallList[i]==-1){
    		i++;
    	}
    	start = i-4;
    	if(start < 0)start = 0;
    	i=9;
    	while(fallList[i]==-1){
    		i--;
    	}
    	end = i;/* + 2;
    	if(end >= 10)end = 9;*/
		fs->point[chain] = point;
		chain++;
		fall(b,maxHeight,fallList);
	}
	fs->chain =chain;

}
int isDead_(Board_t *b){
	int ret = 0;
	for(int i=0; i<3;i++){
		for(int j=0;j<width;j++){
			if(b->board[j][i] != EMPTY){
				ret = 1;
				i = 4;
				break;
			}
		}
	}

	return ret;
}
//そのブロックをその位置に落とした時発火するかどうかを判定
int haveOneChain(Board_t *b, int w, int h){
	for(int i=0;i<12;i++){
    	int k = 0;
		int ah =0,aw = 0;
		switch(i){
		case 0:
			aw = 1;ah=-1;
			break;
		case 1:
			aw=1;
			break;
		case 2:
			aw=1;ah=1;
			break;
		case 3:
			aw=-1;ah=1;
			break;
		case 4://多様性減らして強くするためにあえてダブらせる
			aw=-1;
			break;
		case 5:
			aw = -1	;ah = -1;
			break;
		case 6:
			aw = 1;ah=-1;
			w--;h++;
			break;
		case 7://逆ダブってる
			aw = 1;
			h--;
			break;
		case 8:
			aw=1;ah=1;
			h--;
			break;
		case 9:
			aw=1;ah=1;
			w--;h--;
			break;
		case 10:
			aw = 1;
			h+=2;
			break;
		case 11:
			aw = 1;ah=-1;
			h+=2;
			break;
		}

		int tmp = 0;
		while(tmp<10){
			if(h+ah*k<0 || h+ah*k>=height || w+aw*k>=width || w+aw*k<0)break;
			if(b->board[w+aw*k][h+ah*k]==EMPTY)break;
			tmp += b->board[w+aw*k][h+ah*k];
			k++;
		}
		if(tmp ==10){
			return i;
		}
	}

	return -1;
}
int getJama(Board_t *b){
	int ret = 0;
	for(int i=0; i<width;i++){
		for(int j=0; j<height;j++){
			if(b->board[i][j] == 11){
				ret++;
			}
		}
	}
	return ret;
}
//潜在的連鎖スコアを計算
int potencialChain(Board_t *b,int maxHeight){
	int tmpFireId[100] = {};
	int ret = 0;
	int ii=0;
	int tmpCount = 0;
	for(int i=0; i<width;i++){
		int tmp = 0;
		int k = height-maxHeight;
		while(k < height && b->board[i][k] ==EMPTY){
			k++;
		}
		k--;
		if(k == -1)continue;
    	for(int n=1; n<= 9;n++){
    		b->board[i][k] = n;
    		int hoc = haveOneChain(b,i,k);
    		if(hoc != -1){
    			tmpFireId[tmpCount] = hoc*10000+i*1000+(k)*10+n;
    			tmpCount++;
    		}
    		b->board[i][k] = EMPTY;
    	}
	}

	for(int a=0;a<tmpCount;a++){

		int tmp=tmpFireId[a];
		int hoc = tmp/10000;
		int i=tmp/1000%10;
		int k = (tmp/10)%100;
		int n = tmp%10;
		boardCopyAtoB(b,&tmpB);
		tmpB.board[i][k] = n;
		FireScore_t fs = {};
		fire(&tmpB,&fs,maxHeight+1,i,k,hoc);
		int bbb = toScore(&fs) + fs.chain;
		if(ret < bbb)ret = bbb;

	}
	return ret;
}
void boardInput(Board_t *b, int height, int width) {
	cin >> b->obstacleNum;//scanf("%d",&(b->obstacleNum));// = in.nextInt();
	//char ch[10];
	//fgets(ch, 10, stdin);
	//board = new int[width][height];
	for (int j = height-3; j < height; ++j) {
	    for (int i = 0; i < width; ++i){
			b->board[i][j] = 0;
		}
	}
	for (int j = 0; j < height-3; ++j){
		for (int i = 0; i < width; ++i){
			cin >> b->board[i][j+3];//scanf("%d",&(b->board[i][j+3]));// = in.nextInt();
		}
		//fgets(ch, 10, stdin);
	}

	string str;
		cin >> str;
	//fgets(ch, 10, stdin);
}


int oldCount = 20;
//ビームサーチの深さを決める
int calcCount(){
	int count = 20;
	if(turn >= 30-count)count = count-turn+30-count;
	if(turn >= 20)count = 10;
	//if(my.obstacleNum >= 4)count=10;
	//if(turn > 20)count = 15;
	if(millitime <= 30000){
		count = oldCount-1;
		if(count < 7)count = 7;
	}
	if(turn + count>=500)count = 500-turn;
	return count;
}
int calcUseJama(int pack[PACK_SIZE][PACK_SIZE]){

	int useJama = 0;
	for(int pi=0;pi<3;pi++){
		for(int pj=0;pj<3;pj++){
			if(pack[pi][pj] == 11)useJama++;
		}
	}
	return useJama;
}

int calcMaxHeight(Board_t *b){
	int ans = 0;
	for(int i=0; i<width;i++){
		int j=0;
		for(j=0;j<height;j++){
			if(b->board[i][height-j-1] == EMPTY)break;
		}
		if(ans< j)ans = j;;
	}
	return ans;
}
void copyPack(int pack[][PACK_SIZE], int packTmp[][PACK_SIZE]) {
    for (int i = 0; i < PACK_SIZE; ++i) {
    	for(int j=0;j<PACK_SIZE;j++){
    		packTmp[i][j] = pack[i][j];
    	}
    }
}


void rot1(int pack[][PACK_SIZE]) {
	int packTmp2[PACK_SIZE][PACK_SIZE] = {};
    for (int i = 0; i < PACK_SIZE; ++i) {
        for (int j = 0; j < PACK_SIZE; ++j) {
        	packTmp2[j][PACK_SIZE - i - 1] = pack[i][j];
        }
    }
    for (int i = 0; i < PACK_SIZE; ++i) {
        for (int j = 0; j < PACK_SIZE; ++j) {
        	pack[i][j] = packTmp2[i][j];
        }
    }
}
void packRotate(int pack[][PACK_SIZE], int rot) {
    for (int i = 0; i < rot; ++i) {
    	rot1(pack);
    }
}
//int packTmp3[PACK_SIZE][PACK_SIZE] = {};
void fillObstaclePack(int pack[][PACK_SIZE],int packB[][PACK_SIZE], int obstacleNum){
    copyPack(pack,packB);
    for (int i = 0; i < PACK_SIZE; ++i) {
        for (int j = 0; j < PACK_SIZE; ++j) {
            if (obstacleNum > 0 && packB[i][j] == EMPTY) {
                --obstacleNum;
                packB[i][j] = obstacle;
            }
        }
    }
}
void startInput(){
	cin >> width;//scanf("%d",&width);//width = in.nextInt();
    cin >> height;//scanf("%d",&height);//height = in.nextInt();
    height += 3;
    cin >> packSize;//scanf("%d",&packSize);//packSize = in.nextInt();
    cin >> summation;//scanf("%d",&summation);//summation = in.nextInt();
    obstacle = summation + 1;
    cin >> maxTurn;//scanf("%d",&maxTurn);//maxTurn = in.nextInt();
    //char ch[100];
    //fgets(ch, 100, stdin);

    //pack = new int[maxTurn][packSize][packSize];
    for (int i = 0; i < maxTurn; ++i) {
        for (int j = 0; j < packSize; ++j) {
            for (int k = 0; k < packSize; ++k) {
                cin >> allPack[i][j][k];//scanf("%d",&allPack[i][j][k]);//pack[i][j][k] = in.nextInt();
            }
       	 	//fgets(ch, 100, stdin);
        }

	string str;
		cin >> str;
   		//fprintf(stderr, "w:%d h:%d\n",width, height);
        //in.next();
        //fgets(ch, 100, stdin);
        //fgets(ch, 100, stdin);
       	//fgets(ch, 3, stdin);
   }
}
int  getRDrop(int pack[][PACK_SIZE]){
	int right = width - packSize;
    for (int i = packSize-1; i >= 0; --i) {
        for (int j = packSize - 1; j >= 0; --j) {
            if (pack[j][i] != EMPTY){
            	i = -1;
            	right--;
            	break;
            }
        }
        ++right;
    }
	return right;
}
int getLDrop(int pack[][PACK_SIZE]){
	int left = 0;
    for (int i = 0; i < packSize; ++i) {
        for (int j = 0; j < packSize; ++j) {
            if (pack[j][i] != EMPTY){
            	left--;
            	i = 100;
                break;
            }
        }
        ++left;
   	}
	return left;
}
void printCommand(long long c){
	for(int i=0; i<10;i++){
		long long tmp = c;
		for(int j=0;j<10-i-1;j++){
			tmp /= 60;
		}
		if(tmp == 0)continue;
		cerr << tmp%60 << " ";
	}
	cerr.flush();
}
State_t *stateList[50000];
State_t *tmpList[50000];
Board_t tmpBoard[2][500000];
State_t tState[2][500000];

map<long long,State_t*> stateMap_[2];
map<string,State_t*> stateMapS_[2];
map<long long,map<long long, State_t*>*> stateMap2_[2];
map<double,State_t*> stateTree_;
map<double,State_t*> finalTree_;
map<double, State_t*>::iterator it;
map<long long,map<long long, State_t*>*>::iterator it2;

//乱数で間引く処理（mapは同じkeyを所持できないことを応用して）
double getRandScore(){
	//if(/*millitime <= 30000*/gi>=10){
	int tmp = (30.0-gi/1.0);
	if(tmp < 10 || turn %2==1 || turn %3==2)tmp = 10;
		return (int)(getRand()*tmp)*0.001;
	//}
	//return getRand()*0.001;
}
void mapClear(){
	stateMap_[turn%2].clear();
	for(it2 = stateMap2_[turn%2].begin(); it2!=stateMap2_[turn%2].end(); it2++ ) {
		it2->second->clear();
    }
    finalTree_.clear();
}

int maxJama[20]={};int maxPoint[20]={};
double fireScore[20]={};double poteScore[20]={};
long long fireAllCommand[20]={};long long poteAllCommand[20]={};
long long fireAllCommand2[20]={};long long poteAllCommand2[20]={};
int tmpBoardCount = 0,tmpStateCount = 0;

State_t *fireState = 0;
void arrayInit(){
	for(int i=0; i<20;i++){
		maxJama[i] = 0;
		maxPoint[i] = 0;
		fireScore[i] = 0;
		poteScore[i] = 0;
		fireAllCommand[i] = 0;
		poteAllCommand[i] = 0;
		fireAllCommand2[i] = 0;
		poteAllCommand2[i] = 0;
		fireScore[i] = -99999;poteScore[i] = -99999;

	}
}
void simulate(State_t *s, int i, int j, int rot, int count, int maxHeight, int useJama, int pack[3][3]){
	long long allCommand = s->allCommand;
	long long beforeAllCommand = s->beforeAllCommand;
	long long allCommand2 = 0;
	long long beforeAllCommand2 = 0;
	if(i >=9){beforeAllCommand2 = s->beforeAllCommand2*60+(j+2)*5+rot+1;}
	else{beforeAllCommand = s->beforeAllCommand*60+(j+2)*5+rot+1;
	}
	if(i >= 10){allCommand2 = s->allCommand2*60+(j+2)*5+rot+1;}
	else{allCommand = s->allCommand*60+(j+2)*5+rot+1;
	}
	State_t *tmpState = 0;
	int fire2_ = 0;
	if(tmpBoardCount>=499000)cerr<<tmpBoardCount<<endl;cerr.flush();
	int include2 = 0;
	map<long long,State_t*> *tmpMap = 0;
	if(i>=9 && i<19){
		if(stateMap2_[(turn+1)%2].find(beforeAllCommand) != stateMap2_[(turn+1)%2].end()){
			tmpMap = stateMap2_[(turn+1)%2][beforeAllCommand];
			if(tmpMap->find(beforeAllCommand2)!= tmpMap->end()){
				include2 = 1;
			}
		}
	}
	if(include2 == 1 && attacked == 0){
		tmpState = &tState[turn%2][tmpStateCount];
		tmpStateCount++;
		stateCopyAtoB((*tmpMap)[beforeAllCommand2], tmpState, &tmpBoard[turn%2][tmpBoardCount]);
		tmpBoardCount++;
		tmpState->allCommand = allCommand;tmpState->beforeAllCommand = beforeAllCommand;
		tmpState->allCommand2 = allCommand2;tmpState->beforeAllCommand2 = beforeAllCommand2;
    	if(maxJama[i] < tmpState->oldPotePoint/5/10)maxJama[i] = tmpState->oldPotePoint/5/10;
    	if(maxPoint[i] < tmpState->oldPoint/5)maxPoint[i] = tmpState->oldPoint/5;
		tmpState->oldRandScore = getRandScore();;
    	tmpState->treeScore = tmpState->point+tmpState->oldRandScore;
    	if(poteScore[i] < tmpState->point){
    		poteScore[i] = tmpState->point;
    		poteAllCommand[i] = allCommand;
    		poteAllCommand2[i] = allCommand2;
    	}
		if(turn != 0 && attacked==false && i<count-1){
    		for(int o = 10;o<oldCount;o++){
        		long long tmpC  =1;
        		for(int ti=0;ti<o-i-1;ti++)tmpC*=60;
        		if( (beforeTurnAllCommand[o]) == beforeAllCommand && beforeTurnAllCommand2[o]/tmpC == beforeAllCommand2 ){
        			tmpState->treeScore += 999999;
        		}
        		if( (beforeTurnFireAllCommand[o]) == beforeAllCommand && (beforeTurnFireAllCommand2[o]/tmpC) == beforeAllCommand2){
        			tmpState->treeScore += 999999;
        		}
    		}
    	}
		if(stateMap2_[(turn)%2].find(allCommand) == stateMap2_[(turn)%2].end()){
			stateMap2_[(turn)%2][allCommand] = new map<long long,State_t*>;
		}
		(*stateMap2_[(turn)%2][allCommand])[allCommand2] = tmpState;

	}
	else if(i < 9 && stateMap_[(turn+1)%2].find(beforeAllCommand) != stateMap_[(turn+1)%2].end() && attacked == 0){

		tmpState = &tState[turn%2][tmpStateCount];
		tmpStateCount++;
		stateCopyAtoB(stateMap_[(turn+1)%2][beforeAllCommand], tmpState, &tmpBoard[turn%2][tmpBoardCount]);
		tmpBoardCount++;
		tmpState->allCommand = allCommand;tmpState->beforeAllCommand = beforeAllCommand;
		tmpState->allCommand2 = allCommand2;tmpState->beforeAllCommand2 = beforeAllCommand2;
    	if(maxJama[i] < tmpState->oldPotePoint/5/10)maxJama[i] = tmpState->oldPotePoint/5/10;
    	if(maxPoint[i] < tmpState->oldPoint/5)maxPoint[i] = tmpState->oldPoint/5;
		tmpState->oldRandScore = getRandScore();
    	tmpState->treeScore = tmpState->point+tmpState->oldRandScore;
    	if(poteScore[i] < tmpState->point){
    		poteScore[i] = tmpState->point;
    		poteAllCommand[i] = allCommand;
    		poteAllCommand2[i] = allCommand2;
    	}
		if(turn != 0 && attacked==false && i<count-1){
    		for(int o = i+1;o<10;o++){
        		long long tmpC  =1;
        		for(int ti=0;ti<o-i-1;ti++)tmpC*=60;
        		if( (beforeTurnAllCommand[o]/tmpC) == beforeAllCommand){
        			tmpState->treeScore += 999999;
        		}
        		if( (beforeTurnFireAllCommand[o]/tmpC) == beforeAllCommand){
        			tmpState->treeScore += 999999;
        		}
    		}
    		for(int o = 10;o<oldCount;o++){
        		long long tmpC  =1;
        		for(int ti=0;ti<9-i-1;ti++)tmpC*=60;
        		if( (beforeTurnAllCommand[o]/tmpC) == beforeAllCommand){
        			tmpState->treeScore += 999999;
        		}
        		if( (beforeTurnFireAllCommand[o]/tmpC) == beforeAllCommand){
        			tmpState->treeScore += 999999;
        		}
    		}
    	}
		stateMap_[turn%2][allCommand] = tmpState;
	}else{
		Board_t *tmp = &tmpBoard[turn%2][tmpBoardCount];
		boardCopyAtoB(s->b, tmp/*new Board()*/);//newしてる
		tmpBoardCount++;
    	fall(tmp,pack,j);

    	FireScore_t fs;
    	fsInit(&fs);
    	fire(tmp,&fs,maxHeight+3,j);
    	if(isDead_(tmp) == 1)return ;
    	int point = toScore(&fs);
    	double score = s->point - s->beforePotePoint;;
    	double randScore = 0;
		randScore = getRandScore();;

    	int aaa = maxHeight+3;if(aaa >height-3)aaa = height-3;
    	double jamaPenalty = 0/*jamaBottomPenalty(tmp)*1 + pattern(tmp)*/;;// + existUpBreak(tmp);;//+ maxNumHeight2(tmp)*0.1;
    	int fired_ = 0;
    	int semiFire = s->semiFire;
    	int potePoint = 0;
    	int tmpB = 3;
    	if(turn <= 8)tmpB = 1;

    	if(fs.chain <=0 ){//終盤標高高くなって詰みそうになった時、制限を緩める
    		potePoint =potencialChain(tmp,aaa)*10;//chain1のときだけむしろマイナスにあどで
    	}
    	else if(point<=tmpB){//1にしたい
    		score -= point*15;//Pote倍率より高くないと意味なくね？
    		potePoint =potencialChain(tmp,aaa)*10;//chain1のときだけむしろマイナスにあどで
    		semiFire = 1;
    	}
    	else{
    		fire2_ = 1;
    		double mul = 1.05;
    		double tmpA = 10;
    		if(point/5>=100 && turn+i+1 >= 15){
    			mul = //1.17;
    			1.0+((point/5)*0.0009);//1.15//////
    			if(point/5>=300){
	    			//tmpA = 10 + (point/5)*0.005;
	    		}
    		}
    		if(attackedCount < 7){mul = 1.001;tmpA = 30;}
    		score += point*tmpA*(pow(mul, count-i));
    		if(i==0)fired_ = 1;
    	}

    	if(maxJama[i] < potePoint/5/10)maxJama[i] = potePoint/5/10;
    	if(maxPoint[i] < point/5)maxPoint[i] = point/5;
    	score += randScore + potePoint + jamaPenalty;
    	State_t *st = &tState[turn%2][tmpStateCount]/*new State()*/;
    	stateInit(st);
    	tmpStateCount++;

    	s->oldChain = fs.chain;
    	st->oldPoint = point;
    	st->oldPotePoint = potePoint;
    	st->oldJamaPenalty = jamaPenalty;
    	st->oldRandScore = randScore;
    	st->fire_ = s->fire_;
    	st->allCommand = allCommand;
    	st->beforeAllCommand = beforeAllCommand;
    	st->allCommand2 = allCommand2;
    	st->beforeAllCommand2 = beforeAllCommand2;
    	stateInit(st,tmp,score,s, s->jamaStock - useJama);
		st->point -= /*potePoint +*/ randScore /*- jamaPenalty*/;
		st->semiFire = semiFire;

		if(fired_) st->fire_  =fired_;
    	if(fire2_==1 && fireScore[i] < score){
    		fireScore[i] = score;
    		fireAllCommand[i] = allCommand;
    		fireAllCommand2[i] = allCommand2;
    	}
    	if(fire2_==0 && poteScore[i] < score){
    		poteScore[i] = score;
    		poteAllCommand[i] = allCommand;
    		poteAllCommand2[i] = allCommand2;
    	}
		st->treeScore = score;
    	st->beforePotePoint = potePoint + (int)jamaPenalty;
    	tmpState = st;
    	if(turn != 0 && attacked==false && i<count-1){
    		if(i>=9){
        		for(int o = 10;o<oldCount;o++){
            		long long tmpC  =1;
            		for(int ti=0;ti<o-i-1;ti++)tmpC*=60;
            		if( (beforeTurnAllCommand[o]) == beforeAllCommand && beforeTurnAllCommand2[o]/tmpC == beforeAllCommand2 ){
            			tmpState->treeScore += 999999;
        			}
            		if( (beforeTurnFireAllCommand[o]) == beforeAllCommand && (beforeTurnFireAllCommand2[o]/tmpC) == beforeAllCommand2){
            			tmpState->treeScore += 999999;
        			}
        		}
        	}
        	else{
        		for(int o = i+1;o<10;o++){
        			long long tmpC  =1;
            		for(int ti=0;ti<o-i-1;ti++)tmpC*=60;
        			if( (beforeTurnAllCommand[o]/tmpC) == beforeAllCommand){
            			tmpState->treeScore += 999999;
            		}
            		if( (beforeTurnFireAllCommand[o]/tmpC) == beforeAllCommand){
            			tmpState->treeScore += 999999;
            		}
        		}
        		for(int o = 10;o<oldCount;o++){
            		long long tmpC  =1;
            		for(int ti=0;ti<9-i-1;ti++)tmpC*=60;
            		if( (beforeTurnAllCommand[o]/tmpC) == beforeAllCommand){
            			tmpState->treeScore += 999999;
        			}
            		if( (beforeTurnFireAllCommand[o]/tmpC) == beforeAllCommand){
            			tmpState->treeScore += 999999;
        			}
        		}
        	}
    	}
    	if(fire2_==0){
    		if(i<10){
        		stateMap_[turn%2][allCommand] = st;
        	}
        	else{
        		if(stateMap2_[(turn)%2].find(allCommand) == stateMap2_[(turn)%2].end()){
        			stateMap2_[(turn)%2][allCommand] = new map<long long,State_t*>;
        		}
        		(*stateMap2_[(turn)%2][allCommand])[allCommand2] = st;
        	}
    	}
    	if(fire2_==1 && fireState->point < st->point)fireState= st;
	}
	if(fire2_ == 0){
		stateTree_[tmpState->treeScore] = tmpState;//newしてる
	}
}
double randhaba = 40, haba0 = 100, haba = 40, haba1 = 40,haba2 = 40, haba3 = 40;
int cancel = 0;
void decideHaba(int i, int count){
	cancel = 0;
	randhaba = 40, haba0 = 100, haba = 40, haba1 = 40,haba2 = 40, haba3 = 40;

	double tmpC = 1.5 - i/20.0;//C15とC20を入れ替える p3でもいい（疑似C15でも）
	if( i<count-2){
		haba0 = 200*tmpC;haba =20*tmpC;haba1 = 20*tmpC;haba2 = 20*tmpC;haba3 = 20*tmpC;randhaba=20*tmpC;
		if(turn < 10){
				haba0 = 250*tmpC;
			}
		if(turn %2==0){//turn<10はhaba0だけふやす
			haba0 = 200*tmpC;haba =30*tmpC;haba1 = 30*tmpC;haba2 = 30*tmpC;haba3 = 30*tmpC;randhaba=30*tmpC;
			if(turn < 10){
				//haba0 = 300*tmpC;haba =50*tmpC;haba1 = 50*tmpC;haba2 = 50*tmpC;haba3 = 50*tmpC;randhaba=50*tmpC;
			}
		}
		if(turn >= 10){
			//haba =70;haba1 = 70;haba2 = 70;haba3 = 70;randhaba=70;
		}
		if(i>=10){//このときrandint10にしてみては
			//haba0 = 80*tmpC;haba = 30*tmpC;haba1 = 30*tmpC;haba2 = 30*tmpC;haba3 =30*tmpC;randhaba=30*tmpC;
		}
	}
	else{//５ターンに１階アシストむし
		cancel = 1;
		haba0 = 200;haba = 0; haba1 = 0; haba2 = 0; haba3 = 0; randhaba=0;
	}

	if(turn %3 == 2 && turn <= 21) {//p3のときはRandint調整しないと
		haba0 = 250/**tmpC*/;haba = 0;haba1 = 0;haba2 = 0;haba3 = 0;randhaba=0;
		if(turn < 10){
				haba0 = 300*tmpC;//haba =60*tmpC;haba1 = 60*tmpC;haba2 = 60*tmpC;haba3 = 60*tmpC;randhaba=60*tmpC;
			}
		if(i>=10){
			//haba0 = 300;
		}
	}
	if(remainTime< 5000){
		//haba0 = 50;haba = 0; haba1 = 0; haba2 = 0; haba3 = 0; randhaba=0;
		//System.err.println("Fast");
	}

	if( attacked ==     1){
		haba0 = 250;haba = 30;haba1 = 30;haba2 = 30;haba3 = 30;randhaba=30;
	}
	clock_t end = clock();
	if(end - start > 15000000){
		cerr << "Fast" <<(end - start)<< endl;cerr.flush();
		haba0*=0.5;haba*=0.5;haba1*=0.5;haba2*=0.5;haba3*=0.5;randhaba*=0.5;
	}
	if(delay == 1){
		haba0*=0.8;haba*=0.8;haba1*=0.8;haba2*=0.8;haba3*=0.8;randhaba*=0.8;
	}
	if(millitime <= 30000){
		haba0 =  oldCount*2+3;haba =  5;haba1 =  5;haba2 =  0;haba3 =  0;randhaba= 0;
	}
	if(count > 15){
		//haba0*=0.8;haba*=0.8;haba1*=0.8;haba2*=0.8;haba3*=0.8;randhaba*=0.8;
	}
}
int highNum =0 ;int highNum1 =0 ;int highNum2 = 0;int highNum3 = 0;

//盤面を評価値順にソートした時、どのあたりの盤面を多く選定するかを決める（上から60~80%の（評価値が低めの）盤面を選ぶようにしている）
void calcHighNum(){
	highNum =0 ;highNum1 =0 ;highNum2 = 0;highNum3 = 0;
	double highScore = 0;
	int tmpListCount = 0;
	for(it = stateTree_.begin(); it!=stateTree_.end(); it++ ){
		State_t *s = it->second;
		tmpList[tmpListCount] = s;
		tmpListCount++;
		if(highScore< s->point)highScore=s->point;
	}
	int p =0;
	double semiScore = -99999;
	for(p=0;p<tmpListCount;p++){
		if(tmpList[p]->semiFire == 0){break;}
		if(semiScore < tmpList[p]->point) semiScore = tmpList[p]->point;
	}
	int pa=0;
	//highScore -= semiScore;
	double tmp1 = 0.4;
	double tmp2 = 0.3;
	double tmp3 = 0.2;
	double tmp4 = 0.1;
	if(turn %3==1){//たまにsemifireを全くさせないのは
		p=0;
	}

	if(turn %2 == 3){
		tmp1 = 0.25;
		tmp2 = 0.2;
		tmp3 = 0.15;
		tmp4 = 0.1;
	}
	for(it = stateTree_.begin(); it!=stateTree_.end(); it++ ) {
		State_t *s = it->second;
		if(pa >= stateTree_.size()-haba0-1)break;
		if(s->point/* - semiScore*/>=highScore*tmp1 && pa>=p)highNum++;//ここかえる
		if(s->point/* - semiScore*/>=highScore*tmp2 && pa>=p)highNum1++;
		if(s->point/* - semiScore*/>=highScore*tmp3 && pa>=p)highNum2++;
		if(s->point/* - semiScore*/>=highScore*tmp4 && pa>=p)highNum3++;
		pa++;
	}
	highNum3 -= highNum2;highNum2 -= highNum1;highNum1 -= highNum;


	if(turn %2 == 0){
		highNum3 = (stateTree_.size()-p)*0.1;//paつかってうまくやる
		highNum2 = (stateTree_.size()-p)*0.1;
		highNum1 = (stateTree_.size()-p)*0.1;
		highNum = (stateTree_.size()-p)*0.6;
	}
	/*
	else{
		highNum3 = (stateTree_.size()-p)*0.2;//paつかってうまくやる
		highNum2 = (stateTree_.size()-p)*0.2;
		highNum1 = (stateTree_.size()-p)*0.2;
		highNum = (stateTree_.size()-p)*0.2;
	}*/

	if(haba  > highNum -haba0){   haba1 += haba -highNum-haba0 ;haba  = highNum-haba0 ;}
	if(haba1 > highNum1){   haba2 += haba1-highNum1;haba1 = highNum1;}
	if(haba2 > highNum2){   haba3 += haba2-highNum2;haba2 = highNum2;}
	if(haba3 > highNum3){randhaba += haba3-highNum3;haba3 = highNum3;}
}
int addNextState(int i){
	int addNum = 0, okNum = 0, randNum = 0,okNum0 = 0,okNum1=0,okNum2=0,okNum3=0;
	int len = stateTree_.size();
	int stateListCount = 0;
	for(int q=0;q<len;q++){
		okNum0++;
		if((i!=0 && q >= haba0) ){
			okNum0--;
			if(     q<=/*haba0+*/highNum ){
				okNum ++;
				if( highNum !=0 && getRand() >= haba /highNum   ){
					okNum --;continue;
				}
			}
			else if(q<=/*haba0+*/highNum+highNum1 ){
				okNum1++;
				if( highNum1!=0 && getRand() >= haba1/(highNum1)){
					okNum1--;continue;
				}
			}
			else if(q<=/*haba0+*/highNum+highNum1+highNum2 ){
				okNum2++;
				if( highNum2!=0 && getRand() >= haba2/(highNum2)){
					okNum2--;continue;
				}
			}
			else if(q<=/*haba0+*/highNum+highNum1+highNum2+highNum3){
				okNum3++;
				if( highNum3!=0 && getRand() >= haba3/(highNum3)){
					okNum3--;continue;
				}
			}
			else{
				randNum++;
				if(getRand() >= randhaba/(len-highNum-highNum1-highNum2-highNum3)){
					randNum--;continue;
				}
			}
		}
		stateList[stateListCount] = tmpList[len-q-1];
		stateListCount++;
		addNum++;
	}
	//cerr<<"\ti:"<<i<<" hN:"<<highNum<<" hN1:"<<highNum1<<" hN2:"<<highNum2<<" hN3:"<<highNum3<<" hN4:"<<(len-highNum-highNum1-highNum2-highNum3)<<endl;
	//cerr<<"\taddN:"<<addNum<<" ok0:"<<okNum0<<" ok:"<<okNum<<" ok1:"<<okNum1<<" ok2:"<<okNum2<<" ok3:"<<okNum3<<" ok4:"<<randNum<<endl;
	cerr.flush();
	return stateListCount;
}
void chokudai(State_t *ans){
	int count = calcCount();
	mapClear();
	arrayInit();
	tmpBoardCount = 0;tmpStateCount = 0;
	fireState = ans;
	State_t state ;stateInit(&state);state.b = &my;state.jamaStock = my.obstacleNum;state.beforeAllCommand = (oldCol+2)*5+oldRot+1;
	stateList[0] = &state;
	finalTree_[-999999] = &state;
	int stateListCount = 1;
	start = clock();
	for(int i=0;i<count;i++){
		gi = i;
		stateTree_.clear();
		for(int r=0;r<stateListCount;r++){
			State_t *s = stateList[r];
        	int maxHeight = calcMaxHeight(s->b);
			for(int rot=0;rot<4;rot++){
				int pack[PACK_SIZE][PACK_SIZE];
				fillObstaclePack(allPack[turn+i],pack, s->jamaStock);//new減らせる
    			int useJama = calcUseJama(pack);
    			packRotate(pack, rot);
                int left = getLDrop(pack);int right = getRDrop(pack);
                for(int j=-left;j<=right; j++){
                	simulate(s,i,j,rot,count, maxHeight, useJama, pack);
                }
			}
		}
		if(stateTree_.size() == 0)break;
		decideHaba(i,count);
		calcHighNum();
		stateListCount = addNextState(i);
		finalTree_[stateList[0]->point] = stateList[0];
	}
	State_t *ret = stateList[0];
	//for(it = finalTree_.begin(); it!=finalTree_.end(); it++ ) {
	//	ret = it->second;
    //}
	if(fireState != 0 && ret->point < fireState->point)ret = fireState;
	*ans = *ret;

	for(int i=0; i<count;i++){cerr<<"i:"<<i<<" mJ:"<<maxPoint[i]<<" mPJ:"<<maxJama[i]<<endl;}
	int pack[3][3];fillObstaclePack(allPack[turn],pack, my.obstacleNum);
	oldJama = my.obstacleNum - calcUseJama(pack);
	for(int i=0;i<20;i++){
		beforeTurnFireAllCommand[i] = fireAllCommand[i];
		beforeTurnAllCommand[i] = poteAllCommand[i];
		beforeTurnFireAllCommand2[i] = fireAllCommand2[i];
		beforeTurnAllCommand2[i] = poteAllCommand2[i];
	}
	oldCount = count;
}

void run() {
    cout << "Kera" << endl;cout.flush();
   startInput();
	long long oldMilliTime = millitime;
   while (1) {
   		oldMilliTime = millitime;
       	cin >> turn; cin >> millitime;
        boardInput(&my,height, width);
        boardInput(&op,height, width);
        attacked = 0;
        if(oldJama<my.obstacleNum){
        	attacked = 1;
            if(oldJama+30<my.obstacleNum){attackedCount = 0; }
        }
        if(turn == 5 && millitime <= 150000)  {
        	delay = 1;
        }
        cerr << "turn=" << turn <<  " militime:"<<millitime<<"useTime:"<<(oldMilliTime - millitime)<<endl;cerr.flush();
        State_t s;
        stateInit(&s);
        s.allCommand = 0; s.point = -99999;
        chokudai(&s);
        attackedCount++;
    	long long command = s.allCommand;
    	while(command>=60){command/=60;}
    	int col = (int)command/5-2;int rot = (int)command%5-1;
    	cout << col<< " " <<rot << endl;
        oldCol = col;  oldRot = rot;
    	cout.flush();
    }
}
int main(void){
	run();
}

