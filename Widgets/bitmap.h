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

#ifndef BITMAP_H
#define BITMAP_H

#include <QGraphicsItem>

#include "animationitem.h"

class Bitmap : public AnimationItem
{
    Q_OBJECT
public:
    Bitmap();
    Bitmap(QString filename, AnimationScene *scene);
    Bitmap(QImage img, AnimationScene *scene);

    ~Bitmap();

    void paint( QPainter *paint, const QStyleOptionGraphicsItem *, QWidget *);
    QString getHtml(QString id, QString assetsPath) Q_DECL_OVERRIDE;
    QImage getImage();
    void setImage(QImage image);

    enum { Type = UserType + 4 };
    int type() const Q_DECL_OVERRIDE;
    QString typeName() {return "Bitmap";}
    QDomElement getXml(QDomDocument);
    bool hasBrushAndPen() {return false;}

private slots:
    void exchangeBitmapAction();

private:
    QImage m_image;
    QAction *exchangeBitmapAct;

    void addMenu();
};

#endif // BITMAP_H
