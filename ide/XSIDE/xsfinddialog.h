#ifndef XSFINDDIALOG_H
#define XSFINDDIALOG_H

#include <QDialog>
#include "editor.h"

namespace Ui {
class XSFindDialog;
}

class XSFindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XSFindDialog(QWidget *parent = 0);
    QStringList getSearchOptions();
    void readSettings();
    void writeSettings();
    void addSearchItem();
    void addReplaceItem();
    void updateStatus(const QStringList &searchFlags);
    void setEditor(Novile::Editor *editor);
    ~XSFindDialog();

protected:
    void	closeEvent(QCloseEvent * event);

private slots:
    void on_btn_close_clicked();

    void on_btn_find_clicked();

    void on_cbox_find_editTextChanged(const QString &arg1);

public slots:
    void on_btn_replace_all_clicked();

    void on_btn_replace_clicked();

    void on_btn_replace_find_clicked();

private:
    Ui::XSFindDialog *ui;
    Novile::Editor *editor;
    QString currentSearch;
    QStringList searchItems;
    QStringList replaceItems;
    bool isFirstFindBackwards;
};

#endif // XSFINDDIALOG_H
