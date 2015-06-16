#include "CommandData.h"


CommandData::~CommandData()
{
}

int CommandData::addChangeStateAction(CommandData cmd)
{
	if(( this->type != commandType::STATE_CHANGE) || (cmd.type == commandType::STATE_CHANGE))
	{
		// error - can't add state_change or else infinite loops possible.
		// or error - can only add action to state_change commands.
		return 1;
	}

	changeStateActions.push_back(cmd);

	return 0;
}

void CommandData::clearChangeStateActions()
{
	changeStateActions.clear();
}