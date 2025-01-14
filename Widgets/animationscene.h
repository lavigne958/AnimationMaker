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

#ifndef ANIMATIONSCENE_H
#define ANIMATIONSCENE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QDataStream>
#include <QGraphicsItem>
#include <QTest>
#include <QFileDialog>
#include <QUndoStack>
#include <QDomDocument>

#include "keyframe.h"

class Rectangle;
class AnimationItem;
class AnimationScene : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(int fps READ fps)
    Q_PROPERTY(int width READ width)
    Q_PROPERTY(int height READ height)

public:
    AnimationScene();

    void reset();

    enum ItemType { TypeItem, TypeRectangle, TypeEllipse, TypeText, TypeBitmap, TypeSvg };
    enum EditMode { ModeSelect, ModeRectangle, ModeEllipse, ModeText, ModeBitmap, ModeSvg, ModePlugin };

    void setEditMode(EditMode mode);
    void setEditMode(QString pluginName);
    void setFileVersion(int version) {m_fileVersion = version;}
    void addNewImage(QString filename, EditMode mode);

    int fps() const;
    inline void setFps(int value) {m_fps = value;}

    void setWidth(int value);
    void setHeight(int value);

    inline QColor backgroundColor() const {return m_backgroundColor;}
    void setBackgroundColor(QColor value);

    Rectangle *backgroundRect() {return m_rect;}

    inline int playheadPosition() {return m_playheadPosition;}

    void setCursor(const QCursor &cursor);
    void copyItem();
    void pasteItem();

    void deleteItem(AnimationItem *item);
    inline bool autokeyframes() {return m_autokeyframes;}
    inline void setAutokeyframes(bool value) {m_autokeyframes = value;}

    inline bool autotransition() {return m_autotransitions;}
    inline void setAutotransition(bool value) {m_autotransitions = value;}

    inline void registerUndoStack(QUndoStack *stack) {m_undoStack = stack;}
    inline QUndoStack *undoStack() {return m_undoStack;}

    QString actPluginName() {return m_actPluginName;}

    void exportXml(QString fileName, bool exportAll = true);
    bool importXml(QString fileName);

    int scaling() {return m_scaling;}
    void setScaling(int scaling) {m_scaling = scaling;}
        
signals:
    void itemAdded(QGraphicsItem *item);
    void sizeChanged(int width, int height);
    void keyframeAdded(AnimationItem *item, QString propertyName, KeyFrame *key);
    void itemRemoved(AnimationItem *item);
    void backgroundColorChanged(QColor color);
    void animationResetted();

public slots:
    void setPlayheadPosition(int value);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

private:
    EditMode m_editMode;
    int m_fps;
    AnimationItem *m_copy;
    QColor m_backgroundColor;
    Rectangle *m_rect;
    int m_playheadPosition;
    bool m_autokeyframes;
    bool m_autotransitions;
    QUndoStack *m_undoStack;
    AnimationItem *m_movingItem;
    QPointF m_oldPos;
    int m_fileVersion;
    QString m_actPluginName;
    QGraphicsRectItem *m_blackSelectionRect;
    QGraphicsRectItem *m_whiteSelectionRect;
    int m_scaling;

    void initialize();
    void addBackgroundRect();
    void copyKeyframes(AnimationItem *item);
    void writeKeyframes(QDomDocument *doc, QDomElement *element, AnimationItem *item);
    void readKeyframes(QDomElement *element, AnimationItem *item);
};

qreal getProgressValue(KeyFrame *found, int playheadPosition);
qreal calculateRealValue(KeyFrame *found, int playheadPosition);
QColor calculateColorValue(KeyFrame *found, int playheadPosition);
int calculateIntValue(KeyFrame *found, int playheadPosition);
QString calculateRotationValue(KeyFrame *found, int playheadPosition);

#endif // ANIMATIONSCENE_H
