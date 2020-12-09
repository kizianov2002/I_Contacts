#ifndef KI_SQLQUERYMODEL
#define KI_SQLQUERYMODEL

#include <QSqlQueryModel>

class ki_persSqlQueryModel : public QSqlQueryModel {
public:
    ki_persSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_polSqlQueryModel : public QSqlQueryModel {
public:
    ki_polSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_calendarSqlQueryModel : public QSqlQueryModel {
public:
    ki_calendarSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_names2sexSqlQueryModel : public QSqlQueryModel {
public:
    ki_names2sexSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_talksSqlQueryModel : public QSqlQueryModel {
public:
    ki_talksSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

#endif // KI_SQLQUERYMODEL

