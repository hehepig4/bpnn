#include "dispatch.h"
#include<deque>
#include<algorithm>
int fortime=0;
class FCFS : public Dispatch {
private:
	
	queue<Process> Q;
	 
	void setName() {
		strMethod = "FCFS";
	}
	bool nowaitting() {
		if(!Q.empty()){
			return 0;
		}
		else{
			return 1;
		}
	}
	void insert(Process proNew, bool isNewProcess) {
		Q.push(proNew);
	}
	bool select(Process& proSelcet) {
		proSelcet=Q.front();
		if(!Q.empty()){
			Q.pop();
			return 1;
		}
		else{
			return 0;
		}
	}
	bool goOnRunning(Process proCurrent){
		if(proCurrent.Ts==getCurrentTimeStamp()-proCurrent.tSelected){
			return 0;
		}
		else{
			return 1;
		}
	}
};

class RR : public Dispatch {
public:
	void setQ(int _q)
	{
		q = _q;
	}
private:
	int q;
	deque<Process> dq;
	void setName() {
		strMethod = "RR"+to_string(q);
	}
	bool nowaitting() {
		if(!dq.empty()){
			return 0;
		}
		else{
			return 1;
		}
	}
	void insert(Process proNew, bool isNewProcess) {
		dq.push_back(proNew);
	}
	bool select(Process& proSelcet) {
		if(dq.empty()){
			return 0;
		}
		Process now=dq.front();
//		cout<<now.id<<endl;
		proSelcet=now;		
//		cout<<now.id<<endl;
//		cout<<proSelcet.id<<endl;
		if(!dq.empty()){
			dq.pop_front();
			return 1;
		}
	}
	bool goOnRunning(Process proCurrent) {
//		cout<<endl<<getCurrentTimeStamp()-proCurrent.tSelected<<" "<<q<<" "<<proCurrent.Ts-proCurrent.done<<endl;
		if((proCurrent.Ts-proCurrent.done==getCurrentTimeStamp()-proCurrent.tSelected)||(getCurrentTimeStamp()-proCurrent.tSelected==q)){
			return 0;
		}
		else{
			return 1;
		}
	}
};

class SPN : public Dispatch {
private:
	struct cmp1{
	bool operator()(const Process &x,const Process &y)const{
		if(x.Ts!=y.Ts)
		return x.Ts>y.Ts;
		else{
			return x.arrive>y.arrive;
		}
	}
};
	priority_queue<Process,vector<Process>,cmp1> pq1;

	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
		if(!pq1.empty()){
			return 0;
		}
		else{
			return 1;
		}
	}
	
	void insert(Process proNew, bool isNewProcess) {

		pq1.push(proNew);

	}
	bool select(Process& proSelcet) {
		if(pq1.empty()){
			return 0;
		}
		proSelcet=pq1.top();
		pq1.pop();
		return 1;
	}
	bool goOnRunning(Process proCurrent) {
		if(proCurrent.Ts-proCurrent.done==getCurrentTimeStamp()-proCurrent.tSelected){
			return 0;
		}
		else{
			return 1;
		}
	}
};

class SRT : public Dispatch {
private:
	struct cmp2{
	bool operator()(const Process &x,const Process &y)const{
		if(x.Ts-x.done!=y.Ts-y.done)
		return x.Ts-x.done>y.Ts-y.done;
		else{
			return x.arrive>y.arrive;
		}
	}
};
	priority_queue<Process,vector<Process>,cmp2> pq1;
	void setName() {
		strMethod = "SRT";
	}
	bool nowaitting() {
		if(!pq1.empty()){
			return 0;
		}
		else{
			return 1;
		}
	}
	void insert(Process proNew, bool isNewProcess) {
		pq1.push(proNew);
	}
	bool select(Process& proSelcet) {
		if(pq1.empty()){
			return 0;
		}
		proSelcet=pq1.top();
		pq1.pop();
		return 1;
	}
	bool goOnRunning(Process proCurrent) {
		Process now=pq1.top();
		if((now.Ts-now.done<proCurrent.Ts-proCurrent.done)||(proCurrent.Ts-proCurrent.done==getCurrentTimeStamp()-proCurrent.tSelected))
		{
			return 0;
		}
		else{
			return 1;
		}
		
	}
};

class HRRN : public Dispatch {
private:
//	int now=getCurrentTimeStamp();
	int pan[32767],num=0,number=0;
	
	vector<Process> a;
	
	void setName() {
		strMethod = "HRRN";
		
	}
	bool nowaitting() {
		int now=getCurrentTimeStamp();
		if(num<number){
			return 0;
		}
		else{
			return 1;
		}
	}
	void insert(Process proNew, bool isNewProcess) {
		a.push_back(proNew);
		number++;
	}
	
	bool select(Process& proSelcet) {
		
		if(num==number){
			return 0;
		}
		int maxn=-1,keep=-1;
		for(int i=0;i<a.size();i++)
		{
			if(pan[i]==1){
				continue;
			}
			Process x=a[i];
			if((fortime-x.arrive)/x.Ts>maxn){
				maxn=(fortime-x.arrive)/x.Ts;
				keep=i;
			}
		}
		proSelcet=a[keep];
		pan[keep]=1;
		num++;
		return 1;
	}
	bool goOnRunning(Process proCurrent) {
		if(proCurrent.Ts-proCurrent.done==getCurrentTimeStamp()-proCurrent.tSelected)
		{
			fortime+=proCurrent.Ts-proCurrent.done;
			return 0;
		}
		else{
			return 1;
		}
	}
};

class Feedback : public Dispatch {
public:
	Feedback(int _n=3) : n(_n),vRQ(_n+1),iLevel(-1),count(0) {	}
	
private:
	int n;
	int iLevel; //the current running process comes from which RQ.
	vector<queue<Process> > vRQ;
	int count; //how many waitting processes.

	void setName() {
		strMethod = "Feedback";
	}

	void insert(Process proNew, bool isNewProcess) {
		if (isNewProcess)
		{
			vRQ[0].push(proNew);
			++count;
		}
		else
		{
			assert(iLevel < n);
			vRQ[iLevel + 1].push(proNew);
			++count;
		}
	}

	bool nowaitting()
	{
		return count == 0;
	}

	bool select(Process& proSelcet) {
		bool bFound = false;
		for (int ii = 0; ii <= n; ++ii)
		{
			if (vRQ[ii].empty())
				continue;
			bFound = true;
			proSelcet = vRQ[ii].front();
			vRQ[ii].pop();
			--count;
			iLevel = ii;
			break;
		}
		return bFound;
	}

	bool goOnRunning(Process proCurrent) {
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
		{
			return false;
		}
		
		if (iLevel == n)
			return true;

		HowLong_TYPE duration = getCurrentTimeStamp() - proCurrent.tSelected;
		return (duration < (1 << iLevel));
	}
};

void test()
{
	FCFS a;
	a.run();

	int q = 1;
	RR b;
	b.setQ(q);
	b.run();

	RR c;
	q = 4;
	c.setQ(q);
	c.run();

	SPN d;
	d.run();

	SRT e;
	e.run();

	HRRN f;
	f.run();
}

//please use this main function when submitting.
int main() {
	fout.open("result.txt");
	test();
	fout.close();
	return 0;
}
