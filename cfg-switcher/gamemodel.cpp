#include <QMessageBox>
#include "gamemodel.h"

GameModel::GameModel(QObject *parent) : QAbstractTableModel(parent) { }

int GameModel::rowCount(const QModelIndex &) const {
    return rows.size();
}

int GameModel::columnCount(const QModelIndex &) const {
    return NUM_COL;
}

QVariant GameModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rows.size() || index.row() < 0)
        return QVariant();

    Game game;
    switch(role) {
        case Qt::DisplayRole:
            game = rows.at(index.row()).second;
            switch(index.column()) {
                case 1:
                    return game.ID;
                case 2:
                    return game.cfgPath;
                case 3:
                    return game.mainCfgSet;
                case 4:
                    return game.battCfgSet;
                case 5:
                    return game.enabled;
                default:
                    return QVariant();
            }
        case Qt::CheckStateRole:
            if (index.column() == 0)
                return rows.at(index.row()).first;
            else
                return QVariant();
    }

    return QVariant();
}

bool GameModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid())
        return false;

    Game game = rows.value(index.row()).second;
    Qt::CheckState state = rows.value(index.row()).first;
    switch(role) {
        case Qt::EditRole:
            switch(index.column()) {
                case 1:
                    game.ID = value.toString();
                    break;
                case 2:
                    game.cfgPath = value.toString();
                    break;
                case 3:
                    game.mainCfgSet = value.toBool();
                    break;
                case 4:
                    game.battCfgSet = value.toBool();
                    break;
                case 5:
                    game.enabled = value.toBool();
                    break;
                default:
                    return false;
            }
            rows.replace(index.row(), QPair<Qt::CheckState, Game>(state, game));
            emit dataChanged(index, index);
            return true;
        case Qt::CheckStateRole:
            rows.replace(index.row(), QPair<Qt::CheckState, Game>(qvariant_cast<Qt::CheckState>(value), game));

            // Check if select all should be unchecked
            emit setSelectAll(getSelects().indexOf(Qt::Unchecked) == -1);

            // Check if game buttons should be disabled
            emit setGameBtns(getSelects().indexOf(Qt::Checked) != -1);

            // Update table
            emit dataChanged(index, index);

            return true;
        default:
            return false;
    }
}

Qt::ItemFlags GameModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    switch(index.column()) {
        case 0:
           return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
        default:
           return QAbstractTableModel::flags(index);
    }
}

QVariant GameModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch(role) {
        case Qt::DisplayRole:
            if (orientation == Qt::Horizontal) {
                switch (section) {
                    case 1:
                        return tr("Game ID");
                    case 2:
                        return tr("Config Path");
                    case 3:
                        return tr("Main Config Set");
                    case 4:
                        return tr("Battery Config Set");
                    case 5:
                        return tr("Enabled");
                }
            }
            break;
    }

    return QVariant();
}

bool GameModel::insertRows(int position, int nRows, const QModelIndex &) {
    beginInsertRows(QModelIndex(), position, position+nRows-1);
    for (int row=0; row < nRows; row++)
        rows.insert(position, QPair<Qt::CheckState, Game>(Qt::Unchecked, Game()));
    endInsertRows();
    return true;
}

bool GameModel::removeRows(int position, int nRows, const QModelIndex &) {
    beginRemoveRows(QModelIndex(), position, position+nRows-1);
    for (int row = 0; row < nRows; ++row)
        rows.removeAt(position);
    emit setSelectAll(false);
    endRemoveRows();
    return true;
}

QList<Game> GameModel::getGames() {
    QList<Game> games;
    for(QPair<Qt::CheckState, Game> row : rows)
        games.push_back(row.second);
    return games;
}

QList<Qt::CheckState> GameModel::getSelects() {
    QList<Qt::CheckState> selects;
    for(QPair<Qt::CheckState, Game> row : rows)
        selects.push_back(row.first);
    return selects;
}

void GameModel::selectAll(Qt::CheckState state) {
    for(int i = 0; i < rows.size(); i++) {
        QModelIndex index = this->index(i, 0, QModelIndex());
        setData(index, state, Qt::CheckStateRole);
    }
}

bool GameModel::gameExists(QString gameID) {
    for(QPair<Qt::CheckState, Game> row : rows)
        if(!gameID.compare(row.second.ID))
            return true;
    return false;
}
