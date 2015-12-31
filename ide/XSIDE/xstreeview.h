#ifndef XSTREEVIEW_H
#define XSTREEVIEW_H

#include <QTreeView>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include<QFileSystemModel>

class XSTreeView: public QTreeView
{
    Q_OBJECT

public:
    XSTreeView(QWidget *parent = 0);

    void showProject(const QString &path);
    bool removeDirWithContent(const QString &dirName);
    void renameFailed(const QString &text);
    const QString getProjectName();
    ~XSTreeView();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void rowsInserted(const QModelIndex & parent, int start, int end);

signals:
   void sendItem(bool isDir);
   void openFile(QFileInfo info);
   void removeTabPage(const QString &filPath);
   void updateTabName(const QString & path, const QString & oldName, const QString & newName);

public slots:
      void addNewFile();

private slots:
   void onItemclick(const QModelIndex &index);
   void renameFile();
   void deleteFile();

private:
    bool isAddFile;
    QMenu *contextMenu;
    QAction *act_new;
    QAction *act_delete;
    QAction *act_rename;

    QFileSystemModel *dirMod;
};

#endif // XSTREEVIEW_H
