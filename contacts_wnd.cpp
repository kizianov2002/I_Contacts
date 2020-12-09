#include "contacts_wnd.h"
#include "ui_contacts_wnd.h"

#include <QtSql>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QScrollBar>
#include <QModelIndexList>

#include <winuser.h>
#include "packs_wnd.h"

int max(int A, int B) {
    return (A>B) ? A : B;
}

contacts_wnd::contacts_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), settings(settings), QMainWindow(parent), ui(new Ui::contacts_wnd)
{
    ui->setupUi(this);

    send2fond_w = NULL;
    print_monitoring_w = NULL;
    outerdb_w = NULL;
    get_date_w = NULL;
    show_tab_w = NULL;

    this->setWindowTitle("ИНКО-МЕД  -  «КОНТАКТ-центр»;  v. " + data_app.version);

    refresh_combo_filials();

    refresh_contacts_tab();

    refresh_combo_sex();
    refresh_combo_soc_status();
    refresh_combo_doc_type();
    refresh_combo_talker_form();

    refresh_combo_req_way();
    refresh_combo_req_type();
    refresh_combo_req_reason();
    refresh_combo_req_status();
    refresh_kladr(0,0,0,0,0);
    refresh_combo_req_result();

    on_tab_contacts_clicked(ui->tab_contacts->indexAt(QPoint(-100,-100)));

    on_line_fam_textChanged("");
    on_line_im_textChanged("");
    on_line_ot_textChanged("");
    on_line_req_location_textChanged("");
    on_line_disp_fio_textChanged("");
    on_line_doc_org_textChanged("");
    on_line_address_reg_textChanged("");
    on_line_address_liv_textChanged("");
    on_plain_req_petition_textChanged();
    on_line_req_comment_textChanged("");
    on_line_req_perform_textChanged("");
    on_line_req_performer_textChanged("");
    //on_plain_req_result_textChanged();

    if (data_app.is_call) {
        // правка интерфейса для телефонистов
        ui->bn_add_bybase->setText("Поск застрахованного \nпо базе полисов");
        ui->bn_add_byhand->setVisible(false);
        ui->bn_edit->setVisible(false);
        ui->bn_delete->setVisible(false);
    }
}

contacts_wnd::~contacts_wnd() {
    delete ui;
}

void contacts_wnd::on_bn_exit_clicked() {
    QApplication::closeAllWindows();
    QApplication::exit();
}

void contacts_wnd::refresh_combo_filials() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '('||id||')  '||filial_name "
                  "  from public.filials "
                  " order by id ; ";
    mySQL.exec(this, sql, QString("Список филиалов"), *query, true, db, data_app);
    ui->combo_filial_filter->clear();
    ui->combo_filial_filter->addItem(" - все филиалы - ", -1);
    ui->combo_filial->clear();
    while (query->next()) {
        ui->combo_filial_filter->addItem(query->value(1).toString(), query->value(0).toInt());
        ui->combo_filial->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    delete query;
    int nf = ui->combo_filial_filter->findData(data_app.id_filial);
    ui->combo_filial_filter->setCurrentIndex(nf);
    int n = ui->combo_filial->findData(data_app.id_filial);
    ui->combo_filial->setCurrentIndex(n);
    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::refresh_combo_sex() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  "  from public.spr_sex "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Список полов"), *query, true, db, data_app);
    ui->combo_sex->clear();
    while (query->next())
        ui->combo_sex->addItem(query->value(1).toString(), query->value(0).toInt());
    delete query;
    ui->combo_sex->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::refresh_combo_soc_status() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  "  from public.spr_soc_status "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Список соц. статусов"), *query, true, db, data_app);
    ui->combo_soc_status->clear();
    while (query->next())
        ui->combo_soc_status->addItem(query->value(1).toString(), query->value(0).toInt());
    delete query;
    ui->combo_soc_status->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::refresh_combo_doc_type() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  "  from public.spr_f011 "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Список типов документов"), *query, true, db, data_app);
    ui->combo_doc_type->clear();
    while (query->next())
        ui->combo_doc_type->addItem(query->value(1).toString(), query->value(0).toInt());
    delete query;
    ui->combo_doc_type->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::refresh_combo_talker_form() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  "  from public.spr_talker_form "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Список форм физ/юр лиц"), *query, true, db, data_app);
    ui->combo_talker_form->clear();
    while (query->next())
        ui->combo_talker_form->addItem(query->value(1).toString(), query->value(0).toInt());
    delete query;
    ui->combo_talker_form->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::refresh_combo_req_way() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  "  from public.spr_req_way "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Список способов обращшений"), *query, true, db, data_app);
    ui->combo_req_way->clear();
    while (query->next())
        ui->combo_req_way->addItem(query->value(1).toString(), query->value(0).toInt());
    delete query;
    ui->combo_req_way->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::refresh_combo_req_type() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select distinct code, '('||code||')  -  ' || req_type, id "
                  "  from public.spr_req_reason "
                  " order by id ; ";
    mySQL.exec(this, sql, QString("Список типов обращений"), *query, true, db, data_app);
    ui->combo_req_type->clear();
    ui->combo_req_type->addItem(" - не определено - ", "0");
    while (query->next())
        if (query->value(0).toString().length()==1) {
            QString req_type = query->value(0).toString().left(1);
            QString req_text = query->value(1).toString();
            ui->combo_req_type->addItem(req_text, req_type);
        }
    delete query;
    ui->combo_req_type->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);

    refresh_combo_req_reason();
}

void contacts_wnd::refresh_combo_req_reason() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '('||code||')  -  ' || req_reason, id "
                  "  from public.spr_req_reason "
                  " where code like('" + ui->combo_req_type->currentData().toString() + "%') "
                  "   and code<>'" + ui->combo_req_type->currentData().toString() + "%' "
                  " order by id ; ";
    mySQL.exec(this, sql, QString("Список причин обращений"), *query, true, db, data_app);
    ui->combo_req_reason->clear();
    ui->combo_req_reason->addItem(" - не определено - ", "");
    while (query->next())
        if (query->value(0).toString().length()>1)
            ui->combo_req_reason->addItem(query->value(1).toString(), query->value(0).toString());
    delete query;
    ui->combo_req_reason->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::refresh_combo_req_result() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '('||code||')  -  ' || text "
                  "  from public.spr_req_result "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Список результатов обращений"), *query, true, db, data_app);
    ui->combo_req_result->clear();
    ui->combo_req_result->addItem(" - не определено - ", "0. 0");
    while (query->next())
        ui->combo_req_result->addItem(query->value(1).toString(), query->value(0).toString());
    delete query;
    ui->combo_req_result->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::refresh_combo_req_status() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  "  from public.spr_req_status "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Список способов обращений"), *query, true, db, data_app);
    ui->combo_req_status->clear();
    while (query->next())
        ui->combo_req_status->addItem(query->value(1).toString(), query->value(0).toInt());
    delete query;
    ui->combo_req_status->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}


// КЛАДР места события
void contacts_wnd::refresh_kladr(int subj, int dstr, int city, int nasp, int strt) {
    // регион регистрации
    if (subj>=0) {
        this->setCursor(Qt::WaitCursor);
        // обновление выпадающего списка регионов
        QSqlQuery *query = new QSqlQuery(db);
        QString sq_ = "select subj, name||' '||socr, RIGHT('00000000000'||TRIM(ocatd),11), code, trim(index) "
                      "  from public.spr_kladr_2016_06_24 s "
                      " where s.subj<>0 and s.dstr=0 and s.city=0 and s.nasp=0 /*and s.actual=0*/ "
                      " order by s.name ; ";
        mySQL.exec(this, sq_, QString("КЛАДР регионов"), *query, true, db, data_app);
        kladr_subj.clear();
        kladr_subj.append("");
        ui->combo_kladr_subj->clear();
        ui->combo_kladr_subj->addItem(" - нет - ", 0);
        while (query->next()) {
            kladr_subj.append(query->value(3).toString());
            ui->combo_kladr_subj->addItem(query->value(1).toString(), query->value(0).toInt());
        }
        delete query;
        this->setCursor(Qt::ArrowCursor);
        // зададим текущее значение
        if (subj>0) ui->combo_kladr_subj->setCurrentIndex(ui->combo_kladr_subj->findData(subj));
        else        ui->combo_kladr_subj->setCurrentIndex(0);
    }

    //район регистрации
    if (dstr>=0) {
        this->setCursor(Qt::WaitCursor);
        // обновление выпадающего списка районов
        QSqlQuery *query = new QSqlQuery(db);
        QString sql = "select dstr, name||' '||socr, RIGHT('00000000000'||TRIM(ocatd),11), code, trim(index) "
                      "  from public.spr_kladr s "
                      " where s.subj=" + QString::number(ui->combo_kladr_subj->currentData().toInt()) +
                      "       and s.dstr<>0 and s.city=0 and s.nasp=0 /*and s.actual=0*/ "
                      " order by s.name ; ";
        mySQL.exec(this, sql, QString("КЛАДР районов"), *query, true, db, data_app);
        kladr_dstr.clear();
        kladr_dstr.append("");
        ui->combo_kladr_dstr->clear();
        ui->combo_kladr_dstr->addItem(" - нет - ", 0);
        while (query->next()) {
            kladr_dstr.append(query->value(3).toString());
            ui->combo_kladr_dstr->addItem(query->value(1).toString(), query->value(0).toInt());
        }
        delete query;
        this->setCursor(Qt::ArrowCursor);
        // зададим текущее значение
        if (dstr>0) ui->combo_kladr_dstr->setCurrentIndex(ui->combo_kladr_dstr->findData(dstr));
        else        ui->combo_kladr_dstr->setCurrentIndex(0);
    }

    // город регистрации
    if (city>=0) {
        this->setCursor(Qt::WaitCursor);
        // обновление выпадающего списка городов
        QSqlQuery *query = new QSqlQuery(db);
        QString sql = "select city, name||' '||socr, RIGHT('00000000000'||TRIM(ocatd),11), code, trim(index) "
                      "  from public.spr_kladr s "
                      " where s.subj=" + QString::number(ui->combo_kladr_subj->currentData().toInt()) +
                      "       and s.dstr=" + QString::number(ui->combo_kladr_dstr->currentData().toInt()) +
                      "       and s.city<>0 and s.nasp=0 /*and s.actual=0*/ "
                      " order by s.name ; ";
        mySQL.exec(this, sql, QString("КЛАДР городов"), *query, true, db, data_app);
        kladr_city.clear();
        kladr_city.append("");
        ui->combo_kladr_city->clear();
        ui->combo_kladr_city->addItem(" - нет - ", 0);
        while (query->next()) {
            kladr_city.append(query->value(3).toString());
            ui->combo_kladr_city->addItem(query->value(1).toString(), query->value(0).toInt());
        }
        delete query;
        this->setCursor(Qt::ArrowCursor);
        // зададим текущее значение
        if (city>0) ui->combo_kladr_city->setCurrentIndex(ui->combo_kladr_city->findData(city));
        else        ui->combo_kladr_city->setCurrentIndex(0);
    }

    // нас.пункт регистрации
    if (nasp>=0) {
        this->setCursor(Qt::WaitCursor);
        // обновление выпадающего списка нас.пунктов
        QString sql = "select nasp, name||' '||socr, RIGHT('00000000000'||TRIM(ocatd),11), code, trim(index) "
                      "  from public.spr_kladr s "
                      " where s.subj=" + QString::number(ui->combo_kladr_subj->currentData().toInt()) +
                      "       and s.dstr=" + QString::number(ui->combo_kladr_dstr->currentData().toInt()) +
                      "       and s.city=" + QString::number(ui->combo_kladr_city->currentData().toInt()) +
                      "       and s.nasp<>0 /*and s.actual=0*/ "
                      " order by s.name ; ";
        QSqlQuery *query = new QSqlQuery(db);
        mySQL.exec(this, sql, QString("КЛАДР нас.пунктов"), *query, true, db, data_app);
        kladr_nasp.clear();
        kladr_nasp.append("");
        ui->combo_kladr_nasp->clear();
        ui->combo_kladr_nasp->addItem(" - нет - ", 0);
        while (query->next()) {
            kladr_nasp.append(query->value(3).toString());
            ui->combo_kladr_nasp->addItem(query->value(1).toString(), query->value(0).toInt());
        }
        this->setCursor(Qt::ArrowCursor);
        // зададим текущее значение
        if (nasp>0) ui->combo_kladr_nasp->setCurrentIndex(ui->combo_kladr_nasp->findData(nasp));
        else        ui->combo_kladr_nasp->setCurrentIndex(0);
        delete query;
    }

    // улица регистрации
    if (strt>=0) {
        this->setCursor(Qt::WaitCursor);
        // обновление выпадающего списка улиц
        QSqlQuery *query = new QSqlQuery(db);
        QString sql = "select strt, name||' '||socr, RIGHT('00000000000'||TRIM(ocatd),11), code, trim(index) "
                      "  from public.spr_kladr_street s "
                      " where s.subj=" + QString::number(ui->combo_kladr_subj->currentData().toInt()) +
                      "       and s.dstr=" + QString::number(ui->combo_kladr_dstr->currentData().toInt()) +
                      "       and s.city=" + QString::number(ui->combo_kladr_city->currentData().toInt()) +
                      "       and s.nasp=" + QString::number(ui->combo_kladr_nasp->currentData().toInt()) +
                      "       and s.strt<>0 /*and s.actual=0*/ "
                      " order by s.name ; ";
        mySQL.exec(this, sql, QString("КЛАДР улиц"), *query, true, db, data_app);
        kladr_strt.clear();
        kladr_strt.append("");
        ui->combo_kladr_strt->clear();
        ui->combo_kladr_strt->addItem(" - нет - ", 0);
        while (query->next()) {
            kladr_strt.append(query->value(3).toString());
            ui->combo_kladr_strt->addItem(query->value(1).toString(), query->value(0).toInt());
        }
        delete query;
        this->setCursor(Qt::ArrowCursor);
        // зададим текущее значение
        int i = ui->combo_kladr_strt->findData(strt,Qt::UserRole,Qt::MatchExactly);
        //int i = ui->combo_kladr_strt->findData(strt,Qt::UserRole,Qt::MatchExactly);
        QString str = ui->combo_kladr_strt->itemText(i);
        if (strt>0) ui->combo_kladr_strt->setCurrentIndex(i);
        else        ui->combo_kladr_strt->setCurrentIndex(0);
    }
}

