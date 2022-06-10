#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <unordered_map>
#include <cassert>
using namespace std;
#define INVALID -1

const int UControl = 2;
class ControlBits {
public:
	ControlBits() :iControl(0) {}
	ControlBits(int control) { iControl = (INVALID == control ? 0 : control); }

	void set(int _control)
	{
		iControl = _control;
	}

	bool getBit(int iWhich) const {
		return (iControl & (1 << iWhich));
	}
	void setBit(int iWhich) {
		iControl = (iControl | (1 << iWhich));
	}

	void resetBit(int iWhich) {
		iControl = (iControl & (~(1 << iWhich)));
	}
private:
	int iControl;
};

class PageItem {
public:
	PageItem(int pageNo = INVALID) :iControl(0), iPageNo(pageNo) {}
	ControlBits iControl;
	int iPageNo;

	bool bInvalid()
	{
		return iPageNo == INVALID;
	}
};

class ClockPolicy {
public:
	ClockPolicy(int capacity = 3) :iCapacity(capacity), vpi(capacity), iCurrent(0) {}

	void visitPage(PageItem* pi)
	{
		//write your code here
		unordered_map<int, int>::const_iterator got = pageidTovpiIndex.find(pi->iPageNo);
		//find, then set bit to 1
		if(got != pageidTovpiIndex.end())
		{
			vpi[got->second]->iControl.setBit(UControl);
		}
		else//not find, then find a place to insert it
		{
			findReplacePosition(pi);
			pageidTovpiIndex[pi->iPageNo] = iCurrent;
			vpi[iCurrent] = pi;
			vpi[iCurrent]->iControl.setBit(UControl);
			iCurrent = (iCurrent + 1) % iCapacity;
		}
	}

	void show() {
		cout << "current=" << iCurrent << '\t';
		cout << "buffer:\t";
		for (int ii = 0; ii < iCapacity; ++ii)
		{
			if (NULL == vpi[ii])
				break;
			cout << vpi[ii]->iPageNo;
			if (vpi[ii]->iControl.getBit(UControl))
				cout << '*';
			cout << '\t';
		}
		cout << endl;
	}

private:
	int iCapacity;
	int iCurrent;
	vector<PageItem*> vpi;
	unordered_map<int, int> pageidTovpiIndex;

	void findReplacePosition(PageItem* pi)
	{
		for (int ii = 0; ii <= iCapacity; ++ii)
		{
			if (iCurrent >= iCapacity)
				iCurrent -= iCapacity;
			if (NULL == vpi[iCurrent] || vpi[iCurrent]->bInvalid())
				break;
			if (vpi[iCurrent]->iControl.getBit(UControl))
				vpi[iCurrent]->iControl.resetBit(UControl);
			else
			{
				//get the position to replace. need to erase the pageNo in the unorder_map
				pageidTovpiIndex.erase(pageidTovpiIndex.find(vpi[iCurrent]->iPageNo));
				break;
			}
			++iCurrent;
		}
		//when it gets here, the iCurrent is the one need to replace.
	}
};

void load(vector<PageItem>& vpi)
{
	ifstream fin("visitOrder.txt");
	int iPageID;
	while (!fin.eof()) {
		fin >> iPageID;
		vpi.push_back(PageItem(iPageID));
	}
	fin.close();
}

void test() {
	vector<PageItem> vpi;
	load(vpi);

	ClockPolicy cp(3);
	for (vector<PageItem>::iterator iter = vpi.begin(); iter != vpi.end(); ++iter)
	{
		cp.visitPage(&(*iter));
		cp.show();
	}
}

int main() {
	test();
	return 0;
}
