#include <QMessageBox>
#include "gamemodel.h"
#include "game.h"

GameModel::GameModel(QObject *parent) : QAbstractTableModel(parent) { }

int GameModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return games.size();
}

int GameModel::columnCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return NUM_COL;
}

QVariant GameModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= games.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        QPair<QString, QString> game = games.at(index.row());
        if(index.column() == 0)
            return game.first;
        else
            return game.second;
    }

    return QVariant();
}

bool GameModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        QPair<QString, QString> p = games.value(row);

        if (index.column() == 0)
            p.first = value.toString();
        else if (index.column() == 1)
            p.second = value.toString();
        else
            return false;

        games.replace(row, p);
        emit(dataChanged(index, index));

        return true;
    }

    return false;
}

Qt::ItemFlags GameModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QVariant GameModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0:
                    return tr("Game ID");
                case 1:
                    return tr("Config Path");
            }
        }
    }
    return QVariant();
}

bool GameModel::insertRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        QPair<QString, QString> pair(" ", " ");
        games.insert(position, pair);
    }

    endInsertRows();
    return true;
}

bool GameModel::removeRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row)
        games.removeAt(position);

    endRemoveRows();
    return true;
}

QList< QPair<QString, QString> > GameModel::getGames()
{
    return games;
}
