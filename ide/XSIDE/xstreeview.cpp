#include "xstreeview.h"
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QHeaderView>

XSTreeView::XSTreeView(QWidget *parent)
    :QTreeView(parent)
{
    isAddFile = false;
    contextMenu = new QMenu(this);
    act_new = new QAction(tr("new"), this);
    act_delete = new QAction(tr("delete"), this);
    act_rename = new QAction(tr("rename"), this);

    contextMenu->addAction( act_new);
    contextMenu->addAction( act_rename);
    contextMenu->addAction( act_delete);

    dirMod = new QFileSystemModel;
    dirMod->setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    dirMod->setReadOnly(false);
    this->setModel(dirMod);
    this->setMinimumSize(200, 0);

    connect(this, SIGNAL(clicked(QModelIndex)),this, SLOT(onItemclick(QModelIndex)));
    connect(act_new, SIGNAL(triggered()), this, SLOT(addNewFile()));
    connect(act_rename, SIGNAL(triggered()), this, SLOT(renameFile()));
    connect(act_delete, SIGNAL(triggered()), this, SLOT(deleteFile()));
    connect(dirMod, SIGNAL(fileRenamed(QString,QString,QString)), this ,SIGNAL(updateTabName(QString,QString,QString)));
}

void XSTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    contextMenu->exec(this->cursor().pos());
}

void XSTreeView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    if(start == end && isAddFile)
    {
        QModelIndex son = parent.child(start, 0);
        this->setCurrentIndex(son);
        this->expand(parent);
        isAddFile = false;
        this->renameFile();
    }
}

void XSTreeView::showProject(const QString &path)
{
    dirMod->setRootPath(path);
    this->setRootIndex(dirMod->index(path));
    //隐藏其他文件属性列
    for(int i = 0; i < dirMod->columnCount(); i++)
    {
        this->setColumnHidden(i + 1, true);
    }
    //设置项目标题
    QStandardItemModel *headModel=new QStandardItemModel(0,1,this);
    headModel->setHeaderData(0, Qt::Horizontal, getProjectName());
    this->header()->setModel(headModel);
}

bool XSTreeView::removeDirWithContent(const QString &dirName)
{
    static QVector<QString> dirNames;
    QDir dir;
    QFileInfoList filst;
    QFileInfoList::iterator curFi;

    //初始化
    dirNames.clear();
    if(dir.exists()){
        dirNames<<dirName;
    }
    else{
        return true;
    }

    //遍历各级文件夹，并将这些文件夹中的文件删除
    for(int i=0;i<dirNames.size();++i){
        dir.setPath(dirNames[i]);
        filst=dir.entryInfoList(QDir::Dirs|QDir::Files
                                |QDir::Readable|QDir::Writable
                                |QDir::Hidden|QDir::NoDotAndDotDot
                                ,QDir::Name);
        if(filst.size()>0){
            curFi=filst.begin();
            while(curFi!=filst.end()){
                 //遇到文件夹,则添加至文件夹列表dirs尾部
                if(curFi->isDir()){
                    dirNames.push_back(curFi->filePath());
                }else if(curFi->isFile()){
                    //遇到文件,则删除之
                    if(!dir.remove(curFi->fileName())){
                        return false;
                    }
                    else
                    {
                        emit removeTabPage(curFi->filePath());
                    }
                }
                curFi++;
            }//end of while
        }
    }

    qDebug()<<dirNames.size();
    //删除文件夹
    for(int i=dirNames.size()-1;i>=0;--i){
        dir.setPath(dirNames[i]);
        if(!dir.rmdir(dir.path())){
            return false;
        }
    }
    return true;
}

void XSTreeView::renameFailed(const QString &text)
{
    dirMod->setData(this->currentIndex(), text);
}

const QString XSTreeView::getProjectName()
{
    return dirMod->fileName(this->rootIndex());
}

XSTreeView::~XSTreeView()
{
    if(dirMod != NULL)
    {
        delete dirMod;
    }

    if(contextMenu != NULL)
    {
        delete contextMenu;
    }
}

void XSTreeView::onItemclick(const QModelIndex &index)
{
    if(dirMod->isDir(index))
    {

    }
    else
    {
        emit openFile(dirMod->fileInfo((index)));
    }
}

void XSTreeView::addNewFile()
{
    QString filePath;
    if(!this->currentIndex().isValid())
    {
        filePath = dirMod->rootPath() + "/newfile%1.js";
    }
    else
    {
        if(dirMod->isDir(this->currentIndex()))
        {
            filePath =dirMod->filePath(this->currentIndex()) + "/newfile%1.js";
        }
        else
        {
            filePath = dirMod->fileInfo(this->currentIndex()).dir().path() + "/newfile%1.js";
        }
    }

    QFile file(filePath.arg(""));
    if(!file.exists())
    {
        file.open(QIODevice::WriteOnly);
        file.close();
        isAddFile = true;
        return;
    }

    int i = 1;
    while(file.exists())
    {
        file.setFileName(filePath.arg(i));
        i++;
    }
    file.open(QIODevice::WriteOnly);
    file.close();
    isAddFile = true;
}

void XSTreeView::renameFile()
{
    QString path = dirMod->filePath(this->currentIndex());
    if(path.isNull())
    {
        qDebug() << "file path is null";
        return;
    }

    QKeyEvent event(QEvent::KeyPress, Qt::Key_F2, Qt::NoModifier);
    edit(this->currentIndex(), EditKeyPressed, &event);
}

void XSTreeView::deleteFile()
{
    QMessageBox deleteMsg;
    if(dirMod->isDir(this->currentIndex()))
    {
        deleteMsg.setText(tr("Are you sure to delete the directory?"));
    }
    else
    {
        deleteMsg.setText(tr("Are you sure to delete the file?"));
    }
    deleteMsg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    deleteMsg.setDefaultButton(QMessageBox::Cancel);
    int ret = deleteMsg.exec();

    if(ret == QMessageBox::Yes)
    {
        if(dirMod->isDir(this->currentIndex()))
        {
            removeDirWithContent(dirMod->filePath(this->currentIndex()));
        }
        else
        {
            QFile file(dirMod->filePath(this->currentIndex()));
            if(file.open(QIODevice::ReadWrite))
            {
                file.remove();
                emit removeTabPage(dirMod->filePath(this->currentIndex()));
            }
        }
    }
    else
    {
        return;
    }
}
