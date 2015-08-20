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

void ics_core_create(ics_data_t *data);
void ics_core_config_default(ics_data_t *data);
void ics_core_init(ics_data_t *data);
void ics_core_connect(ics_data_t *data);

void ics_core_start(ics_data_t *data);
void ics_core_end(ics_data_t *data);

void ics_core_clean(ics_data_t *data);

void ics_core_register(ics_data_t *data,char *s_ip, char *username, char*password); // for testing
