#ifndef OUTERDB_SETTINGS_H
#define OUTERDB_SETTINGS_H

#include <QDialog>

#include "ki_exec_query.h"
#include "ki_SqlQueryModel.h"

#include "s_data_app.h"
#include "s_data_contact.h"

namespace Ui {
class outerdb_wnd;
}

typedef struct {
    QString filial_name, name, driv, host, base, user, pass;
    int id, port, status;
    QString sql_persons_tab;
    QString sql_polises_tab;
    QSqlDatabase db_filial;
} s_filial_data;

class outerdb_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;       // это - наша база данных
    QSqlQueryModel model_persons;
    QSqlQueryModel model_polises;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_contact &data_contact;

    QList<s_filial_data> filials_list;

    void refresh_filials();
    bool try_connect_outerdb();
    void refresh_persons_tab();
    void refresh_polises_tab();

public:
    explicit outerdb_wnd(QSqlDatabase &db, s_data_contact &data_contact, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~outerdb_wnd();

private slots:
    void on_bn_close_clicked();

    void on_combo_filial_activated(int index);

    void on_combo_filial_currentIndexChanged(int index);

    void on_bn_connect_clicked();

    void on_ch_host_clicked(bool checked);
    void on_ch_port_clicked(bool checked);
    void on_ch_base_clicked(bool checked);
    void on_ch_user_clicked(bool checked);
    void on_ch_pass_clicked(bool checked);

    void on_bn_refresh_clicked();

    void on_line_fam_filter_returnPressed();

    void on_line_im_filter_returnPressed();

    void on_line_ot_filter_returnPressed();

    void on_line_enp_filter_returnPressed();

    void on_bn_clear_filter_clicked();

    void on_bn_disconnect_clicked();

    void on_bn_process_clicked();

    void on_tab_persons_clicked(const QModelIndex &index);

private:
    Ui::outerdb_wnd *ui;
};

#endif // OUTERDB_SETTINGS_H
