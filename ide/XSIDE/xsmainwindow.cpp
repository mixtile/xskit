#include "xsmainwindow.h"
#include "ui_xsmainwindow.h"

#include "xsdialog.h"
#include "xsfilesavedialog.h"
#include "xsfinddialog.h"
#include "xsappbuilder.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QMouseEvent>
#include <QClipboard>
#include <QSettings>
#include <QShortcut>

using namespace Novile;

XSMainWindow::XSMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::XSMainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("XSEditor"));
    ui->centralWidget->setLayout(ui->v_main_layout);
    QHBoxLayout *top_layout = new QHBoxLayout;
    top_layout->addWidget(ui->btn_widget, 0, Qt::AlignCenter);
    ui->frame_top->setLayout(top_layout);

    project_view = new XSTreeView;
    project_view->setVisible(false);
    ui->splitter->insertWidget(0, project_view);
    ui->action_delete_project->setEnabled(false);

    findDialog = new XSFindDialog(this);
    goToDialog = new XSGoToLineDialog(this);

    projectMenu = new QMenu(this);
    act_new_project = new QAction(tr("New Project"), this);
    act_open_project = new QAction(tr("Open Project"), this);
    QAction *myproject = new QAction(tr("Recent Project"), this);
    myproject->setEnabled(false);
    projectMenu->addAction(act_new_project);
    projectMenu->addAction(act_open_project);
    projectMenu->addAction(myproject);
    ui->btn_open_project->setMenu(projectMenu);

    editor = NULL;
    isFirstOpen = true;
    isCurrentFileChanged = false;
    isEditorFocused = false;
    currentUndoStatus = 0;

    ui->widget_file->setCurrentIndex(0);
    ui->widget_file->setTabsClosable(true);
    ui->widget_file->removeTab(1);
    ui->widget_file->removeTab(0);

    setButtonImage();
    this->readSettings();
    this->setFont(QFont("monospace"));

    connect(ui->action_open_project, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(ui->action_new_project, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(ui->action_delete_project, SIGNAL(triggered()), this, SLOT(deleteProject()));
    connect(project_view, SIGNAL(openFile(QFileInfo)), this, SLOT(showFile(QFileInfo)));
    connect(ui->action_new, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->action_save, SIGNAL(triggered()), this, SLOT(saveCurrentFile()));
    connect(ui->action_save_as, SIGNAL(triggered()), this, SLOT(saveCurrentFileAs()));
    connect(ui->widget_file, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabPage(int)));
    connect(ui->widget_file, SIGNAL(currentChanged(int)), this, SLOT(switchTabPage(int)));
    connect(project_view, SIGNAL(removeTabPage(QString)), this, SLOT(removeTabPage(QString)));
    connect(ui->action_copy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(ui->action_cut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(ui->action_paste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(ui->action_undo, SIGNAL(triggered()), this, SLOT(undo()));
    connect(ui->action_redo, SIGNAL(triggered()), this, SLOT(redo()));
    connect(ui->action_goto, SIGNAL(triggered()), this, SLOT(openGoToLineWindow()));
    connect(projectMenu, SIGNAL(triggered(QAction*)), this, SLOT(projectOpenMenu(QAction*)));
    connect(project_view,SIGNAL(updateTabName(QString,QString,QString)), this, SLOT(updateTabName(QString,QString,QString)));
}

void XSMainWindow::setButtonImage()
{
    QPixmap *px_start = new QPixmap(50, 50);
    px_start->load(":xs_icon/images/start.jpeg");
    QIcon *icon_start = new QIcon(*px_start);
    ui->btn_start->setIcon(*icon_start);
    ui->btn_start->setIconSize(QSize(50, 50));
    ui->btn_start->setFixedSize(QSize(50, 50));

    QPixmap *px_end = new QPixmap(50, 50);
    px_end->load(":xs_icon/images/stop.jpeg");
    QIcon *icon_end = new QIcon(*px_end);
    ui->btn_end->setIcon(*icon_end);
    ui->btn_end->setIconSize(QSize(50, 50));
    ui->btn_end->setFixedSize(QSize(50, 50));

    QPixmap *px_package = new QPixmap(50, 50);
    px_package->load(":xs_icon/images/package.jpeg");
    QIcon *icon_package = new QIcon(*px_package);
    ui->btn_package->setIcon(*icon_package);
    ui->btn_package->setIconSize(QSize(50, 50));
    ui->btn_package->setFixedSize(QSize(50, 50));

//    QPixmap *px_open_project = new QPixmap(100, 30);
//    px_open_project->load(":xs_icon/images/open.png");
//    QIcon *icon_open = new QIcon(*px_open_project);
//    ui->btn_open_project->setIcon(*icon_open);
//    ui->btn_open_project->setIconSize(QSize(100, 30));
    ui->btn_open_project->setFixedSize(QSize(100, 30));
    ui->btn_open_project->setText("Project");
}

void XSMainWindow::openNewFile(const QString &fileName)
{
    Editor *newEditor = new Editor(this);
    newEditor->setActiveLineHighlighted(true);
    newEditor->setTheme(Editor::ThemeTextmate);
    newEditor->setHighlightMode(Editor::ModeJavaScript);
    newEditor->hidePrintMargin();
    newEditor->setFontSize(fontSize);
    editors.push_back(newEditor);
    editor = newEditor;
    QHBoxLayout *tab_layout = new QHBoxLayout;
    tab_layout->addWidget(editor);
    QWidget *tab_widget = new QWidget;
    ui->widget_file->addTab(tab_widget, fileName);
    ui->widget_file->setCurrentWidget(tab_widget);
    ui->widget_file->currentWidget()->setLayout(tab_layout);
    connect(editor, SIGNAL(textChanged()), this, SLOT(scratchFile()));
    connect(editor, SIGNAL(cutEnabled(bool)), this, SLOT(setCutEnabled(bool)));
    connect(editor, SIGNAL(pasteEnabled(bool)), this, SLOT(setPasteEnabled(bool)));
    connect(editor, SIGNAL(postUndoStatus(bool)), this, SLOT(updateUndoStatus(bool)));
    connect(editor, SIGNAL(updteFontSize(bool)), this, SLOT(updateFontSize(bool)));

    QShortcut *shortcutFind = new QShortcut(QKeySequence("Ctrl+F"), editor);
    connect(shortcutFind, SIGNAL(activated()), this, SLOT(openFindWindow()));
}

bool XSMainWindow::checkOpenExistence(const QString &filePath)
{
    for(int i = 0; i< filePaths.size(); i++)
    {
        if(filePaths.at(i) == filePath)
        {
            ui->widget_file->setCurrentIndex(i);
            return true;
        }
    }

    return false;
}

void XSMainWindow::readSettings()
{
    QSettings *settings = new QSettings("./XSEditor.ini", QSettings::IniFormat);
    windPosition = settings->value("window/pos", QVariant(QPoint(200,200))).toPoint();
    windowSize = settings->value("window/size", QVariant(QSize(1024,768))).toSize();
    bool isMaximized = settings->value("window/isMaximized", QVariant(false)).toBool();
    fontSize = settings->value("window/fontSize", QVariant(12)).toInt();
    this->resize(windowSize);
    this->move(windPosition);
    if(isMaximized)
    {
        this->showMaximized();
    }
    settings->beginGroup("recentProjects");
    QStringList myProjectNames = settings->childKeys();
    for(int i = 0 ; i < myProjectNames.size(); i++)
    {
        QString myProjectPath =settings->value(myProjectNames.at(i), QVariant(QString("Recent Project %1").arg(i))).toString();
        projectNames.append(myProjectNames.at(i));
        projectPaths.append(myProjectPath);
        QAction *act = new QAction(myProjectNames.at(i), this);
        act->setToolTip(myProjectPath);
        projectMenu->addAction(act);
    }
    settings->endGroup();
}

void XSMainWindow::writeSettings()
{
    QSettings *settings = new QSettings("./XSEditor.ini", QSettings::IniFormat);
    settings->setValue("window/pos", QVariant(windPosition));
    settings->setValue("window/size", QVariant(windowSize));
    settings->setValue("window/isMaximized", this->isMaximized());
    settings->setValue("window/fontSize", fontSize);

    settings->remove("recentProjects");
    settings->beginGroup("recentProjects");
    for(int i = 0; i < projectNames.size(); i++)
    {
        settings->setValue(projectNames.at(i), projectPaths.at(i));
    }
    settings->endGroup();
}

bool XSMainWindow::checkFileChanges()
{
    QVector<int> changedFileIndexs;
    QStringList saveFileNames, saveFilePaths;
    for(int i = 0; i < fileChangeStatus.size(); i++)
    {
        if(fileChangeStatus.at(i))
        {
            changedFileIndexs.append(i);
            saveFileNames.append(ui->widget_file->tabText(i).replace(0, 1, ""));
            saveFilePaths.append(filePaths.at(i));
        }
    }

    if(changedFileIndexs.size() == 1)
    {
        ui->widget_file->setCurrentIndex(changedFileIndexs.first());
        QMessageBox saveMsg;
        QString msg = "'" + ui->widget_file->tabText(changedFileIndexs.first()).replace(0, 1, "") + "' has been modified. Save changes?";
        saveMsg.setText(msg);
        saveMsg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        saveMsg.setDefaultButton(QMessageBox::Yes);
        int ret = saveMsg.exec();
        if(ret == QMessageBox::Yes)
        {
            saveCurrentFile();
        }
        else if(ret == QMessageBox::No)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(changedFileIndexs.size() > 1)
    {
        XSFileSaveDialog *dialog = new XSFileSaveDialog;
        dialog->updateList(saveFileNames, saveFilePaths);
        int ret = dialog->exec();
        if(ret == QDialog::Accepted)
        {
            QVector<int> indexes = dialog->getSaveFileIndexes();
            for(int i = 0; i < indexes.size(); i++)
            {
                for(int j = 0; j < filePaths.size(); j++)
                {
                    if(saveFilePaths.at(i) == filePaths.at(j))
                    {
                        ui->widget_file->setCurrentIndex(j);
                        saveCurrentFile();
                    }
                }
            }
        }
        else
        {
            return false;
        }
    }


    return true;
}

bool XSMainWindow::maybeSave()
{
    QMessageBox closeMsg;
    QString msg = "' Are you sure to exit XSEditor?";
    closeMsg.setText(msg);
    closeMsg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    closeMsg.setDefaultButton(QMessageBox::Cancel);
    int ret = closeMsg.exec();

    if(ret == QMessageBox::Yes)
    {
        return checkFileChanges();
    }
    else
    {
        return false;
    }
}

void XSMainWindow::addMyproject(const QString &projectName)
{
    for(int i = 0; i < projectPaths.size(); i++)
    {
        if(projectPaths.at(i) == currentRootPath)
        {
            return;
        }
    }
    QAction *act = new QAction(projectName, this);
    act->setToolTip(currentRootPath);
    projectMenu->addAction(act);
    projectPaths.append(currentRootPath);
    projectNames.append(projectName);
}

void XSMainWindow::openSelectedProject(QAction *act)
{
    if(!act)
    {
        return;
    }

    openProjectFromPath(act->toolTip());
}

XSMainWindow::~XSMainWindow()
{
    delete ui;
}

void XSMainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
    {
        this->writeSettings();
        this->activateWindow();
        findDialog->writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void XSMainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if(!this->isMaximized())
    {
        windowSize = this->size();
        windPosition = this->pos();
    }
}

void XSMainWindow::moveEvent(QMoveEvent *ev)
{
    Q_UNUSED(ev);
    if(!this->isMaximized())
    {
        windowSize = this->size();
        windPosition = this->pos();
    }
}

void XSMainWindow::projectOpenMenu(QAction *action)
{
    if(action == act_new_project)
    {
        this->newProject();
    }
    else if(action == act_open_project)
    {
        QString path = QFileDialog::getExistingDirectory(this, tr("Open Project"), ".");
        openProjectFromPath(path);
    }
    else
    {
        openSelectedProject(action);
    }
}

void XSMainWindow::openProject()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Project"), ".");
    openProjectFromPath(path);
}

void XSMainWindow::openProjectFromPath(const QString path)
{
    if(path.isEmpty())
    {
        return;
    }
    else
    {
        if(currentRootPath == path)
        {
            return;
        }
        else
        {
            if(!checkFileChanges())
            {
                return;
            }
            ui->widget_file->clear();
        }
        filePaths.clear();
        editors.clear();
        fileChangeStatus.clear();
        undoStatus.clear();
        tmpUndoStatus.clear();
        savedUndoStatus.clear();
        ui->action_new->setEnabled(true);
        project_view->showProject(path);
        project_view->setVisible(true);
        ui->widget_file->setVisible(true);
        ui->action_delete_project->setEnabled(true);
    }
    currentRootPath = path;
    QString projectName = project_view->getProjectName();
    this->addMyproject(projectName);
}

void XSMainWindow::newProject()
{
    XSDialog *dia = new XSDialog();
    dia->exec();
    if(dia->result() == QDialog::Accepted)
    {
        ui->action_new->setEnabled(true);
        QString projectPath = dia->createProjectByModel();

        if(!projectPath.isEmpty())
        {
            openProjectFromPath(projectPath);
        }
        else
        {
            newProject();
        }
    }
}

void XSMainWindow::deleteProject()
{
    ui->action_new->setEnabled(false);
    ui->action_save->setEnabled(false);
    ui->action_save_as->setEnabled(false);
    ui->action_copy->setEnabled(false);
    ui->action_cut->setEnabled(false);
    ui->action_paste->setEnabled(false);
    ui->action_redo->setEnabled(false);
    ui->action_undo->setEnabled(false);
    project_view->setVisible(false);
    editor = NULL;
    ui->widget_file->clear();
    filePaths.clear();
    editors.clear();
    fileChangeStatus.clear();
    undoStatus.clear();
    tmpUndoStatus.clear();
    savedUndoStatus.clear();
    for(int i = 0; i < projectPaths.size(); i++)
    {
        if(projectPaths.at(i) == currentRootPath)
        {
            projectPaths.removeAt(i);
            projectNames.removeAt(i);
        }
    }
    foreach (QAction *act, projectMenu->actions()) {
       if(act->text() == project_view->getProjectName())
       {
           projectMenu->removeAction(act);
       }
    }
    currentRootPath.clear();
    currentFilePath.clear();
    currentUndoStatus = 0;
}

void XSMainWindow::newFile()
{
    project_view->addNewFile();
}

void XSMainWindow::showFile(QFileInfo info)
{
    QString filePath = info.filePath();

    if(checkOpenExistence(filePath))
    {
        return;
    }
    else
    {
        currentFilePath = filePath;
        filePaths.push_back(filePath);
        fileChangeStatus.push_back(false);
        undoStatus.push_back(0);
        tmpUndoStatus.push_back(0);
        savedUndoStatus.push_back(false);
        openNewFile(info.fileName());
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly))
        {
            QString text = file.readAll();
            editor->setText(text);
            editor->setCursorPosition(0, 0);
            file.close();
            if(text.isEmpty())
            {
                isFirstOpen = false;
            }
            else
            {
                isFirstOpen = true;
            }
        }
    }
}

void XSMainWindow::scratchFile()
{
    if(isFirstOpen)
    {
        isFirstOpen = false;
        isCurrentFileChanged = false;
        currentUndoStatus = 0;
        return;
    }

    ui->action_undo->setEnabled(editor->undoStatus());
    ui->action_redo->setEnabled(editor->redoStatus());

    if(currentUndoStatus != 0 && !editor->undoStatus() && isCurrentFileChanged && !savedUndoStatus.at(ui->widget_file->currentIndex()))
    {
        isCurrentFileChanged = false;
        ui->widget_file->setTabText(ui->widget_file->currentIndex(), ui->widget_file->tabText(ui->widget_file->currentIndex()).replace(0, 1, ""));
        fileChangeStatus.replace(ui->widget_file->currentIndex(), isCurrentFileChanged);
        ui->action_save->setEnabled(false);
        ui->action_save_as->setEnabled(false);
        return;
    }

    if(currentUndoStatus == undoStatus.at(ui->widget_file->currentIndex()) && isCurrentFileChanged && savedUndoStatus.at(ui->widget_file->currentIndex()))
    {
        isCurrentFileChanged = false;
        ui->widget_file->setTabText(ui->widget_file->currentIndex(), ui->widget_file->tabText(ui->widget_file->currentIndex()).replace(0, 1, ""));
        fileChangeStatus.replace(ui->widget_file->currentIndex(), isCurrentFileChanged);
        ui->action_save->setEnabled(false);
        ui->action_save_as->setEnabled(false);
        return;
    }

    if(isCurrentFileChanged == false)
    {
        QString newName = "*" + ui->widget_file->tabText(ui->widget_file->currentIndex());
        ui->widget_file->setTabText(ui->widget_file->currentIndex(), newName);
        isCurrentFileChanged = true;
        fileChangeStatus.replace(ui->widget_file->currentIndex(), isCurrentFileChanged);
        ui->action_save->setEnabled(true);
        ui->action_save_as->setEnabled(true);
    }
}

void XSMainWindow::closeTabPage(int index)
{
    if(index < 0)
    {
        return;
    }
    if(isCurrentFileChanged)
    {
        QMessageBox closeMsg;
        QString msg = "'" + ui->widget_file->tabText(index).replace(0, 1, "") + "' has been modified. Save changes?";
        closeMsg.setText(msg);
        closeMsg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        closeMsg.setDefaultButton(QMessageBox::Yes);
        int ret = closeMsg.exec();
        if(ret == QMessageBox::Yes)
        {
            saveCurrentFile();
        }
        else if(ret == QMessageBox::No)
        {

        }
        else
        {
            return;
        }
    }
    ui->widget_file->removeTab(index);
    filePaths.removeAt(index);
    fileChangeStatus.remove(index);
    editors.remove(index);
    undoStatus.remove(index);
    tmpUndoStatus.remove(index);
    savedUndoStatus.remove(index);
}

void XSMainWindow::switchTabPage(int index)
{
    if(index < 0)
    {
        ui->action_save->setEnabled(false);
        ui->action_save_as->setEnabled(false);
        findDialog->setEditor(NULL);
        goToDialog->setEditor(NULL);
        return;
    }

    isCurrentFileChanged = fileChangeStatus.at(index);
    currentUndoStatus = tmpUndoStatus.at(index);
    currentFilePath = filePaths.at(index);
    editor = editors.at(index);
    ui->action_save->setEnabled(isCurrentFileChanged);
    findDialog->setEditor(editor);
    goToDialog->setEditor(editor);
}

void XSMainWindow::removeTabPage(const QString &filePath)
{
    for(int i = 0; i< filePaths.size(); i++)
    {
        if(filePaths.at(i) == filePath)
        {
            ui->widget_file->removeTab(i);
            filePaths.removeAt(i);
            fileChangeStatus.remove(i);
            editors.remove(i);
            undoStatus.remove(i);
            tmpUndoStatus.remove(i);
            savedUndoStatus.remove(i);
            return;
        }
    }
}

void XSMainWindow::saveCurrentFile()
{
    QFile file(currentFilePath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(editor->text().toUtf8());
        file.close();
        isCurrentFileChanged = false;
        fileChangeStatus.replace(ui->widget_file->currentIndex(), isCurrentFileChanged);
        undoStatus.replace(ui->widget_file->currentIndex(), currentUndoStatus);
        savedUndoStatus.replace(ui->widget_file->currentIndex(), true);
        ui->action_save->setEnabled(false);
        ui->widget_file->setTabText(ui->widget_file->currentIndex(), ui->widget_file->tabText(ui->widget_file->currentIndex()).remove(0, 1));
    }
}

void XSMainWindow::saveCurrentFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), currentFilePath);
    if(!fileName.isEmpty())
    {
        if(fileName == currentFilePath)
        {
            this->saveCurrentFile();
        }
        else
        {
            QFile file(fileName);
            if(!file.open(QIODevice::WriteOnly))
            {
                return;
            }
            file.write(editor->text().toUtf8());
            file.close();
            isCurrentFileChanged = false;
            currentUndoStatus = 0;
            currentFilePath = fileName;
            filePaths.replace(ui->widget_file->currentIndex(), fileName);
            fileChangeStatus.replace(ui->widget_file->currentIndex(), isCurrentFileChanged);
            undoStatus.replace(ui->widget_file->currentIndex(), currentUndoStatus);
            tmpUndoStatus.replace(ui->widget_file->currentIndex(), currentUndoStatus);
            savedUndoStatus.replace(ui->widget_file->currentIndex(), false);
            ui->action_save->setEnabled(false);
            QFileInfo fileInfo(file.fileName());
            ui->widget_file->setTabText(ui->widget_file->currentIndex(), fileInfo.fileName());
        }

    }
}

