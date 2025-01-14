/****************************************************************************
** Copyright (C) 2016 Olaf Japp
**
** This file is part of AnimationMaker.
**
**  AnimationMaker is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  AnimationMaker is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with AnimationMaker.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef TRANSITIONLINE_H
#define TRANSITIONLINE_H

#include <QUndoStack>
#include <QWidget>

#include "animationitem.h"
#include "animationscene.h"
#include "keyframehandle.h"
#include "timeline.h"
#include "transition.h"

class TransitionLine : public QWidget
{
    Q_OBJECT
public:
    TransitionLine(AnimationItem *item, QString propertyName, Timeline *timeline, QUndoStack *undostack);

    void paintEvent(QPaintEvent *ev) Q_DECL_OVERRIDE;

    inline QString propertyName() {return m_propertyName;}
    inline AnimationItem *item() {return m_item;}
    inline void setPlayheadPosition(int value) {m_playheadPosition = value; update();}
    inline KeyFrame *selectedFrame() {return m_selectedFrame;}
    inline void deselectFrame() {m_selectedFrame = nullptr; update();}

    void setScrollValue(int value);
    KeyframeHandle *getKeyframeHandle(KeyFrame *key);
    int horizontalScrollValue() {return m_horizontalScrollValue;}
    Transition *getTransition(KeyFrame *key);

    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

    void addTransitionGui(KeyFrame *key);

signals:
    void keyframeDeleted(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void deleteTransition(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void transitionAdded(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void transitionSelected(KeyFrame *frame);

private:
    AnimationItem *m_item;
    QString m_propertyName;
    KeyFrame *m_frame;
    KeyFrame *m_selectedFrame;
    int m_oldx;
    int m_playheadPosition;
    int m_horizontalScrollValue;
    QUndoStack *m_undostack;
    Timeline *m_timeline;


public slots:
    void addKeyframe(QString propertyName, KeyFrame *key);
    void removeKeyframe(QString propertyName, KeyFrame *key);
    void removeTransition(QString propertyName, KeyFrame *key);
    void addTransition(KeyFrame *key);

private slots:
    void deleteKeyframe(KeyframeHandle *handle);
    void moveKeyframe(KeyframeHandle *handle, int dist);
    void moveTransition(Transition *transition, int dist);
    void transitionResized();
};

#endif // TRANSITIONLINE_H
