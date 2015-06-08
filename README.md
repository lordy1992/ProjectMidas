ProjectMidas
============

A wearable interface for the Windows OS, replacing various peripherals.

Please refer to PMMain/doc/Project Midas.docx for a full system overview.

Currently setup to work with Myo Armband - 
  - SDK Win 0.8.1
  - Firmware 1.2.995
  - Myo Connect V 0.9.1

To build MidasGUI:
- Need Visual Studio 2013
- Download and install Qt 5.4.0 2013 x86 with opengl (qt-opensource-windows-x86-msvc2013_opengl-5.4.0.exe from http://download.qt.io/archive/qt/5.4/5.4.0/)
- Put the bin folder of the Qt SDK onto your PATH environment variable
- Download and install the most recent Qt Visual Studio Plugin (http://www.qt.io/download-open-source/#section-2)
- Download Boost 1.57.0 (http://www.boost.org/users/history/version_1_57_0.html)
- Unzip Boost so that your file system has the following structure: C:\Program Files\boost\boost_1_57_0\* , where '*' holds the boost, docs, lib, etc folders
- You may need to copy Hub_console.lib from the Midas/MyoSim directory to MidasGUI/MyoSim, or remove the dependancy from the project file to Hub_console.lib, as it is only required if you use the MyoSim build settings.
- Build and enjoy :)
