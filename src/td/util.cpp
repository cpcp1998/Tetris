#ifndef NOT_STAND_ALONE_FIND

#include<array>
#include<vector>
#include<queue>
#include<cstring>
#include<iostream>
#include<random>
#include<ctime>
#include<cmath>
#include<random>

#define BATCH 1000
#define LAMBDA 0.99
double epsilon = 1;
double alpha = 0.01;
int epoch = 0;
#define MINVALUE -10000000
#define MAXVALUE  10000000

using namespace std;

mt19937 engine(time(0));
uniform_real_distribution<double> distri(0,1);

#define MAPWIDTH 10
#define MAPHEIGHT 20
static const int blockShape[7][4][8] = {
	{ { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
	{ { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
	{ { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
	{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
	{ { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
	{ { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
	{ { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
};// 7种形状(长L| 短L| 反z| 正z| T| 直一| 田格)，4种朝向(上左下右)，8:每相邻的两个分别为x，y

#endif

static const int effiShape[7]={4,4,2,2,4,2,1}; 
static const int Xbegin[7][4]={{2,1,2,2},{2,1,2,2},{2,1,2,2},{2,1,2,2},{2,2,2,1},{1,3,1,2},{2,2,1,1}};
static const int Xend[7][4]={{9,9,9,10},{9,9,9,10},{9,9,9,10},{9,9,9,10},{9,10,9,9},{10,9,10,8},{10,10,9,9}};
static const int Ybegin[7][4]={{2,2,1,2},{2,2,1,2},{2,2,1,2},{2,2,1,2},{1,2,2,2},{2,1,3,1},{1,2,2,1}};
static const int Yend[7][4]={{20,19,19,19},{20,19,19,19},{20,19,19,19},{20,19,19,19},{19,19,20,19},{18,20,19,20},{19,20,20,19}};
static const int Ymax[7][4] = { {0,1,1,1},{0,1,1,1},{0,1,1,1},{0,1,1,1},{1,1,0,1},{2,0,1,0},{1,0,0,1} };
static const int Ymin[7][4] = { {-1,-1,0,-1},{-1,-1,0,-1},{-1,-1,0,-1},{-1,-1,0,-1},{0,-1,-1,-1},{-1,0,-2,0},{0,-1,-1,0} };

struct Block{
	int t;   // 标记方块类型的序号 0~6
	int x;            // 旋转中心的x轴坐标
	int y;            // 旋转中心的y轴坐标
	int o;       // 标记方块的朝向 0~3
};

struct Board{
	int grid[MAPHEIGHT + 2][MAPWIDTH + 2];
	Board(int g[][MAPWIDTH + 2]){
		memcpy(grid,g,sizeof grid);
	}
};

inline void printField(const Board& board)
{
	static const char *i2s[] = {
		"~~",
		"~~",
		"  ",
		"[]",
		"##"
	};
	cout << "~~：墙，[]：块，##：新块" << endl;
	for (int y = MAPHEIGHT + 1; y >= 0; y--)
	{
		for (int x = 0; x <= MAPWIDTH + 1; x++)
			cout << i2s[board.grid[y][x] + 2];
		cout << endl;
	}
}

//assume the range of x and y of block is right
inline bool isValid(const Block& block, const Board& board)
{
	for (int i = 0; i < 4; i++)
	{
		int tmpX = block.x + blockShape[block.t][block.o][2 * i];
		int tmpY = block.y + blockShape[block.t][block.o][2 * i + 1];
		if (//tmpX < 1 || tmpX > MAPWIDTH ||
			//tmpY < 1 || tmpY > MAPHEIGHT ||
			board.grid[tmpY][tmpX] != 0)
			return false;
	}
	return true;
}

inline Block moveDown(const Block& block){
	Block ret = block;
	--ret.y;
	return ret;
}

inline bool onGround(const Block& block, const Board& board)
{
	if (isValid(block,board) && !isValid(moveDown(block),board))
		return true;
	return false;
}

inline bool rotation(const Block& block, const Board& board, int dest)
{
	Block temp = block;
	while (true)
	{
		if (!isValid(temp, board))
			return false;
		if (temp.o == dest)
			break;
		temp.o = (temp.o + 1) % 4;
	}
	return true;
}

//assume the place is correct
inline bool place(const Block& block, Board& board)
{
	//if (!onGround(block, board))
	//	return false;
	int i, tmpX, tmpY;
	for (i = 0; i < 4; i++)
	{
		tmpX = block.x + blockShape[block.t][block.o][2 * i];
		tmpY = block.y + blockShape[block.t][block.o][2 * i + 1];
		board.grid[tmpY][tmpX] = 1;
	}
	return true;
}

inline bool checkDirectDropTo(const Block& block, const Board& board)
{
	const int x=block.x;
	int y=block.y;
	auto &def = blockShape[block.t][block.o];
	for (; y <= MAPHEIGHT; y++)
		for (int i = 0; i < 4; i++)
		{
			int _x = def[i * 2] + x, _y = def[i * 2 + 1] + y;
			if (_y > MAPHEIGHT)
				continue;
			if (_y < 1 || _x < 1 || _x > MAPWIDTH || board.grid[_y][_x])
				return false;
		}
	return true;
}

inline int eliminate(Board& board, int start = 1, int end = 20)
{
	int count = 0;
	for (int i = start; i <= MAPHEIGHT; i++)
	{
		if (i > end) {
			if (!count) return 0;
			memmove(board.grid[i - count], board.grid[i],12*sizeof(int)*(20-end) );
			memset(board.grid[21 - count], 0, 12 * sizeof(int)*count);
			return count;
		}
		int emptyFlag = 1;
		int fullFlag = 1;
		for (int j = 1; j <= MAPWIDTH; j++)
		{
			if (board.grid[i][j] == 0)
				fullFlag = 0;
			else
				emptyFlag = 0;
			if(!fullFlag&&!emptyFlag) break;
		}
		if (fullFlag)
		{
			for (int j = 1; j <= MAPWIDTH; j++)
			{
				board.grid[i][j] = 0;
			}
			count++;
		}
		else if (emptyFlag)
		{
			break;
		}
		else if(count){
			memcpy(board.grid[i-count],board.grid[i],12*sizeof(int));
			memset(board.grid[i]+1,0,10*sizeof(int));
		}
	}
	return count;
}

inline bool canPut(int blockType, const Board& board)
{
	for (int y = MAPHEIGHT; y >= 1; y--)
		for (int x = 1; x <= MAPWIDTH; x++)
			for (int o = 0; o < 4; o++)
			{
				Block block{blockType,x,y,o};
				if (isValid(block, board) && checkDirectDropTo(block, board))
					return true;
			}
	return false;
}

#define MOVEIN if(block.x >= 1 && block.x <= MAPWIDTH &&\
	 block.y >= 1 && block.y <= MAPHEIGHT &&\
	!valid[block.x][block.y][block.o]&&isValid(block, board))\
	{valid[block.x][block.y][block.o] = true; validList[end++]=block;}

inline vector<Block> allPossibleState(int blockType, const Board& board){
	bool valid[MAPWIDTH+2][MAPHEIGHT+2][4]={};
	Block validList[800];
	int begin = 0, end = 0;
	for (int y = MAPHEIGHT; y >= 1; y--)
		for (int x = 1; x <= MAPWIDTH; x++)
			for (int o = 0; o < 4; o++)
			{
				Block block{blockType,x,y,o};
				if (isValid(block, board) && checkDirectDropTo(block, board)){
					valid[block.x][block.y][block.o]=true;
					validList[end++]=block;
				}
			}
	while(begin!=end){
		Block block = validList[begin++];
		block.y--;
		MOVEIN
		block.y++; block.x--;
		MOVEIN
		block.x++; block.x++;
		MOVEIN
		block.x--; block.o=(block.o+1)%4;
		MOVEIN
	}
	vector<Block> ret;
	for (int x = 1; x <= MAPWIDTH; x++)
		for (int y = 1; y <= MAPHEIGHT; y++)
			for (int o = 0; o < 4; o++)
				if(valid[x][y][o]&&onGround(Block{blockType,x,y,o}, board))
					ret.push_back(Block{blockType,x,y,o});
	return ret;				
}

inline int simpleMoves(int blockType, const Board& board, Block* ret){
	int count = 0;
	int altitude[12]={};
	altitude[0]=altitude[MAPWIDTH+1]=MAPHEIGHT;
	for(int x=1;x<=MAPWIDTH;x++){
		for(int y=1;y<=MAPHEIGHT;y++)
			if(board.grid[y][x]) altitude[x]=y;
	}
	for(int o=0;o<effiShape[blockType];o++){
		int X[4];
		for(int i=0;i<4;++i){
			X[i]=blockShape[blockType][o][2*i]+Xbegin[blockType][o];
		}
		for(int x=Xbegin[blockType][o];x<=Xend[blockType][o];x++){
			int lowest = 0;
			for(int i=0;i<4;i++){
				int height = altitude[X[i]++];
				int y = blockShape[blockType][o][2*i+1];
				int delta = height - y + 1;
				if(delta > lowest) lowest = delta;
			}
			if(lowest<=Yend[blockType][o])
				ret[count++]=(Block{blockType,x,lowest,o});
		}
	}
	return count;				
}

//features


typedef int* Altitude; // int[12]

//terrible bug
Altitude altitude(const Board& board, Altitude ret){
	ret[0]=ret[MAPWIDTH+1]=MAPHEIGHT;
	for(int x=1;x<=MAPWIDTH;x++){
		ret[x]=0;
		for(int y=MAPHEIGHT;y>=1;y--){
			if(board.grid[y][x]){
				ret[x]=y; break;
			}
		}
	}
	return ret;
}
/*
double landHeight(const Block& block) {
	int min = 100, max = -100;
	for (int i = 0; i < 4; i++)
	{
		int tmpY = block.y + blockShape[block.t][block.o][2 * i + 1];
		if(tmpY>max) max=tmpY;
		if(tmpY<min) min=tmpY;
	}
	return (max+min)/2.0;
}
*/
double rowTransitions(const Board& board){
	int count=0;
	for(int y=1;y<=MAPHEIGHT;y++){
		for(int x=1;x<MAPWIDTH;x++)
			if(board.grid[y][x]!=board.grid[y][x+1]) count++;
		if(!board.grid[y][1]) count++;
		if(!board.grid[y][MAPWIDTH]) count++;
	}
		
	return count;
}

double colTransitions(const Board& board){
	int count=0;
	for(int y=1;y<MAPHEIGHT;y++)
		for(int x=1;x<=MAPWIDTH;x++){
			if(board.grid[y][x]!=board.grid[y+1][x]) count++;
		}
	for(int x=1;x<=MAPWIDTH;x++)
		if(!board.grid[1][x]) count++;
	return count;
}

double numHoles(const Board& board){
	int count=0;
	for(int x=1;x<=MAPWIDTH;x++){
		int cc=0;
		for(int y=1;y<=MAPHEIGHT;y++)
			if(board.grid[y][x]){
				count+=cc;
				cc=0;
			}else ++cc;
	}
	return count;
}

double connectdHoles(const Board& board){
	int count=0;
	for(int x=1;x<=MAPWIDTH;x++){
		int cc=0;
		for(int y=1;y<=MAPHEIGHT;y++)
			if(board.grid[y][x]){
				count+=cc;
				cc=0;
			}else cc=1;
	}
	return count;
}

double wellSums(const Board& board){
	int count=0;
	for(int y=1;y<=MAPHEIGHT;y++){
		for(int x=2;x<MAPWIDTH;x++){
			if(!board.grid[y][x]&&board.grid[y][x+1]&&board.grid[y][x-1]) count++;
			
		}
		if(!board.grid[y][1]&&board.grid[y][2]) count++;
		if(!board.grid[y][MAPWIDTH]&&board.grid[y][MAPWIDTH-1]) count++;
	}		
	return count;
}

double pileHeight(const Altitude& alti){
	int max = 0;
	for(int i=1; i<=MAPWIDTH;i++)
		if(alti[i]>max) max = alti[i];
	return max;
}


double altitudeDiff(const Altitude& alti){
	int min = 100; int max = -100;
	for(int i=1;i<=MAPWIDTH;i++){
		if(alti[i]<min) min=alti[i];
		if(alti[i]>max) max=alti[i];
	}
	return max-min;
}

double wellDepth(const Altitude& alti){
	int max = 0;
	for(int i=1;i<=MAPWIDTH;++i){
		if(alti[i]<alti[i-1]&&alti[i]<alti[i+1]){
			int ld = alti[i-1]-alti[i];
			int rd = alti[i+1]-alti[i];
			int depth = (ld>rd)?rd:ld;
			if(depth>max) max=depth;
		}
	}
	return max;
}

double wellSum(const Altitude& alti){
	int total = 0;
	for(int i=1;i<=MAPWIDTH;++i){
		if(alti[i]<alti[i-1]&&alti[i]<alti[i+1]){
			int ld = alti[i-1]-alti[i];
			int rd = alti[i+1]-alti[i];
			int depth = (ld>rd)?rd:ld;
			total += depth;
		}
	}
	return total;
}

double weightedBlock(const Board& board){
	int total = 0;
	for (int i = 1; i <= MAPHEIGHT; i++)
	{
		int linesum = 0;
		for (int j = 1; j <= MAPWIDTH; j++)
		{
			linesum+=board.grid[i][j];
		}
		if (!linesum)
		{
			break;
		}else{
			total+=linesum*i;
		}
	}
	return total;
}

typedef double* Param;

double value(const Board& board, const int *blockCount){
	int alti[12];
	altitude(board, alti);
	return
			 4.592814
			-0.466936*log(21-pileHeight(alti))	*1
			-1.840799*rowTransitions(board)		*0.03
			+0.281079*colTransitions(board)		*0.03
			-0.799894*numHoles(board)			*0.04
			-0.037264*altitudeDiff(alti)		*0.25
			+0.031132*wellDepth(alti)			*0.3
			-0.211828*wellSum(alti)				*0.15;			
}
/* 
void update(const Board& board, const int *blockCount, Param param, double delta){
	int alti[12];
	altitude(board, alti);
	//for(int i=0;i<12;i++) cout<<alti[i]<<'\t'; cout<<endl; system("pause");
	param[0]+=1									*delta;
	param[1]+=log(21-pileHeight(alti))	*1		*delta;
	param[2]+=rowTransitions(board)		*0.03	*delta;
	param[3]+=colTransitions(board)		*0.03	*delta;
	param[4]+=numHoles(board)			*0.04	*delta;
	param[5]+=altitudeDiff(alti)		*0.25	*delta;
	param[6]+=wellDepth(alti)			*0.3	*delta;
	param[7]+=wellSum(alti)				*0.15	*delta;
}

double newvalue(const Board& board, int *blockCount, Param param){
	int opChoice[7]; int ptoOC = 0;
	int maxCount = 0, minCount = 99;
	for (int i = 0; i < 7; i++)
	{
		if (blockCount[i] > maxCount)
			maxCount = blockCount[i];
		if (blockCount[i] < minCount)
			minCount = blockCount[i];
	}
	for(int i = 0; i < 7; i++)
		if(blockCount[i]<minCount+2) opChoice[ptoOC++] = i;
	
	double worst = MAXVALUE;
	for(int i=0;i<ptoOC; i++){
		blockCount[i]++;
		Block moves[40];
		int size = simpleMoves(i, board, moves);
		double best = MINVALUE;
		
		if(size)
			if(distri(engine)>epsilon)
				for (int i = 0; i < size;i++) {
					Board b = board; place(moves[i],b);
					double se = eliminate(b);
					se += value(b,blockCount,param)*LAMBDA;
					if(se>best) best = se;
				}
			else{
				uniform_int_distribution<int> ran(0,size-1);
				Board b = board; place(moves[ran(engine)],b);
				double se = eliminate(b);
				se += value(b,blockCount,param)*LAMBDA;
				best = se;
			}
		else best = 0;
		
		blockCount[i]--;
		if(best<worst) worst = best;
	}
}

double param[8]={7,-1,-1,-1,-1,-1,-1,-1};

int main(){
	cout<<fixed;
	int grid[MAPHEIGHT + 2][MAPWIDTH + 2] ={};
	for (int i = 0; i < MAPHEIGHT + 2; i++)
		grid[i][0] = grid[i][MAPWIDTH + 1] = -2;
	for (int i = 0; i < MAPWIDTH + 2; i++)
		grid[0][i] = grid[MAPHEIGHT + 1][i] = -2;
	Board blank(grid);
	Board board = blank;
	int blockCount[7] = {0};
	while(true){
		board = blank;
		memset(blockCount, 0, sizeof blockCount);
		cout<<"epsilon: "; cin>>epsilon;
		cout<<"learning rate: "; cin>>alpha;
		cout<<"times: "; int times; cin>>times;
		double totalLoss = 0;
		for(int i = 0; i < times; i++){
			cout<<"epoch: "<<epoch++<<'\t';
			double loss = 0; double delta = 0;
			double paramDelta[8] ={};
			for(int j = 0; j <BATCH; j++){
				double curr = value(board,blockCount,param);
				double newv = newvalue(board,blockCount,param);
				loss += (newv - curr)*(newv - curr);
				double delta = (newv - curr);
				update(board,blockCount,paramDelta,delta);
				
				//update board
				int opChoice[7]; int ptoOC = 0;
				int maxCount = 0, minCount = 99;
				for (int i = 0; i < 7; i++)
				{
					if (blockCount[i] > maxCount)
						maxCount = blockCount[i];
					if (blockCount[i] < minCount)
						minCount = blockCount[i];
				}
				for(int i = 0; i < 7; i++)
					if(blockCount[i]<minCount+2) opChoice[ptoOC++] = i;
				
				double worst = MAXVALUE;
				int choice; Board newboard = board;
				for(int i=0;i<ptoOC; i++){
					blockCount[i]++;
					Block moves[40];
					int size = simpleMoves(i, board, moves);
					double best = MINVALUE;
					Board nb = board;
					
					if(size)
						if(distri(engine)>epsilon)
							for (int i = 0; i < size;i++) {
								Board b = board; place(moves[i],b);
								double se = eliminate(b);
								se += value(b,blockCount,param)*LAMBDA;
								if(se>best){best = se; nb = b;}
							}
						else{
							uniform_int_distribution<int> ran(0,size-1);
							Board b = board; place(moves[ran(engine)],b);
							double se = eliminate(b);
							se += value(b,blockCount,param)*LAMBDA;
							best = se;
							nb = b;
						}
					else goto newgame;
					
					blockCount[i]--;
					if(best<worst) {
						worst = best;
						newboard = nb;
					}
				}
				board = newboard;
				blockCount[choice]++;
				//printField(board);
				//system("pause");	
				goto finish;
newgame:
				board = blank;
				memset(blockCount, 0, sizeof blockCount);
				cout<<'n';
finish:
				;
			}
			cout<<"loss: "<<loss<<endl;
			totalLoss += loss/BATCH;
			for(int i=0;i<8;i++)
				param[i]+=alpha*paramDelta[i]/BATCH; 
		}
		cout<<"----------------------------"<<endl;
		for(int i=0;i<8;i++) cout<<i<<"\t\t"; cout<<endl;
		for(int i=0;i<8;i++) cout<<param[i]<<"\t"; cout<<endl;
		cout<<"Loss: "<<totalLoss/times<<endl;
	}
}*/ 


