#include "connection_window.h"
#include "ui_connection_window.h"
#include "contacts_wnd.h"
#include <QMessageBox>
#include <QDesktopWidget>


ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);

    ui->tc_pass->setEchoMode(QLineEdit::Password);
    ui->tc_op_pass->setEchoMode(QLineEdit::Password);

    // версия программы
    data_app.version = "0.2.0";

    // проверим разрешение экрана
    QDesktopWidget *scr_widget = QApplication::desktop();
    data_app.screen_w = scr_widget->width();
    data_app.screen_h = scr_widget->height();
    data_app.screen_small = data_app.screen_w<1500;

    // обнулим дочерние окна
    contacts_w = NULL;

    data_app.id_filial   = -1;
    data_app.id_point    = -1;
    data_app.id_operator = -1;

    // ini-файл
    data_app.ini_path = QApplication::applicationDirPath() + "/inko_contacts.ini";
    settings = new QSettings(data_app.ini_path, QSettings::IniFormat , NULL);

    // [contactsDB]
    data_app.host = settings->value("contactsDB/host", "localhost").toString();
    data_app.port = settings->value("contactsDB/port", 5432).toInt();
    data_app.base = settings->value("contactsDB/base", "inko_contacts").toString();
    data_app.user = settings->value("contactsDB/user", "postgres").toString();
    data_app.pass = settings->value("contactsDB/pass", "650325").toString();

    // [flags]
    data_app.is_tech   = settings->value("flags/is_tech",  (true)).toBool();
    data_app.is_head   = settings->value("flags/is_head",  (true)).toBool();
    data_app.is_locl   = settings->value("flags/is_locl",  (true)).toBool();
    data_app.is_call   = settings->value("flags/is_call",  (true)).toBool();

    // [log]
    data_app.log_bord = settings->value("log/log_bord", 1000).toInt();

    // [paths]
    data_app.path_install = QString::fromUtf8(settings->value("paths/path_install", "D:/INKO/").toByteArray());
    data_app.path_arch    = QString::fromUtf8(settings->value("paths/path_arch",    data_app.path_install + "7-Zip/").toByteArray());
    data_app.path_reports = QString::fromUtf8(settings->value("paths/path_reports", data_app.path_install + "reports/").toByteArray());
    data_app.path_temp    = QString::fromUtf8(settings->value("paths/path_temp",    "D:/TEMP/").toByteArray());
    data_app.path_out     = QString::fromUtf8(settings->value("paths/path_out",     "D:/INKO/_OUT_/").toByteArray());
    data_app.path_in      = QString::fromUtf8(settings->value("paths/path_in",      "D:/INKO/_IN_/").toByteArray());
    data_app.path_dbf     = QString::fromUtf8(settings->value("paths/path_dbf",     "D:/INKO/_DBF_/").toByteArray());
    data_app.path_to_TFOMS   = QString::fromUtf8(settings->value("paths/path_to_TFOMS",   "D:/INKO/_TFOMS_to_/").toByteArray());
    data_app.path_from_TFOMS = QString::fromUtf8(settings->value("paths/path_from_TFOMS", "D:/INKO/_TFOMS_from_/").toByteArray());

    // [filials]
    ui->combo_base->clear();
    data_app.bases_list = settings->value("bases/bases_list", "work;test").toString();
    if (data_app.bases_list.indexOf("work")>=0) {
        data_app.work_host = settings->value("work/host", "127.0.0.1").toString();
        data_app.work_port = settings->value("work/port", 5432).toInt();
        data_app.work_base = settings->value("work/base", "inko_contacts").toString();
        data_app.work_user = settings->value("work/user", "postgres").toString();
        data_app.work_pass = settings->value("work/pass", "650325").toString();
        ui->combo_base->addItem(" - рабочая база - ", -1);
    }
    if (data_app.bases_list.indexOf("test")>=0) {
        data_app.test_host = settings->value("test/host", "172.20.250.15").toString();
        data_app.test_port = settings->value("test/port", 5432).toInt();
        data_app.test_base = settings->value("test/base", "inko_contacts_test").toString();
        data_app.test_user = settings->value("test/user", "belg").toString();
        data_app.test_pass = settings->value("test/pass", "belg").toString();
        ui->combo_base->addItem("тестовая база, г. Воронеж", 0);
    }

    // [ContactCenter]
    data_app.subj_rf = settings->value("ContactCenter/subj_rf", " - регион не выбран - ").toString();
    data_app.smo_name = settings->value("ContactCenter/smo_name", "Медицинская страховая кампания \"ООО МСК \"ИНКО-МЕД\"").toString();
    data_app.smo_short = settings->value("ContactCenter/smo_ырщке", "ИНКО-МЕД").toString();
    data_app.has_federal_cc = settings->value("ContactCenter/has_federal_cc", 0).toInt();
    data_app.has_regional_cc = settings->value("ContactCenter/has_regional_cc", 0).toInt();
    data_app.cnt_w1 = settings->value("ContactCenter/cnt_w1", 0).toInt();
    data_app.cnt_w1_cc = settings->value("ContactCenter/cnt_w1_cc", 0).toInt();
    data_app.cnt_w2 = settings->value("ContactCenter/cnt_w2", 0).toInt();

    ui->frame_bn_messages->setVisible(data_app.is_tech);
    ui->text_comments->setReadOnly(!data_app.is_tech);

    this->move(100,100);
    refresh_connection();
    ui->tc_op_pass->setFocus();
}