void contacts_wnd::on_bn_hide_clicked() {
    if (ui->pan_contact->isEnabled()) {
        if ( QMessageBox::question(this, "Отказаться от ввода данных?",
                                   "Вы действительно хотите отказаться от ввода данных?",
                                   QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No )
            return;
    }
    ui->pan_tab->setEnabled(true);
    ui->pan_tab->setVisible(true);
    ui->pan_contact->setEnabled(false);
    ui->pan_contact->setVisible(false);
}

void contacts_wnd::refresh_contacts_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    ui->lab_info->setText(" ");

    QString contacts_sql = "";
    contacts_sql += "select c.n_irp, c.id_filial, f.filial_name, c.id_operator, c.disp_fio, c.id_person, c.id_doc, c.id_polis, "
                    "       c.fam, c.im, c.ot, c.sex, c.year_birth, c.date_birth, c.doc_type, c.doc_ser, c.doc_sernom, c.doc_date, c.doc_org, c.pol_ser, c.pol_sernom, c.pol_enp, "
                    "       c.address_reg, c.address_liv, c.phone, c.email, c.soc_status, ss.text as soc_status_text, "
                    "       c.talker_form, c.req_reason_code, rr.req_type, rr.req_reason, c.req_dt, c.req_petition, c.req_comment, "
                    "       c.req_kladr_subj, ks.name || ' ' || ks.socr as req_subj, "
                    "       c.req_kladr_dstr, kd.name || ' ' || kd.socr as req_dstr, "
                    "       c.req_kladr_city, kc.name || ' ' || kc.socr as req_city, "
                    "       c.req_kladr_nasp, kn.name || ' ' || kn.socr as req_nasp, "
                    "       c.req_kladr_strt, ku.name || ' ' || ku.socr as req_strt, c.req_location, "
                    "       c.req_way, rw.text as req_way_text, c.req_date_incident, c.req_perform, c.req_performer, "
                    "       c.req_status, rs.text as req_status_text, c.req_date_control, c.req_date_done, c.req_result, "
                    "       cast(dt_ins as date) as date_ins, cast(dt_upd as date) as date_upd, "
                    "       check_efficiency, c.id, c.req_result_code, "
                    "       case c.req_result_code "
                    "        when 1 then 'дана консультация' "
                    "        when 2 then 'рассмотрено обращение' "
                    "        when 3 then 'заявление удовлетворено' "
                    "        when 4 then 'заявление не удовлетворено' "
                    "        when 5 then 'рассмотрена жалоба' "
                    "        when 6 then 'звонок переадресован' "
                    "        when 7 then 'обращение переадресовано в другую организацию' end as result, "
                    "       c.snils, c.pol_vs_num ";
    contacts_sql += "  from contact_center c "
                    "       left join filials f on(f.id=c.id_filial) "
                    "       left join spr_soc_status ss on(ss.code=c.soc_status) "
                    "       left join spr_talker_form tf on(tf.code=c.talker_form) "
                    "       left join spr_kladr ks on(ks.code=(right('00'||c.req_kladr_subj,2) || '000000000') ) "
                    "       left join spr_kladr kd on(kd.code=(right('00'||c.req_kladr_subj,2) || right('000'||c.req_kladr_dstr,3) || '000000') ) "
                    "       left join spr_kladr kc on(kc.code=(right('00'||c.req_kladr_subj,2) || right('000'||c.req_kladr_dstr,3) || right('000'||c.req_kladr_city,3) || '000') ) "
                    "       left join spr_kladr kn on(kn.code=(right('00'||c.req_kladr_subj,2) || right('000'||c.req_kladr_dstr,3) || right('000'||c.req_kladr_city,3) || right('000'||c.req_kladr_nasp,3)) ) "
                    "       left join spr_kladr_street ku on(ku.code=(right('00'||c.req_kladr_subj,2) || right('000'||c.req_kladr_dstr,3) || right('000'||c.req_kladr_city,3) || right('000'||c.req_kladr_nasp,3) || right('0000'||c.req_kladr_strt,4)) ) "
                    "       left join spr_req_reason rr on(rr.code=c.req_reason_code) "
                    "       left join spr_req_way rw on(rw.code=c.req_way) "
                    "       left join spr_req_status rs on(rs.code=c.req_status) ";
    contacts_sql += " where ((c.fam is NULL) or (c.fam like('" + ui->line_fam_filter->text().trimmed().toUpper() + "%') )) "
                    "   and ((c.im  is NULL) or (c.im  like('" + ui->line_im_filter->text().trimmed().toUpper()  + "%') )) "
                    "   and ((c.ot  is NULL) or (c.ot  like('" + ui->line_ot_filter->text().trimmed().toUpper()  + "%') )) ";

    if (!ui->line_snils_filter->text().trimmed().isEmpty())
        contacts_sql += "   and c.snils like('" + ui->line_snils_filter->text().trimmed() + "%') ";
    if (!ui->line_enp_filter->text().trimmed().isEmpty())
        contacts_sql += "   and c.pol_enp like('" + ui->line_enp_filter->text().trimmed() + "%') ";
    if (!ui->line_vs_filter->text().trimmed().isEmpty())
        contacts_sql += "   and c.pol_vs_num like('" + ui->line_vs_filter->text().trimmed() + "%') ";

    if (ui->combo_filial_filter->currentIndex()>0)
        contacts_sql += "   and c.id_filial=" + QString::number(ui->combo_filial_filter->currentData().toInt()) + " ";
    if (ui->ch_new_lines->isChecked())
        contacts_sql += "   and ((c.status is NUll) or (c.status=0)) ";

    contacts_sql += " order by c.dt_ins DESC ; ";

    model_contacts.setQuery(contacts_sql,db);
    QString err2 = model_contacts.lastError().driverText();

    // подключаем модель из БД
    ui->tab_contacts->setModel(&model_contacts);

    // обновляем таблицу
    ui->tab_contacts->reset();

    // задаём ширину колонок
    ui->tab_contacts->setColumnWidth( 0,140); // n_irp
    ui->tab_contacts->setColumnWidth( 1,  1); // c.id_filial
    ui->tab_contacts->setColumnWidth( 2,  1); // f.filial_name
    ui->tab_contacts->setColumnWidth( 3,  1); // c.id_operator
    ui->tab_contacts->setColumnWidth( 4,100); // c.disp_fio
    ui->tab_contacts->setColumnWidth( 5,  1); // c.id_person
    ui->tab_contacts->setColumnWidth( 6,  1); // c.id_doc
    ui->tab_contacts->setColumnWidth( 7,  1); // c.id_polis
    ui->tab_contacts->setColumnWidth( 8,120); // c.fam
    ui->tab_contacts->setColumnWidth( 9,120); // c.im
    ui->tab_contacts->setColumnWidth(10,120); // c.ot
    ui->tab_contacts->setColumnWidth(11, 40); // c.sex
    ui->tab_contacts->setColumnWidth(12, 40); // c.year_birth
    ui->tab_contacts->setColumnWidth(13, 70); // c.date_birth
    ui->tab_contacts->setColumnWidth(14,  1); // c.doc_type
    ui->tab_contacts->setColumnWidth(15, 40); // c.doc_ser
    ui->tab_contacts->setColumnWidth(16, 80); // c.doc_sernom
    ui->tab_contacts->setColumnWidth(17, 70); // c.doc_date
    ui->tab_contacts->setColumnWidth(18,100); // c.doc_org
    ui->tab_contacts->setColumnWidth(19, 40); // c.pol_ser
    ui->tab_contacts->setColumnWidth(20, 80); // c.pol_sernom
    ui->tab_contacts->setColumnWidth(21,100); // c.pol_enp
    ui->tab_contacts->setColumnWidth(22,  1); // c.address_reg
    ui->tab_contacts->setColumnWidth(23,  1); // c.address_liv
    ui->tab_contacts->setColumnWidth(24,100); // c.phone
    ui->tab_contacts->setColumnWidth(25,  1); // c.email
    ui->tab_contacts->setColumnWidth(26,  1); // c.soc_status
    ui->tab_contacts->setColumnWidth(27,100); // ss.text as soc_ststus_text
    ui->tab_contacts->setColumnWidth(28, 70); // c.talker_form
    ui->tab_contacts->setColumnWidth(29,  1); // c.req_reason_code
    ui->tab_contacts->setColumnWidth(30,100); // rr.req_type
    ui->tab_contacts->setColumnWidth(31,150); // rr.req_reason
    ui->tab_contacts->setColumnWidth(32, 80); // c.req_dt
    ui->tab_contacts->setColumnWidth(33,200); // c.req_petition
    ui->tab_contacts->setColumnWidth(34,100); // c.req_comment

    ui->tab_contacts->setColumnWidth(35,  1); // c.req_region
    ui->tab_contacts->setColumnWidth(36, 50); // rg.text as req_region_text
    ui->tab_contacts->setColumnWidth(37,  1); // c.req_region
    ui->tab_contacts->setColumnWidth(38,100); // rg.text as req_region_text
    ui->tab_contacts->setColumnWidth(39,  1); // c.req_region
    ui->tab_contacts->setColumnWidth(40, 50); // rg.text as req_region_text
    ui->tab_contacts->setColumnWidth(41,  1); // c.req_region
    ui->tab_contacts->setColumnWidth(42, 50); // rg.text as req_region_text
    ui->tab_contacts->setColumnWidth(43,  1); // c.req_region
    ui->tab_contacts->setColumnWidth(44,100); // rg.text as req_region_text

    ui->tab_contacts->setColumnWidth(45,100); // c.req_location
    ui->tab_contacts->setColumnWidth(46,  1); // c.req_way
    ui->tab_contacts->setColumnWidth(47, 80); // rw.text as req_way_text
    ui->tab_contacts->setColumnWidth(48, 70); // c.req_date_incident
    ui->tab_contacts->setColumnWidth(49,150); // c.req_perform
    ui->tab_contacts->setColumnWidth(50,100); // c.req_performer
    ui->tab_contacts->setColumnWidth(51,  1); // c.req_status
    ui->tab_contacts->setColumnWidth(52, 80); // rs.text as req_status_text
    ui->tab_contacts->setColumnWidth(53, 90); // c.req_date_control
    ui->tab_contacts->setColumnWidth(54, 90); // c.req_date_done
    ui->tab_contacts->setColumnWidth(55,200); // c.req_result

    ui->tab_contacts->setColumnWidth(56, 80); // date_ins
    ui->tab_contacts->setColumnWidth(57, 80); // date_upd

    ui->tab_contacts->setColumnWidth(59,  2); // check_efffectivity
    ui->tab_contacts->setColumnWidth(60,  2); // result_code
    ui->tab_contacts->setColumnWidth(61,100); // result_code

    ui->tab_contacts->setColumnWidth(62,  2); // snils
    ui->tab_contacts->setColumnWidth(63,  2); // pol_vs_num

    // правим заголовки
    model_contacts.setHeaderData( 0, Qt::Horizontal, ("№№"));                       // id == req_num
    model_contacts.setHeaderData( 4, Qt::Horizontal, ("диспетчер"));                // c.disp_fio
    model_contacts.setHeaderData( 8, Qt::Horizontal, ("фамилия"));                  // c.fam
    model_contacts.setHeaderData( 9, Qt::Horizontal, ("имя"));                      // c.im
    model_contacts.setHeaderData(10, Qt::Horizontal, ("отчество"));                 // c.ot
    model_contacts.setHeaderData(11, Qt::Horizontal, ("пол"));                      // c.sex
    model_contacts.setHeaderData(12, Qt::Horizontal, ("год\nрожд."));               // c.year_birth
    model_contacts.setHeaderData(13, Qt::Horizontal, ("дата\nрождения"));           // c.date_birth
    model_contacts.setHeaderData(14, Qt::Horizontal, ("тип документа"));            // c.doc_sernom
    model_contacts.setHeaderData(15, Qt::Horizontal, ("серия"));                    // c.doc_sernom
    model_contacts.setHeaderData(16, Qt::Horizontal, ("номер док."));               // c.doc_sernom
    model_contacts.setHeaderData(17, Qt::Horizontal, ("дата\nдокумента"));          // c.doc_date
    model_contacts.setHeaderData(18, Qt::Horizontal, ("выдающя\nорганизация"));     // c.doc_org
    model_contacts.setHeaderData(19, Qt::Horizontal, ("серия"));                    // c.pol_ser
    model_contacts.setHeaderData(20, Qt::Horizontal, ("полис"));                    // c.pol_sernom
    model_contacts.setHeaderData(21, Qt::Horizontal, ("ЕНП"));                      // c.pol_enp
    model_contacts.setHeaderData(22, Qt::Horizontal, ("адрес регистрации"));        // c.address_reg
    model_contacts.setHeaderData(23, Qt::Horizontal, ("адрес проживания"));         // c.address_liv
    model_contacts.setHeaderData(24, Qt::Horizontal, ("телефон"));                  // c.phone
    model_contacts.setHeaderData(25, Qt::Horizontal, ("e-mail"));                   // c.email
    model_contacts.setHeaderData(27, Qt::Horizontal, ("соц. статус"));              // ss.text as soc_ststus_text
    model_contacts.setHeaderData(28, Qt::Horizontal, ("физ/юр\nлицо"));             // c.talker_form
    model_contacts.setHeaderData(29, Qt::Horizontal, ("код\nобращения"));           // c.req_reason_code
    model_contacts.setHeaderData(30, Qt::Horizontal, ("тип\nобращения"));           // rr.req_type
    model_contacts.setHeaderData(31, Qt::Horizontal, ("причина\nобращения"));       // rr.req_reason
    model_contacts.setHeaderData(32, Qt::Horizontal, ("дата\nобращения"));          // c.req_dt
    model_contacts.setHeaderData(33, Qt::Horizontal, ("текст обращения"));          // c.req_petition
    model_contacts.setHeaderData(34, Qt::Horizontal, ("служебный\nкомментарий"));   // c.req_comment
    model_contacts.setHeaderData(36, Qt::Horizontal, ("регион"));                   // rg.text as req_region_text
    model_contacts.setHeaderData(38, Qt::Horizontal, ("район"));                    // rg.text as req_region_text
    model_contacts.setHeaderData(40, Qt::Horizontal, ("город"));                    // rg.text as req_region_text
    model_contacts.setHeaderData(42, Qt::Horizontal, ("нас.пункт"));                // rg.text as req_region_text
    model_contacts.setHeaderData(44, Qt::Horizontal, ("улица"));                    // rg.text as req_region_text
    model_contacts.setHeaderData(45, Qt::Horizontal, ("место\nсобытия"));           // c.req_location
    model_contacts.setHeaderData(47, Qt::Horizontal, ("способ\nобращения"));        // rw.text as req_way_text
    model_contacts.setHeaderData(48, Qt::Horizontal, ("дата\nсобытия"));            // c.req_date_incident
    model_contacts.setHeaderData(49, Qt::Horizontal, ("поручение"));                // c.req_perform
    model_contacts.setHeaderData(50, Qt::Horizontal, ("кому поручено"));            // c.req_performer
    model_contacts.setHeaderData(52, Qt::Horizontal, ("статус"));                   // rs.text as req_status_text
    model_contacts.setHeaderData(53, Qt::Horizontal, ("контрольная\nдата"));        // c.req_date_control
    model_contacts.setHeaderData(54, Qt::Horizontal, ("дата\nвыполнения"));         // c.req_date_done
    model_contacts.setHeaderData(55, Qt::Horizontal, ("результат"));                // c.req_result

    model_contacts.setHeaderData(56, Qt::Horizontal, ("дата\nвставки"));            // date_ins
    model_contacts.setHeaderData(57, Qt::Horizontal, ("дата\nправки"));             // date_upd

    model_contacts.setHeaderData(58, Qt::Horizontal, ("эффективность"));            // check_efffectivity
    model_contacts.setHeaderData(61, Qt::Horizontal, ("результат"));                // result

    model_contacts.setHeaderData(62, Qt::Horizontal, ("СНИЛС"));                    //
    model_contacts.setHeaderData(63, Qt::Horizontal, ("№ ВС"));                     //
    ui->tab_contacts->repaint();

    //data_assig.id = -1;
    /*data_assig.code_mo = "";
    //data_assig.name_mo = "";
    data_assig.code_mp = "";
    data_assig.code_ms = "";
    data_assig.code_mt = "";*/

    /*refresh_head_mdorgs();

    ui->ln_ocato->setText("");
    ui->ln_code_mo->setText("");
    ui->ln_oid_mo->setText("");
    ui->ln_name_mo->setText("");
    ui->ln_name_mo_full->setText("");
    ui->ln_address->setText("");
    ui->ch_is_head->setChecked(true);
    ui->combo_head_mo->setCurrentIndex(-1);
    ui->ln_phone->setText("");
    ui->ln_fax->setText("");
    ui->ln_email->setText("");
    ui->ln_www->setText("");
    ui->ln_comment->setText("");*/

    //ui->bn_ok->setEnabled(true);

    this->setCursor(Qt::ArrowCursor);
}


