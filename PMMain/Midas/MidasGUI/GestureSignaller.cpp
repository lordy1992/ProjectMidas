#include "GestureSignaller.h"

bool GestureSignaller::showAll;

GestureSignaller::GestureSignaller()
{
}


GestureSignaller::~GestureSignaller()
{
}

bool GestureSignaller::getShowAll()
{
    return showAll;
}

void GestureSignaller::handleShowAllToggle(bool showAll)
{
    this->showAll = showAll;
}
