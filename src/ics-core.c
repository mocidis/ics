#include <pjsua-lib/pjsua.h>
#include <stdio.h>
#include "ics-core.h"
#include "ics-event.h"
#include "ics-command.h"
#include "ics-common.h"
#include "queue.h"

pjsua_call_id current_call = PJSUA_INVALID_ID;

void (*on_reg_start_p)(int account_id);
void (*on_reg_state_p)(int account_id, char *is_registration,int code, char *reason);
void (*on_incoming_call_p)(int account_id, int call_id, char *remote_contact, char *local_contact);
void (*on_call_state_p)(int call_id, char *st_text);
void (*on_call_transfer_p)(int call_id, int st_code, char *st_text);
void (*on_call_media_state_p)(int call_id, int st_code);


void ics_core_set_reg_start_callback(void (*func)(int accid)) {
	on_reg_start_p = func;
}

void ics_core_set_reg_state_callback(void (*func)(int account_id, char* is_registration, int code, char *reason)) {
	on_reg_state_p = func;
}

void ics_core_set_incoming_call_callback(void (*func)(int account_id, int call_id, char *remote_contact, char *local_contact)) {
	on_incoming_call_p = func;
}

void ics_core_set_call_state_callback(void (*func)(int call_id, char *st_text)) {
	on_call_state_p = func;
}

void ics_core_set_call_transfer_callback(void (*func)(int call_id, int st_code, char *st_text)) {
	on_call_transfer_p = func;
}

void ics_core_set_call_media_state_callback(void (*func)(int call_id, int st_code)) {
	on_call_media_state_p = func;
}

void process_event(ics_event_t *event) {
	ICS_EXIT_IF_TRUE(event->event.eventid >= ICS_EVENT_END, "Unknown event id\n");

	printf("Event type: %s\n", ICS_EVENT_NAME[event->event.eventid]);

	switch(event->event.eventid){
		case ICS_REG_START:
			on_reg_start_p(event->reg_start_event.account_id);
			break;
		case ICS_REG_STATE:
			on_reg_state_p(event->reg_state_event.account_id, 
					(event->reg_state_event.is_registration>0 ? "Yes" : "No"), 
					event->reg_state_event.code, 
					event->reg_state_event.reason);
			break;
		case ICS_INCOMING_CALL:
			on_incoming_call_p(event->incoming_call_event.account_id, event->incoming_call_event.call_id,
					event->incoming_call_event.remote_contact,
					event->incoming_call_event.local_contact);
			break;
		case ICS_CALL_STATE:
			on_call_state_p(event->call_state_event.call_id, event->call_state_event.state);
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
			printf("Invalid event id %d\n", event->event.eventid);
			break;
	}
}

pj_bool_t find_call(void) {
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

#if 0
void list_active_call(void) {
	int i, max;
	pjsua_call_info ci;

	max = pjsua_call_get_count();

	for (i = 0; i < max; i++){	
		if (pjsua_call_is_active(i)) {
			pjsua_call_get_info(i, &ci);
			printf("Call id : %d to %.*s [%.*s]\n", ci.id,
					(int)ci.remote_info.slen, ci.remote_info.ptr,
					(int)ci.state_text.slen, ci.state_text.ptr);
		}
	}
	printf("Your current call id : %d\n", current_call);
}
#endif

void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew) {
	ics_data_t *data;
	data = (ics_data_t *)pjsua_acc_get_user_data(acc_id);

	opool_item_t *p_item = opool_get(&data->opool);

	ics_event_t *event = (ics_event_t *)p_item->data;

	build_reg_start_event((ics_event_t *)p_item->data, acc_id);
	//queue_enqueue(&data->queue, (void *)p_item);
	process_event(event);

}

void on_reg_state(pjsua_acc_id acc_id, pjsua_reg_info *info) {

	//PJ_LOG(2, (__FILE__, "Current Account %d Status: %d(%.*s)",
	//			acc_id,	info->cbparam->code, info->cbparam->reason.slen, info->cbparam->reason.ptr));

	ics_data_t *data;

	data = (ics_data_t *)pjsua_acc_get_user_data(acc_id);
	opool_item_t *p_item = opool_get(&data->opool);
	ics_event_t *event = (ics_event_t *)p_item->data;

	build_reg_state_event((ics_event_t *)p_item->data, acc_id,info->cbparam->expiration, info->cbparam->code, 
			info->cbparam->reason.ptr, info->cbparam->reason.slen);
	//queue_enqueue(&data->queue, (void *)p_item);
	process_event(event);
}

