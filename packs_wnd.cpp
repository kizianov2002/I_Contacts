#include "packs_wnd.h"
#include "ui_packs_wnd.h"

packs_wnd::packs_wnd(QSqlDatabase &db, s_data_app &data_app, QWidget *parent)
   : db(db), data_app(data_app), QDialog(parent), ui(new Ui::packs_wnd)
{
    ui->setupUi(this);

    ui->dateEdit->setDate(QDate::currentDate());
    refresh_packs_tab();
}

packs_wnd::~packs_wnd() {
    delete ui;
}

void packs_wnd::on_bn_close_clicked() {
    close();
}

void packs_wnd::refresh_packs_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString packs_sql = "select id, date_send, file_n, file_name, n_rec "
                        "  from files_out_g1 "
                        " where 1=1 ";

    if (ui->ch_date->isChecked()) {
        packs_sql += "   and date_send='" + ui->dateEdit->date().toString("yyyy-MM-dd") + "' ";
    }
    packs_sql += " order by id desc ; ";


    model_packs.setQuery(packs_sql,db);
    QString err2 = model_packs.lastError().driverText();

    // подключаем модель из БД
    ui->tab_packs->setModel(&model_packs);

    // обновляем таблицу
    ui->tab_packs->reset();

    // задаём ширину колонок
    ui->tab_packs->setColumnWidth( 0,  2);     // id,
    ui->tab_packs->setColumnWidth( 1,100);     // date_send
    ui->tab_packs->setColumnWidth( 2, 80);     // file_n,
    ui->tab_packs->setColumnWidth( 3,200);     // file_name
    ui->tab_packs->setColumnWidth( 4,100);     // file_name

    // правим заголовки
    model_packs.setHeaderData( 1, Qt::Horizontal, ("дата выгрузки"));
    model_packs.setHeaderData( 2, Qt::Horizontal, ("№ за день"));
    model_packs.setHeaderData( 3, Qt::Horizontal, ("имя фапйла"));
    model_packs.setHeaderData( 4, Qt::Horizontal, ("число строк"));
    ui->tab_packs->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void packs_wnd::on_ch_date_clicked(bool checked) {
    ui->dateEdit->setEnabled(checked);
    ui->bn_today->setEnabled(checked);
    refresh_packs_tab();
}

void packs_wnd::on_bn_today_clicked() {
    ui->dateEdit->setDate(QDate::currentDate());
    refresh_packs_tab();
}

void packs_wnd::on_dateEdit_dateChanged(const QDate &date) {
    refresh_packs_tab();

}

void packs_wnd::on_bn_refresh_clicked() {
    refresh_packs_tab();
}

void packs_wnd::on_bn_delete_pack_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_packs || !ui->tab_packs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите посылку из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_pack = ui->tab_packs->selectionModel()->selection().indexes();

    if (indexes_pack.size()>0) {
        QModelIndex index_pack = indexes_pack.front();
        db.transaction();

        // данные посылки
        int id_file_g1= model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).toInt();

        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Вы действительно хотите удалить все данные выбранного файла из базы данных обращений?\n\n"
                                  "Внимание - это необратимая операция. \nДанные afqkf будут полностью удалены из базы и новый файл может получить номер и имя, совпадающие с удалённымю\n"
                                  "Данная опция предназначена для удаления из базы файлов, которые так и небыли отправлены в фонд.\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QString sql_del = "";
        sql_del += "delete from files_out_g1 where id=" + QString::number(id_file_g1) + " ; ";
        QSqlQuery *query_del = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_del, "Удаление файла", *query_del, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке удалить данные файла произошла неожиданная ошибка.\n\n"
                                  "Удаление отменено.");
            delete query_del;
            return;
        }
        delete query_del;

        db.commit();

        refresh_packs_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void packs_wnd::on_bn_save_file_g1_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_packs || !ui->tab_packs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите посылку из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_pack = ui->tab_packs->selectionModel()->selection().indexes();

    if (indexes_pack.size()>0) {
        QModelIndex index_pack = indexes_pack.front();
        db.transaction();

        // данные посылки
        int id_file_g1= model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).toInt();


        QString sql_sel = "select file_name, bin "
                          "  from files_out_g1 "
                          " where id=" + QString::number(id_file_g1) + " ; ";

        QSqlQuery *query_sel = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_sel, "Получим файл посылки", *query_sel, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке получить файл посылки произошла неожиданная ошибка.\n\n"
                                  "Действие отменено.");
            delete query_sel;
            return;
        }
        if (query_sel->next()) {
            QString name_file_g1 = query_sel->value(0).toString();
            QByteArray arr = query_sel->value(1).toByteArray();
            QString fname_res = data_app.path_temp + name_file_g1 + ".zip";
            //mySQL.myUnBYTEA(BYTEA_str, fname_res);
            QFile f(fname_res);

            f.remove();
            if (!f.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this,
                                     "Ошибка создания файла",
                                     "При создании целевого файла произошла ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
            f.write(arr );
            f.close();
            long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
        } else {
            QMessageBox::warning(this, "Ничего не получено", "При попытке получить данныые файла ничего не получено.");
        }
        delete query_sel;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}
