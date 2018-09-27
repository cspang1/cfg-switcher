#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractTableModel>
#include "game.h"

class GameModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QList<Game> games;
    QList<Qt::CheckState> selects;
    const int NUM_COL = 5;

public:
    GameModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool insertRows(int position, int rows, const QModelIndex &index) override;
    bool removeRows(int position, int rows, const QModelIndex &index) override;
    QList<Game> getGames();
    QList<Qt::CheckState> getSelects();

signals:
    void setSelectAll(bool state);
    void setGameBtns(bool state);

public slots:
    void selectAll(Qt::CheckState state);
};

#endif // GAMEMODEL_H
