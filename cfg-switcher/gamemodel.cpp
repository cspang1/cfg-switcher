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

    if (index.row() >= games.size() || index.row() >= selects.size() || index.row() < 0)
        return QVariant();

    QPair<QString, QString> game;
    switch(role) {
    case Qt::DisplayRole:
        game = games.at(index.row());
        if(index.column() == 1)
            return game.first;
        else if(index.column() == 2)
            return game.second;
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

    QPair<QString, QString> p;
    bool state;
    switch(role) {
    case Qt::EditRole:
        p = games.value(index.row());

        if (index.column() == 1)
            p.first = value.toString();
        else if (index.column() == 2)
            p.second = value.toString();
        else
            return false;

        games.replace(index.row(), p);
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
            }
        }
        break;
    }

    return QVariant();
}

bool GameModel::insertRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        QPair<QString, QString> pair(" ", " ");
        Qt::CheckState select = Qt::Unchecked;
        games.insert(position, pair);
        selects.insert(position, select);
    }

    endInsertRows();
    return true;
}

bool GameModel::removeRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        selects.removeAt(position);
        games.removeAt(position);
    }
    emit setSelectAll(false);

    endRemoveRows();
    return true;
}

QList< QPair<QString, QString> > GameModel::getGames() {
    return games;
}

QList<Qt::CheckState> GameModel::getSelects() {
    return selects;
}

void GameModel::selectAll(bool state) {
    QModelIndex top = createIndex(0, 0, nullptr);
    QModelIndex bottom = createIndex(selects.size() - 1, 0, nullptr);
    for(Qt::CheckState &cs : selects)
        cs = state? Qt::Checked : Qt::Unchecked;
    emit dataChanged(top, bottom);
}
