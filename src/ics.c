/**
 * @file ics.c
 * @brief core of ics, allow to call, answer, transfer, adjust volume,etc.
 */

/*! INLUCDE FILES */
#include <stdio.h>
#include "ics.h"
#include "ics-common.h"

/*! GLOBAL VARIABLES */
pjsua_call_id current_call = PJSUA_INVALID_ID;


/*! FUNCTION PROTOTYPES 
 ************************************************** **********/
void (*on_reg_start_p)(int account_id);
void (*on_reg_state_p)(int account_id, char *is_registration,int code, char *reason);
void (*on_incoming_call_p)(int account_id, int call_id, int st_code, char *remote_contact, char *local_contact);
void (*on_call_state_p)(int call_id,int st_code, char *st_text);
void (*on_call_transfer_p)(int call_id, int st_code, char *st_text);
void (*on_call_media_state_p)(int call_id, int st_code);

void on_reg_start_default(void) {};

void ics_set_default_callback(void (*func)()) {
    on_reg_start_p = func;
    on_reg_state_p = func;
    on_incoming_call_p = func;
    on_call_state_p = func;
    on_call_transfer_p = func;
    on_call_media_state_p = func;
}
void ics_set_reg_start_callback(void (*func)(int accid)) {
    on_reg_start_p = func;
}

void ics_set_reg_state_callback(void (*func)(int account_id, char* is_registration, int code, char *reason)) {
    on_reg_state_p = func;
}

void ics_set_incoming_call_callback(void (*func)(int account_id, int call_id, int st_code, char *remote_contact, char *local_contact)) {
    on_incoming_call_p = func;
}

void ics_set_call_state_callback(void (*func)(int call_id,int st_code, char *st_text)) {
    on_call_state_p = func;
}

void ics_set_call_transfer_callback(void (*func)(int call_id, int st_code, char *st_text)) {
    on_call_transfer_p = func;
}

void ics_set_call_media_state_callback(void (*func)(int call_id, int st_code)) {
    on_call_media_state_p = func;
}

/**
 * \fn process_event()
 * \brief Xu ly cac event, trong vi du la in ra cac thong tin cua event
 * \param agr1: ics_event_t *event
 */
void process_event(ics_event_t *event) {
    ICS_EXIT_IF_TRUE(event->event.eventid >= ICS_EVENT_END, "Unknown event id\n");

    SHOW_LOG(3, "Event type: %s\n", ICS_EVENT_NAME[event->event.eventid]);

    switch(event->event.eventid){
        case ICS_REG_START:
            on_reg_start_p(event->reg_start_event.account_id);
            break;
        case ICS_REG_STATE:
            on_reg_state_p(event->reg_state_event.account_id, 
                    (event->reg_state_event.is_registration < 1 || event->reg_state_event.code<0 || event->reg_state_event.code >= 300 ? "No" : "Yes"), 
                    event->reg_state_event.code, 
                    event->reg_state_event.reason);
            break;
        case ICS_INCOMING_CALL:
            on_incoming_call_p(event->incoming_call_event.account_id, event->incoming_call_event.call_id,
                    event->incoming_call_event.st_code,
                    event->incoming_call_event.remote_contact,
                    event->incoming_call_event.local_contact);
            break;
        case ICS_CALL_STATE:
            on_call_state_p(event->call_state_event.call_id, event->call_state_event.state_code, event->call_state_event.state);
            break;
        case ICS_TRANSFER:
            on_call_transfer_p(event->transfer_event.call_id,
                    event->transfer_event.st_code,
                    event->transfer_event.st_text);
            break;
        case ICS_CALL_MEDIA_STATE:
            on_call_media_state_p(event->call_media_state_event.call_id, event->call_media_state_event.st_code);
            break;
        default:
            SHOW_LOG(3, "Invalid event id %d\n", event->event.eventid);
            break;
    }
}


/**
 * \fn find_call()
 * \brief Xac dinh call id cua cuoc goi gan nhat
 * \return PJ_SUCCESS
 */
static pj_bool_t find_call(void) {
    int i, max;

    max = pjsua_call_get_max_count();
    for (i = max-1; i >= 0; --i) {
        if (pjsua_call_is_active(i)) {
            current_call = i;
            return PJ_TRUE;
        }
    }
    current_call = PJSUA_INVALID_ID;
    return PJ_FALSE;
}

