#include "queue.h"
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
void ics_core_make_call(ics_data_t *data, char *sip_addr);
void ics_core_answer_call(ics_data_t *data);
void ics_core_hangup_call(ics_data_t *data, int renew);
void ics_core_hold_call(ics_data_t *data);
void ics_core_release_hold(ics_data_t *data);
void ics_core_transfer_call(ics_data_t *data, int call_id_1, int call_id_2);
void ics_core_set_register(ics_data_t *data, int renew);
void ics_core_adjust_audio_volume(ics_data_t *data, char *device, float level);
void ics_core_clean(ics_data_t *data);

void _ics_core_connect(ics_data_t *data, int port);
void _ics_core_add_account(ics_data_t *data, char *s_ip, char *username, char *password);
void _ics_core_make_call(ics_data_t *data, char *sip_addr);
void _ics_core_answer_call(ics_data_t *data);
void _ics_core_hangup_call(ics_data_t *data, int renew);
void _ics_core_hold_call(ics_data_t *data);
void _ics_core_release_hold(ics_data_t *data);
void _ics_core_transfer_call(ics_data_t *data, int call_id_1, int call_id_2);
void _ics_core_set_register(ics_data_t *data, int renew);
void _ics_core_adjust_audio_volume(ics_data_t *data, char *device, float level);


