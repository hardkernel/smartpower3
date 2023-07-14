#include <Wire.h>
#include "smartpower3.h"

#include "esp_event.h"
#include "esp_event_base.h"

uint32_t ctime1 = 0;

volatile int interruptFlag;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

ESP_EVENT_DEFINE_BASE(SETTINGS_EVENTS);

void IRAM_ATTR onTimer()
{
	portENTER_CRITICAL_ISR(&timerMux);
	interruptFlag++;
	portEXIT_CRITICAL_ISR(&timerMux);
}

void isr_stpd01_ch0()
{
	screen_manager.getVoltageScreen()->setIntFlag(0);
}

void isr_stpd01_ch1()
{
	screen_manager.getVoltageScreen()->setIntFlag(1);
}

void initPAC1933(void)
{
	PAC.UpdateProductID();
	PAC.UpdateManufacturerID();
	PAC.UpdateRevisionID();
}

void btnTask(void *parameter)
{
	for (;;) {
		for (int i = 0; i < 4; i++)
			button[i].isr_pol();
		vTaskDelay(10);
	}
}






static void settings_voltage0_changed_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
	screen_manager.getVoltageScreen()->getChannel(0)->setVolt(settings.getChannel0Voltage(true), 2);
}

static void settings_voltage1_changed_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
	screen_manager.getVoltageScreen()->getChannel(1)->setVolt(settings.getChannel1Voltage(true), 2);
}

static void settings_current0_changed_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
	screen_manager.getVoltageScreen()->getChannel(0)->setCurrentLimit(settings.getChannel0CurrentLimit(true), 2);
}

static void settings_current1_changed_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
	screen_manager.getVoltageScreen()->getChannel(1)->setCurrentLimit(settings.getChannel1CurrentLimit(true), 2);
}

static void settings_visible_settings_changed_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
	wifi_manager->update_udp_info = true;
	wifi_manager->port_udp = settings.getWifiIpv4UdpLoggingServerPort(true);
	wifi_manager->ipaddr_udp = settings.getWifiIpv4UdpLoggingServerIpAddress(true);
}

static void wifi_disconnected_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
}

void logTask(void *parameter)
{
	char buffer_input[SIZE_LOG_BUFFER0];
	char buffer_ch0[SIZE_LOG_BUFFER1];
	char buffer_ch1[SIZE_LOG_BUFFER2];
	char buffer_checksum[SIZE_CHECKSUM_BUFFER];
	uint16_t log_interval = 0;
	uint8_t checksum8 = 0;
	uint8_t checksum8_xor = 0;
	SettingScreen *setting_screen = screen_manager.getSettingScreen();
	VoltageScreen *voltage_screen = screen_manager.getVoltageScreen();

	for (;;) {
		if (interruptFlag > 0) {
			portENTER_CRITICAL(&timerMux);
			interruptFlag--;
			portEXIT_CRITICAL(&timerMux);

			if (log_interval != setting_screen->getEnabledLogInterval()) {
				log_interval = setting_screen->getEnabledLogInterval();
				if (log_interval == 0)
					timerAlarmWrite(timer, 250000, true);
				else
					timerAlarmWrite(timer, 1000*log_interval, true);
			}
			if ((log_interval > 0) && !wifi_manager->isCommandMode()) {
				// this has a few interesting points:
				// %04d specifier is max 4 digits for uint16_t - which might be 5 digits
				// %1d specifier for uint8_t - which might be 3 digits
				// It does not happen because of the used data ranges, but it's safer to truncate the result
				// to prevent possible buffer overflow
				snprintf(buffer_input, SIZE_LOG_BUFFER0, "%010lu,%05d,%04d,%05d,%1d,", millis(), mCh0.V(), mCh0.A(log_interval), mCh0.W(log_interval), low_input);
				snprintf(buffer_ch0, SIZE_LOG_BUFFER1, "%05d,%04d,%05d,%1d,%02x,", mCh1.V(), mCh1.A(log_interval), mCh1.W(log_interval), onoff[0], voltage_screen->getIntStat(0));
				snprintf(buffer_ch1, SIZE_LOG_BUFFER2, "%05d,%04d,%05d,%1d,%02x,", mCh2.V(), mCh2.A(log_interval), mCh2.W(log_interval), onoff[1], voltage_screen->getIntStat(1));

				checksum8 = 0;
				checksum8_xor = 0;
				for (int i = 0; i < SIZE_LOG_BUFFER0-1; i++) {
					checksum8 += buffer_input[i];
					checksum8_xor ^= buffer_input[i];
				}
				for (int i = 0; i < SIZE_LOG_BUFFER1-1; i++) {
					checksum8 += buffer_ch0[i];
					checksum8_xor ^= buffer_ch0[i];
				}
				for (int i = 0; i < SIZE_LOG_BUFFER2-1; i++) {
					checksum8 += buffer_ch1[i];
					checksum8_xor ^= buffer_ch1[i];
				}
				snprintf(buffer_checksum, SIZE_CHECKSUM_BUFFER, "%02x,%02x\r\n", static_cast<byte>((~checksum8)+1), checksum8_xor);

				Serial.printf(buffer_input);
				Serial.printf(buffer_ch0);
				Serial.printf(buffer_ch1);
				Serial.printf(buffer_checksum);
				wifi_manager->runWiFiLogging(buffer_input, buffer_ch0, buffer_ch1, buffer_checksum);
			} else {
				vTaskDelay(10);
			}
		} else if (log_interval == 0) {
			vTaskDelay(250);
		} else if (log_interval > 5) {
			vTaskDelay(1);
		}
	}
}

