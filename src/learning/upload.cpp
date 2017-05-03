/**
 * Tetris �򵥽�����������
 * https://wiki.botzone.org/index.php?title=Tetris
 * ������2017��4��20�գ�
 * ������rotation��������������ʽ�޸�Ϊ�¹���ĸ�ʽ������transfer������`if (h2 >= MAPHEIGHT)`��Ϊ`if (h2 > MAPHEIGHT)`
 */
// ע�⣺x�ķ�Χ��1~MAPWIDTH��y�ķ�Χ��1~MAPHEIGHT
// ���������У�y�����У�c��
// ����ϵ��ԭ�������½�

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <array>
#include <vector>
#include <queue>
#include <cstring>
#include <random>
using namespace std;

#define MAPWIDTH 10
#define MAPHEIGHT 20

// �����ڶ������ɫ��0Ϊ�죬1Ϊ��������ʾ���飬�����Ⱥ�
int currBotColor;
int enemyColor;

// ��y��x����¼��ͼ״̬��0Ϊ�գ�1Ϊ��ǰ���ã�2Ϊ�ոշ��ã�����ΪԽ��
// ��2���������к����һ���������͸��Է���
int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2] = { 0 };

// ����ֱ���Է�ת�Ƶ���
int trans[2][4][MAPWIDTH + 2] = { 0 };

// ת������
int transCount[2] = { 0 };

// ����eliminate��ĵ�ǰ�߶�
int maxHeight[2] = { 0 };

// ����ȥ�����ķ���֮��
int elimTotal[2] = { 0 };

// һ������ȥ������Ӧ����
const int elimBonus[] = { 0, 1, 3, 5, 7 };

// ����Ӧ��ҵĸ�������Ŀ�ܼ�
int typeCountForColor[2][7] = { 0 };

