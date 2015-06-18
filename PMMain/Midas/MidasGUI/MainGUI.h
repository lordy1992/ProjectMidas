#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include <QDialog>
#include "MouseIndicator.h"
#include "SequenceDisplayer.h"
#include "InfoIndicator.h"
#include "GestureSignaller.h"
#include "PoseDisplayer.h"
#include "ProfileDisplayer.h"
#include "ProfileSignaller.h"
#include "ProfileManager.h"

/**
 * The MainGUI class is the parent GUI of all the widgets used in Midas. It contains the mouse
 * indicator, sequence displayer, and info indicator. The mouse indicator shows the relative mouse 
 * velocity, the sequence displayer shows the gesture sequences, and the info indicator shows the
 * current status of Midas.
 */
class MainGUI : public DraggableWidget
{
    Q_OBJECT

public:
    /**
     * The constructor for the MainGUI.
     *
     * @param mainThread The main Midas thread; used to pass information between the GUI and back-end.
     * @param deadZoneRad The radius of the dead zone in the mouse indicator.
     */
    MainGUI(MidasThread *mainThread, ProfileManager* pm, int deadZoneRad);
    ~MainGUI();

    void connectSignallerToProfileWidgets(ProfileSignaller* signaller);

    void connectSignallerToInfoIndicator(GestureSignaller *signaller);
    void connectSignallerToSequenceDisplayer(GestureSignaller *signaller);
    void connectSignallerToPoseDisplayer(GestureSignaller *signaller);

private:
    QVBoxLayout *layout;
    MouseIndicator *mouseIndicator;
    InfoIndicator *infoIndicator;
    SequenceDisplayer *sequenceDisplayer;
    PoseDisplayer *poseDisplayer;
	PoseDisplayer *poseDisplayer2;
	PoseDisplayer *poseDisplayer3;
    std::vector<ProfileDisplayer*> profileWidgets;
    int totalWidth, totalHeight;
};

#endif