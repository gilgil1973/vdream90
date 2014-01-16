rem ---------------------------------------------------------------------------
rem vdream
rem ---------------------------------------------------------------------------

rem vdreamd.lib
cd lib
qmake "CONFIG+=debug" "QT-=gui"
nmake
nmake clean
cd ..

rem vdreamd_gui.lib
cd lib
qmake "CONFIG+=debug" "QT+=gui" "QT+=widgets"
nmake
nmake clean
cd ..

rem vdream.lib
cd lib
qmake "CONFIG+=release" "QT-=gui"
nmake
nmake clean
cd ..

rem vdream_gui.lib
cd lib
qmake "CONFIG+=release" "QT+=gui" "QT+=widgets"
nmake
rem nmake clean
cd ..

rem ---------------------------------------------------------------------------
rem app
rem ---------------------------------------------------------------------------
qmake
nmake