void contacts_wnd::on_bn_refresh_clicked() {
    refresh_contacts_tab();
}

void contacts_wnd::on_combo_filial_filter_activated(int index) {
    data_app.id_filial = ui->combo_filial_filter->currentData().toInt();
    refresh_contacts_tab();
}

void contacts_wnd::on_line_fam_filter_editingFinished() {
    refresh_contacts_tab();
}

void contacts_wnd::on_line_im_filter_editingFinished() {
    refresh_contacts_tab();
}

void contacts_wnd::on_line_ot_filter_editingFinished() {
    refresh_contacts_tab();
}

void contacts_wnd::on_ch_new_lines_clicked(bool checked) {
    refresh_contacts_tab();
}

void contacts_wnd::on_bn_clear_filter_clicked() {
    ui->line_fam_filter->clear();
    ui->line_im_filter->clear();
    ui->line_ot_filter->clear();

    ui->line_snils_filter->clear();
    ui->line_enp_filter->clear();
    ui->line_vs_filter->clear();

    refresh_contacts_tab();
}

void contacts_wnd::on_tab_contacts_clicked(const QModelIndex &index) {    
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_contacts || !ui->tab_contacts->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!", "Таблица не загружена!\nНажмите кнопку [Обновить] и выберите строку таблицы.");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    if (index.isValid()) {
        ui->lab_info->setText("VVV  Просмотр данных обращения  VVV");
        ui->pan_contact->setVisible(true);
        
        // выберем данные из таблицы
        data_contact.id              = model_contacts.data(model_contacts.index(index.row(),59), Qt::EditRole).toInt();     // c.id
        data_contact.n_irp           = model_contacts.data(model_contacts.index(index.row(), 0), Qt::EditRole).toString();  // c.n_irp
        data_contact.id_filial       = model_contacts.data(model_contacts.index(index.row(), 1), Qt::EditRole).toInt();     // f.id_filial
        data_contact.id_point        = model_contacts.data(model_contacts.index(index.row(), 2), Qt::EditRole).toInt();     // f.id_filial
        data_contact.id_operator     = model_contacts.data(model_contacts.index(index.row(), 3), Qt::EditRole).toInt();     // c.id_operator
        data_contact.disp_fio        = model_contacts.data(model_contacts.index(index.row(), 4), Qt::EditRole).toString();  // c.disp_fio
        data_contact.id_person       = model_contacts.data(model_contacts.index(index.row(), 5), Qt::EditRole).toInt();     // c.id_person
        data_contact.id_doc          = model_contacts.data(model_contacts.index(index.row(), 6), Qt::EditRole).toInt();     // c.id_doc
        data_contact.id_polis        = model_contacts.data(model_contacts.index(index.row(), 7), Qt::EditRole).toInt();     // c.id_polis
        data_contact.fam             = model_contacts.data(model_contacts.index(index.row(), 8), Qt::EditRole).toString();  // c.fam
        data_contact.im              = model_contacts.data(model_contacts.index(index.row(), 9), Qt::EditRole).toString();  // c.im
        data_contact.ot              = model_contacts.data(model_contacts.index(index.row(),10), Qt::EditRole).toString();  // c.ot
        data_contact.sex             = model_contacts.data(model_contacts.index(index.row(),11), Qt::EditRole).toInt();     // c.sex
        data_contact.has_year_birth  =!model_contacts.data(model_contacts.index(index.row(),12), Qt::EditRole).isNull();
        data_contact.year_birth      = model_contacts.data(model_contacts.index(index.row(),12), Qt::EditRole).toInt();     // c.year_birth
        data_contact.has_date_birth  =!model_contacts.data(model_contacts.index(index.row(),13), Qt::EditRole).isNull();
        data_contact.date_birth      = model_contacts.data(model_contacts.index(index.row(),13), Qt::EditRole).toDate();    // c.date_birth
        data_contact.doc_type        = model_contacts.data(model_contacts.index(index.row(),14), Qt::EditRole).toInt();     // c.doc_type
        data_contact.doc_ser         = model_contacts.data(model_contacts.index(index.row(),15), Qt::EditRole).toString();  // c.doc_ser
        data_contact.doc_sernum      = model_contacts.data(model_contacts.index(index.row(),16), Qt::EditRole).toString();  // c.doc_sernum
        data_contact.has_date_doc    =!model_contacts.data(model_contacts.index(index.row(),17), Qt::EditRole).isNull();
        data_contact.doc_date        = model_contacts.data(model_contacts.index(index.row(),17), Qt::EditRole).toDate();    // c.doc_date
        data_contact.doc_org         = model_contacts.data(model_contacts.index(index.row(),18), Qt::EditRole).toString();  // c.doc_org
        data_contact.pol_ser         = model_contacts.data(model_contacts.index(index.row(),19), Qt::EditRole).toString();  // c.pol_ser
        data_contact.pol_sernum      = model_contacts.data(model_contacts.index(index.row(),20), Qt::EditRole).toString();  // c.pol_sernom
        data_contact.pol_enp         = model_contacts.data(model_contacts.index(index.row(),21), Qt::EditRole).toString();  // c.pol_enp
        data_contact.address_reg     = model_contacts.data(model_contacts.index(index.row(),22), Qt::EditRole).toString();  // c.address_reg
        data_contact.address_liv     = model_contacts.data(model_contacts.index(index.row(),23), Qt::EditRole).toString();  // c.address_liv
        data_contact.phone           = model_contacts.data(model_contacts.index(index.row(),24), Qt::EditRole).toString();  // c.phone
        data_contact.email           = model_contacts.data(model_contacts.index(index.row(),25), Qt::EditRole).toString();  // c.email
        data_contact.soc_status      = model_contacts.data(model_contacts.index(index.row(),26), Qt::EditRole).toInt();     // c.soc_status
        data_contact.talker_form     = model_contacts.data(model_contacts.index(index.row(),28), Qt::EditRole).toInt();     // c.talker_form
        data_contact.req_type_code   = model_contacts.data(model_contacts.index(index.row(),29), Qt::EditRole).toString().left(1);
        data_contact.req_reason_code = model_contacts.data(model_contacts.index(index.row(),29), Qt::EditRole).toString();  // c.req_reason_code
        data_contact.req_datetime    = model_contacts.data(model_contacts.index(index.row(),32), Qt::EditRole).toDateTime();// c.req_dt
        data_contact.req_petition    = model_contacts.data(model_contacts.index(index.row(),33), Qt::EditRole).toString();  // c.req_petition
        data_contact.req_comment     = model_contacts.data(model_contacts.index(index.row(),34), Qt::EditRole).toString();  // c.req_comment
        data_contact.req_kladr_subj  = model_contacts.data(model_contacts.index(index.row(),35), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_kladr_dstr  = model_contacts.data(model_contacts.index(index.row(),37), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_kladr_city  = model_contacts.data(model_contacts.index(index.row(),39), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_kladr_nasp  = model_contacts.data(model_contacts.index(index.row(),41), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_kladr_strt  = model_contacts.data(model_contacts.index(index.row(),43), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_location    = model_contacts.data(model_contacts.index(index.row(),45), Qt::EditRole).toString();  // c.req_location
        data_contact.req_way         = model_contacts.data(model_contacts.index(index.row(),46), Qt::EditRole).toInt();     // c.req_way
        data_contact.has_date_incid  =!model_contacts.data(model_contacts.index(index.row(),48), Qt::EditRole).isNull();
        data_contact.req_date_incid  = model_contacts.data(model_contacts.index(index.row(),48), Qt::EditRole).toDate();    // c.req_date_incident
        data_contact.req_perform     = model_contacts.data(model_contacts.index(index.row(),49), Qt::EditRole).toString();  // c.req_perform
        data_contact.req_performer   = model_contacts.data(model_contacts.index(index.row(),50), Qt::EditRole).toString();  // c.req_performer
        data_contact.req_status      = model_contacts.data(model_contacts.index(index.row(),51), Qt::EditRole).toInt();     // c.req_status
        data_contact.req_status_text = model_contacts.data(model_contacts.index(index.row(),52), Qt::EditRole).toString();  // c.req_status
        data_contact.has_date_contrl =!model_contacts.data(model_contacts.index(index.row(),53), Qt::EditRole).isNull();
        data_contact.req_date_contrl = model_contacts.data(model_contacts.index(index.row(),53), Qt::EditRole).toDate();    // c.req_date_control
        data_contact.has_date_done   =!model_contacts.data(model_contacts.index(index.row(),54), Qt::EditRole).isNull();
        data_contact.req_date_done   = model_contacts.data(model_contacts.index(index.row(),54), Qt::EditRole).toDate();    // c.req_date_done
        data_contact.req_result      = model_contacts.data(model_contacts.index(index.row(),55), Qt::EditRole).toString();  // c.req_result
        data_contact.check_efficiency=!(model_contacts.data(model_contacts.index(index.row(),58), Qt::EditRole).toInt()<0); // c.check_efficiency
        data_contact.req_result_code = model_contacts.data(model_contacts.index(index.row(),60), Qt::EditRole).toInt();     // c.req_result_code

        data_contact.snils = model_contacts.data(model_contacts.index(index.row(),62), Qt::EditRole).toString();     // c.snills
        data_contact.vs_num = model_contacts.data(model_contacts.index(index.row(),63), Qt::EditRole).toString();    // c.vs_num

        // отобразим данные на форме
        ui->lab_req_num->setText(data_contact.n_irp);

        ui->combo_filial->setCurrentIndex(ui->combo_filial->findData(data_contact.id_filial));                       // c.id_filial
        ui->line_disp_fio->setText(data_contact.disp_fio);                  // c.disp_fio
        ui->line_fam->setText(data_contact.fam);                            // c.fam
        ui->line_im->setText(data_contact.im);                              // c.im
        ui->line_ot->setText(data_contact.ot);                              // c.ot
        ui->combo_sex->setCurrentIndex(ui->combo_sex->findData(data_contact.sex));                                   // c.sex
        ui->ch_year_birth->setChecked(data_contact.has_year_birth);
        ui->spin_year_birth->setValue(data_contact.year_birth);             // c.year_birth
        ui->ch_date_birth->setChecked(data_contact.has_date_birth);
        ui->date_birth->setEnabled(data_contact.has_date_birth);
        ui->date_birth->setDate(data_contact.has_date_birth ? data_contact.date_birth : QDate(1900,1,1));            // c.date_birth
        ui->combo_doc_type->setCurrentIndex(ui->combo_doc_type->findData(data_contact.doc_type));                    // c.doc_serno)m
        ui->line_doc_ser->setText(data_contact.doc_ser);                    // c.doc_ser
        ui->line_doc_sernum->setText(data_contact.doc_sernum);              // c.doc_sernom
        ui->ch_date_doc->setChecked(data_contact.has_date_doc);
        ui->date_doc->setEnabled(data_contact.has_date_doc);
        ui->date_doc->setDate(data_contact.has_date_doc ? data_contact.doc_date : QDate(1900,1,1));                  // c.doc_date
        ui->line_doc_org->setText(data_contact.doc_org);                    // c.doc_org
        ui->line_pol_sernum->setText(data_contact.pol_sernum);              // c.pol_sernom
        ui->line_pol_enp->setText(data_contact.pol_enp);                    // c.pol_enp
        ui->line_address_reg->setText(data_contact.address_reg);            // c.address_reg
        ui->line_address_liv->setText(data_contact.address_liv);            // c.address_liv
        ui->line_phone->setText(data_contact.phone);                        // c.phone
        ui->line_email->setText(data_contact.email);                        // c.email
        ui->combo_soc_status->setCurrentIndex(ui->combo_soc_status->findData(data_contact.soc_status));              // c.soc_status
        ui->combo_talker_form->setCurrentIndex(ui->combo_talker_form->findData(data_contact.talker_form));           // c.talker_form
        ui->combo_req_type->setCurrentIndex(ui->combo_req_type->findData(data_contact.req_type_code));
        ui->combo_req_reason->setCurrentIndex(ui->combo_req_reason->findData(data_contact.req_reason_code));         // c.req_reason_code
        ui->date_req->setDate(data_contact.req_datetime.date());            // c.req_dt
        ui->time_req->setTime(data_contact.req_datetime.time());            // c.req_dt
        ui->plain_req_petition->setPlainText(data_contact.req_petition);    // c.req_petition
        ui->line_req_comment->setText(data_contact.req_comment);            // c.req_comment
        ui->combo_kladr_subj->setCurrentIndex(ui->combo_kladr_subj->findData(data_contact.req_kladr_subj));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, 0, 0, 0, 0);
        ui->combo_kladr_dstr->setCurrentIndex(ui->combo_kladr_dstr->findData(data_contact.req_kladr_dstr));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, data_contact.req_kladr_dstr, 0, 0, 0);
        ui->combo_kladr_city->setCurrentIndex(ui->combo_kladr_city->findData(data_contact.req_kladr_city));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, data_contact.req_kladr_dstr, data_contact.req_kladr_city, 0, 0);
        ui->combo_kladr_nasp->setCurrentIndex(ui->combo_kladr_nasp->findData(data_contact.req_kladr_nasp));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, data_contact.req_kladr_dstr, data_contact.req_kladr_city, data_contact.req_kladr_nasp, 0);
        ui->combo_kladr_strt->setCurrentIndex(ui->combo_kladr_strt->findData(data_contact.req_kladr_strt));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, data_contact.req_kladr_dstr, data_contact.req_kladr_city, data_contact.req_kladr_nasp, data_contact.req_kladr_strt);
        ui->line_req_location->setText(data_contact.req_location);          // c.req_location
        ui->combo_req_way->setCurrentIndex(ui->combo_req_way->findData(data_contact.req_way));                       // c.req_way
        ui->ch_date_incident->setChecked(data_contact.has_date_incid);      // c.req_date_incident
        ui->date_incident->setEnabled(data_contact.has_date_incid);         // c.req_date_incident
        ui->date_incident->setDate(data_contact.has_date_incid ? data_contact.req_date_incid : QDate(1900,1,1));     // c.req_date_incident
        ui->line_req_perform->setText(data_contact.req_perform);            // c.req_perform
        ui->line_req_performer->setText(data_contact.req_performer);        // c.req_performer
        ui->combo_req_status->setCurrentIndex(ui->combo_req_status->findData(data_contact.req_status));              // c.req_status
        ui->ch_date_control->setChecked(data_contact.has_date_contrl);      // c.req_date_control
        ui->date_control->setEnabled(data_contact.has_date_contrl);         // c.req_date_control
        ui->date_control->setDate(data_contact.has_date_contrl ? data_contact.req_date_contrl : QDate(1900,1,1));    // c.req_date_control
        ui->ch_date_done->setChecked(data_contact.has_date_done);           // c.req_date_done
        ui->date_done->setEnabled(data_contact.has_date_done);              // c.req_date_done
        ui->date_done->setDate(data_contact.has_date_done ? data_contact.req_date_done : QDate(1900,1,1));           // c.req_date_done
        //ui->plain_req_result->setPlainText(data_contact.req_result);      // c.req_result
        ui->combo_req_result->setCurrentIndex(ui->combo_req_result->findData(data_contact.req_result_code));         // c.req_reason_code
        ui->ch_efficiency->setChecked(!data_contact.check_efficiency);

        ui->lab_status->setText(data_contact.req_status_text);
        ui->bn_status_2->setEnabled(data_contact.req_status==0);

        ui->pan_contact->setEnabled(false);
        ui->pan_contact->setVisible(true);

    } else {
        ui->lab_status->setText("-//-");
        ui->bn_status_2->setEnabled(false);

        ui->pan_contact->setEnabled(false);
        ui->pan_contact->setVisible(false);
        ui->lab_req_num->setText("-//-");
    }

    this->setCursor(Qt::ArrowCursor);
}


