#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>
#include <vector>
#include "game.h"

class GameModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    std::vector<game>* games;

public:
    GameModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void setGames(std::vector<game> &gs) {
        games = &gs;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void updateGamesView();
};

#endif // GAMEMODEL_H
