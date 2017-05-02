/**
 * Tetris �򵥽�����������
 * https://wiki.botzone.org/index.php?title=Tetris
 * ������2017��4��20�գ�
 * ������rotation��������������ʽ�޸�Ϊ�¹���ĸ�ʽ������transfer������`if (h2 >= MAPHEIGHT)`��Ϊ`if (h2 > MAPHEIGHT)`
 */
// ע�⣺x�ķ�Χ��1~MAPWIDTH��y�ķ�Χ��1~MAPHEIGHT
// ���������У�y�����У�c��
// ����ϵ��ԭ�������½�

#define USE_OPENMP

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cstring>
#include "mcts.h"
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

const int effOri[7]={4,4,2,2,4,2,1};

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

struct Block{
	int blockType;   // ��Ƿ������͵���� 0~6
	int blockX;            // ��ת���ĵ�x������
	int blockY;            // ��ת���ĵ�y������
	int orientation;       // ��Ƿ���ĳ��� 0~3
	//const int(*shape)[8]; // ��ǰ���ͷ������״����
};

ostream& operator<<(ostream& o, const Block& b){
	return o;
} 

bool operator==(const Block& a, const Block& b){
	if(a.blockX==0) return a.blockType==b.blockType;
	return a.blockType==b.blockType&&a.blockX==b.blockX
		&&a.blockY==b.blockY&&a.orientation==b.orientation;
}

bool operator!=(const Block& a, const Block& b){
	return !(a==b);
}

bool operator<(const Block& a, const Block& b){
	int ia=(a.blockType<<18)+(a.blockX<<10)+(a.blockY<<2)+a.orientation;
	int ib=(b.blockType<<18)+(b.blockX<<10)+(b.blockY<<2)+b.orientation;
	return ia<ib;
}

struct Board{
	// ��¼��ͼ״̬��0Ϊ�գ�1Ϊ��ǰ���ã�����ΪԽ��
	int gridInfo[MAPHEIGHT + 2][MAPWIDTH + 2];
	// ת������
	int transCount = 0;
	// ����eliminate��ĵ�ǰ�߶�
	int maxHeight = 0;
	// ����Ӧ��ҵĸ�������Ŀ�ܼ�
	int typeCount[7];
	
	typedef Block Move;
	static const Move no_move;
	
	Move next;
	
	int player_to_move; // 1 for settle down, 2 for pick up.
	int moveCount = 0;
	
	mutable std::vector<Move> moves;
	mutable bool movesValid = false;
	
	Board(int g[][MAPWIDTH + 2], int t[], int p, Move nextMove=no_move){
		next = nextMove;
		player_to_move = p;
		memcpy(gridInfo, g, sizeof(gridInfo));
		memcpy(typeCount, t, sizeof(typeCount));
	}
	
	bool check(Move move) const {
		for (int i = 0; i < 4; i++)
		{
			int tmpX = move.blockX + blockShape[move.blockType][move.orientation][2 * i];
			int tmpY = move.blockY + blockShape[move.blockType][move.orientation][2 * i + 1];
			if (tmpX < 1 || tmpX > MAPWIDTH ||
				tmpY < 1 || 
				tmpY<MAPHEIGHT &&gridInfo[tmpY][tmpX] != 0)
				return false;
		}
		return true;
	}
	
	void do_move(Move move){
		movesValid = false;
		player_to_move = 3 - player_to_move;
		if(next.blockType == -1){
			next = move;
			return;
		}
		for (int i = 0; i < 4; i++)
		{
			int tmpX = move.blockX + blockShape[move.blockType][move.orientation][2 * i];
			int tmpY = move.blockY + blockShape[move.blockType][move.orientation][2 * i + 1];
			gridInfo[tmpY][tmpX] = 1;
		}
		
		int count = 0;
		maxHeight = MAPHEIGHT;
		for (int i = 1; i <= MAPHEIGHT; i++)
		{
			int emptyFlag = 1;
			int fullFlag = 1;
			for (int j = 1; j <= MAPWIDTH; j++)
			{
				if (gridInfo[i][j] == 0)
					fullFlag = 0;
				else
					emptyFlag = 0;
			}
			if (fullFlag)
			{
				for (int j = 1; j <= MAPWIDTH; j++)
				{
					gridInfo[i][j] = 0;
				}
				count++;
			}
			else if (emptyFlag)
			{
				maxHeight = i - 1;
				break;
			}
			else
				for (int j = 1; j <= MAPWIDTH; j++)
				{
					gridInfo[i - count][j] = gridInfo[i][j];
					if (count)
						gridInfo[i][j] = 0;
				}
		}
		maxHeight -= count;
		transCount += count;
		typeCount[move.blockType]++;
		next = no_move;
		moveCount++;
	};
	