void contacts_wnd::on_ch_year_birth_clicked(bool checked) {
    ui->spin_year_birth->setEnabled(checked);
    if (checked) {
        ui->ch_date_birth->setChecked(false);
        on_ch_date_birth_clicked(false);
    }
}
void contacts_wnd::on_ch_date_birth_clicked(bool checked) {
    ui->date_birth->setEnabled(checked);
}
void contacts_wnd::on_ch_date_doc_clicked(bool checked) {
    ui->date_doc->setEnabled(checked);
}
void contacts_wnd::on_ch_date_incident_clicked(bool checked) {
    ui->date_incident->setEnabled(checked);
}
void contacts_wnd::on_ch_date_control_clicked(bool checked) {
    ui->date_control->setEnabled(checked);
}
void contacts_wnd::on_ch_date_done_clicked(bool checked) {
    ui->date_done->setEnabled(checked);
}

void contacts_wnd::on_bn_add_bybase_clicked() {
    // выберем данные из таблицы
    data_contact.id              = -1;                  // c.id
    data_contact.n_irp           = "";                  // c.n_irp
    data_contact.id_filial       = data_app.id_filial;  // f.id_filial
    data_contact.id_point        = data_app.id_point;   // f.id_point
    data_contact.id_operator     = data_app.id_operator;// c.id_operator
    data_contact.disp_fio        = data_app.operator_fio;// c.disp_fio
    data_contact.id_person       = -1;                  // c.id_person
    data_contact.id_doc          = -1;                  // c.id_doc
    data_contact.id_polis        = -1;                  // c.id_polis
    data_contact.fam             = "";                  // c.fam
    data_contact.im              = "";                  // c.im
    data_contact.ot              = "";                  // c.ot
    data_contact.sex             = 0;                   // c.sex
    data_contact.year_birth             = 0;                   // c.year_birth
    data_contact.has_date_birth  = false;
    data_contact.date_birth      = QDate(1900,1,1);     // c.date_birth
    data_contact.doc_type        = -1;                  // c.doc_type
    data_contact.doc_ser         = "";                  // c.doc_ser
    data_contact.doc_sernum      = "";                  // c.doc_sernum
    data_contact.has_date_doc    = false;
    data_contact.doc_date        = QDate(1900,1,1);     // c.doc_date
    data_contact.doc_org         = "";                  // c.doc_org
    data_contact.pol_ser         = "";                  // c.pol_ser
    data_contact.pol_sernum      = "";                  // c.pol_sernum
    data_contact.pol_enp         = "";                  // c.pol_enp
    data_contact.address_reg     = "";                  // c.address_reg
    data_contact.address_liv     = "";                  // c.address_liv
    data_contact.phone           = "";                  // c.phone
    data_contact.email           = "";                  // c.email
    data_contact.soc_status      = 0;                   // c.soc_status
    data_contact.talker_form     = 0;                   // c.talker_form
    data_contact.req_type_code   = "0";                 // c.req_reason_code
    data_contact.req_reason_code = "0. 0";              // c.req_reason_code
    data_contact.req_datetime    = QDateTime::currentDateTime();     // c.req_dt
    data_contact.req_petition    = "";                  // c.req_petition
    data_contact.req_comment     = "";                  // c.req_comment
    data_contact.req_kladr_subj  = 0;                   // c.req_region
    data_contact.req_kladr_dstr  = 0;                   // c.req_region
    data_contact.req_kladr_city  = 0;                   // c.req_region
    data_contact.req_kladr_nasp  = 0;                   // c.req_region
    data_contact.req_kladr_strt  = 0;                   // c.req_region
    data_contact.req_location    = "";                  // c.req_location
    data_contact.req_way         = 0;                   // c.req_way
    data_contact.has_date_incid  = false;
    data_contact.req_date_incid  = QDate(1900,1,1);     // c.req_date_incident
    data_contact.req_perform     = "";                  // c.req_perform
    data_contact.req_performer   = "";                  // c.req_performer
    data_contact.req_status      = 0;                   // c.req_status
    data_contact.has_date_contrl = false;
    data_contact.req_date_contrl = QDate(1900,1,1);     // c.req_date_control
    data_contact.has_date_done   = false;
    data_contact.req_date_done   = QDate(1900,1,1);     // c.req_date_done
    data_contact.req_result      = "";                  // c.req_result
    data_contact.req_result_code = -1;                  // c.req_result

    data_contact.snils           = "";                  // c.snils
    data_contact.vs_num          = "";                  // c.vs_num

    // запросим даные застрахованного во внешней базе
    outerdb_w = new outerdb_wnd(db, data_contact, data_app, settings, this);
    if (!outerdb_w->exec()) {
        if (!data_app.is_call) {
            QMessageBox::information( this, "Действие отменено",
                                     "Дейстие было отменено пользователем." );
        }
        return;
    }
    ui->pan_tab->setEnabled(false);
    ui->pan_tab->setVisible(true);
    ui->pan_contact->setVisible(true);
    ui->pan_contact->setEnabled(true);
    ui->lab_info->setText("VVV  Новое обращение  VVV");
    QApplication::processEvents();

    // отобразим данные на форме
    ui->lab_req_num->setText(data_contact.n_irp.isEmpty() ? "-//-" : data_contact.n_irp);

    ui->combo_filial->setCurrentIndex(ui->combo_filial->findData(data_contact.id_filial));      // c.id_filial
    ui->line_disp_fio->setText(data_contact.disp_fio);                  // c.disp_fio
    ui->line_fam->setText(data_contact.fam);                            // c.fam
    ui->line_im->setText(data_contact.im);                              // c.im
    ui->line_ot->setText(data_contact.ot);                              // c.ot
    ui->combo_sex->setCurrentIndex(ui->combo_sex->findData(data_contact.sex));      // c.sex
    ui->ch_year_birth->setChecked(data_contact.has_year_birth);
    ui->spin_year_birth->setValue(data_contact.year_birth);             // c.year_birth
    ui->ch_date_birth->setChecked(data_contact.has_date_birth);         // c.date_birth
    ui->date_birth->setEnabled(data_contact.has_date_birth);
    ui->date_birth->setDate(data_contact.date_birth);                   // c.date_birth
    ui->combo_doc_type->setCurrentIndex(ui->combo_doc_type->findData(data_contact.doc_type));   // c.doc_type
    ui->line_doc_ser->setText(data_contact.doc_ser);                    // c.doc_ser
    ui->line_doc_sernum->setText(data_contact.doc_sernum);              // c.doc_num
    ui->ch_date_doc->setChecked(data_contact.has_date_doc);             // c.doc_date
    ui->date_doc->setEnabled(data_contact.has_date_doc);                // c.doc_date
    ui->date_doc->setDate(data_contact.doc_date);                       // c.doc_date
    ui->line_doc_org->setText(data_contact.doc_org);                    // c.doc_org
    ui->line_pol_ser->setText(data_contact.pol_ser);                    // c.pol_ser
    ui->line_pol_sernum->setText(data_contact.pol_sernum);              // c.pol_num
    ui->line_pol_enp->setText(data_contact.pol_enp);                    // c.pol_enp
    ui->line_address_reg->setText(data_contact.address_reg);            // c.address_reg
    ui->line_address_liv->setText(data_contact.address_liv);            // c.address_liv
    ui->line_phone->setText(data_contact.phone);                        // c.phone
    ui->line_email->setText(data_contact.email);                        // c.email
    ui->combo_soc_status->setCurrentIndex(ui->combo_soc_status->findData(data_contact.soc_status));         // c.soc_status
    ui->combo_talker_form->setCurrentIndex(ui->combo_talker_form->findData(data_contact.talker_form));      // c.talker_form
    ui->combo_req_type->setCurrentIndex(ui->combo_req_type->findData(data_contact.req_type_code));
    ui->combo_req_reason->setCurrentIndex(ui->combo_req_reason->findData(data_contact.req_reason_code));    // c.req_reason_code
    ui->date_req->setDate(data_contact.req_datetime.date());            // c.req_dt
    ui->time_req->setTime(data_contact.req_datetime.time());            // c.req_dt
    ui->plain_req_petition->setPlainText(data_contact.req_petition);    // c.req_petition
    ui->line_req_comment->setText(data_contact.req_comment);            // c.req_comment
    //ui->combo_req_region->setCurrentIndex(ui->combo_req_region->findData(data_contact.req_region));         // c.req_region
    ui->line_req_location->setText(data_contact.req_location);          // c.req_location
    ui->combo_req_way->setCurrentIndex(ui->combo_req_way->findData(data_contact.req_way));                  // c.req_way
    ui->ch_date_incident->setChecked(data_contact.has_date_incid);      // c.req_date_incident
    ui->date_incident->setEnabled(data_contact.has_date_incid);         // c.req_date_incident
    ui->date_incident->setDate(data_contact.req_date_incid);            // c.req_date_incident
    ui->line_req_perform->setText(data_contact.req_perform);            // c.req_perform
    ui->line_req_performer->setText(data_contact.req_performer);        // c.req_performer
    ui->combo_req_status->setCurrentIndex(ui->combo_req_status->findData(data_contact.req_status));         // c.req_status
    ui->ch_date_control->setChecked(data_contact.has_date_contrl);      // c.req_date_control
    ui->date_control->setEnabled(data_contact.has_date_contrl);         // c.req_date_control
    ui->date_control->setDate(data_contact.req_date_contrl);            // c.req_date_control
    ui->ch_date_done->setChecked(data_contact.has_date_done);           // c.req_date_done
    ui->date_done->setEnabled(data_contact.has_date_done);              // c.req_date_done
    ui->date_done->setDate(data_contact.req_date_done);                 // c.req_date_done
    //ui->plain_req_result->setPlainText(data_contact.req_result);        // c.req_result
    ui->combo_req_result->setCurrentIndex(ui->combo_req_result->findData(data_contact.req_result_code));    // c.req_result_code
    ui->ch_efficiency->setChecked(!data_contact.check_efficiency);

    ui->lab_status->setText(data_contact.req_status_text);
    ui->bn_status_2->setEnabled(data_contact.req_status==0);

    ui->pan_contact->setEnabled(true);
}

void contacts_wnd::on_bn_add_byhand_clicked() {
    ui->pan_tab->setEnabled(false);
    ui->pan_tab->setVisible(true);

    ui->pan_contact->setVisible(true);
    ui->pan_contact->setEnabled(true);

    ui->lab_info->setText("VVV  Новое обращение  VVV");
    QApplication::processEvents();

    // выберем данные из таблицы
    data_contact.id              = -1;                  // c.id
    data_contact.n_irp           = "";                  // c.n_irp
    data_contact.id_filial       = data_app.id_filial;  // f.id_filial
    data_contact.id_point        = data_app.id_point;   // f.id_point
    data_contact.id_operator     = data_app.id_operator;// c.id_operator
    data_contact.disp_fio        = data_app.operator_fio;// c.disp_fio
    data_contact.id_person       = -1;                  // c.id_person
    data_contact.id_doc          = -1;                  // c.id_doc
    data_contact.id_polis        = -1;                  // c.id_polis
    data_contact.fam             = "АНОНИМНО";          // c.fam
    data_contact.im              = "";                  // c.im
    data_contact.ot              = "";                  // c.ot
    data_contact.sex             = 0;                   // c.sex
    data_contact.year_birth             = 0;                   // c.year_birth
    data_contact.has_date_birth  = false;
    data_contact.date_birth      = QDate(1900,1,1);     // c.date_birth
    data_contact.doc_type        = -1;                  // c.doc_type
    data_contact.doc_ser         = "";                  // c.doc_ser
    data_contact.doc_sernum      = "";                  // c.doc_num
    data_contact.has_date_doc    = false;
    data_contact.doc_date        = QDate(1900,1,1);     // c.doc_date
    data_contact.doc_org         = "";                  // c.doc_org
    data_contact.pol_ser         = "";                  // c.pol_ser
    data_contact.pol_sernum      = "";                  // c.pol_num
    data_contact.pol_enp         = "";                  // c.pol_enp
    data_contact.address_reg     = "";                  // c.address_reg
    data_contact.address_liv     = "";                  // c.address_liv
    data_contact.phone           = "";                  // c.phone
    data_contact.email           = "";                  // c.email
    data_contact.soc_status      = 0;                   // c.soc_status
    data_contact.talker_form     = 0;                   // c.talker_form
    data_contact.req_type_code   = "0";                 // c.req_reason_code
    data_contact.req_reason_code = "0. 0";              // c.req_reason_code
    data_contact.req_datetime    = QDateTime::currentDateTime();     // c.req_dt
    data_contact.req_petition    = "";                  // c.req_petition
    data_contact.req_comment     = "";                  // c.req_comment
    data_contact.req_kladr_subj  = 0;                   // c.req_region
    data_contact.req_kladr_dstr  = 0;                   // c.req_region
    data_contact.req_kladr_city  = 0;                   // c.req_region
    data_contact.req_kladr_nasp  = 0;                   // c.req_region
    data_contact.req_kladr_strt  = 0;                   // c.req_region
    data_contact.req_location    = "";                  // c.req_location
    data_contact.req_way         = 0;                   // c.req_way
    data_contact.has_date_incid  = false;
    data_contact.req_date_incid  = QDate(1900,1,1);     // c.req_date_incident
    data_contact.req_perform     = "";                  // c.req_perform
    data_contact.req_performer   = "";                  // c.req_performer
    data_contact.req_status      = 0;                   // c.req_status
    data_contact.has_date_contrl = false;
    data_contact.req_date_contrl  = QDate(1900,1,1);     // c.req_date_control
    data_contact.has_date_done   = false;
    data_contact.req_date_done   = QDate(1900,1,1);     // c.req_date_done
    data_contact.req_result      = "";                  // c.req_result
    data_contact.req_result_code = -1;                  // c.req_result_code

    data_contact.snils           = "";                  // c.snils
    data_contact.vs_num          = "";                  // c.vs_num

    // отобразим данные на форме
    //ui->lab_req_num->setText(data_contact.id>0 ? QString::number(data_contact.id) : "-//-");
    ui->lab_req_num->setText(data_contact.n_irp.isEmpty() ? "-//-" : data_contact.n_irp);

    ui->combo_filial->setCurrentIndex(ui->combo_filial->findData(data_contact.id_filial));      // c.id_filial
    ui->line_disp_fio->setText(data_contact.disp_fio);                  // c.disp_fio
    ui->line_fam->setText(data_contact.fam);                            // c.fam
    ui->line_im->setText(data_contact.im);                              // c.im
    ui->line_ot->setText(data_contact.ot);                              // c.ot
    ui->combo_sex->setCurrentIndex(ui->combo_sex->findData(data_contact.sex));      // c.sex
    ui->ch_year_birth->setChecked(data_contact.has_year_birth);
    ui->spin_year_birth->setValue(data_contact.year_birth);             // c.year_birth
    ui->ch_date_birth->setChecked(data_contact.has_date_birth);         // c.date_birth
    ui->date_birth->setEnabled(data_contact.has_date_birth);
    ui->date_birth->setDate(data_contact.date_birth);                   // c.date_birth
    ui->combo_doc_type->setCurrentIndex(ui->combo_doc_type->findData(data_contact.doc_type));   // c.doc_type
    ui->line_doc_ser->setText(data_contact.doc_ser);                    // c.doc_ser
    ui->line_doc_sernum->setText(data_contact.doc_sernum);              // c.doc_num
    ui->ch_date_doc->setChecked(data_contact.has_date_doc);             // c.doc_date
    ui->date_doc->setEnabled(data_contact.has_date_doc);                // c.doc_date
    ui->date_doc->setDate(data_contact.doc_date);                       // c.doc_date
    ui->line_doc_org->setText(data_contact.doc_org);                    // c.doc_org
    ui->line_pol_ser->setText(data_contact.pol_ser);                    // c.pol_ser
    ui->line_pol_sernum->setText(data_contact.pol_sernum);              // c.pol_num
    ui->line_pol_enp->setText(data_contact.pol_enp);                    // c.pol_enp
    ui->line_address_reg->setText(data_contact.address_reg);            // c.address_reg
    ui->line_address_liv->setText(data_contact.address_liv);            // c.address_liv
    ui->line_phone->setText(data_contact.phone);                        // c.phone
    ui->line_email->setText(data_contact.email);                        // c.email
    ui->combo_soc_status->setCurrentIndex(ui->combo_soc_status->findData(data_contact.soc_status));         // c.soc_status
    ui->combo_talker_form->setCurrentIndex(ui->combo_talker_form->findData(data_contact.talker_form));      // c.talker_form
    ui->combo_req_type->setCurrentIndex(ui->combo_req_type->findData(data_contact.req_type_code));
    ui->combo_req_reason->setCurrentIndex(ui->combo_req_reason->findData(data_contact.req_reason_code));    // c.req_reason_code
    ui->date_req->setDate(data_contact.req_datetime.date());            // c.req_dt
    ui->time_req->setTime(data_contact.req_datetime.time());            // c.req_dt
    ui->plain_req_petition->setPlainText(data_contact.req_petition);    // c.req_petition
    ui->line_req_comment->setText(data_contact.req_comment);            // c.req_comment
    //ui->combo_req_region->setCurrentIndex(ui->combo_req_region->findData(data_contact.req_region));         // c.req_region
    ui->line_req_location->setText(data_contact.req_location);          // c.req_location
    ui->combo_req_way->setCurrentIndex(ui->combo_req_way->findData(data_contact.req_way));                  // c.req_way
    ui->ch_date_incident->setChecked(data_contact.has_date_incid);      // c.req_date_incident
    ui->date_incident->setEnabled(data_contact.has_date_incid);         // c.req_date_incident
    ui->date_incident->setDate(data_contact.req_date_incid);            // c.req_date_incident
    ui->line_req_perform->setText(data_contact.req_perform);            // c.req_perform
    ui->line_req_performer->setText(data_contact.req_performer);        // c.req_performer
    ui->combo_req_status->setCurrentIndex(ui->combo_req_status->findData(data_contact.req_status));         // c.req_status
    ui->ch_date_control->setChecked(data_contact.has_date_contrl);      // c.req_date_control
    ui->date_control->setEnabled(data_contact.has_date_contrl);         // c.req_date_control
    ui->date_control->setDate(data_contact.req_date_contrl);            // c.req_date_control
    ui->ch_date_done->setChecked(data_contact.has_date_done);           // c.req_date_done
    ui->date_done->setEnabled(data_contact.has_date_done);              // c.req_date_done
    ui->date_done->setDate(data_contact.req_date_done);                 // c.req_date_done
    //ui->plain_req_result->setPlainText(data_contact.req_result);        // c.req_result
    ui->combo_req_result->setCurrentIndex(ui->combo_req_result->findData(data_contact.req_result_code));    // c.req_result_code
    ui->ch_efficiency->setChecked(!data_contact.check_efficiency);

    ui->lab_status->setText(data_contact.req_status_text);
    ui->bn_status_2->setEnabled(data_contact.req_status==0);

    ui->pan_contact->setEnabled(true);
}


