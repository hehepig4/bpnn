#include "dispatch.h"
#include <map>
class FCFS : public Dispatch {
private:
	void setName() {
		strMethod = "FCFS";
	}
	bool nowaitting() {
		return wProcess.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		wProcess.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (!wProcess.empty()) {
			Process nex = wProcess.front();
			proSelcet = nex;
			wProcess.pop();
			return true;
		} else{
			return false;
		}
	}
	bool goOnRunning(Process proCurrent){
		TimeStamp_TYPE trun= getCurrentTimeStamp()-proCurrent.tSelected;
		bool res= proCurrent.done + trun < proCurrent.Ts;
		if(!res) {
			return false;
		} else {
			return true;
		}
	}
	queue<Process> wProcess;
};

class RR : public Dispatch {
public:
	void setQ(int _q)
	{
		q = _q;
	}
private:
	int q;

	void setName() {
		strMethod = "RR"+to_string(q);
	}
	bool nowaitting() {
		return wProcess.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		wProcess.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (!wProcess.empty()) {
			proSelcet = wProcess.front();
			wProcess.pop();
			return true;
		} else{
			return false;
		}
	}
	bool goOnRunning(Process proCurrent) {
		TimeStamp_TYPE trun= getCurrentTimeStamp()-proCurrent.tSelected;
		bool res = proCurrent.done+trun < proCurrent.Ts;	
		bool rr = trun < q;
		if (res && rr) return true;
		return false;
	}
	queue<Process> wProcess;
};

class SPN : public Dispatch {
private:
	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
		return wProcess.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		wProcess.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (!wProcess.empty()) {
			proSelcet = wProcess.top();
			wProcess.pop();
			return true;
		} else{
			return false;
		}
	}
	bool goOnRunning(Process proCurrent) {
		TimeStamp_TYPE trun= getCurrentTimeStamp()-proCurrent.tSelected;
		bool res = proCurrent.done + trun < proCurrent.Ts;
		if(res) return true;
		else {
			return false;
		}
	}
	struct opt{
		bool operator()(Process a, Process b) {
			return a.Ts > b.Ts;
		}
	};
	priority_queue<Process,vector<Process>,opt> wProcess;
};

class SRT : public Dispatch {
private:
	void setName() {
		strMethod = "SRT";
	}
	bool nowaitting() {
		return wProcess.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		wProcess.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (!wProcess.empty()) {
			proSelcet = wProcess.top();
			wProcess.pop();
			return true;
		} else{
			return false;
		}
	}
	bool goOnRunning(Process proCurrent) {
		TimeStamp_TYPE trun= getCurrentTimeStamp()-proCurrent.tSelected;
		bool res = proCurrent.done + trun < proCurrent.Ts;
		bool islast= proCurrent.id==wProcess.top().id;
		if(res&&islast) return true;
		else {
			return false;
		}
	}
	struct opt{
		bool operator()(Process a, Process b) {
			return a.Ts-a.done > b.Ts-b.done;
		}
	};
	priority_queue<Process,vector<Process>,opt> wProcess;
};

class HRRN : public Dispatch {
private:
	void setName() {
		strMethod = "HRRN";
	}
	bool nowaitting() {
		return wProcess.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		wProcess[proNew.id] = proNew;
		wProcessOnV.push(pair<ID_TYPE,float>(proNew.id,-((float)proNew.arrive)/(float)proNew.Ts));
		wProcessOnB.push(pair<ID_TYPE,float>(proNew.id,-1./(float)proNew.Ts));
	}
	bool select(Process& proSelcet) {
		if (!wProcess.empty()) {
			bool flag = false;
			int cont = 0;
			int max = 0;
			while (!flag) {
				auto iter1=wProcessOnV.
			}
		}
	}
	bool goOnRunning(Process proCurrent) {
		TimeStamp_TYPE trun= getCurrentTimeStamp()-proCurrent.tSelected;
		bool res = proCurrent.done + trun < proCurrent.Ts;
		if(res) return true;
		else {
			return false;
		}
	}
	struct opt{
		bool operator()(pair<ID_TYPE,float> a, pair<ID_TYPE,float> b) {
			return a.second>b.second;
		}
	};
	map<ID_TYPE,Process> wProcess;
	priority_queue<pair<ID_TYPE,float>,vector<pair<ID_TYPE,float> >,opt> wProcessOnV;
	priority_queue<pair<ID_TYPE,float>,vector<pair<ID_TYPE,float> >,opt> wProcessOnB;
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
