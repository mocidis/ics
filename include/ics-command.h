#ifndef __ICS_COMMAND_H__
#define __ICS_COMMAND_H__

enum {
	CMD_CONNECT = 1,
	CMD_ADD_ACC = 2,
	CMD_MAKE_CALL = 3,
	CMD_ANSWER_CALL = 4,
	CMD_HANGUP_CALL = 5,
	CMD_HOLD_CALL = 6,
	CMD_RELEASE_HOLD = 7,
	CMD_TRANSFER_CALL = 8,
	CMD_SET_REGISTER = 9,
	CMD_CLEAN = 10,
	CMD_ADJUST_AUDIO = 11,
	CMD_CONFERENCE_CALL = 12,
	CMD_END = 13
};

extern char *ICS_CMD_NAME[];

#define ICS_CMD_COMMON_FIELDS \
	int cmd_id

typedef struct ics_common_cmd_s {
	ICS_CMD_COMMON_FIELDS;
} ics_common_cmd_t;

typedef struct ics_init_cmd_s {
	ICS_CMD_COMMON_FIELDS;
} ics_init_cmd_t;

typedef struct ics_connect_cmd_s {
	ICS_CMD_COMMON_FIELDS;
	int port;
} ics_connect_cmd_t;

typedef struct ics_add_acc_cmd_s {
	ICS_CMD_COMMON_FIELDS;
	char s_ip[50];
	char username[20];
	char password[20];
} ics_add_acc_cmd_t;

typedef struct ics_make_call_cmd_s {
	ICS_CMD_COMMON_FIELDS;
	char sip_addr[50];
} ics_make_call_cmd_t;

typedef struct ics_answer_call_cmd_s {
	ICS_CMD_COMMON_FIELDS;
} ics_answer_call_cmd_t;

typedef struct ics_hangup_call_cmd_s {
	ICS_CMD_COMMON_FIELDS;
	int renew;
} ics_hangup_call_cmd_t;

typedef struct ics_hold_call_cmd_s {
	ICS_CMD_COMMON_FIELDS;
} ics_hold_call_cmd_t;

typedef struct ics_release_hold_cmd_s {
	ICS_CMD_COMMON_FIELDS;
} ics_release_hold_cmd_t;

typedef struct ics_transfer_call_cmd_s {
	ICS_CMD_COMMON_FIELDS;
	int call_id_1;
	int call_id_2;
} ics_transfer_call_cmd_t;

typedef struct ics_set_registration_cmd_s {
	ICS_CMD_COMMON_FIELDS;
	int renew;
} ics_set_registration_cmd_t;

typedef struct ics_adjust_audio_cmd_s {
	ICS_CMD_COMMON_FIELDS;
	char device[10];
        float level;
} ics_adjust_audio_cmd_t;

typedef struct ics_conference_call_cmd_s {
	ICS_CMD_COMMON_FIELDS;
	int call_id;
} ics_conference_call_cmd_t;

typedef struct ics_clean_cmd_s {
	ICS_CMD_COMMON_FIELDS;
} ics_clean_cmd_t;

typedef union ics_cmd_s {
	ics_common_cmd_t cmd;
	ics_init_cmd_t init_cmd;
	ics_connect_cmd_t connect_cmd;
	ics_add_acc_cmd_t add_acc_cmd;
	ics_make_call_cmd_t make_call_cmd;
	ics_answer_call_cmd_t answer_call_cmd;
	ics_hangup_call_cmd_t hangup_call_cmd;
	ics_hold_call_cmd_t hold_call_cmd;
	ics_release_hold_cmd_t release_hold_cmd;
	ics_transfer_call_cmd_t transfer_call_cmd;
	ics_set_registration_cmd_t set_registration_cmd;
	ics_adjust_audio_cmd_t adjust_audio_cmd;
	ics_conference_call_cmd_t conference_call_cmd;
	ics_clean_cmd_t clean_cmd;
} ics_cmd_t;

void build_init_cmd(ics_cmd_t *cmd);
void build_connect_cmd(ics_cmd_t *cmd, int port);
void build_add_acc_cmd(ics_cmd_t *cmd, char *s_ip, char *username, char *password);
void build_make_call_cmd(ics_cmd_t *cmd, char *sip_addr);
void build_answer_call_cmd(ics_cmd_t *cmd);
void build_hangup_cal_cmd(ics_cmd_t *cmd, int renew);
void build_hold_call_cmd(ics_cmd_t *cmd);
void build_release_call_cmd(ics_cmd_t *cmd);
void build_transfer_call_cmd(ics_cmd_t *cmd, int call_id_1, int call_id_2);
void build_set_registration_cmd(ics_cmd_t *cmd, int renew);
void build_adjust_audio_cmd(ics_cmd_t *cmd, char *device, float level);
void build_conference_call_cmd(ics_cmd_t *cmd, int call_id);
void build_clean_cmd(ics_cmd_t *cmd);

int check_cmd_type(ics_cmd_t *cmd);

#endif




















