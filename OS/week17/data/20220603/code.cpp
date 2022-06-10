#include "dispatch.h"
#include <queue>
#include <list>

class FCFS : public Dispatch {
private:
	queue<Process> ready;
	
	void setName() 
	{
		strMethod = "FCFS";
	}
	
	bool nowaitting() 
	{
		return ready.empty();
	}
	
	void insert(Process proNew, bool isNewProcess) 
	{
		ready.push(proNew);
	}
	
	bool select(Process& proSelcet) 
	{
		if (ready.empty())
		{
			return false;
		}
		else{
			proSelcet = ready.front();
			ready.pop();
			return true;
		}
	}
	
	bool goOnRunning(Process proCurrent)
	{
		if (getCurrentTimeStamp() - proCurrent.tSelected < proCurrent.Ts)
		{
			return true;
		}
		else
		{
			return false;
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
	queue<Process> ready;

	void setName() 
	{
		strMethod = "RR"+to_string(q);
	}
	
	bool nowaitting() 
	{
		return ready.empty(); 
	}
	
	void insert(Process proNew, bool isNewProcess) 
	{
		ready.push(proNew);
	}
	
	bool select(Process& proSelcet) 
	{
		if (ready.empty())
		{
			return false;
		}
		else
		{
			proSelcet = ready.front();
			ready.pop();
			return true;
		}
	}
	
	bool goOnRunning(Process proCurrent) 
	{
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
		{
			return false;
		}
		else if (getCurrentTimeStamp() - proCurrent.tSelected < q)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class SPN : public Dispatch {
private:
	queue<Process> ready;
	
	void setName() 
	{
		strMethod = "SPN";
	}
	
	bool nowaitting() 
	{
		return ready.empty(); 
	}
	
	void insert(Process proNew, bool isNewProcess) 
	{
		ready.push(proNew);
	}
	
	bool select(Process& proSelcet) 
	{
		if (ready.empty())
		{
			return false;
		}
		else
		{
			Process a;
			int i;
			int z = 100;
			int x = ready.size();
			for(i = 0; i < x; i++)
			{
				a = ready.front();
				int y = a.Ts;
				if (y < z)
				{
					z = y;
				} 
				ready.push(ready.front());
				ready.pop();
			}
			for(i = 0; i < x; i++)
			{
				a = ready.front();
				int y = a.Ts;
				if (y == z)
				{
					proSelcet = ready.front();
					ready.pop();
				} 
				ready.push(ready.front());
				ready.pop();
			}
			return true;
		}
	}
	
	bool goOnRunning(Process proCurrent) 
	{
		if ((getCurrentTimeStamp() - proCurrent.tSelected) < proCurrent.Ts)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class SRT : public Dispatch {
private:
	queue<Process> ready;
	
	void setName() 
	{
		strMethod = "SRT";
	}
	
	bool nowaitting() 
	{
		return ready.empty();
	}
	
	void insert(Process proNew, bool isNewProcess) 
	{
		ready.push(proNew);
	}
	
	bool select(Process& proSelcet) 
	{
		if (ready.empty())
		{
			return false;
		}
		else
		{
			Process a;
			int i;
			int z = 100;
			int x = ready.size();
			for(i = 0; i < x; i++)
			{
				a = ready.front();
				int y = a.Ts - a.done;
				if (y < z)
				{
					z = y;
				}
				ready.push(ready.front());
				ready.pop();
			}
			for(i = 0; i < x; i++)
			{
				a = ready.front();
				int y = a.Ts - a.done;
				if (y == z)
				{
					proSelcet = ready.front();
					ready.pop();
					break;
				} 
				ready.push(ready.front());
				ready.pop();
			}
			return true;
		}
	}
	
	bool goOnRunning(Process proCurrent) 
	{	
		Process a;
		Process b;
		int i;
		int z = 100;
		int x = ready.size();
		for(i = 0; i < x; i++)
		{
			a = ready.front();
			int y = a.Ts - a.done;
			if (y < z)
			{
				z = y;
			} 
			ready.push(ready.front());
			ready.pop();
		}
		for(i = 0; i < x; i++)
		{
			a = ready.front();
			int y = a.Ts - a.done;
			if (y == z)
			{
				b = ready.front();
				ready.push(ready.front());
				ready.pop();
			} 
			ready.push(ready.front());
			ready.pop();
		}
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
		{
			return false;
		}
		else 
		{
			if ( (b.Ts - b.done) < (proCurrent.Ts - proCurrent.done - (getCurrentTimeStamp() - proCurrent.tSelected)))
			{
				return false;
			}
			else
			{
				return true;
			}
		}	
	}
};

class HRRN : public Dispatch {
private:
	list <Process> ready;
	
	void setName() 
	{
		strMethod = "HRRN";
	}
	
	bool nowaitting() 
	{
		return ready.empty(); 
	}
	
	void insert(Process proNew, bool isNewProcess) 
	{
		ready.push_back(proNew);
	}
	
	bool select(Process& proSelcet) 
	{
		if (ready.empty())
		{
			return false;
		}
		else
		{
			int i = 0;
			std::list<Process>::iterator x;
			std::list<Process>::iterator y;
			for (x = ready.begin(); x != ready.end(); x++)
			{
				double a = (double)(getCurrentTimeStamp() - (*x).arrive - (*x).done);
				double b = (double)(*x).Ts;
				double temp = (a+b) / b;
				if( i  < temp )
				{
					y = x;
					i = temp;	
				}
			}
			proSelcet = *y;
			ready.erase(y);
			return true;
		}
	}
	
	bool goOnRunning(Process proCurrent) 
	{
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		return !bFinished;
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
int main() 
{
	fout.open("result.txt");
	test();
	fout.close();
	return 0;
}
