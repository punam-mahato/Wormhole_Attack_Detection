#include<cstdio>
#include<ctime>
#include<cstdlib>
#include<vector>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<set>
#include<limits.h>
#include<queue>
using namespace std;

#define MAXN 100

vector<pair<double,double> > nodes;
vector<set<int> > two_hop_neighbors(MAXN);
vector<set<int> > four_hop_neighbors(MAXN);
vector<vector<int> > neigh(MAXN);
vector<int> wormholePt1_neighbors;
vector<int> wormholePt2_neighbors;

int gloflag;

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double compute_pn (double r, double R, double b)
{
	double t1 = (8*(R/b + 0.5)*(R/b + 0.5)*acos(r/(2*R+b)))/M_PI;
	double t2 = (4*r*sqrt((R+b/2)*(R+b/2) - ((r*r)/4)))/(M_PI*b*b);

	return (int)(t1-t2);
}
double distance(double x1, double y1,double x2, double y2)
{
	double dx=x2-x1;
	double dy=y2-y1;
	return sqrt(dx*dx + dy*dy);
}

double dis(int i, int j)
{
	double x1 = nodes[i].first;
	double y1 = nodes[i].second;
	double x2 = nodes[j].first;
	double y2 = nodes[j].second;
	double dx=x2-x1;
	double dy=y2-y1;
	return sqrt(dx*dx + dy*dy);
}
void findNeighbors(double r)
{
	int i,j;
	for (i=0;i<MAXN;i++)
		for (j=0;j<MAXN;j++)
			if (i!=j && dis(i,j) <= r)
				neigh[i].push_back(j);
}

void insertwormhole(double r)
{

	double wx1=fRand(0.5,1.0);
	double wy1=fRand(0.5,1.0);

	double wx2=fRand(4.0,4.5);
	double wy2=fRand(4.0,4.5);
	
	int i;
	for (i=0;i<MAXN;i++)
		if (distance(nodes[i].first,nodes[i].second,wx1,wy1) <= r )
			wormholePt1_neighbors.push_back(i);
	
	for (i=0;i<MAXN;i++)
		if (distance(nodes[i].first,nodes[i].second,wx2,wy2) <= r )
			wormholePt2_neighbors.push_back(i);
	cout << "Wormhole link inserted:" << endl;
	cout <<"(" <<wx1 << "," << wy1 << ")  (" << wx2 << "," << wy2<<")" << "\n"; 
}

void updateNeighbors()
{
	int i,j;
	for (i=0;i<wormholePt1_neighbors.size();i++)
	{
		for (j=0;j<wormholePt2_neighbors.size();j++)
		{
			if (find(neigh[wormholePt1_neighbors[i]].begin(),neigh[wormholePt1_neighbors[i]].end(),wormholePt2_neighbors[j])==neigh[wormholePt1_neighbors[i]].end())
				neigh[wormholePt1_neighbors[i]].push_back(wormholePt2_neighbors[j]);
		}
	}

	for (i=0;i<wormholePt2_neighbors.size();i++)
	{
		for (j=0;j<wormholePt1_neighbors.size();j++)
		{
			if (find(neigh[wormholePt2_neighbors[i]].begin(),neigh[wormholePt2_neighbors[i]].end(),wormholePt1_neighbors[j])==neigh[wormholePt2_neighbors[i]].end())
				neigh[wormholePt2_neighbors[i]].push_back(wormholePt1_neighbors[j]);
		}
	}

}

void computeTwoKHopNeighbours()
{
	int i,j,k;
	for (i=0;i<MAXN;i++)
	{
		for (j=0;j<neigh[i].size();j++)
			two_hop_neighbors[i].insert(neigh[i][j]);
		for (j=0;j<neigh[i].size();j++){
			for (k=0;k<neigh[neigh[i][j]].size();k++)
				if(neigh[neigh[i][j]][k] != i)
					two_hop_neighbors[i].insert(neigh[neigh[i][j]][k]);	
		}
	}

}