void contacts_wnd::on_bn_cansel_clicked() {
    ui->pan_tab->setEnabled(true);
    ui->pan_tab->setVisible(true);

    ui->pan_contact->setEnabled(false);
    ui->pan_contact->setVisible(false);

    ui->lab_info->setText("Операция отменена пользователем");
    QApplication::processEvents();
}

void contacts_wnd::on_combo_req_type_currentIndexChanged(int index) {
    refresh_combo_req_reason();
}

void contacts_wnd::on_bn_save_clicked() {
    save_contact_data();
}

bool contacts_wnd::save_contact_data() {
    // проверка заполнения полей
    if (ui->combo_req_way->currentIndex()<1) {
        QMessageBox::warning(this, "Не хватает данных!", "Выберите способ связи.");
        return false;
    }
    //if (ui->combo_req_region->currentIndex()<0) {
    //    QMessageBox::warning(this, "Не хватает данных!", "Выберите регион, на территории которого принято это обращение.");
    //    return false;
    //}
    /*if (ui->combo_soc_status->currentIndex()<1) {
        QMessageBox::warning(this, "Не хватает данных!", "Укажите социальный статус застрахованного.");
        return false;
    }
    if (ui->combo_talker_form->currentIndex()<0) {
        QMessageBox::warning(this, "Не хватает данных!", "Выберите, является ли обратившийся физ.лицом или юр.лицом.");
        return false;
    }*/
    if (ui->combo_req_type->currentIndex()<1) {
        QMessageBox::warning(this, "Не хватает данных!", "Выберите вид обращения застрахованного.");
        return false;
    }
    if (/*ui->combo_req_type->currentIndex()==1 &&*/ ui->combo_req_reason->currentIndex()<1) {
        QMessageBox::warning(this, "Не хватает данных!", "Выберите тему обращения застрахованного.");
        return false;
    }
    if (ui->plain_req_petition->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не хватает данных!", "Введите текст обращения.");
        return false;
    }
    if (ui->combo_req_status->currentIndex()<1) {
        QMessageBox::warning(this, "Не хватает данных!", "Задайте текущий статус обращения.");
        return false;
    }
    if (ui->line_disp_fio->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не хватает данных!", "Укажите ФИО принявшего обращение диспетчера.");
        return false;
    }
    if ( !ui->line_pol_enp->text().trimmed().isEmpty() &&
         ui->line_pol_enp->text().trimmed().length()!=16 ) {
        QMessageBox::warning(this, "Ошибка данных!", "Номер ЕНП должен состоять из 16 цифр.");
        return false;
    }

    // проверим даты
    if ( ui->ch_year_birth->isChecked() &&
         ( ui->spin_year_birth->value()<1920 ||
           ui->spin_year_birth->value()>QDate::currentDate().year() ) ) {
        QMessageBox::warning(this, "Ошибка данных!", "Год рождения \n не может быть раньше 1920г. \n или позже текущего года.");
        return false;
    }
    if ( ui->ch_date_birth->isChecked() &&
         ( ui->date_birth->date()<QDate(1920,1,1) ||
           ui->date_birth->date()>QDate::currentDate() ) ) {
        QMessageBox::warning(this, "Ошибка данных!", "Дата рождения \n не может быть раньше 01.01.1920 \n или позже текущей даты.");
        return false;
    }
    if ( ui->ch_date_control->isChecked() &&
         ( ui->date_control->date()<QDate(1920,1,1) ||
           ui->date_control->date()<QDate::currentDate() ) ) {
        QMessageBox::warning(this, "Ошибка данных!", "Контрольная дата \n не может быть раньше 01.01.1920 \n или раньше текущей даты.");
        return false;
    }
    if ( ui->ch_date_doc->isChecked() &&
         ( ui->date_doc->date()<QDate(1920,1,1) ||
           ui->date_doc->date()>QDate::currentDate() ) ) {
        QMessageBox::warning(this, "Ошибка данных!", "Дата выдачи документа \n не может быть раньше 01.01.1920 \n или позже текущей даты.");
        return false;
    }
    if ( ui->ch_date_done->isChecked() &&
         ( ui->date_done->date()<QDate(1920,1,1) ||
           ui->date_done->date()<QDate::currentDate().addDays(-1) ) ) {
        QMessageBox::warning(this, "Ошибка данных!", "Дата выполнения поставленной задачи \n не может быть раньше 01.01.1920 \n или раньше вчерашней даты.");
        return false;
    }
    if ( ui->ch_date_incident->isChecked() &&
         ( ui->date_incident->date()<QDate(1920,1,1) ||
           ui->date_incident->date()>QDate::currentDate() ) ) {
        QMessageBox::warning(this, "Ошибка данных!", "Дата происшестрия \n не может быть раньше 01.01.1920 \n или позже текущей даты.");
        return false;
    }
    if ( ( ui->date_req->date()<QDate(1920,1,1) ||
           ui->date_req->date()>QDate::currentDate() ) ) {
        QMessageBox::warning(this, "Ошибка данных!", "Дата обращения \n не может быть раньше 01.01.1920 \n или позже текущей даты.");
        return false;
    }
    if ( ui->combo_req_result->currentData().toInt()<1
         && QMessageBox::question(this, "Нужно подтверждение",
                                  "Не выбран результат обработки заявки.\n\n"
                                  "Всё равно сохранить?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel ) {
        return false;
    }


    // выберем данные из компонентов
    data_contact.id_filial       = ui->combo_filial->currentData().toInt();
    data_contact.disp_fio        = ui->line_disp_fio->text();// c.disp_fio
    data_contact.fam             = ui->line_fam->text().trimmed().simplified().toUpper();                           // c.fam
    data_contact.im              = ui->line_im->text().trimmed().simplified().toUpper();                            // c.im
    data_contact.ot              = ui->line_ot->text().trimmed().simplified().toUpper();                            // c.ot
    data_contact.sex             = ui->combo_sex->currentData().toInt();                                            // c.sex
    data_contact.has_year_birth  = ui->ch_year_birth->isChecked();
    data_contact.year_birth      = ui->spin_year_birth->value();                                                    // c.year_birth
    data_contact.has_date_birth  = ui->ch_date_birth->isChecked();
    data_contact.date_birth      = data_contact.has_date_birth ? ui->date_birth->date() : QDate(1900,1,1);          // c.date_birth
    data_contact.doc_type        = ui->combo_doc_type->currentData().toInt();                                       // c.doc_type
    data_contact.doc_ser         = ui->line_doc_ser->text().trimmed().simplified().toUpper();                       // c.doc_ser
    data_contact.doc_sernum      = ui->line_doc_sernum->text().trimmed().simplified().toUpper();                    // c.doc_sernom
    data_contact.has_date_doc    = ui->ch_date_doc->isChecked();
    data_contact.doc_date        = data_contact.has_date_doc ? ui->date_doc->date() : QDate(1900,1,1);              // c.doc_date
    data_contact.doc_org         = ui->line_doc_org->text().trimmed().simplified();                                 // c.doc_org
    data_contact.pol_ser         = ui->line_pol_ser->text().trimmed().simplified().toUpper();                       // c.pol_ser
    data_contact.pol_sernum      = ui->line_pol_sernum->text().trimmed().simplified().toUpper();                    // c.pol_sernom
    data_contact.pol_enp         = ui->line_pol_enp->text().trimmed().simplified().toUpper();                       // c.pol_enp
    data_contact.address_reg     = ui->line_address_reg->text().trimmed().simplified();                             // c.address_reg
    data_contact.address_liv     = ui->line_address_liv->text().trimmed().simplified();                             // c.address_liv
    data_contact.phone           = ui->line_phone->text().trimmed().simplified();                                   // c.phone
    data_contact.email           = ui->line_email->text().trimmed().simplified();                                   // c.email
    data_contact.soc_status      = ui->combo_soc_status->currentData().toInt();                                     // c.soc_status
    data_contact.talker_form     = ui->combo_talker_form->currentData().toInt();                                    // c.talker_form
    if (data_contact.talker_form==0)    data_contact.talker_form = 1;
    data_contact.req_type_code   = ui->combo_req_type->currentData().toString();
    data_contact.req_reason_code = ui->combo_req_reason->currentData().toString();                                  // c.req_reason_code
    data_contact.req_datetime    = QDateTime(ui->date_req->date(), ui->time_req->time());                           // c.req_dt
    data_contact.req_petition    = ui->plain_req_petition->toPlainText().trimmed().simplified();                    // c.req_petition
    data_contact.req_comment     = ui->line_req_comment->text().trimmed().simplified();                             // c.req_comment
    data_contact.req_kladr_subj  = ui->combo_kladr_subj->currentData().toInt();                                     // c.req_region
    data_contact.req_kladr_dstr  = ui->combo_kladr_dstr->currentData().toInt();                                     // c.req_region
    data_contact.req_kladr_city  = ui->combo_kladr_city->currentData().toInt();                                     // c.req_region
    data_contact.req_kladr_nasp  = ui->combo_kladr_nasp->currentData().toInt();                                     // c.req_region
    data_contact.req_kladr_strt  = ui->combo_kladr_strt->currentData().toInt();                                     // c.req_region
    data_contact.req_location    = ui->line_req_location->text().trimmed().simplified();                            // c.req_location
    data_contact.req_way         = ui->combo_req_way->currentData().toInt();                                        // c.req_way
    data_contact.has_date_incid  = ui->ch_date_incident->isChecked();
    data_contact.req_date_incid  = data_contact.has_date_incid ? ui->date_incident->date() : QDate(1900,1,1);       // c.req_date_incident
    data_contact.req_perform     = ui->line_req_perform->text().trimmed().simplified();                             // c.req_perform
    data_contact.req_performer   = ui->line_req_performer->text().trimmed().simplified();                           // c.req_performer
    data_contact.req_status      = ui->combo_req_status->currentData().toInt();                                     // c.req_status
    data_contact.has_date_contrl = ui->ch_date_control->isChecked();
    data_contact.req_date_contrl = data_contact.has_date_contrl ? ui->date_control->date() : QDate(1900,1,1);       // c.req_date_control
    data_contact.has_date_done   = ui->ch_date_done->isChecked();
    data_contact.req_date_done   = data_contact.has_date_done ? ui->date_done->date() : QDate(1900,1,1);            // c.req_date_done
    data_contact.req_result      = ui->combo_req_result->currentText().trimmed().simplified();                      // c.req_result
    data_contact.req_result_code = ui->combo_req_result->currentData().toInt();                                     // c.req_result_code
    if ( data_contact.req_status==2 && !data_contact.has_date_done ) {
        data_contact.has_date_done = true;
        data_contact.req_date_done = QDate::currentDate();
    }
    data_contact.check_efficiency = !ui->ch_efficiency->isChecked();

    // собственно сохранение
    db.transaction();
    QString sql = "";

    if (data_contact.id<=0) {
        // добавим новую запись
        sql += "insert into contact_center"
               " ( id_filial, id_point, id_operator, disp_fio, id_person, id_doc, id_polis, fam, im, ot, sex, year_birth, date_birth, "
               "   doc_ser, doc_sernom, doc_date, doc_type, doc_org, pol_ser, pol_sernom, pol_enp, address_reg, address_liv, phone, email, soc_status, talker_form, "
               "   req_reason_code, req_dt, req_petition, req_comment, "
               "   req_kladr_subj, req_kladr_dstr, req_kladr_city, req_kladr_nasp, req_kladr_strt, "
               "   req_location, req_way, req_date_incident, "
               "   req_perform, req_performer, req_status, req_date_control, req_date_done, req_result, "
               "   status, dt_ins, dt_upd, "
               "   check_efficiency, n_irp, req_result_code, "
               "   snils, pol_vs_num "
               " ) "
               " values "
               " ( " + QString::number(data_contact.id_filial) + ", "
               "   " + QString::number(data_contact.id_point) + ", "
               "   " + QString::number(data_contact.id_operator) + ", "
               "   '" + data_contact.disp_fio + "', "
               "   " + (data_contact.id_person>=0 ? QString::number(data_contact.id_person) : "NULL") + ", "
               "   " + (data_contact.id_doc>=0 ? QString::number(data_contact.id_doc) : "NULL") + ", "
               "   " + (data_contact.id_polis>=0 ? QString::number(data_contact.id_polis) : "NULL") + ", "
               "   " + (data_contact.fam.isEmpty() ? QString("NULL") : QString("'" + data_contact.fam + "'")) + ", "
               "   " + (data_contact.im.isEmpty() ? QString("NULL")  : QString("'" + data_contact.im  + "'")) + ", "
               "   " + (data_contact.ot.isEmpty() ? QString("NULL")  : QString("'" + data_contact.ot  + "'")) + ", "
               "   " + (data_contact.sex>=0 ? QString::number(data_contact.sex) : "NULL") + ", "
               "   " + (data_contact.has_year_birth ? QString::number(data_contact.year_birth) : "NULL") + ", "
               "   " + (data_contact.has_date_birth ? QString("'" + data_contact.date_birth.toString("yyyy-MM-dd") + "'") : "NULL") + ", "

               "   " + (data_contact.doc_ser.isEmpty() ? QString("NULL") : QString("'" + data_contact.doc_ser  + "'")) + ", "
               "   " + (data_contact.doc_sernum.isEmpty() ? QString("NULL") : QString("'" + data_contact.doc_sernum  + "'")) + ", "
               "   " + (data_contact.has_date_doc ? QString("'" + data_contact.doc_date.toString("yyyy-MM-dd") + "'") : "NULL") + ", "
               "   " + (data_contact.doc_type<1 ? QString("NULL") : QString::number(data_contact.doc_type)) + ", "
               "   " + (data_contact.doc_org.isEmpty() ? QString("NULL") : QString("'" + data_contact.doc_org  + "'")) + ", "
               "   " + (data_contact.pol_ser.isEmpty() ? QString("NULL") : QString("'" + data_contact.pol_ser  + "'")) + ", "
               "   " + (data_contact.pol_sernum.isEmpty() ? QString("NULL") : QString("'" + data_contact.pol_sernum  + "'")) + ", "
               "   " + (data_contact.pol_enp.isEmpty() ? QString("NULL") : QString("'" + data_contact.pol_enp  + "'")) + ", "
               "   " + (data_contact.address_reg.isEmpty() ? QString("NULL") : QString("'" + data_contact.address_reg  + "'")) + ", "
               "   " + (data_contact.address_liv.isEmpty() ? QString("NULL") : QString("'" + data_contact.address_liv  + "'")) + ", "
               "   " + (data_contact.phone.isEmpty() ? QString("NULL") : QString("'" + data_contact.phone  + "'")) + ", "
               "   " + (data_contact.email.isEmpty() ? QString("NULL") : QString("'" + data_contact.email  + "'")) + ", "
               "   " + (data_contact.soc_status>=0 ? QString::number(data_contact.soc_status) : "NULL") + ", "
               "   " + (data_contact.talker_form>=0 ? QString::number(data_contact.talker_form) : "NULL") + ", "

               "   " + (data_contact.req_reason_code.isEmpty() ? QString("'" + data_contact.req_type_code  + "'") : QString("'1." + data_contact.req_reason_code.right(data_contact.req_reason_code.length()-2)  + "'")) + ", "
               "   " + QString("'" + data_contact.req_datetime.toString("yyyy-MM-dd hh:mm:ss")  + "'") + ", "
               "   " + (data_contact.req_petition.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_petition  + "'")) + ", "
               "   " + (data_contact.req_comment.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_comment  + "'")) + ", "
               "   " + QString::number(data_contact.req_kladr_subj) + ", "
               "   " + QString::number(data_contact.req_kladr_dstr) + ", "
               "   " + QString::number(data_contact.req_kladr_city) + ", "
               "   " + QString::number(data_contact.req_kladr_nasp) + ", "
               "   " + QString::number(data_contact.req_kladr_strt) + ", "
               "   " + (data_contact.req_location.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_location  + "'")) + ", "
               "   " + (data_contact.req_way>=0 ? QString::number(data_contact.req_way) : "NULL") + ", "
               "   " + (data_contact.has_date_incid ? QString("'" + data_contact.req_date_incid.toString("yyyy-MM-dd") + "'") : "NULL") + ", "

               "   " + (data_contact.req_perform.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_perform  + "'")) + ", "
               "   " + (data_contact.req_performer.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_performer  + "'")) + ", "
               "   " + (data_contact.req_status>=0 ? QString::number(data_contact.req_status) : "NULL") + ", "
               "   " + (data_contact.has_date_contrl ? QString("'" + data_contact.req_date_contrl.toString("yyyy-MM-dd") + "'") : "NULL") + ", "
               "   " + (data_contact.has_date_done ? QString("'" + data_contact.req_date_done.toString("yyyy-MM-dd") + "'") : "NULL") + ", "
               "   " + (data_contact.req_result.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_result  + "'")) + ", "
               "   0, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, "
               "   " + (data_contact.check_efficiency ? "1" : "0") + ", "
               "   NULL, "
               "   " + (data_contact.req_result_code<1 ? QString::number(data_contact.req_result_code) : "NULL") + ", "

               "   " + (data_contact.snils.isEmpty() ? "NULL" : QString("'" + data_contact.snils + "'")) + ", "
               "   " + (data_contact.vs_num.isEmpty() ? "NULL" : QString("'" + data_contact.vs_num + "'")) + " "
               " ) "
               " returning id, n_irp ; ";

        QSqlQuery *query = new QSqlQuery(db);
        mySQL.exec(this, sql, QString("Новое обращение"), *query, true, db, data_app);
        if (query->next()) {
            int id_contact = query->value(0).toInt();
            data_contact.id = id_contact;
            //data_contact.n_irp = "31003_" + QDate::currentDate().toString("yyMMdd") + "_" + QString("0000000" + QString::number(id_contact)).right(7) ;
            QString n_irp = query->value(1).toString();
            data_contact.n_irp = n_irp ;
            delete query;
        } else {
            delete query;
            db.rollback();

            ui->lab_info->setText("Ошибка записи в базу данных !!!");
            QMessageBox::warning(this, "Ошибка записи в базу данных",
                                 "При попытке сохранитьь данные в базу данных не получен ID новой записи.\n"
                                 "Это ненормальная ситуация. Надо разобраться.");
            return false;
        }

        db.commit();

        ui->pan_tab->setEnabled(true);
        ui->pan_tab->setVisible(true);

        ui->pan_contact->setEnabled(false);
        ui->pan_contact->setVisible(false);
        refresh_contacts_tab();
        return true;

    } else {
        // изменим выбранную запись
        sql += "update contact_center"
                " set n_irp='" + data_contact.n_irp + "', "
                "     id_filial=" + QString::number(data_contact.id_filial) + ", "
                "     id_point=" + QString::number(data_contact.id_point) + ", "
                "     id_operator=" + QString::number(data_contact.id_operator) + ", "
                "     disp_fio='" + data_contact.disp_fio + "', "
                "     id_person=" + (data_contact.id_person>=0 ? QString::number(data_contact.id_person) : "NULL") + ", "
                "     id_doc=" + (data_contact.id_doc>=0 ? QString::number(data_contact.id_doc) : "NULL") + ", "
                "     id_polis=" + (data_contact.id_polis>=0 ? QString::number(data_contact.id_polis) : "NULL") + ", "
                "     fam=" + (data_contact.fam.isEmpty() ? QString("NULL") : QString("'" + data_contact.fam + "'")) + ", "
                "     im=" + (data_contact.im.isEmpty() ? QString("NULL")  : QString("'" + data_contact.im  + "'")) + ", "
                "     ot=" + (data_contact.ot.isEmpty() ? QString("NULL")  : QString("'" + data_contact.ot  + "'")) + ", "
                "     sex=" + (data_contact.sex>=0 ? QString::number(data_contact.sex) : "NULL") + ", "
                "     year_birth=" + (data_contact.has_year_birth ? QString::number(data_contact.year_birth) : "NULL") + ", "
                "     date_birth=" + (data_contact.has_date_birth ? QString("'" + data_contact.date_birth.toString("yyyy-MM-dd") + "'") : "NULL") + ", "

                "     doc_type=" + QString::number(ui->combo_doc_type->currentData().toInt()) + ", "
                "     doc_ser=" + (data_contact.doc_ser.isEmpty() ? QString("NULL") : QString("'" + data_contact.doc_ser  + "'")) + ", "
                "     doc_sernom=" + (data_contact.doc_sernum.isEmpty() ? QString("NULL") : QString("'" + data_contact.doc_sernum  + "'")) + ", "
                "     doc_date=" + (data_contact.has_date_doc ? QString("'" + data_contact.doc_date.toString("yyyy-MM-dd") + "'") : "NULL") + ", "
                "     doc_org=" + (data_contact.doc_org.isEmpty() ? QString("NULL") : QString("'" + data_contact.doc_org  + "'")) + ", "
                "     pol_ser=" + (data_contact.pol_ser.isEmpty() ? QString("NULL") : QString("'" + data_contact.pol_ser  + "'")) + ", "
                "     pol_sernom=" + (data_contact.pol_sernum.isEmpty() ? QString("NULL") : QString("'" + data_contact.pol_sernum  + "'")) + ", "
                "     pol_enp=" + (data_contact.pol_enp.isEmpty() ? QString("NULL") : QString("'" + data_contact.pol_enp  + "'")) + ", "
                "     address_reg=" + (data_contact.address_reg.isEmpty() ? QString("NULL") : QString("'" + data_contact.address_reg  + "'")) + ", "
                "     address_liv=" + (data_contact.address_liv.isEmpty() ? QString("NULL") : QString("'" + data_contact.address_liv  + "'")) + ", "
                "     phone=" + (data_contact.phone.isEmpty() ? QString("NULL") : QString("'" + data_contact.phone  + "'")) + ", "
                "     email=" + (data_contact.email.isEmpty() ? QString("NULL") : QString("'" + data_contact.email  + "'")) + ", "
                "     soc_status=" + (data_contact.soc_status>=0 ? QString::number(data_contact.soc_status) : "NULL") + ", "
                "     talker_form=" + (data_contact.talker_form>=0 ? QString::number(data_contact.talker_form) : "NULL") + ", "

                "     req_reason_code=" + (data_contact.req_reason_code.isEmpty() ? QString("'" + data_contact.req_type_code  + "'") : QString("'1." + data_contact.req_reason_code.right(data_contact.req_reason_code.length()-2)  + "'")) + ", "
                "     req_dt=" + QString("'" + data_contact.req_datetime.toString("yyyy-MM-dd hh:mm:ss")  + "'") + ", "
                "     req_petition=" + (data_contact.req_petition.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_petition  + "'")) + ", "
                "     req_comment=" + (data_contact.req_comment.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_comment  + "'")) + ", "
                "     req_kladr_subj=" + QString::number(data_contact.req_kladr_subj) + ", "
                "     req_kladr_dstr=" + QString::number(data_contact.req_kladr_dstr) + ", "
                "     req_kladr_city=" + QString::number(data_contact.req_kladr_city) + ", "
                "     req_kladr_nasp=" + QString::number(data_contact.req_kladr_nasp) + ", "
                "     req_kladr_strt=" + QString::number(data_contact.req_kladr_strt) + ", "
                "     req_location=" + (data_contact.req_location.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_location  + "'")) + ", "
                "     req_way=" + (data_contact.req_way>=0 ? QString::number(data_contact.req_way) : "NULL") + ", "
                "     req_date_incident=" + (data_contact.has_date_incid ? QString("'" + data_contact.req_date_incid.toString("yyyy-MM-dd") + "'") : "NULL") + ", "

                "     req_perform=" + (data_contact.req_perform.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_perform  + "'")) + ", "
                "     req_performer=" + (data_contact.req_performer.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_performer  + "'")) + ", "
                "     req_status=" + (data_contact.req_status>=0 ? QString::number(data_contact.req_status) : "NULL") + ", "
                "     req_date_control=" + (data_contact.has_date_contrl ? QString("'" + data_contact.req_date_contrl.toString("yyyy-MM-dd") + "'") : "NULL") + ", "
                "     req_date_done=" + (data_contact.has_date_done ? QString("'" + data_contact.req_date_done.toString("yyyy-MM-dd") + "'") : "NULL") + ", "
                "     req_result=" + (data_contact.req_result.isEmpty() ? QString("NULL") : QString("'" + data_contact.req_result  + "'")) + ", "
                "     status=0, "
                "     dt_upd=CURRENT_TIMESTAMP,"
                "     check_efficiency=" + (data_contact.check_efficiency ? "1" : "0") + ", "
                "     req_result_code=" + QString::number(data_contact.req_result_code) + ", "

                "     snils='" + data_contact.snils + "', "
                "     pol_vs_num='" + data_contact.vs_num + "' "
                " where id=" + QString::number(data_contact.id) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if ( mySQL.exec(this, sql, QString("Правим обращение"), *query, true, db, data_app) ) {
            delete query;
        } else {
            delete query;
            db.rollback();

            ui->lab_info->setText("Ошибка записи в базу данных !!!");
            QMessageBox::warning(this, "Ошибка записи в базу данных",
                                 "При попытке изменить данные в базе данных не получен ID новой записи.\n"
                                 "Это ненормальная ситуация. Надо разобраться.");
            return false;
        }

        db.commit();

        ui->pan_tab->setEnabled(true);
        ui->pan_tab->setVisible(true);

        ui->pan_contact->setEnabled(false);
        refresh_contacts_tab();
        return true;
    }

    return true;
}

