#include <QMessageBox>
#include "gamemodel.h"
#include "game.h"

GameModel::GameModel(QObject *parent)
    :QAbstractTableModel(parent)
{
}

int GameModel::rowCount(const QModelIndex & /*parent*/) const
{
   return static_cast<int>(games->size());
}

int GameModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}

QVariant GameModel::data(const QModelIndex &index, int role) const
{
    /*QMessageBox msg;
    for(game &g : *games) {
        msg.setText(QString::fromStdString(g.ID));
        msg.exec();
    }*/

    if (role == Qt::DisplayRole)
    {
        int row = index.row();
        int col = index.column();
        game g = games->at(row);
        if(col == 0)
            return QString(QString::fromStdString(g.ID));
        else
            return QString(QString::fromStdString(g.cfgPath));
    }
    return QVariant();
}

bool GameModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

}

Qt::ItemFlags GameModel::flags(const QModelIndex &index) const
{

}

QVariant GameModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0:
                    return QString("Game ID");
                case 1:
                    return QString("Config Path");
            }
        }
    }
    return QVariant();
}

void GameModel::updateGamesView()
{
    QMessageBox msg;
    msg.setText("GAME ADDED!!!");
    msg.exec();
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight);
}
