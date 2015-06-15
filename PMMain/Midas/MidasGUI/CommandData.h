#pragma once

#include "MidasCommon.h"
#include "KeyboardVector.h"

// TODO TEMP - modify member varialbes to be private.

/**
* This bundles the Midas command data. It represents
* all Midas command types.
*/
class CommandData
{
public:
	CommandData()
	{
		this->type = commandType::NONE;
		this->name = "";
	}

	CommandData(commandType type, action action, std::string name, KeyboardVector vec)
	{
		this->type = type;
		this->action = action;
		this->name = name;
		this->keyboardVector = vec;
	}
	~CommandData();

	commandType type;
	action action;

	std::string name;
	KeyboardVector keyboardVector;
};

