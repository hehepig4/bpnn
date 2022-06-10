#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <unordered_map>
#include <cassert>
#include <queue>
using namespace std;
typedef char ID_TYPE;
typedef int TimeStamp_TYPE;
typedef int HowLong_TYPE;
#define NO_PROCESS ' '
#define MAX_TimeStamp 100

ofstream fout;

class Process {
public:
	Process() : done(0), id(NO_PROCESS) {}
	Process(ID_TYPE _id, TimeStamp_TYPE _arrive, HowLong_TYPE _Ts) :id(_id), arrive(_arrive), Ts(_Ts), done(0) {}

	ID_TYPE id;
	int arrive;
	HowLong_TYPE Ts;

	HowLong_TYPE done;//how much has been done. 0<= done <= Ts.
	TimeStamp_TYPE tSelected;//when it is selected to run.
};

class LoadProcess {
public:
	TimeStamp_TYPE tLast;
	ifstream fin;
	Process* pProcess;

	LoadProcess() : tLast(-1), pProcess(NULL) {
		fin.open("process.txt");
	}

	//if there exists a process comes at tCurrent timestamp, then load it in pResult, and return true.
	//otherwise, return false.
	bool load(TimeStamp_TYPE tCurrent, Process& pResult)
	{
		if (tCurrent <= tLast) {
			cout << "Error: loading a process happended before last loading" << endl;
			return false;
		}
		if (pProcess == NULL)
		{
			if (fin.eof())
				return false;
			fin >> pBuffer.id >> pBuffer.arrive >> pBuffer.Ts;
			pProcess = &pBuffer;
		}
		assert(pProcess);
		if (pProcess->arrive == tCurrent)
		{
			//find the process
			pResult = *pProcess;
			pProcess = NULL;
			tLast = tCurrent;
			return true;
		}
		else if (pProcess->arrive > tCurrent)
			return false; // the process haven't came.
		else
		{
			cout << "Error: miss one process" << endl;
			return false;
		}
	}
private:
	Process pBuffer;
};

class Dispatch {
private:
	TimeStamp_TYPE tMax;
	TimeStamp_TYPE tCurrent;
	Process proCurrent;
	Process proSelect;
	LoadProcess lp;
	vector<ID_TYPE> vRecords;

	void handleNext()
	{
		bool bSelected = select(proSelect);
		if (bSelected)
		{
			proSelect.tSelected = tCurrent;
			proCurrent = proSelect;
			vRecords[tCurrent] = proCurrent.id;
			cout << "select Process " << proCurrent.id << endl;
		}
		else
		{
			cout << "cpu is free from timestamp:" << tCurrent << endl;
			proCurrent.id = NO_PROCESS;
			vRecords[tCurrent] = NO_PROCESS;
		}
	}
	void next() {
		cout << "At timestamp " << tCurrent << ", ";

		Process proNew;
		//push the new come process at tCurrent into the ready queue.
		if (lp.load(tCurrent, proNew))
		{
			insert(proNew,true);
			cout << "Process " << proNew.id << " comes, ";
		}

		if (proCurrent.id == NO_PROCESS)
		{
			cout << "cpu is free currently,";
			
			//handle the next process
			handleNext();
		}
		else {
			//stop the current running process if it should be stopped, push to the ready queue if needed.
			if (goOnRunning(proCurrent))
			{
				cout << "Process " << proCurrent.id << " goes on running" << endl;
				vRecords[tCurrent] = proCurrent.id;
				return;
			}

			//check if the current process finished		
			proCurrent.done += tCurrent - proCurrent.tSelected;
			bool bFinished = (proCurrent.id == NO_PROCESS || proCurrent.done == proCurrent.Ts);
			bool bNoWaitting = nowaitting();
			
			if (!bFinished)
			{
				if (!bNoWaitting)
				{
					//if no finish and exist waitting process, put it back to the ready queue.
					insert(proCurrent, false);
				}
				else
				{
					//no finish, no one waitting
					proCurrent.tSelected = tCurrent;
					vRecords[tCurrent] = proCurrent.id;
					cout << "Process " << proCurrent.id << " should stop but no one waitting so continue" << endl;
					return;
				}
			}
			//if yes, consider to select another
			//handle the next process
			handleNext();
		}		
	}

protected:
	Dispatch() :tCurrent(0), tMax(20) { vRecords.resize(tMax+1); }
	TimeStamp_TYPE getCurrentTimeStamp() { return tCurrent; }

	string strMethod;
	virtual void setName() = 0;

	virtual void insert(Process proNew, bool isNewProcess) = 0;
	virtual bool nowaitting() = 0;
	virtual bool select(Process& proSelcet) = 0; //if select one process, return true; otherwise return false.
	virtual bool goOnRunning(Process proCurrent) = 0;//should go on running then return true; otherwise return false.

	void showRecords()
	{
		cout << "The overall running:" << endl;
		for (int ii = 0; ii < vRecords.size(); ++ii)
			cout << vRecords[ii];
		cout << "reach max runtime"<<endl;
		
		fout << strMethod << ":";
		for (int ii = 0; ii < vRecords.size(); ++ii)
			fout << vRecords[ii];
		fout << endl;
	}

public:
	
	void resetMaxTimeStamp(TimeStamp_TYPE _tMax) {
		tMax = _tMax;
		vRecords.resize(tMax+1);
	}
	void run()
	{
		setName();
		while (tCurrent < tMax) {
			next();
			++tCurrent;
		}
		showRecords();
	}
};