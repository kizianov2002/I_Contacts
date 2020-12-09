#include "settings_wnd.h"
#include "ui_settings_wnd.h"


settings_wnd::settings_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::settings_wnd) {
    ui->setupUi(this);

    refresh_folders();

    ui->lab_ini->setText(data_app.ini_path);

    // отображения данных
    ui->ln_host->setText(data_app.host);
    ui->spin_port->setValue(data_app.port);
    ui->ln_base->setText(data_app.base);
    ui->ln_user->setText(data_app.user);
    ui->ln_pass->setText(data_app.pass);

    ui->rb_tech->setChecked(data_app.is_tech);
    ui->rb_head->setChecked(data_app.is_head);
    ui->rb_locl->setChecked(data_app.is_locl);

    ui->ln_path_install->setText(data_app.path_install);
    ui->ln_path_arch->setText(data_app.path_arch);
    ui->ln_path_reports->setText(data_app.path_reports);
    ui->ln_path_temp->setText(data_app.path_temp);
    ui->ln_path_in->setText(data_app.path_in);
    ui->ln_path_out->setText(data_app.path_out);
    ui->ln_path_dbf->setText(data_app.path_dbf);
    ui->ln_path_to_TFOMS->setText(data_app.path_to_TFOMS);
    ui->ln_path_from_TFOMS->setText(data_app.path_from_TFOMS);

    refresh_ocato();

    // филиалы
    ui->ln_belg_host->setText(data_app.belg_host);
    ui->spin_belg_port->setValue(data_app.belg_port);
    ui->ln_belg_base->setText(data_app.belg_base);
    ui->ln_belg_user->setText(data_app.belg_user);
    ui->ln_belg_pass->setText(data_app.belg_pass);

    ui->ln_kursk_host->setText(data_app.kursk_host);
    ui->spin_kursk_port->setValue(data_app.kursk_port);
    ui->ln_kursk_base->setText(data_app.kursk_base);
    ui->ln_kursk_user->setText(data_app.kursk_user);
    ui->ln_kursk_pass->setText(data_app.kursk_pass);

    ui->ln_test_host->setText(data_app.test_host);
    ui->spin_test_port->setValue(data_app.test_port);
    ui->ln_test_base->setText(data_app.test_base);
    ui->ln_test_user->setText(data_app.test_user);
    ui->ln_test_pass->setText(data_app.test_pass);

    ui->ln_work_host->setText(data_app.work_host);
    ui->spin_work_port->setValue(data_app.work_port);
    ui->ln_work_base->setText(data_app.work_base);
    ui->ln_work_user->setText(data_app.work_user);
    ui->ln_work_pass->setText(data_app.work_pass);
}

settings_wnd::~settings_wnd() {
    delete ui;
}

void settings_wnd::refresh_folders() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, folder_name "
                  " from public.folders "
                  " where date_open<='" + QDate::currentDate().toString("yyyy-MM-dd") + "' "
                  "   and status>=0 "
                  " order by folder_name ; ";
    mySQL.exec(this, sql, QString("Список папок"), *query, true, db, data_app);
    ui->combo_folder->clear();
    ui->combo_folder->addItem(" - без папки - ", 0);
    while (query->next()) {
        ui->combo_folder->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    delete query;
    int n = 0;
    ui->combo_folder->setCurrentIndex(n);
    this->setCursor(Qt::ArrowCursor);
}

void settings_wnd::on_bn_cansel_clicked() {
    reject();
}

