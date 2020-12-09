#ifndef S_DATA_CONTACT
#define S_DATA_CONTACT

#include <QDateTime>

typedef struct {
    int id;
    QString n_irp;
    int id_filial;
    int id_point;
    int id_operator;
    QString disp_fio;
    int id_person;
    int id_doc;
    int id_polis;

    QString fam, im, ot;
    int sex;
    bool has_year_birth;
    int year_birth;
    bool has_date_birth;
    QDate date_birth;
    QString snils;
    QString enp;
    QString vs_num;

    int doc_type;
    QString doc_ser, doc_sernum;
    bool has_date_doc;
    QDate doc_date;
    QString doc_org;

    QString pol_ser, pol_sernum, pol_enp;
    QString address_reg, address_liv;
    QString phone, email;
    int soc_status, talker_form;

    int req_way;
    QString req_type_code;
    QString req_reason_code;
    QDateTime req_datetime;
    int req_kladr_subj;
    int req_kladr_dstr;
    int req_kladr_city;
    int req_kladr_nasp;
    int req_kladr_strt;
    QString req_location;
    QString req_disp_fio;

    QString req_petition;
    QString req_comment;
    bool has_date_incid;
    QDate req_date_incid;
    QString req_perform;
    QString req_performer;
    int req_status;
    QString req_status_text;
    bool has_date_contrl;
    QDate req_date_contrl;
    bool has_date_done;
    QDate req_date_done;
    QString req_result;
    int req_result_code;

    bool check_efficiency;

} s_data_contact;


#endif // S_DATA_CONTACT

