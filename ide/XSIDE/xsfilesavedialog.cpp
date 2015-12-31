#include "xsfilesavedialog.h"
#include "ui_xsfilesavedialog.h"
#include <QStandardItemModel>

XSFileSaveDialog::XSFileSaveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XSFileSaveDialog)
{
    ui->setupUi(this);
}

void XSFileSaveDialog::updateList(const QStringList &fileNames, const QStringList &filePaths)
{
    QStandardItemModel *model = new QStandardItemModel;
    for(int i = 0; i < fileNames.size(); i++)
    {
        QString text = fileNames.at(i) + " [" + filePaths.at(i) + "]";
        QStandardItem *item = new QStandardItem(text);
        item->setCheckable(true);
        item->setCheckState(Qt::Checked);
        model->setItem(i, item);
    }
    ui->listView->setModel(model);
}

const QVector<int> XSFileSaveDialog::getSaveFileIndexes()
{
    return indexes;
}

XSFileSaveDialog::~XSFileSaveDialog()
{
    delete ui;
}

void XSFileSaveDialog::on_btn_select_clicked()
{
    for(int i = 0; i < ui->listView->model()->rowCount(); i++)
    {
        ((QStandardItemModel *)ui->listView->model())->item(i)->setCheckState(Qt::Checked);
    }
}

void XSFileSaveDialog::on_btn_unselect_clicked()
{
    for(int i = 0; i < ui->listView->model()->rowCount(); i++)
    {
        ((QStandardItemModel *)ui->listView->model())->item(i)->setCheckState(Qt::Unchecked);
    }
}

void XSFileSaveDialog::on_buttonBox_confirm_accepted()
{
    indexes.clear();
    for(int i = 0; i < ui->listView->model()->rowCount(); i++)
    {
        if(((QStandardItemModel *)ui->listView->model())->item(i)->checkState() == Qt::Checked)
        {
            indexes.append(i);
        }
    }
}
