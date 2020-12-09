#include "ki_sqlquerymodel.h"
#include <QTextCodec>
#include <QDate>
#include <QColor>
#include <QFont>

ki_persSqlQueryModel::ki_persSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_persSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    QModelIndex index_65 = index.sibling( index.row(),65 );
    QModelIndex index_66 = index.sibling( index.row(),66 );

    // выделим ВС с СМС, с заявкой фоном
    if( role==Qt::BackgroundRole ) {
        //int pol_v = QSqlQueryModel::data(index_06).toInt();
        bool no_zay = QSqlQueryModel::data(index_65).isNull();
        bool no_sms = QSqlQueryModel::data(index_66).isNull();

        /*if (!no_zay && !no_sms)
            return QColor( QColor(210,255,120) );
        else if (!no_sms)
            return QColor( QColor(255,255,160) );
        else*/ if (!no_zay)
            return QColor( QColor(210,255,230) );
        else
            return QColor( Qt::white );
    }

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==4) {
            if (value.toInt()==1) {
                return QString (" М");
            } else if (value.toInt()==2) {
                return QString ("  Ж");
            } else {
                return QString (" ?");
            }
        }
        if (index.column()==5) {
            if (value.toDate()==QDate(1900,1,1)) {
                return QString (" ");
            } else {
                return value.toDate();
            }
        }
        if (index.column()==7) {
            if (value.toDate()==QDate(1900,1,1)) {
                return QString (" ");
            } else {
                return value.toDate();
            }
        }
        if (index.column()==11) {
            if (value.toInt()==0) {
                return QString (" ");
            } else if (value.toInt()==1) {
                return QString (" бомж");
            } else {
                return QString (" ?");
            }
        }
        if (index.column()==21) {
            if (value.toInt()==0) {
                return QString ("~ новый");
            } else if (value.toInt()==1) {
                return QString ("+ ВС");
            } else if (value.toInt()==2) {
                return QString ("+ полис");
            } else if (value.toInt()==3) {
                return QString ("+ перестр.");
            } else if (value.toInt()==4) {
                return QString ("+ без полиса");
            } else if (value.toInt()==-1) {
                return QString ("- нет ");
            } else if (value.toInt()==-2) {
                return QString ("- др.СМО");
            } else if (value.toInt()==-3) {
                return QString ("- др.терр.");
            } else if (value.toInt()==-4) {
                return QString ("- закрыт ТФ");
            } else if (value.toInt()==-5) {
                return QString ("- отказ ТФ");
            } else if (value.toInt()==-6) {
                return QString ("- дубль");
            } else if (value.toInt()==-10) {
                return QString ("- умер");
            } else {
                return QString (" ?");
            }
        }
        if (index.column()==25) {
            if (value.toDate()==QDate(1900,1,1)) {
                return QString (" ");
            } else {
                return value.toDate();
            }
        }
        if (index.column()==64) {
            switch (value.toInt()) {
            case 1:
                return QString ("+ сам придёт");
                break;
            case 2:
                return QString ("!! ПО ПОЧТЕ");
                break;
            default:
                return QString (" - не указал");
                break;
            }
        }
    }
    return value;
}


ki_polSqlQueryModel::ki_polSqlQueryModel(QObject *parent) : QSqlQueryModel(parent) {
}

