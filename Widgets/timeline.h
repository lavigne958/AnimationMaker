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

#ifndef TIMELINE_H
#define TIMELINE_H

#include <QTreeWidget>
#include <QGraphicsItem>
#include <QLabel>
#include <QToolButton>
#include <QPropertyAnimation>

#include "animationitem.h"
#include "animationscene.h"
#include "playhead.h"

class TransitionLine;
class Timeline : public QWidget
{
    Q_OBJECT
public:
    Timeline(AnimationScene *scene);
    ~Timeline();

    void reset();
    inline void setPlayheadPosition(int value) {m_playhead->setValue(value);}
    inline void expandTree() {m_tree->expandAll();}
    int lastKeyframe();
    void removeItem(AnimationItem *item);
    void selectItem(AnimationItem *item);
    void addKeyFrame(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void deleteKeyFrame(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void addTransition(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void deleteTransition(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void moveKeyframe(KeyFrame *key, int time);
    void moveTransition(KeyFrame *key, int time);
    void resizeTransition(KeyFrame *key, int startTime, int endTime);
    TransitionLine *getTransitionLine(AnimationItem *item, QString propertyName);

public slots:
    void onCustomContextMenu(const QPoint &point);
    void playAnimation();
    void pauseAnimation();
    void revertAnimation();
    void forwardAnimation();
    void playheadValueChanged(int val);
    void addKeyFrame(AnimationItem *item,QString property, QVariant value);
    void autokeyframes(bool value);
    void autotransitions(bool value);
    void idChanged(AnimationItem *item, QString value);
    void treeCurrentItemChanged(QTreeWidgetItem *currentItem, QTreeWidgetItem*);
    void keyframeAdded(AnimationItem * item, QString propertyName, KeyFrame *key);
    void deleteKeyFrameSlot(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void deleteTransitionSlot(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void addTransitionSlot(AnimationItem *item, QString propertyName, KeyFrame *frame);
    void transitionSelected(KeyFrame *frame);
    void transitionDeselected();
    void scrollValueChanged(int value);

signals:
    void itemSelectionChanged(AnimationItem *item);
    void keyframeDeleted(AnimationItem *item, QString propertyName);
    void transitionDeleted(AnimationItem *item, QString propertyName);
    void transitionSelectionChanged(KeyFrame *frame);

private:
    QScrollBar *m_sb;
    QTreeWidget *m_tree;
    QMenu *m_contextMenu;
    QAction *m_delAct;
    QAction *pauseAct;
    QAction *reverseAct;
    QAction *forwardAct;
    QAction *playAct;
    AnimationScene *m_scene;
    PlayHead *m_playhead;
    QLabel *m_time;
    bool m_playing;
    QToolButton *playButton;
    QToolButton *pauseButton;
    int m_horizontalScrollPos;

    void addProperty(const QString name);
    void createAnimationGroup();
    void addTransitionLine(QTreeWidgetItem *tvi, AnimationItem *item);
    void addProperty(QTreeWidgetItem *treeChildItem, AnimationItem *item, QString propertyName);
    void addPropertyKey(QTreeWidgetItem *treeChildItem, QString propertyName, KeyFrame *key);
    void propertyKeyRemoved(AnimationItem *item, QString propertyName, KeyFrame *key);
    void transitionResized(KeyFrame *key);
    void transitionMoved(KeyFrame *key);
    QTreeWidgetItem *search(AnimationItem *item);
    QTreeWidgetItem *search(QTreeWidgetItem *treeItem, QString propertyName);
};
QString timeString(int milliseconds, bool showMinutes = true);
#endif // TIMELINE_H
