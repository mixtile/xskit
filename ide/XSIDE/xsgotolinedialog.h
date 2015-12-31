#ifndef XSGOTOLINEDIALOG_H
#define XSGOTOLINEDIALOG_H

#include <QDialog>
#include "editor.h"

namespace Ui {
class XSGoToLineDialog;
}

class XSGoToLineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XSGoToLineDialog(QWidget *parent = 0);
    void setLineRange(int maxLineNum);
    void setEditor(Novile::Editor *editor);
    ~XSGoToLineDialog();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::XSGoToLineDialog *ui;
    int maxLineNum;
    Novile::Editor *editor;
};

#endif // XSGOTOLINEDIALOG_H
