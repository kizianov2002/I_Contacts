#include "send2fond_wnd.h"
#include "ui_send2fond_wnd.h"

send2fond_wnd::send2fond_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::send2fond_wnd)
{
    ui->setupUi(this);

    ui->date_from->setDate(QDate::currentDate().addMonths(-1));
    ui->date_to->setDate(QDate::currentDate());
    on_date_to_editingFinished();

    refresh_combo_filial();
}

send2fond_wnd::~send2fond_wnd() {
    delete ui;
}

void send2fond_wnd::refresh_combo_filial() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, filial_name "
                  "  from public.filials "
                  " order by id ; ";
    mySQL.exec(this, sql, QString("Список филиалов"), *query, true, db, data_app);
    ui->combo_filial->clear();
    ui->combo_filial->addItem(" - все филиалы - ", -1);
    while (query->next()) {
        ui->combo_filial->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    delete query;
    int n = ui->combo_filial->findData(data_app.id_filial);
    ui->combo_filial->setCurrentIndex(n);
    this->setCursor(Qt::ArrowCursor);
}

void send2fond_wnd::on_bn_close_clicked() {
    close();
}

void send2fond_wnd::on_bn_process_clicked() {
    switch (ui->combo_filial->currentData().toInt()) {
    case 1:
        gen_Voronej();
        break;
    case 2:
        gen_Belgorod();
        break;
    case 3:
        gen_Kursk();
        break;
    default:
        break;
    }
}


bool send2fond_wnd::gen_Voronej() {

}