//Maybe useful someday
/**
 * \fn ics_list_call()
 * \brief Liet ke danh sach cac cuoc goi hien co
 */

void _ics_list_call(ics_t *data) {
    int i, max;
    pjsua_call_info ci;

    PJ_UNUSED_ARG(data);
    max = pjsua_call_get_count();
    SHOW_LOG(3, "Your call list:\n");
    for (i = 0; i < max; i++){	
        if (pjsua_call_is_active(i)) {
            pjsua_call_get_info(i, &ci);
            SHOW_LOG(3, "Call id : %d to %.*s [%.*s]\n", ci.id,
                    (int)ci.remote_info.slen, ci.remote_info.ptr,
                    (int)ci.state_text.slen, ci.state_text.ptr);
        }
    }
    SHOW_LOG(3, "Your current call id : %d\n", current_call);
}


/**
 * \fn _ics_connect()
 * \brief Tao kenh ket noi (TCP,UDP,TLS,...)
 * \param agr1: ics_t *data
 * agr2: int port
 */
static void _ics_connect(ics_t *data, int port) {
    PJ_UNUSED_ARG(data);
    PJ_UNUSED_ARG(port);

    pj_status_t status;
    pjsua_transport_config cfg;

    pjsua_transport_config_default(&cfg);
    cfg.port = 0;
    status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
    ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot create connect");

    status = pjsua_start();
    ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot start pjsua");
}

/**
 * \fn _ics_add_account()
 * \brief Them mot tai khoan, neu tai khoan da duoc config se tien hanh register
 * \param agr1: ics_t *data
 * agr2: char * server ip
 * agr3: char * username
 * agr4: char * password
 */
static void _ics_add_account(ics_t *data,char *s_ip, char *username, char*password) {
    pj_status_t status;
    pjsua_acc_config_default(&data->acfg);	
    pj_str_t server_ip;
    pj_str_t uri;
    pj_str_t passwd;

    char s[50],s1[50];
    sprintf(s, "sip:%s@%s", username, s_ip);
    sprintf(s1, "sip:%s",s_ip);

    pj_strdup2(data->pool, &server_ip, s);
    pj_strdup2(data->pool, &uri, s1);
    pj_strdup2(data->pool, &passwd, password);

    data->acfg.id = server_ip;
    data->acfg.reg_uri = uri;
    data->acfg.cred_count = 1;
    data->acfg.cred_info[0].realm = pj_str("asterisk");
    data->acfg.cred_info[0].scheme = pj_str("digest");
    data->acfg.cred_info[0].username = pj_str(username);
    data->acfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    data->acfg.cred_info[0].data = passwd;

    data->acfg.user_data = (void *)data;

    data->acfg.reg_timeout = 5;

    if(data->f_account_added) {
        status = pjsua_acc_modify(data->acc_id, &data->acfg);
        ICS_RETURN_IF_TRUE(status != PJ_SUCCESS, "Cannot update account");
        pjsua_acc_set_registration(data->acc_id, 1);
    }
    else {
        status = pjsua_acc_add(&data->acfg, PJ_TRUE, &data->acc_id);
        ICS_RETURN_IF_TRUE(status != PJ_SUCCESS, "Cannot register account");
        data->f_account_added = 1;
    }
}

/**
 * \fn _ics_make_call()
 * \brief Tao cuoc goi
 * \param agr1: ics_t *data
 * agr2: char *sip address
 */
static void _ics_make_call(ics_t *data, char * sip_addr) {
    pj_str_t uri;

    uri = pj_str(sip_addr);
    pjsua_call_make_call(data->acc_id, &uri, 0, NULL, NULL, NULL);
}

/**
 * \fn _ics_answer_call()
 * \brief Tra loi cuoc goi
 * \param agr1: ics_t *data
 */
static void _ics_answer_call(ics_t *data) {
    PJ_UNUSED_ARG(data);

    if (current_call == PJSUA_INVALID_ID)
        SHOW_LOG(3, "No current call\n");
    else {
        pjsua_call_answer(current_call, 200, NULL, NULL);
    }
}

/**
 * \fn _ics_hangup_call()
 * \brief Ket thuc cuoc goi
 * \param agr1: ics_t *data
 * agr2: int renew (1= hangup all, 0= hangup current call)
 */