ConnectionWindow::~ConnectionWindow() {
    delete ui;
}

void ConnectionWindow::refresh_connection() {
    ui->lab_DB_no->hide();
    ui->lab_DB_ok->hide();

    settings->sync();

    if (settings->status()==0) {
        // загрузим из инишника параметры подключения и проч.
        ui->tc_host->setText(data_app.host);
        ui->spin_port->setValue(data_app.port);
        ui->tc_base->setText(data_app.base);
        ui->tc_user->setText(data_app.user);
        ui->tc_pass->setText(data_app.pass);

        ui->ch_change_pass->setVisible(data_app.is_tech);
    }

    // создадим подключение к базе обновлений
    db = QSqlDatabase::addDatabase("QPSQL", "My Database");

    db.setConnectOptions();
    // сервер, на котором расположена БД - у нас localhost
    db.setHostName(ui->tc_host->text());
    // порт
    db.setDatabaseName(ui->tc_base->text());
    // имя базы данных
    db.setPort(ui->spin_port->value());
    // имя пользователя - postgres
    db.setUserName(ui->tc_user->text());
    // пароль для подключения к БД
    db.setPassword(ui->tc_pass->text());

    while (!db.open()) {
        // подключение не удалось
        ui->lab_DB_no->show();
        ui->lab_DB_ok->hide();
        ui->pan_DB_params->show();
        ui->pan_login->hide();
        ui->pan_messages->hide();
        //ui->plain_log->hide();
        QString pass_ = data_app.pass;
        if (QMessageBox::question(this,
                                  "Подключение не удалось",
                                  "Подключение не удалось\n\nDB: " + db.lastError().databaseText() +
                                  "\n\ndriver: "+db.lastError().driverText()+
                                  "\n\nПараметры: h=" + data_app.host + ":" + QString::number(data_app.port) + ", u=" + data_app.user + ", p=" + pass_.replace(QRegExp("[A-Z,a-z,0-9]"),"*") +
                                  "\n\nПовторить попытку?",
                                  QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) {
            if (this->windowState()==Qt::WindowFullScreen) {
                //this->resize(300,300);
            } else {
                this->resize(300,300);
            }
            break;
        }

        if (this->windowState()==Qt::WindowFullScreen) {
            //this->resize(300,300);
        } else {
            this->resize(300,300);
        }
    }
    if (db.isOpen()) {
        // подключение готово
        ui->lab_DB_no->hide();
        ui->lab_DB_ok->show();
        ui->pan_DB_params->hide();
        ui->pan_login->show();
        ui->pan_messages->show();
        if (this->windowState()==Qt::WindowFullScreen) {
            //this->resize(1000,800);
        } else {
            this->resize(1000,800);
        }

        refresh_filials();
        ui->tc_op_pass->setText("");

        // загрузим описание
        QString sql_load = "select text from messages where version='" + data_app.version + "' ; ";
        QSqlQuery *query_load = new QSqlQuery(db);
        mySQL.exec(this, sql_load, "Загрузка описания программы", *query_load, true, db, data_app);
        QString s = "версия программы : " + data_app.version + "\n\n";
        while (query_load->next()) {
            s += query_load->value(0).toString() + "\n\n";
        }
        ui->text_comments->setPlainText(s);
        delete query_load;




        QSqlQuery query = QSqlQuery(db);
        QString sql;

        // проверка сервера
        sql = "select d.name, d.value   from db_data d   where d.name='host_name' ; ";
        /*bool res_h =*/ query.exec( sql );
        s = "";
        /*int n = query.size();*/
        while (query.next()) {
            s += "  " + query.value(1).toString();
        }
        ui->lab_host->setText(s);
        data_app.db_host = s.trimmed();

        // проверка имени БД
        sql = "select d.name, d.value   from db_data d   where d.name='DB_name' ; ";
        /*bool res_db =*/ query.exec( sql );
        s = "";
        //n = query.size();
        while (query.next()) {
            s += "  " + query.value(1).toString();
        }
        data_app.db_name = s.trimmed();

        // проверка версии
        sql = "select d.name, d.value   from db_data d   where d.name='DB_version' ; ";
        bool res_v = query.exec( sql );
        s = "";
        int m = query.size();
        while (query.next()) {
            s += "  " + query.value(1).toString();
        }
        ui->lab_vers->setText(s);
        data_app.db_vers = s.trimmed();




        // реестер
        data_app.id_point = ui->combo_point->currentData().toInt();

        winreg = new QSettings(QSettings::UserScope, "МСК 'ИНКО-МЕД'", "ИНКО-МЕД - Контактный центр", NULL);
        data_app.terminal_id  = winreg->value(data_app.db_host + " - " + data_app.db_name + " - terminal_num", -1).toInt();
        data_app.terminal_name = winreg->value(data_app.db_host + " - " + data_app.db_name + " - terminal_name", " ").toString();

        db.transaction();

        // если новый терминал - получим новый terminal_num из базы
        if (data_app.terminal_id<0) {
            QString sql_terminal = "insert into terminals(terminal_name, status, id_point, dt_last) "
                                   " values (' ', 1, " + QString::number(data_app.id_point) + ", CURRENT_TIMESTAMP) "
                                   " returning id ; ";
            QSqlQuery *query_terminal = new QSqlQuery(db);
            if ( !query_terminal->exec(sql_terminal) ||
                 !query_terminal->next() ) {

                delete query_terminal;
                db.rollback();

                QMessageBox::warning(this, "Ошибка регистрации терминала",
                                     "При попытке регистрации нового терминала произошла ошибка");
            } else {
                data_app.terminal_id = query_terminal->value(0).toInt();
                data_app.terminal_name = " ";
                data_app.terminal_status = 1;

                winreg->setValue(data_app.db_host + " - " + data_app.db_name + " - terminal_num", data_app.terminal_id);
                winreg->setValue(data_app.db_host + " - " + data_app.db_name + " - terminal_name", data_app.terminal_name);

                winreg->sync();

                delete query_terminal;
                db.commit();
            }
        } else {
            // терминал уже известен - обновим его данные из базы
            QString sql_terminal = "select id_point, terminal_name, status "
                                   " from terminals "
                                   " where id=" + QString::number(data_app.terminal_id) + " ";
            QSqlQuery *query_terminal = new QSqlQuery(db);
            if ( !query_terminal->exec(sql_terminal) ) {

                delete query_terminal;
                db.rollback();

                QMessageBox::warning(this, "Ошибка чтения данных терминала",
                                     "При попытке чтения данных терминала произошла ошибка");
            } else {
                query_terminal->next();

                data_app.terminal_id_point = query_terminal->value(0).toInt();
                data_app.terminal_name = query_terminal->value(1).toString();
                data_app.terminal_status = query_terminal->value(2).toInt();

                winreg->setValue(data_app.db_host + " - " + data_app.db_name + " - terminal_num", data_app.terminal_id);
                winreg->setValue(data_app.db_host + " - " + data_app.db_name + " - terminal_name", data_app.terminal_name);

                winreg->sync();

                delete query_terminal;
                db.commit();
            }
        }

        // обновим в базе закешированный ini-файл
        db.transaction();
        QFile ini_f(data_app.ini_path);
        ini_f.open(QIODevice::ReadOnly);
        QString ini_s = ini_f.readAll();
        ini_s = ini_s.replace("'","\"").replace("\n","[br]");
        ini_f.close();
        QString sql_ini = "update terminals "
                          " set ini='" + ini_s + "', "
                          "     dt_last=CURRENT_TIMESTAMP, "
                          "     id_point=" + QString::number(data_app.id_point) + " "
                          " where id=" + QString::number(data_app.terminal_id) + " ; ";
        QSqlQuery *query_ini = new QSqlQuery(db);

        query_ini->exec(sql_ini);

        delete query_ini;
        db.commit();
    }

    if (ui->ch_change_pass->isChecked()) {
        ui->pan_DB_params->show();
    }

    // ini-файл
    // [contactsDB]
    settings->beginGroup("contactsDB");
    settings->setValue("host", data_app.host);
    settings->setValue("port", data_app.port);
    settings->setValue("base", data_app.base);
    settings->setValue("user", data_app.user);
    settings->setValue("pass", data_app.pass);
    settings->endGroup();
    // [flags]
    settings->beginGroup("flags");
    settings->setValue("is_tech", data_app.is_tech);
    settings->setValue("is_head", data_app.is_head);
    settings->setValue("is_locl", data_app.is_locl);
    settings->setValue("is_call", data_app.is_call);
    settings->endGroup();
    // [paths]
    settings->beginGroup("paths");
    settings->setValue("path_install", data_app.path_install.toUtf8());
    settings->setValue("path_arch", data_app.path_arch.toUtf8());
    settings->setValue("path_reports", data_app.path_reports.toUtf8());
    settings->setValue("path_temp", data_app.path_temp.toUtf8());
    settings->setValue("path_in", data_app.path_in.toUtf8());
    settings->setValue("path_out", data_app.path_out.toUtf8());
    settings->setValue("path_dbf", data_app.path_dbf.toUtf8());
    settings->setValue("path_to_TFOMS", data_app.path_to_TFOMS.toUtf8());
    settings->setValue("path_from_TFOMS", data_app.path_from_TFOMS.toUtf8());
    settings->endGroup();
    // [resent]
    settings->beginGroup("resent");
    settings->setValue("filial", ui->combo_filial->currentText().toUtf8());
    settings->setValue("point", ui->combo_point->currentText().toUtf8());
    settings->setValue("operator", ui->combo_operator->currentText().toUtf8());
    settings->endGroup();
    // [log]
    settings->beginGroup("log");
    settings->setValue("log_bord", data_app.log_bord);
    settings->endGroup();
    // [filials]
    settings->beginGroup("filials");
    settings->setValue("bases_list", data_app.bases_list);
    settings->endGroup();
    if (data_app.bases_list.indexOf("test")>=0) {
        // [test]
        settings->beginGroup("test");
        settings->setValue("host", data_app.test_host);
        settings->setValue("port", QString::number(data_app.test_port));
        settings->setValue("base", data_app.test_base);
        settings->setValue("user", data_app.test_user);
        settings->setValue("pass", data_app.test_pass);
        settings->endGroup();
    }
    if (data_app.bases_list.indexOf("work")>=0) {
        // [work]
        settings->beginGroup("work");
        settings->setValue("host", data_app.work_host);
        settings->setValue("port", QString::number(data_app.work_port));
        settings->setValue("base", data_app.work_base);
        settings->setValue("user", data_app.work_user);
        settings->setValue("pass", data_app.work_pass);
        settings->endGroup();
    }


    settings->sync();

    // TEMP
    QDir p_temp(data_app.path_temp);
    if (!p_temp.exists()) p_temp.mkpath(data_app.path_temp);
    // IN
    QDir p_in(data_app.path_in);
    if (!p_in.exists()) p_in.mkpath(data_app.path_in);
    // OUT
    QDir p_out(data_app.path_out);
    if (!p_out.exists()) p_out.mkpath(data_app.path_out);
    QDir p_out_backup(data_app.path_out + "/backup");
    if (!p_out_backup.exists()) p_out_backup.mkpath(data_app.path_out + "/backup");
    // DBF
    QDir p_dbf(data_app.path_dbf);
    if (!p_dbf.exists()) p_dbf.mkpath(data_app.path_dbf);
    // TFOMS
    QDir p_to_TFOMS(data_app.path_to_TFOMS);
    if (!p_to_TFOMS.exists()) p_to_TFOMS.mkpath(data_app.path_to_TFOMS);
    QDir p_from_TFOMS(data_app.path_from_TFOMS);
    if (!p_from_TFOMS.exists()) p_from_TFOMS.mkpath(data_app.path_from_TFOMS);
    QDir p_from_TFOMS_backup(data_app.path_from_TFOMS + "/backup");
    if (!p_from_TFOMS_backup.exists()) p_from_TFOMS_backup.mkpath(data_app.path_from_TFOMS + "/backup");
    // INSTALL
    QDir p_install(data_app.path_install);
    if (!p_install.exists()) p_install.mkpath(data_app.path_install);
    // ARCH
    QDir p_arch(data_app.path_arch);
    if (!p_arch.exists()) QMessageBox::warning(this, "Отсутствует модуль!", "ВНИМАНИЕ!\nОтстутствует предопределённый путь к папке модуля архивирования:\n" + data_app.path_arch);
    // REPORTS
    QDir p_reports(data_app.path_reports);
    if (!p_reports.exists()) QMessageBox::warning(this, "Отсутствует модуль!", "ВНИМАНИЕ!\nОтстутствует предопределённый путь к папке шаблонов отчётов:\n" + data_app.path_reports);
    // LOG
    QDir p_log(data_app.path_install + "/_LOG_");
    if (!p_log.exists()) p_log.mkpath(data_app.path_install + "/_LOG_");
}

void ConnectionWindow::refresh_filials() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select p.id, '('||p.id||') '||p.filial_name as name"
           " from filials p "
           " where status>0 "
           " order by p.id ; ";
    query->exec( sql );
    ui->combo_filial->clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        ui->combo_filial->addItem( query->value(1).toString(),
                                   query->value(0).toInt() );
    }
    query->finish();
    delete query;

    // восстановим прежнее значение
    if (settings->value("resent/filial", "").toString()!="") {
        ui->combo_filial->setCurrentIndex(ui->combo_filial->findText(QString::fromUtf8(settings->value("resent/filial", " ").toByteArray())));
    } else {
        ui->combo_filial->setCurrentIndex(0);
    }

    this->setCursor(Qt::ArrowCursor);

    refresh_points(ui->combo_filial->currentData().toInt());
}

