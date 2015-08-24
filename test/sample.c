#include <pjsua-lib/pjsua.h>
#include "ics-core.h"

void print_menu();

void on_reg_start_impl(int account_id) {
	printf("Acc id:: %d\n", account_id); 
}

void on_reg_state_impl(int account_id, char* is_registration, int code, char *reason){
	printf("Acc id: %d\n", account_id);
	printf("Registed: %s \n", is_registration);
	printf("Status: %d(%s)\n", code, reason);
}

void on_incoming_call_impl(int account_id, int call_id, char *remote_contact, char *local_contact) {
	printf("Acc id: %d\n", account_id);
	printf("Call id: %d\n", call_id);
	printf("From: %s\n", remote_contact);	
	printf("To: %s\n", local_contact);
}

void on_call_state_impl(int call_id, char *st_text) {
	printf("Call %d state= %s\n", call_id, st_text);
}

void on_call_transfer_impl(int call_id, int st_code, char *st_text) {
	printf("Call id: %d\n", call_id);
	printf("Status: %d(%s)\n", st_code, st_text);
}

int main() {
	ics_data_t app_data;
	char option[10];
	char *retVal;
	int is_running;
	int chose;
	char sip_add[50];

	ics_core_create(&app_data);
	ics_core_config_default(&app_data);
	ics_core_init(&app_data);

	ics_core_set_reg_start_callback(&on_reg_start_impl);
	ics_core_set_reg_state_callback(&on_reg_state_impl);
	ics_core_set_incoming_call_callback(&on_incoming_call_impl);
	ics_core_set_call_state_callback(&on_call_state_impl);
	ics_core_set_call_transfer_callback(&on_call_transfer_impl);

	ics_core_connect(&app_data);
	ics_core_register(&app_data, "192.168.2.50", "quy", "1234");

	ics_core_start(&app_data);

	is_running = 1;
	while(is_running) {
		//	print_menu();
		if (fgets(option, sizeof(option), stdin) == NULL ) {
			puts("NULL command\n");
		}
		switch(option[0]) {
			case 'm':
				printf("Chose a call:\n");
				printf("1.quy2@192.168.2.50\n");
				printf("2.quy3@192.168.2.50\n");
				printf("3.quy10@192.168.2.50\n");
				if (scanf("%d",&chose) != 1){
					printf("Invalid input value\n");
				}
				switch(chose) {
					case 1:
						strcpy(sip_add, "sip:quy2@192.168.2.50");
						ics_core_make_call(&app_data, sip_add);
						break;
					case 2:
						strcpy(sip_add, "sip:quy3@192.168.2.50");
						ics_core_make_call(&app_data, sip_add);
						break;	
					case 3:
						strcpy(sip_add, "sip:quy10@192.168.2.50");
						ics_core_make_call(&app_data, sip_add);
						break;
					default:
						printf("Press 'm' to make another call\n");
						break;
				}
				break;
			case 'a':
				ics_core_answer_call(&app_data);
				break;
			case 'h':
				if (option[1] == 'a')
					ics_core_hangup_call(&app_data, 1);
				else
					ics_core_hangup_call(&app_data, 0);
				break;
			case 'H':
				ics_core_hold_call(&app_data);
				break;
			case 'R':
				ics_core_release_hold(&app_data);
				break;
			case 't':
				ics_core_transfer_call_q(&app_data);
				break;
			case 'l':
				list_active_call();
			case 'u':
				ics_core_set_register(&app_data, 0);
				break;
			case 'r':
				ics_core_set_register(&app_data, 1);
				break;
			case 'q':
				ics_core_end(&app_data);
				ics_core_clean(&app_data);
				is_running = 0;
				break;
			case 'p':
				print_menu();
				break;
			default:
				break;
		}
		usleep(1000);
	}
	return 0;
}

void print_menu() {
	puts("+==============================+");
	puts("|       Call Commands:         |");
	puts("|                              |");
	puts("|  m  Make new call            |");
	puts("|  a  Answer call              |");
	puts("|  h  Hangup call  (ha=all)    |");
	puts("|  H  Hold call                |");
	puts("|  R  release hold             |");
	puts("|  t  Tranfer call             |");
	puts("|  l  List active call         |");
	puts("|  u  Un-register              |");
	puts("|  r  Re-register              |");
	puts("+==============================+");
}

