#ifndef XSDIALOG_H
#define XSDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class XSDialog;
}

class XSDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XSDialog(QWidget *parent = 0);
    const QString createProjectByModel();
    const QString checkProjectExistance(const QString &projectName, const QString &projectFolder, int modelStyle);
    bool removeDirWithContent(const QString &dirName);
    void createProjectFiles(const QString &projectName, const QString &projectPath, int modelStyle);
    ~XSDialog();

private:
    Ui::XSDialog *ui;
    QStandardItemModel *model;
};

#endif // XSDIALOG_H
