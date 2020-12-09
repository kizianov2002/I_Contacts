#ifndef SHOW_TAB_WND_H
#define SHOW_TAB_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_app.h"
#include "ki_sqlquerymodel.h"

namespace Ui {
class show_tab_wnd;
}

#include "ki_exec_query.h"

class show_tab_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_tab;
    exec_SQL mySQL;

    s_data_app &data_app;
    bool f_hist;

    QString &sql;
    QString sql_show;
    QString header;

    void refresh_tab();

public:
    explicit show_tab_wnd(QString header, QString &sql, QSqlDatabase &db, s_data_app &data_app, QWidget *parent = 0, bool f_hist=false);
    ~show_tab_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_toCSV_clicked();

private:
    Ui::show_tab_wnd *ui;
};

#endif // SHOW_TAB_WND_H
