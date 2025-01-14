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

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QGraphicsItem>
#include <QObject>

#include "animationitem.h"
#include "animationscene.h"

class Rectangle : public AnimationItem
{
    Q_OBJECT
public:
    Rectangle(AnimationScene *scene, bool isSceneRect = false);

    void paint( QPainter *paint, const QStyleOptionGraphicsItem *, QWidget *);
    QString getHtml(QString id, QString assetsPath) Q_DECL_OVERRIDE;
    enum { Type = UserType + 1 };
    int type() const Q_DECL_OVERRIDE;
    QString typeName() {return "Rectangle";}
    QDomElement getXml(QDomDocument);
    bool hasBrushAndPen() {return true;}
};

#endif // RECTANGLE_H
