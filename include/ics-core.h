#include <queue.h>

typedef struct _app_data {
	pjsua_config cfg;
	pjsua_logging_config log_cfg;
	pjsua_acc_config acfg;
	
	queue_t queue;
	pjsua_acc_id acc_id;

	pj_caching_pool cp;
	pj_pool_t *pool;
	pjsua_acc_config *aconfig;

}ics_data_t;

void ics_core_create(ics_data_t *data);
void ics_core_config_default(ics_data_t *data);
void ics_core_init(ics_data_t *data);
void ics_core_connect(ics_data_t *data);
void ics_core_register(ics_data_t *data,char *s_ip, char *username, char*password);
void ics_core_clean(ics_data_t *data);

