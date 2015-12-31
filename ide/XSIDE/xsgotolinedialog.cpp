#include "xsgotolinedialog.h"
#include "ui_xsgotolinedialog.h"
#include <QIntValidator>
#include <QPushButton>

XSGoToLineDialog::XSGoToLineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XSGoToLineDialog)
{
    ui->setupUi(this);
    this->hide();
    this->setWindowTitle("Go to Line");
    this->setFixedSize(this->size());
    QIntValidator *validator = new QIntValidator(this);
    ui->lineEdit->setValidator(validator);
    ui->lineEdit->setFocus();
    ui->label_warn->hide();
    editor = NULL;
}

void XSGoToLineDialog::setLineRange(int maxLineNum)
{
    this->maxLineNum = maxLineNum;
    QString tipStr = "Enter line number (1--%1):";
    ui->label->setText(tipStr.arg(maxLineNum));
}

void XSGoToLineDialog::setEditor(Novile::Editor *editor)
{
    if(NULL == editor)
    {
        this->close();
    }
    else
    {
        this->editor = editor;
    }
}

XSGoToLineDialog::~XSGoToLineDialog()
{
    delete ui;
}

void XSGoToLineDialog::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui->label_warn->hide();
        return;
    }
    int currentNum = arg1.toInt();
    if(currentNum < 1 || arg1.toInt() > maxLineNum)
    {
        ui->label_warn->show();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else
    {
        ui->label_warn->hide();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void XSGoToLineDialog::on_buttonBox_accepted()
{
    int lineNum = ui->lineEdit->text().toInt();
    editor->gotoLine(lineNum);
}
