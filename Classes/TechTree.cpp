#include "TechTree.h"

using namespace cocos2d;
using techTree::mTechStringEnumMap;
void TechTree::addTechInit(TechEnum tech, const std::vector<TechEnum> & fathers)
{
	std::vector<int> indexFather;
	for (auto father : fathers)
	{
		for (int i = 0; i < mTechTree.size(); ++i)
		{
			if (mTechTree[i].techName == father)
			{
				indexFather.push_back(i);
			}
		}
	}
	if (indexFather.empty())
	{
		//error
	}
	mTechTree.push_back(TechTreeNodeStruct{ tech, indexFather, false });
}

TechTree::TechTree()
{
	//init here
	mTechTree.clear();
	mTechTree.push_back(TechTreeNodeStruct{ techroot, std::vector<int>{0}, true });
	//get json file
	auto jsonFile = FileUtils::getInstance()->fullPathForFilename("dictionary/techtree.json");
    CCLOG("%s",jsonFile.c_str());
	ssize_t size = 0;
	unsigned char * loadStr = FileUtils::getInstance()->getFileData(jsonFile, "r", &size);
	std::string jsonStr = std::string((const char *)loadStr, size);
	//json
	rapidjson::Document document;
	document.Parse<0>(jsonStr.c_str());
	if (document.HasParseError())
	{
		CCLOG("json parsing error!");
	}
	if (!document.IsObject())
	{
		CCLOG("not object");
	}
	rapidjson::Value & techs = document["techs"];
	if (techs.IsArray())
	{
		for (int i = 0; i < techs.Size(); ++i)
		{
			//CCLOG("this:  %s", techs[i]["tech"].GetString());
			rapidjson::Value & fathers = techs[i]["fathers"];
			std::vector<TechEnum> fathersList;
			if (fathers.IsArray())
			{
				for (int j = 0; j < fathers.Size(); ++j)
				{
					if (fathers[j].IsString())
					{
						//CCLOG("fathers: %s", fathers[j].GetString());
						fathersList.push_back(mTechStringEnumMap.at(std::string{ fathers[j].GetString() }));
					}
				}
			}
			addTechInit( mTechStringEnumMap.at(techs[i]["tech"].GetString()),fathersList);
		}
	}
}

TechTree::~TechTree()
{
}

void TechTree::unlock(TechEnum tech)
{
	for (auto & i : mTechTree)
	{
		if (i.techName == tech)
		{
			i.isUnclocked = true;
			return;
		}
	}
	//error
}

bool TechTree::isUnlocked(TechEnum tech)
{
	for (const auto & i : mTechTree)
	{
		if (i.techName == tech)
		{
			return i.isUnclocked;
		}
	}
}

bool TechTree::unlockable(TechEnum tech)
{
	CCLOG("\n");
	if (tech == techroot)
	{
		return false;
	}
	for (auto i : mTechTree)
	{
		if (i.techName == tech)
		{
			CCLOG("checked %d", tech);
			for (auto f : i.indexParents)
			{
				CCLOG("check father: %d,unlock? %d", f, mTechTree[f].isUnclocked);
				if (mTechTree[f].isUnclocked == false)
					return false;
			}
			return true;
		}
	}
	//error
}
