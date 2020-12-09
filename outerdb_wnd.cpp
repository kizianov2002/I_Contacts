#include "outerdb_wnd.h"
#include "ui_outerdb_wnd.h"

outerdb_wnd::outerdb_wnd(QSqlDatabase &db, s_data_contact &data_contact, s_data_app &data_app, QSettings &settings, QWidget *parent)
 : db(db), data_contact(data_contact), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::outerdb_wnd)
{
    ui->setupUi(this);

    refresh_filials();

    if (data_app.is_call) {
        // правка интерфейса для телефонистов
        ui->bn_process->setEnabled(false);
    }

    ui->combo_db_driv->setCurrentIndex(0);
    ui->ch_host->setChecked(!data_app.host.isEmpty());
    ui->line_db_host->setText(data_app.host);
    ui->ch_port->setChecked(data_app.port>0);
    ui->line_db_port->setText(QString::number(data_app.port));
    ui->ch_base->setChecked(!data_app.base.isEmpty());
    ui->line_db_base->setText(data_app.base);
    ui->ch_user->setChecked(!data_app.user.isEmpty());
    ui->line_db_user->setText(data_app.user);
    ui->ch_pass->setChecked(!data_app.pass.isEmpty());
    ui->line_db_pass->setText(data_app.pass);

    ui->line_fam_filter->setText(data_contact.fam);
    ui->line_im_filter->setText(data_contact.im);
    ui->line_ot_filter->setText(data_contact.ot);
    ui->line_enp_filter->setText(data_contact.pol_enp);

    ui->splitter->setStretchFactor(0,3);
    ui->splitter->setStretchFactor(1,1);

    QApplication::processEvents();

    on_combo_filial_currentIndexChanged(ui->combo_filial->findData(data_contact.id_filial));
    bool res = try_connect_outerdb();
    if (!res) {
        QMessageBox::warning(this,"Косяк","База не открылась");
    }
}

outerdb_wnd::~outerdb_wnd() {
    delete ui;
}

void outerdb_wnd::refresh_filials() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.id||') '||p.filial_name as filial_name, p.status, p.driv, p.name, p.host, p.port, p.base, p.user, p.pass, p.sql_persons_tab, p.sql_polises_tab "
                   " from filials p "
                   " where status>0 "
                   " order by p.id ; ");
    mySQL.exec(this, sql, "Получим список филиалов", query,true, db, data_app);
    ui->combo_filial->clear();
    filials_list.clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        s_filial_data data;
        data.id = query->value(0).toInt();
        data.filial_name = query->value(1).toString();
        data.status = query->value(2).toInt();
        data.driv = query->value(3).toString();
        data.name = query->value(4).toString();
        data.host = query->value(5).toString();
        data.port = query->value(6).toInt();
        data.base = query->value(7).toString();
        data.user = query->value(8).toString();
        data.pass = query->value(9).toString();
        data.sql_persons_tab = query->value(10).toString();
        data.sql_polises_tab = query->value(11).toString();
        //data.db_filial = NULL;

        int id = query->value(0).toInt();
        QString filial_name = query->value(1).toString();

        ui->combo_filial->addItem(filial_name, id);
        filials_list.append(data);
    }
    query->finish();
    delete query;

    ui->combo_filial->addItem(" - ввести параметры вручную - ", 0);

    // восстановим прежнее значение
    ui->combo_filial->setCurrentIndex(ui->combo_filial->findData(data_contact.id_filial));

    this->setCursor(Qt::ArrowCursor);
}

void outerdb_wnd::on_bn_close_clicked() {
    reject();
}

void outerdb_wnd::on_combo_filial_activated(int index) {
}


