#include "dispatch.h"

class FCFS : public Dispatch {
public:
	FCFS(int _n=3) : n(_n),count(0) {	}
private:
	int n;
	queue<Process> vRQ;
	int count; //how many waitting processes.

	void setName() {
		strMethod = "FCFS";
	}
	void insert(Process proNew, bool isNewProcess) {
		vRQ.push(proNew);
	}
	bool nowaitting()
	{
		return count == 0;
	}
	bool select(Process& proSelcet) {
		bool bFound = false;
		for (int ii = 0; ii <= n; ++ii)
		{
			if (vRQ.empty())
				continue;
			bFound = true;
			proSelcet = vRQ.front();
			vRQ.pop();
			--count;
			break;
		}
		return bFound;
	}
	bool goOnRunning(Process proCurrent) {
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
			return false;

		HowLong_TYPE duration = getCurrentTimeStamp() - proCurrent.tSelected;
		return (duration < proCurrent.Ts);
	}
};

class RR : public Dispatch {
public:
	RR(int _n=3) : n(_n),count(0) {	}
	void setQ(int _q)
	{
		q = _q;
	}
private:
	int q;
	int n;
	queue<Process> vRQ;
	int count; //how many waitting processes.

	void setName() {
		strMethod = "RR"+to_string(q);
	}
	bool nowaitting() {
		return count == 0;
	}
	void insert(Process proNew, bool isNewProcess) {
		vRQ.push(proNew);
	}
	bool select(Process& proSelcet) {
		bool bFound = false;
		for (int ii = 0; ii <= n; ++ii)
		{
			if (vRQ.empty())
				continue;
			bFound = true;
			proSelcet = vRQ.front();
			vRQ.pop();
			--count;
			break;
		}
		return bFound;
	}
	bool goOnRunning(Process proCurrent) {
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
			return false;
		
		HowLong_TYPE duration = getCurrentTimeStamp() - proCurrent.tSelected;
		return (duration < q);
	}
};

class SPN : public Dispatch {
public:
	SPN(int _n=3) : n(_n),count(0) {	}
private:
	struct cmp{
		bool operator() (Process &a, Process &b){
			if (a.Ts > b.Ts)
				return true;
			else if (a.Ts == b.Ts)
				if (a.arrive > b.arrive)
					return true;
				else
					return false;
			else
				return false;
		}
	};
	
	int n;
	priority_queue<Process, vector<Process>, cmp> vRQ;
	int count; //how many waitting processes.
	
	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
		return count == 0;
	}
	void insert(Process proNew, bool isNewProcess) {
		vRQ.push(proNew);
	}
	bool select(Process& proSelcet) {
		bool bFound = false;
		for (int ii = 0; ii <= n; ++ii)
		{
			if (vRQ.empty())
				continue;
			bFound = true;
			proSelcet = vRQ.top();
			vRQ.pop();
			--count;
			break;
		}
		return bFound;
	}
	bool goOnRunning(Process proCurrent) {
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
			return false;
		else
			return true;
	}
};

class SRT : public Dispatch {
public:
	SRT(int _n=3) : n(_n),count(0) {	}
private:
	struct cmp{
		bool operator() (Process &a, Process &b){
			if ((a.Ts - a.done) > (b.Ts - b.done))
				return true;
			else if ((a.Ts - a.done) == (b.Ts - b.done))
				if (a.arrive > b.arrive)
					return true;
				else
					return false;
			else
				return false;
		}
	};
	
	int n;
	priority_queue<Process, vector<Process>, cmp> vRQ;
	int count; //how many waitting processes.
	
	void setName() {
		strMethod = "SRT";
	}
	bool nowaitting() {
		return count == 0;
	}
	void insert(Process proNew, bool isNewProcess) {
		vRQ.push(proNew);
	}
	bool select(Process& proSelcet) {
		bool bFound = false;
		for (int ii = 0; ii <= n; ++ii)
		{
			if (vRQ.empty())
				continue;
			bFound = true;
			proSelcet = vRQ.top();
			vRQ.pop();
			--count;
			break;
		}
		return bFound;
	}
	bool goOnRunning(Process proCurrent) {
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
			return false;
	}
};

class HRRN : public Dispatch {
public:
	//HRRN(int _n=3) : n(_n),count(0) {	}
private:
	//struct cmp{
	//	bool operator() (Process &a, Process &b){
	//		HowLong_TYPE a1 = (/*getCurrentTimeStamp() - */a.tSelected + a.Ts) / a.Ts;
	//		HowLong_TYPE b1 = (/*getCurrentTimeStamp() - */b.tSelected + b.Ts) / b.Ts;
	//		if (a1 < b1)
	//			return true;
	//		else if (a1 == b1)
	//			if (a.arrive > b.arrive)
	//				return true;
	//			else
	//				return false;
	//		else
	//			return false;
	//	}
	//};
	
	//int n;
	//priority_queue<Process, vector<Process>, cmp> vRQ;
	int count; //how many waitting processes.
	vector <Process> vHRRN;
	void setName() {
		strMethod = "HRRN";
		count=0;
	}
	bool nowaitting() {
		return count == 0;
	}
	void insert(Process proNew, bool isNewProcess) {
		vHRRN.push_back(proNew);
		count++;
	}
	bool select(Process& proSelcet) {
		if(count!=0)
		{
			int flag;
			int maxx=0;
			for(int i=0;i<count;i++)
			{
				int t;
				t=(getCurrentTimeStamp()-vHRRN[i].arrive+vHRRN[i].Ts)/vHRRN[i].Ts;
				if(t>maxx)
				{
					maxx=t;
					flag=i;
				}
			}
			proSelcet=vHRRN[flag];
			vHRRN.erase(vHRRN.begin()+flag);
			count--;
			return true;
		}
		return false;
	}
	bool goOnRunning(Process proCurrent) {
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
			return false;

		HowLong_TYPE duration = getCurrentTimeStamp() - proCurrent.tSelected;
		return (duration < proCurrent.Ts);
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
