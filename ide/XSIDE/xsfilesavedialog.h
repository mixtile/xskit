#ifndef XSFILESAVEDIALOG_H
#define XSFILESAVEDIALOG_H

#include <QDialog>

namespace Ui {
class XSFileSaveDialog;
}

class XSFileSaveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XSFileSaveDialog(QWidget *parent = 0);
    void updateList(const QStringList &fileNames, const QStringList &filePaths);
    const QVector<int> getSaveFileIndexes();
    ~XSFileSaveDialog();

signals:

private slots:
    void on_btn_select_clicked();

    void on_btn_unselect_clicked();

    void on_buttonBox_confirm_accepted();

private:
    Ui::XSFileSaveDialog *ui;
    QVector<int > indexes;
};

#endif // FILESAVEDIALOG_H
