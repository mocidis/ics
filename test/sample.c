#include <pjsua-lib/pjsua.h>
#include "ics-core.h"

int main() {
	ics_data_t app_data;
	char option[10];
	char *retVal;
	int is_running;

	ics_core_create(&app_data);
	ics_core_config_default(&app_data);
	ics_core_init(&app_data);
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
				ics_core_make_call(&app_data);
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
				ics_core_tranfer_call(&app_data);
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