static void _ics_hangup_call(ics_t *data, int renew) {
    PJ_UNUSED_ARG(data);

    if (current_call == PJSUA_INVALID_ID)
        SHOW_LOG(3, "No current call\n");
    else {
        if (renew == -2) 
            pjsua_call_hangup_all();
        else {
            renew = current_call;
            pjsua_call_hangup(renew, 0, NULL, NULL);	
        }
    }
}

/**
 * \fn _ics_hold_call()
 * \brief Giu cuoc goi
 * \param agr1: ics_t *data
 */
static void _ics_hold_call(ics_t *data) {
    PJ_UNUSED_ARG(data);

    if (current_call < 0)
        SHOW_LOG(3, "No current call\n");
    else {
        pjsua_call_set_hold(current_call, NULL);
    }
}

/**
 * \fn _ics_release_hold()
 * \brief Tha? cuoc goi
 * \param agr1: ics_t *data
 * agr2: int port
 */
static void _ics_release_hold(ics_t *data) {
    PJ_UNUSED_ARG(data);

    if (current_call < 0)
        SHOW_LOG(3, "No current call\n");
    else {
        pjsua_call_reinvite(current_call, PJ_TRUE, NULL);
    }
}

/**
 * \fn _ics_set_registration()
 * \brief Tuy chinh che do registration
 * \param agr1: ics_t *data
 * agr2: int renew(1= re-register, 0= un-register)
 */
static void _ics_set_registration(ics_t *data, int renew) {
    PJ_UNUSED_ARG(data);

    if (renew == 1 || renew == 0)
        pjsua_acc_set_registration(data->acc_id, renew);
    else
        SHOW_LOG(3, "Invalid input");
}

/**
 * \fn _ics_transfer_call()
 * \brief Chuyen huong cuoc goi
 * \param agr1: ics_t *data
 * agr2: int call_id_1
 * agr3: int call_id_2
 */
static void _ics_transfer_call(ics_t *data, int call_id_1, int call_id_2) {
    PJ_UNUSED_ARG(data);
    PJ_UNUSED_ARG(call_id_1);
    PJ_UNUSED_ARG(call_id_2);

#if 0
    if ( (call_id_1 != call_id_2) && pjsua_call_is_active(call_id_1) && pjsua_call_is_active(call_id_2) ) {
        pjsua_call_xfer_replaces(call_id_1, call_id_2, 0, NULL);
    }
    else
        SHOW_LOG(3, "Cannot transfer call!\n");
#endif

    //For test only:
#if 1
    int i, max;
    pjsua_call_info ci;

    max = pjsua_call_get_count();
    SHOW_LOG(3, "You have %d active call%s\n", max, (max>1?"s":""));

    for (i = 0; i < max; i++){	
        if (pjsua_call_is_active(i)) {
            pjsua_call_get_info(i, &ci);
            pjsua_call_xfer_replaces(current_call, ci.id, 0, NULL);
            break;
        }
    }
#endif
}

static void _ics_conference_call(ics_t *data, int call_id) {
    int i, max;
    PJ_UNUSED_ARG(data);

    max = pjsua_call_get_count();
    SHOW_LOG(3, "Let's conference call!\n");
    pjsua_call_info ci;

#if 1
    if ( (call_id != current_call) && pjsua_call_is_active(call_id)  ) {
        pjsua_call_reinvite(call_id, PJ_TRUE, NULL);
        for (i = 0; i < max; i++) {
            if (pjsua_call_has_media(i) != 0) {
                pjsua_conf_connect(pjsua_call_get_conf_port(call_id), pjsua_call_get_conf_port(i));		
                pjsua_conf_connect(pjsua_call_get_conf_port(i), pjsua_call_get_conf_port(call_id));
            }
        }
    }
    else
        SHOW_LOG(3, "Cannot transfer call!\n");
#endif

    //For test only:
#if 1


    for (i = 0; i < max; i++){	
        if (pjsua_call_is_active(i) && (i != current_call)) {
            pjsua_call_reinvite(i, PJ_TRUE, NULL);
            pjsua_call_get_info(i, &ci);
            pjsua_conf_connect(pjsua_call_get_conf_port(ci.id), pjsua_call_get_conf_port(current_call));		
            pjsua_conf_connect(pjsua_call_get_conf_port(current_call), pjsua_call_get_conf_port(ci.id));
        }
        break;
    }
#endif

}

