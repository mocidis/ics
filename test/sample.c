#include <pjsua-lib/pjsua.h>
#include "ics-core.h"

int main() {
	ics_data_t app_data;
	ics_core_create(&app_data);
	ics_core_config_default(&app_data);
	ics_core_init(&app_data);
	ics_core_connect(&app_data);
	ics_core_register(&app_data, "192.168.235.129", "quy", "1234");
	//while (1) {};
	ics_core_clean(&app_data);
	return 0;
}

