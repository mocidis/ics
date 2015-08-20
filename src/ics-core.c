#include <pjsua-lib/pjsua.h>
#include <stdio.h>
#include "ics-core.h"
#include "ics-event.h"
#include "ics-common.h"
#include "queue.h"

pjsua_call_id current_acc = PJSUA_INVALID_ID;

void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew) {
	ics_data_t *data;
	data = (ics_data_t *)pjsua_acc_get_user_data(acc_id);

	opool_item_t *p_item = opool_get(&data->opool);

	build_reg_start_event((ics_event_t *)p_item->data, acc_id);
	queue_enqueue(&data->queue, (void *)p_item);
}

void on_reg_state(pjsua_acc_id acc_id, pjsua_reg_info *info) {

	//PJ_LOG(2, (__FILE__, "Current Account %d Status: %d(%.*s)",
	//			acc_id,	info->cbparam->code, info->cbparam->reason.slen, info->cbparam->reason.ptr));
	
	ics_data_t *data;
	
	data = (ics_data_t *)pjsua_acc_get_user_data(acc_id);
	opool_item_t *p_item = opool_get(&data->opool);

	build_reg_state_event((ics_event_t *)p_item->data, acc_id,info->cbparam->expiration, info->cbparam->code, 
				info->cbparam->reason.ptr, info->cbparam->reason.slen);
	queue_enqueue(&data->queue, (void *)p_item);
}

void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
	ics_data_t *data;
	pjsua_call_info ci;

	current_acc = call_id;

	pjsua_call_get_info(call_id, &ci);
	data = (ics_data_t *)pjsua_acc_get_user_data(acc_id);
	opool_item_t *p_item = opool_get(&data->opool);
	build_incoming_call_event((ics_event_t *)p_item->data, acc_id,call_id, ci.remote_info.ptr, ci.local_info.ptr);	

	queue_enqueue(&data->queue, (void *)p_item);
}

void find_call(void) {
	int i, max;
	
	max = pjsua_call_get_max_count();
	for (i = 0; i < max; i++) {
		if (pjsua_call_is_active(i)) {
			current_acc = i;
		}
	}
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
	data->cfg.cb.on_incoming_call = &on_incoming_call;

	pjsua_logging_config_default(&data->log_cfg);
	data->log_cfg.console_level = 2;
}

void ics_core_init(ics_data_t *data) {
	pj_status_t status;
	int queue_capacity = 100;
	status = pjsua_init(&data->cfg, &data->log_cfg, NULL);
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot initializing pjsua");

	pj_caching_pool_init(&data->cp, NULL, 1024);
	data->pool = pj_pool_create(&data->cp.factory, "pool", 64, 64, NULL);
	queue_init(&data->queue, queue_capacity, 10 /* NOT USED */, data->pool);

	opool_init(&data->opool, queue_capacity, sizeof(ics_event_t), data->pool );
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

	current_acc = data->acc_id;

	data->acfg.id = pj_str(s);
	data->acfg.reg_uri = pj_str(s1);
	data->acfg.cred_count = 1;
	data->acfg.cred_info[0].realm = pj_str("asterisk");
	data->acfg.cred_info[0].scheme = pj_str("digest");
	data->acfg.cred_info[0].username = pj_str(username);
	data->acfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	data->acfg.cred_info[0].data = pj_str(password);

	data->acfg.user_data = (void *)data;
	
	pjsua_set_no_snd_dev();
	status = pjsua_acc_add(&data->acfg, PJ_TRUE, &data->acc_id);
	ICS_RETURN_IF_TRUE(status != PJ_SUCCESS, "Cannot register account");
}

void ics_core_make_call(ics_data_t *data) {
	int chose;
	char sip_add[50];
	pj_str_t uri;
	
	printf("Chose a call:\n");
	printf("1.quy2@192.168.235.129\n");
	printf("2.quy3@192.168.235.129\n");
	printf("3.quy10@192.168.235.129\n");
	printf("Current acc = %d\n", data->acc_id);
	if (scanf("%d",&chose) != 1){
		printf("Invalid input value\n");
	}
	switch(chose) {
		case 1:
			strcpy(sip_add, "sip:quy2@192.168.235.129");
			break;
		case 2:
			strcpy(sip_add, "sip:quy3@192.168.235.129");
			break;	
		case 3:
			strcpy(sip_add, "sip:quy10@192.168.235.129");
			break;
		default:
			printf("Chose again\n");
			break;
	}
	uri = pj_str(sip_add);
	pjsua_call_make_call(data->acc_id, &uri, 0, NULL, NULL, NULL);
}

void ics_core_answer_call(ics_data_t *data) {
	pjsua_call_answer(current_acc, 200, NULL, NULL);
}

void ics_core_hangup_call(ics_data_t *data) {
}

void ics_core_hold_call(ics_data_t *data) {
	
}

void ics_core_release_hold(ics_data_t *data) {
}

void ics_core_set_register(ics_data_t *data) {
}

void ics_core_tranfer_call(ics_data_t *data) {
}

void print_menu() {
	puts("+==============================+");
	puts("|       Call Commands:         |");
	puts("|                              |");
	puts("|  m  Make new call            |");
	puts("|  a  Answer call              |");
	puts("|  h  Hangup call  (ha=all)    |");
	puts("|  H  Hold call                |");
	puts("|  r  release hold             |");
	puts("|  t  Tranfer call             |");
	puts("|  u  Un-register              |");
	puts("|  r  Re-register              |");
	puts("+==============================+");
}

static void *thread_proc(void *param) {
	ics_data_t *data = (ics_data_t *) param;	
	while(1) {

		opool_item_t *p_item = opool_get(&data->opool);
		p_item = (opool_item_t *)queue_dequeue(&data->queue);
		ics_event_t *event = (ics_event_t *)p_item->data;
		print_event(event);
		opool_free(&data->opool, p_item);
	}
}

void ics_core_start(ics_data_t *data) {
	int rc = pj_thread_create(data->pool, "ics_core_loop_thread", (pj_thread_proc*)&thread_proc, data, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &data->thread);
	ICS_EXIT_IF_TRUE(rc != PJ_SUCCESS, "Cannot start thread for ics_core_start");
}

void ics_core_end(ics_data_t *data) {
	data->f_quit = 0;	
}

void ics_core_clean(ics_data_t *data) {
	pjsua_destroy();
	pj_pool_release(data->pool);
	pj_caching_pool_destroy(&data->cp);
}