void ConnectionWindow::refresh_points(int id_filial) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select p.id, '('||p.point_regnum||') '||p.point_name as name"
           " from points p "
           "      left join filials f on(p.id_filial=f.id)"
           " where p.status>0 "
           "   and f.id='" + QString::number(id_filial) + "' "
           " order by f.id, p.id ; ";
    query->exec( sql );
    ui->combo_point->clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        ui->combo_point->addItem( query->value(1).toString(),
                                  query->value(0).toInt() );
    }
    query->finish();
    delete query;

    // восстановим прежнее значение
    if (settings->value("resent/point", "").toString()!="") {
        ui->combo_point->setCurrentIndex(ui->combo_point->findText(QString::fromUtf8(settings->value("resent/point", " ").toByteArray())));
    } else {
        ui->combo_point->setCurrentIndex(0);
    }

    this->setCursor(Qt::ArrowCursor);

    refresh_operators(ui->combo_point->currentData().toInt());
}

void ConnectionWindow::refresh_operators(int id_point) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, u.oper_fio ";
    sql += " from operators u "
           "      left join points p on(p.id=u.id_point) "
           " where u.status>0 "
           "       and p.id_filial=" + QString::number(ui->combo_filial->currentData().toInt()) + " ";

    // фильтрация по ПВП только если надо
    if (ui->ch_oper_by_point->isChecked())
        sql += (id_point==-1) ? (QString(" ")) : (QString(" and u.id_point=") + QString::number(id_point));

    sql += " order by u.oper_fio ; ";
    query->exec( sql );
    ui->combo_operator->clear();

    // заполнение списка операторов
    while (query->next()) {
        ui->combo_operator->addItem( query->value(1).toString(),
                                     query->value(0).toInt() );
    }
    query->finish();
    delete query;

    // восстановим прежнее значение
    if (settings->value("resent/operator", "").toString()!="") {
        ui->combo_operator->setCurrentIndex(ui->combo_operator->findText(QString::fromUtf8(settings->value("resent/operator", " ").toByteArray())));
    } else {
        ui->combo_operator->setCurrentIndex(0);
    }

    this->setCursor(Qt::ArrowCursor);
}

