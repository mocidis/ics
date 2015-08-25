#include "ics-event.h"
#include <string.h>
#include "ics-common.h"
#include "ics-command.h"
#include <stdlib.h>



#include <stdio.h>

char *ICS_CMD_NAME[] = {
	"CMD_CONNECT",
	"CMD_ADD_ACC",
	"CMD_MAKE_CALL",
	"CMD_ANSWER_CALL",
	"CMD_HANGUP_CALL",
	"CMD_HOLD_CALL",
	"CMD_RELEASE_HOLD",
	"CMD_TRANSFER_CALL",
	"CMD_SET_REGISTRER",
	"CMD_ADJUST_AUDIO" , 
	"CMD_CLEAN"
};

void build_connect_cmd(ics_cmd_t *cmd, int port) {
	cmd->connect_cmd.cmd_id = CMD_CONNECT;
	cmd->connect_cmd.port = port;
}
void build_add_acc_cmd(ics_cmd_t *cmd, char *s_ip, char *username, char *password) {
	int s_ip_len = sizeof(cmd->add_acc_cmd.s_ip);
	int username_len = sizeof(cmd->add_acc_cmd.username);
	int password_len = sizeof(cmd->add_acc_cmd.password);

	cmd->init_cmd.cmd_id = CMD_ADD_ACC;

	ICS_EXIT_IF_TRUE((sizeof(s_ip) < 0 || sizeof(username) < 0) || sizeof(password) < 0, "invalid value add_acc_cmd len");
	ICS_EXIT_IF_TRUE(sizeof(s_ip) > s_ip_len, "Overflow in add_acc_cmd.s_ip");
	ICS_EXIT_IF_TRUE(sizeof(username) > username_len, "Overflow in add_acc_cmd.username");
	ICS_EXIT_IF_TRUE(sizeof(password) > password_len, "Overflow in add_acc_cmd.password");

	strncpy(cmd->add_acc_cmd.s_ip, s_ip, s_ip_len);
	strncpy(cmd->add_acc_cmd.username, username, username_len);
	strncpy(cmd->add_acc_cmd.password, password, password_len);
}
void build_make_call_cmd(ics_cmd_t *cmd, char *sip_addr) {
	int sip_len = sizeof(cmd->make_call_cmd.sip_addr);

	cmd->init_cmd.cmd_id = CMD_MAKE_CALL;
	ICS_EXIT_IF_TRUE(sizeof(sip_addr) < 0, "Invalid value sip_addr");
	ICS_EXIT_IF_TRUE(sizeof(sip_addr) > sip_len, "Overflow in make_call_cmd.sip_addr");
	strncpy(cmd->make_call_cmd.sip_addr, sip_addr, sip_len);
}
void build_answer_call_cmd(ics_cmd_t *cmd) {
	cmd->answer_call_cmd.cmd_id = CMD_ANSWER_CALL;
}
void build_hangup_call_cmd(ics_cmd_t *cmd, int renew) {
	cmd->hangup_call_cmd.cmd_id = CMD_HANGUP_CALL;
	cmd->hangup_call_cmd.renew = renew;
}
void build_hold_call_cmd(ics_cmd_t *cmd) {
	cmd->hold_call_cmd.cmd_id = CMD_HOLD_CALL;
}
void build_release_call_cmd(ics_cmd_t *cmd) {
	cmd->release_hold_cmd.cmd_id = CMD_RELEASE_HOLD;
}
void build_transfer_call_cmd(ics_cmd_t *cmd, int call_id_1, int call_id_2) {
	cmd->transfer_call_cmd.cmd_id = CMD_TRANSFER_CALL;
	cmd->transfer_call_cmd.call_id_1 = call_id_1;
	cmd->transfer_call_cmd.call_id_2 = call_id_2;
}
void build_set_register_cmd(ics_cmd_t *cmd, int renew) {
	cmd->init_cmd.cmd_id = CMD_SET_REGISTER;
	cmd->set_register_cmd.renew = renew;
}
void build_adjust_audio_cmd(ics_cmd_t *cmd, char *device, float level) {
	int device_len = sizeof(cmd->adjust_audio_cmd.device);

	cmd->adjust_audio_cmd.cmd_id = CMD_ADJUST_AUDIO;
	
	ICS_EXIT_IF_TRUE(sizeof(device[0]) < 0, "Invalid value device\n");
	ICS_EXIT_IF_TRUE(sizeof(device[0]) > device_len, "Overflow in adjust_audio_cmd.device\n");
	strncpy(cmd->adjust_audio_cmd.device, device, device_len);
	cmd->adjust_audio_cmd.level = level;
}

void build_clean_cmd(ics_cmd_t *cmd) {
	cmd->cmd.cmd_id	= CMD_CLEAN;
	cmd->clean_cmd.cmd_id = CMD_CLEAN;
}

int check_cmd_type(ics_cmd_t *cmd){
	return cmd->cmd.cmd_id;
}
