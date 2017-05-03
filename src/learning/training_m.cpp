#include<utility> 
#include<algorithm>
#include<cmath>
#include<fstream>
#include"windows.h"
#define NDEBUG
#include "util.cpp"
#define PARAM_ranCopy 12
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

int ranCopy = 100;
int result[200][200];
vector<pair<int,Param> > *def,*att;

DWORD WINAPI calc(LPVOID param) {
	int i = (int)param;
	for (int j = 0; j<ranCopy; ++j) {
		int ret = simulate(paramDefenser((*def)[i].second), paramAttacker((*att)[j].second), 40);
		result[i][j] = ret;
	}
	return 0;
}

Param generate(Param p, double sigma){
	for(int i=0;i<PARAM_ranCopy;i++)
		p[i]+=normal_distribution<double>(0.0,sigma)(randomEngine);
	return p;
}

int main(){
	ofstream fout("mout.txt");
	double sigma = 0.5;
	int epoch = 0;
	
	while(true){
		//cout<<"sigma="; cin>>sigma;
		//if(sigma == 0) break;
		epoch++;
		//cout<<"ranCopy="; cin>>ranCopy;
		def = new vector<pair<int, Param> >(ranCopy);
		att = new vector<pair<int, Param> >(ranCopy);
		for(int i=0;i<ranCopy;i++){
			(*def)[i].first=0;
			(*def)[i].second=generate(Defparam,sigma);
		}
		for(int i=0;i<ranCopy;i++){
			(*att)[i].first=0;
			(*att)[i].second=generate(Attparam,sigma);
		}
		memset(result, 0, sizeof result);

		HANDLE hThread[200];
		for (int i = 0; i < ranCopy; ++i)
			hThread[i] = CreateThread(NULL, 0, calc, (LPVOID)i, 0, NULL);
		for (int i = 0; i < ranCopy; ++i) {
			WaitForSingleObject(hThread[i], INFINITE);
			CloseHandle(hThread[i]);
		}
			

		for (int i = 0; i < ranCopy; i++) 
			for (int j = 0; j < ranCopy; j++) {
				(*def)[i].first += result[i][j];
				(*att)[j].first -= result[i][j];
			}

		sort(att->begin(),att->end());
		sort(def->begin(),def->end());
		int edge = ranCopy * 0.5;
		double max = 0;
		for(int i=0;i<PARAM_ranCopy;i++){
			double sum = 0;
			for(int j=edge;j<ranCopy;j++)
				sum+=(*def)[j].second[i];
			double newvalue = sum/(ranCopy-edge);
			double diff = abs(newvalue - Defparam[i]);
			if(diff>max) max = diff;
			Defparam[i]=newvalue;
		}
		cout<<"Defenser change max: "<<max<<endl;
		max = 0;
		for(int i=0;i<PARAM_ranCopy;i++){
			double sum = 0;
			for(int j=edge;j<ranCopy;j++)
				sum+=(*att)[j].second[i];
			double newvalue = sum/(ranCopy-edge);
			double diff = abs(newvalue - Attparam[i]);
			if(diff>max) max = diff;
			Attparam[i]=newvalue;
		}
		cout<<"Attacker change max: "<<max<<endl;
		cout<<"Best attacker: "<<(*att)[ranCopy-1].first/(double)ranCopy<<endl;
		cout<<"Best defenser: "<<(*def)[ranCopy-1].first/(double)ranCopy<<endl;
		cout<<"Result against random: "<<simulate(paramDefenser(Defparam),randomAttacker,30)<<endl;
		cout<<"vs: "<<simulate(paramDefenser(Defparam),paramAttacker(Attparam),40)<<endl;
		fout<<"//epoch: "<<epoch<<endl;
		fout<<"//sigma: "<<sigma<<endl;
		fout<<"//ranCopy: "<<ranCopy<<endl;
		fout<<"Param Defparam = {"<<endl;
		for(int i=0;i<PARAM_ranCopy;++i) fout<<"  "<<Defparam[i]<<','<<endl;
		fout<<"};"<<endl;
		fout<<"//epoch: "<<epoch<<endl;
		fout<<"//sigma: "<<sigma<<endl;
		fout<<"//ranCopy: "<<ranCopy<<endl;
		fout<<"Param Attparam = {"<<endl;
		for(int i=0;i<PARAM_ranCopy;++i) fout<<"  "<<Attparam[i]<<','<<endl;
		fout<<"};"<<endl;
		fout<<endl; 

		delete def;
		delete att;
	}
	fout.close();
}