bool outerdb_wnd::try_connect_outerdb() {
    ui->lab_connect->setText("База не доступна!!");
    QPalette pal = ui->lab_connect->palette();
    pal.setColor(QPalette::WindowText, QColor(150,0,0));
    ui->lab_connect->setPalette(pal);
    ui->pan_data->setEnabled(false);
    QApplication::processEvents();

    /*if (filials_list[ui->combo_filial->currentIndex()].db_filial==NULL) {
        filials_list[ui->combo_filial->currentIndex()].db_filial = new QSqlDatabase();
    }*/

    if ( !(filials_list[ui->combo_filial->currentIndex()].db_filial.isOpen()) ) {
        // ------------------------------------------------ //
        // откроем новое подключение к внешней базе полисов //
        // ------------------------------------------------ //
        filials_list[ui->combo_filial->currentIndex()].db_filial = QSqlDatabase::addDatabase(ui->combo_db_driv->currentText(), filials_list.at(ui->combo_filial->currentIndex()).name);

        filials_list[ui->combo_filial->currentIndex()].db_filial.setConnectOptions();
        if (!filials_list.at(ui->combo_filial->currentIndex()).host.isEmpty())
            filials_list[ui->combo_filial->currentIndex()].db_filial.setHostName(filials_list.at(ui->combo_filial->currentIndex()).host);
        if (filials_list.at(ui->combo_filial->currentIndex()).port>0)
            filials_list[ui->combo_filial->currentIndex()].db_filial.setPort(filials_list.at(ui->combo_filial->currentIndex()).port);
        if (!filials_list.at(ui->combo_filial->currentIndex()).base.isEmpty())
            filials_list[ui->combo_filial->currentIndex()].db_filial.setDatabaseName(filials_list.at(ui->combo_filial->currentIndex()).base);
        if (!filials_list.at(ui->combo_filial->currentIndex()).user.isEmpty())
            filials_list[ui->combo_filial->currentIndex()].db_filial.setUserName(filials_list.at(ui->combo_filial->currentIndex()).user);
        if (!filials_list.at(ui->combo_filial->currentIndex()).pass.isEmpty())
            filials_list[ui->combo_filial->currentIndex()].db_filial.setPassword(filials_list.at(ui->combo_filial->currentIndex()).pass);

        if (!filials_list[ui->combo_filial->currentIndex()].db_filial.open()) {
            ui->pan_connect->setVisible(true);
            ui->pan_connect->setEnabled(true);
            ui->pan_data->setVisible(true);
            ui->pan_data->setEnabled(false);

            // подключение не удалось
            ui->line_db_host->setEnabled(false);
            ui->line_db_port->setEnabled(false);
            ui->line_db_base->setEnabled(false);
            ui->line_db_user->setEnabled(false);
            ui->line_db_pass->setEnabled(false);
            ui->ch_host->setEnabled(false);
            ui->ch_port->setEnabled(false);
            ui->ch_base->setEnabled(false);
            ui->ch_user->setEnabled(false);
            ui->ch_pass->setEnabled(false);

            QMessageBox::warning(this,"Ошибка подключения к базе полисов",
                                  "При попытке подключиться к база полисов произошла ошибка.\n");

        } else {
            ui->pan_connect->setVisible(false);
            ui->pan_connect->setEnabled(false);
            ui->pan_data->setVisible(true);
            ui->pan_data->setEnabled(true);

            ui->line_db_host->setEnabled(true);
            ui->line_db_port->setEnabled(true);
            ui->line_db_base->setEnabled(true);
            ui->line_db_user->setEnabled(true);
            ui->line_db_pass->setEnabled(true);
            ui->ch_host->setEnabled(true);
            ui->ch_port->setEnabled(true);
            ui->ch_base->setEnabled(true);
            ui->ch_user->setEnabled(true);
            ui->ch_pass->setEnabled(true);

            ui->lab_connect->setText("База доступна");
            QPalette pal = ui->lab_connect->palette();
            pal.setColor(QPalette::WindowText, QColor(0,150,0));
            ui->lab_connect->setPalette(pal);
            // подключение готово
            return true;
        }
        return false;

    } else {
        ui->pan_connect->setVisible(false);
        ui->pan_connect->setEnabled(false);
        ui->pan_data->setVisible(true);
        ui->pan_data->setEnabled(true);

        ui->lab_connect->setText("База доступна");
        QPalette pal = ui->lab_connect->palette();
        pal.setColor(QPalette::WindowText, QColor(0,150,0));
        ui->lab_connect->setPalette(pal);

        // подключение уже есть
        return true;
    }
}