void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
	ics_data_t *data;
	pjsua_call_info ci;
	

	pjsua_call_get_info(call_id, &ci);
	data = (ics_data_t *)pjsua_acc_get_user_data(acc_id);

	_ics_core_hold_call(data);

	opool_item_t *p_item = opool_get(&data->opool);
	build_incoming_call_event((ics_event_t *)p_item->data, acc_id,call_id, ci.remote_info.ptr, ci.local_info.ptr);	
	ics_event_t *event = (ics_event_t *)p_item->data;

	//queue_enqueue(&data->queue, (void *)p_item);
	process_event(event);

	current_call = call_id;
}

void on_call_state (pjsua_call_id call_id, pjsip_event *e) {
	ics_data_t *data;
	pjsua_call_info ci;

	PJ_UNUSED_ARG(e);

	pjsua_call_get_info(call_id, &ci);

//Handle hold call problem
#if 0
	if (current_call != PJSUA_INVALID_ID)
		_ics_core_hold_call(data);
#endif
	
	current_call = call_id;

	if (strcmp(ci.state_text.ptr,"DISCONNCTD") == 0){
		find_call();
	}

	data = (ics_data_t *)pjsua_acc_get_user_data(ci.acc_id);
	opool_item_t *p_item = opool_get(&data->opool);
	build_call_state_event((ics_event_t *)p_item->data, call_id, ci.state_text.ptr);	
	ics_event_t *event = (ics_event_t *)p_item->data;

	//	queue_enqueue(&data->queue, (void *)p_item);
	process_event(event);

}

void on_call_media_state(pjsua_call_id call_id) {
	ics_data_t *data;
	pjsua_call_info ci;

	pjsua_call_get_info(call_id, &ci);

	if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
		pjsua_conf_connect(ci.conf_slot, 0);
		pjsua_conf_connect(0, ci.conf_slot);
	}

	pjsua_call_get_info(call_id, &ci);
	data = (ics_data_t *)pjsua_acc_get_user_data(ci.acc_id);
	opool_item_t *p_item = opool_get(&data->opool);
	build_call_media_state_event((ics_event_t *)p_item->data, call_id, ci.media_status);	
	ics_event_t *event = (ics_event_t *)p_item->data;

	//	queue_enqueue(&data->queue, (void *)p_item);
	process_event(event);

}

void on_call_transfer_status (pjsua_call_id call_id, int st_code, const pj_str_t *st_text, pj_bool_t final, pj_bool_t *p_cont) {
	ics_data_t *data;
	pjsua_call_info ci;
	current_call = call_id;
	int len;

	pjsua_call_get_info(call_id, &ci);
	data = (ics_data_t *)pjsua_acc_get_user_data(ci.acc_id);

	len = (int)st_text->slen;
	if (st_text->ptr[len] == '\n' || st_text->ptr[len] == '\r')
		st_text->ptr[len] = '\0';

	opool_item_t *p_item = opool_get(&data->opool);
	build_transfer_event((ics_event_t *)p_item->data,call_id, st_code, st_text->ptr);	
	ics_event_t *event = (ics_event_t *)p_item->data;

	//	queue_enqueue(&data->queue, (void *)p_item);
	process_event(event);

}

void ics_core_init(ics_data_t *data) {
	//CREATE
	pj_status_t status;

	status = pj_init();
	status = pjsua_create();
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot create pjsua");

	//CONFIG DEFAULT
	pjsua_config_default(&data->cfg);

	data->cfg.cb.on_reg_started = &on_reg_started;
	data->cfg.cb.on_reg_state2 = &on_reg_state;
	data->cfg.cb.on_incoming_call = &on_incoming_call;
	data->cfg.cb.on_call_state = &on_call_state;
	data->cfg.cb.on_call_media_state = &on_call_media_state;
	data->cfg.cb.on_call_transfer_status = &on_call_transfer_status;

	pjsua_logging_config_default(&data->log_cfg);
	data->log_cfg.console_level = 2;

	//INIT
	int queue_capacity = 100;
	status = pjsua_init(&data->cfg, &data->log_cfg, NULL);
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot initializing pjsua");

	pj_caching_pool_init(&data->cp, NULL, 1024);
	data->pool = pj_pool_create(&data->cp.factory, "pool", 64, 64, NULL);
	queue_init(&data->queue, queue_capacity, 10 /* NOT USED */, data->pool);

	opool_init(&data->opool, queue_capacity, sizeof(ics_event_t), data->pool );

	pjsua_set_snd_dev(0,2);
}

