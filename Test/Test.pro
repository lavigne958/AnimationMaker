#    Copyright (C) 2018 Olaf Japp
#    https://facebook.com/artanidos
#
#    This file is part of AnimationMaker.
#
#    AnimationMaker is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    AnimationMaker is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with AnimationMaker.  If not, see <http://www.gnu.org/licenses/>.

QT += widgets testlib xml svg

SOURCES += \
    testgui.cpp \
    ../Widgets/flatbutton.cpp \
    ../Widgets/itemhandle.cpp \
    ../Widgets/keyframe.cpp \
    ../Widgets/animationscene.cpp \
    ../Widgets/commands.cpp \
    ../Widgets/timeline.cpp \
    ../Widgets/playhead.cpp \
    ../Widgets/transitionline.cpp \
    ../Widgets/ellipse.cpp \
    ../Widgets/rectangle.cpp \
    ../Widgets/text.cpp \
    ../Widgets/vectorgraphic.cpp \
    ../Widgets/bitmap.cpp \
    ../Widgets/plugins.cpp \
    ../Widgets/expander.cpp \
    ../Widgets/animationitem.cpp \
    ../Widgets/coloreditor.cpp \
    ../Widgets/colorpicker.cpp \
    ../Widgets/colorrect.cpp \
    ../Widgets/xmleditor.cpp \
    ../Widgets/xmlhighlighter.cpp \
    ../Widgets/transition.cpp \
    ../Widgets/keyframehandle.cpp \
    ../Widgets/transitionhandleright.cpp \
    ../Widgets/transitionhandleleft.cpp \
    ../Widgets/transitionhandle.cpp \
    ../Widgets/itempropertyeditor.cpp \
    ../Widgets/mainwindow.cpp \
    ../Widgets/scenepropertyeditor.cpp \
    ../Widgets/sceneview.cpp \
    ../Widgets/transitioneditor.cpp \
    ../Widgets/svgattributeeditor.cpp \
    ../Widgets/ruler.cpp

HEADERS += \
        ../Widgets/flatbutton.h \
        ../Widgets/itemhandle.h \
        ../Widgets/keyframe.h \
        ../Widgets/animationscene.h \
        ../Widgets/commands.h \
        ../Widgets/timeline.h \
        ../Widgets/playhead.h \
        ../Widgets/transitionline.h \
        ../Widgets/ellipse.h \
        ../Widgets/rectangle.h \
        ../Widgets/text.h \
        ../Widgets/vectorgraphic.h \
        ../Widgets/bitmap.h \
        ../Widgets/plugins.h \
        ../Widgets/interfaces.h \
        ../Widgets/expander.h \
        ../Widgets/propertyeditorinterface.h \
        ../Widgets/animationitem.h \
        ../Widgets/coloreditor.h \
        ../Widgets/colorpicker.h \
        ../Widgets/colorrect.h \
        ../Widgets/xmleditor.h \
        ../Widgets/xmlhighlighter.h \
        ../Widgets/transition.h \
        ../Widgets/keyframehandle.h \
        ../Widgets/transitionhandleright.h \
        ../Widgets/transitionhandleleft.h \
        ../Widgets/transitionhandle.h \
        ../Widgets/itempropertyeditor.h \
        ../Widgets/mainwindow.h \
        ../Widgets/scenepropertyeditor.h \
        ../Widgets/sceneview.h \
        ../Widgets/transitioneditor.h \
        ../Widgets/interfaces.h \
        ../Widgets/svgattributeeditor.h \
        ../Widgets/ruler.h

DEFINES += TEST
INCLUDEPATH += ../

