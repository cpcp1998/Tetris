#include<utility> 
#include<algorithm>
#define NDEBUG
#include "util.cpp"
#define PARAM_SIZE 12
Param Defparam = {
	-4.500158825082766, //landHeight
	3.4181268101392694, //rowEliminated
	-3.2178882868487753,//rowTransitions
	-9.348695305445199, //colTransitions
	-7.899265427351652, //numHoles
	-3.3855972247263626,//wellSums
	0,//connectdHoles
	0,//pileHeight
	0,//altitudeDiff
	-1,//wellDepth
	0,//wellSum
	0,//weightedBlock
	};

Param Attparam = {
	-4.500158825082766, //landHeight
	3.4181268101392694, //rowEliminated
	-3.2178882868487753,//rowTransitions
	-9.348695305445199, //colTransitions
	-7.899265427351652, //numHoles
	-3.3855972247263626,//wellSums
	0,//connectdHoles
	0,//pileHeight
	0,//altitudeDiff
	0,//wellDepth
	0,//wellSum
	0,//weightedBlock
	};

Param generate(Param p, double sigma){
	for(int i=0;i<PARAM_SIZE;i++)
		p[i]+=normal_distribution<double>(0.0,sigma)(randomEngine);
	return p;
}
int main(){
	double sigma = 2.0;
	//int epoch = 100;
	int size = 100;
	while(true){
		cout<<"sigma="; cin>>sigma;
		//cout<<"epoch="; cin>>epoch;
		cout<<"size="; cin>>size;
		vector<pair<int,Param> > def(size),att(size);
		for(int i=0;i<size;i++){
			def[i].first=0;
			def[i].second=generate(Defparam,sigma);
		}
		for(int i=0;i<size;i++){
			att[i].first=0;
			att[i].second=generate(Attparam,sigma);
		}
		for(int i=0;i<size;++i){
			cout<<"Calculating #"<<i<<endl;
			for(int j=0;j<size;++j){
				int ret = simulate(paramDefenser(def[i].second),paramAttacker(att[i].second),40);
				def[i].first+=ret;
				att[i].first-=ret;
			}
		}
		sort(att.begin(),att.end());
		sort(def.begin(),def.end());
		int edge = size * 0.5;
		for(int i=0;i<PARAM_SIZE;i++){
			double sum = 0;
			for(int j=edge;j<size;j++)
				sum+=def[j].second[i];
			Defparam[i]=sum/(size-edge);
		}
		for(int i=0;i<PARAM_SIZE;i++){
			double sum = 0;
			for(int j=edge;j<size;j++)
				sum+=att[j].second[i];
			Attparam[i]=sum/(size-edge);
		}
		cout<<"Best attacker: "<<att[size-1].first/(double)size<<endl;
		cout<<"Best defenser: "<<def[size-1].first/(double)size<<endl;
		cout<<"Result against random"<<simulate(paramDefenser(Defparam),randomAttacker,30)<<endl;
	}
}