void contacts_wnd::on_bn_edit_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_contacts || !ui->tab_contacts->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!", "Таблица не загружена!\nНажмите кнопку [Обновить] и выберите строку таблицы.");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_contacts->selectionModel()->selection().indexes();

    ui->pan_tab->setEnabled(false);
    ui->pan_tab->setVisible(true);

    if (indexes.size()>0) {
        ui->lab_info->setText("VVV  Правка данных обращения  VVV");

        QModelIndex index = indexes.front();

        ui->pan_contact->setVisible(true);

        // выберем данные из таблицы
        data_contact.id              = model_contacts.data(model_contacts.index(index.row(),59), Qt::EditRole).toInt();     // c.id
        data_contact.n_irp           = model_contacts.data(model_contacts.index(index.row(), 0), Qt::EditRole).toString();  // c.n_irp
        data_contact.id_filial       = model_contacts.data(model_contacts.index(index.row(), 1), Qt::EditRole).toInt();     // f.id_filial
        data_contact.id_point        = model_contacts.data(model_contacts.index(index.row(), 2), Qt::EditRole).toInt();     // f.id_filial
        data_contact.id_operator     = model_contacts.data(model_contacts.index(index.row(), 3), Qt::EditRole).toInt();     // c.id_operator
        data_contact.disp_fio        = model_contacts.data(model_contacts.index(index.row(), 4), Qt::EditRole).toString();  // c.disp_fio
        data_contact.id_person       = model_contacts.data(model_contacts.index(index.row(), 5), Qt::EditRole).toInt();     // c.id_person
        data_contact.id_doc          = model_contacts.data(model_contacts.index(index.row(), 6), Qt::EditRole).toInt();     // c.id_doc
        data_contact.id_polis        = model_contacts.data(model_contacts.index(index.row(), 7), Qt::EditRole).toInt();     // c.id_polis
        data_contact.fam             = model_contacts.data(model_contacts.index(index.row(), 8), Qt::EditRole).toString();  // c.fam
        data_contact.im              = model_contacts.data(model_contacts.index(index.row(), 9), Qt::EditRole).toString();  // c.im
        data_contact.ot              = model_contacts.data(model_contacts.index(index.row(),10), Qt::EditRole).toString();  // c.ot
        data_contact.sex             = model_contacts.data(model_contacts.index(index.row(),11), Qt::EditRole).toInt();     // c.sex
        data_contact.has_year_birth  = !model_contacts.data(model_contacts.index(index.row(),12), Qt::EditRole).isNull();
        data_contact.year_birth      = model_contacts.data(model_contacts.index(index.row(),12), Qt::EditRole).toInt();     // c.year_birth
        data_contact.has_date_birth  = !model_contacts.data(model_contacts.index(index.row(),13), Qt::EditRole).isNull();
        data_contact.date_birth      = model_contacts.data(model_contacts.index(index.row(),13), Qt::EditRole).toDate();    // c.date_birth
        data_contact.doc_type        = model_contacts.data(model_contacts.index(index.row(),14), Qt::EditRole).toInt();     // c.doc_type
        data_contact.doc_ser         = model_contacts.data(model_contacts.index(index.row(),15), Qt::EditRole).toString();  // c.doc_ser
        data_contact.doc_sernum      = model_contacts.data(model_contacts.index(index.row(),16), Qt::EditRole).toString();  // c.doc_sernum
        data_contact.has_date_doc    = !model_contacts.data(model_contacts.index(index.row(),17), Qt::EditRole).isNull();
        data_contact.doc_date        = model_contacts.data(model_contacts.index(index.row(),17), Qt::EditRole).toDate();    // c.doc_date
        data_contact.doc_org         = model_contacts.data(model_contacts.index(index.row(),18), Qt::EditRole).toString();  // c.doc_org
        data_contact.pol_ser         = model_contacts.data(model_contacts.index(index.row(),19), Qt::EditRole).toString();  // c.pol_ser
        data_contact.pol_sernum      = model_contacts.data(model_contacts.index(index.row(),20), Qt::EditRole).toString();  // c.pol_sernom
        data_contact.pol_enp         = model_contacts.data(model_contacts.index(index.row(),21), Qt::EditRole).toString();  // c.pol_enp
        data_contact.address_reg     = model_contacts.data(model_contacts.index(index.row(),22), Qt::EditRole).toString();  // c.address_reg
        data_contact.address_liv     = model_contacts.data(model_contacts.index(index.row(),23), Qt::EditRole).toString();  // c.address_liv
        data_contact.phone           = model_contacts.data(model_contacts.index(index.row(),24), Qt::EditRole).toString();  // c.phone
        data_contact.email           = model_contacts.data(model_contacts.index(index.row(),25), Qt::EditRole).toString();  // c.email
        data_contact.soc_status      = model_contacts.data(model_contacts.index(index.row(),26), Qt::EditRole).toInt();     // c.soc_status
        data_contact.talker_form     = model_contacts.data(model_contacts.index(index.row(),28), Qt::EditRole).toInt();     // c.talker_form
        data_contact.req_type_code   = model_contacts.data(model_contacts.index(index.row(),29), Qt::EditRole).toString().left(1);
        data_contact.req_reason_code = model_contacts.data(model_contacts.index(index.row(),29), Qt::EditRole).toString();  // c.req_reason_code
        QDate date_req               = model_contacts.data(model_contacts.index(index.row(),32), Qt::EditRole).toDate();
        QTime time_req               = model_contacts.data(model_contacts.index(index.row(),32), Qt::EditRole).toTime();
        data_contact.req_datetime    = QDateTime(date_req, time_req);// c.req_dt
        data_contact.req_petition    = model_contacts.data(model_contacts.index(index.row(),33), Qt::EditRole).toString();  // c.req_petition
        data_contact.req_comment     = model_contacts.data(model_contacts.index(index.row(),34), Qt::EditRole).toString();  // c.req_comment
        data_contact.req_kladr_subj  = model_contacts.data(model_contacts.index(index.row(),35), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_kladr_dstr  = model_contacts.data(model_contacts.index(index.row(),37), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_kladr_city  = model_contacts.data(model_contacts.index(index.row(),39), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_kladr_nasp  = model_contacts.data(model_contacts.index(index.row(),41), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_kladr_strt  = model_contacts.data(model_contacts.index(index.row(),43), Qt::EditRole).toInt();     // c.req_region
        data_contact.req_location    = model_contacts.data(model_contacts.index(index.row(),45), Qt::EditRole).toString();  // c.req_location
        data_contact.req_way         = model_contacts.data(model_contacts.index(index.row(),46), Qt::EditRole).toInt();     // c.req_way
        data_contact.has_date_incid  =!model_contacts.data(model_contacts.index(index.row(),48), Qt::EditRole).isNull();
        data_contact.req_date_incid  = model_contacts.data(model_contacts.index(index.row(),48), Qt::EditRole).toDate();    // c.req_date_incident
        data_contact.req_perform     = model_contacts.data(model_contacts.index(index.row(),49), Qt::EditRole).toString();  // c.req_perform
        data_contact.req_performer   = model_contacts.data(model_contacts.index(index.row(),50), Qt::EditRole).toString();  // c.req_performer
        data_contact.req_status      = model_contacts.data(model_contacts.index(index.row(),51), Qt::EditRole).toInt();     // c.req_status
        data_contact.has_date_contrl =!model_contacts.data(model_contacts.index(index.row(),53), Qt::EditRole).isNull();
        data_contact.req_date_contrl = model_contacts.data(model_contacts.index(index.row(),53), Qt::EditRole).toDate();    // c.req_date_control
        data_contact.has_date_done   =!model_contacts.data(model_contacts.index(index.row(),54), Qt::EditRole).isNull();
        data_contact.req_date_done   = model_contacts.data(model_contacts.index(index.row(),54), Qt::EditRole).toDate();    // c.req_date_done
        data_contact.req_result      = model_contacts.data(model_contacts.index(index.row(),55), Qt::EditRole).toString();  // c.req_result
        data_contact.req_result_code = model_contacts.data(model_contacts.index(index.row(),60), Qt::EditRole).toInt();     // c.req_result_code
        data_contact.check_efficiency= !(model_contacts.data(model_contacts.index(index.row(),58), Qt::EditRole).toInt()<0);// c.check_efficiency

        data_contact.snils           = model_contacts.data(model_contacts.index(index.row(),62), Qt::EditRole).toString();  // c.snils
        data_contact.vs_num          = model_contacts.data(model_contacts.index(index.row(),63), Qt::EditRole).toString();  // c.vs_num

        // отобразим данные на форме
        ui->lab_req_num->setText(data_contact.n_irp);

        ui->combo_filial->setCurrentIndex(ui->combo_filial->findData(data_contact.id_filial));      // c.id_filial
        ui->line_disp_fio->setText(data_contact.disp_fio);                  // c.disp_fio
        ui->line_fam->setText(data_contact.fam);                            // c.fam
        ui->line_im->setText(data_contact.im);                              // c.im
        ui->line_ot->setText(data_contact.ot);                              // c.ot
        ui->combo_sex->setCurrentIndex(ui->combo_sex->findData(data_contact.sex));      // c.sex
        ui->ch_year_birth->setChecked(data_contact.has_year_birth);
        ui->spin_year_birth->setValue(data_contact.year_birth);             // c.year_birth
        ui->ch_date_birth->setChecked(data_contact.has_date_birth);         // c.date_birth
        ui->date_birth->setEnabled(data_contact.has_date_birth);            // c.date_birth
        ui->date_birth->setDate(data_contact.has_date_birth ? data_contact.date_birth : QDate(1900,1,1));            // c.date_birth
        ui->combo_doc_type->setCurrentIndex(ui->combo_doc_type->findData(data_contact.doc_type));                    // c.doc_type
        ui->line_doc_ser->setText(data_contact.doc_ser);                    // c.doc_ser
        ui->line_doc_sernum->setText(data_contact.doc_sernum);              // c.doc_sernom
        ui->ch_date_doc->setChecked(data_contact.has_date_doc);             // c.date_birth
        ui->date_doc->setEnabled(data_contact.has_date_doc);                // c.date_birth
        ui->date_doc->setDate(data_contact.has_date_doc ? data_contact.doc_date : QDate(1900,1,1));                  // c.doc_date
        ui->line_doc_org->setText(data_contact.doc_org);                    // c.doc_org
        ui->line_pol_ser->setText(data_contact.pol_ser);                    // c.pol_ser
        ui->line_pol_sernum->setText(data_contact.pol_sernum);              // c.pol_sernom
        ui->line_pol_enp->setText(data_contact.pol_enp);                    // c.pol_enp
        ui->line_address_reg->setText(data_contact.address_reg);            // c.address_reg
        ui->line_address_liv->setText(data_contact.address_liv);            // c.address_liv
        ui->line_phone->setText(data_contact.phone);                        // c.phone
        ui->line_email->setText(data_contact.email);                        // c.email
        ui->combo_soc_status->setCurrentIndex(ui->combo_soc_status->findData(data_contact.soc_status));              // c.soc_status
        ui->combo_talker_form->setCurrentIndex(ui->combo_talker_form->findData(data_contact.talker_form));           // c.talker_form
        ui->combo_req_type->setCurrentIndex(ui->combo_req_type->findData(data_contact.req_type_code));
        ui->combo_req_reason->setCurrentIndex(ui->combo_req_reason->findData(data_contact.req_reason_code));         // c.req_reason_code
        ui->date_req->setDate(data_contact.req_datetime.date());            // c.req_dt
        ui->time_req->setTime(data_contact.req_datetime.time());            // c.req_dt
        ui->plain_req_petition->setPlainText(data_contact.req_petition);    // c.req_petition
        ui->line_req_comment->setText(data_contact.req_comment);            // c.req_comment
        ui->combo_kladr_subj->setCurrentIndex(ui->combo_kladr_subj->findData(data_contact.req_kladr_subj));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, 0, 0, 0, 0);
        ui->combo_kladr_dstr->setCurrentIndex(ui->combo_kladr_dstr->findData(data_contact.req_kladr_dstr));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, data_contact.req_kladr_dstr, 0, 0, 0);
        ui->combo_kladr_city->setCurrentIndex(ui->combo_kladr_city->findData(data_contact.req_kladr_city));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, data_contact.req_kladr_dstr, data_contact.req_kladr_city, 0, 0);
        ui->combo_kladr_nasp->setCurrentIndex(ui->combo_kladr_nasp->findData(data_contact.req_kladr_nasp));          // c.req_region
        refresh_kladr(data_contact.req_kladr_subj, data_contact.req_kladr_dstr, data_contact.req_kladr_city, data_contact.req_kladr_nasp, 0);
        ui->combo_kladr_strt->setCurrentIndex(ui->combo_kladr_strt->findData(data_contact.req_kladr_strt));          // c.req_region
        ui->line_req_location->setText(data_contact.req_location);          // c.req_location
        ui->combo_req_way->setCurrentIndex(ui->combo_req_way->findData(data_contact.req_way));                       // c.req_way
        ui->ch_date_incident->setChecked(data_contact.has_date_incid);      // c.req_date_incident
        ui->date_incident->setEnabled(data_contact.has_date_incid);         // c.req_date_incident
        ui->date_incident->setDate(data_contact.has_date_incid ? data_contact.req_date_incid : QDate(1900,1,1));     // c.req_date_incident
        ui->line_req_perform->setText(data_contact.req_perform);            // c.req_perform
        ui->line_req_performer->setText(data_contact.req_performer);        // c.req_performer
        ui->combo_req_status->setCurrentIndex(ui->combo_req_status->findData(data_contact.req_status));              // c.req_status
        ui->ch_date_control->setChecked(data_contact.has_date_contrl);      // c.req_date_control
        ui->date_control->setEnabled(data_contact.has_date_contrl);         // c.req_date_control
        ui->date_control->setDate(data_contact.has_date_contrl ? data_contact.req_date_contrl : QDate(1900,1,1));    // c.req_date_control
        ui->ch_date_done->setChecked(data_contact.has_date_done);           // c.req_date_done
        ui->date_done->setEnabled(data_contact.has_date_done);              // c.req_date_done
        ui->date_done->setDate(data_contact.has_date_done ? data_contact.req_date_done : QDate(1900,1,1));           // c.req_date_done
        //ui->plain_req_result->setPlainText(data_contact.req_result);      // c.req_result
        ui->combo_req_result->setCurrentIndex(ui->combo_req_result->findData(data_contact.req_result_code));         // c.req_result_code
        ui->ch_efficiency->setChecked(!data_contact.check_efficiency);

        ui->pan_contact->setEnabled(true);
        ui->pan_contact->setVisible(true);

    } else {
        ui->pan_contact->setEnabled(false);
        ui->pan_contact->setVisible(false);

        QMessageBox::warning(this, "Ничего не выбрано",
                             "Выберите строку данных обращения застрахованного, которую хотите изменить.");

        ui->pan_tab->setEnabled(true);
        ui->pan_tab->setVisible(true);
    }

    this->setCursor(Qt::ArrowCursor);
}

