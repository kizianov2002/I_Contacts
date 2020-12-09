#include "print_monitoring_wnd.h"
#include "ui_print_monitoring_wnd.h"

print_monitoring_wnd::print_monitoring_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), QDialog(parent), settings(settings), ui(new Ui::print_monitoring_wnd)
{
    ui->setupUi(this);

    ui->ln_subj_rf->setText(data_app.subj_rf);
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_smo_short->setText(data_app.smo_short);

    ui->ch_federal_cc->setChecked(data_app.has_federal_cc>0);
    ui->ch_regional_cc->setChecked(data_app.has_regional_cc>0);

    ui->spin_w1->setValue(data_app.cnt_w1);
    ui->spin_w1_cc->setValue(data_app.cnt_w1_cc);
    ui->spin_w2->setValue(data_app.cnt_w2);

    ui->frame->setEnabled(false);
}

print_monitoring_wnd::~print_monitoring_wnd() {
    delete ui;
}

void print_monitoring_wnd::on_bn_close_clicked() {
    reject();
}

QString print_monitoring_wnd::date_to_str(QDate date) {
    QString res = QString::number(date.day()) + " ";
    switch (date.month()) {
    case 1:
        res += "января ";
        break;
    case 2:
        res += "февраля ";
        break;
    case 3:
        res += "марта ";
        break;
    case 4:
        res += "апреля ";
        break;
    case 5:
        res += "мая ";
        break;
    case 6:
        res += "июня ";
        break;
    case 7:
        res += "июля ";
        break;
    case 8:
        res += "августа ";
        break;
    case 9:
        res += "сентября ";
        break;
    case 10:
        res += "октября ";
        break;
    case 11:
        res += "ноября ";
        break;
    case 12:
        res += "декабря ";
        break;
    default:
        break;
    }
    res += QString::number(date.year()) + " г.";
    return res;
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_monitoring_wnd::on_bn_print_clicked() {
    this->setCursor(Qt::WaitCursor);


    // получим статистику по качеству приёма звонков
    // полное число звонков
    QString sql_all_1 = "select count(*) from contact_center "
                        " where id_filial=" + QString::number(data_app.id_filial) + " and req_way=1; ";
    QSqlQuery *query_all_1 = new QSqlQuery(db);

    if ( !mySQL.exec(this, sql_all_1, QString("Число принятых звонков"), *query_all_1, true, db, data_app) ) {
        delete query_all_1;
        QMessageBox::warning(this, "Ошибка при подсчёте числа принятых звонков",
                             "При подсчёте числа принятых звонков произошла неожиданная ошибка.\n\n" +
                             sql_all_1);
        return;
    }
    query_all_1->next();
    int cnt_all_1 = query_all_1->value(0).toInt();
    delete query_all_1;
    // число быстро принятых звонков
    QString sql_eff_1 = "select count(*) from contact_center "
                        " where id_filial=" + QString::number(data_app.id_filial) + " and req_way=1 and (check_efficiency is NULL or check_efficiency>0) ; ";
    QSqlQuery *query_eff_1 = new QSqlQuery(db);

    if ( !mySQL.exec(this, sql_eff_1, QString("Число быстро принятых звонков"), *query_eff_1, true, db, data_app) ) {
        delete query_eff_1;
        QMessageBox::warning(this, "Ошибка при подсчёте числа быстро принятых звонков",
                             "При подсчёте числа быстро принятых звонков произошла неожиданная ошибка.\n\n" +
                             sql_eff_1);
        return;
    }
    query_eff_1->next();
    int cnt_eff_1 = query_eff_1->value(0).toInt();
    delete query_eff_1;
    int eff_1_prc = ((float)cnt_eff_1)/((float)cnt_all_1)*100 +0.5;


    // получим статистику по качеству приёма посетителей
    // число принятых посетителей
    QString sql_all_4 = "select count(*) from contact_center "
                        " where id_filial=" + QString::number(data_app.id_filial) + " and req_way=4 ; ";
    QSqlQuery *query_all_4 = new QSqlQuery(db);

    if ( !mySQL.exec(this, sql_all_4, QString("Число принятых посетителей"), *query_all_4, true, db, data_app) ) {
        delete query_all_4;
        QMessageBox::warning(this, "Ошибка при подсчёте числа быстро принятых посетителей",
                             "При подсчёте числа принятых посетителей произошла неожиданная ошибка.\n\n" +
                             sql_all_4);
        return;
    }
    query_all_4->next();
    int cnt_all_4 = query_all_4->value(0).toInt();
    delete query_all_4;
    // число быстро принятых посетителей
    QString sql_eff_4 = "select count(*) from contact_center "
                        " where id_filial=" + QString::number(data_app.id_filial) + " and req_way=4 and (check_efficiency is NULL or check_efficiency>0) ; ";
    QSqlQuery *query_eff_4 = new QSqlQuery(db);

    if ( !mySQL.exec(this, sql_eff_4, QString("Число быстро принятых посетителей"), *query_eff_4, true, db, data_app) ) {
        delete query_eff_4;
        QMessageBox::warning(this, "Ошибка при подсчёте числа быстро принятых посетителей",
                             "При подсчёте числа быстро принятых посетителей произошла неожиданная ошибка.\n\n" +
                             sql_eff_4);
        return;
    }
    query_eff_4->next();
    int cnt_eff_4 = query_eff_4->value(0).toInt();
    delete query_eff_4;
    int eff_4_prc = ((float)cnt_eff_4)/((float)cnt_all_4)*100 +0.5;


    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_CallCenter.odt";


    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы отчёта: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    tmp_dir.mkpath(tmp_folder);
    QProcess myProcess;
    QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" content.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл контента шаблона\n" + rep_template + " -> content.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" styles.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл стилей шаблона \n" + rep_template + " -> styles.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }

    // ===========================
    // правка контента шаблона
    // ===========================
    // открытие контента шаблона
    QFile f_content(tmp_folder + "/content.xml");
    f_content.open(QIODevice::ReadOnly);
    QString s_content = f_content.readAll();
    f_content.close();
    if (s_content.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл контента пустой",
                             "Файл контента пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }

    // правка полей контента шаблона
    s_content = s_content.
        replace("#CUR_DATE#", date_to_str(QDate::currentDate())).
        replace("#SUBJECT_RF#", data_app.subj_rf).
        replace("#SMO_NAME#", data_app.smo_name).
        replace("#CNT_#", "").
        replace("#NFC#", QString::number(data_app.has_federal_cc)).
        replace("#NRC#", QString::number(data_app.has_regional_cc)).
        replace("#N_C1#", QString::number(data_app.cnt_w1)).
        replace("#N_C1+#", QString::number(data_app.cnt_w1_cc)).
        replace("#N_C2#", QString::number(data_app.cnt_w2)).
        replace("#%CALL_2-#", QString::number(eff_1_prc)).
        replace("#%CALL_8+#", QString::number(eff_4_prc));

    // получим данные
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select count(*) "
           " from contact_center "
           " where id_filial=" + QString::number(data_app.id_filial) + " "
           "   and req_way in(1,4) ; ";
    mySQL.exec(this, sql, QString("Статисттика обращений"), *query, true, db, data_app);
    query->next();
    int cnt0 = query->value(0).toInt();
    delete query;

    query = new QSqlQuery(db);
    sql = "";
    sql += "select count(*) "
           "  from contact_center "
           " where id_filial=" + QString::number(data_app.id_filial) + " "
           "   and req_way not in(1,4); ; ";
    mySQL.exec(this, sql, QString("Статисттика обращений"), *query, true, db, data_app);
    query->next();
    int cnt1 = query->value(0).toInt();
    delete query;

    // добавим эти данные в отчёт
    s_content = s_content.
        replace("#N_CALL#", QString::number(cnt0)).
        replace("#N_LETR#", QString::number(cnt1));

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/monitoring - " + QDate::currentDate().toString("yyyy-MM-dd") + ".odt";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения печатной формы отчёта",
                                 "Не удалось сохранить шаблон печатной формы отчёта: \n" + rep_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы отчёта: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон печатной формы отчёта произошла непредвиденная ошибка\n\nОпреация прервана.");
    }


    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии файла печатной формы отчёта",
                             "Файл печатной формы отчёта не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
    close();
}