void _ics_core_connect(ics_data_t *data, int port) {
	pj_status_t status;
	pjsua_transport_config cfg;

	pjsua_transport_config_default(&cfg);
	cfg.port = port;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot create connect");

	status = pjsua_start();
	ICS_EXIT_IF_TRUE(status != PJ_SUCCESS, "Cannot start pjsua");
}

void _ics_core_add_account(ics_data_t *data,char *s_ip, char *username, char*password) {

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

	data->acfg.user_data = (void *)data;

	status = pjsua_acc_add(&data->acfg, PJ_TRUE, &data->acc_id);
	ICS_RETURN_IF_TRUE(status != PJ_SUCCESS, "Cannot register account");
}

void _ics_core_make_call(ics_data_t *data, char * sip_addr) {
	pj_str_t uri;

	uri = pj_str(sip_addr);
	pjsua_call_make_call(data->acc_id, &uri, 0, NULL, NULL, NULL);
}

void _ics_core_answer_call(ics_data_t *data) {
	if (current_call == PJSUA_INVALID_ID)
		printf("No current call\n");
	else {
		pjsua_call_answer(current_call, 200, NULL, NULL);
	}
}

void _ics_core_hangup_call(ics_data_t *data, int renew) {
	if (current_call == PJSUA_INVALID_ID)
		printf("No current call\n");
	else {
		if (renew) 
			pjsua_call_hangup_all();
		else
			pjsua_call_hangup(current_call, 0, NULL, NULL);	
	}
}

void _ics_core_hold_call(ics_data_t *data) {
	if (current_call < 0)
		printf("No current call\n");
	else {
		pjsua_call_set_hold(current_call, NULL);
	}
}
void _ics_core_release_hold(ics_data_t *data) {
	if (current_call < 0)
		printf("No current call\n");
	else {
		pjsua_call_reinvite(current_call, PJ_TRUE, NULL);
	}
}

void _ics_core_set_register(ics_data_t *data, int renew) {
	if (renew == 1 || renew == 0)
		pjsua_acc_set_registration(data->acc_id, renew);
	else
		printf("Invalid input");
}

void _ics_core_transfer_call(ics_data_t *data, int call_id_1, int call_id_2) {
#if 0
	if ( (call_id_1 != call_id_2) && pjsua_call_is_active(call_id_1) && pjsua_call_is_active(call_id_2) ) {
		pjsua_call_xfer_replaces(call_id_1, call_id_2, 0, NULL);
	}
	else
		printf("Cannot transfer call!\n");
#endif

//For test only:
#if 1
	int i, max;
	pjsua_call_info ci;

	max = pjsua_call_get_count();
	printf("You have %d active call%s\n", max, (max>1?"s":""));

	for (i = 0; i < max; i++){	
		if (pjsua_call_is_active(i)) {
			pjsua_call_get_info(i, &ci);
			pjsua_call_xfer_replaces(current_call, ci.id, 0, NULL);
			break;
		}
	}
#endif
}

void _ics_core_adjust_audio_volume(ics_data_t *data, char *device, float level) {
	if (strcmp(device,"r") == 0) {
		pjsua_conf_adjust_rx_level(0, level);
	}
	else if (strcmp(device,"t") == 0) {
		pjsua_conf_adjust_tx_level(0, level);
	}
	else
		printf("Invalid input to adjust device");
}

void _ics_core_clean(ics_data_t *data) {
	data->f_quit = 0;
	pjsua_destroy();
	pj_pool_release(data->pool);
	pj_caching_pool_destroy(&data->cp);
}

