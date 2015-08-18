#include <pjsua-lib/pjsua.h>
#include "ics-core.h"
#include "ics-common.h"

void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew) {
	pj_status_t status;
	PJ_LOG(2, (__FILE__,"Reg started acc_id = %d",acc_id));
}

void on_reg_state(pjsua_acc_id acc_id, pjsua_reg_info *info) {

	PJ_LOG(2, (__FILE__, "Current Account %d Status: %d(%.*s)",
				acc_id,	info->cbparam->code, info->cbparam->reason.slen, info->cbparam->reason.ptr));
	
	if (info->cbparam->code/100 == 2 ) {
		if (info->cbparam->expiration > 0) {
			//READY
		}
		else;
			//STARTED
	}	
	else;
		//STARTED
}

void ics_core_create(ics_data_t *data) {
	pj_status_t status;
	status = pj_init();
	status = pjsua_create();
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot create pjsua");
}

void ics_core_config_default(ics_data_t *data) {
	pjsua_config_default(&data->cfg);

	data->cfg.cb.on_reg_started = &on_reg_started;
	data->cfg.cb.on_reg_state2 = &on_reg_state;

	pjsua_logging_config_default(&data->log_cfg);
	data->log_cfg.console_level = 2;
}

void ics_core_init(ics_data_t *data) {
	pj_status_t status;
	status = pjsua_init(&data->cfg, &data->log_cfg, NULL);
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot initializing pjsua");

	pj_caching_pool_init(&data->cp, NULL, 1024);
	data->pool = pj_pool_create(&data->cp.factory, "pool", 64, 64, NULL);
}

void ics_core_connect(ics_data_t *data) {
	pj_status_t status;
	pjsua_transport_config cfg;
	
	pjsua_transport_config_default(&cfg);
	cfg.port = 12345;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot create connect");

	status = pjsua_start();
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot start pjsua");
}

void ics_core_register(ics_data_t *data,char *s_ip, char *username, char*password) {
	pj_status_t status;
	pjsua_acc_config_default(&data->acfg);	
	
	char s[50],s1[50];
	sprintf(s, "sip:%s@%s", username, s_ip);
	sprintf(s1, "sip:%s",s_ip);

	data->acfg.id = pj_str(s);
	data->acfg.reg_uri = pj_str(s1);
	data->acfg.cred_count = 1;
	data->acfg.cred_info[0].realm = pj_str("asterisk");
	data->acfg.cred_info[0].scheme = pj_str("digest");
	data->acfg.cred_info[0].username = pj_str(username);
	data->acfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	data->acfg.cred_info[0].data = pj_str(password);

	status = pjsua_acc_add(&data->acfg, PJ_TRUE, &data->acc_id);
	ICS_RETURN_IF_TRUE(status != PJ_SUCCESS, "Cannot register account");
	
}

void ics_core_clean(ics_data_t *data) {
	pjsua_destroy();
	pj_pool_release(data->pool);
	pj_caching_pool_destroy(&data->cp);
}
