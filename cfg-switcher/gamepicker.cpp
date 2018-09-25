#include "gamepicker.h"
#include "ui_gamepicker.h"

#include <QFileDialog>
#include <QMessageBox>

GamePicker::GamePicker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GamePicker)
{
    ui->setupUi(this);
    ui->gamePathLE->setReadOnly(true);
}

GamePicker::~GamePicker()
{
    delete ui;
}

QString GamePicker::getGameName() {
    return ui->gameNameLE->text();
}

QString GamePicker::getGamePath() {
    return ui->gamePathLE->text();
}

void GamePicker::on_cancelAddGameBtn_clicked()
{
    this->reject();
}

void GamePicker::on_browseGameBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, ("Select Game Config File"), "", (""));
    if(!fileName.isEmpty())
        ui->gamePathLE->setText(fileName);
}

void GamePicker::on_saveGameBtn_clicked()
{
    if(getGameName().isEmpty() || getGamePath().isEmpty())
        QMessageBox::question(this, tr("Error"), tr("Both a game name and valid game config file path required"), QMessageBox::Ok);
    else
        this->accept();
}