void outerdb_wnd::on_combo_filial_currentIndexChanged(int index) {
    int n = ui->combo_filial->currentData().toInt();
    if ( n==0 || index>=filials_list.size() ) {
        ui->pan_connect->setVisible(true);
        ui->pan_connect->setEnabled(true);
        ui->pan_data->setVisible(true);
        ui->pan_data->setEnabled(false);
    } else {

        ui->combo_db_driv->setCurrentIndex(ui->combo_db_driv->findText(filials_list.at(ui->combo_filial->currentIndex()).driv));
        ui->line_db_host->setText(filials_list.at(ui->combo_filial->currentIndex()).host);
        ui->line_db_port->setText(QString::number(filials_list.at(ui->combo_filial->currentIndex()).port));
        ui->line_db_base->setText(filials_list.at(ui->combo_filial->currentIndex()).base);
        ui->line_db_user->setText(filials_list.at(ui->combo_filial->currentIndex()).user);
        ui->line_db_pass->setText(filials_list.at(ui->combo_filial->currentIndex()).pass);

        ui->ch_host->setChecked(!filials_list.at(ui->combo_filial->currentIndex()).host.isEmpty());
        ui->ch_base->setChecked(!filials_list.at(ui->combo_filial->currentIndex()).base.isEmpty());
        ui->ch_user->setChecked(!filials_list.at(ui->combo_filial->currentIndex()).user.isEmpty());
        ui->ch_pass->setChecked(!filials_list.at(ui->combo_filial->currentIndex()).pass.isEmpty());
        ui->ch_port->setChecked(filials_list.at(ui->combo_filial->currentIndex()).port>0);

        ui->line_db_host->setEnabled(!filials_list.at(ui->combo_filial->currentIndex()).host.isEmpty());
        ui->line_db_base->setEnabled(!filials_list.at(ui->combo_filial->currentIndex()).base.isEmpty());
        ui->line_db_user->setEnabled(!filials_list.at(ui->combo_filial->currentIndex()).user.isEmpty());
        ui->line_db_pass->setEnabled(!filials_list.at(ui->combo_filial->currentIndex()).pass.isEmpty());
        ui->line_db_port->setEnabled(filials_list.at(ui->combo_filial->currentIndex()).port>0);

        bool res = try_connect_outerdb();
        if (res) {
            refresh_persons_tab();
        } else {
            QMessageBox::warning(this,"Косяк","База не открылась");
        }
    }
}

void outerdb_wnd::on_bn_connect_clicked() {
    filials_list[ui->combo_filial->currentIndex()].driv = ui->combo_db_driv->currentText();
    filials_list[ui->combo_filial->currentIndex()].host = ui->line_db_host->text();
    filials_list[ui->combo_filial->currentIndex()].base = ui->line_db_base->text();
    filials_list[ui->combo_filial->currentIndex()].port = ui->line_db_port->text().toInt();
    filials_list[ui->combo_filial->currentIndex()].user = ui->line_db_user->text();
    filials_list[ui->combo_filial->currentIndex()].pass = ui->line_db_pass->text();

    if (try_connect_outerdb()) {
        ui->bn_connect->setEnabled(false);
        ui->bn_disconnect->setEnabled(true);
        ui->pan_data->setEnabled(true);
    } else {
        //ui->bn_connect->setEnabled(true);
        ui->bn_disconnect->setEnabled(false);
        ui->pan_data->setEnabled(false);
    }
}

void outerdb_wnd::on_bn_disconnect_clicked() {
    //ui->bn_connect->setEnabled(true);
    ui->bn_disconnect->setEnabled(false);
    ui->pan_data->setEnabled(false);
}

void outerdb_wnd::on_ch_host_clicked(bool checked) {
    ui->line_db_host->setEnabled(checked);
    if (!checked)
        ui->line_db_host->setText("");
}

void outerdb_wnd::on_ch_port_clicked(bool checked) {
    ui->line_db_port->setEnabled(checked);
    if (!checked)
        ui->line_db_port->setText("0");
}