void XSMainWindow::undo()
{
    editor->undo();
}

void XSMainWindow::redo()
{
    editor->redo();
}

void XSMainWindow::setCutEnabled(bool status)
{
    ui->action_copy->setEnabled(status);
    ui->action_cut->setEnabled(status);
}

void XSMainWindow::setPasteEnabled(bool status)
{
    QClipboard *clip = QApplication::clipboard();
    if(!clip->text().isEmpty())
    {
        ui->action_paste->setEnabled(status);
    }
    isEditorFocused = status;
    ui->action_next->setEnabled(status);
    ui->action_previous->setEnabled(status);
    ui->actionReplace->setEnabled(status);
    ui->actionReplace_All->setEnabled(status);
    ui->actionReplace_Find->setEnabled(status);
    ui->action_goto->setEnabled(status);
}

void XSMainWindow::updateUndoStatus(bool status)
{
    if(status)
    {
        currentUndoStatus++;
    }
    else if(!status)
    {
        currentUndoStatus--;
    }

    tmpUndoStatus.replace(ui->widget_file->currentIndex(), currentUndoStatus);
}

void XSMainWindow::cut()
{
    editor->cut();
}

void XSMainWindow::copy()
{
    editor->copy();
}

void XSMainWindow::paste()
{
    editor->paste();
}