/**
 * \fn _ics_adjust_audio_volume()
 * \brief Tuy chinh am luong cho mic va speaker
 * \param agr1: ics_t *data
 * agr2: char *device
 * agr3: level
 */
static void _ics_adjust_audio_volume(ics_t *data, int device, float level) {
    PJ_UNUSED_ARG(data);

    if (device == 0) {
        pjsua_conf_adjust_rx_level(0, level);
    }
    else if (device == 1) {
        pjsua_conf_adjust_tx_level(0, level);
    }
    else
        SHOW_LOG(3, "Invalid input to adjust device");
}

void ics_clean(ics_t *data) {
    data->f_quit = 1;
    pjsua_destroy();
    pj_pool_release(data->pool);
    pj_caching_pool_destroy(&data->cp);
}


/**
 * \fn on_reg_started(), on_reg_state(), on_incoming_call(), on_call_state(), on_call_transfer_status(), on_call_media_state()
 * \brief cac ham callback
 */
static void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew) {
    ics_t *data;
    PJ_UNUSED_ARG(renew);

    data = (ics_t *)pjsua_acc_get_user_data(acc_id);
    opool_item_t *p_item = opool_get(&data->opool);
    ics_event_t *event = (ics_event_t *)p_item->data;

    build_reg_start_event((ics_event_t *)p_item->data, acc_id);

    process_event(event);

    opool_free(&data->opool, p_item);
}

static void on_reg_state(pjsua_acc_id acc_id, pjsua_reg_info *info) {
    ics_t *data;

    data = (ics_t *)pjsua_acc_get_user_data(acc_id);
    opool_item_t *p_item = opool_get(&data->opool);
    ics_event_t *event = (ics_event_t *)p_item->data;

    build_reg_state_event((ics_event_t *)p_item->data, acc_id,info->cbparam->expiration, info->cbparam->code, 
            info->cbparam->reason.ptr, info->cbparam->reason.slen);
    process_event(event);

    opool_free(&data->opool, p_item);
}

static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
    PJ_UNUSED_ARG(rdata);

    ics_t *data;
    pjsua_call_info ci;


    pjsua_call_get_info(call_id, &ci);
    data = (ics_t *)pjsua_acc_get_user_data(acc_id);

    //_ics_hold_call(data);

    opool_item_t *p_item = opool_get(&data->opool);
    build_incoming_call_event((ics_event_t *)p_item->data, acc_id,call_id, ci.state, ci.remote_info.ptr, ci.local_info.ptr);	
    ics_event_t *event = (ics_event_t *)p_item->data;

    //queue_enqueue(&data->queue, (void *)p_item);
    process_event(event);

    current_call = call_id;

    opool_free(&data->opool, p_item);
}

static void on_call_state (pjsua_call_id call_id, pjsip_event *e) {
    ics_t *data;
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);

    //Handle hold call problem
#if 0
    if ( (strcmp(ci.state_text.ptr,"CALLING") == 0) && (current_call != PJSUA_INVALID_ID) )
        _ics_hold_call(data);
#endif
    current_call = call_id;

    if (strcmp(ci.state_text.ptr,"DISCONNCTD") == 0){
        find_call();
    }

    data = (ics_t *)pjsua_acc_get_user_data(ci.acc_id);
    opool_item_t *p_item = opool_get(&data->opool);
    build_call_state_event((ics_event_t *)p_item->data, call_id,ci.state, ci.state_text.ptr);	
    ics_event_t *event = (ics_event_t *)p_item->data;

    process_event(event);

    opool_free(&data->opool, p_item);
}

static void on_call_media_state(pjsua_call_id call_id) {
    ics_t *data;
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    //! Khoi tao mot connect media flow tu app nay sang app khac
    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        pjsua_conf_connect(ci.conf_slot, 0); // ntt1->quy
        pjsua_conf_connect(0, ci.conf_slot); // quy->ntt1
    }

    pjsua_call_get_info(call_id, &ci);
    data = (ics_t *)pjsua_acc_get_user_data(ci.acc_id);
    opool_item_t *p_item = opool_get(&data->opool);
    build_call_media_state_event((ics_event_t *)p_item->data, call_id, ci.media_status);	
    ics_event_t *event = (ics_event_t *)p_item->data;

    //	queue_enqueue(&data->queue, (void *)p_item);
    process_event(event);

    opool_free(&data->opool, p_item);

}

