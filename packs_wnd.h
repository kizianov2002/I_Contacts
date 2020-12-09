#ifndef PACKS_WND_H
#define PACKS_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_app.h"

namespace Ui {
class packs_wnd;
}

#include "ki_exec_query.h"


class packs_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_packs;
    exec_SQL mySQL;

    s_data_app &data_app;

    void refresh_packs_tab();

public:
    explicit packs_wnd(QSqlDatabase &db, s_data_app &data_app, QWidget *parent = 0);
    ~packs_wnd();

private slots:
    void on_bn_close_clicked();

    void on_ch_date_clicked(bool checked);

    //void on_dateEdit_editingFinished();

    void on_bn_today_clicked();

    void on_dateEdit_dateChanged(const QDate &date);

    void on_bn_refresh_clicked();

    void on_bn_delete_pack_clicked();

    void on_bn_save_file_g1_clicked();

private:
    Ui::packs_wnd *ui;
};

#endif // PACKS_WND_H