void XSMainWindow::updateTabName(const QString &path, const QString &oldName, const QString &newName)
{
    QString oldFilePath = path + "/" + oldName;
    QString newFilePath = path + "/" + newName;

    for(int i = 0; i < filePaths.size(); i++)
    {
        if(filePaths.at(i) == oldFilePath)
        {
            if(fileChangeStatus.at(i))
            {
                QString text = "Please save '" + oldName + "' before changing its name!";
                QMessageBox::information(this, "Waring!", text);
                project_view->renameFailed(oldName);
                return;
            }
            ui->widget_file->setTabText(i, newName);
            filePaths.replace(i, newFilePath);
        }
    }
}

void XSMainWindow::openFindWindow()
{
    findDialog->show();
}

void XSMainWindow::openGoToLineWindow()
{
    if(isEditorFocused)
    {
        goToDialog->setLineRange(editor->lines());
        goToDialog->exec();
    }
}

void XSMainWindow::updateFontSize(bool status)
{
    if(status)
    {
        for(int i = 0; i < editors.size(); i++)
        {
            fontSize = editors.at(i)->fontSize() + 1;
            editors.at(i)->setFontSize(fontSize);
        }
    }
    else
    {
        for(int i = 0; i < editors.size(); i++)
        {
            fontSize = editors.at(i)->fontSize() - 1 <= 1 ? 2 : editors.at(i)->fontSize() - 1;
            editors.at(i)->setFontSize(fontSize);
        }
    }
}

void XSMainWindow::on_action_next_triggered()
{
    editor->findNext();
}

void XSMainWindow::on_action_previous_triggered()
{
    editor->findPrevious();
}

void XSMainWindow::on_actionReplace_triggered()
{
    findDialog->on_btn_replace_clicked();
}

void XSMainWindow::on_actionReplace_Find_triggered()
{
    findDialog->on_btn_replace_find_clicked();
}

void XSMainWindow::on_actionReplace_All_triggered()
{
    findDialog->on_btn_replace_all_clicked();
}

void XSMainWindow::on_btn_package_clicked()
{
    if(currentRootPath.isEmpty())
    {
        return;
    }
    XSAppBuilder builder(currentRootPath, project_view->getProjectName());
    builder.packProject();

}
