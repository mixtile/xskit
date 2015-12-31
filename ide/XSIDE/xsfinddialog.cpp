#include "xsfinddialog.h"
#include "ui_xsfinddialog.h"
#include <QDebug>
#include <QSettings>
#include <QCompleter>

XSFindDialog::XSFindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XSFindDialog)
{
    ui->setupUi(this);
    this->hide();
    this->setWindowTitle("Find/Replace");
//    this->setWindowFlags(windowFlags() | Qt::Dialog);
    ui->cbox_find->setFocus();
    ui->cbox_find->completer()->setModel(ui->cbox_find->model());
    ui->cbox_find->completer()->setCaseSensitivity(Qt::CaseSensitive);
    ui->cbox_replace->completer()->setModel(ui->cbox_replace->model());
    ui->cbox_replace->completer()->setCaseSensitivity(Qt::CaseSensitive);
    isFirstFindBackwards = true;
    this->readSettings();
}

QStringList XSFindDialog::getSearchOptions()
{
    QStringList searchFlags;
    if(ui->rbtn_backword->isChecked())
    {
        searchFlags.append("true");
    }
    else
    {
        searchFlags.append("false");
    }

    if(ui->checkBox_wrap->isChecked())
    {
        searchFlags.append("true");
    }
    else
    {
        searchFlags.append("false");
    }

    if(ui->checkBox_case->isChecked())
    {
        searchFlags.append("true");
    }
    else
    {
        searchFlags.append("false");
    }

    if(ui->checkBox_whole->isChecked())
    {
        searchFlags.append("true");
    }
    else
    {
        searchFlags.append("false");
    }

    if(ui->checkBox_regular->isChecked())
    {
        searchFlags.append("true");
    }
    else
    {
        searchFlags.append("false");
    }

    searchFlags.append("false");

    if(ui->rbtn_selected->isChecked())
    {
        searchFlags.append("true");
    }
    else
    {
        searchFlags.append("false");
    }

    return searchFlags;
}

void XSFindDialog::readSettings()
{
        QSettings *settings = new QSettings("./XSEditor.ini", QSettings::IniFormat);
        settings->beginGroup("searchOptions");
        bool backword = settings->value(ui->rbtn_backword->text(), QVariant(false).toBool()).toBool();
        bool selected = settings->value(ui->rbtn_selected->text(), QVariant(false).toBool()).toBool();
        bool caseSen = settings->value(ui->checkBox_case->text(), QVariant(false).toBool()).toBool();
        bool wrap = settings->value(ui->checkBox_wrap->text(), QVariant(false).toBool()).toBool();
        bool whole = settings->value(ui->checkBox_whole->text(), QVariant(false).toBool()).toBool();
        bool incremental = settings->value(ui->checkBox_incremental->text(), QVariant(false).toBool()).toBool();
        bool regular = settings->value(ui->checkBox_regular->text(), QVariant(false).toBool()).toBool();
        settings->endGroup();

        if(backword)
        {
            ui->rbtn_backword->setChecked(true);
            ui->rbtn_forward->setChecked(false);
        }
        else
        {
            ui->rbtn_backword->setChecked(false);
            ui->rbtn_forward->setChecked(true);
        }

        if(selected)
        {
            ui->rbtn_selected->setChecked(true);
            ui->rbtn_all->setChecked(false);
        }
        else
        {
            ui->rbtn_selected->setChecked(false);
            ui->rbtn_all->setChecked(true);
        }

        ui->checkBox_case->setChecked(caseSen);
        ui->checkBox_wrap->setChecked(wrap);
        ui->checkBox_whole->setChecked(whole);
        ui->checkBox_incremental->setChecked(incremental);
        ui->checkBox_regular->setChecked(regular);

        settings->beginGroup("searchHistory");
        QStringList searchKeys = settings->childKeys();
        for(int i = 0; i < searchKeys.size(); i++)
        {
            QString searchItem = settings->value(searchKeys.at(i)).toString();
            ui->cbox_find->addItem(searchItem);
            searchItems.append(searchItem);
        }
        settings->endGroup();

        settings->beginGroup("replaceHistory");
        QStringList replaceKeys = settings->childKeys();
        for(int i = 0; i < replaceKeys.size(); i++)
        {
            QString replaceItem = settings->value(replaceKeys.at(i)).toString();
            ui->cbox_replace->addItem(replaceItem);
            replaceItems.append(replaceItem);
        }
        settings->endGroup();
}

void XSFindDialog::writeSettings()
{
        QSettings *settings = new QSettings("./XSEditor.ini", QSettings::IniFormat);
        settings->remove("searchHistory");
        settings->remove("replaceHistory");
        settings->beginGroup("searchOptions");
        settings->setValue(ui->rbtn_backword->text(), ui->rbtn_backword->isChecked());
        settings->setValue(ui->rbtn_selected->text().toUtf8().data(), ui->rbtn_selected->isChecked());
        settings->setValue(ui->checkBox_case->text(), ui->checkBox_case->isChecked());
        settings->setValue(ui->checkBox_wrap->text(), ui->checkBox_wrap->isChecked());
        settings->setValue(ui->checkBox_whole->text(), ui->checkBox_whole->isChecked());
        settings->setValue(ui->checkBox_incremental->text(), ui->checkBox_incremental->isChecked());
        settings->setValue(ui->checkBox_regular->text(), ui->checkBox_regular->isChecked());
        settings->endGroup();

        settings->beginGroup("searchHistory");
        int searchHistoryCount = searchItems.size() > 8 ? 8 : searchItems.size();
        for(int i = 0; i < searchHistoryCount; i++)
        {
            settings->setValue(QString::number(i), searchItems.at(i));
        }
        settings->endGroup();

        settings->beginGroup("replaceHistory");
        int replaceHistoryCount = replaceItems.size() > 8 ? 8 : replaceItems.size();
        for(int i = 0; i < replaceHistoryCount; i++)
        {
            settings->setValue(QString::number(i), replaceItems.at(i));
        }
        settings->endGroup();
}

