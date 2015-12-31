#ifndef XSMAINWINDOW_H
#define XSMAINWINDOW_H

#include <QMainWindow>
#include "editor.h"
#include "xstreeview.h"
#include "xsfinddialog.h"
#include "xsgotolinedialog.h"

namespace Ui {
class XSMainWindow;
}

class XSMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit XSMainWindow(QWidget *parent = 0);
    void setButtonImage();
    void openNewFile(const QString &fileName);
    void openProjectFromPath(const QString path);
    bool checkOpenExistence(const QString &filePath);
    void readSettings();
    void writeSettings();
    bool checkFileChanges();
    bool maybeSave();
    void addMyproject(const QString &projectName);
    void openSelectedProject(QAction *act);
    bool getFileChangeStatusWithUndo();
    ~XSMainWindow();

protected:
    void	closeEvent(QCloseEvent * event);
    void resizeEvent(QResizeEvent * event);
    void moveEvent(QMoveEvent * ev);

private slots:
    void projectOpenMenu(QAction *action);
    void openProject();
    void newProject();
    void deleteProject();
    void newFile();
    void showFile(QFileInfo info);
    void scratchFile();
    void closeTabPage(int index);
    void switchTabPage(int index);
    void removeTabPage(const QString &filePath);
    void saveCurrentFile();
    void saveCurrentFileAs();
    void undo();
    void redo();
    void setCutEnabled(bool status);
    void setPasteEnabled(bool status);
    void updateUndoStatus(bool status);
    void cut();
    void copy();
    void paste();
    void updateTabName(const QString & path, const QString & oldName, const QString & newName);
    void openFindWindow();
    void openGoToLineWindow();
    void updateFontSize(bool status);

    void on_action_next_triggered();

    void on_action_previous_triggered();

    void on_actionReplace_triggered();

    void on_actionReplace_Find_triggered();

    void on_actionReplace_All_triggered();

    void on_btn_package_clicked();

private:
    Ui::XSMainWindow *ui;
    Novile::Editor *editor;//当前使用的文件编辑器
    QString currentFilePath;//当前打开的文件路径
    QString currentRootPath;//当前项目目录
    bool isFirstOpen;//是否第一次打开文件
    bool isCurrentFileChanged;//当前文件是否被改变
    bool isEditorFocused;//编辑器焦点判断;
    int currentUndoStatus;//当前文件的undo状态
    XSTreeView *project_view;
    XSFindDialog *findDialog;
    XSGoToLineDialog *goToDialog;

    QMenu *projectMenu;
    QAction *act_new_project;
    QAction *act_open_project;

    QStringList projectPaths;//打开过的项目路径列表
    QStringList projectNames;//打开过的项目名列表
    QStringList  filePaths;//已打开文件路径列表
    QVector<int> tmpUndoStatus;//文件当前的undo状态列表
    QVector<int> undoStatus;//文件保存时的undo状态列表
    QVector<bool> savedUndoStatus;//文件是否已被保存过
    QVector<Novile::Editor *> editors;//已打开文件编辑器列表
    QVector<bool> fileChangeStatus;//文件编辑状态列表

    QSize windowSize;
    QPoint windPosition;
    int fontSize;
};

#endif // XSMAINWINDOW_H
