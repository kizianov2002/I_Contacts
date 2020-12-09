#ifndef SEND2FOND_WND_H
#define SEND2FOND_WND_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class send2fond_wnd;
}

class send2fond_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlDatabase db_ODBC;    // подключение к ODBC-DBF
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_combo_filial();

    bool gen_Voronej();
    bool gen_Belgorod();
    bool gen_Kursk();

public:
    explicit send2fond_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~send2fond_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_process_clicked();

    void on_rb_by_dates_clicked(bool checked);

    void on_rb_new_clicked(bool checked);

    void on_date_from_editingFinished();

    void on_date_to_editingFinished();

private:
    Ui::send2fond_wnd *ui;
};

#endif // SEND2FOND_WND_H