void XSFindDialog::addSearchItem()
{
    for(int i = 0; i < searchItems.size(); i++)
    {
        if(searchItems.at(i) == ui->cbox_find->currentText())
        {
            QString tmp = searchItems.at(i);
            ui->cbox_find->removeItem(i);
            searchItems.removeAt(i);
            searchItems.push_front(tmp);
            ui->cbox_find->insertItem(0, tmp);
            ui->cbox_find->setCurrentIndex(0);
            return;
        }
    }
    ui->cbox_find->insertItem(0, ui->cbox_find->currentText());
    searchItems.push_front(ui->cbox_find->currentText());
    ui->cbox_find->setCurrentIndex(0);
}

void XSFindDialog::addReplaceItem()
{
    for(int i = 0; i < replaceItems.size(); i++)
    {
        if(replaceItems.at(i) == ui->cbox_replace->currentText())
        {
            QString tmp = replaceItems.at(i);
            ui->cbox_replace->removeItem(i);
            replaceItems.removeAt(i);
            replaceItems.push_front(tmp);
            ui->cbox_replace->insertItem(0, tmp);
            ui->cbox_replace->setCurrentIndex(0);
            return;
        }
    }
    ui->cbox_replace->insertItem(0, ui->cbox_replace->currentText());
    replaceItems.push_front(ui->cbox_replace->currentText());
    ui->cbox_replace->setCurrentIndex(0);
}

void XSFindDialog::updateStatus(const QStringList &searchFlags)
{
    if(currentSearch != ui->cbox_find->currentText())
    {
        int row, column;
        editor->cursorPosition(&row, &column);
        currentSearch = ui->cbox_find->currentText();
        int searchCount = editor->findAll(currentSearch, searchFlags);
        if(searchCount == 0)
        {
            ui->label_result->setText("Strinng Not Found");
        }
        else
        {
            ui->label_result->setText("");
        }
        editor->setCursorPosition(row, column);
        this->addSearchItem();
    }

    isFirstFindBackwards = true;
    this->addReplaceItem();
}

void XSFindDialog::setEditor(Novile::Editor *editor)
{
    if(NULL == editor)
    {
        ui->cbox_replace->setEnabled(false);
        ui->btn_find->setEnabled(false);
        ui->btn_replace_all->setEnabled(false);
        ui->btn_replace->setEnabled(false);
        ui->btn_replace_find->setEnabled(false);
    }
    else
    {
        ui->cbox_replace->setEnabled(true);
        this->editor = editor;
    }
}

XSFindDialog::~XSFindDialog()
{
    delete ui;
}

void XSFindDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    this->writeSettings();
}

void XSFindDialog::on_btn_close_clicked()
{
    this->close();
}

void XSFindDialog::on_btn_find_clicked()
{
    QStringList searchFlags = this->getSearchOptions();
    if(currentSearch != ui->cbox_find->currentText())
    {
        int row, column;
        editor->cursorPosition(&row, &column);
        currentSearch = ui->cbox_find->currentText();
        int searchCount = editor->findAll(currentSearch, searchFlags);
        if(searchCount == 0)
        {
            ui->label_result->setText("Strinng Not Found");
            ui->btn_replace->setEnabled(false);
            ui->btn_replace_find->setEnabled(false);
        }
        else
        {
            ui->label_result->setText("");
            ui->btn_replace->setEnabled(true);
            ui->btn_replace_find->setEnabled(true);
        }
        editor->setCursorPosition(row, column);
        this->addSearchItem();
    }

    if(searchFlags[0] == "true")
    {
        if(isFirstFindBackwards)
        {
            isFirstFindBackwards= false;
            editor->find(currentSearch, searchFlags);
            editor->findNext();
        }
        editor->findPrevious();
        return;
    }
    editor->find(currentSearch, searchFlags);
}

void XSFindDialog::on_cbox_find_editTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui->checkBox_whole->setEnabled(false);
        ui->btn_find->setEnabled(false);
        ui->btn_replace_all->setEnabled(false);
        ui->btn_replace->setEnabled(false);
        ui->btn_replace_find->setEnabled(false);
    }
    else
    {
        ui->checkBox_whole->setEnabled(true);
        ui->btn_find->setEnabled(true);
        ui->btn_replace_all->setEnabled(true);
    }
}

void XSFindDialog::on_btn_replace_all_clicked()
{
    QStringList searchFlags = this->getSearchOptions();
     this->updateStatus(searchFlags);
    editor->replaceAll(ui->cbox_replace->currentText(), ui->cbox_find->currentText(), searchFlags);
}

void XSFindDialog::on_btn_replace_clicked()
{
    QStringList searchFlags = this->getSearchOptions();
    searchFlags.replace(6, "true");
    searchFlags.replace(0, "true");
    this->updateStatus(searchFlags);
    editor->replace(ui->cbox_replace->currentText(), ui->cbox_find->currentText(), searchFlags);
}

void XSFindDialog::on_btn_replace_find_clicked()
{
    QStringList searchFlags = this->getSearchOptions();
    this->updateStatus(searchFlags);
    editor->replace(ui->cbox_replace->currentText(), ui->cbox_find->currentText(), searchFlags);
}
