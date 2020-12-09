#ifndef S_DATA_APP
#define S_DATA_APP

#include <QDate>

typedef struct {
    bool screen_small;
    int screen_w, screen_h;

    // ini
    QString ini_path;
    QString version;

    bool is_tech;
    bool is_head;
    bool is_locl;
    bool is_call;

    // данные подключения к БД
    QString host, base, user, pass;
    int port;

    QString db_host;
    QString db_name;
    QString db_vers;

    QString path_install;
    QString path_arch;
    QString path_reports;
    QString path_temp;
    QString path_in;
    QString path_out;
    QString path_dbf;

    QString path_to_TFOMS;      // папка автопроцессинга для отправки запросов в ТФОМС - проверка новых клиентов в региональном сегменте
    QString path_from_TFOMS;    // папка автопроцессинга для приёма ответов от ТФОМС   - проверка новых клиентов в региональном сегменте

    int log_bord;       // минимальное времы испольнения запроса для занесения в журнал


    QString subj_rf;    // Регион РФ
    QString smo_name;   // Название СМО
    QString smo_short;  // краткое название СМО
    // данные для Контакт-Центра
    // Наличие контакт-центров СМО :
    int has_federal_cc;
    int has_regional_cc;
    // Численность сотрудников СМО, работающих с обращениями граждан :
    int cnt_w1;     // Численность страховых \nпредставителей 1 уровня
    int cnt_w1_cc;  // из них: сотруюников \nКонтакт-центра
    int cnt_w2;     // Численность страховых \nпредставителей 2 уровня

    int id_filial;
    QString filial_regnum;
    QString filial_name;
    int id_point;
    QString point_regnum;
    QString point_name;
    QString point_signer;   // ФИО сотрудника с правом подписи ВС
    int id_operator;
    QString operator_fio;

    int font_size;

    // филиалы
    QString bases_list;
    QString test_host, test_base, test_user, test_pass;
    int test_port;
    QString work_host, work_base, work_user, work_pass;
    int work_port;

    int terminal_id;
    int terminal_id_point;
    QString terminal_name;
    int terminal_status;

    int count_insured;

} s_data_app;

#endif // S_DATA_APP

