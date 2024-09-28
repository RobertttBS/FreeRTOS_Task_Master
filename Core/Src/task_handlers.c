/*
 * task_handlers.c
 *
 *  Created on: Sep 10, 2024
 *      Author: robert
 */
#include "main.h"

const char *msg_inv = "!!!!Invalid option!!!!\n";

state_t curr_state = sMainMenu;

void menu_task(void *param) {
	uint32_t cmd_addr;
	command_t *cmd;
	int option;
	const char *msg_menu = "\n========================\n"
			"|         Menu         |\n"
			"========================\n"
			"LED effect    ----> 0\n"
			"Date and time ----> 1\n"
			"Exit          ----> 2\n"
			"Enter your choice here : ";

	while (1) {
		xQueueSend(q_print, &msg_menu, portMAX_DELAY);

		//wait for menu commands
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
		cmd = (command_t*) cmd_addr;

		if (cmd->len == 1) {
			option = cmd->payload[0] - '0';
			switch (option) {
			case 0:
				const char *msg_led = "Switch to LED task\n";
				xQueueSend(q_print, &msg_led, portMAX_DELAY);
				curr_state = sLedEffect;
				xTaskNotify(handle_led_task, 0, eNoAction);
				break;
			case 1:
				const char *msg_rtc = "Switch to RTC task\n";
				xQueueSend(q_print, &msg_rtc, portMAX_DELAY);
				curr_state = sRtcMenu;
				xTaskNotify(handle_rtc_task, 0, eNoAction);
				break;
			case 2: /*implement exit */
				break;
			default:
				xQueueSend(q_print, &msg_inv, portMAX_DELAY);
				continue;
			}

		} else {
			// invalid entry
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);
			continue;
		}
		// wait to run again when some other task notifies
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	} // while super loop
}

void uart_task(void *param) {
	uint32_t *msg;

	while (1) {
		// Get string from queue and then send it out.
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen((char*) msg),
		HAL_MAX_DELAY);
	} //while super loop
}

static inline int extract_command(command_t *cmd) {
	uint8_t item;
	BaseType_t status;
	uint8_t i = 0;

	// Check if there are any messages waiting in the queue.s
	status = uxQueueMessagesWaiting(q_data);
	if (!status)
		return -1;

	do {
		status = xQueueReceive(q_data, &item, 0);
		if (status == pdTRUE && item != '\r')
			cmd->payload[i++] = item;
	} while (item != '\n');

	cmd->payload[i - 1] = '\0';
	cmd->len = i - 1; /*save  length of the command excluding null char */

	return 0;
}

void cmd_handler_task(void *param) {
	BaseType_t ret;
	command_t cmd;

	while (1) {
		/*Implement notify wait */
		ret = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		if (ret == pdTRUE) {
			/*process the user data(command) stored in input data queue */
			extract_command(&cmd);

			switch (curr_state) {
			case sMainMenu:
				xTaskNotify(handle_menu_task, (uint32_t )&cmd,
						eSetValueWithOverwrite);
				break;

			case sLedEffect:
				xTaskNotify(handle_led_task, (uint32_t )&cmd,
						eSetValueWithOverwrite);
				break;

			case sRtcMenu:
			case sRtcTimeConfig:
			case sRtcDateConfig:
			case sRtcReport:
				xTaskNotify(handle_rtc_task, (uint32_t )&cmd,
						eSetValueWithOverwrite);
				break;

			}
		}
	}
}

void led_effect_stop(void) {
	xTimerStop(handle_led_timer, portMAX_DELAY);
}

static inline void led_effect(int n) {
	// use sprintf() put "You select type %d\n"
	char msg[50];
	sprintf(msg, "You select type %d led effect\n", n);
	HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	led_effect_stop();
	xTimerChangePeriod(handle_led_timer, n * portTICK_PERIOD_MS * 1000, 100);
}

void rtc_time_config(void) {
	RTC_TimeTypeDef time;
	const char *msg = "Enter hour(1-12):minutes(0-59):seconds(0-59)\n";
	int hour;
	int min;
	int sec;
	uint32_t cmd_addr;
	command_t *cmd;

	xQueueSend(q_print, &msg, portMAX_DELAY);

	xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
	cmd = (command_t*) cmd_addr;

	// parse the string "cmd", which format is <hour>:<min>:<sec> into the uint8_t variables
	if (sscanf((char*) cmd->payload, "%d:%d:%d", &hour, &min, &sec) == 3) {
		// Validate the input
		if (hour >= 1 && hour <= 12 && min >= 0 && min <= 59 && sec >= 0
				&& sec <= 59) {
			time.Hours = (uint8_t) hour;
			time.Minutes = (uint8_t) min;
			time.Seconds = (uint8_t) sec;

			rtc_configure_time(&time);
			// Print success message
			msg = "Time set successfully\n";
		} else {
			// invalid entry
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);
		}
	} else {
		// invalid entry
		xQueueSend(q_print, &msg_inv, portMAX_DELAY);
	}
}