static void on_call_transfer_status (pjsua_call_id call_id, int st_code, const pj_str_t *st_text, pj_bool_t final, pj_bool_t *p_cont) {
    PJ_UNUSED_ARG(final);
    PJ_UNUSED_ARG(p_cont);

    ics_t *data;
    pjsua_call_info ci;
    current_call = call_id;
    int len;

    pjsua_call_get_info(call_id, &ci);
    data = (ics_t *)pjsua_acc_get_user_data(ci.acc_id);	

    len = (int)st_text->slen;
    if (st_text->ptr[len] == '\n' || st_text->ptr[len] == '\r')
        st_text->ptr[len] = '\0';

    opool_item_t *p_item = opool_get(&data->opool);
    build_transfer_event((ics_event_t *)p_item->data,call_id, st_code, st_text->ptr);	
    ics_event_t *event = (ics_event_t *)p_item->data;

    //	queue_enqueue(&data->queue, (void *)p_item);
    process_event(event);

    opool_free(&data->opool, p_item);
}

/**
 * \fn ics_init()
 * \brief Khoi tao cac thanh phan can thiet cho chuong trinh
 * \param agr1: ics_t *data
 */

void ics_pool_init(ics_t *data) {
    //! CREATE
    pj_status_t status;

    //! Khoi tao pj lib
    status = pj_init();
    EXIT_IF_TRUE(status != PJ_SUCCESS, "Error pj_init");
    //! Khoi tao pj caching pool va tao poll
    pj_caching_pool_init(&data->cp, NULL, 1024);
    data->pool = pj_pool_create(&data->cp.factory, "pool", 6400, 6400, NULL);
}
void ics_init(ics_t *data) {

    //! Khoi tao queue va opool
    int queue_capacity = 100;

    queue_init(&data->queue, queue_capacity, 10 /* NOT USED */, data->pool);

    opool_init(&data->opool, queue_capacity, sizeof(ics_event_t), data->pool );
}

void ics_pjsua_init(ics_t *data) {
    int dev_count;

    pj_status_t status;
    //! Goi pjsua
    status = pjsua_create();
    ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot create pjsua");

    //!CONFIG DEFAULT
    pjsua_config_default(&data->cfg);

    data->cfg.cb.on_reg_started = &on_reg_started;
    data->cfg.cb.on_reg_state2 = &on_reg_state;
    data->cfg.cb.on_incoming_call = &on_incoming_call;
    data->cfg.cb.on_call_state = &on_call_state;
    data->cfg.cb.on_call_media_state = &on_call_media_state;
    data->cfg.cb.on_call_transfer_status = &on_call_transfer_status;

    pjsua_logging_config_default(&data->log_cfg);
    data->log_cfg.console_level = 2;

    //! INIT
    //! Khoi tao pjsua
    status = pjsua_init(&data->cfg, &data->log_cfg, NULL);
    ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot initializing pjsua");

    //! Chon sound device
    dev_count = pjmedia_aud_dev_count();
    pjsua_set_snd_dev(0, dev_count-1);
    //pjsua_set_null_snd_dev();
}
//Put command into queue
/**
 * \fn ics_...()
 * \brief Tao mot cmd object
 * \param agr1: ics_t *data
 * agr2:...
 */
void ics_connect(ics_t *data, int port){
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_connect_cmd((ics_cmd_t *)p_item->data, port);
    queue_enqueue(&data->queue, (void *)p_item);
}
void ics_add_account(ics_t *data, char *s_ip, char *username, char *password) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_add_acc_cmd((ics_cmd_t *)p_item->data, s_ip, username, password);
    queue_enqueue(&data->queue, (void *)p_item);
}
void ics_make_call(ics_t *data, char *sip_addr) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_make_call_cmd((ics_cmd_t *)p_item->data, sip_addr);
    queue_enqueue(&data->queue, (void *)p_item);
}
void ics_answer_call(ics_t *data) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_answer_call_cmd((ics_cmd_t *)p_item->data);
    queue_enqueue(&data->queue, (void *)p_item);
}
void ics_hangup_call(ics_t *data, int renew) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_hangup_call_cmd((ics_cmd_t *)p_item->data, renew);
    queue_enqueue(&data->queue, (void *)p_item);
}
void ics_hold_call(ics_t *data) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_hold_call_cmd((ics_cmd_t *)p_item->data);
    queue_enqueue(&data->queue, (void *)p_item);
}