void contacts_wnd::on_line_pol_enp_textChanged(const QString &arg1) {
    QString enp0 = ui->line_pol_enp->text().trimmed(), enp = "";

    for (int i=0; i<enp0.size(); ++i) {
        if (enp.length()>=16)
            break;
        QString c = enp0.mid(i, 1);
        if ( c=="0" ||
             c=="1" ||
             c=="2" ||
             c=="3" ||
             c=="4" ||
             c=="5" ||
             c=="6" ||
             c=="7" ||
             c=="8" ||
             c=="9" ) {
            enp += c;
        }
    }
    ui->line_pol_enp->setText(enp);
    ui->line_pol_enp->setCursorPosition(enp.length());
}

void contacts_wnd::on_bn_delete_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_contacts || !ui->tab_contacts->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!", "Таблица не загружена!\nНажмите кнопку [Обновить] и выберите строку.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_contacts->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Нужно подтверждение", "Удалить выбранную строку из базы?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        db.transaction();

        QString sql = "delete from contact_center "
                      " where id=" + QString::number(model_contacts.data(model_contacts.index(index.row(), 0), Qt::EditRole).toInt()) + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "удаление записи обращения", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить запись обращения произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        data_contact.id = -1;
        data_contact.n_irp = " -//- ";
        delete query2;

        db.commit();

        refresh_contacts_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void contacts_wnd::on_tab_contacts_activated(const QModelIndex &index) {
    on_bn_edit_clicked();
}

