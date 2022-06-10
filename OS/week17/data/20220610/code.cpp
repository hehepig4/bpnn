#include "dispatch.h"

class FCFS : public Dispatch {
private:
	queue<Process> wait;
	void setName() {
		strMethod = "FCFS";
	}
	bool nowaitting() {
		return wait.size() == 0;
	}
	void insert(Process proNew, bool isNewProcess) {
		wait.push(proNew);
	}
	bool select(Process& proSelcet) {
		if(nowaitting())
			return false;
		proSelcet = wait.front();
		wait.pop();
		return true;
	}
	bool goOnRunning(Process proCurrent){
		if (getCurrentTimeStamp() - proCurrent.tSelected == proCurrent.Ts)
			return false;
//		cout << proCurrent.id <<proCurrent.done << " " <<proCurrent.Ts<<endl;
		return true;
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
    queue<Process> wait;
	void setName() {
		strMethod = "RR"+to_string(q);
	}
	bool nowaitting() {
		return wait.size() == 0;
	}
	void insert(Process proNew, bool isNewProcess) {

		wait.push(proNew);
	}
	bool select(Process& proSelcet) {
	    if(nowaitting())
			return false;
		proSelcet = wait.front();
		wait.pop();
		return true;
	}
	bool goOnRunning(Process proCurrent) {
		if ((getCurrentTimeStamp() - proCurrent.tSelected == proCurrent.Ts - proCurrent.done) || (getCurrentTimeStamp() - proCurrent.tSelected == q))
			return false;
		return true;
	}
};

class SPN : public Dispatch {
private:
	struct cmp{
		bool operator()(const Process& x, const Process& y)
		{
			if(x.Ts == y.Ts)
			    return x.arrive > y.arrive;
			return x.Ts > y.Ts;
		}
	};
	priority_queue<Process, vector<Process>, cmp> wait;
	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
		return wait.size() == 0;
	}
	void insert(Process proNew, bool isNewProcess) {
	    wait.push(proNew);
	}
	bool select(Process& proSelcet) {
		if(nowaitting())
			return false;
		proSelcet = wait.top();
		wait.pop();
		return true;
	}
	bool goOnRunning(Process proCurrent) {
	    if (getCurrentTimeStamp() - proCurrent.tSelected == proCurrent.Ts - proCurrent.done)
			return false;
		return true;
	}
};

class SRT : public Dispatch {
private:
    struct cmp{
		bool operator()(const Process& x, const Process& y)
		{
			if(x.Ts-x.done == y.Ts-y.done)
				return x.arrive > y.arrive;
			return x.Ts-x.done > y.Ts-y.done;
		}
	};
	priority_queue<Process, vector<Process>, cmp> wait;
	void setName() {
		strMethod = "SRT";
	}
	bool nowaitting() {
	    return wait.size() == 0;
	}
	void insert(Process proNew, bool isNewProcess) {
		wait.push(proNew);
	}
	bool select(Process& proSelcet) {
		if(nowaitting())
			return false;
		proSelcet = wait.top();
		wait.pop();
		return true;
	}
	bool goOnRunning(Process proCurrent) {
	    if (getCurrentTimeStamp() - proCurrent.tSelected == proCurrent.Ts - proCurrent.done)
			return false;

		if(wait.top().id != proCurrent.id)
		{
			if(wait.top().Ts - wait.top().done < proCurrent.Ts - proCurrent.done - getCurrentTimeStamp() + proCurrent.tSelected)
		    	return false;
		}
		return true;
	}
};

class HRRN : public Dispatch {
private:
	vector<Process> wait;
	void setName() {
		strMethod = "HRRN";
	}
	bool nowaitting() {
		return wait.size() == 0;
	}
	void insert(Process proNew, bool isNewProcess) {
		wait.push_back(proNew);
	}
	bool select(Process& proSelcet) {
		if(nowaitting())
			return false;
		int now = getCurrentTimeStamp();
		vector<Process>::iterator it = wait.begin();
		float last = float((now-it->arrive)/it->Ts);
		vector<Process>::iterator res = it;
		for(it=it+1; it != wait.end(); it++)
		{
			float here = float((now-it->arrive)/it->Ts);
			if(last < here)
			{
				last = here;
				res = it;
			}
		}
		proSelcet = *res;
		wait.erase(res);
		return true;
	}
	bool goOnRunning(Process proCurrent) {
		if (getCurrentTimeStamp() - proCurrent.tSelected == proCurrent.Ts - proCurrent.done)
			return false;
		return true;
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
