#include <queue>
#include <vector>
#include <deque>
#include "dispatch.h"

using namespace std;


bool operator==(const Process& a, const Process& b)
{
	return a.id == b.id && a.arrive == b.arrive && a.Ts == b.Ts;
}

class FCFS : public Dispatch {
private:
	queue<Process> qReady;
	void setName() {
		strMethod = "FCFS";
	}
	bool nowaitting() {
		return qReady.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		qReady.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (qReady.empty())
		{
			return false;
		}
		else
		{
			proSelcet = qReady.front();
			qReady.pop();
			return true;
		}
	}
	bool goOnRunning(Process proCurrent) {
		return getCurrentTimeStamp() - proCurrent.tSelected < proCurrent.Ts;
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
	queue<Process> qReady;

	void setName() {
		strMethod = "RR" + to_string(q);
	}
	bool nowaitting() {
		return qReady.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		qReady.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (qReady.empty())
		{
			return false;
		}
		else
		{
			proSelcet = qReady.front();
			qReady.pop();
			return true;
		}
	}
	bool goOnRunning(Process proCurrent) {
		return getCurrentTimeStamp() - proCurrent.tSelected + proCurrent.done < proCurrent.Ts && getCurrentTimeStamp() < proCurrent.tSelected + q;
	}
};

class cmpForSPN
{
public:
	bool operator()(const Process& a, const Process& b)
	{
		return a.Ts > b.Ts;
	}
};

class SPN : public Dispatch {
private:
	priority_queue<Process, vector<Process>, cmpForSPN> qReady;
	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
		return qReady.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		qReady.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (qReady.empty())
		{
			return false;
		}
		else
		{
			proSelcet = qReady.top();
			qReady.pop();
			return true;
		}
	}
	bool goOnRunning(Process proCurrent) {
		return getCurrentTimeStamp() - proCurrent.tSelected < proCurrent.Ts;;
	}
};

class cmpForSRT
{
public:
	bool operator()(const Process& a, const Process& b)
	{
		return a.Ts - a.done > b.Ts - b.done;
	}
};

class SRT : public Dispatch {
private:
	priority_queue<Process, vector<Process>, cmpForSRT> qReady;
	void setName() {
		strMethod = "SRT";
	}
	bool nowaitting() {
		return qReady.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		qReady.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (qReady.empty())
		{
			return false;
		}
		else
		{
			proSelcet = qReady.top();
			qReady.pop();
			return true;
		}
	}
	bool goOnRunning(Process proCurrent) {
		return cmpForSRT()(qReady.top(), proCurrent) && getCurrentTimeStamp() - proCurrent.tSelected + proCurrent.done < proCurrent.Ts;
	}
};

class HRRN : public Dispatch {
private:
	vector<Process> qReady;

	Process getProcessWithMaxRValue() {
		Process max = qReady[0];
		for (int i = 1; i < qReady.size(); i++)
		{
			if (((double)(getCurrentTimeStamp() - qReady[i].arrive - qReady[i].done + qReady[i].Ts) / qReady[i].Ts) > ((double)(getCurrentTimeStamp() - max.arrive - max.done + max.Ts) / max.Ts))
			{
				max = qReady[i];
			}
		}
		return max;
	}

	void setName() {
		strMethod = "HRRN";
	}
	bool nowaitting() {
		return qReady.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		qReady.push_back(proNew);
	}
	bool select(Process& proSelcet) {
		if (qReady.empty())
		{
			return false;
		}
		else
		{
			Process m = getProcessWithMaxRValue();
			proSelcet = m;
			for (auto iter = qReady.begin(); iter != qReady.end(); iter++)
			{
				if (*iter == m)
				{
					qReady.erase(iter);
					break;
				}
			}
			return true;
		}
	}
	bool goOnRunning(Process proCurrent) {
		return getCurrentTimeStamp() - proCurrent.tSelected + proCurrent.done < proCurrent.Ts;
	}
};

class Feedback : public Dispatch {
public:
	Feedback(int _n = 3) : n(_n), vRQ(_n + 1), iLevel(-1), count(0) {	}

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