void outerdb_wnd::on_ch_base_clicked(bool checked) {
    ui->line_db_base->setEnabled(checked);
    if (!checked)
        ui->line_db_base->setText("");
}

void outerdb_wnd::on_ch_user_clicked(bool checked) {
    ui->line_db_user->setEnabled(checked);
    if (!checked)
        ui->line_db_user->setText("");
}

void outerdb_wnd::on_ch_pass_clicked(bool checked) {
    ui->line_db_pass->setEnabled(checked);
    if (!checked)
        ui->line_db_pass->setText("");
}

void outerdb_wnd::on_bn_refresh_clicked() {
    refresh_persons_tab();
}

void outerdb_wnd::refresh_persons_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!filials_list[ui->combo_filial->currentIndex()].db_filial.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    ui->pan_data->setEnabled(false);
    QApplication::processEvents();

    QString persons_sql = filials_list[ui->combo_filial->currentIndex()].sql_persons_tab;

    persons_sql = persons_sql.replace("\r"," ").replace("\n"," ")
            .replace("#LINES#", QString::number(ui->spin_count_lines->value()))
            .replace("#FAM#", ui->line_fam_filter->text().trimmed().simplified().toUpper())
            .replace("#IM#", ui->line_im_filter->text().trimmed().simplified().toUpper())
            .replace("#OT#", ui->line_ot_filter->text().trimmed().simplified().toUpper())
            .replace("#ENP#", ui->line_enp_filter->text().trimmed().simplified().toUpper());

    model_persons.setQuery(persons_sql,filials_list[ui->combo_filial->currentIndex()].db_filial);
    QString err2 = model_persons.lastError().driverText();

    // подключаем модель из БД
    ui->tab_persons->setModel(&model_persons);

    // обновляем таблицу
    ui->tab_persons->reset();
    ui->tab_persons->setColumnWidth( 0,  1);
    ui->tab_persons->setColumnWidth( 1,  1);
    ui->tab_persons->setColumnWidth( 2,  1);
    ui->tab_persons->setColumnWidth( 6, 40);
    ui->tab_persons->setColumnWidth( 8, 40);
    ui->tab_persons->setColumnWidth( 9, 50);
    ui->tab_persons->setColumnWidth(10, 60);
    ui->tab_persons->setColumnWidth(13, 50);
    ui->tab_persons->setColumnWidth(15,120);
    ui->tab_persons->setColumnWidth(16,250);
    ui->tab_persons->setColumnWidth(17,250);
    ui->tab_persons->setColumnWidth(18,110);
    ui->tab_persons->setColumnWidth(20,110);
    ui->tab_persons->setColumnWidth(21,120);

    model_persons.setHeaderData( 3, Qt::Horizontal, ("фамилия"));      // e.fam,
    model_persons.setHeaderData( 4, Qt::Horizontal, ("имя"));      // e.im,
    model_persons.setHeaderData( 5, Qt::Horizontal, ("отчество"));      // e.ot,
    model_persons.setHeaderData( 6, Qt::Horizontal, ("пол"));      // case e.sex when 1 then 'М' when 2 then 'Ж' else '?' end as sex,
    model_persons.setHeaderData( 7, Qt::Horizontal, ("дата\nррожд."));      // e.date_birth
    model_persons.setHeaderData( 8, Qt::Horizontal, ("УДЛ"));      // d.doc_type as doc,
    model_persons.setHeaderData( 9, Qt::Horizontal, ("УДЛ\nсерия"));      // d.doc_ser as doc_s,
    model_persons.setHeaderData(10, Qt::Horizontal, ("УДЛ\nномер"));      // d.doc_num as doc_n,
    model_persons.setHeaderData(11, Qt::Horizontal, ("УДЛ\nдата"));      // d.doc_date,
    model_persons.setHeaderData(12, Qt::Horizontal, ("УДЛ\nкем выдан"));      // d.doc_org,
    model_persons.setHeaderData(13, Qt::Horizontal, ("полис\nсерия"));      // p.pol_ser as pol_s,
    model_persons.setHeaderData(14, Qt::Horizontal, ("полис\nномер"));      // coalesce(p.pol_num, cast(p.vs_num as text)) as pol_n,
    model_persons.setHeaderData(15, Qt::Horizontal, ("действующий\nЕНП"));      // p.enp,
    model_persons.setHeaderData(16, Qt::Horizontal, ("адрес регистрации"));      // ar.rg_name||', '||ar.rn_name||', '||ar.ct_name||', '||ar.np_name||', '||ar.st_name||', '||ar.house as addr_reg,
    model_persons.setHeaderData(17, Qt::Horizontal, ("адрес проживания"));      // al.rg_name||', '||al.rn_name||', '||al.ct_name||', '||al.np_name||', '||al.st_name||', '||al.house as addr_liv,
    model_persons.setHeaderData(18, Qt::Horizontal, ("сотвый \nтелефон"));      // e.phone_cell,
    model_persons.setHeaderData(19, Qt::Horizontal, ("e-mail"));      // e.email,
    model_persons.setHeaderData(20, Qt::Horizontal, ("СНИЛС"));      // e.snils,
    model_persons.setHeaderData(21, Qt::Horizontal, ("действующий\n№ ВС"));      // p.vs_num
    ui->tab_persons->repaint();

    ui->pan_data->setEnabled(true);
    this->setCursor(Qt::ArrowCursor);

    refresh_polises_tab();
}

