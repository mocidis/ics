#ifndef __ICS_CORE_H__
#define __ICS_CORE_H__

#include "queue.h"
#include "ics-event.h"
#include "ics-command.h"
#include "object-pool.h"

typedef struct _app_data {
	pjsua_config cfg;
	pjsua_logging_config log_cfg;
	pjsua_acc_config acfg;
	pjsua_acc_id acc_id;

	int f_quit;
	
	pj_caching_pool cp;
	queue_t queue;
	opool_t opool;
	pj_pool_t *pool;
	pj_thread_t *thread;
	pj_thread_t *thread2;

}ics_data_t;

void ics_core_init(ics_data_t *data);
void ics_core_connect(ics_data_t *data, int port);
void ics_core_add_account(ics_data_t *data, char *s_ip, char *username, char *password);
void ics_core_receive_command(ics_data_t *data);
void ics_core_make_call(ics_data_t *data, char *sip_addr);
void ics_core_answer_call(ics_data_t *data);
void ics_core_hangup_call(ics_data_t *data, int renew);
void ics_core_hold_call(ics_data_t *data);
void ics_core_release_hold(ics_data_t *data);
void ics_core_transfer_call(ics_data_t *data, int call_id_1, int call_id_2);
void ics_core_set_registration(ics_data_t *data, int renew);
void ics_core_adjust_audio_volume(ics_data_t *data, char *device, float level);
void ics_core_conference_call(ics_data_t *data, int call_id);

void ics_core_clean(ics_data_t *data);

void _ics_core_connect(ics_data_t *data, int port);
void _ics_core_add_account(ics_data_t *data, char *s_ip, char *username, char *password);
void _ics_core_make_call(ics_data_t *data, char *sip_addr);
void _ics_core_answer_call(ics_data_t *data);
void _ics_core_hangup_call(ics_data_t *data, int renew);
void _ics_core_hold_call(ics_data_t *data);
void _ics_core_release_hold(ics_data_t *data);
void _ics_core_transfer_call(ics_data_t *data, int call_id_1, int call_id_2);
void _ics_core_set_registration(ics_data_t *data, int renew);
void _ics_core_adjust_audio_volume(ics_data_t *data, char *device, float level);

void ics_core_set_reg_start_callback(void (*func)(int accid));
void ics_core_set_reg_state_callback(void (*func)(int account_id, char* is_registration, int code, char *reason));
void ics_core_set_incoming_call_callback(void (*func)(int account_id, int call_id, char *remote_contact, char *local_contact));
void ics_core_set_call_state_callback(void (*func)(int call_id, char *st_text));
void ics_core_set_call_transfer_callback(void (*func)(int call_id, int st_code, char *st_text));
void ics_core_set_call_media_state_callback(void (*func)(int call_id, int st_code));

void process_event(ics_event_t *event);
pj_bool_t find_call(void);
//void list_active_call(void);

void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew);
void on_reg_state(pjsua_acc_id acc_id, pjsua_reg_info *info);
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);
void on_call_state (pjsua_call_id call_id, pjsip_event *e);
void on_call_transfer_status (pjsua_call_id call_id, int st_code, const pj_str_t *st_text, pj_bool_t final, pj_bool_t *p_cont);
void on_call_media_state(pjsua_call_id call_id);

#endif
