/****************************************************************************
**  Copyright (C) 2018 Olaf Japp and CrowdWare
**
**  This file is part of AnimationMaker.
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

#include "mainwindow.h"
#include "animationscene.h"
#include "vectorgraphic.h"
#include "interfaces.h"
#include "plugins.h"
#include "bitmap.h"
#include "itempropertyeditor.h"
#include "timeline.h"
#include "scenepropertyeditor.h"
#include "sceneview.h"
#include "transitioneditor.h"
#include "expander.h"
#include "rectangle.h"
#include "ellipse.h"
#include "text.h"
#include "bitmap.h"
#include "vectorgraphic.h"
#include "keyframe.h"
#include <QMessageBox>
#include <QGraphicsSvgItem>
#include <QTreeWidget>
#include <QMainWindow>
#include <QtWidgets>
#include <QTextBrowser>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_undoStack = new QUndoStack(this);

    setDockNestingEnabled(true);
    loadPlugins();
    createStatusBar();
    createActions();
    createMenus();
    createGui();
    readSettings();
}

MainWindow::~MainWindow()
{
    delete m_transitionEditor;
    delete m_scene;
    delete m_timeline;
    delete m_elementTree;
    delete m_view;
    delete showToolPanelAct;
    delete showElementsAct;
    delete showPropertyPanelAct;
    delete showRulerAct;
    delete importMenu;
    delete m_undoStack;
}

void MainWindow::loadPlugins()
{
    QDir pluginsDir(QDir::homePath() + "/AnimationMaker/plugins");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin)
        {
            ItemInterface *item = qobject_cast<ItemInterface*>(plugin);
            if(item)
            {
                Plugins::insert(item->className(), item);
                qDebug() << "Plugin loaded" << fileName;
            }
            ExportInterface *ei = qobject_cast<ExportInterface*>(plugin);
            if(ei)
            {
                Plugins::insert(ei->className(), ei);
                qDebug() << "Plugin loaded" << fileName;
            }
        }
        else
        {
            qDebug() << "Plugin could not ne loaded" << fileName << loader.errorString();
        }
    }
}

void MainWindow::save()
{
    writeFile(loadedFile.filePath());
}

void MainWindow::saveAs()
{
    QString fileName;
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter(tr("AnimationMaker XML (*.amx);;All Files (*)"));
    dialog->setWindowTitle(tr("Save Animation"));
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setDefaultSuffix("amx");
    if(dialog->exec())
        fileName = dialog->selectedFiles().first();
    delete dialog;
    if(fileName.isEmpty())
        return;

    writeFile(fileName);
    loadedFile.setFile(fileName);
    saveAct->setEnabled(true);
    setTitle();
}

void MainWindow::saveItemAs()
{
    QString fileName;
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter(tr("AnimationMaker XML (*.amx);;All Files (*)"));
    dialog->setWindowTitle(tr("Save Animation"));
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setDefaultSuffix("amx");
    if(dialog->exec())
        fileName = dialog->selectedFiles().first();
    delete dialog;
    if(fileName.isEmpty())
        return;

    m_scene->exportXml(fileName, false);
    statusBar()->showMessage(QString("Item saved as " + fileName));
}

void MainWindow::writeFile(QString fileName)
{
    m_scene->clearSelection();
    m_timeline->setPlayheadPosition(0);

    m_scene->exportXml(fileName);
    statusBar()->showMessage(QString("File saved as " + fileName));

    m_undoStack->clear();
}

void MainWindow::setTitle()
{
    if(loadedFile.completeBaseName().isEmpty())
        setWindowTitle(QCoreApplication::applicationName());
    else
        setWindowTitle(QCoreApplication::applicationName() + " - " + loadedFile.completeBaseName() + "." + loadedFile.suffix());
}

void MainWindow::reset()
{
    setCentralWidget(m_splitter);
    m_scene->reset();
    m_timeline->reset();
}

void MainWindow::newfile()
{
    reset();
    fillTree();
    saveAct->setEnabled(false);
    loadedFile.setFile("");
    setTitle();
    m_scenePropertyEditor->setScene(m_scene);
    m_propertiesdock->setWidget(m_scenePropertyEditor);
}

void MainWindow::open()
{
    QString fileName;
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter(tr("AnimationMaker (*.amx);;All Files (*)"));
    dialog->setWindowTitle(tr("Open Animation"));
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    if(dialog->exec())
        fileName = dialog->selectedFiles().first();
    delete dialog;
    if (fileName.isEmpty())
        return;

    bool fullyLoaded = true;
    // read xml version
    fullyLoaded = m_scene->importXml(fileName);

    fillTree();
    m_elementTree->expandAll();
    m_scenePropertyEditor->setScene(m_scene);
    m_timeline->expandTree();

    if(fullyLoaded)
    {
        loadedFile.setFile(fileName);
        saveAct->setEnabled(true);
        setTitle();
    }
    m_timeline->setPlayheadPosition(0);
}

void MainWindow::fillTree()
{
    for(int i = m_root->childCount() - 1; i >= 0; i--)
    {
        QTreeWidgetItem *treeItem = m_root->child(i);
        m_root->removeChild(treeItem);
        delete treeItem;
    }

    QList<QGraphicsItem*> itemList = m_scene->items(Qt::AscendingOrder);
    foreach (QGraphicsItem *item, itemList)
    {
        AnimationItem *ri = dynamic_cast<AnimationItem*>(item);
        if(ri && !ri->isSceneRect())
        {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem();

            treeItem->setText(0, ri->id());
            treeItem->setIcon(0, QIcon(":/images/rect.png"));
            treeItem->setData(0, 3, qVariantFromValue((void *) ri));
            m_root->addChild(treeItem);
            addCheckboxes(treeItem, ri);
            connect(ri, SIGNAL(idChanged(AnimationItem *, QString)), this, SLOT(idChanged(AnimationItem *, QString)));
        }
    }
}

void MainWindow::idChanged(AnimationItem *item, QString id)
{
    for(int i=0; i < m_root->childCount(); i++)
    {
        if(m_root->child(i)->data(0, 3).value<void *>() == item)
        {
            m_root->child(i)->setText(0, id);
            break;
        }
    }
}

void MainWindow::createGui()
{
    QToolBar *toolpanel = new QToolBar();
    toolpanel->setOrientation(Qt::Vertical);
    QActionGroup *anActionGroup = new QActionGroup(toolpanel);
    m_selectAct = new QAction("Select", anActionGroup);
    m_selectAct->setIcon(QIcon(":/images/arrow.png"));
    m_selectAct->setCheckable(true);

    m_rectangleAct = new QAction("Rectangle", anActionGroup);
    m_rectangleAct->setIcon(QIcon(":/images/rectangle.png"));
    m_rectangleAct->setCheckable(true);

    m_ellipseAct = new QAction("Ellipse", anActionGroup);
    m_ellipseAct->setIcon(QIcon(":/images/ellipse.png"));
    m_ellipseAct->setCheckable(true);

    m_textAct = new QAction("Text", anActionGroup);
    m_textAct->setIcon(QIcon(":/images/text.png"));
    m_textAct->setCheckable(true);

    m_bitmapAct = new QAction("Bitmap", anActionGroup);
    m_bitmapAct->setIcon(QIcon(":/images/camera.png"));
    m_bitmapAct->setCheckable(true);

    m_svgAct = new QAction("Vectorgraphic", anActionGroup);
    m_svgAct->setIcon(QIcon(":/images/svg.png"));
    m_svgAct->setCheckable(true);

    connect(m_selectAct, SIGNAL(triggered()), this, SLOT(setSelectMode()));
    connect(m_rectangleAct, SIGNAL(triggered()), this, SLOT(setRectangleMode()));
    connect(m_ellipseAct, SIGNAL(triggered()), this, SLOT(setEllipseMode()));
    connect(m_textAct, SIGNAL(triggered()), this, SLOT(setTextMode()));
    connect(m_bitmapAct, SIGNAL(triggered()), this, SLOT(loadNewBitmap()));
    connect(m_svgAct, SIGNAL(triggered()), this, SLOT(loadNewSvg()));

    toolpanel->addAction(m_selectAct);
    toolpanel->addAction(m_rectangleAct);
    toolpanel->addAction(m_ellipseAct);
    toolpanel->addAction(m_textAct);
    toolpanel->addAction(m_bitmapAct);
    toolpanel->addAction(m_svgAct);

    // load plugins here
    foreach(QString pluginName, Plugins::itemPluginNames())
    {
        ItemInterface *plugin = Plugins::getItemPlugin(pluginName);
        QAction *act = new QAction(plugin->displayName(), anActionGroup);
        act->setData(QVariant(plugin->className()));
        act->setIcon(plugin->icon());
        act->setCheckable(true);
        connect(act, SIGNAL(triggered()), this, SLOT(setPluginMode()));
        toolpanel->addAction(act);
    }
    m_selectAct->toggle();

    m_tooldock = new QDockWidget(tr("Tools"), this);
    m_tooldock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_tooldock->setWidget(toolpanel);
    m_tooldock->setObjectName("Tools");
    addDockWidget(Qt::LeftDockWidgetArea, m_tooldock);

    m_scene = new AnimationScene();
    m_scene->registerUndoStack(m_undoStack);

    m_timeline = new Timeline(m_scene);
    m_timeline->setMinimumHeight(110);

    m_itemPropertyEditor = new ItemPropertyEditor(m_timeline);
    m_scenePropertyEditor = new ScenePropertyEditor();
    m_transitionEditor = new TransitionEditor();
    m_transitionEditor->setUndoStack(m_undoStack);

    m_scenePropertyEditor->setScene(m_scene);

    m_propertiesdock = new QDockWidget(tr("Properties"), this);
    m_propertiesdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_propertiesdock->setWidget(m_scenePropertyEditor);
    m_propertiesdock->setObjectName("Properties");

    addDockWidget(Qt::RightDockWidgetArea, m_propertiesdock);

    m_view = new SceneView(m_scene);
    m_view->setSceneRect(-100, -100, m_scene->width() + 200, m_scene->height() + 200);
    m_view->setRenderHint(QPainter::RenderHint::Antialiasing);
    connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
    connect(m_scene, SIGNAL(itemAdded(QGraphicsItem*)), this, SLOT(sceneItemAdded(QGraphicsItem*)));
    connect(m_scene, SIGNAL(sizeChanged(int,int)), this, SLOT(sceneSizeChanged(int, int)));
    connect(m_scene, SIGNAL(itemRemoved(AnimationItem*)), this, SLOT(sceneItemRemoved(AnimationItem*)));
    connect(m_scene, SIGNAL(animationResetted()), this, SLOT(reset()));

    QWidget *w = new QWidget();
    QVBoxLayout *vbox = new QVBoxLayout();
    QHBoxLayout *hbox = new QHBoxLayout();
    QComboBox *zoom = new QComboBox();
    zoom->addItem("1:2");
    zoom->addItem("1:1");
    zoom->addItem("2:1");
    zoom->addItem("4:1");
    zoom->addItem("8:1");
    zoom->setCurrentIndex(1);
    connect(zoom, SIGNAL(currentIndexChanged(int)), this, SLOT(changeZoom(int)));

    vbox->addWidget(m_view);
    vbox->addLayout(hbox);
    hbox->addWidget(zoom);
    hbox->addStretch();
    w->setLayout(vbox);

    m_elementTree = new QTreeWidget();
    m_elementTree->header()->close();
    m_elementTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_root = new QTreeWidgetItem();
    m_root->setText(0, "Scene");
    m_elementTree->setColumnCount(3);
    m_elementTree->setColumnWidth(1, 18);
    m_elementTree->setColumnWidth(2, 18);
    m_elementTree->header()->moveSection(0,2);
    m_elementTree->addTopLevelItem(m_root);
    connect(m_elementTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(elementTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    m_elementsdock = new QDockWidget(tr("Elements"), this);
    m_elementsdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_elementsdock->setWidget(m_elementTree);
    m_elementsdock->setObjectName("Elements");
    addDockWidget(Qt::LeftDockWidgetArea, m_elementsdock);
    splitDockWidget(m_tooldock, m_elementsdock, Qt::Horizontal);

    connect(m_timeline, SIGNAL(itemSelectionChanged(AnimationItem *)), this, SLOT(timelineSelectionChanged(AnimationItem*)));
    connect(m_timeline, SIGNAL(transitionSelectionChanged(KeyFrame*)), this, SLOT(transitionSelectionChanged(KeyFrame*)));
    connect(m_itemPropertyEditor, SIGNAL(addKeyFrame(AnimationItem*,QString,QVariant)), m_timeline, SLOT(addKeyFrame(AnimationItem*,QString,QVariant)));
    connect(m_scenePropertyEditor, SIGNAL(addKeyFrame(AnimationItem*,QString,QVariant)), m_timeline, SLOT(addKeyFrame(AnimationItem*,QString,QVariant)));

    m_splitter = new QSplitter(Qt::Vertical);
    m_splitter->addWidget(w);
    m_splitter->addWidget(m_timeline);

    setCentralWidget(m_splitter);
}

void MainWindow::elementTreeItemChanged(QTreeWidgetItem *newItem, QTreeWidgetItem *)
{
    m_scene->clearSelection();
    AnimationItem *item = (AnimationItem *)  newItem->data(0, 3).value<void *>();
    if(item)
    {
        item->setSelected(true);
        m_itemPropertyEditor->setItem(item);
        m_propertiesdock->setWidget(m_itemPropertyEditor);
    }
    else
    {
        m_propertiesdock->setWidget(m_scenePropertyEditor);
    }
}

void MainWindow::sceneSizeChanged(int width, int height)
{
    m_view->setSceneRect(-100, -100, width + 200, height + 200);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("rulers", showRulerAct->isChecked());
}

void MainWindow::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,  QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
    }
    else
    {
        restoreGeometry(geometry);
        restoreState(settings.value("state").toByteArray());
    }
    bool showRulers = settings.value("rulers", "true").toBool();
    if(!showRulers)
    {
        showRulerAct->setChecked(false);
        m_view->showRulers(false);
    }
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    newAct = new QAction(tr("&New"), this);
    newAct->setShortcut(QKeySequence::New);
    connect(newAct, SIGNAL(triggered()), this, SLOT(newfile()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setEnabled(false);
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    saveItemAsAct = new QAction(tr("Save &Item as..."), this);
    saveItemAsAct->setEnabled(false);
    connect(saveItemAsAct, SIGNAL(triggered()), this, SLOT(saveItemAs()));

    exportMovieAct = new QAction(tr("Export Movie"), this);
    connect(exportMovieAct, SIGNAL(triggered()), this, SLOT(exportMovie()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    undoAct = m_undoStack->createUndoAction(this, tr("&Undo"));
    undoAct->setShortcuts(QKeySequence::Undo);

    redoAct = m_undoStack->createRedoAction(this, tr("&Redo"));
    redoAct->setShortcuts(QKeySequence::Redo);

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    delAct = new QAction(tr("&Delete"), this);
    delAct->setShortcut(QKeySequence::Delete);
    connect(delAct, SIGNAL(triggered()), this, SLOT(del()));

    showElementsAct = new QAction("Elements");
    connect(showElementsAct, SIGNAL(triggered()), this, SLOT(showElementsPanel()));

    showPropertyPanelAct = new QAction("Properties");
    connect(showPropertyPanelAct, SIGNAL(triggered()), this, SLOT(showPropertyPanel()));

    showToolPanelAct = new QAction("Tools");
    connect(showToolPanelAct, SIGNAL(triggered()), this, SLOT(showToolPanel()));

    showRulerAct = new QAction("Rulers");
    showRulerAct->setCheckable(true);
    showRulerAct->setChecked(true);
    connect(showRulerAct, SIGNAL(triggered()), this, SLOT(showRuler()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    addAction(copyAct);
    addAction(pasteAct);
    addAction(exitAct);
    addAction(delAct);
}

void MainWindow::createMenus()
{
    importMenu = new QMenu(tr("Import"));
    importMenu->setEnabled(false);

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(saveItemAsAct);
    fileMenu->addSeparator();
    exportMenu = fileMenu->addMenu("Export");
    exportMenu->addAction(exportMovieAct);
    foreach(QString pluginName, Plugins::exportPluginNames())
    {
        ExportInterface *ei = Plugins::getExportPlugin(pluginName);
        QAction *exportAct = new QAction(ei->displayName(), ei);
        connect(exportAct, SIGNAL(triggered()), this, SLOT(pluginExport()));
        exportMenu->addAction(exportAct);
    }
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(delAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(showToolPanelAct);
    viewMenu->addAction(showElementsAct);
    viewMenu->addAction(showPropertyPanelAct);
    viewMenu->addAction(showRulerAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    this->addAction(undoAct);
    this->addAction(redoAct);
    this->addAction(copyAct);
    this->addAction(pasteAct);
}

void MainWindow::about()
{
    QMessageBox msg;
    msg.setWindowTitle("About AnimationMaker");
    msg.setText("AnimationMaker\nVersion: " + QCoreApplication::applicationVersion() + "\n(C) Copyright 2018 Olaf Japp. All rights reserved.\n\nThe program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.");
    msg.setIconPixmap(QPixmap(":/images/logo.png"));
    msg.exec();
}

void MainWindow::setSelectMode()
{
    m_scene->setEditMode(AnimationScene::EditMode::ModeSelect);
    m_scene->setCursor(Qt::ArrowCursor);
}

void MainWindow::setRectangleMode()
{
    m_scene->clearSelection();
    m_scene->setCursor(QCursor(QPixmap::fromImage(QImage(":/images/rect_cursor.png"))));
    m_scene->setEditMode(AnimationScene::EditMode::ModeRectangle);
}

void MainWindow::setEllipseMode()
{
    m_scene->clearSelection();
    m_scene->setCursor(QCursor(QPixmap::fromImage(QImage(":/images/ellipse_cursor.png"))));
    m_scene->setEditMode(AnimationScene::EditMode::ModeEllipse);
}

void MainWindow::setTextMode()
{
    m_scene->clearSelection();
    m_scene->setCursor(QCursor(QPixmap::fromImage(QImage(":/images/text_cursor.png"))));
    m_scene->setEditMode(AnimationScene::EditMode::ModeText);
}

void MainWindow::addNewImage(AnimationScene::EditMode mode)
{
    QString fileName;
    QString filter;
    QString title;
    if(mode == AnimationScene::EditMode::ModeBitmap)
    {
        QList<QByteArray> formatList = QImageReader::supportedImageFormats();
        filter = "Image Files (";

        for (auto format: formatList) {
            // we have a dedicated button for svg images
            if (format.contains("svg") == 0) {
                filter += " *." + format;
            }
        }

        filter += ")";
        title = "Open Bitmap";
    }
    else if(mode == AnimationScene::EditMode::ModeSvg)
    {
        filter = "SVG Files (*.svg)";
        title = "Open SVG";
    }

    if(!filter.isEmpty())
    {
        QFileDialog *dialog = new QFileDialog(this, Qt::Dialog);
        dialog->setFileMode(QFileDialog::ExistingFile);
        dialog->setNameFilter(filter);
        dialog->setWindowTitle(title);
        dialog->setAcceptMode(QFileDialog::AcceptOpen);
        if(dialog->exec())
            fileName = dialog->selectedFiles().first();
        delete dialog;
        if(fileName.isEmpty())
            return;
        this->m_scene->addNewImage(fileName, mode);
    }
}

void MainWindow::loadNewBitmap()
{
    addNewImage(AnimationScene::ModeBitmap);
}

void MainWindow::loadNewSvg()
{
    addNewImage(AnimationScene::ModeSvg);
}

void MainWindow::setPluginMode()
{
    QAction *act = qobject_cast<QAction*>(sender());
    if(act)
    {
        QString pluginName = act->data().toString();
        ItemInterface *item = Plugins::getItemPlugin(pluginName);
        m_scene->clearSelection();
        m_scene->setCursor(item->getCursor());
        m_scene->setEditMode(item->className());
    }
}

void MainWindow::sceneSelectionChanged()
{
    AnimationItem *item = NULL;

    if(m_scene->selectedItems().count())
    {
        item = dynamic_cast<AnimationItem*>(m_scene->selectedItems().first());
        saveItemAsAct->setEnabled(true);
    }
    else
        saveItemAsAct->setEnabled(false);

    if(item)
    {
        m_itemPropertyEditor->setItem(item);
        m_propertiesdock->setWidget(m_itemPropertyEditor);
        for(int i=0; i<m_root->childCount(); i++)
        {
            QTreeWidgetItem *treeItem = m_root->child(i);
            if(treeItem->data(0, 3).value<void *>() == item)
                treeItem->setSelected(true);
            else
                treeItem->setSelected(false);
        }
        m_root->setSelected(false);

        m_timeline->selectItem(item);
    }
    else
    {
        m_propertiesdock->setWidget(m_scenePropertyEditor);
        for(int i=0; i<m_root->childCount(); i++)
            m_root->child(i)->setSelected(false);
        m_root->setSelected(true);
        m_timeline->selectItem(nullptr);
    }
}

void MainWindow::timelineSelectionChanged(AnimationItem* item)
{
    m_scene->clearSelection();
    item->setSelected(true);

    m_itemPropertyEditor->setItem(item);
    m_propertiesdock->setWidget(m_itemPropertyEditor);
}

void MainWindow::addCheckboxes(QTreeWidgetItem *treeItem, AnimationItem *item)
{
    QCheckBox *elementVisible = new QCheckBox();
    elementVisible->setProperty("item", qVariantFromValue((void *)item));
    elementVisible->setFixedWidth(18);
    QCheckBox *elementLocked = new QCheckBox();
    elementLocked->setProperty("item", qVariantFromValue((void *)item));
    elementLocked->setFixedWidth(18);
    m_elementTree->setItemWidget(treeItem, 1, elementVisible);
    m_elementTree->setItemWidget(treeItem, 2, elementLocked);
    elementVisible->setStyleSheet("QCheckBox {spacing: 3px;} QCheckBox::indicator{width: 18px; height: 9px;} QCheckBox::indicator:unchecked {image: url(:/images/eye_unchecked.png);} QCheckBox::indicator:checked {image: url(:/images/eye_checked.png);}");
    elementLocked->setStyleSheet("QCheckBox {spacing: 3px;} QCheckBox::indicator{width: 18px; height: 9px;} QCheckBox::indicator:unchecked {image: url(:/images/lock_unchecked.png);} QCheckBox::indicator:checked {image: url(:/images/lock_checked.png);}");
    elementVisible->setToolTip("Visibility");
    elementLocked->setToolTip("Lock");
    elementVisible->setChecked(!item->isVisible());
    elementLocked->setChecked(!item->flags().testFlag(QGraphicsItem::ItemIsSelectable));
    connect(elementVisible, SIGNAL(stateChanged(int)), this, SLOT(elementVisibleStateChanged(int)));
    connect(elementLocked, SIGNAL(stateChanged(int)), this, SLOT(elementLockStateChanged(int)));
}

void MainWindow::sceneItemAdded(QGraphicsItem *item)
{
    AnimationItem *ri = dynamic_cast<AnimationItem*>(item);
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, ri->id());
    treeItem->setIcon(0, QIcon(":/images/rect.png"));
    treeItem->setData(0, 3, qVariantFromValue((void *) ri));
    connect(ri, SIGNAL(idChanged(AnimationItem *, QString)), this, SLOT(idChanged(AnimationItem *, QString)));

    m_root->addChild(treeItem);
    m_root->setExpanded(true);
    m_root->setSelected(false);

    addCheckboxes(treeItem, ri);

    for(int i=0; i<m_root->childCount(); i++)
        m_root->child(i)->setSelected(false);
    treeItem->setSelected(true);

    item->setSelected(true);
    m_selectAct->setChecked(true);
    m_scene->setEditMode(AnimationScene::EditMode::ModeSelect);
    m_scene->setCursor(Qt::ArrowCursor);
}

void MainWindow::showPropertyPanel()
{
    m_propertiesdock->setVisible(true);
}

void MainWindow::showToolPanel()
{
    m_tooldock->setVisible(true);
}

void MainWindow::showElementsPanel()
{
    m_elementsdock->setVisible(true);
}

void MainWindow::showRuler()
{
    m_view->showRulers(showRulerAct->isChecked());
}

void MainWindow::copy()
{
    m_scene->copyItem();
}

void MainWindow::paste()
{
    m_scene->pasteItem();
}

void MainWindow::del()
{
    while(m_scene->selectedItems().count())
    {
        AnimationItem *item = dynamic_cast<AnimationItem*>(m_scene->selectedItems().first());
        if(item)
            m_scene->deleteItem(item);
    }
}

void MainWindow::sceneItemRemoved(AnimationItem *item)
{
    for(int i=0; i<m_root->childCount(); i++)
    {
        if(m_root->child(i)->data(0, 3).value<void *>() == item)
        {
            QTreeWidgetItem *treeItem = m_root->child(i);
            m_root->removeChild(treeItem);
            delete treeItem;
            break;
        }
    }
    m_timeline->removeItem(item);
}

void MainWindow::transitionSelectionChanged(KeyFrame *frame)
{
    if(frame)
    {
        m_scene->clearSelection();
        m_transitionEditor->setKeyframe(frame);
        m_propertiesdock->setWidget(m_transitionEditor);
    }
    else
        m_propertiesdock->setWidget(m_scenePropertyEditor);
}

void MainWindow::changeZoom(int zoom)
{
    m_view->resetMatrix();
    m_scene->setScaling(zoom);
    QList<QGraphicsItem*> list = m_scene->selectedItems();
    foreach(QGraphicsItem* item,list)
    {
        item->setSelected(false);
    }
    switch(zoom)
    {
    case 0:
        m_view->scale(0.5, 0.5);
        break;
    case 1:
        m_view->scale(1.,1.);
        break;
    case 2:
        m_view->scale(2.,2.);
        break;
    case 3:
        m_view->scale(4.,4.);
        break;
    case 4:
        m_view->scale(8.,8.);
        break;
    }
    foreach(QGraphicsItem* item,list)
    {
        item->setSelected(true);
    }
}

void MainWindow::elementVisibleStateChanged(int state)
{
    AnimationItem *item = (AnimationItem *)sender()->property("item").value<void *>();
    item->setVisible(state == 0);
}

void MainWindow::elementLockStateChanged(int state)
{
    m_scene->clearSelection();
    AnimationItem *item = (AnimationItem *)sender()->property("item").value<void *>();
    item->setFlag(QGraphicsItem::ItemIsMovable, state == 0);
    item->setFlag(QGraphicsItem::ItemIsSelectable, state == 0);
}

void MainWindow::exportMovie()
{
    QString fileName;
    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter("Video format (*.mpg *.mp4 *.avi *.gif);;All Files (*)");
    dialog->setWindowTitle("Export Animation to Movie");
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    if(dialog->exec())
        fileName = dialog->selectedFiles().first();
    delete dialog;
    if(fileName.isEmpty())
        return;

    m_scene->clearSelection();
    m_view->setUpdatesEnabled(false);
    QGraphicsView *exportView = new QGraphicsView(m_scene);
    exportView->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    exportView->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    exportView->setGeometry(0, 0, m_scene->width(), m_scene->height());

    int delay = 1000 / m_scene->fps();
    int frames = m_timeline->lastKeyframe() / delay + 2;

    QDir tmp = QDir::temp();
    tmp.mkdir("animationmaker");
    QString filterString = "frame%03d.png";
    tmp.cd("animationmaker");

    QFile list(tmp.absolutePath() + "/list");
    if(!list.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Error", "Unable to create list file");
        return;
    }

    for (int i = 0; i < frames; i++)
    {
        statusBar()->showMessage(QString("Writing frame %1 of %2 frames").arg(i).arg(frames));

        m_timeline->setPlayheadPosition(i * delay);

        this->thread()->msleep(delay);
        QCoreApplication::processEvents(QEventLoop::AllEvents, delay);

        QImage img = exportView->grab().toImage();
        QString imgName = tmp.absolutePath() + "/" + QString::asprintf(filterString.toLatin1(), i);
        img.save(imgName);
        QString entry = "file '" + QString::asprintf(filterString.toLatin1(), i) + "'\n";
        list.write(entry.toLatin1());
    }
    list.close();
    statusBar()->showMessage(QString("Creating movie file"));


    if(fileName.endsWith(".gif"))
    {
        QString output = tmp.absolutePath() + "/temp.mp4";
        QStringList args;
        statusBar()->showMessage("Creating temp movie");
        args << "-r" << QString::number(m_scene->fps()) << "-safe" << "0" << "-f" << "concat" << "-i" << "list" << "-b:v" << "4m" << "-y" << output;
        runCommand(FFMPEG, args, tmp.absolutePath());

        statusBar()->showMessage("Creating palette file");
        args.clear();
        args << "-i" << output << "-vf" << "palettegen" << "-y" << tmp.absolutePath() + "/temp.png";
        runCommand(FFMPEG, args, tmp.absolutePath());

        statusBar()->showMessage("Converting temp movie");
        args.clear();
        args << "-r" << QString::number(m_scene->fps()) << "-i" << output << "-i" << tmp.absolutePath() + "/temp.png" << "-lavfi" << "paleteuse" << "-y" << fileName;
        runCommand(FFMPEG, args, tmp.absolutePath());
    }
    else
    {
        statusBar()->showMessage("Creating movie file");
        QStringList args;
        args << "-r" << QString::number(m_scene->fps()) << "-safe" << "0" << "-f" << "concat" << "-i" << "list" << "-b:v" << "4M" << "-y" << fileName;
        qDebug() << "Run command:\n" << FFMPEG << args.join(" ");
        runCommand(FFMPEG, args, tmp.absolutePath());
    }

    tmp.removeRecursively();
    m_view->setUpdatesEnabled(true);
    statusBar()->showMessage("Ready");
    delete exportView;
}

void MainWindow::pluginExport()
{
    QAction *act = qobject_cast<QAction*>(sender());
    if(act)
    {
        ExportInterface *ei = qobject_cast<ExportInterface *>(act->parent());
        ei->exportAnimation(m_scene, statusBar());
    }
}

void MainWindow::runCommand(QString command, QStringList args, QString path)
{
    QProcess *proc = new QProcess();

    // check first if 'command' is installed on the system and accessible via PATH
    proc->setProgram(command);
    proc->setArguments(QStringList("-version"));
    proc->start();
    proc->waitForFinished(1000);
    if (proc->exitStatus() != QProcess::NormalExit) {
        statusBar()->showMessage("missing program: \"" + command + "\", please install it first");
        return;
    }

    proc->setWorkingDirectory(path);
    proc->start(command, args);
    proc->waitForFinished(-1);
    qDebug() << proc->readAllStandardOutput();
    qDebug() << proc->readAllStandardError();
    delete proc;
}
