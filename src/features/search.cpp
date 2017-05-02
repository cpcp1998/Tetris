#ifndef NOT_STAND_ALONE_FIND

#include<vector>
#include<queue>
#include<cstring>
#include<iostream>

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
};// 7种形状(长L| 短L| 反z| 正z| T| 直一| 田格)，4种朝向(上左下右)，8:每相邻的两个分别为x，y

#endif

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

inline void eliminate(Board& board)
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
	{valid[block.x][block.y][block.o] = true; validList.push(block);}

inline vector<Block> allPossibleState(int blockType, const Board& board){
	bool valid[MAPWIDTH+2][MAPHEIGHT+2][4]={};
	queue<Block> validList;
	for (int y = MAPHEIGHT; y >= 1; y--)
		for (int x = 1; x <= MAPWIDTH; x++)
			for (int o = 0; o < 4; o++)
			{
				Block block{blockType,x,y,o};
				if (isValid(block, board) && checkDirectDropTo(block, board)){
					valid[block.x][block.y][block.o]=true;
					validList.push(block);
				}
			}
	while(!validList.empty()){
		Block block = validList.front();
		validList.pop();
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
