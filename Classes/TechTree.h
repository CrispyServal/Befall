#ifndef TECHTREE_H
#define TECHTREE_H

#include <vector>
#include "MyStructs.h"

class TechTree
{
public:
	TechTree();
	~TechTree();
	bool unlockable(TechEnum tech);
	void unlock(TechEnum tech);
private:
	std::vector<TechTreeNodeStruct> mTechTree;
};

#endif // !TECHTREE_H
