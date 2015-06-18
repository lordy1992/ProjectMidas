#include "ChangeStateAction.h"

int ChangeStateAction::addAction(midasMode entryState, CommandData cmd)
{
	if (cmd.type == commandType::STATE_CHANGE)
	{
		// error - can't add state_change or else infinite loops possible.
		return 1;
	}

	std::vector<CommandData> stateMap = changeStateActionMap[entryState];
	stateMap.push_back(cmd);

	changeStateActionMap[entryState] = stateMap;

	return 0;
}

std::vector<CommandData> ChangeStateAction::getActionsForEntryState(midasMode entryState)
{
	return changeStateActionMap[entryState];
}

void ChangeStateAction::clearMap()
{
	changeStateActionMap.clear();
}