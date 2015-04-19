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
	techroot,
	techtree1,
	techtree2,
	techtree3,
	unlocklongrangeunit1,
	unlocklongrangeunit2,
	unlockshortrangeunit1,
	unlockshortrangeunit2,
	unlocklongrangeunit3,
	productivity1,
	productivity2,
	productivity3,
	resourcefixed1,
	resourcefixed2,
	resourcefixed3,
	resourcerandom1,
	resourcerandom2,
	resourcerandom3,
	hplongrange1,
	hplongrange2,
	arglongrange1,
	arglongrange2,
	atklongrange1,
	atklongrange2,
	atkshortrange1,
	atkshortrange2,
	defshortrange1,
	defshortrange2,
	mrgshortrange1,
	mrgshortrange2,
	defbase1,
	defbase2
};

enum TechStateEnum
{
	unavailable,
	available,
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