void print_monitoring_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");
        ui->frame->setEnabled(true);
        ui->bn_print->setEnabled(false);
    } else {
        data_app.subj_rf = ui->ln_subj_rf->text();
        data_app.smo_name = ui->ln_smo_name->text();
        data_app.smo_short = ui->ln_smo_short->text();
        data_app.has_federal_cc  = ui->ch_federal_cc->isChecked() ? 1 : 0;
        data_app.has_regional_cc = ui->ch_regional_cc->isChecked() ? 1 : 0;
        data_app.cnt_w1 = ui->spin_w1->value();
        data_app.cnt_w1_cc = ui->spin_w1_cc->value();
        data_app.cnt_w2 = ui->spin_w2->value();

        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Сохранить сделанные изменения в настройках программы?\n\n"
                                  "В случае согласия новые данные будут сохранены и останутся доступны после перезагрузки программы.\n"
                                  "В случае отказа эти изменения будут доступны только до первого перезапуска программы.",
                                  QMessageBox::Yes|QMessageBox::No,
                                  QMessageBox::No)==QMessageBox::Yes)
        {
            // [req_ch_smo]
            settings.beginGroup("ContactCenter");
            settings.setValue("subj_rf", data_app.subj_rf.toUtf8());
            settings.setValue("smo_name", data_app.smo_name.toUtf8());
            settings.setValue("smo_short", data_app.smo_short.toUtf8());
            settings.setValue("has_federal_cc", data_app.has_federal_cc>0 ? QString::number(data_app.has_federal_cc) : "");
            settings.setValue("has_regional_cc", data_app.has_regional_cc>0 ? QString::number(data_app.has_regional_cc) : "");
            settings.setValue("cnt_w1", data_app.cnt_w1>0 ? QString::number(data_app.cnt_w1) : "");
            settings.setValue("cnt_w1_cc", data_app.cnt_w1_cc>0 ? QString::number(data_app.cnt_w1_cc) : "");
            settings.setValue("cnt_w2", data_app.cnt_w2>0 ? QString::number(data_app.cnt_w2) : "");
            settings.endGroup();
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->frame->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

void print_monitoring_wnd::on_bn_cansel_clicked() {
    reject();
}