void screenTask(void *parameter)
{
	wl_status_t old_state = WL_IDLE_STATUS;
	for (;;) {
		if (old_state != WiFi.status()) {
			old_state = WiFi.status();
			screen_manager.getActiveScreen()->updateWifiInfo();
		}

		screen_manager.run();
		vTaskDelay(10);
	}
}

void inputTask(void *parameter)
{
	uint8_t pressed;
	unsigned long cur_time;

	for (;;) {
		cur_time = millis();

		for (int i = 0; i < 4; i++) {
			pressed = button[i].checkPressed();
			if (pressed == 1)
				screen_manager.getBtnPress(i, cur_time, false);
			else if (pressed == 2)  // long press
				screen_manager.getBtnPress(i, cur_time, true);
		}
		if (dial.cnt != 0) {
			screen_manager.getActiveScreen()->countDial(dial.cnt, dial.direct, dial.step, cur_time);
			dial.cnt = 0;
		}
		screen_manager.getActiveScreen()->setTime(cur_time);
		vTaskDelay(10);
	}
}

void wifiTask(void *parameter)
{
	for (;;) {
		if (wifi_manager->canConnect() && wifi_manager->isWiFiEnabled()) {
			wifi_manager->apConnectFromSettings();
		} else if (!wifi_manager->isWiFiEnabled()) {
			wifi_manager->apDisconnectAndTurnWiFiOff();
		}
		screen_manager.setWiFiIconState();

		if (Serial.available()) {
//TODO: Check the functionality of Serial in SCPI and non-SCPI mode
			if (wifi_manager->isCommandMode()) {
				wifi_manager->WiFiMenuMain(Serial.read());
			} else if (wifi_manager->getOperationMode() == OPERATION_MODE_SCPI) {
				scpi_manager->processInput(Serial);
			} else {
				if (Serial.read() == SERIAL_CTRL_C && wifi_manager->getOperationMode() == OPERATION_MODE_DEFAULT) {
					wifi_manager->enterCommandMode();
					wifi_manager->viewMainMenu();
				} else {
					Serial.printf(F(">>> Unknown command <<<\n\r"));
				}
			}
		}
		vTaskDelay(50);
	}
}

