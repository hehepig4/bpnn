#include "dispatch.h"

class FCFS : public Dispatch {
private:
	queue<Process> v;
	void setName() {
		strMethod = "FCFS";
	}	
	bool nowaitting() {
		if(v.size()==0)
		    return true;
		else return false;
	}
	void insert(Process proNew, bool isNewProcess) {
	    v.push(proNew);
	}
	bool select(Process& proSelcet) {	
		bool beFound=false;
		if(v.empty()){
			beFound=false;
		}
		else{
			proSelcet=v.front();
			v.pop();
			beFound=true;
		}
		return beFound;
}	
	bool goOnRunning(Process proCurrent){
		if((getCurrentTimeStamp()-proCurrent.tSelected)>=(proCurrent.Ts-proCurrent.done))
		{
			return false;
		}
		else return true;
	}
};

class RR : public Dispatch {//虚拟轮转 
public:
	void setQ(int _q)
	{
		q = _q;
	}
private:
	int q;
    queue<Process> v;
	void setName() {
		strMethod = "RR"+to_string(q);
	}
	bool nowaitting() {
		if(v.size()==0)
		    return true;
		else return false;
	}
	void insert(Process proNew, bool isNewProcess) {
		v.push(proNew);
	}
	bool select(Process& proSelcet) {
		bool beFound=false;
		if(v.empty()){
			beFound=false;
		}
		else{
			proSelcet=v.front();
			v.pop();
			beFound=true;
		}
		return beFound;
	}
	bool goOnRunning(Process proCurrent) {
	    if((getCurrentTimeStamp()-proCurrent.tSelected)>=(proCurrent.Ts-proCurrent.done)||(getCurrentTimeStamp()-proCurrent.tSelected)>=q)
		{
			return false;
		}
		else return true;
	}
};

class SPN : public Dispatch {//最短进程优先 ，无抢占 
public:
	struct cmp{
		bool operator()(Process &a,Process &b){
			return a.Ts>b.Ts;
		}
	};
	priority_queue<Process,vector<Process>,cmp> v;
private:
	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
	    if(v.size()==0)
		    return true;
		else return false;
	}
	void insert(Process proNew, bool isNewProcess) {
		v.push(proNew);	
	}
	bool select(Process& proSelcet) {
		bool beFound=false;
		if(v.empty()){
			beFound=false;
		}
		else{
			proSelcet=v.top();
			v.pop();
			beFound=true;
		}
		return beFound;
		
	}
	bool goOnRunning(Process proCurrent) {
		if((getCurrentTimeStamp()-proCurrent.tSelected)>=(proCurrent.Ts-proCurrent.done))
		{
			return false;
		}
		else return true;
	}
};

class SRT : public Dispatch {//最短剩余时间 
public:
	struct cmp{
		bool operator()(Process &a,Process &b){
			if((a.Ts-a.done)==(b.Ts-b.done)){
				if(a.arrive<b.arrive){
					return false;
				}
			}
			return (a.Ts-a.done)>=(b.Ts-b.done);
		}
	};
	priority_queue<Process,vector<Process>,cmp> v;
private:
	void setName() {
		strMethod = "SRT";
	}
	bool nowaitting() {
		if(v.size()==0)
		    return true;
		else return false;
	}
	void insert(Process proNew, bool isNewProcess) {
		v.push(proNew);	
	}
	bool select(Process& proSelcet) {
		bool beFound=false;
		if(v.empty()){
			beFound=false;
		}
		else{
			proSelcet=v.top();
			v.pop();
			beFound=true;
		}
		return beFound;
	}
	bool goOnRunning(Process proCurrent) {
		if((getCurrentTimeStamp()-proCurrent.tSelected)>=(proCurrent.Ts-proCurrent.done)||
		proCurrent.Ts-(proCurrent.done+getCurrentTimeStamp()-proCurrent.tSelected)>(v.top().Ts-v.top().done))
		{
			return false;
		}
		else return true;
	}
};

class HRRN : public Dispatch {
private:
	vector<Process> v;
	void setName() {
		strMethod = "HRRN";
	}
	bool nowaitting() {
		if(v.size()==0)
		    return true;
		else return false;
	}
	void insert(Process proNew, bool isNewProcess) {
		v.push_back(proNew);
	}
	bool select(Process& proSelcet) {
		vector<Process>::iterator first = v.begin();
		Process p = *first;
		HowLong_TYPE r0 = (getCurrentTimeStamp()-first->arrive+first->Ts)/first->Ts;
		vector<Process>::iterator ii;
		for(; first!=v.end(); first++){
			HowLong_TYPE r = (getCurrentTimeStamp()-first->arrive+first->Ts)/first->Ts;
			if(r>=r0){
				p = *first;
				r0 = r;
				ii = first;
			}
		}
		proSelcet = p;
		v.erase(ii);
	}
	bool goOnRunning(Process proCurrent) {
		if((getCurrentTimeStamp()-proCurrent.tSelected)>=(proCurrent.Ts-proCurrent.done))
		{
			return false;
		}
		else return true;
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