void ConnectionWindow::on_tc_pass_returnPressed() {
    on_bn_save_DB_params_clicked();
}

void ConnectionWindow::on_bn_save_DB_params_clicked() {
    data_app.host = ui->tc_host->text();
    data_app.port = ui->spin_port->value();
    data_app.base = ui->tc_base->text();
    data_app.user = ui->tc_user->text();
    data_app.pass = ui->tc_pass->text();

    // сохраним параметры подключения к базе обновлений
    settings->beginGroup("contactsDB");
    settings->setValue("host", data_app.host);
    settings->setValue("port", data_app.port);
    settings->setValue("base", data_app.base);
    settings->setValue("user", data_app.user);
    settings->setValue("pass", data_app.pass);
    settings->endGroup();

    settings->sync();

    // обновим подключение к базе обновлений
    refresh_connection();
}

void ConnectionWindow::on_combo_filial_activated(int index) {
    refresh_points(ui->combo_filial->currentData().toInt());
    data_app.point_name = ui->combo_point->currentText();
    QString name = ui->combo_point->currentText();
    data_app.point_regnum = QString("000" + QString(name.mid(name.indexOf("(")+1, name.indexOf(")")-name.indexOf("(")-1))).right(3);
}

void ConnectionWindow::on_combo_point_activated(int index) {
    refresh_operators(ui->combo_point->currentData().toInt());
    data_app.point_name = ui->combo_point->currentText();
    QString name = ui->combo_point->currentText();
    data_app.point_regnum = QString("000" + QString(name.mid(name.indexOf("(")+1, name.indexOf(")")-name.indexOf("(")-1))).right(3);
}

