#include "dispatch.h"

class FCFS : public Dispatch {
public:
    queue<Process> RQ;
private:
	void setName() {
		strMethod = "FCFS";
	}
	bool nowaitting() {
		if(RQ.empty()){
			return true;
		}
		else{
			return false;
		}
	}
	void insert(Process proNew,bool isNewProcess) {
		RQ.push(proNew);
	}
	bool select(Process& proSelect1) {
		proSelect1=RQ.front();
		RQ.pop();
	}
	bool goOnRunning(Process proCurrent){
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
		{
			return false;
		}
		else{
			return true;
		}
	}
};

class RR : public Dispatch {
public:
	void setQ(int _q)
	{
		q = _q;
	}
	 queue<Process> RQ;
private:
	int q;
   
	void setName() {
		strMethod = "RR"+to_string(q);
	}
	bool nowaitting() {
		if(RQ.empty()){
			return true;
		}
		else{
			return false;
		}
	}
	void insert(Process proNew, bool isNewProcess) {
		RQ.push(proNew);
	}
	bool select(Process& proSelect1) {
		proSelect1=RQ.front();
		RQ.pop();
	}
	bool goOnRunning(Process proCurrent) {
		if(((getCurrentTimeStamp()-proCurrent.tSelected)==q)||(proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts)){
			return false;
		}
		else{
			return true;
		}
		 
	}
};

class SPN : public Dispatch {
public:
	struct cmp{
		bool operator()(Process &a,Process &b){
			return a.Ts>b.Ts;
		}
	};
	priority_queue<Process,vector<Process>,cmp> RQ;
private:
	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
		if(RQ.empty()){
			return true;
		}
		else{
			return false;
		}
	}
	void insert(Process proNew, bool isNewProcess) {
		RQ.push(proNew);
	}
	bool select(Process& proSelect1) {
		proSelect1=RQ.top();
		RQ.pop();
	}
	bool goOnRunning(Process proCurrent) {
			bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
		{
			return false;
		}
		else{
			return true;
		}
		
	}
};

class SRT : public Dispatch {
public:
	struct cmp{
		bool operator()(Process &a,Process &b){
			return a.Ts-a.done>b.Ts-b.done;
		}
	};
	priority_queue<Process,vector<Process>,cmp> RQ;//创建优先级队列 
private:
	void setName() {
		strMethod = "SRT";
	}
	bool nowaitting() {
		if(RQ.empty()){
			return true;
		}
		else{
			return false;
		}
		
	}
	void insert(Process proNew, bool isNewProcess) {
		RQ.push(proNew);
	}
	bool select(Process& proSelect1) {
		proSelect1=RQ.top();
		RQ.pop();
	}
	bool goOnRunning(Process proCurrent) {
		if((proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts)||((proCurrent.Ts-proCurrent.done)>(RQ.top().Ts-RQ.top().done))){
			return false;//如果当前进程已完成或者当前RQ里存在剩余时间比当前进程更短的进程，则存在抢占 
		}
		else{
			return true;
		}
	}
};

class HRRN : public Dispatch {
public:
    vector<Process> RQ;
	
private:
	void setName() {
		strMethod = "HRRN";
	}
	bool nowaitting() {
		if(RQ.empty()){
			return true;
		}
		else{
			return false;
		}
	}
	void insert(Process proNew, bool isNewProcess) {
		RQ.push_back(proNew);
	}
	bool select(Process& proSelect1) {
	    vector<Process>::iterator it=RQ.begin();
	    vector<Process>::iterator iit=RQ.begin();
		Process p=*it;
	    HowLong_TYPE r0=(getCurrentTimeStamp()-it->arrive+it->Ts)/it->Ts;
	    for(;it!=RQ.end();it++){
	    	HowLong_TYPE r=(getCurrentTimeStamp()-it->arrive+it->Ts)/it->Ts;
	    	if(r>=r0){
	    		r0=r;
	    		p=*it;
	    		iit=it;
			}
		}
		proSelect1=p;
		RQ.erase(iit);
		//在vectorRQ中选择r最小的就绪进程运行 
	    
	}
	bool goOnRunning(Process proCurrent) {
			bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
		{
			return false;
		}
		else{
			return true;
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
