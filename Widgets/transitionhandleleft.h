/****************************************************************************
** Copyright (C) 2018 Olaf Japp
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

#ifndef TRANSITIONHANDLELEFT_H
#define TRANSITIONHANDLELEFT_H

#include <QWidget>
#include <QMenu>

#include "keyframe.h"
#include "transition.h"
#include "transitionhandle.h"

class TransitionHandleLeft : public TransitionHandle
{
    Q_OBJECT
public:
    TransitionHandleLeft(Transition *parent, KeyFrame *key);
    ~TransitionHandleLeft();

protected:
    void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;

signals:
    void transitionAdded(KeyFrame *key);

private slots:
    void onCustomContextMenu(const QPoint &point);
    void addTransition();

private:
    QMenu *m_contextMenu;
    QAction *m_transitionAct;
};

#endif // TRANSITIONHANDLELEFT_H
