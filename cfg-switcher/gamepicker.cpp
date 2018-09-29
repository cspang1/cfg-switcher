#include <QFileDialog>
#include <QMessageBox>
#include "gamepicker.h"
#include "ui_gamepicker.h"

GamePicker::GamePicker(QList<Game> _games, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GamePicker),
    games(_games) {
    ui->setupUi(this);
    ui->gamePathLE->setReadOnly(true);
}

GamePicker::~GamePicker() {
    delete ui;
}

QString GamePicker::getGameName() {
    return ui->gameNameLE->text();
}

QString GamePicker::getGamePath() {
    return ui->gamePathLE->text();
}

void GamePicker::on_cancelAddGameBtn_clicked() {
    this->reject();
}

void GamePicker::on_browseGameBtn_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, ("Select Game Config File"), "", (""));
    if(!fileName.isEmpty())
        ui->gamePathLE->setText(fileName);
}

void GamePicker::on_saveGameBtn_clicked() {
    QString illChars = "< > * : \" / \\ | ?";
    QRegExp re("[<>*:\"\\\\/\\|?]");
    if(re.indexIn(getGameName()) >= 0)
        QMessageBox::information(this, tr("Error"), tr("The following characters are not permitted in the game name:\n%1").arg(illChars), QMessageBox::Ok);
    else if(getGameName().isEmpty() || getGamePath().isEmpty())
        QMessageBox::information(this, tr("Error"), tr("Both a game name and valid game config file path required"), QMessageBox::Ok);
    else if(gameExists(getGameName()))
        QMessageBox::information(this, tr("Error"), tr("%1 already exists in configuration").arg(getGameName()), QMessageBox::Ok);
    else
        this->accept();
}

bool GamePicker::gameExists(QString game) {
    for(Game &g : games)
        if(!game.compare(g.ID))
            return true;
    return false;
}