void rtc_date_config(void) {
	RTC_DateTypeDef date;
	const char *msg = "Enter month:day:year (e.g., 1:1:2023)\n";
	int month, day, year;
	uint32_t cmd_addr;
	command_t *cmd;

	xQueueSend(q_print, &msg, portMAX_DELAY);

	xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
	cmd = (command_t*) cmd_addr;

	if (sscanf((char*) cmd->payload, "%d:%d:%d", &month, &day, &year) == 3) {
		if (year >= 0 && year <= 100 && month >= 1 && month <= 12 && day >= 1
				&& day <= 31) {
			date.Date = (uint8_t) day;
			date.Month = (uint8_t) month;
			date.Year = (uint8_t) (year - 2000); // RTC typically uses years since 2000

			rtc_configure_date(&date);
			msg = "Date set successfully\n";
			xQueueSend(q_print, &msg, portMAX_DELAY);
		} else {
			msg = "Invalid date range. Please enter valid values.\n";
			xQueueSend(q_print, &msg, portMAX_DELAY);
		}
	} else {
		msg = "Invalid input format. Please use MONTH:DAY:YEAR format.\n";
		xQueueSend(q_print, &msg, portMAX_DELAY);
	}
}

portTASK_FUNCTION( rtc_task, pvParameters ) {
	uint32_t cmd_addr;
	command_t *cmd;
	int option;
	const char *msg_rtc1 = "========================\n"
			"|         RTC          |\n"
			"========================\n";
	const char *msg_rtc2 = "Configure Time            ----> 0\n"
			"Configure Date            ----> 1\n"
			"Enable reporting          ----> 2\n"
			"Exit                      ----> 3\n"
			"Enter your choice here : ";

	while (1) {
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		// Show the RTC menu
		xQueueSend(q_print, &msg_rtc1, portMAX_DELAY);
		show_time_date();
		xQueueSend(q_print, &msg_rtc2, portMAX_DELAY);

		/*Wait for command notification (Notify wait) */
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
		cmd = (command_t*) cmd_addr;

		if (cmd->len == 1) {
			option = cmd->payload[0] - '0';
			switch (option) {
			case 0:
				xQueueSend(q_print, &(char* ) { "Configure time\n" },
						portMAX_DELAY);
				curr_state = sRtcTimeConfig;
				rtc_time_config();
				break;
			case 1:
				xQueueSend(q_print, &(char* ) { "Configure Date\n" },
						portMAX_DELAY);
				curr_state = sRtcDateConfig;
				rtc_date_config();
				break;
			case 2: /*implement exit */
				xQueueSend(q_print, &(char* ) { "Enable export time\n" },
						portMAX_DELAY);
				curr_state = sRtcReport;

				break;
			default:
				xQueueSend(q_print, &msg_inv, portMAX_DELAY);
				continue;
			}
		} else {
			// invalid entry
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);
			continue;
		}

		/* update state variable */
		curr_state = sMainMenu;
		/*Notify menu task */
		xTaskNotify(handle_menu_task, 0, eNoAction);
	}
}

portTASK_FUNCTION( led_task, pvParameters ) {
	uint32_t cmd_addr;
	command_t *cmd;
	const char *msg_led = "========================\n"
			"|      LED Effect     |\n"
			"========================\n"
			"(none,e1,e2,e3,e4)\n"
			"Enter your choice here : ";
	while (1) {
		/*Wait for notification (Notify wait) */
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		/*Print LED menu */
		xQueueSend(q_print, &msg_led, portMAX_DELAY);

		/*wait for LED command (Notify wait) */
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
		cmd = (command_t*) cmd_addr;

		if (cmd->len <= 4) {
			if (!strcmp((char*) cmd->payload, "none"))
				led_effect_stop();
			else if (!strcmp((char*) cmd->payload, "e1"))
				led_effect(1);
			else if (!strcmp((char*) cmd->payload, "e2"))
				led_effect(2);
			else if (!strcmp((char*) cmd->payload, "e3"))
				led_effect(3);
			else if (!strcmp((char*) cmd->payload, "e4"))
				led_effect(4);
			else
				xQueueSend(q_print, &msg_inv, portMAX_DELAY); /*print invalid message */
		} else
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);

		/* update state variable */
		curr_state = sMainMenu;

		/*Notify menu task */
		xTaskNotify(handle_menu_task, 0, eNoAction);
	}
}