void ConnectionWindow::on_tc_op_pass_returnPressed() {
    on_bn_start_clicked();
}

#include "contacts_wnd.h"

void ConnectionWindow::on_bn_start_clicked() {
    // проверим пароль оператора
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select u.id, u.oper_pass, u.oper_rights "
                   " from operators u "
                   " where u.id=");
    sql += QString::number(ui->combo_operator->currentData().toInt()) + " ; ";
    query->exec( sql );
    query->next();
    QString pass = query->value(1).toString();
    int rights = query->value(2).toInt();
    if (rights<1 || rights>4)
        rights = 4;

    if (pass==ui->tc_op_pass->text()) {
        ui->tc_op_pass->setText("");

        // [contactsDB]
        settings->beginGroup("contactsDB");
        settings->setValue("host", data_app.host);
        settings->setValue("port", data_app.port);
        settings->setValue("base", data_app.base);
        settings->setValue("user", data_app.user);
        settings->setValue("pass", data_app.pass);
        settings->endGroup();
        // [flags]
        data_app.is_tech = (rights==1);
        data_app.is_head = (rights==2);
        data_app.is_locl = (rights==3);
        data_app.is_call = (rights==4);
        settings->beginGroup("flags");
        settings->setValue("is_tech", data_app.is_tech);
        settings->setValue("is_head", data_app.is_head);
        settings->setValue("is_locl", data_app.is_locl);
        settings->setValue("is_call", data_app.is_call);
        settings->endGroup();
        // [paths]
        settings->beginGroup("paths");
        settings->setValue("path_install", data_app.path_install.toUtf8());
        settings->setValue("path_arch", data_app.path_arch.toUtf8());
        settings->setValue("path_reports", data_app.path_reports.toUtf8());
        settings->setValue("path_temp", data_app.path_temp.toUtf8());
        settings->setValue("path_in", data_app.path_in.toUtf8());
        settings->setValue("path_out", data_app.path_out.toUtf8());
        settings->setValue("path_dbf", data_app.path_dbf.toUtf8());
        settings->setValue("path_to_TFOMS", data_app.path_to_TFOMS.toUtf8());
        settings->setValue("path_from_TFOMS", data_app.path_from_TFOMS.toUtf8());
        settings->endGroup();
        // [resent]
        settings->beginGroup("resent");
        settings->setValue("filial", ui->combo_filial->currentText().toUtf8());
        settings->setValue("point", ui->combo_point->currentText().toUtf8());
        settings->setValue("operator", ui->combo_operator->currentText().toUtf8());
        settings->endGroup();
        // [log]
        settings->beginGroup("log");
        settings->setValue("log_bord", data_app.log_bord);
        settings->endGroup();
        // [filials]
        settings->beginGroup("filials");
        settings->setValue("bases_list", data_app.bases_list);
        settings->endGroup();
        if (data_app.bases_list.indexOf("test")>=0) {
            // [test]
            settings->beginGroup("test");
            settings->setValue("host", data_app.test_host);
            settings->setValue("port", QString::number(data_app.test_port));
            settings->setValue("base", data_app.test_base);
            settings->setValue("user", data_app.test_user);
            settings->setValue("pass", data_app.test_pass);
            settings->endGroup();
        }
        if (data_app.bases_list.indexOf("work")>=0) {
            // [work]
            settings->beginGroup("work");
            settings->setValue("host", data_app.work_host);
            settings->setValue("port", QString::number(data_app.work_port));
            settings->setValue("base", data_app.work_base);
            settings->setValue("user", data_app.work_user);
            settings->setValue("pass", data_app.work_pass);
            settings->endGroup();
        }

        settings->sync();

        data_app.id_filial = ui->combo_filial->currentData().toInt();
        data_app.filial_name = ui->combo_filial->currentText();
        data_app.id_point = ui->combo_point->currentData().toInt();
        data_app.id_operator = ui->combo_operator->currentData().toInt();
        data_app.operator_fio = ui->combo_operator->currentText();
        data_app.font_size = 8;

        // откроем окно полисов
        if (contacts_w==NULL) {
            contacts_w = new contacts_wnd(db, data_app, *settings);
            //contacts_w->setWindowState( contacts_w->windowState() | Qt::WindowFullScreen);
        }

        contacts_w->show();
        this->hide();

    } else {
        QMessageBox::warning(this, QString("Неверный пароль"), QString("Неверный пароль\n"));
    }
    ui->tc_op_pass->setText("");
}

