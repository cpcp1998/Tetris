#include<utility> 
#include<algorithm>
#include<cmath>
#include<fstream>
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
	ofstream fout("out.txt");
	double sigma = 0.5;
	int epoch = 0;
	int size = 100;
	while(true){
		//cout<<"sigma="; cin>>sigma;
		//if(sigma == 0) break;
		epoch++;
		//cout<<"size="; cin>>size;
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
				int ret = simulate(paramDefenser(def[i].second),paramAttacker(att[j].second),40);
				def[i].first+=ret;
				att[j].first-=ret;
			}
		}
		sort(att.begin(),att.end());
		sort(def.begin(),def.end());
		int edge = size * 0.5;
		double max = 0;
		for(int i=0;i<PARAM_SIZE;i++){
			double sum = 0;
			for(int j=edge;j<size;j++)
				sum+=def[j].second[i];
			double newvalue = sum/(size-edge);
			double diff = abs(newvalue - Defparam[i]);
			if(diff>max) max = diff;
			Defparam[i]=newvalue;
		}
		cout<<"Defenser change max: "<<max<<endl;
		max = 0;
		for(int i=0;i<PARAM_SIZE;i++){
			double sum = 0;
			for(int j=edge;j<size;j++)
				sum+=att[j].second[i];
			double newvalue = sum/(size-edge);
			double diff = abs(newvalue - Attparam[i]);
			if(diff>max) max = diff;
			Attparam[i]=newvalue;
		}
		cout<<"Attacker change max: "<<max<<endl;
		cout<<"Best attacker: "<<att[size-1].first/(double)size<<endl;
		cout<<"Best defenser: "<<def[size-1].first/(double)size<<endl;
		cout<<"Result against random: "<<simulate(paramDefenser(Defparam),randomAttacker,30)<<endl;
		cout<<"vs: "<<simulate(paramDefenser(Defparam),paramAttacker(Attparam),40)<<endl;
		fout<<"//epoch: "<<epoch<<endl;
		fout<<"//sigma: "<<sigma<<endl;
		fout<<"//size: "<<size<<endl;
		fout<<"Param Defparam = {"<<endl;
		for(int i=0;i<PARAM_SIZE;++i) fout<<"  "<<Defparam[i]<<','<<endl;
		fout<<"};"<<endl;
		fout<<"//epoch: "<<epoch<<endl;
		fout<<"//sigma: "<<sigma<<endl;
		fout<<"//size: "<<size<<endl;
		fout<<"Param Attparam = {"<<endl;
		for(int i=0;i<PARAM_SIZE;++i) fout<<"  "<<Attparam[i]<<','<<endl;
		fout<<"};"<<endl;
		fout<<endl; 
	}
	fout.close();
}
