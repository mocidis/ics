#include <pjsua-lib/pjsua.h>
#include "ics-core.h"

int main() {
	ics_data_t app_data;
	char option[10];

	ics_core_create(&app_data);
	ics_core_config_default(&app_data);
	ics_core_init(&app_data);
	ics_core_connect(&app_data);
	ics_core_register(&app_data, "192.168.235.129", "quy", "1234");
	for (;;) {
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
		puts("Enter your command:");
		if (fgets(option, sizeof(option), stdin) == NULL ) {
			puts("NULL command\n");
		}
		switch(option[0]) {
			case 'm':
				ics_core_make_call();
				
		}
		
	}
	ics_core_clean(&app_data);
	return 0;
}

