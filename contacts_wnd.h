#ifndef contacts_WND_H
#define contacts_WND_H

#include <QMainWindow>

#include "ki_exec_query.h"
#include "ki_SqlQueryModel.h"

#include "s_data_app.h"
#include "s_data_contact.h"

#include "send2fond_wnd.h"
#include "print_monitoring_wnd.h"
#include "outerdb_wnd.h"
#include "packs_wnd.h"
#include "get_date_wnd.h"
#include "show_tab_wnd.h"

namespace Ui {
class contacts_wnd;
}

class contacts_wnd : public QMainWindow
{
    Q_OBJECT
    QSqlDatabase &db;       // это - наша база данных
    QSqlDatabase db_ODBC;   // это - ODBC-окошко для генерации dbf
    QSqlQueryModel model_contacts;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_contact data_contact;

    send2fond_wnd *send2fond_w;
    print_monitoring_wnd *print_monitoring_w;
    outerdb_wnd *outerdb_w;
    packs_wnd *packs_w;
    get_date_wnd *get_date_w;
    show_tab_wnd *show_tab_w;

    QStringList kladr_subj, kladr_dstr, kladr_city, kladr_nasp, kladr_strt;
    void refresh_kladr(int subj, int dstr, int city, int nasp, int strt);

    void refresh_combo_filials();
    void refresh_combo_sex();
    void refresh_combo_soc_status();
    void refresh_combo_doc_type();
    void refresh_combo_talker_form();
    void refresh_combo_req_way();
    void refresh_combo_req_type();
    void refresh_combo_req_reason();
    void refresh_combo_req_status();
    void refresh_combo_req_result();

    void refresh_contacts_tab();

    bool save_contact_data();

public:
    explicit contacts_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~contacts_wnd();

private slots:

    void on_bn_exit_clicked();

    void on_bn_refresh_clicked();
    void on_combo_filial_filter_activated(int index);
    void on_line_fam_filter_editingFinished();
    void on_line_im_filter_editingFinished();
    void on_line_ot_filter_editingFinished();

    void on_bn_clear_filter_clicked();

    void on_ch_date_birth_clicked(bool checked);
    void on_ch_date_doc_clicked(bool checked);
    void on_ch_date_control_clicked(bool checked);
    void on_ch_date_incident_clicked(bool checked);
    void on_ch_date_done_clicked(bool checked);
    
    void on_tab_contacts_clicked(const QModelIndex &index);

    void on_combo_req_type_currentIndexChanged(int index);

    void on_bn_add_bybase_clicked();
    void on_bn_add_byhand_clicked();
    void on_bn_cansel_clicked();

    void on_bn_save_clicked();
    void on_bn_edit_clicked();

    void on_line_pol_enp_textChanged(const QString &arg1);

    void on_bn_delete_clicked();

    void on_tab_contacts_activated(const QModelIndex &index);

    void on_act_send2fond_triggered();

    void on_ch_year_birth_clicked(bool checked);
    void on_spin_year_birth_valueChanged(int arg1);
    void on_date_birth_dateChanged(const QDate &date);

    void on_ch_new_lines_clicked(bool checked);

    void on_act_close_triggered();

    void on_act_about_triggered();

    void on_combo_kladr_subj_activated(const QString &arg1);

    void on_combo_kladr_dstr_activated(const QString &arg1);

    void on_combo_kladr_city_activated(const QString &arg1);

    void on_combo_kladr_nasp_activated(const QString &arg1);

    void on_act_monitoring_triggered();

    void on_bn_hide_clicked();

    void on_combo_req_way_currentIndexChanged(int index);

    void on_line_fam_textChanged(const QString &arg1);
    void on_line_im_textChanged(const QString &arg1);
    void on_line_ot_textChanged(const QString &arg1);
    void on_line_req_location_textChanged(const QString &arg1);
    void on_line_disp_fio_textChanged(const QString &arg1);
    void on_line_doc_org_textChanged(const QString &arg1);
    void on_line_address_reg_textChanged(const QString &arg1);
    void on_line_address_liv_textChanged(const QString &arg1);
    void on_plain_req_petition_textChanged();
    void on_line_req_comment_textChanged(const QString &arg1);
    void on_line_req_perform_textChanged(const QString &arg1);
    void on_line_req_performer_textChanged(const QString &arg1);
    //void on_plain_req_result_textChanged();

    void on_act_files_triggered();

    void on_act_req_reester_triggered();

    void on_line_snils_filter_editingFinished();

    void on_line_enp_filter_editingFinished();

    void on_line_vs_filter_editingFinished();

    void on_line_snils_filter_textEdited(const QString &arg1);

    void on_req_by_months_triggered();

    void on_bn_status_2_clicked();

    void on_combo_req_status_currentIndexChanged(int index);

private:
    Ui::contacts_wnd *ui;
};

#endif // contacts_WND_H
