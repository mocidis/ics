#ifndef __ICS_EVENT_H__
#define __ICS_EVENT_H__

enum {
	ICS_REG_START = 0,
	ICS_REG_STATE = 1,
	ICS_INCOMING_CALL = 2,
	ICS_CALL_STATE = 3,
	ICS_TRANSFER = 4,
	ICS_CALL_MEDIA_STATE = 5,
	ICS_EVENT_END = 6
};

extern char *ICS_EVENT_NAME[];


#define ICS_EVENT_COMMON_FIELDS \
	int eventid

typedef struct ics_common_event_s {
	ICS_EVENT_COMMON_FIELDS;
} ics_common_event_t;

typedef struct ics_reg_start_event_s {
	ICS_EVENT_COMMON_FIELDS;
	int account_id;
} ics_reg_start_event_t;

typedef struct ics_reg_state_event_s {
	ICS_EVENT_COMMON_FIELDS;
	int account_id;
	int is_registration;
	int code; // SIP protocol status code
	char reason[100]; // explanation for "code"
} ics_reg_state_event_t;

typedef struct ics_incoming_call_event_s {
	ICS_EVENT_COMMON_FIELDS;
	int account_id;
	int call_id;
	char local_contact[100];
	char remote_contact[100];
} ics_incoming_call_event_t;

typedef struct ics_call_state_event_s {
	ICS_EVENT_COMMON_FIELDS;
	int call_id;
	char state[100];
} ics_call_state_event_t;

typedef struct ics_transfer_event_s {
	ICS_EVENT_COMMON_FIELDS;
	int call_id;
	int st_code;
	char st_text[100];
} ics_transfer_event_t;

typedef struct ics_call_media_state_event_s {
	ICS_EVENT_COMMON_FIELDS;
	int call_id;
	int st_code;
} ics_call_media_state_event_t;

typedef union ics_event_s {
	ics_common_event_t event;
	ics_reg_start_event_t reg_start_event;
	ics_reg_state_event_t reg_state_event;
	ics_incoming_call_event_t incoming_call_event;
	ics_call_state_event_t call_state_event;
	ics_transfer_event_t transfer_event;
	ics_call_media_state_event_t call_media_state_event;
} ics_event_t;

void build_reg_start_event(ics_event_t *event, int account_id);
void build_reg_state_event(ics_event_t *event, int account_id, int is_registration, int code, char *reason, int reason_len);
void build_incoming_call_event(ics_event_t *event, int account_id,int call_id, char *remote_contact, char *local_contact);
void build_call_state(ics_event_t *event, int call_id, char *state);
void build_transfer_event(ics_event_t *event, int call_id, int st_code, char *st_text);
void build_call_media_state_event(ics_event_t *event, int call_id, int st_code);

int is_reg_start_event(ics_event_t *event);
int is_reg_state_event(ics_event_t *event);
int is_incoming_call_event(ics_event_t *event);
int is_call_state_event(ics_event_t *event);
int is_transfer_event(ics_event_t *event);
int is_call_media_state_event(ics_event_t *event);

#endif
