#include "ics-event.h"
#include <string.h>
#include "ics-common.h"
#include <stdlib.h>



#include <stdio.h>

char *ICS_EVENT_NAME[] = {
	"ICS_REG_START",
	"ICS_REG_STATE",
	"ICS_INCOMING_CALL",
	"ICS_TRANSFER"
};

void build_reg_start_event(ics_event_t *event, int account_id) {
	event->reg_start_event.eventid = ICS_REG_START;
	event->event.eventid = ICS_REG_START;	
	event->reg_start_event.account_id = account_id;
}
void build_reg_state_event(ics_event_t *event, int account_id, int is_registration, int code, char *reason, int reason_len) {
	event->reg_state_event.eventid = ICS_REG_STATE;
	event->event.eventid = ICS_REG_STATE;	
	event->reg_state_event.account_id = account_id;
	event->reg_state_event.is_registration = is_registration;
	event->reg_state_event.code = code;

	ICS_EXIT_IF_TRUE(reason_len < 0, "invalid value reason_len");
	ICS_EXIT_IF_TRUE(reason_len > sizeof(event->reg_state_event.reason), "Overflow in ics_reg_state_event.reason");
	strncpy(event->reg_state_event.reason, reason, reason_len);

	//event->reg_state_event.reason[reason_len] = '\0';
}
void build_incoming_call_event(ics_event_t *event, int account_id, int call_id, char *remote_contact, char *local_contact) {
	int local_len = sizeof(event->incoming_call_event.local_contact);
	int remote_len = sizeof(event->incoming_call_event.remote_contact);

	event->incoming_call_event.eventid = ICS_INCOMING_CALL;
	event->event.eventid = ICS_INCOMING_CALL;	
	event->incoming_call_event.account_id = account_id;
	event->incoming_call_event.call_id = call_id;

	ICS_EXIT_IF_TRUE((sizeof(remote_contact) < 0 || sizeof(local_contact) < 0), "invalid value reason_len");
	ICS_EXIT_IF_TRUE(sizeof(local_contact) > local_len, "Overflow in ics_incoming_call_event.local_contact");
	ICS_EXIT_IF_TRUE(sizeof(remote_contact) > remote_len, "Overflow in ics_incoming_call_event.remote_contact");
	strncpy(event->incoming_call_event.remote_contact, remote_contact, remote_len);
	strncpy(event->incoming_call_event.local_contact, local_contact, local_len);
	//event->incoming_call_event.remote_contact[remote_len] = '\0';
	//event->incoming_call_event.local_contact[local_len] = '\0';
}

void build_transfer_event(ics_event_t *event, int call_id, int st_code, char *st_text) {
	int len = sizeof(event->transfer_event.st_text);

	event->transfer_event.eventid = ICS_TRANSFER;
	event->event.eventid = ICS_TRANSFER;
	event->transfer_event.call_id = call_id;
	event->transfer_event.st_code = st_code;

	ICS_EXIT_IF_TRUE(sizeof(st_text) < 0, "Invalid value st_text");
	ICS_EXIT_IF_TRUE(sizeof(st_text) > len, "Overflow in ics_transfer_event.st_text");
	strncpy(event->transfer_event.st_text, st_text, len);
}


int is_reg_start_event(ics_event_t *event) {
	return event->event.eventid == ICS_REG_START;
}
int is_reg_state_event(ics_event_t *event) {
	return event->event.eventid == ICS_REG_STATE;
}
int is_incoming_call_event(ics_event_t *event) {
	return event->event.eventid == ICS_INCOMING_CALL;
}
int is_transfer_event(ics_event_t *event) {
	return event->event.eventid == ICS_TRANSFER;
}

void print_event(ics_event_t *event) {
	ICS_EXIT_IF_TRUE(event->event.eventid >= ICS_EVENT_END, "Unknown event id\n");
	printf("<=================================================>\n");
	printf("Event type: %s\n", ICS_EVENT_NAME[event->event.eventid]);
	if( is_reg_start_event(event) ) {
		printf("Acc id: %d\n", event->reg_start_event.account_id); 
	}
	else if (is_reg_state_event(event)) {
		printf("Acc id: %d\n", event->reg_state_event.account_id);
		printf("Registed: %s \n", (event->reg_state_event.is_registration>0 ? "Yes" : "No"));
		printf("Status: %d(%s)\n", event->reg_state_event.code, event->reg_state_event.reason);
	}
	else if (is_incoming_call_event(event)) {
		printf("Acc id: %d\n", event->incoming_call_event.account_id);
		printf("Call id: %d\n", event->incoming_call_event.call_id);
		printf("From: %s\n", event->incoming_call_event.remote_contact);	
		printf("To: %s\n",event->incoming_call_event.local_contact);
	}
	else if (is_transfer_event(event)) {
		printf("Call id: %d\n", event->transfer_event.call_id);
		printf("Status: %d(%s)\n", event->transfer_event.st_code, event->transfer_event.st_text);
	}
}