void ConnectionWindow::on_ch_change_pass_clicked(bool checked) {
    ui->pan_DB_params->setVisible(checked);
    on_combo_base_currentIndexChanged("");
}

void ConnectionWindow::on_combo_operator_activated(const QString &arg1) {
    ui->tc_op_pass->setText("");
}

void ConnectionWindow::on_tc_op_pass_editingFinished() {
    //on_bn_start_clicked();
}

void ConnectionWindow::on_bn_exit_clicked() {
    QApplication::closeAllWindows();
}

void ConnectionWindow::on_combo_operator_currentIndexChanged(const QString &arg1) {
    data_app.operator_fio = arg1;
}

void ConnectionWindow::on_bn_messages_clicked() {
    // удалим старое описание
    QString sql_del = "delete from messages where version='" + data_app.version + "' ; ";
    QSqlQuery *query_del = new QSqlQuery(db);
    bool res_del = mySQL.exec(this, sql_del, "Очистка старого описания программы", *query_del, true, db, data_app);

    // сохраним текст в базу
    QString s = ui->text_comments->toPlainText();
    s = s.right(s.size()-s.indexOf("\n\n")-2);
    QString sql_ins = "insert into messages(version, text) "
                      "values('" + data_app.version + "','" + s + "') ; ";
    QSqlQuery *query_ins = new QSqlQuery(db);
    bool res_ins = mySQL.exec(this, sql_ins, "Сохранение нового описания программы", *query_ins, true, db, data_app);
}


void ConnectionWindow::on_combo_base_currentIndexChanged(const QString &arg1) {
    int i = ui->combo_base->currentData().toInt();
    switch (i) {
    case -1: {
            ui->tc_host->setText(data_app.work_host);
            ui->spin_port->setValue(data_app.work_port);
            ui->tc_base->setText(data_app.work_base);
            ui->tc_user->setText(data_app.work_user);
            ui->tc_pass->setText(data_app.work_pass);
        }
        break;
    case 0: {
            ui->tc_host->setText(data_app.test_host);
            ui->spin_port->setValue(data_app.test_port);
            ui->tc_base->setText(data_app.test_base);
            ui->tc_user->setText(data_app.test_user);
            ui->tc_pass->setText(data_app.test_pass);
        }
        break;
    default:
        break;
    }
}

void ConnectionWindow::on_ch_oper_by_point_clicked(bool checked) {
    refresh_operators(ui->combo_point->currentData().toInt());
}
