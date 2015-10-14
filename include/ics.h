#ifndef __ICS_CORE_H__
#define __ICS_CORE_H__

#include "queue.h"
#include "ics-event.h"
#include "ics-command.h"
#include "object-pool.h"

#include <pjsua-lib/pjsua.h>

typedef struct _app_data {
	pjsua_config cfg;
	pjsua_logging_config log_cfg;
	pjsua_acc_config acfg;
	pjsua_acc_id acc_id;

	int f_quit;
	int f_start;
	
	pj_caching_pool cp;
	queue_t queue;
	opool_t opool;
	pj_pool_t *pool;
	pj_thread_t *thread;

}ics_t;

void ics_pool_init(ics_t *data);
void ics_pjsua_init(ics_t *data);
void ics_init(ics_t *data);

void ics_connect(ics_t *data, int port);
void ics_add_account(ics_t *data, char *s_ip, char *username, char *password);
void ics_start(ics_t *data);
void ics_make_call(ics_t *data, char *sip_addr);
void ics_answer_call(ics_t *data);
void ics_hangup_call(ics_t *data, int renew);
void ics_hold_call(ics_t *data);
void ics_release_hold(ics_t *data);
void ics_transfer_call(ics_t *data, int call_id_1, int call_id_2);
void ics_set_registration(ics_t *data, int renew);
void ics_adjust_audio_volume(ics_t *data, char *device, float level);
void ics_conference_call(ics_t *data, int call_id);
void ics_list_call(ics_t *data);
void ics_clean(ics_t *data);

void ics_set_default_callback(void (*func)());
void on_reg_start_default(void);
void ics_set_reg_start_callback(void (*func)(int accid));
void ics_set_reg_state_callback(void (*func)(int account_id, char* is_registration, int code, char *reason));
void ics_set_incoming_call_callback(void (*func)(int account_id, int call_id, char *remote_contact, char *local_contact));
void ics_set_call_state_callback(void (*func)(int call_id,int st_code, char *st_text));
void ics_set_call_transfer_callback(void (*func)(int call_id, int st_code, char *st_text));
void ics_set_call_media_state_callback(void (*func)(int call_id, int st_code));

void list_active_call(void);
#endif
