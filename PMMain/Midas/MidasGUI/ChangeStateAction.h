#ifndef CHANGE_STATE_ACTION_H
#define CHANGE_STATE_ACTION_H

#include "CommandData.h"
#include <vector>
#include <map>

class ChangeStateAction {
public:
	/* 
	* Return 0 for success, positive otherwise 
	*/
	int addAction(midasMode entryState, CommandData cmd);

	std::vector<CommandData> getActionsForEntryState(midasMode entryState);

	void clearMap();

private:
	std::map<midasMode, std::vector<CommandData>> changeStateActionMap;
};

#endif /* CHANGE_STATE_ACTION_H */