void contacts_wnd::on_act_send2fond_triggered() {
    delete send2fond_w;
    send2fond_w = new send2fond_wnd(db, data_app, settings);
    send2fond_w->exec();
}

void contacts_wnd::on_spin_year_birth_valueChanged(int arg1) {
    ui->date_birth->setDate(QDate(arg1, ui->date_birth->date().month(), ui->date_birth->date().day()));
}

void contacts_wnd::on_date_birth_dateChanged(const QDate &date) {
    ui->spin_year_birth->setValue(date.year());
}

void contacts_wnd::on_act_close_triggered() {
    QApplication::closeAllWindows();
    QApplication::exit();
}

void contacts_wnd::on_act_about_triggered() {
    QMessageBox::about(this,
                       "О программе",
                       "ИНКО-МЕД  -  «КОНТАКТ-центр»\n\n"
                       "Утилита для ведения базы обращений застрахованных ООО \"МСК «ИНКО-МЕД»\"\nверсия " + data_app.version + "\n\n"
                       "© ООО МСК \"ИНКО-МЕД\", 2015 г.\nРазработчик  -  Кизянов А.Ф.\n"
                       "______________________________\n"
                       "ПО тестируется под ОС\nWindows 7 Prof. SP1, 32 bit"
                       );
}

void contacts_wnd::on_combo_kladr_subj_activated(const QString &arg1) {
    refresh_kladr(ui->combo_kladr_subj->currentData().toInt(),
                  0,0,0,0);
}

void contacts_wnd::on_combo_kladr_dstr_activated(const QString &arg1) {
    refresh_kladr(ui->combo_kladr_subj->currentData().toInt(),
                  ui->combo_kladr_dstr->currentData().toInt(),
                  0,0,0);
}

void contacts_wnd::on_combo_kladr_city_activated(const QString &arg1) {
    refresh_kladr(ui->combo_kladr_subj->currentData().toInt(),
                  ui->combo_kladr_dstr->currentData().toInt(),
                  ui->combo_kladr_city->currentData().toInt(),
                  0,0);
}

void contacts_wnd::on_combo_kladr_nasp_activated(const QString &arg1) {
    refresh_kladr(ui->combo_kladr_subj->currentData().toInt(),
                  ui->combo_kladr_dstr->currentData().toInt(),
                  ui->combo_kladr_city->currentData().toInt(),
                  ui->combo_kladr_nasp->currentData().toInt(),
                  0);
}

void contacts_wnd::on_act_monitoring_triggered() {
    delete  print_monitoring_w;
    print_monitoring_w = new print_monitoring_wnd(db, data_app, settings, this);
    print_monitoring_w->exec();
}

void contacts_wnd::on_combo_req_way_currentIndexChanged(int index) {
    switch (ui->combo_req_way->currentData().toInt()) {
    case 1:
        ui->ch_efficiency->setText("Ожидание ответа \nбольше 2 минут ?");
        ui->ch_efficiency->setEnabled(true);
        break;
    case 4:
        ui->ch_efficiency->setText("Ожидание приёма \nбольше 8 минут ?");
        ui->ch_efficiency->setEnabled(true);
        break;
    default:
        ui->ch_efficiency->setText(" -//- ");
        ui->ch_efficiency->setEnabled(false);
        break;
    }
}






void contacts_wnd::on_line_fam_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_fam->setText("фамилия (60)");
    } else {
        ui->lab_fam->setText("фамилия (" + QString::number(n) + "/60)");
    }
    if (n>60) {
        QPalette pal = ui->lab_fam->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_fam->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_fam->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_fam->setPalette(pal);
    } else {
        QPalette pal = ui->lab_fam->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_fam->setPalette(pal);
    }
}

void contacts_wnd::on_line_im_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_im->setText("имя (40)");
    } else {
        ui->lab_im->setText("имя (" + QString::number(n) + "/40)");
    }
    if (n>40) {
        QPalette pal = ui->lab_im->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_im->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_im->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_im->setPalette(pal);
    } else {
        QPalette pal = ui->lab_im->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_im->setPalette(pal);
    }
}

void contacts_wnd::on_line_ot_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_ot->setText("отчество (40)");
    } else {
        ui->lab_ot->setText("отчество (" + QString::number(n) + "/40)");
    }
    if (n>40) {
        QPalette pal = ui->lab_ot->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_ot->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_ot->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_ot->setPalette(pal);
    } else {
        QPalette pal = ui->lab_ot->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_ot->setPalette(pal);
    }
}

void contacts_wnd::on_line_req_location_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_req_location->setText("место события (100)");
    } else {
        ui->lab_req_location->setText("место события (" + QString::number(n) + "/100)");
    }
    if (n>100) {
        QPalette pal = ui->lab_req_location->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_req_location->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_req_location->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_req_location->setPalette(pal);
    } else {
        QPalette pal = ui->lab_req_location->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_req_location->setPalette(pal);
    }
}

void contacts_wnd::on_line_disp_fio_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_disp_fio->setText("диспетчер (40)");
    } else {
        ui->lab_disp_fio->setText("диспетчер (" + QString::number(n) + "/40)");
    }
    if (n>40) {
        QPalette pal = ui->lab_disp_fio->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_disp_fio->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_disp_fio->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_disp_fio->setPalette(pal);
    } else {
        QPalette pal = ui->lab_disp_fio->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_disp_fio->setPalette(pal);
    }
}

void contacts_wnd::on_line_doc_org_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_doc_org->setText("кем выдан (250)");
    } else {
        ui->lab_doc_org->setText("кем выдан (" + QString::number(n) + "/250)");
    }
    if (n>250) {
        QPalette pal = ui->lab_doc_org->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_doc_org->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_doc_org->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_doc_org->setPalette(pal);
    } else {
        QPalette pal = ui->lab_doc_org->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_doc_org->setPalette(pal);
    }
}

void contacts_wnd::on_line_address_reg_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_address_reg->setText("адрес регистрации (250)");
    } else {
        ui->lab_address_reg->setText("адрес регистрации (" + QString::number(n) + "/250)");
    }
    if (n>250) {
        QPalette pal = ui->lab_address_reg->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_address_reg->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_address_reg->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_address_reg->setPalette(pal);
    } else {
        QPalette pal = ui->lab_address_reg->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_address_reg->setPalette(pal);
    }
}

void contacts_wnd::on_line_address_liv_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_address_liv->setText("адрес проживания (250)");
    } else {
        ui->lab_address_liv->setText("адрес проживания (" + QString::number(n) + "/250)");
    }
    if (n>250) {
        QPalette pal = ui->lab_address_liv->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_address_liv->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_address_liv->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_address_liv->setPalette(pal);
    } else {
        QPalette pal = ui->lab_address_liv->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_address_liv->setPalette(pal);
    }
}

void contacts_wnd::on_plain_req_petition_textChanged() {
    QString arg1 = ui->plain_req_petition->toPlainText();
    int n = arg1.length();
    if (n==0) {
        ui->lab_req_petition->setText("текст обращения\n(1000)");
    } else {
        ui->lab_req_petition->setText("текст обращения\n(" + QString::number(n) + "/1000)");
    }
    if (n>1000) {
        QPalette pal = ui->lab_req_petition->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_req_petition->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_req_petition->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_req_petition->setPalette(pal);
    } else {
        QPalette pal = ui->lab_req_petition->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_req_petition->setPalette(pal);
    }
}
/*
void contacts_wnd::on_plain_req_result_textChanged() {
    QString arg1 = ui->plain_req_result->toPlainText();
    int n = arg1.length();
    if (n==0) {
        ui->lab_req_result->setText("результат\n(1000)");
    } else {
        ui->lab_req_result->setText("результат\n(" + QString::number(n) + "/1000)");
    }
    if (n>1000) {
        QPalette pal = ui->lab_req_result->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_req_result->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_req_result->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_req_result->setPalette(pal);
    } else {
        QPalette pal = ui->lab_req_result->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_req_result->setPalette(pal);
    }
}
*/
void contacts_wnd::on_line_req_comment_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_req_comment->setText("комментарий (250)");
    } else {
        ui->lab_req_comment->setText("комментарий (" + QString::number(n) + "/250)");
    }
    if (n>250) {
        QPalette pal = ui->lab_req_comment->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_req_comment->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_req_comment->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_req_comment->setPalette(pal);
    } else {
        QPalette pal = ui->lab_req_comment->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_req_comment->setPalette(pal);
    }
}

void contacts_wnd::on_line_req_perform_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_req_perform->setText("поручение (250)");
    } else {
        ui->lab_req_perform->setText("поручение (" + QString::number(n) + "/250)");
    }
    if (n>250) {
        QPalette pal = ui->lab_req_perform->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_req_perform->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_req_perform->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_req_perform->setPalette(pal);
    } else {
        QPalette pal = ui->lab_req_perform->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_req_perform->setPalette(pal);
    }
}

void contacts_wnd::on_line_req_performer_textChanged(const QString &arg1) {
    int n = arg1.length();
    if (n==0) {
        ui->lab_req_performer->setText("кому поручено (40)");
    } else {
        ui->lab_req_performer->setText("кому поручено (" + QString::number(n) + "/40)");
    }
    if (n>40) {
        QPalette pal = ui->lab_req_performer->palette();
        pal.setColor(QPalette::WindowText, QColor(255,0,0));
        ui->lab_req_performer->setPalette(pal);
    } else if (n>0) {
        QPalette pal = ui->lab_req_performer->palette();
        pal.setColor(QPalette::WindowText, QColor(0,80,0));
        ui->lab_req_performer->setPalette(pal);
    } else {
        QPalette pal = ui->lab_req_performer->palette();
        pal.setColor(QPalette::WindowText, QColor(0,0,0));
        ui->lab_req_performer->setPalette(pal);
    }
}

void contacts_wnd::on_act_files_triggered() {
    packs_wnd *w = new packs_wnd(db, data_app);
    if (w->exec()) {
        //refresh_pers_pan_pack();
        //ui->combo_pers_pan_pack->setCurrentIndex(ui->combo_pers_pan_pack->findData(data_pack.id));
    }
}

void contacts_wnd::on_act_req_reester_triggered() {
    db.transaction();

    delete get_date_w;
    QDate date1 = QDate::currentDate(), date2 = QDate::currentDate();
    date1 = date2 = date1.addDays(-1);
    get_date_w = new get_date_wnd("Генерация журнала обращений застрахованных",
                                  "Задайте дату или диапазон дат, на которую надо сформировать журнал обращений",
                                  true, true, true, true, date1, date2, db, data_app, settings, this);
    if (!get_date_w->exec()) {
        QMessageBox::warning(this, "Отмена операции",
                             "Операция отменена пользователем.");
        return;
    }
    date1 = get_date_w->date1;
    date2 = get_date_w->date2;

    //------------------------------------------------------------
    // генерация реестра
    //------------------------------------------------------------
    QString sql = "select row_number() over(ORDER BY cc.n_irp) NN, cc.n_irp, "
                  "       coalesce(cc.fam, ' - ')||' '||coalesce(cc.im, ' ')||' '||coalesce(cc.ot, ' ') as FIO, \n"
                  "       coalesce(right('00'||extract(day from cc.date_birth),2)||'.'||right('00'||extract(month from cc.date_birth),2)||'.'||right('0000'||extract(year from cc.date_birth),4), cast(cc.year_birth as text)) as date_birth, \n"
                  "       cast(cc.req_dt as date), "
                  "       cc.req_petition, "
                  "       coalesce(cc.req_comment||'; ','')||coalesce('диспетчер - '||cc.disp_fio, '') as req_comment, \n"
                  "       st.text as status, \n"
                  "       res.text as result, \n"
                  "       cc.req_reason_code \n"
                  "  from contact_center cc \n"
                  "  left join spr_req_status st on(st.code=cc.req_status) \n"
                  "  left join spr_req_result res on(res.code=cc.req_result_code) \n"
                  " where id_filial=" + QString::number(ui->combo_filial_filter->currentData().toInt()) + " \n"
                  "   and cc.req_dt>='" + date1.toString("yyyy-MM-dd") + " 00:00:00' \n"
                  "   and cc.req_dt<='" + date2.toString("yyyy-MM-dd") + " 23:59:59' \n"
                  " order by cc.n_irp ; ";

    delete show_tab_w;
    show_tab_w = new show_tab_wnd("Журнал обращений застрахованных в СМО за период " + date1.toString("dd.MM.yyyy") + " - " + date2.toString("dd.MM.yyyy") + " ", sql, db, data_app, this);
    show_tab_w->exec();
}

void contacts_wnd::on_line_snils_filter_editingFinished() {
    refresh_contacts_tab();
}
void contacts_wnd::on_line_enp_filter_editingFinished() {
    refresh_contacts_tab();
}
void contacts_wnd::on_line_vs_filter_editingFinished() {
    refresh_contacts_tab();
}

void contacts_wnd::on_line_snils_filter_textEdited(const QString &arg1) {
    //refresh_contacts_tab();
}

void contacts_wnd::on_req_by_months_triggered() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    delete get_date_w;
    QDate date1 = QDate(QDate::currentDate().year(), 1, 1), date2 = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    get_date_w = new get_date_wnd("Задайте диапазон дат генерации отчёта",
                                  "Задайте диапазон дат помесячного отчёта о количестве и способах обращения застрахованных в СМО.\n",
                                  true, true, true, false, date1, date2, db, data_app, settings);
    if (!get_date_w->exec()) {
        QMessageBox::warning(this, "Отмена операции",
                             "Операция отменена пользователем.");
        return;
    }

    QString sql = "select date_part('year', c.req_dt)||'-'||right('00'||date_part('month',req_dt),2) as month, req_way as req_way_code, w.text as req_way_text, count(*) \n"
                  "  from contact_center c \n"
                  "  left join spr_req_way w on(w.code=c.req_way) \n"
                  " where id_filial=" + QString::number(data_app.id_filial) + " "
                  "   and req_dt>='" + date1.toString("yyyy-MM-dd") + "' \n"
                  "   and req_dt< '" + date2.toString("yyyy-MM-dd") + "' \n"
                  " group by date_part('year', c.req_dt)||'-'||right('00'||date_part('month',req_dt),2), req_way, w.text \n"
                  " order by date_part('year', c.req_dt)||'-'||right('00'||date_part('month',req_dt),2), req_way, w.text ; ";

    delete show_tab_w;
    show_tab_w = new show_tab_wnd("Отчёта о количестве и способах обращения застрахованных в СМО за период с " + date1.toString("yyyy-MM-dd") + " до " + date2.toString("yyyy-MM-dd"), sql, db, data_app, this);
    show_tab_w->exec();
}

void contacts_wnd::on_bn_status_2_clicked() {
    if (QMessageBox::question(this, "Нужно подтверждение",
                              "Вы действительно хотите передать выбранное обращение агенту 2-го уровня?\n\n"
                              "На данный момент это простая смена статуса обращения, но в последствии эта операция будет ограничивать доступность обращения для всех, кроме агентов второго уровня.\n\n"
                              "Продолжить?",
                              QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Yes) {
        ui->combo_req_status->setCurrentIndex(ui->combo_req_status->findData(-2));
    }
}

void contacts_wnd::on_combo_req_status_currentIndexChanged(int index) {
    ui->bn_status_2->setEnabled(ui->combo_req_status->currentData().toInt()==0);
}