	std::vector<Move> get_moves() const {
		prepare_moves();
		return moves;
	}
	
	void prepare_moves() const {
		if(movesValid) return;
		moves.clear();
		if(next.blockType==-1){
			if(moveCount > 10) return;
			int minCount=1000;
			for(int i = 0; i < 7; ++i){
				if(typeCount[i]<minCount) minCount = typeCount[i];
			}
			minCount+=2;
			for(int i=0; i<7; ++i){
				if(typeCount[i]<minCount) moves.push_back({i,0,0,0});
			}
		}else{
			for (int o = 0; o < effOri[next.blockType]; o++)
				for (int x = 1; x <= MAPWIDTH; x++){
					Move prev = no_move;
					for (int y = MAPHEIGHT; y >= 1; y--)
					{
						Move move = {next.blockType, x, y, o};
						if (check(move)) prev=move;
						else break;
					}
					if (prev.blockType != -1) {
						moves.push_back(prev);
					}
			}
				
		}
		movesValid = true;
	}
	
	bool has_moves() const {
		prepare_moves();
		return !moves.empty();
	}
	
	template<typename RandomEngine>
	void do_random_move(RandomEngine *engine){
		prepare_moves();
		if(player_to_move==2){
			std::uniform_int_distribution<> dis(0,moves.size()-1);
			do_move(moves[dis(*engine)]);
		}else{//player_to_move==1
			shuffle(moves.begin(),moves.end(),*engine);
			int minY = 100;
			Move move;
			for(Move m : moves){
				if(m.blockY<minY){
					minY=m.blockY;
					move=m;
				}
			}
			do_move(move);
		}
	}
	
	double get_result(int current_player_to_move) const{
		double result = 0;
		result+=-height();
		if(current_player_to_move==2) result=-result;
		return result;
	}
	
	int height() const{
		for(int y = 1; y <= MAPHEIGHT; ++y){
			int empty = 0;
			for(int x = 1; x <= MAPWIDTH; ++x)
				if(gridInfo[y][x]){
					empty++; break;
				}
			if(empty) return y;
		}
		return MAPHEIGHT+1;
	}
	
}; 

const Block Board::no_move={-1,0,0,0};

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
	
	Block nextBlock{nextTypeForColor[currBotColor],0,0,0};
	Board myBoard(gridInfo[currBotColor], typeCountForColor[currBotColor], 1, nextBlock);
	Board enemyBoard(gridInfo[enemyColor], typeCountForColor[enemyColor], 2);
	
	Block blockForMe = MCTS::compute_move(myBoard);
	Block blockForEn = MCTS::compute_move(enemyBoard);
	
	blockForEnemy = blockForEn.blockType;
	finalX = blockForMe.blockX;
	finalY = blockForMe.blockY;
	finalO = blockForMe.orientation;
/*
	// �������ߣ���ֻ���޸����²��֣�

	// ���²����������ƽ̨�ϱ��벻�������
	Util::printField();

	// ̰�ľ���
	// ���������Ը�����̬�ҵ���һ��λ�ã�Ҫ���ܹ�ֱ������
	Tetris block(nextTypeForColor[currBotColor], currBotColor);
	for (int y = 1; y <= MAPHEIGHT; y++)
		for (int x = 1; x <= MAPWIDTH; x++)
			for (int o = 0; o < 4; o++)
			{
				if (block.set(x, y, o).isValid() &&
					Util::checkDirectDropTo(currBotColor, block.blockType, x, y, o))
				{
					finalX = x;
					finalY = y;
					finalO = o;
					goto determined;
				}
			}

determined:
	// �ٿ������Է�ʲô��

	int maxCount = 0, minCount = 99;
	for (int i = 0; i < 7; i++)
	{
		if (typeCountForColor[enemyColor][i] > maxCount)
			maxCount = typeCountForColor[enemyColor][i];
		if (typeCountForColor[enemyColor][i] < minCount)
			minCount = typeCountForColor[enemyColor][i];
	}
	if (maxCount - minCount == 2)
	{
		// Σ�գ���һ���������Ŀ���Է���
		for (blockForEnemy = 0; blockForEnemy < 7; blockForEnemy++)
			if (typeCountForColor[enemyColor][blockForEnemy] != maxCount)
				break;
	}
	else
	{
		blockForEnemy = rand() % 7;
	}

	// ���߽���������������ֻ���޸����ϲ��֣�
*/
	cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO;
	//system("pause");
	return 0;
}