void outerdb_wnd::refresh_polises_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!filials_list[ui->combo_filial->currentIndex()].db_filial.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    ui->pan_data->setEnabled(false);
    QApplication::processEvents();

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_persons->selectionModel()->selection().indexes();

    int id_person = -1;
    if (indexes.size()>0) {
        this->setCursor(Qt::WaitCursor);
        QModelIndex index = indexes.front();
        id_person = model_persons.data(model_persons.index(index.row(), 0), Qt::EditRole).toInt();
    }

    QString polises_sql = filials_list[ui->combo_filial->currentIndex()].sql_polises_tab;

    polises_sql = polises_sql.replace("\r"," ").replace("\n"," ")
            .replace("#ID_PERSON#", QString::number(id_person));       // polises.id

    model_polises.setQuery(polises_sql,filials_list[ui->combo_filial->currentIndex()].db_filial);
    QString err2 = model_polises.lastError().driverText();

    // подключаем модель из БД
    ui->tab_polises->setModel(&model_polises);

    // обновляем таблицу
    ui->tab_polises->reset();
    model_polises.setHeaderData( 0, Qt::Horizontal, ("id_polis"));                // p.id,
    model_polises.setHeaderData( 1, Qt::Horizontal, ("статаус\nВС"));             // vs.status,
    model_polises.setHeaderData( 2, Qt::Horizontal, ("статаус\nполиса"));         // pol.status,
    model_polises.setHeaderData( 3, Qt::Horizontal, ("ОКАТО"));                   // p.terr_ocato,
    model_polises.setHeaderData( 4, Qt::Horizontal, ("ЕНП"));                     // p.enp,
    model_polises.setHeaderData( 5, Qt::Horizontal, ("ОГРН СМО"));                // p.ogrn_smo,
    model_polises.setHeaderData( 7, Qt::Horizontal, ("тип полиса"));              // f8.text as f008_text,
    model_polises.setHeaderData( 9, Qt::Horizontal, ("№ ВС"));                    // p.vs_num,
    model_polises.setHeaderData(10, Qt::Horizontal, ("серия"));                   // p.pol_ser,
    model_polises.setHeaderData(11, Qt::Horizontal, ("номер"));                   // p.pol_num,
    model_polises.setHeaderData(12, Qt::Horizontal, ("№ УЭК"));                   // p.uec_num,
    model_polises.setHeaderData(13, Qt::Horizontal, ("дата\nначала"));            // p.date_begin,
    model_polises.setHeaderData(14, Qt::Horizontal, ("срок\nдействия"));          // p.date_end,
    model_polises.setHeaderData(15, Qt::Horizontal, ("дата\nизъятия"));           // p.date_stop,
    model_polises.setHeaderData(16, Qt::Horizontal, ("ЕРП ?"));                   // p.in_erp,
    model_polises.setHeaderData(19, Qt::Horizontal, ("форма"));                   // p.f_polis,
    model_polises.setHeaderData(22, Qt::Horizontal, ("код\nПВП"));                // pt1.point_regnum,
    model_polises.setHeaderData(23, Qt::Horizontal, ("первый\nПВП"));             // pt1.point_name,
    model_polises.setHeaderData(25, Qt::Horizontal, ("первый\nоператор"));        // op1.oper_fio,
    model_polises.setHeaderData(27, Qt::Horizontal, ("код\nПВП"));                // pt2.point_regnum,
    model_polises.setHeaderData(28, Qt::Horizontal, ("последний\nПВП"));          // pt2.point_name,
    model_polises.setHeaderData(30, Qt::Horizontal, ("последний\nоператор"));     // op2.oper_fio,
    model_polises.setHeaderData(35, Qt::Horizontal, ("отправка в\nТФОМС"));       // p.tfoms_date,
    model_polises.setHeaderData(36, Qt::Horizontal, ("дата\nСМС"));               // p.date_sms_pol,
    model_polises.setHeaderData(37, Qt::Horizontal, ("дата\nактивации"));         // p.date_activate,
    model_polises.setHeaderData(38, Qt::Horizontal, ("выдача\nна руки"));         // p.date_get2hand,
    model_polises.setHeaderData(39, Qt::Horizontal, ("ID\nТФОМС"));               // p.tfoms_id,
    model_polises.setHeaderData(40, Qt::Horizontal, ("дата\nТФОМС"));             // p.tfoms_date,
    model_polises.setHeaderData(41, Qt::Horizontal, ("причина \nвыбора СМО"));    // p.r_smo,
    model_polises.setHeaderData(42, Qt::Horizontal, ("причина \nвыдачи полиса")); // p.r_polis

    ui->tab_polises->setColumnWidth( 0,  1);   // p.id,
    ui->tab_polises->setColumnWidth( 1, 70);   // vs.status,
    ui->tab_polises->setColumnWidth( 2, 70);   // pol.status,
    ui->tab_polises->setColumnWidth( 3, 60);   // p.terr_ocato,
    ui->tab_polises->setColumnWidth( 4,120);   // p.enp,
    ui->tab_polises->setColumnWidth( 5, 80);   // p.ogrn_smo,
    ui->tab_polises->setColumnWidth( 6,  2);   // p.pol_v as f008_code,
    ui->tab_polises->setColumnWidth( 7,120);   // f8.text as f008_text,
    ui->tab_polises->setColumnWidth( 8,  2);   // p.pol_type,
    ui->tab_polises->setColumnWidth( 9, 80);   // p.vs_num,
    ui->tab_polises->setColumnWidth(10, 60);   // p.pol_ser,
    ui->tab_polises->setColumnWidth(11, 70);   // p.pol_num,
    ui->tab_polises->setColumnWidth(12,120);   // p.uec_num,
    ui->tab_polises->setColumnWidth(13, 80);   // p.date_begin,
    ui->tab_polises->setColumnWidth(14, 80);   // p.date_end,
    ui->tab_polises->setColumnWidth(15, 80);   // p.date_stop,
    ui->tab_polises->setColumnWidth(16,  2);   // p.in_erp,
    ui->tab_polises->setColumnWidth(17,  2);   // p.order_num,
    ui->tab_polises->setColumnWidth(18,  2);   // p.order_date,
    ui->tab_polises->setColumnWidth(19, 60);   // p.f_polis,
    ui->tab_polises->setColumnWidth(20,  2);   // p.id_person,
    ui->tab_polises->setColumnWidth(21,  2);   // p._id_first_point,
    ui->tab_polises->setColumnWidth(22, 50);   // pt1.point_regnum,
    ui->tab_polises->setColumnWidth(23,200);   // pt1.point_name,
    ui->tab_polises->setColumnWidth(24,  2);   // p._id_first_operator,
    ui->tab_polises->setColumnWidth(25,150);   // op1.oper_fio,
    ui->tab_polises->setColumnWidth(26,  2);   // p._id_last_point,
    ui->tab_polises->setColumnWidth(27, 50);   // pt2.point_regnum,
    ui->tab_polises->setColumnWidth(28,200);   // pt2.point_name,
    ui->tab_polises->setColumnWidth(29,  2);   // p._id_last_operator,
    ui->tab_polises->setColumnWidth(30,150);   // op2.oper_fio,
    ui->tab_polises->setColumnWidth(31,  2);   // pol.id,
    ui->tab_polises->setColumnWidth(32,  2);   // vs.vs_num,
    ui->tab_polises->setColumnWidth(33,  2);   // p.dt_ins,
    ui->tab_polises->setColumnWidth(34,  2);   // p.dt_upd,
    ui->tab_polises->setColumnWidth(35, 80);   // p.tfoms_date,
    ui->tab_polises->setColumnWidth(36, 80);   // p.date_sms_pol,
    ui->tab_polises->setColumnWidth(37, 80);   // p.date_activate,
    ui->tab_polises->setColumnWidth(38, 80);   // p.date_get2hand,
    ui->tab_polises->setColumnWidth(39, 70);   // p.tfoms_id,
    ui->tab_polises->setColumnWidth(40, 80);   // p.tfoms_date,
    ui->tab_polises->setColumnWidth(41,120);   // p.r_smo,
    ui->tab_polises->setColumnWidth(42,120);   // p.r_polis
    ui->tab_polises->repaint();

    ui->pan_data->setEnabled(true);
    this->setCursor(Qt::ArrowCursor);
}

