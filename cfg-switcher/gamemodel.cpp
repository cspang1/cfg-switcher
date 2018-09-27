#include <QMessageBox>
#include "gamemodel.h"

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

    if (index.row() >= games.size() || index.row() >= selects.size() || index.row() < 0)
        return QVariant();

    Game game;
    switch(role) {
    case Qt::DisplayRole:
        game = games.at(index.row());
        if(index.column() == 1)
            return game.ID;
        else if(index.column() == 2)
            return game.cfgPath;
        else if(index.column() == 3)
            return game.mainCfgSet;
        else if(index.column() == 4)
            return game.battCfgSet;
        else if(index.column() == 5)
            return game.enabled;
        else
            return QVariant();
    case Qt::CheckStateRole:
        if (index.column() == 0)
            return selects.at(index.row());
        else
            return QVariant();
    }

    return QVariant();
}

bool GameModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid())
        return false;

    Game game;
    bool state;
    switch(role) {
    case Qt::EditRole:
        game = games.value(index.row());

        if (index.column() == 1)
            game.ID = value.toString();
        else if (index.column() == 2)
            game.cfgPath = value.toString();
        else if (index.column() == 3)
            game.mainCfgSet = value.toBool();
        else if (index.column() == 4)
            game.battCfgSet = value.toBool();
        else if (index.column() == 5)
            game.enabled = value.toBool();
        else
            return false;

        games.replace(index.row(), game);
        emit dataChanged(index, index);
        return true;
    case Qt::CheckStateRole:
        selects.replace(index.row(), qvariant_cast<Qt::CheckState>(value));
        state = true;
        for(Qt::CheckState &s : selects) {
            if(s == Qt::Unchecked) {
                state = false;
                break;
            }
        }

        if(selects.indexOf(Qt::Checked) == -1)
            emit setGameBtns(false);
        else
            emit setGameBtns(true);

        emit setSelectAll(state);
        emit dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags GameModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    if(index.column() == 0)
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    else
        return QAbstractTableModel::flags(index);
}

QVariant GameModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch(role) {
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 1:
                return tr("Game ID");
            case 2:
                return tr("Config Path");
            case 3:
                return tr("Main Config Set");
            case 4:
                return tr("Battery Config Set");
            case 5:
                return tr("Enabled");            }
        }
        break;
    }

    return QVariant();
}

bool GameModel::insertRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        Game game("", "", false, false);
        Qt::CheckState select = Qt::Unchecked;
        games.insert(position, game);
        selects.insert(position, select);
    }

    endInsertRows();
    return true;
}

bool GameModel::removeRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        selects.removeAt(position);
        games.removeAt(position);
    }
    emit setSelectAll(false);

    endRemoveRows();
    return true;
}

QList<Game> GameModel::getGames() {
    return games;
}

QList<Qt::CheckState> GameModel::getSelects() {
    return selects;
}

void GameModel::selectAll(Qt::CheckState state) {
    for(int i = 0; i < selects.size(); i++) {
        QModelIndex index = this->index(i, 0, QModelIndex());
        setData(index, state, Qt::CheckStateRole);
    }
}

bool GameModel::gameExists(QString gameID) {
    for(Game &g : games)
        if(!gameID.compare(g.ID))
            return true;
    return false;
}