void computeFourKHopNeighbours()
{
	int i,j;
	for (i=0;i<MAXN;i++)
	{
		queue<pair<int,int> > q;
		q.push(make_pair(i,0));
		four_hop_neighbors[i].insert(i);
		while (!q.empty())
		{
			pair<int,int> ss = q.front();
			q.pop();
			int node = ss.first;
			int level = ss.second;
			if (level <=4)
			{
				for (j=0;j<neigh[node].size();j++)
				{
					if (four_hop_neighbors[i].find(neigh[node][j]) == four_hop_neighbors[i].end()){
						four_hop_neighbors[i].insert(neigh[node][j]);
						q.push(make_pair(neigh[node][j],level+1));
					}
				}
			}
		}
		four_hop_neighbors[i].erase(four_hop_neighbors[i].find(i));
	}
}

vector<int> compute_common1(int u, int v){
	vector<int> myvec;
	for(int i=0; i<neigh[u].size(); i++)
		if(find(neigh[v].begin(), neigh[v].end(), neigh[u][i]) != neigh[v].end())
			myvec.push_back(neigh[u][i]);
	return myvec;
}


vector<int> compute_common2(int u, int v){
	vector<int> myvec;
	set<int>::iterator it1;
	for(it1=two_hop_neighbors[u].begin(); it1 != two_hop_neighbors[u].end(); it1++)
		if(two_hop_neighbors[v].find(*it1) != two_hop_neighbors[v].end())
			myvec.push_back((*it1));
	return myvec;
}

vector<int> maximal_independent_set(vector<int>& myvec){
	vector<int> vec;
	for(int i=0;i<myvec.size();i++){
		int flag = 1;
		for(int j=0;j<vec.size();j++){
			int vert = vec[j];
			if( find(neigh[vert].begin(), neigh[vert].end(), myvec[i]) != neigh[vert].end() ){
				flag = 0;
				break;
			}
		}
		if(flag)
			vec.push_back(myvec[i]);
	}
	return vec;
}

void interrupt(int node){
	cout << "Forbidden Substructure found in UDG of node " << nodes[node].first << " " << nodes[node].second <<  endl ;
	return ;
}

void detectWormhole1(){
	int i;
	cout << "Detecting wormholes for 1 hop neighbors with fk=3" <<"\n";
	for(i=0; i<MAXN; i++){
		set<int> tset;
		tset = two_hop_neighbors[i];
		set<int>::iterator it = tset.begin();
		for(; it!=tset.end(); it++){
			if(find(neigh[i].begin(), neigh[i].end(), (*it) ) == neigh[i].end()){
				int ver1 = i, ver2 = (*it);
				vector<int> common;
				common = compute_common1(ver1, ver2);
				common = maximal_independent_set(common);
			//	int fk = compute_pn(dis(ver1,ver2),1.0,1.0) + 1;
				int fk=3;
				if(common.size() >= fk){
					gloflag = 1;
					interrupt(i);
					break;

				}
			}
		}
	}
	return ;
}

void detectWormhole2(){
	int i;
	cout << "Detecting wormholes for 2 hop neighbors with fk=8" <<"\n";
	for(i=0; i<MAXN; i++){
		set<int> tset;
		tset = four_hop_neighbors[i];
		set<int>::iterator it = tset.begin();
		for(; it!=tset.end(); it++){
			if(find(neigh[i].begin(), neigh[i].end(), (*it) ) == neigh[i].end()){
				int ver1 = i, ver2 = (*it);
				vector<int> common;
				common = compute_common2(ver1, ver2);
				common = maximal_independent_set(common);
				//int fk = compute_pn(dis(ver1,ver2),2.0,1.0) + 1;
				int fk=8;
				if(common.size() >= fk){
					interrupt(i);

				}
			}
		}
	}
	return ;
}

int main()
{
	int i,j;
	gloflag = 0;
	double xoff,yoff,genx,geny;	
	set<int>::iterator it;
	freopen("graph.txt", "r", stdin);
	srand(time(NULL));
	xoff=0.7;
	yoff=0.7;

	for (i=1;i<=10;i++){
		for (j=1;j<=10;j++){
			//genx=fRand(xoff*(i-1),xoff*i);
			//geny=fRand(yoff*(j-1),yoff*j);
			cin >> genx >> geny;
			nodes.push_back(make_pair(genx,geny));
		}
	}
	findNeighbors(1.000);
	
	insertwormhole(1.000);
	updateNeighbors();
	computeTwoKHopNeighbours();
	computeFourKHopNeighbours();
	detectWormhole1();

	if (gloflag == 0)
	{
		detectWormhole2();
	}

	return 0;
}
