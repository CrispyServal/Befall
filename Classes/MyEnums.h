#ifndef MYENUMS_H
#define MYENUMS_H

enum GameModeEnum
{
	server = 0,
	client = 1,
	vsPlayer = 2,
	story = 3,
	vsAi = 4
};

enum TechEnum
{
	techroot = 0,
	techtree1 = 1,
	techtree2 = 2,
	techtree3 = 3,
	unlocklongrangeunit1 = 4,
	unlocklongrangeunit2 = 5,
	unlockshortrangeunit1 = 6,
	unlockshortrangeunit2 = 7,
	unlocklongrangeunit3 = 8,
	productivity1 = 9,
	productivity2 = 10,
	productivity3 = 11,
	resourcefixed1 = 12,
	resourcefixed2 = 13,
	resourcefixed3 = 14,
	resourcerandom1 = 15,
	resourcerandom2 = 16,
	resourcerandom3 = 17,
	hplongrange1 = 18,
	hplongrange2 = 19,
	arglongrange1 = 20,
	arglongrange2 = 21,
	atklongrange1 = 22,
	atklongrange2 = 23,
	atkshortrange1 = 24,
	atkshortrange2 = 25,
	defshortrange1 = 26,
	defshortrange2 = 27,
	mrgshortrange1 = 28,
	mrgshortrange2 = 29,
	defbase1 = 30,
	defbase2 = 31
};

enum TechStateEnum
{
	unavailable,
	available,
	unlocking,
	unlocked
};

enum UnitEnum
{
	base,
	farmer,
	shortrangeunit1,
	shortrangeunit2,
	longrangeunit1,
	longrangeunit2,
	longrangeunit3,
	fixedResource,
	randomResource
};

#endif // !MYENUMS_H