#ifndef print_monitoring_WND_H
#define print_monitoring_WND_H

#include <QDialog>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class print_monitoring_wnd;
}

class print_monitoring_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_polises;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    QString date_to_str(QDate date);

public:
    explicit print_monitoring_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_monitoring_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_print_clicked();

    void on_bn_edit_clicked(bool checked);

    void on_bn_cansel_clicked();

private:
    Ui::print_monitoring_wnd *ui;
};

#endif // print_monitoring_WND_H