bool send2fond_wnd::gen_Belgorod() {

    // подготовка выгрузки DBF
    ui->lab_dst->setText("ODBC");
    ui->lab_cnt->setText("-//-");
    QApplication::processEvents();
    int cnt_KSP = 0;

    // подключимся к ODBC
    db_ODBC = QSqlDatabase::addDatabase("QODBC", "INKO_DBF");
    db_ODBC.setConnectOptions();
    // имя источника данных
    db_ODBC.setDatabaseName("INKO_DBF");
    while (!db_ODBC.open()) {
        if (QMessageBox::warning(this, "Не найден ODBC-источник данных INKO_DBF",
                                 QString("Источник данных должен использовать папку, указанную в параметре программы \"path_dbf\":\n"
                                 "При попытке подключиться к испточнику данных ODBC \"INKO_DBF\" операционная система вернула отказ. \n"
                                 "Этот источник данных необходим для генерации и чтения dbf-файлов обмена с ТФОМС. \n"
                                 "Если такого источника данных нет - создайте его.\n\n")
                                 + data_app.path_dbf + "\n\n"
                                 "Попробовать снова?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::No) {

            ui->bn_process->setEnabled(true);
            return false;
        }
    }

    ui->lab_dst->setText("KSP.dbf");
    ui->lab_cnt->setText("-//-");
    QApplication::processEvents();

    // обновим файл DBF
    QFile::remove(data_app.path_dbf + "/KSP.DBF");
    QFile::copy(data_app.path_dbf + "/KSP_src.DBF", data_app.path_dbf + "/KSP.DBF");

    // очистим таблицу
    /*
    QString sql_del_KSP = "delete from KSP ; ";
    QSqlQuery *query_del_KSP = new QSqlQuery(db_ODBC);
    bool res_del_KSP = query_del_KSP->exec(sql_del_KSP);
    QString err = db_ODBC.lastError().databaseText();
    if (!res_del_KSP) {
        delete query_del_KSP;
        QMessageBox::warning(this, "Ошибка очистки таблицы KSP",
                             "При попытке очистить таблицу KSP произошла ошибка!\n\n"
                             "Операция отменена");
        db_ODBC.close();
        QSqlDatabase::removeDatabase("INKO_DBF");

        ui->bn_process->setEnabled(true);
        db_ODBC.close();
        QSqlDatabase::removeDatabase("INKO_DBF");
        return false;
    }
    delete query_del_KSP;
    */





    // получим количество уже выгруженных файлов

    QString sql_fN = "select coalesce(max(file_n), 0) as file_n "
                      "  from files_out_g1 f "
                      " where date_send=CURRENT_DATE ; ";

    QSqlQuery *query_fN = new QSqlQuery(db);
    bool res_fN = query_fN->exec(sql_fN);
    QString err = db.lastError().databaseText();
    if (!res_fN) {
        delete query_fN;
        QMessageBox::warning(this, "Ошибка при проверке номера последнего выгруженного файла",
                             "При проверке номера последнего выгруженного файла произошла ошибка!\n\n"
                             "Операция отменена");

        ui->bn_process->setEnabled(true);
        db_ODBC.close();
        QSqlDatabase::removeDatabase("INKO_DBF");
        db.rollback();
        return false;
    }
    query_fN->next();
    int file_num = query_fN->value(0).toInt();
    file_num++;
    delete query_fN;

    // подготовка выгрузки XML
    QString file_name = "G131006_" + QDateTime::currentDateTime().toString("MMyydd") + QString("0000" + QString::number(file_num)).right(4);
    QString xml_name = file_name + ".xml";
    QString xml = "Проверка кодировки";
    QXmlStreamWriter stream(&xml);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("IRP_LIST"); {
        stream.writeStartElement("ZGLV"); {
            stream.writeTextElement("VERSIA", "1.0");
            stream.writeTextElement("DATA", QDate::currentDate().toString("yyyy-MM-dd"));
            stream.writeTextElement("YEAR", QDate::currentDate().toString("yyyy"));
            stream.writeTextElement("MONTH", QDate::currentDate().toString("MM"));
            stream.writeTextElement("DAY", QDate::currentDate().toString("dd"));
            stream.writeTextElement("TIME", QTime::currentTime().toString("hh:mm"));
            stream.writeTextElement("SMO", "31006");
            stream.writeTextElement("FILENAME", xml_name.toUpper());
        } stream.writeEndElement();

        // сформируем список новых обращений
        db.transaction();
        QString sql_KSP = "select id, fam, im, ot, date_birth, sex, coalesce(pol_sernom, pol_enp), doc_sernom, phone, year_birth, "
                          "       req_way, req_reason_code, req_dt, req_petition, disp_fio, req_comment, address_reg, email, soc_status, "
                          "       case talker_form when 0 then 1 else talker_form end as talker_form, "
                          "       req_kladr_subj, req_kladr_dstr, req_kladr_city, req_kladr_nasp, req_kladr_strt, req_location, "
                          "       req_way, req_date_incident, req_perform, req_performer, req_date_control, req_date_done, req_result_code, "
                          "       dt_ins, pol_enp, doc_type, n_irp "
                          "  from contact_center "
                          " where id_filial=" + QString::number(ui->combo_filial->currentData().toInt()) + " ";
        if (ui->rb_new->isChecked())
            sql_KSP   +=  "   and ((status is NULL) or (status=0)) ";
        else
            sql_KSP   +=  "   and dt_ins>='" + ui->date_from->dateTime().toString("yyyy-MM-dd hh:mm:ss") + "' "
                          "   and dt_ins<='" + ui->date_to->dateTime().toString("yyyy-MM-dd hh:mm:ss")   + "' "
                          " order by dt_ins ; ";

        QSqlQuery *query_KSP = new QSqlQuery(db);
        bool res_KSP = query_KSP->exec(sql_KSP);
        QString err = db.lastError().databaseText();
        if (!res_KSP) {
            delete query_KSP;
            QMessageBox::warning(this, "Ошибка получения данных новых обращений",
                                 "При попытке получить данные новых обращений произошла ошибка!\n\n"
                                 "Операция отменена");

            ui->bn_process->setEnabled(true);
            db_ODBC.close();
            QSqlDatabase::removeDatabase("INKO_DBF");
            db.rollback();
            return false;
        }
        while (query_KSP->next()) {
            QString petition  = query_KSP->value(13).toString();
            /*QString petition1 = petition.mid(0,250).simplified() + " ";
            QString petition2 = petition.mid(250,250).simplified() + " ";
            QString petition3 = petition.mid(500,250).simplified() + " ";
            QString petition4 = petition.mid(750,250).simplified() + " ";*/

            /*QString result  = query_KSP->value(32).toString();
            QString result1 = result.mid(0,250).simplified() + " ";
            QString result2 = result.mid(250,250).simplified() + " ";
            QString result3 = result.mid(500,250).simplified() + " ";
            QString result4 = result.mid(750,250).simplified() + " ";*/

            // код района
            int subj = query_KSP->value(19).toInt();
            int dstr = query_KSP->value(20).toInt();
            int reg = (subj==31 ? dstr : 0);

            // добавим запись в XML
            stream.writeStartElement("IRP"); {
                stream.writeTextElement("N_IRP", query_KSP->value(36).toString());
                stream.writeTextElement("TF_ID", "");
                stream.writeTextElement("IRP_TYPE", QString::number(query_KSP->value(10).toInt()));
                stream.writeTextElement("DATE_CREATE", (query_KSP->value(33).toDate().toString("yyyy-MM-dd")));
                stream.writeTextElement("TIME_CREATE", (query_KSP->value(33).toTime().toString("hh:mm")));
                stream.writeTextElement("WAY", "1"/*QString::number(query_KSP->value(26).toInt())*/);
                stream.writeTextElement("WAY_N", "");
                stream.writeTextElement("HOW", QString::number(query_KSP->value(10).toInt()));

                if (!query_KSP->value(11).toString().isEmpty())
                    stream.writeTextElement("THEME", query_KSP->value(11).toString().replace(" ",""));
                else  stream.writeTextElement("THEME", "");

                stream.writeTextElement("TEXT", petition);
                stream.writeTextElement("ZH_D", "0");
                stream.writeTextElement("OTV_T", "2");
                stream.writeTextElement("OTV_KON", "31006");
                stream.writeTextElement("EMPLOYEE_1", query_KSP->value(14).toString());

                if (!query_KSP->value(29).toString().isEmpty())
                    stream.writeTextElement("EMPLOYEE_2", query_KSP->value(29).toString());
                else  stream.writeTextElement("EMPLOYEE_2", "");

                stream.writeStartElement("Z_SV"); {
                    if ( !query_KSP->value(1).isNull() && query_KSP->value(1).toString()!="0" )
                        stream.writeTextElement("Z_F", query_KSP->value(1).toString());
                    else stream.writeTextElement("Z_F", "");

                    if ( !query_KSP->value(2).isNull() && query_KSP->value(2).toString()!="0" )
                        stream.writeTextElement("Z_I", query_KSP->value(2).toString());
                    else stream.writeTextElement("Z_I", "");

                    if ( !query_KSP->value(3).isNull() && query_KSP->value(3).toString()!="0" )
                        stream.writeTextElement("Z_O", query_KSP->value(3).toString());
                    else stream.writeTextElement("Z_O", "");

                    if ( !query_KSP->value(4).isNull() && !query_KSP->value(4).toDate().isValid() && query_KSP->value(4).toDate()<QDate(1920,1,1) )
                        stream.writeTextElement("Z_DR", query_KSP->value(4).toDate().toString("yyyy-MM-dd"));
                    else stream.writeTextElement("Z_DR", "");

                    if ( !query_KSP->value(34).isNull() && query_KSP->value(34).toString().length()!=16 )
                        stream.writeTextElement("Z_ENP", query_KSP->value(34).toString());
                    else stream.writeTextElement("Z_ENP", "");

                    stream.writeTextElement("Z_SMO", "31006");

                    if ( !query_KSP->value(35).isNull() && query_KSP->value(35).toString()!="0" )
                         stream.writeTextElement("Z_DOCTYPE", QString("00" + query_KSP->value(35).toString()).right(2));
                    else stream.writeTextElement("Z_DOCTYPE", "");

                    //stream.writeEmptyElement("Z_DOCSER");
                    stream.writeTextElement("Z_DOCSER", "");

                    if ( !query_KSP->value(8).isNull() )
                        stream.writeTextElement("Z_DOCNUM", query_KSP->value(8).toString());
                    else stream.writeTextElement("Z_DOCNUM", "");

                    stream.writeTextElement("ADR", "");
                    stream.writeTextElement("PHONE", "");
                    stream.writeTextElement("E-MAIL", "");
                } stream.writeEndElement();
                stream.writeStartElement("IN_SV"); {
                    stream.writeTextElement("IN_F", "");
                    stream.writeTextElement("IN_I", "");
                    stream.writeTextElement("IN_O", "");
                    stream.writeTextElement("IN_DR", "");
                    stream.writeTextElement("IN_ENP", "");
                    stream.writeTextElement("IN_SMO", "");
                    stream.writeTextElement("IN_DOCTYPE", "");
                    stream.writeTextElement("IN_DOCSER", "");
                    stream.writeTextElement("IN_DOCNUM", "");
                } stream.writeEndElement();

                if ( !query_KSP->value(30).isNull() )
                     stream.writeTextElement("DATA_PLAN", (query_KSP->value(30).toDate().toString("yyyy-MM-dd")));
                else stream.writeTextElement("DATA_PLAN", (query_KSP->value(33).toDate().addDays(1).toString("yyyy-MM-dd")));

                if ( !query_KSP->value(31).isNull() )
                     stream.writeTextElement("DATE_CLOSE", (query_KSP->value(31).toDate().toString("yyyy-MM-dd")));
                else stream.writeTextElement("DATE_CLOSE", "");

                if ( query_KSP->value(32).toInt()>0 )
                     stream.writeTextElement("RESULT", QString::number(query_KSP->value(32).toInt()) );
                //else stream.writeEmptyElement("RESULT");
                else stream.writeTextElement("RESULT", "1");

            } stream.writeEndElement();
            /*
            // добавим в файл для отправки
            QString sql_ins_KSP = "";
            sql_ins_KSP += "insert into KSP "
                           " ( IDREQ, NNREQ, "
                           "   FAM, IM, OT, DR, W, SN_POL, SN_PAS, PHONENUM, BIRTHYEAR, REQ_WAY, REASON, \"DATE\", \"TIME\", "
                           "   PETITION1, PETITION2, PETITION3, PETITION4, DISP, SCOMMENT, ADRESS, EMAIL, SOCSTATUS, "
                           "   REGION, FIZUR, LOCATION, TYPEREQ, DATEDONE, PORUCH, WPORUCH, CONTROLDAT, DONEDATE, "
                           "   RESULT1, RESULT2, RESULT3, RESULT4 "
                           " ) "
                           " values "
                           " ( " + (query_KSP->value( 0).isNull() ? QString("NULL") : QString::number(query_KSP->value(0).toInt())) + ", "                                              // id
                             " " + (query_KSP->value( 0).isNull() ? QString("NULL") : QString("'" + QString::number(query_KSP->value(0).toInt()) + "'")) +  ", "                        // nreq
                             " " + (query_KSP->value( 1).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(1).toString().trimmed().simplified() + "'")) + ", "   // fam
                             " " + (query_KSP->value( 2).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(2).toString().trimmed().simplified() + "'")) + ", "   // im
                             " " + (query_KSP->value( 3).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(3).toString().trimmed().simplified() + "'")) + ", "   // ot
                             " " + (query_KSP->value( 4).isNull() ? QString("NULL") : QString("Format('" + query_KSP->value(4).toDate().toString("dd.MM.yy") + "', 'dd.mm.yy')")) + ", "         // dr
                             " " + (query_KSP->value( 5).isNull() ? QString("NULL") : QString::number(query_KSP->value(5).toInt())) + ", "                                              // w
                             " " + (query_KSP->value( 6).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(6).toString().trimmed().simplified() + "'")) + ", "   // pol_sernom
                             " " + (query_KSP->value( 7).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(7).toString().trimmed().simplified() + "'")) + ", "   // doc_sernom
                             " " + (query_KSP->value( 8).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(8).toString().trimmed().simplified() + "'")) + ", "   // phone
                             " " + (query_KSP->value( 9).isNull() ? QString("NULL") : QString::number(query_KSP->value(9).toInt())) + ", "                                              // year_birth
                             " " + (query_KSP->value(10).toString().trimmed().isEmpty()?"NULL":QString::number(query_KSP->value(10).toInt()))+", "    // req_way
                             " " + (query_KSP->value(11).toString().trimmed().isEmpty()?"NULL":QString("'"+query_KSP->value(11).toString().trimmed().simplified().replace(" ","")+"'"))+", "    // req_reason
                             " " + (query_KSP->value(12).isNull() ? QString("NULL") : QString("Format('" + query_KSP->value(12).toDate().toString("dd.MM.yyyy") + "', 'dd.mm.yy')")) + ", "     // req_date
                             " " + (query_KSP->value(12).isNull() ? QString("NULL") : QString("'" + query_KSP->value(12).toTime().toString("hh:mm:ss")) + "'") + ", "                 // req_time

                             " " + (petition1.isEmpty() ? "NULL" : QString("'" + petition1.simplified() + "'")) + ", "                                             // petition
                             " " + (petition2.isEmpty() ? "NULL" : QString("'" + petition2.simplified() + "'")) + ", "                                             // petition
                             " " + (petition3.isEmpty() ? "NULL" : QString("'" + petition3.simplified() + "'")) + ", "                                             // petition
                             " " + (petition4.isEmpty() ? "NULL" : QString("'" + petition4.simplified() + "'")) + ", "                                             // petition
                             " " + (query_KSP->value(14).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(14).toString().trimmed().simplified() + "'")) + ", " // disp

                             " " + (query_KSP->value(15).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(15).toString().trimmed().simplified() + "'")) + ", " // comment

                             " " + (query_KSP->value(16).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(16).toString().trimmed().simplified() + "'")) + ", " // address
                             " " + (query_KSP->value(17).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(17).toString().trimmed().simplified() + "'")) + ", " // e-mail
                             " " + (query_KSP->value(18).isNull() ? QString("NULL") : QString::number(query_KSP->value(18).toInt())) + ", "                                            // socstatus

                             " " + QString::number(reg) + ", "                                                                                                                         // region
                             " " + (query_KSP->value(19).isNull() ? QString("1") : QString::number(query_KSP->value(19).toInt())) + ", "                                               // talker_form
                             " " + (query_KSP->value(25).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(25).toString().trimmed().simplified() + "'")) + ", " // location
                             " " + (query_KSP->value(26).isNull() ? QString("NULL") : QString::number(query_KSP->value(26).toInt())) + ", "                                            // req_way
                             " " + (query_KSP->value(27).isNull() ? QString("NULL") : QString("Format('" + query_KSP->value(27).toDate().toString("dd.MM.yyyy") + "', 'dd.mm.yy')")) + ", "    // date_incident
                             " " + (query_KSP->value(28).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(28).toString().trimmed().simplified() + "'")) + ", " // perform
                             " " + (query_KSP->value(29).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query_KSP->value(29).toString().trimmed().simplified() + "'")) + ", " // performer
                             " " + (query_KSP->value(30).isNull() ? QString("NULL") : QString("Format('" + query_KSP->value(30).toDate().toString("dd.MM.yyyy") + "', 'dd.mm.yy')")) + ", "    // date_control
                             " " + (query_KSP->value(31).isNull() ? QString("NULL") : QString("Format('" + query_KSP->value(31).toDate().toString("dd.MM.yyyy") + "', 'dd.mm.yy')")) + ", "    // date_done

                             " " + (result1.isEmpty() ? "NULL" : QString("'" + result1 + "'")) + ", "                                                 // result
                             " " + (result2.isEmpty() ? "NULL" : QString("'" + result2 + "'")) + ", "                                                 // result
                             " " + (result3.isEmpty() ? "NULL" : QString("'" + result3 + "'")) + ", "                                                 // result
                             " " + (result4.isEmpty() ? "NULL" : QString("'" + result4 + "'")) +                                                      // result

                            " ) ";

            QSqlQuery *query_ins_KSP = new QSqlQuery(db_ODBC);
            bool res_ins_KSP = query_ins_KSP->exec(sql_ins_KSP);
            if (!res_ins_KSP) {
                QString err = query_ins_KSP->lastError().driverText() + "   -   " + query_ins_KSP->lastError().databaseText();
                QMessageBox::warning(this, "Ошибка добавления строки обращения",
                                     "При попытке добавить строку обращения в файл обмена произошла ошибка!"
                                     "\n\nDB.driver:  " + db_ODBC.lastError().driverText() + "\nDB.database:  " + db_ODBC.lastError().databaseText() +
                                     "\n\ndriver:  " + query_ins_KSP->lastError().driverText() + "\ndatabase:  " + query_ins_KSP->lastError().databaseText() +
                                     "\n\nОперация отменена");
                delete query_ins_KSP;

                ui->bn_process->setEnabled(true);
                db_ODBC.close();
                QSqlDatabase::removeDatabase("INKO_DBF");
                db.rollback();
                return false;
            }
            delete query_ins_KSP;
            */
            cnt_KSP++;
        }
        delete query_KSP;

        if (ui->rb_new->isChecked()) {
            // поправим статус отправленных обращений
            QString sql_stat_KSP = "update contact_center s "
                                   "   set status=1 "
                                   " where status is NULL "
                                   "    or status=0 ; ";
            QSqlQuery *query_stat_KSP = new QSqlQuery(db);
            bool res_stat_KSP = query_stat_KSP->exec(sql_stat_KSP);
            err = db.lastError().databaseText();
            if (!res_stat_KSP) {
                delete query_stat_KSP;
                QMessageBox::warning(this, "Ошибка обновления статуса отправленных обращения, отправленных в фонд",
                                     "При попытке обновления статуса отправленных обращения произошла ошибка!\n\n"
                                     "Операция отменена");
                delete query_stat_KSP;

                ui->bn_process->setEnabled(true);
                db.rollback();
                db_ODBC.close();
                QSqlDatabase::removeDatabase("INKO_DBF");
                return false;
            }
        }

        // скопируем список новых имён в папку OUT
        //QFile::copy(data_app.path_dbf + "/KSP.DBF", data_app.path_out + "/KSP.DBF");



        db.commit();

    } stream.writeEndElement(); // IRP_LIST
    //...
    stream.writeEndDocument();


    QString msg_s = "";


    /*if (ui->ch_dbf->isChecked()) {
        QFile::remove(data_app.path_out + "KSP.dbf");
        if (cnt_KSP>0) {
            QString fname = "KSP31006_" + QDate::currentDate().toString("ddMMyyyy") + QTime::currentTime().toString("hhmmss") + ".dbf";
            QFile::copy(data_app.path_dbf + "KSP.dbf", data_app.path_out + fname);

            msg_s += "\nВыгрузка DBF готова\n"
                     "Выбрано  " + QString::number(cnt_KSP) + "  записей.  \nФайл выгрузки  -  " + fname + "  \nсохранён в обменную папку  -  " + data_app.path_out + "\n";
        } else {
            msg_s += "\nПри выгрузке DBF ничего не выбрано\n"
                     "В базе данных не найдено ни одной новой записи - операция отменена.";
        }
    }*/



    if (ui->ch_xml->isChecked()) {
        // подготовка сохранения файла
        QString s_out_xml = data_app.path_out + xml_name;
        QFile::remove(s_out_xml);
        while (QFile::exists(s_out_xml)) {
            if (QMessageBox::warning(this,
                                     "Ошибка выгрузки файла",
                                     "Не удалось сохранить файл выгрузки: \n" + s_out_xml +
                                     "\n\nво временную папку\n" + data_app.path_temp +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                db_ODBC.close();
                QSqlDatabase::removeDatabase("INKO_DBF");
                return false;
            }
            QFile::remove(s_out_xml);
        }

        // сохраним файл на диск
        QFile *f_xml = new QFile(s_out_xml);
        f_xml->open(QIODevice::WriteOnly);
        QTextCodec *codec = QTextCodec::codecForName("cp1251");
        xml = xml.replace("<?xml version=\"1.0\"?>", "<?xml version=\"1.0\" encoding=\"windows-1251\"?>");
        xml = xml.replace("Проверка кодировки", "");
        QByteArray encodedString = codec->fromUnicode(xml);

        f_xml->write(encodedString);
        f_xml->close();

        // сформируем архив
        QProcess myProcess;
        QDir d_out = data_app.path_out;
        QString s_out_zip = data_app.path_temp + file_name + ".zip";
        QFile::remove(s_out_zip);
        while (QFile::exists(s_out_zip)) {
            if (QMessageBox::warning(this,
                                     "Ошибка выгрузки файла",
                                     "Не удалось заархивировать файл выгрузки: \n" + s_out_zip +
                                     "\n\nво временную папку\n" + data_app.path_temp +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                db_ODBC.close();
                QSqlDatabase::removeDatabase("INKO_DBF");
                return false;
            }
            QFile::remove(s_out_zip);
        }
        QString com = "\"" + data_app.path_arch + "7z.exe\" a \"" + s_out_zip + "\" \"" + s_out_xml + "\"";
        while (myProcess.execute(com)!=0) {
            if (QMessageBox::warning(this,
                                     "Ошибка архивации контента",
                                     "При формировании ZIP-архива произошла непредвиденная ошибка\n\nОпреация прервана."
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                db_ODBC.close();
                QSqlDatabase::removeDatabase("INKO_DBF");
                return false;
            }
        }

        QFile::remove(data_app.path_out + file_name + ".zip");
        if (cnt_KSP>0) {
            QString fname = data_app.path_out + file_name + ".zip";
            QFile::copy(s_out_zip, fname);
            QFile::remove(s_out_xml);

            msg_s += "\nВыгрузка XML готова.\n"
                     "Выбрано  " + QString::number(cnt_KSP) + "  записей.  \nФайл выгрузки  -  " + fname + "  \nсохранён в обменную папку  -  " + data_app.path_out + "\n";
        } else {
            msg_s += "\nПри выгрузке XML ничего не выбрано\n"
                     "В базе данных не найдено ни одной новой записи - операция отменена.";
        }




        // сохраним выгруженныю посылку в базу данных
        QString bin;
        QString full_name = data_app.path_out + file_name + ".zip";
        mySQL.myBYTEA(full_name, bin);
        QString sql_file;
        sql_file = "insert into files_out_g1 (file_name, date_send, file_n, n_rec, bin) ";
        sql_file+= " values("
                        "'" + file_name + "', "
                        "'" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
                        " " + QString::number(file_num) + ", "
                        " " + QString::number(cnt_KSP) + ", "
                        "'" + bin + "') ";
        sql_file+= " returning id ; ";

        QSqlQuery *query_file = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_file, "Запись резервной копии выгруженного файла в базу полисов", *query_file, true, db, data_app) || !query_file->next()) {
            delete query_file;
            QMessageBox::warning(this, "Операция отменена", "При записи резервной копии выгруженного файла в базу полисов произошла неожиданная ошибка.\n\nОперация выгрузки данных для ТФОМС отменена.");
            db.rollback();
            return 0;
        }
        int id_file_i = query_file->value(0).toInt();
    }

    // закроем соединение с ODBC
    db_ODBC.close();
    QSqlDatabase::removeDatabase("INKO_DBF");


    QMessageBox::information(this, "Выгрузка данных", msg_s);
    if (cnt_KSP>0) {
        // =========================
        // открытие папки сохранения
        // =========================
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(data_app.path_out.utf16()), 0, 0, SW_NORMAL);
    }

    this->close();
    return true;
}

void send2fond_wnd::on_rb_by_dates_clicked(bool checked) {
    ui->pan_dates->setEnabled(checked);
}

void send2fond_wnd::on_rb_new_clicked(bool checked) {
    ui->pan_dates->setEnabled(!checked);
}


bool send2fond_wnd::gen_Kursk() {

}

void send2fond_wnd::on_date_from_editingFinished() {
    QDate date_from = ui->date_from->date();
    QDate date_to = ui->date_to->date();

    if (date_to<date_from)
        date_to = date_from;
    if (date_to>QDate::currentDate())
        date_to = QDate::currentDate();
    if (date_to<date_from)
        date_from = date_to;

    ui->date_from->setDate(date_from);
    ui->date_to->setDate(date_to);

    int days = date_from.daysTo(date_to) +1;
    ui->lab_days->setText(QString::number(days));
}

void send2fond_wnd::on_date_to_editingFinished() {
    QDate date_from = ui->date_from->date();
    QDate date_to = ui->date_to->date();

    if (date_to<date_from)
        date_from = date_to;
    if (date_to>QDate::currentDate())
        date_to = QDate::currentDate();
    if (date_to<date_from)
        date_to = date_from;

    ui->date_from->setDate(date_from);
    ui->date_to->setDate(date_to);

    int days = date_from.daysTo(date_to) +1;
    ui->lab_days->setText(QString::number(days));
}