void setup(void) {
	Serial.begin(115200);

	settings.init();

/*	esp_event_loop_args_t loop_with_task_args = {
			.queue_size = 5,
			.task_name = "loop_task", // task will be created
			.task_priority = uxTaskPriorityGet(NULL),
			.task_stack_size = 3072,
			.task_core_id = tskNO_AFFINITY
	};*/
	//esp_event_loop_handle_t& loop_with_task = settings.getEventLoopHandleAddress();

	//esp_event_loop_create(&loop_with_task_args, &loop_with_task);
	esp_event_loop_create_default();

	I2CA.begin(15, 4, (uint32_t)10000);
	I2CB.begin(21, 22, (uint32_t)800000);
	PAC.begin(&I2CB);
	wifi_manager = new WiFiManager(&settings);
	screen_manager.begin(&settings, wifi_manager, &I2CA);
	initEncoder(&dial);  // this also starts a task, without specified core
	scpi_manager = new SCPIManager(&screen_manager, &mChs);
	scpi_manager->init();

	pinMode(25, INPUT_PULLUP);
	pinMode(26, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(25), isr_stpd01_ch0, FALLING);
	attachInterrupt(digitalPinToInterrupt(26), isr_stpd01_ch1, FALLING);

	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, &onTimer, true);
	timerAlarmWrite(timer, 1000000, true);
	timerAlarmEnable(timer);

	xTaskCreatePinnedToCore(screenTask, "Draw Screen", 2048, NULL, 1, &screen_handle, 1);  // delay 10
	xTaskCreatePinnedToCore(wifiTask, "WiFi Task", 2512, NULL, 1, &wifi_handle, 1);  // delay 50
	xTaskCreatePinnedToCore(logTask, "Log Task", 2048, NULL, 1, &log_handle, 1);  // delay 10, 250 or 1 depending on logging interval and interrupt count
	xTaskCreate(inputTask, "Input Task", 672, NULL, 1, &input_handle);  // delay 10, also counts for screen
	xTaskCreate(btnTask, "Button Task", 672, NULL, 1, &button_handle);  // delay 10

	//esp_event_loop_create_default();
	/*esp_event_handler_instance_register_with((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_VOLTAGE0_CHANGED_EVENT, settings_voltage0_changed_handler, NULL, NULL);
	esp_event_handler_instance_register_with((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_VOLTAGE1_CHANGED_EVENT, settings_voltage1_changed_handler, NULL, NULL);
	esp_event_handler_instance_register_with((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_CURRENT0_CHANGED_EVENT, settings_current0_changed_handler, NULL, NULL);
	esp_event_handler_instance_register_with((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_CURRENT1_CHANGED_EVENT, settings_current1_changed_handler, NULL, NULL);
	esp_event_handler_instance_register_with((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_LOGGING_PORT_CHANGED_EVENT, settings_visible_settings_changed_handler, NULL, NULL);
	esp_event_handler_instance_register_with((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_LOGGING_ADDRESS_CHANGED_EVENT, settings_visible_settings_changed_handler, NULL, NULL);*/

	esp_event_handler_instance_register(SETTINGS_EVENTS, SETTINGS_VOLTAGE0_CHANGED_EVENT, settings_voltage0_changed_handler, NULL, NULL);
	esp_event_handler_instance_register(SETTINGS_EVENTS, SETTINGS_VOLTAGE1_CHANGED_EVENT, settings_voltage1_changed_handler, NULL, NULL);
	esp_event_handler_instance_register(SETTINGS_EVENTS, SETTINGS_CURRENT0_CHANGED_EVENT, settings_current0_changed_handler, NULL, NULL);
	esp_event_handler_instance_register(SETTINGS_EVENTS, SETTINGS_CURRENT1_CHANGED_EVENT, settings_current1_changed_handler, NULL, NULL);
	esp_event_handler_instance_register(SETTINGS_EVENTS, SETTINGS_LOGGING_PORT_CHANGED_EVENT, settings_visible_settings_changed_handler, NULL, NULL);
	esp_event_handler_instance_register(SETTINGS_EVENTS, SETTINGS_LOGGING_ADDRESS_CHANGED_EVENT, settings_visible_settings_changed_handler, NULL, NULL);
	esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, wifi_disconnected_handler, NULL, NULL);  // sent even if the password is wrong etc.
}

void loop() {
	onoff = screen_manager.getOnOff();

	mChs.sample();

	if ((millis() - ctime1) > 300) {
		ctime1 = millis();
		if (mCh0.V() < 6000) {
			screen_manager.debug();
			for (int i = 0; i < 3; i++) {
				mChs.sample();
				if (mCh0.V() > 6000) {
					break;
				}
				low_input = true;
			}
		} else {
			low_input = false;
		}
		screen_manager.pushInputPower(mCh0.V(), mCh0.A(300), mCh0.W(300));
		screen_manager.pushPower(mCh1.V(), mCh1.A(300), mCh1.W(300), 0);
		screen_manager.pushPower(mCh2.V(), mCh2.A(300), mCh2.W(300), 1);

		if (!screen_manager.getShutdown()) {
			if ((millis()/1000)%2)
				screen_manager.writeSysLED(50);
			else
				screen_manager.writeSysLED(0);
			screen_manager.writePowerLED(50);
		}
	}

	if (screen_manager.getShutdown()) {
		screen_manager.dimmingLED(1);
	}
#ifdef TASK_STACK_HIGH_WATERMARK_SERIAL_PRINT
	//Serial.printf("Loop: %u\n\r", uxTaskGetStackHighWaterMark(NULL));
	Serial.printf("WiFi task: %u\n\r", uxTaskGetStackHighWaterMark(wifi_handle));
	Serial.printf("Screen task: %u\n\r", uxTaskGetStackHighWaterMark(screen_handle));
	Serial.printf("Button task: %u\n\r", uxTaskGetStackHighWaterMark(button_handle));
	Serial.printf("Input task: %u\n\r", uxTaskGetStackHighWaterMark(input_handle));
	Serial.printf("Log task: %u\n\r", uxTaskGetStackHighWaterMark(log_handle));
	//Serial.printf("Encoder task: %u\n\r", uxTaskGetStackHighWaterMark(encoder_handle));
#endif
#ifdef HEAP_INFO_SERIAL_PRINT
	heap_caps_print_heap_info(MALLOC_CAP_8BIT);
#endif
}