void settings_wnd::on_bn_save_clicked() {
    if (QMessageBox::question(this, "Нужно подтверждение", "Сохранить все сделанные изменеия в INI-файл?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel)
        return;

    // сохраним данные в settings
    data_app.host = ui->ln_host->text();
    data_app.port = ui->spin_port->value();
    data_app.base = ui->ln_base->text();
    data_app.user = ui->ln_user->text();
    data_app.pass = ui->ln_pass->text();

    data_app.is_tech = ui->rb_tech->isChecked();
    data_app.is_head = ui->rb_head->isChecked();
    data_app.is_locl = ui->rb_locl->isChecked();
    data_app.is_call = ui->rb_call->isChecked();

    data_app.path_install = ui->ln_path_install->text();
    data_app.path_arch = ui->ln_path_arch->text();
    data_app.path_reports = ui->ln_path_reports->text();
    data_app.path_temp = ui->ln_path_temp->text();
    data_app.path_in = ui->ln_path_in->text();
    data_app.path_out = ui->ln_path_out->text();
    data_app.path_dbf = ui->ln_path_dbf->text();
    data_app.path_to_TFOMS = ui->ln_path_to_TFOMS->text();
    data_app.path_from_TFOMS = ui->ln_path_from_TFOMS->text();

    // филиалы
    data_app.belg_host = ui->ln_belg_host->text();
    data_app.belg_port = ui->spin_belg_port->value();
    data_app.belg_base = ui->ln_belg_base->text();
    data_app.belg_user = ui->ln_belg_user->text();
    data_app.belg_pass = ui->ln_belg_pass->text();

    data_app.kursk_host = ui->ln_kursk_host->text();
    data_app.kursk_port = ui->spin_kursk_port->value();
    data_app.kursk_base = ui->ln_kursk_base->text();
    data_app.kursk_user = ui->ln_kursk_user->text();
    data_app.kursk_pass = ui->ln_kursk_pass->text();

    data_app.test_host = ui->ln_test_host->text();
    data_app.test_port = ui->spin_test_port->value();
    data_app.test_base = ui->ln_test_base->text();
    data_app.test_user = ui->ln_test_user->text();
    data_app.test_pass = ui->ln_test_pass->text();

    data_app.work_host = ui->ln_work_host->text();
    data_app.work_port = ui->spin_work_port->value();
    data_app.work_base = ui->ln_work_base->text();
    data_app.work_user = ui->ln_work_user->text();
    data_app.work_pass = ui->ln_work_pass->text();

    //сохраним данные в ini-файл
    // [polisesDB]
    settings.beginGroup("polisesDB");
    settings.setValue("host", data_app.host);
    settings.setValue("port", data_app.port);
    settings.setValue("base", data_app.base);
    settings.setValue("user", data_app.user);
    settings.setValue("pass", data_app.pass);
    settings.endGroup();
    // [flags]
    settings.beginGroup("flags");
    settings.setValue("is_tech", data_app.is_tech);
    settings.setValue("is_head", data_app.is_head);
    settings.setValue("is_locl", data_app.is_locl);
    settings.endGroup();
    // [paths]
    settings.beginGroup("paths");
    settings.setValue("path_install", data_app.path_install.toUtf8());
    settings.setValue("path_arch",    data_app.path_arch.toUtf8());
    settings.setValue("path_reports", data_app.path_reports.toUtf8());
    settings.setValue("path_temp", data_app.path_temp.toUtf8());
    settings.setValue("path_in",   data_app.path_in.toUtf8());
    settings.setValue("path_out",  data_app.path_out.toUtf8());
    settings.setValue("path_dbf",  data_app.path_dbf.toUtf8());
    settings.setValue("path_to_TFOMS",   data_app.path_to_TFOMS.toUtf8());
    settings.setValue("path_from_TFOMS", data_app.path_from_TFOMS.toUtf8());
    settings.endGroup();
    // [smo]
    settings.beginGroup("smo");
    settings.setValue("point_regnum", data_app.point_regnum);
    settings.setValue("point_name", data_app.point_name.toUtf8());
    settings.endGroup();
    // [filials]
    settings.beginGroup("filials");
    settings.setValue("bases_list", data_app.bases_list);
    settings.endGroup();
    if (data_app.bases_list.indexOf("kursk")>=0) {
        // [kursk]
        settings.beginGroup("kursk");
        settings.setValue("host", data_app.kursk_host);
        settings.setValue("port", QString::number(data_app.kursk_port));
        settings.setValue("base", data_app.kursk_base);
        settings.setValue("user", data_app.kursk_user);
        settings.setValue("pass", data_app.kursk_pass);
        settings.endGroup();
    }
    if (data_app.bases_list.indexOf("belg")>=0) {
        // [belg]
        settings.beginGroup("belg");
        settings.setValue("host", data_app.belg_host);
        settings.setValue("port", QString::number(data_app.belg_port));
        settings.setValue("base", data_app.belg_base);
        settings.setValue("user", data_app.belg_user);
        settings.setValue("pass", data_app.belg_pass);
        settings.endGroup();
    }
    if (data_app.bases_list.indexOf("test")>=0) {
        // [test]
        settings.beginGroup("test");
        settings.setValue("host", data_app.test_host);
        settings.setValue("port", QString::number(data_app.test_port));
        settings.setValue("base", data_app.test_base);
        settings.setValue("user", data_app.test_user);
        settings.setValue("pass", data_app.test_pass);
        settings.endGroup();
    }
    if (data_app.bases_list.indexOf("work")>=0) {
        // [work]
        settings.beginGroup("work");
        settings.setValue("host", data_app.work_host);
        settings.setValue("port", QString::number(data_app.work_port));
        settings.setValue("base", data_app.work_base);
        settings.setValue("user", data_app.work_user);
        settings.setValue("pass", data_app.work_pass);
        settings.endGroup();
    }
    settings.sync();

    close();
}