const int blockShape[7][4][8] = {
	{ { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
	{ { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
	{ { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
	{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
	{ { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
	{ { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
	{ { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
};// 7����״(��L| ��L| ��z| ��z| T| ֱһ| ���)��4�ֳ���(��������)��8:ÿ���ڵ������ֱ�Ϊx��y

#define NOT_STAND_ALONE_FIND

#ifndef NOT_STAND_ALONE_FIND

#include<array>
#include<vector>
#include<queue>
#include<cstring>
#include<iostream>
#include<random>
#include<ctime>

using namespace std;

#define MAPWIDTH 10
#define MAPHEIGHT 20
const int blockShape[7][4][8] = {
	{ { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
	{ { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
	{ { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
	{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
	{ { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
	{ { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
	{ { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
};// 7����״(��L| ��L| ��z| ��z| T| ֱһ| ���)��4�ֳ���(��������)��8:ÿ���ڵ������ֱ�Ϊx��y

#endif

struct Block{
	int t;   // ��Ƿ������͵���� 0~6
	int x;            // ��ת���ĵ�x������
	int y;            // ��ת���ĵ�y������
	int o;       // ��Ƿ���ĳ��� 0~3
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
	cout << "~~��ǽ��[]���飬##���¿�" << endl;
	for (int y = MAPHEIGHT + 1; y >= 0; y--)
	{
		for (int x = 0; x <= MAPWIDTH + 1; x++)
			cout << i2s[board.grid[y][x] + 2];
		cout << endl;
	}
}

inline bool isValid(const Block& block, const Board& board)
{
	int i, tmpX, tmpY;
	for (i = 0; i < 4; i++)
	{
		tmpX = block.x + blockShape[block.t][block.o][2 * i];
		tmpY = block.y + blockShape[block.t][block.o][2 * i + 1];
		if (tmpX < 1 || tmpX > MAPWIDTH ||
			tmpY < 1 || tmpY > MAPHEIGHT ||
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

inline bool place(const Block& block, Board& board)
{
	if (!onGround(block, board))
		return false;
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

inline int eliminate(Board& board)
{
	int count = 0;
	int i, j, emptyFlag, fullFlag;
	for (i = 1; i <= MAPHEIGHT; i++)
	{
		emptyFlag = 1;
		fullFlag = 1;
		for (j = 1; j <= MAPWIDTH; j++)
		{
			if (board.grid[i][j] == 0)
				fullFlag = 0;
			else
				emptyFlag = 0;
		}
		if (fullFlag)
		{
			for (j = 1; j <= MAPWIDTH; j++)
			{
				board.grid[i][j] = 0;
			}
			count++;
		}
		else if (emptyFlag)
		{
			break;
		}
		else if(count)
			for (j = 1; j <= MAPWIDTH; j++)
			{
				board.grid[i - count][j] = board.grid[i][j];
				if (count)
					board.grid[i][j] = 0;
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

//features

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

//caution: board should not be eliminated, but the block should be placed.
double rowEliminated(const Board& board){
	int count = 0;
	int i, j, emptyFlag, fullFlag;
	for (i = 1; i <= MAPHEIGHT; i++)
	{
		emptyFlag = 1;
		fullFlag = 1;
		for (j = 1; j <= MAPWIDTH; j++)
		{
			if (board.grid[i][j] == 0)
				fullFlag = 0;
			else
				emptyFlag = 0;
		}
		if (fullFlag)
		{
			count++;
		}
		else if (emptyFlag)
		{
			break;
		}
	}
	return count;
}

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

double pileHeight(const Board& board){
	for (int i = 1; i <= MAPHEIGHT; i++)
	{
		int emptyFlag = 1;
		for (int j = 1; j <= MAPWIDTH; j++)
		{
			if (board.grid[i][j])
				emptyFlag = 0;
		}
		if (emptyFlag)
		{
			return i-1;
		}
	}
	return MAPHEIGHT;
}

typedef array<int,MAPWIDTH+2> Altitude;

Altitude altitude(const Board& board){
	Altitude ret={};
	ret[0]=ret[MAPWIDTH+1]=MAPHEIGHT;
	for(int x=1;x<=MAPWIDTH;x++){
		for(int y=1;y<=MAPHEIGHT;y++)
			if(board.grid[y][x]) ret[x]=y;
	}
	return ret;
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

typedef array<double,12> Param;

double value(const Block& block, const Board& board, Param param){
	double ret=0;
	Board b = board;
	ret+=param[0]*landHeight(block);
	place(block,b);
	ret+=param[1]*rowEliminated(b);
	eliminate(b);
	ret+=param[2]*rowTransitions(b);
	ret+=param[3]*colTransitions(b);
	ret+=param[4]*numHoles(b);
	ret+=param[5]*wellSums(b);
	ret+=param[6]*connectdHoles(b);
	ret+=param[7]*pileHeight(b);
	Altitude alti = altitude(b);
	ret+=param[8]*altitudeDiff(alti);
	ret+=param[9]*wellDepth(alti);
	ret+=param[10]*wellSum(alti);
	ret+=param[11]*weightedBlock(b);
	return ret;
}

//Block defenser(int blockType, const Board& board);
//int attacker(int blockCount[7], const Board& board);
//assume both work right
template<typename T1, typename T2>
int simulate(T1 defenser, T2 attacker, int depth){
	int grid[MAPHEIGHT + 2][MAPWIDTH + 2]={};
	for (int i = 0; i < MAPHEIGHT + 2; i++)
	{
		grid[i][0] = grid[i][MAPWIDTH + 1] = -2;
		grid[i][0] = grid[i][MAPWIDTH + 1] = -2;
	}
	for (int i = 0; i < MAPWIDTH + 2; i++)
	{
		grid[0][i] = grid[MAPHEIGHT + 1][i] = -2;
		grid[0][i] = grid[MAPHEIGHT + 1][i] = -2;
	}
	Board board{grid};
	int blockCount[7]={};
	int t=0;
	int eli=0;
	for(t=0;t<depth;++t){
		int blockType = attacker(blockCount,board);
		if(allPossibleState(blockType,board).empty()) break;
		Block block = defenser(blockType, board);
		place(block,board);
		eli+=eliminate(board);
		blockCount[blockType]++;
#ifndef NDEBUG
		printField(board);
		cout<<"Round: "<<t<<endl;
		cout<<"Elimiated lines: "<<eli<<endl;
		system("pause");
#endif
	}
	t-=depth;
	if(t==0){
		t=MAPHEIGHT-pileHeight(board);
	}
	return t;
} 

vector<int> availTypes(int* blockCount){
	vector<int> ret;
	int maxCount = 0, minCount = 99;
	for (int i = 0; i < 7; i++)
	{
		if (blockCount[i] > maxCount)
			maxCount = blockCount[i];
		if (blockCount[i] < minCount)
			minCount = blockCount[i];
	}
	for(int i = 0; i < 7; i++)
		if(blockCount[i]<minCount+2) ret.push_back(i);
	return ret;
}

mt19937 randomEngine(time(0));

struct paramDefenser{
	Param param;
	paramDefenser(Param p):param(p){
	}
	Block operator()(int blockType, const Board& board){
		vector<Block> all = allPossibleState(blockType,board);
		double max=-10000000;
		Block best;
		for(Block block : all){
			double v = value(block, board, param);
			if(v>max){
				max = v;
				best = block;
			}
		}
		return best;
	}
};

struct paramAttacker{
	Param param;
	paramAttacker(Param p):param(p){
	}
	int operator()(int blockCount[7], const Board& board){
		vector<int> avail = availTypes(blockCount);
		double min=10000000; int best;
		for(int blockType:avail){
			vector<Block> all = allPossibleState(blockType,board);
			double max=-10000000;
			for(Block block : all){
				double v = value(block, board, param);
				if(v>max){
					max = v;
				}
			}
			if(max<min){
				min=max;
				best = blockType;
			}
		}
		return best;
	}
};

int randomAttacker(int blockCount[7], const Board& board){
	vector<int> avail = availTypes(blockCount);
	int s = avail.size();
	return avail[uniform_int_distribution<int>(0,s-1)(randomEngine)];
}


class Tetris
{
public:
	const int blockType;   // ��Ƿ������͵���� 0~6
	int blockX;            // ��ת���ĵ�x������
	int blockY;            // ��ת���ĵ�y������
	int orientation;       // ��Ƿ���ĳ��� 0~3
	const int(*shape)[8]; // ��ǰ���ͷ������״����

	int color;

	Tetris(int t, int color) : blockType(t), shape(blockShape[t]), color(color)
	{ }

	inline Tetris &set(int x = -1, int y = -1, int o = -1)
	{
		blockX = x == -1 ? blockX : x;
		blockY = y == -1 ? blockY : y;
		orientation = o == -1 ? orientation : o;
		return *this;
	}

	// �жϵ�ǰλ���Ƿ�Ϸ�
	inline bool isValid(int x = -1, int y = -1, int o = -1)
	{
		x = x == -1 ? blockX : x;
		y = y == -1 ? blockY : y;
		o = o == -1 ? orientation : o;
		if (o < 0 || o > 3)
			return false;

		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = x + shape[o][2 * i];
			tmpY = y + shape[o][2 * i + 1];
			if (tmpX < 1 || tmpX > MAPWIDTH ||
				tmpY < 1 || tmpY > MAPHEIGHT ||
				gridInfo[color][tmpY][tmpX] != 0)
				return false;
		}
		return true;
	}

	// �ж��Ƿ����
	inline bool onGround()
	{
		if (isValid() && !isValid(-1, blockY - 1))
			return true;
		return false;
	}

	// ����������ڳ�����
	inline bool place()
	{
		if (!onGround())
			return false;

		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = blockX + shape[orientation][2 * i];
			tmpY = blockY + shape[orientation][2 * i + 1];
			gridInfo[color][tmpY][tmpX] = 2;
		}
		return true;
	}

	// ����ܷ���ʱ����ת�Լ���o
	inline bool rotation(int o)
	{
		if (o < 0 || o > 3)
			return false;

		if (orientation == o)
			return true;

		int fromO = orientation;
		while (true)
		{
			if (!isValid(-1, -1, fromO))
				return false;

			if (fromO == o)
				break;

			fromO = (fromO + 1) % 4;
		}
		return true;
	}
};

// ΧһȦ���Ǻ�
void init()
{
	int i;
	for (i = 0; i < MAPHEIGHT + 2; i++)
	{
		gridInfo[1][i][0] = gridInfo[1][i][MAPWIDTH + 1] = -2;
		gridInfo[0][i][0] = gridInfo[0][i][MAPWIDTH + 1] = -2;
	}
	for (i = 0; i < MAPWIDTH + 2; i++)
	{
		gridInfo[1][0][i] = gridInfo[1][MAPHEIGHT + 1][i] = -2;
		gridInfo[0][0][i] = gridInfo[0][MAPHEIGHT + 1][i] = -2;
	}
}

namespace Util
{

	// ����ܷ�ӳ��ض���ֱ���䵽��ǰλ��
	inline bool checkDirectDropTo(int color, int blockType, int x, int y, int o)
	{
		auto &def = blockShape[blockType][o];
		for (; y <= MAPHEIGHT; y++)
			for (int i = 0; i < 4; i++)
			{
				int _x = def[i * 2] + x, _y = def[i * 2 + 1] + y;
				if (_y > MAPHEIGHT)
					continue;
				if (_y < 1 || _x < 1 || _x > MAPWIDTH || gridInfo[color][_y][_x])
					return false;
			}
		return true;
	}

	// ��ȥ��
	void eliminate(int color)
	{
		int &count = transCount[color] = 0;
		int i, j, emptyFlag, fullFlag;
		maxHeight[color] = MAPHEIGHT;
		for (i = 1; i <= MAPHEIGHT; i++)
		{
			emptyFlag = 1;
			fullFlag = 1;
			for (j = 1; j <= MAPWIDTH; j++)
			{
				if (gridInfo[color][i][j] == 0)
					fullFlag = 0;
				else
					emptyFlag = 0;
			}
			if (fullFlag)
			{
				for (j = 1; j <= MAPWIDTH; j++)
				{
					// ע������ֻת����ǰ�Ŀ飬���������һ�����µĿ飨���������һ������
					trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
					gridInfo[color][i][j] = 0;
				}
				count++;
			}
			else if (emptyFlag)
			{
				maxHeight[color] = i - 1;
				break;
			}
			else
				for (j = 1; j <= MAPWIDTH; j++)
				{
					gridInfo[color][i - count][j] =
						gridInfo[color][i][j] > 0 ? 1 : gridInfo[color][i][j];
					if (count)
						gridInfo[color][i][j] = 0;
				}
		}
		maxHeight[color] -= count;
		elimTotal[color] += elimBonus[count];
	}

	// ת��˫����ȥ���У�����-1��ʾ���������򷵻�����
	int transfer()
	{
		int color1 = 0, color2 = 1;
		if (transCount[color1] == 0 && transCount[color2] == 0)
			return -1;
		if (transCount[color1] == 0 || transCount[color2] == 0)
		{
			if (transCount[color1] == 0 && transCount[color2] > 0)
				swap(color1, color2);
			int h2;
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
			if (h2 > MAPHEIGHT)
				return color2;
			int i, j;

			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];

			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];
			return -1;
		}
		else
		{
			int h1, h2;
			maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//��color1���ƶ�count1ȥcolor2
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];

			if (h1 > MAPHEIGHT) return color1;
			if (h2 > MAPHEIGHT) return color2;

			int i, j;
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];

			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];

			for (i = h1; i > transCount[color2]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color1][i][j] = gridInfo[color1][i - transCount[color2]][j];

			for (i = transCount[color2]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color1][i][j] = trans[color2][i - 1][j];

			return -1;
		}
	}

	// ��ɫ�����ܷ������Ϸ
	inline bool canPut(int color, int blockType)
	{
		Tetris t(blockType, color);
		for (int y = MAPHEIGHT; y >= 1; y--)
			for (int x = 1; x <= MAPWIDTH; x++)
				for (int o = 0; o < 4; o++)
				{
					t.set(x, y, o);
					if (t.isValid() && checkDirectDropTo(color, blockType, x, y, o))
						return true;
				}
		return false;
	}

	// ��ӡ�������ڵ���
	inline void printField()
	{
#ifndef _BOTZONE_ONLINE
		static const char *i2s[] = {
			"~~",
			"~~",
			"  ",
			"[]",
			"##"
		};
		cout << "~~��ǽ��[]���飬##���¿�" << endl;
		for (int y = MAPHEIGHT + 1; y >= 0; y--)
		{
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[0][y][x] + 2];
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[1][y][x] + 2];
			cout << endl;
		}
#endif
	}
}

//epoch: 9
//sigma: 1
//size: 100
Param Defparam = {
  -4.29637,
  3.70631,
  -4.55411,
  -9.64372,
  -7.88114,
  -4.2003,
  0.280425,
  0.0214817,
  -0.400753,
  -0.549649,
  0.32996,
  -1.50115,
};
//epoch: 9
//sigma: 1
//size: 100
Param Attparam = {
  -3.42054,
  3.87926,
  -3.40044,
  -8.69313,
  -6.8124,
  -3.19314,
  0.689469,
  -0.429236,
  0.329621,
  0.289658,
  -0.408609,
  -1.82687,
};

int main()
{
	// ��������
	istream::sync_with_stdio(false);
	srand(time(NULL));
	init();

	int turnID, blockType;
	int nextTypeForColor[2];
	cin >> turnID;

	// �ȶ����һ�غϣ��õ��Լ�����ɫ
	// ˫���ĵ�һ��϶���һ����
	cin >> blockType >> currBotColor;
	enemyColor = 1 - currBotColor;
	nextTypeForColor[0] = blockType;
	nextTypeForColor[1] = blockType;
	typeCountForColor[0][blockType]++;
	typeCountForColor[1][blockType]++;

	// Ȼ�������ǰÿ�غϵ�������������ָ�״̬
	// ѭ���У�color ��ʾ��ǰ��һ���� color ����Ϊ
	// ƽ̨��֤�������붼�ǺϷ�����
	for (int i = 1; i < turnID; i++)
	{
		int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1] };
		int x, y, o;
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�

		// �ȶ��Լ��������Ҳ�����Լ�����Ϊ
		// �Լ���������Լ������һ��
		// Ȼ��ģ�����һ�����ÿ�
		cin >> blockType >> x >> y >> o;

		// �ҵ�ʱ����һ���䵽�� x y o��
		Tetris myBlock(currTypeForColor[currBotColor], currBotColor);
		myBlock.set(x, y, o).place();

		// �Ҹ��Է�ʲô�����ţ�
		typeCountForColor[enemyColor][blockType]++;
		nextTypeForColor[enemyColor] = blockType;

		// Ȼ����Լ������룬Ҳ���ǶԷ�����Ϊ
		// ���и��Լ��������ǶԷ������һ��
		cin >> blockType >> x >> y >> o;

		// �Է���ʱ����һ���䵽�� x y o��
		Tetris enemyBlock(currTypeForColor[enemyColor], enemyColor);
		enemyBlock.set(x, y, o).place();

		// �Է�����ʲô�����ţ�
		typeCountForColor[currBotColor][blockType]++;
		nextTypeForColor[currBotColor] = blockType;

		// �����ȥ
		Util::eliminate(0);
		Util::eliminate(1);

		// ����ת��
		Util::transfer();
	}


	int blockForEnemy, finalX, finalY, finalO;

	// �������ߣ���ֻ���޸����²��֣�

	// ���²����������ƽ̨�ϱ��벻�������
	Util::printField();

	Board myBoard(gridInfo[currBotColor]);
	Board enemyBoard(gridInfo[enemyColor]);
	Block ans = paramDefenser(Defparam)(nextTypeForColor[currBotColor],myBoard);
	blockForEnemy = paramAttacker(Attparam)(typeCountForColor[enemyColor],enemyBoard);
	finalX = ans.x;
	finalY = ans.y;
	finalO = ans.o;
	
	// ���߽���������������ֻ���޸����ϲ��֣�

	cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO;

	return 0;
}