QVariant ki_polSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    /*QModelIndex index_00 = index.sibling( index.row(), 0 );
    QModelIndex index_01 = index.sibling( index.row(), 1 );
    QModelIndex index_02 = index.sibling( index.row(), 2 );
    QModelIndex index_03 = index.sibling( index.row(), 3 );
    QModelIndex index_04 = index.sibling( index.row(), 4 );
    QModelIndex index_05 = index.sibling( index.row(), 5 );*/
    QModelIndex index_06 = index.sibling( index.row(), 6 );
    /*QModelIndex index_07 = index.sibling( index.row(), 7 );
    QModelIndex index_08 = index.sibling( index.row(), 8 );
    QModelIndex index_09 = index.sibling( index.row(), 9 );
    QModelIndex index_10 = index.sibling( index.row(),10 );
    QModelIndex index_11 = index.sibling( index.row(),11 );
    QModelIndex index_12 = index.sibling( index.row(),12 );
    QModelIndex index_13 = index.sibling( index.row(),13 );
    QModelIndex index_14 = index.sibling( index.row(),14 );
    QModelIndex index_15 = index.sibling( index.row(),15 );
    QModelIndex index_16 = index.sibling( index.row(),16 );
    QModelIndex index_17 = index.sibling( index.row(),17 );
    QModelIndex index_18 = index.sibling( index.row(),18 );
    QModelIndex index_19 = index.sibling( index.row(),19 );
    QModelIndex index_20 = index.sibling( index.row(),20 );
    QModelIndex index_21 = index.sibling( index.row(),21 );
    QModelIndex index_22 = index.sibling( index.row(),22 );
    QModelIndex index_23 = index.sibling( index.row(),23 );
    QModelIndex index_24 = index.sibling( index.row(),24 );
    QModelIndex index_25 = index.sibling( index.row(),25 );
    QModelIndex index_26 = index.sibling( index.row(),26 );
    QModelIndex index_27 = index.sibling( index.row(),27 );
    QModelIndex index_28 = index.sibling( index.row(),28 );
    QModelIndex index_29 = index.sibling( index.row(),29 );*/
    QModelIndex index_30 = index.sibling( index.row(),30 );
    QModelIndex index_31 = index.sibling( index.row(),31 );
    QModelIndex index_32 = index.sibling( index.row(),32 );
    QModelIndex index_33 = index.sibling( index.row(),33 );

    // выделим ВС с СМС, с заявкой фоном
    if( role==Qt::BackgroundRole ) {
        int pol_v = QSqlQueryModel::data(index_06).toInt();
        bool no_zay = QSqlQueryModel::data(index_30).isNull();
        bool no_sms = QSqlQueryModel::data(index_31).isNull();

        /*if (!no_zay && !no_sms)
            return QColor( QColor(210,255,120) );
        else if (pol_v==2 && !no_sms)
            return QColor( QColor(255,255,160) );
        else*/ if (pol_v==2 && !no_zay)
            return QColor( QColor(210,255,230) );
        else
            return QColor( Qt::white );
    }

    // выделим не выданные полисы цветом
    if( role==Qt::ForegroundRole ) {
        int pol_v = QSqlQueryModel::data(index_06).toInt();
        bool no_get2hand = QSqlQueryModel::data(index_33).isNull();
        if (pol_v==3 && no_get2hand)
            return QColor( Qt::red );
        else if (pol_v==2)
            return QColor( Qt::blue );
        else
            return QColor( Qt::black );
    }

    // выделим не выданные полисы курсивом
    if( role==Qt::FontRole ) {
        int pol_v = QSqlQueryModel::data(index_06).toInt();
        bool no_get2hand = QSqlQueryModel::data(index_33).isNull();
        //QFont font = QFont("Tahoma", 8, -1, true);
        if (pol_v==3 && no_get2hand)
            return QFont("Tahoma", 8, -1, true);
        else if (pol_v==2)
            return QFont("Tahoma", 8, -1, false);
        else
            return QFont("Tahoma", 8, -1, false);
    }

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==1) {
            if (value.isNull()) {
                return QString ("");
            } else if (value.toInt()==-1) {
                return QString ("X(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==0) {
                return QString ("(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==1) {
                return QString ("+(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==4) {
                return QString ("+(") + QString::number(value.toInt()) + ")";
            } else {
                return QString ("-(") + QString::number(value.toInt()) + ")";
            }
        }
        if (index.column()==2) {
            if (value.isNull()) {
                return QString ("");
            } else if (value.toInt()==-1) {
                return QString ("X(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==0) {
                return QString ("(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==1) {
                return QString ("+(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==4) {
                return QString ("+(") + QString::number(value.toInt()) + ")";
            } else {
                return QString ("-(") + QString::number(value.toInt()) + ")";
            }
        }
        if ( index.column()==13 ||
             index.column()==14 ||
             index.column()==15 ) {
            if (value.isNull()) {
                return QString (" - ");
            } else if (value.toDate()<QDate::currentDate()) {
                return value.toDate().toString("dd.MM.yyyy");
            } else if (value.toDate()>QDate::currentDate()) {
                return QString("+") + value.toDate().toString("dd.MM.yyyy");
            } else {
                return QString("сегодня");
            }
        }
        if (index.column()==19) {
            if (value.isNull()) {
                return QString ("");
            } else if (value.toInt()==0) {
                return QString ("нет");
            } else if (value.toInt()==1) {
                return QString ("бумажн.");
            } else if (value.toInt()==2) {
                return QString ("карта");
            } else if (value.toInt()==3) {
                return QString ("УЭК");
            } else {
                return QString (" ? ");
            }
        }
    }

    return value;
}


ki_calendarSqlQueryModel::ki_calendarSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_calendarSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==2) {
            if (value.toInt()==0) {
                return QString ("выходной");
            } else if (value.toInt()==1) {
                return QString (" ");
            } else {
                return QString (" - ? - ");
            }
        }
    }
    return value;
}


ki_names2sexSqlQueryModel::ki_names2sexSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_names2sexSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==2) {
            if (value.toInt()==0) {
                return QString (" - ? - ");
            } else if (value.toInt()==1) {
                return QString ("М");
            } else {
                return QString (" Ж");
            }
        }
    }
    return value;
}


ki_talksSqlQueryModel::ki_talksSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_talksSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==3) {
            if (QSqlQueryModel::data(index, role).isNull()) {
                return QString (" - ? - ");
            } else if (value.toInt()==-1) {
                return QString (" - ? - ");
            } else if (value.toInt()==0) {
                return QString ("(-) бланк");
            } else if (value.toInt()==1) {
                return QString ("старый");
            } else if (value.toInt()==2) {
                return QString ("   ВС ");
            } else if (value.toInt()==3) {
                return QString ("  ЕНП ");
            }
        }
        if (index.column()==6) {
            if (value.toInt()==1) {
                return QString ("плановое СМС-оповещение о получении полиса");
            } else if (value.toInt()==2) {
                return QString ("неплановое СМС-оповещение");
            } else if (value.toInt()==3) {
                return QString ("звонок из СМО застрахованному");
            } else if (value.toInt()==4) {
                return QString ("визит работника СМО к застрахованному");
            } else if (value.toInt()==5) {
                return QString ("звонок застрахованного в СМО");
            }
        }
        if (index.column()==8) {
            if (value.toInt()==-3) {
                return QString ("(-)  номер / адрес не существует");
            } else if (value.toInt()==-2) {
                return QString ("(-)  чужой номер / адрес");
            } else if (value.toInt()==-1) {
                return QString ("(-)  недозвон / нет ответа");
            } else if (value.toInt()==1) {
                return QString ("(+)  ответил лично");
            } else if (value.toInt()==2) {
                return QString ("(+)  ответил родственик / знакомый");
            }
        }
    }
    return value;
}