//Put command into queue
void ics_core_connect(ics_data_t *data, int port){
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_connect_cmd((ics_cmd_t *)p_item->data, port);
	queue_enqueue(&data->queue, (void *)p_item);
}
void ics_core_add_account(ics_data_t *data, char *s_ip, char *username, char *password) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_add_acc_cmd((ics_cmd_t *)p_item->data, s_ip, username, password);
	queue_enqueue(&data->queue, (void *)p_item);
}
void ics_core_make_call(ics_data_t *data, char *sip_addr) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_make_call_cmd((ics_cmd_t *)p_item->data, sip_addr);
	queue_enqueue(&data->queue, (void *)p_item);
}
void ics_core_answer_call(ics_data_t *data) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_answer_call_cmd((ics_cmd_t *)p_item->data);
	queue_enqueue(&data->queue, (void *)p_item);
}
void ics_core_hangup_call(ics_data_t *data, int renew) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_hangup_call_cmd((ics_cmd_t *)p_item->data, renew);
	queue_enqueue(&data->queue, (void *)p_item);
}
void ics_core_hold_call(ics_data_t *data) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_hold_call_cmd((ics_cmd_t *)p_item->data);
	queue_enqueue(&data->queue, (void *)p_item);
}

void ics_core_release_hold(ics_data_t *data) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_release_call_cmd((ics_cmd_t *)p_item->data);
	queue_enqueue(&data->queue, (void *)p_item);
}

void ics_core_transfer_call(ics_data_t *data, int call_id_1, int call_id_2) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_transfer_call_cmd((ics_cmd_t *)p_item->data, call_id_1, call_id_2);
	queue_enqueue(&data->queue, (void *)p_item);
}

void ics_core_set_register(ics_data_t *data, int renew) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_set_register_cmd((ics_cmd_t *)p_item->data, renew);
	queue_enqueue(&data->queue, (void *)p_item);
}

void ics_core_adjust_audio_volume(ics_data_t *data, char *device, float level) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_adjust_audio_cmd((ics_cmd_t *)p_item->data, device, level);
	queue_enqueue(&data->queue, (void *)p_item);
}

void ics_core_clean(ics_data_t *data) {
	opool_item_t *p_item = opool_get(&data->opool);

	ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;
	build_clean_cmd((ics_cmd_t *)p_item->data);
	queue_enqueue(&data->queue, (void *)p_item);
}


//Dequeue and do command
static void *thread_proc(void *param) {
	ics_data_t *data = (ics_data_t *) param;	
	while(data->f_quit) {

		opool_item_t *p_item = opool_get(&data->opool);
		p_item = (opool_item_t *)queue_dequeue(&data->queue);

		ics_cmd_t *cmd = (ics_cmd_t *)p_item->data;

		switch(cmd->cmd.cmd_id) {
			case CMD_CONNECT:
				_ics_core_connect(data, cmd->connect_cmd.port);
				break;
			case CMD_ADD_ACC:
				_ics_core_add_account(data, cmd->add_acc_cmd.s_ip, cmd->add_acc_cmd.username, cmd->add_acc_cmd.password);
				break;
			case CMD_MAKE_CALL:
				_ics_core_make_call(data, cmd->make_call_cmd.sip_addr);
				break;
			case CMD_ANSWER_CALL: 
				_ics_core_answer_call(data);
				break;
			case CMD_HANGUP_CALL: 
				_ics_core_hangup_call(data, cmd->hangup_call_cmd.renew);
				break;
			case CMD_HOLD_CALL:
				_ics_core_hold_call(data);
				break;
			case CMD_RELEASE_HOLD:
				_ics_core_release_hold(data);
				break;
			case CMD_TRANSFER_CALL:
				_ics_core_transfer_call(data, cmd->transfer_call_cmd.call_id_1, cmd->transfer_call_cmd.call_id_2);
				break;
			case CMD_SET_REGISTER:
				_ics_core_set_register(data, cmd->set_register_cmd.renew);
				break;
			case CMD_ADJUST_AUDIO:
				_ics_core_adjust_audio_volume(data, cmd->adjust_audio_cmd.device, cmd->adjust_audio_cmd.level);
				break;
			case CMD_CLEAN:
				_ics_core_clean(data);
				break;
			default:
				printf("Invalid command id %d\n", cmd->cmd.cmd_id);
				break;
		}
		// Event processing done
		opool_free(&data->opool, p_item);
	}
}

void ics_core_receive_command(ics_data_t *data) {
	data->f_quit = 1;
	int rc = pj_thread_create(data->pool, "ics_core_loop_thread", (pj_thread_proc*)&thread_proc, data, PJ_THREAD_DEFAULT_STACK_SIZE, 0, &data->thread);
	ICS_EXIT_IF_TRUE(rc != PJ_SUCCESS, "Cannot start thread for ics_core_start");
}

