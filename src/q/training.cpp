#include "util.cpp" 

int main(){
	cout<<fixed;
	int grid[MAPHEIGHT + 2][MAPWIDTH + 2] ={};
	for (int i = 0; i < MAPHEIGHT + 2; i++)
		grid[i][0] = grid[i][MAPWIDTH + 1] = -2;
	for (int i = 0; i < MAPWIDTH + 2; i++)
		grid[0][i] = grid[MAPHEIGHT + 1][i] = -2;
	Board blank(grid);
	Board board = blank;
	int blockType = 0;
	int blockCount[7] = {0};
	while(true){
		bool view;
		board = blank;
		memset(blockCount, 0, sizeof blockCount);
		blockType = 0;
		cout<<"epsilon: "; cin>>epsilon;
		cout<<"epsilon2: "; cin>>epsilon2;
		cout<<"learning rate: "; cin>>alpha;
		cout<<"times: "; int times; cin>>times;
		if(times<0) view=true,times=-times;
		else view =false;
		double totalLoss = 0;
		double loss = 0;
		for(int i = 0; i < times; i++){
			epoch++;
			double paramDelta[FEATURE_COUNT] ={};
			for(int j = 0; j <BATCH; j++){
				Block moves[40];
				int size = simpleMoves(blockType, board, moves);
				if(size == 0){
					board = blank;
					memset(blockCount, 0, sizeof blockCount);
					blockType = 0;
				//	cout<<'n';
					continue;
				}
				
				shuffle(moves,moves+size,engine);
				Block best; double max = MINVALUE;
				if(epsilon==0||distri(engine)>epsilon)
					for (int i = 0; i < size;i++) {
						double se = QS<0>(moves[i], board, blockCount PARAM_ARG);
						if(se>max){
							max = se;
							best = moves[i];
						}
					}
				else{
					uniform_int_distribution<int> ran(0,size-1);
					best = moves[ran(engine)];
					max = QS<0>(best, board, blockCount PARAM_ARG);
				}
				
				double fea[FEATURE_COUNT];
				features(best,board,blockCount,fea); 
				
				place(best, board);
				int elimnum = eliminate(board,best.y+Ymin[best.t][best.o], best.y + Ymax[best.t][best.o]);
				if(view){
					printField(board);
					system("pause");
				}
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
				
				shuffle(opChoice,opChoice+ptoOC,engine);
				double worst = MAXVALUE; int choice;
				if(epsilon2==0||distri(engine)>epsilon2)
					for(int i=0;i<ptoOC; i++){
						double op = QO<0>(opChoice[i], board, blockCount PARAM_ARG);
						if(op<worst){
							worst = op;
							choice = i;
						}
					}
				else{
					uniform_int_distribution<int> ran(0,ptoOC-1);
					choice = opChoice[ran(engine)];
					worst = QO<0>(choice, board, blockCount PARAM_ARG);
				}
				
				blockType = choice;
				blockCount[choice]++;
				
				double newv = elimnum + worst*LAMBDA;
				loss += (newv - max)*(newv - max);
				double delta = (newv - max);
				
				for(int i=0;i<FEATURE_COUNT;i++){
					paramDelta[i]=fea[i]*delta;
				}
				
			}
			
			for(int i=0;i<FEATURE_COUNT;i++)
				param[i]+=alpha*paramDelta[i]/BATCH; 
			
			if(epoch%10 == 0){
				cout<<"epoch: "<<epoch<<'\t';
				cout<<"\tloss: "<<loss<<endl;
				totalLoss += loss/(BATCH*10);
				loss = 0;
			}
		}
		cout<<"----------------------------"<<endl;
		for(int i=0;i<FEATURE_COUNT;i++){
			cout<<param[i]<<",\t\t//"<<i<<endl;
		}cout<<endl;
		for(int i=1;i<FEATURE_COUNT;i++){
			cout<<mean[i]<<",\t\t//"<<i<<endl;
		}cout<<endl;
		for(int i=0;i<FEATURE_COUNT;i++){
			cout<<sqrt(stddev[i]-mean[i]*mean[i])<<",\t\t//"<<i<<endl;
		}
		cout<<"Loss: "<<totalLoss/times<<endl;
		cout<<"----------------------------"<<endl;
	}
}