void outerdb_wnd::on_line_fam_filter_returnPressed() {
    refresh_persons_tab();
}

void outerdb_wnd::on_line_im_filter_returnPressed() {
    refresh_persons_tab();
}

void outerdb_wnd::on_line_ot_filter_returnPressed() {
    refresh_persons_tab();
}

void outerdb_wnd::on_line_enp_filter_returnPressed() {
    refresh_persons_tab();
}

void outerdb_wnd::on_bn_clear_filter_clicked() {
    ui->line_fam_filter->clear();
    ui->line_im_filter->clear();
    ui->line_ot_filter->clear();
    ui->line_enp_filter->clear();
}

void outerdb_wnd::on_bn_process_clicked() {
    if (!ui->tab_persons || !ui->tab_persons->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!", "Таблица не загружена!\nВведите параметры фильтрации, нажмите кнопку [Поиск] и выберите строку таблицы для формирования записи нового обращения.");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_persons->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        // выберем данные из таблицы
        data_contact.id_filial       = ui->combo_filial->currentData().toInt();     // f.id_filial
        data_contact.id_person       = model_persons.data(model_persons.index(index.row(), 0), Qt::EditRole).toInt();     // c.id_person
        data_contact.id_doc          = model_persons.data(model_persons.index(index.row(), 1), Qt::EditRole).toInt();     // c.id_doc
        data_contact.id_polis        = model_persons.data(model_persons.index(index.row(), 2), Qt::EditRole).toInt();     // c.id_polis
        data_contact.fam             = model_persons.data(model_persons.index(index.row(), 3), Qt::EditRole).toString();  // c.fam
        data_contact.im              = model_persons.data(model_persons.index(index.row(), 4), Qt::EditRole).toString();  // c.im
        data_contact.ot              = model_persons.data(model_persons.index(index.row(), 5), Qt::EditRole).toString();  // c.ot
        data_contact.sex             = model_persons.data(model_persons.index(index.row(), 6), Qt::EditRole).toInt();     // c.sex
        data_contact.has_year_birth  = true;
        data_contact.year_birth      = model_persons.data(model_persons.index(index.row(), 7), Qt::EditRole).toDate().year();     // c.year_birth
        data_contact.has_date_birth  = true;
        data_contact.date_birth      = model_persons.data(model_persons.index(index.row(), 7), Qt::EditRole).toDate();    // c.date_birth
        data_contact.doc_type        = model_persons.data(model_persons.index(index.row(), 8), Qt::EditRole).toInt();     // c.doc_type
        data_contact.doc_ser         = model_persons.data(model_persons.index(index.row(), 9), Qt::EditRole).toString();  // c.doc_ser
        data_contact.doc_sernum      = model_persons.data(model_persons.index(index.row(),10), Qt::EditRole).toString();  // c.doc_sernum
        data_contact.has_date_doc    = !model_persons.data(model_persons.index(index.row(),11), Qt::EditRole).isNull();
        data_contact.doc_date        = model_persons.data(model_persons.index(index.row(),11), Qt::EditRole).toDate();    // c.doc_date
        data_contact.doc_org         = model_persons.data(model_persons.index(index.row(),12), Qt::EditRole).toString();  // c.doc_org
        data_contact.pol_ser         = model_persons.data(model_persons.index(index.row(),13), Qt::EditRole).toString();  // c.pol_ser
        data_contact.pol_sernum      = model_persons.data(model_persons.index(index.row(),14), Qt::EditRole).toString();  // c.pol_sernom
        data_contact.pol_enp         = model_persons.data(model_persons.index(index.row(),15), Qt::EditRole).toString();  // c.pol_enp
        data_contact.address_reg     = model_persons.data(model_persons.index(index.row(),16), Qt::EditRole).toString();  // c.address_reg
        data_contact.address_liv     = model_persons.data(model_persons.index(index.row(),17), Qt::EditRole).toString();  // c.address_liv
        data_contact.phone           = model_persons.data(model_persons.index(index.row(),18), Qt::EditRole).toString();  // c.phone
        data_contact.email           = model_persons.data(model_persons.index(index.row(),19), Qt::EditRole).toString();  // c.email
        data_contact.snils           = model_persons.data(model_persons.index(index.row(),20), Qt::EditRole).toString();  // c.snils
        data_contact.vs_num          = model_persons.data(model_persons.index(index.row(),21), Qt::EditRole).toString();  // c.vs_num
        data_contact.soc_status      = -1;                    // c.soc_status
        data_contact.talker_form     = -1;                    // c.talker_form
        data_contact.req_type_code   = "1";
        data_contact.req_reason_code = "";                      // c.req_reason_code
        data_contact.req_datetime    = QDateTime::currentDateTime();// c.req_dt
        data_contact.req_petition    = "";                      // c.req_petition
        data_contact.req_comment     = "";                      // c.req_comment
        data_contact.req_kladr_subj  = 0;                       // c.req_region
        data_contact.req_kladr_dstr  = 0;                       // c.req_region
        data_contact.req_kladr_city  = 0;                       // c.req_region
        data_contact.req_kladr_nasp  = 0;                       // c.req_region
        data_contact.req_kladr_strt  = 0;                       // c.req_region
        data_contact.req_location    = "";                      // c.req_location
        data_contact.req_way         = 1;                       // c.req_way
        data_contact.has_date_incid  = true;
        data_contact.req_date_incid  = QDate::currentDate();    // c.req_date_incident
        data_contact.req_perform     = "";                      // c.req_perform
        data_contact.req_performer   = "";                      // c.req_performer
        data_contact.req_status      = 0;                       // c.req_status
        data_contact.has_date_contrl = false;
        data_contact.req_date_contrl = QDate::currentDate();    // c.req_date_control
        data_contact.has_date_done   = false;
        data_contact.req_date_done   = QDate::currentDate();    // c.req_date_done
        data_contact.req_result      = "";                      // c.req_result

        data_contact.snils           = "";                      // c.snils
        data_contact.vs_num          = "";                      // c.vs_num

        this->setCursor(Qt::ArrowCursor);
        accept();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано",
                             "Выберите строку таблицы для формирования записи нового обращения.");

    }

    this->setCursor(Qt::ArrowCursor);
}

void outerdb_wnd::on_tab_persons_clicked(const QModelIndex &index) {
    refresh_polises_tab();
}
