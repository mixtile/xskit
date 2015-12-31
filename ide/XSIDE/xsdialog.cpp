#include "xsdialog.h"
#include "ui_xsdialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

extern bool removeDirWithContent(const QString &dirName);

XSDialog::XSDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XSDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Create New Project"));
    model = new QStandardItemModel;
    QStandardItem *item1 = new QStandardItem("Hello World");
//    QStandardItem *item2 = new QStandardItem("model2");
//    QStandardItem *item3 = new QStandardItem("model3");
    model->appendRow(item1);
//    model->appendRow(item2);
//    model->appendRow(item3);
    ui->listView->setCurrentIndex(model->index(0, 0));
    ui->listView->setModel(model);
    ui->listView->setCurrentIndex(model->index(0, 0));
    ui->lineEdit->setFocus();
}

const QString XSDialog::createProjectByModel()
{
    int selectedIndex = ui->listView->currentIndex().row();
    QString projectName = ui->lineEdit->text();
    QString projectPath;
    if(projectName.isEmpty())
    {
        QMessageBox::information(this, "warning!", "Filename can not be empty!");
        return projectPath;
    }

    QString path = QFileDialog::getExistingDirectory(this, tr("New Project Path"), ".");
    if(path.isEmpty())
    {
        return projectPath;
    }

    switch(selectedIndex)
    {
    case 0:
        projectPath = checkProjectExistance(projectName, path, 0);
        break;
    case 1:
        break;
    case 2:
        break;
    }

    return projectPath;
}

const QString  XSDialog::checkProjectExistance(const QString &projectName, const QString &projectFolder, int modelStyle)
{
    QString projectPath = projectFolder + "/" + projectName;
    QDir *dir = new QDir(projectPath);
    if(dir->exists())
    {
        QString text = "Folder '%1' already exists, are you sure to overwrite it? ";
        int  ret = QMessageBox::information(this, "warning!", text.arg(projectName),QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel)
        {
            QString path = QFileDialog::getExistingDirectory(this, tr("New Project Path"), projectFolder);
            if(path.isEmpty())
            {
                return path;
            }
            return checkProjectExistance(projectName, path, modelStyle);
        }
        else
        {
            if(removeDirWithContent(projectPath))
            {
                dir->mkpath(projectPath);
                dir->mkdir(tr("ui"));
                dir->mkdir(tr("scripts"));
                createProjectFiles(projectName, projectPath, modelStyle);
                return projectPath;
            }
        }
    }
    else
    {
        dir->mkpath(projectPath);
        dir->mkdir(tr("ui"));
        dir->mkdir(tr("scripts"));
        createProjectFiles(projectName, projectPath, modelStyle);
        return projectPath;
    }
}

bool XSDialog::removeDirWithContent(const QString &dirName)
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

void XSDialog::createProjectFiles(const QString &projectName, const QString &projectPath, int modelStyle)
{
    QFile appModel(":xs_model/models/app.xml"), mainModel(":xs_model/models/main.js");
    QString appPath = projectPath + "/app.xml";
    QString mainPath = projectPath + "/scripts/main.js";
    QFile appFile(appPath), mainFile(mainPath);
    appFile.open(QIODevice::WriteOnly);
    mainFile.open(QIODevice::WriteOnly);
    appFile.close();
    mainFile.close();
    if(appModel.open(QIODevice::ReadOnly))
    {
        if(appFile.open(QIODevice::WriteOnly))
        {
            QTextStream modelStream(&appModel);
            QTextStream fileStream(&appFile);
            QString text = modelStream.readAll().arg(projectName).arg(projectName);
            fileStream<<text;
            appModel.close();;
            appFile.close();
        }
    }

    if(mainModel.open(QIODevice::ReadOnly))
    {
        if(mainFile.open(QIODevice::WriteOnly))
        {
            QTextStream modelStream(&mainModel);
            QTextStream fileStream(&mainFile);
            QString text = modelStream.readAll().arg(projectName).arg(projectName);
            fileStream<<text;
            mainModel.close();;
            mainFile.close();
        }
    }
}


XSDialog::~XSDialog()
{
    delete ui;
    if(model != NULL)
    {
        delete model;
    }
}
