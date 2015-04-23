#ifndef TECHTREE_H
#define TECHTREE_H

#include <vector>
#include <map>
#include "MyStructs.h"
#include "json/document.h"
#include "cocos2d.h"

namespace techTree
{
	const std::map<std::string, TechEnum> mTechStringEnumMap =
	{
		{ "techroot", techroot },
		{ "techtree1", techtree1 },
		{ "techtree2", techtree2 },
		{ "techtree3", techtree3 },
		{ "unlocklongrangeunit1", unlocklongrangeunit1 },
		{ "unlocklongrangeunit2", unlocklongrangeunit2 },
		{ "unlockshortrangeunit1", unlockshortrangeunit1 },
		{ "unlockshortrangeunit2", unlockshortrangeunit2 },
		{ "unlocklongrangeunit3", unlocklongrangeunit3 },
		{ "productivity1", productivity1 },
		{ "productivity2", productivity2 },
		{ "productivity3", productivity3 },
		{ "resourcefixed1", resourcefixed1 },
		{ "resourcefixed2", resourcefixed2 },
		{ "resourcefixed3", resourcefixed3 },
		{ "resourcerandom1", resourcerandom1 },
		{ "resourcerandom2", resourcerandom2 },
		{ "resourcerandom3", resourcerandom3 },
		{ "hplongrange1", hplongrange1 },
		{ "hplongrange2", hplongrange2 },
		{ "arglongrange1", arglongrange1 },
		{ "arglongrange2", arglongrange2 },
		{ "atklongrange1", atklongrange1 },
		{ "atklongrange2", atklongrange2 },
		{ "atkshortrange1", atkshortrange1 },
		{ "atkshortrange2", atkshortrange2 },
		{ "defshortrange1", defshortrange1 },
		{ "defshortrange2", defshortrange2 },
		{ "mrgshortrange1", mrgshortrange1 },
		{ "mrgshortrange2", mrgshortrange2 },
		{ "defbase1", defbase1 },
		{ "defbase2", defbase2 }
	};
}
class TechTree
{
public:
	TechTree();
	~TechTree();
	bool unlockable(TechEnum tech);
	void unlock(TechEnum tech);
private:
	std::vector<TechTreeNodeStruct> mTechTree;
	//
	void addTechInit(TechEnum tech, const std::vector<TechEnum> & fathers);
};

#endif // !TECHTREE_H
