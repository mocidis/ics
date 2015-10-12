#include <pjsua-lib/pjsua.h>
#include "ics.h"

void print_menu();

void on_reg_start_impl(int account_id);
void on_reg_state_impl(int account_id, char* is_registration, int code, char *reason);
void on_incoming_call_impl(int account_id, int call_id, char *remote_contact, char *local_contact);
void on_call_state_impl(int call_id, char *st_text);
void on_call_transfer_impl(int call_id, int st_code, char *st_text);
void on_call_media_state_impl(int call_id, int st_code);

int main() {
	ics_t app_data;
	
	//For test only
	char option[10];
	int is_running;
	int chose;
	char sip_add[50];
	//

    ics_pool_init(&app_data);
    ics_pjsua_init(&app_data);
	ics_init(&app_data);

	ics_set_default_callback(&on_reg_start_default);

	ics_set_reg_start_callback(&on_reg_start_impl);
	ics_set_reg_state_callback(&on_reg_state_impl);
	ics_set_incoming_call_callback(&on_incoming_call_impl);
	ics_set_call_state_callback(&on_call_state_impl);
	ics_set_call_transfer_callback(&on_call_transfer_impl);
	ics_set_call_media_state_callback(&on_call_media_state_impl);
	ics_start(&app_data);
	ics_connect(&app_data, 12345);
	ics_add_account(&app_data, "192.168.2.50", "quy10", "1234");

	is_running = 1;
    
	while(is_running) {
		if (fgets(option, sizeof(option), stdin) == NULL ) {
			puts("NULL command\n");
		}
		switch(option[0]) {
			case 'm':
				printf("Chose a call:\n");
				printf("1.quy2@192.168.2.50\n");
				printf("2.quy3@192.168.2.50\n");
				printf("3.quy10@192.168.2.50\n");
				printf("4.ntt@192.168.2.50\n");
				printf("5.ntt1@191.168.2.50\n");
				printf("6. 1\n");
                printf("7.852@101.248.11.221");
				if (scanf("%d",&chose) != 1){
					printf("Invalid input value\n");
				}
				switch(chose) {
					case 1:
						strcpy(sip_add, "sip:quy2@192.168.2.50");
						ics_make_call(&app_data, sip_add);
						break;
					case 2:
						strcpy(sip_add, "sip:quy3@192.168.2.50");
						ics_make_call(&app_data, sip_add);
						break;	
					case 3:
						strcpy(sip_add, "sip:quy10@192.168.2.50");
						ics_make_call(&app_data, sip_add);
						break;
					case 4:
						strcpy(sip_add, "sip:ntt@192.168.2.50");
						ics_make_call(&app_data, sip_add);
						break;
					case 5:
						strcpy(sip_add, "sip:ntt1@192.168.2.50");
						ics_make_call(&app_data, sip_add);
						break;
					case 6:
						strcpy(sip_add, "sip:852901252465677@10.248.11.221");
						ics_make_call(&app_data, sip_add);
						break;
                    case 7:
                        strcpy(sip_add, "sip:@192.168.2.50");
                        ics_make_call(&app_data, sip_add);
                        break;
                    default:
                        printf("Press 'm' to make another call\n");
                        break;
                }
                break;
            case 'a':
                ics_answer_call(&app_data);
                break;
            case 'h':
                if (option[1] == 'a')
                    ics_hangup_call(&app_data, -2);
				else
					ics_hangup_call(&app_data, 0);
				break;
			case 'H':
				ics_hold_call(&app_data);
				break;
			case 'R':
				ics_release_hold(&app_data);
				break;
			case 't':
				if (option[1] == 'x') {
					ics_adjust_audio_volume(&app_data, "t", atof(&option[3])); // Adjust mic level (Transmitter)
				}
				else
					ics_transfer_call(&app_data, 1, 2);
				break;
			case 'c':
				ics_conference_call(&app_data, 1);
				break;
			case 'u':
				ics_set_registration(&app_data, 0);
				break;
			case 'r':
				if (option[1] == 'x')
					ics_adjust_audio_volume(&app_data, "r", atof(&option[3])); // Adjust speaker levela (Recevicer)
				else
					ics_set_registration(&app_data, 1);
				break;
			case 'q':
				ics_clean(&app_data);
				is_running = 0;
				break;
			case 'l':
				ics_list_call(&app_data);
				break;
			case 'p':
				print_menu();
				break;
			default:
				break;
		}
	}
	return 0;
}

void print_menu() {
	puts("+============================+");
	puts("|       Call Commands:       |");
	puts("|                            |");
	puts("|  m  :Make new call         |");
	puts("|  a  :Answer call           |");
	puts("|  h  :Hangup call  (ha=all) |");
	puts("|  H  :Hold call             |");
	puts("|  R  :Release hold          |");
	puts("|  t  :Tranfer call          |");
	puts("|  c  :Conference call       |");
	puts("|  u  :Un-register           |");
	puts("|  r  :Re-register           |");
	puts("|----------------------------|");
	puts("|  tx level :Adjust mic      |");
	puts("|  rx level :Adjust speaker  |");
	puts("+============================+");
}

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

void on_call_media_state_impl(int call_id, int st_code) {
	printf("Call id: %d\n", call_id);
	printf("Status: %d\n", st_code);
}