void ics_release_hold(ics_t *data) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_release_call_cmd((ics_cmd_t *)p_item->data);
    queue_enqueue(&data->queue, (void *)p_item);
}

void ics_transfer_call(ics_t *data, int call_id_1, int call_id_2) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_transfer_call_cmd((ics_cmd_t *)p_item->data, call_id_1, call_id_2);
    queue_enqueue(&data->queue, (void *)p_item);
}

void ics_set_registration(ics_t *data, int renew) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_set_registration_cmd((ics_cmd_t *)p_item->data, renew);
    queue_enqueue(&data->queue, (void *)p_item);
}

void ics_adjust_audio_volume(ics_t *data, int device, float level) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_adjust_audio_cmd((ics_cmd_t *)p_item->data, device, level);
    queue_enqueue(&data->queue, (void *)p_item);
}

void ics_conference_call(ics_t *data, int call_id) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_conference_call_cmd((ics_cmd_t *)p_item->data, call_id);
    queue_enqueue(&data->queue, (void *)p_item);
}

void ics_list_call(ics_t *data) {
    ICS_EXIT_IF_TRUE(data->f_start != 1, "Your must call ics_start function first\n");
    opool_item_t *p_item = opool_get(&data->opool);

    build_list_call_cmd((ics_cmd_t *)p_item->data);
    queue_enqueue(&data->queue, (void *)p_item);
}
//Dequeue and do command

/**
 * \fn thread_proc()
 * \brief Ham goi boi thread phu., thu hien dequeue lay ra cac cmd object va thuc hien cmd
 * \param agr1: void * param
 */
static void *thread_proc(void *param) {
    ics_t *data = (ics_t *) param;	
    while(!data->f_quit) {
        opool_item_t *p_item = opool_get(&data->opool);
        p_item = (opool_item_t *)queue_dequeue(&data->queue);

        ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;

        switch(cmd->cmd.cmd_id) {
            case CMD_CONNECT:
                _ics_connect(data, cmd->connect_cmd.port);
                break;
            case CMD_ADD_ACC:
                _ics_add_account(data, cmd->add_acc_cmd.s_ip, cmd->add_acc_cmd.username, cmd->add_acc_cmd.password);
                break;
            case CMD_MAKE_CALL:
                _ics_make_call(data, cmd->make_call_cmd.sip_addr);
                break;
            case CMD_ANSWER_CALL: 
                _ics_answer_call(data);
                break;
            case CMD_HANGUP_CALL: 
                _ics_hangup_call(data, cmd->hangup_call_cmd.renew);
                break;
            case CMD_HOLD_CALL:
                _ics_hold_call(data);
                break;
            case CMD_RELEASE_HOLD:
                _ics_release_hold(data);
                break;
            case CMD_TRANSFER_CALL:
                _ics_transfer_call(data, cmd->transfer_call_cmd.call_id_1, cmd->transfer_call_cmd.call_id_2);
                break;
            case CMD_SET_REGISTER:
                _ics_set_registration(data, cmd->set_registration_cmd.renew);
                break;
            case CMD_ADJUST_AUDIO:
                _ics_adjust_audio_volume(data, cmd->adjust_audio_cmd.device, cmd->adjust_audio_cmd.level);
                break;
            case CMD_CONFERENCE_CALL:
                _ics_conference_call(data, cmd->conference_call_cmd.call_id);
                break;
            case CMD_LIST_CALL:
                _ics_list_call(data);
                break;
            default:
                SHOW_LOG(3, "Invalid command id %d\n", cmd->cmd.cmd_id);
                break;
        }
        // Event processing done
        opool_free(&data->opool, p_item);
    }
    return NULL;
}

/**
 * \fn ics_start()
 * \brief Tao mot thead moi co chuc nang nhan thong tin command
 * \param agr1: ics_t *data
 */
void ics_start(ics_t *data) {
    data->f_quit = 0;
    data->f_start = 1;
    data->f_account_added = 0;
    int rc = pj_thread_create(data->pool, "ics_loop_thread", (pj_thread_proc*)&thread_proc, data, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &data->thread);
    ICS_EXIT_IF_TRUE(rc != PJ_SUCCESS, "Cannot start thread for ics_start");
}

