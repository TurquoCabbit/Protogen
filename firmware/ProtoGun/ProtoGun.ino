#include <esp_task_wdt.h>
#include <driver/adc.h>
#include <TFT_eSPI.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <EEPROM.h>

#include "./Header Files/option.h"
#include "./Header Files/image.h"
#include "./Header Files/GPIO.h"
#include "./Header Files/parameter.h"
#include "./Header Files/LCD.h"
#include "./Header Files/ADC.h"
#include "./Header Files/BLE.h"


void setup()
{
	Serial.begin(115200);
	serial_log(0, "start up");
	gpio_init();

	parameter_init();
	
	queue_init();

	EEPROM.begin(EEPROM_SIZE);

	/*
	core_0	Fan, ADC, Ctrl, BLE, VM
	Core_1	GUI
	*/

	xTaskCreatePinnedToCore(
		GUI_task, /* Function to implement the task */
		"GUI_task", /* Name of the task */
		GUI_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		GUI_task_priority,  /* Priority of the task */
		&GUI_task_handle,  /* Task handle. */
		core_1);

	xTaskCreatePinnedToCore(
		Fan_task, /* Function to implement the task */
		"Fan_task", /* Name of the task */
		Fan_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		Fan_task_priority,  /* Priority of the task */
		&Fan_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		ADC_task, /* Function to implement the task */
		"ADC_task", /* Name of the task */
		ADC_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		ADC_task_priority,  /* Priority of the task */
		&ADC_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		Ctrl_task, /* Function to implement the task */
		"Ctrl_task", /* Name of the task */
		Ctrl_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		Ctrl_task_priority,  /* Priority of the task */
		&Ctrl_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		BLE_task, /* Function to implement the task */
		"BLE_task", /* Name of the task */
		BLE_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		BLE_task_priority,  /* Priority of the task */
		&BLE_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		VM_task, /* Function to implement the task */
		"VM_task", /* Name of the task */
		VM_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		VM_task_priority,  /* Priority of the task */
		&VM_task_handle,  /* Task handle. */
		core_0);

	serial_log(0, "create task done");

	if (EEPROM.read(EEPROM_Addr_Saved) == EEPROM_saved)
	{
		EEPROM_Load();
		serial_log(0, "Load EEPROM done");
	}

	ADC_mode(ADC_mode_pre_boost);
}

void loop()
{
	//core 1
	vTaskDelete(NULL); //Kill Loop()
}

void Fan_task(void * parameter)
{
	uint8_t mode = 0xFF;
	bool Fan_status = 0;
	uint32_t Fan_on_cnt = 0;
	for (;;)
	{
		if (xQueueReceive(queue_Fan, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Fan_mode_boost:
				Set_pwm(50, Fan_channel);
				vTaskDelay(6000 / portTICK_PERIOD_MS);
				Fan_mode(Fan_mode_general);
				break;
			case Fan_mode_off:
				Set_pwm(50, Fan_channel);
				vTaskDelete(NULL);
				break;
			case Fan_mode_general:
				if (Blaster.Fan_ready && Fan_on_cnt < Fan_on_Max_time)
				{
					if (Get_But_press(&But_Trigger) && !Fan_status)
					{
						if (Blaster.Fan_duty > 100)
						{
							Blaster.Fan_duty = 100;
						}
						else if (Blaster.Fan_duty < 50)
						{
							Blaster.Fan_duty = 50;
						}
						serial_log(0, "Fan on");
						Fan_status = 1;
						Set_pwm(Blaster.Fan_duty, Fan_channel);
					}
					else if(!Get_But_press(&But_Trigger) && Fan_status)
					{
						serial_log(0, "Fan off");
						Fan_status = 0;
						Set_pwm(50, Fan_channel);
					}
					if(Fan_status)
					{
						Fan_on_cnt += Fan_scan_cycle_time;
					}
					vTaskDelay(Fan_scan_cycle_time / portTICK_PERIOD_MS);
					Fan_mode(Fan_mode_general);
				}
				else
				{
					if (Fan_status)
					{
						Fan_status = 0;
						Set_pwm(50, Fan_channel);
					}
					if(Fan_on_cnt)
					{
						Fan_on_cnt = 0;
					}
				}
				break;
			}
		}
	}
}

void ADC_task(void * parameter)
{
	uint8_t mode = 0xFF;
	bool Deter_pin_temp = 0;
	uint8_t ADC_initiated = 0;
	uint32_t ADC_average_cnt = 0;
	uint32_t ADC_average = 0;
	for (;;)
	{
		if (xQueueReceive(queue_ADC, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case ADC_mode_pre_boost:
			
				ADC_average_cnt = 0;
				ADC_average = 0;
				Blaster.remote = !digitalRead(Deter_PIN);
				if (Blaster.remote)		//Blaster Deter_PIN high
				{
					Blaster.Fan_ready = 0;
					serial_log(0, "remote");
				}
				else					//Remote Deter_PIN low
				{
					Blaster.Fan_ready = 1;
					serial_log(0, "BLASTER");
				}
				if(!ADC_initiated)
				{
					ADC_init();
					ADC_initiated = 1;
				}
				Battery_level_init();
				Deter_pin_temp = Blaster.remote;
				vTaskDelay(5 / portTICK_PERIOD_MS);

			case ADC_mode_boost:
				if(ADC_average_cnt < ADC_init_average_time)
				{
					Blaster.remote = !digitalRead(Deter_PIN);
					ADC_average += ADC_convert(Blaster.remote);
					ADC_average_cnt += ADC_init_scan_intervial;
					vTaskDelay(ADC_init_scan_intervial / portTICK_PERIOD_MS);
				}
				else
				{
					Blaster.ADC_value = ADC_average / (ADC_init_average_time / ADC_init_scan_intervial);
					ADC_average_cnt = 0;
					ADC_average = 0;
					if (Blaster.ADC_value > Battery_level.Break && ADC_initiated == 1)
					{
						ADC_initiated = 2;
						Ctrl_mode(Ctrl_mode_boost);
					}
					else
					{
						serial_log(0, "BATT LOW");
						serial_log(0, ADC_average);
						serial_log(0, Battery_level.Break);
						ADC_mode(ADC_mode_boost);
					}
				}

				if(Blaster.remote != Deter_pin_temp)
				{
					ADC_mode(ADC_mode_pre_boost);
				}
				else
				{
					Deter_pin_temp = Blaster.remote;
				}
				ADC_mode(ADC_mode_boost);
				break;

			case ADC_mode_off:
				vTaskDelete(NULL);
				break;

			case ADC_mode_general:
				Blaster.remote = !digitalRead(Deter_PIN);
				if (ADC_average_cnt < ADC_average_time)
				{
					ADC_average += ADC_convert(Blaster.remote);
					ADC_average_cnt += ADC_scan_intervial;
				}
				else
				{
					ADC_mode(ADC_mode_calculation);
				}
				vTaskDelay(ADC_scan_intervial / portTICK_PERIOD_MS);
				ADC_mode(ADC_mode_general);
				break;

			case ADC_mode_calculation:			
				Blaster.ADC_value = ADC_average / (ADC_average_time / ADC_scan_intervial);
				ADC_average_cnt = 0;
				ADC_average = 0;
				if (Blaster.ADC_value < Battery_level.Break && Blaster.Power_Low)
				{
					Blaster.Power_Low = 1;
					Blaster.Power_Break = 1;
					Blaster.Fan_ready = 0;
					GUI_mode(GUI_mode_off);
					Fan_mode(Fan_mode_off);
					Ctrl_mode(Ctrl_mode_off);
					BLE_mode(BLE_mode_off);
					VM_mode(VM_mode_off);
					ADC_mode(ADC_mode_off);
				}
				else if (Blaster.ADC_value < Battery_level.Low)
				{
					Blaster.Power_Low = 1;
					Blaster.Fan_ready = 0;
				}
				else
				{
					Blaster.Power_Low = 0;
					Blaster.Power_Break = 0;
				}
				Blaster.Battery = (Blaster.ADC_value - Battery_level.Break) * 5 / (Battery_level.Full - Battery_level.Break);
				LCD_refresh_cnt.ADC = 1;
				vTaskDelay(5 / portTICK_PERIOD_MS);
				ADC_mode(ADC_mode_general);
				break;
			}
		}
	}
}

void GUI_task(void * parameter)
{
	uint8_t mode = 0xFF;
	uint8_t sleep_mode;
	uint8_t Face_show_index, setting_list_index = 0, leave_index = 1, beep_index = 0;
	uint8_t Face_toggle_temp;
	uint32_t temp_32 = GUI_temp32_init;
	uint16_t temp_16 = GUI_temp16_init;
	bool Max_brightness = 0;
	bool BLE_connect_last = BLE_connected;
	bool show = 0;
	bool toggled = 0;
	char temp_str[20];
	for (;;)
	{
		if (xQueueReceive(queue_GUI, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case GUI_mode_boost:
				LCD_init();
				Face_show_index = Blaster.Face_index;
				clear_encoder_flag();
				GUI_mode(GUI_mode_main);
				LCD_refresh_cnt.Face_resume = Face_resume_time + 1;
				break;

			case GUI_mode_off:
				tft.fillScreen(TFT_WHITE);  // clean screen
				digitalWrite(TFT_BL, 0); //turn off backlight
				vTaskDelete(NULL);
				break;

			case GUI_mode_sleep:
				digitalWrite(TFT_BL, 0); //turn off backlight
				if (Get_But(&But_X) || Get_But(&But_Y) || Get_But(&But_Enter) || Get_But(&But_Return) || Encoder_down || Encoder_up) //wake up
				{
					digitalWrite(TFT_BL, 1); //turn on backlight
					LCD_sleep_cnt = 0;
					clear_encoder_flag();
					GUI_mode(sleep_mode);
				}
				else
				{
					vTaskDelay(50 / portTICK_PERIOD_MS);
					GUI_mode(GUI_mode_sleep);
				}
				break;

			case GUI_mode_main:
				sleep_mode = mode;
				// Battery status=====================================
				if (LCD_refresh_cnt.Batt == Batt_refresh_time || show)
				{
					LCD_refresh_cnt.Batt = 0;

					if (Blaster.Battery < 5)
					{
						LCD_show_Battery(Batt[Blaster.Battery]);
					}
					else
					{
						LCD_show_Battery(Batt[4]);
					}
				}

				// Info===============================================
				if (LCD_refresh_cnt.info == info_refresh_time || show)
				{
					LCD_refresh_cnt.info = 0;

					if (BLE_connect_last)
					{
						LCD_show_BLE(Connected);
					}
					else
					{
						LCD_show_BLE(Unconnected);
					}

					if (Max_brightness)
					{
						LCD_show_lit(Max_lit);
					}
					else
					{
						LCD_show_lit(general_lit);
					}
				}

				// Main===============================================
				if (LCD_refresh_cnt.main == main_refresh_time || show)
				{
					show = 0;
					LCD_refresh_cnt.main = 0;
					LCD_show_main(Face[Face_show_index]);
				}

				//BLE connect status refresh
				if (BLE_connect_last != BLE_connected)
				{
					LCD_refresh_cnt.info = 1;
					BLE_connect_last = BLE_connected;
				}
				
				//Select face			Roll
				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Face_show_index < Face_index_top)
					{
						Face_show_index++;
						LCD_refresh_cnt.main = 1;
					}
					LCD_refresh_cnt.Face_resume = 0;
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Face_show_index > 0)
					{
						Face_show_index--;
						LCD_refresh_cnt.main = 1;
					}
					LCD_refresh_cnt.Face_resume = 0;
				}
				
				//resume face after 10s
				if (LCD_refresh_cnt.Face_resume == Face_resume_time && Face_show_index != Blaster.Face_index)
				{
					serial_log(0, "resume face");
					VM_mode(VM_mode_on_long);
					LCD_refresh_cnt.Face_resume = Face_resume_time + 1;
					Face_show_index = Blaster.Face_index;
					LCD_refresh_cnt.main = 1;
				}

				//Set face				Enter
				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					if(ProtoGun_setCMD(Face_cmd[Blaster.Face_index]))
					{
						serial_log(0, "Set face");
						Blaster.Face_index = Face_show_index;
						if (toggled)
						{
							toggled = 0;
						}
						
						LCD_refresh_cnt.Face_resume = Face_resume_time + 1;
						BLE_mode(BLE_mode_notify);
					}
				}

				//Fast toggle face		X
				if (Get_But(&But_X))
				{
					VM_mode(VM_mode_on_long);
					if(ProtoGun_setCMD(Face_cmd[Blaster.Face_index]))
					{
						serial_log(0, "Toggle face");
						if (!toggled)
						{
							Face_toggle_temp = Blaster.Face_index;
							Blaster.Face_index = Blaster.Face_toggle_index;
							Face_show_index = Blaster.Face_toggle_index;
							toggled = 1;
						}
						else
						{
							Face_show_index = Face_toggle_temp;
							Blaster.Face_index = Face_toggle_temp;
							toggled = 0;
						}					
						LCD_refresh_cnt.main = 1;
						BLE_mode(BLE_mode_notify);
					}
				}

				//Fast animate		Hold X 1sec
				if (Get_But(&But_X, 1000))
				{
					VM_mode(VM_mode_on_long);
					if(ProtoGun_setCMD(0x1A0))    //Fast animate
					{
						serial_log(0, "Replay animate");
						BLE_mode(BLE_mode_notify);
					}
				}

				//Fast Beep		Y
				if (Get_But(&But_Y))
				{
					VM_mode(VM_mode_on_long);
					if(ProtoGun_setCMD(0x000))    //Beep
					{
						serial_log(0, "Beep");
						BLE_mode(BLE_mode_notify);
					}
				}

				//Fast brightness		Return
				if (Get_But(&But_Return))
				{
					serial_log(0, "Brightness");
					VM_mode(VM_mode_on_long);
					if (!Max_brightness)
					{
						if(ProtoGun_setCMD(0x10F))    //Matrix Brightness_15
						{
							Max_brightness = 1;							
							BLE_mode(BLE_mode_notify);
							LCD_refresh_cnt.info = 1;
						}
					}
					else
					{
						if(ProtoGun_setCMD(0x100 + Blaster.Matrix_Brightness))
						{
							Max_brightness = 0;
							BLE_mode(BLE_mode_notify);
							LCD_refresh_cnt.info = 1;
						}
					}
				}

				//Setting				Hold return 1sec
				if (Get_But(&But_Return, 1000))
				{
					serial_log(0, "Setting");
					VM_mode(VM_mode_on_long);
					show = 1;
					setting_list_index = 0;
					GUI_mode(GUI_mode_setting);
				}

				//Signature				Hold Y 1sec
				else if (Get_But(&But_Y, 1000))
				{
					serial_log(0, "Signature");
					VM_mode(VM_mode_on_long);
					temp_16 = GUI_temp16_init;
					show = 1;
					GUI_mode(GUI_mode_signature);
				}
				else
				{
					GUI_mode(GUI_mode_main);
				}
				break;

			case GUI_mode_setting:
				sleep_mode = mode;
				if (show)
				{
					show = 0;
					LCD_show_list(Setting_list, setting_list_index, setting_list_top);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (setting_list_index)
					{
						setting_list_index--;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (setting_list_index < setting_list_top)
					{
						setting_list_index++;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					temp_32 = GUI_temp32_init;
					temp_16 = GUI_temp16_init;
					GUI_mode(GUI_mode_startface + setting_list_index);
				}
				else if (Get_But(&But_Return, 1000))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					temp_32 = GUI_temp32_init;
					temp_16 = GUI_temp16_init;
					GUI_mode(GUI_mode_main);
				}
				else
				{
					GUI_mode(GUI_mode_setting);
				}
				break;

			case GUI_mode_startface:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = Blaster.Face_startup_index;
				}
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Setting_list, setting_list_index);
					LCD_show_main(Face[Blaster.Face_startup_index]);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Face_startup_index > 0)
					{
						Blaster.Face_startup_index--;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Face_startup_index < Face_index_top)
					{
						Blaster.Face_startup_index++;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_setting);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Face_startup_index = temp_16;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_startface);
				}
				break;

			case GUI_mode_toggleface:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = Blaster.Face_toggle_index;
				}
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Setting_list, setting_list_index);
					LCD_show_main(Face[Blaster.Face_toggle_index]);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Face_toggle_index > 0)
					{
						Blaster.Face_toggle_index--;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Face_toggle_index < Face_index_top)
					{
						Blaster.Face_toggle_index++;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_setting);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Face_toggle_index = temp_16;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_toggleface);
				}
				break;

			case GUI_mode_Matrix_brightness:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = Blaster.Matrix_Brightness;
				}
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Setting_list, setting_list_index);

					Font_set_min();
					tft.setCursor(0, 120);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					sprintf(temp_str, "   %02d", Blaster.Matrix_Brightness);
					tft.printf(temp_str);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Matrix_Brightness > 0)
					{
						Blaster.Matrix_Brightness--;
						if(ProtoGun_setCMD(0x100 + Blaster.Matrix_Brightness))    //Matrix Brightness
						{
							BLE_mode(BLE_mode_notify);
							show = 1;
						}
						else
						{
							Blaster.Matrix_Brightness++;
						}
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Matrix_Brightness < 15)
					{
						Blaster.Matrix_Brightness++;
						if(ProtoGun_setCMD(0x100 + Blaster.Matrix_Brightness))    //Matrix Brightness
						{
							BLE_mode(BLE_mode_notify);
							show = 1;
						}
						else
						{
							Blaster.Matrix_Brightness--;
						}
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_setting);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Matrix_Brightness = temp_16;
					if(ProtoGun_setCMD(0x100 + Blaster.Matrix_Brightness))    //Matrix Brightness
					{
						BLE_mode(BLE_mode_notify);
					}
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_Matrix_brightness);
				}
				break;

			case GUI_mode_blink:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = Blaster.Blink_period;
				}
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Setting_list, setting_list_index);

					Font_set_min();
					tft.setCursor(0, 100);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					if (Blaster.Blink_period)
					{
						sprintf(temp_str, "   %02d s", Blaster.Blink_period * 2);
					}
					else
					{
						sprintf(temp_str, "   OFF");
					}
					tft.printf(temp_str);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Blink_period > 0)
					{
						Blaster.Blink_period--;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Blink_period < 15)
					{
						Blaster.Blink_period++;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_setting);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Blink_period = temp_16;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_blink);
				}
				break;

			case GUI_mode_beep:
				sleep_mode = mode;
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Setting_list, setting_list_index);
					LCD_show_list(Beep_list, beep_index, beep_list_top);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (beep_index)
					{
						beep_index--;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (beep_index < beep_list_top)
					{
						beep_index++;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					temp_16 = GUI_temp16_init;
					GUI_mode(GUI_mode_beep_mode + beep_index);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					beep_index = 0;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_beep);
				}
				break;

			case GUI_mode_beep_mode:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = Blaster.Beep_mode;
				}
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Beep_list, beep_index);
					LCD_show_list(Beep_mode_list, Blaster.Beep_mode, beep_mode_list_top);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Beep_mode)
					{
						Blaster.Beep_mode--;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Beep_mode < beep_mode_list_top)
					{
						Blaster.Beep_mode++;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_beep);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Beep_mode = temp_16;
					GUI_mode(GUI_mode_beep);
				}
				else
				{
					GUI_mode(GUI_mode_beep_mode);
				}
				break;

			case GUI_mode_beep_period:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = Blaster.Beep_period;
				}
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Beep_list, beep_index);

					Font_set_min();
					tft.setCursor(0, 100);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					if (Blaster.Beep_period)
					{
						sprintf(temp_str, "  Every\n  %02d time\n  blink", Blaster.Beep_period);
					}
					else
					{
						sprintf(temp_str, "\n   OFF");
					}
					tft.printf(temp_str);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Beep_period > 0)
					{
						Blaster.Beep_period--;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Beep_period < 15)
					{
						Blaster.Beep_period++;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_beep);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Beep_period = temp_16;
					GUI_mode(GUI_mode_beep);
				}
				else
				{
					GUI_mode(GUI_mode_beep_period);
				}
				break;

			case GUI_mode_beep_pitch:
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Beep_list, beep_index);

					Font_set_min();
					tft.setCursor(0, 100);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					if (Blaster.Beep_period)
					tft.printf("\n  Just\n  ROLL");
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if(ProtoGun_setCMD(0x021))    //Pitch down
					{
						BLE_mode(BLE_mode_notify);
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if(ProtoGun_setCMD(0x020))    //Pitch up
					{
						BLE_mode(BLE_mode_notify);
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_beep);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					if(ProtoGun_setCMD(0x022))    //Reset pitch
					{
						BLE_mode(BLE_mode_notify);
					}
					GUI_mode(GUI_mode_beep);
				}
				else
				{
					GUI_mode(GUI_mode_beep_pitch);
				}
				break;

			case GUI_mode_boop:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = Blaster.Boop;
				}
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Setting_list, setting_list_index);
					LCD_show_list(Boop_list, Blaster.Boop, boop_list_top);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Boop)
					{
						Blaster.Boop = 0;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Boop < boop_list_top)
					{
						Blaster.Boop = 1;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_setting);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Boop = temp_16;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_boop);
				}
				break;

			case GUI_mode_Neo_brightness:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = Blaster.Neo_Brightness;
				}
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					LCD_show_title(Setting_list, setting_list_index);

					Font_set_min();
					tft.setCursor(0, 100);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					sprintf(temp_str, "\n   %03d", Blaster.Neo_Brightness);
					tft.printf(temp_str);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Neo_Brightness > 0)
					{
						Blaster.Neo_Brightness--;
						if(ProtoGun_setCMD(0x300 + Blaster.Neo_Brightness))    //Matrix Brightness
						{
							BLE_mode(BLE_mode_notify);
							show = 1;
						}
						else
						{
							Blaster.Neo_Brightness++;
						}
						
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Neo_Brightness < 0xFF)
					{
						Blaster.Neo_Brightness++;
						if(ProtoGun_setCMD(0x300 + Blaster.Neo_Brightness))    //Matrix Brightness
						{
							BLE_mode(BLE_mode_notify);
							show = 1;
						}
						else
						{
							Blaster.Neo_Brightness--;
						}
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_setting);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Neo_Brightness = temp_16;
					if(ProtoGun_setCMD(0x300 + Blaster.Neo_Brightness))    //Matrix Brightness
					{
						BLE_mode(BLE_mode_notify);
					}
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_Neo_brightness);
				}
				break;

			case GUI_mode_Fan:
				sleep_mode = mode;
				if (temp_32 == GUI_temp32_init)
				{
					temp_32 = Blaster.Fan_duty;
					temp_16 = Blaster.Fan_ready;
					Blaster.Fan_ready = 1;
				}

				if (show)
				{
					show = 0;

					tft.fillScreen(TFT_BLACK);  // clean screen

					LCD_show_title(Setting_list, setting_list_index);

					Font_set_min();
					tft.setCursor(0, 100);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					sprintf(temp_str, "\n  %3d", Blaster.Fan_duty);
					tft.printf(temp_str);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.Fan_duty > 55)
					{
						Blaster.Fan_duty -= 5;
					}
					else
					{
						Blaster.Fan_duty = 50;
					}
					show = 1;
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.Fan_duty < 95)
					{
						Blaster.Fan_duty += 5;
					}
					else
					{
						Blaster.Fan_duty = 100;
					}
					show = 1;
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Fan_ready = temp_16;
					GUI_mode(GUI_mode_setting);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.Fan_duty = temp_32;
					Blaster.Fan_ready = temp_16;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_Fan);
				}
				break;

			case GUI_mode_ADC:
				sleep_mode = mode;
				if (temp_32 == GUI_temp32_init)
				{
					temp_32 = Blaster.ADC_gain;
				}
				if (LCD_refresh_cnt.ADC)
				{
					LCD_refresh_cnt.ADC = 0;

					tft.fillScreen(TFT_BLACK);  // clean screen

					LCD_show_title(Setting_list, setting_list_index);

					Font_set_min();
					tft.setCursor(0, 100);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					sprintf(temp_str, "  %.2f V", (float)(Blaster.ADC_value) / Blaster.ADC_gain);

					tft.printf(temp_str);

					tft.setCursor(0, 160);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					sprintf(temp_str, "  Gain\n  %7d", Blaster.ADC_gain);
					tft.printf(temp_str);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (Blaster.ADC_gain > 100)
					{
						Blaster.ADC_gain -= 100;
					}
					else
					{
						Blaster.ADC_gain = 0;
					}
					LCD_refresh_cnt.ADC = 1;
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (Blaster.ADC_gain < (0xFFFFFFFF - 100))
					{
						Blaster.ADC_gain += 100;
					}
					else
					{
						Blaster.ADC_gain = 0xFFFFFFFF;
					}
					show = 1;
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_setting);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					Blaster.ADC_gain = temp_32;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_ADC);
				}
				break;

			case GUI_mode_leave:
				sleep_mode = mode;
				if (show)
				{
					show = 0;
					LCD_show_list(Leave_list ,leave_index, leave_list_top);
				}

				if (Encoder_down)
				{
					Encoder_down = 0;
					if (leave_index)
					{
						leave_index = 0;
						show = 1;
					}
				}
				else if (Encoder_up)
				{
					Encoder_up = 0;
					if (leave_index < leave_list_top)
					{
						leave_index = 1;
						show = 1;
					}
				}

				if (Get_But(&But_Enter))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					if (leave_index)
					{
						GUI_mode(GUI_mode_save);
					}
					else
					{
						show = 1;
						setting_list_index = 0;
						leave_index = 1;
						tft.fillScreen(TFT_BLACK);  // clean screen
						GUI_mode(GUI_mode_main);
					}
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					leave_index = 1;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_leave);
				}
				break;

			case GUI_mode_reset:
				sleep_mode = mode;
				if (show)
				{
					show = 0;
					tft.fillScreen(TFT_BLACK);  // clean screen
					Font_set_min();
					tft.setCursor(0, 100);
					tft.setTextColor(TFT_WHITE);
					tft.setTextSize(1);
					tft.printf("\n  Hold Y");
				}
				
				if (Get_But(&But_Y, 3000))
				{
					VM_mode(VM_mode_on_long_thrice);
					if(ProtoGun_setCMD(0x1FF))    //Reset
					{
						BLE_mode(BLE_mode_notify);
						System_Reset();
					}
					else
					{
						System_Reset_offline();
					}
					Battery_level_init();
					show = 1;
					setting_list_index = 0;
					GUI_mode(GUI_mode_main);
				}
				else if(Get_But(&But_Y, 5000) && Get_But(&But_Enter, 5000))		//hard reset
				{
					VM_mode(VM_mode_on_extra_long);
					ProtoGun_setCMD(0x1FF);    //Reset
					BLE_mode(BLE_mode_notify);
					System_Reset();
					Battery_level_init();
					show = 1;
					setting_list_index = 0;
					GUI_mode(GUI_mode_main);
				}
				else if (Get_But(&But_Return))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_setting);
				}
				else
				{
					GUI_mode(GUI_mode_reset);
				}
				break;

			case GUI_mode_save:
				
				tft.fillScreen(TFT_BLACK);  // clean screen
				Font_set_min();
				tft.setCursor(0, 100);
				tft.setTextColor(TFT_WHITE);
				tft.setTextSize(1);
				tft.printf("\n Saving");				

				//first in last out

				if (ProtoGun_setCMD(0x1F0) &&												//Svae
					ProtoGun_setCMD(0x030 + Blaster.Beep_period) &&							//Period
					ProtoGun_setCMD(0x010 + Blaster.Beep_mode) &&							//Mode
					ProtoGun_setCMD(0x140 + (Blaster.Boop & 0x01)) &&						//Boop
					ProtoGun_setCMD(0x130 + Blaster.Blink_period) &&						//Blink_period
					ProtoGun_setCMD(0x110 + (Blaster.Face_startup_index & 0x0F)) &&			//Starup face_L
					ProtoGun_setCMD(0x120 + ((Blaster.Face_startup_index >> 4) & 0x0F)) &&	//Starup face_H
					ProtoGun_setCMD(0x100 + Blaster.Matrix_Brightness) &&					//Matrix Brightness
					ProtoGun_setCMD(0x300 + Blaster.Neo_Brightness))						//Neo_Brightness
				{
					tft.printf("\n Saving");
					BLE_mode(BLE_mode_notify);
					EEPROM_Save();
				}
				else
				{
					tft.printf("\n Offline");
					tft.printf("\n Saving");
					EEPROM_Save_offline();
				}
				
				Battery_level_init();
				show = 1;
				setting_list_index = 0;
				vTaskDelay(500 / portTICK_PERIOD_MS);
				GUI_mode(GUI_mode_main);
				break;

			case GUI_mode_signature:
				sleep_mode = mode;
				if (temp_16 == GUI_temp16_init)
				{
					temp_16 = 0;
				}

				if (show)
				{
					show = 0;
					tft.pushImage(0, 0, 135, 240, Signature_BG[temp_16]);
					#ifdef has_sig
					tft.pushImage(0, 0, 135, 240, Signature[temp_16], TFT_BLACK);
					#endif
				}

				if (Encoder_up)
				{
					Encoder_up = 0;
					if (temp_16 > 0)
					{
						temp_16--;
						show = 1;
					}
				}
				else if (Encoder_down)
				{
					Encoder_down = 0;
					if (temp_16 < sizeof(Signature_BG) / 64800 - 1)
					{
						temp_16++;
						show = 1;
					}
				}

				if (Get_But(&But_Return) || Get_But(&But_Enter) || Get_But(&But_X) || Get_But(&But_Y))
				{
					VM_mode(VM_mode_on_long);
					show = 1;
					GUI_mode(GUI_mode_main);
				}
				else
				{
					GUI_mode(GUI_mode_signature);
				}
				break;

			}
		}
	}
}

void Ctrl_task(void * parameter)
{
	uint8_t mode = 0xFF;
	uint8_t pos_old, pos_now;
	bool Deter_pin_temp = Blaster.remote;
	TickType_t tick;
	tick = xTaskGetTickCount();
	for (;;)
	{
		if (xQueueReceive(queue_Ctrl, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Ctrl_mode_boost:
				LCD_refresh_cnt.main = 1;
				LCD_refresh_cnt.info = 1;
				LCD_refresh_cnt.Batt = Batt_refresh_time;
				Fan_mode(Fan_mode_boost);
				BLE_mode(BLE_mode_boost);
				GUI_mode(GUI_mode_boost);
				ADC_mode(ADC_mode_general);
				pos_old = digitalRead(Roll_A_PIN);
				vTaskDelay(5 / portTICK_PERIOD_MS);
				Ctrl_mode(Ctrl_mode_general);
				break;

			case Ctrl_mode_off:
				vTaskDelete(NULL);
				break;

			case Ctrl_mode_general:
				#if use_butt_AB
				button_scan(&But_A);
				button_scan(&But_B);
				#endif
				button_scan(&But_Trigger);
				button_scan(&But_Enter);
				button_scan(&But_Return);
				button_scan(&But_X);
				button_scan(&But_Y);

				//Encoder======================================
				pos_now = digitalRead(Roll_A_PIN);
				if (pos_now != pos_old)
				{
					VM_mode(VM_mode_on_short);
					LCD_sleep_cnt = 0;	//clear sleep cnt
					if (pos_now != digitalRead(Roll_B_PIN))
					{
						Encoder_down = 1;
						serial_log(0, "Encoder DOWN");
					}
					else
					{
						Encoder_up = 1;
						serial_log(0, "Encoder UP");
					}
				}
				pos_old = pos_now;
				//Encoder======================================

				if (LCD_sleep_cnt < LCD_sleep_time)
				{
					LCD_sleep_cnt++;
				}
				else
				{
					GUI_mode(GUI_mode_sleep);
				}

				if (LCD_refresh_cnt.Batt < Batt_refresh_time)
				{
					LCD_refresh_cnt.Batt++;
				}
				if (LCD_refresh_cnt.Face_resume < Face_resume_time)
				{
					LCD_refresh_cnt.Face_resume++;
				}

				Blaster.remote = !digitalRead(Deter_PIN);
				if(Blaster.remote != Deter_pin_temp)
				{
					ADC_mode(ADC_mode_pre_boost);
				}
				else
				{
					Deter_pin_temp = Blaster.remote;
				}
				
				vTaskDelayUntil(&tick, 1 / portTICK_PERIOD_MS);
				Ctrl_mode(Ctrl_mode_general);
				break;
			}
		}
	}
}

void BLE_task(void * parameter)
{
	uint8_t mode = 0xFF;
	BLEDevice::init(ProtoGun_Name);
	BLEServer *pServer = BLEDevice::createServer();
	BLEService *pService = pServer->createService(SERVICE_UUID);
	BLECharacteristic *pCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_WRITE |
		BLECharacteristic::PROPERTY_NOTIFY |
		BLECharacteristic::PROPERTY_INDICATE
	);

	// Create a BLE Descriptor
	pCharacteristic->addDescriptor(new BLE2902());

	pServer->setCallbacks(new MyServerCallback);
	BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

	for (;;)
	{
		if (xQueueReceive(queue_BLE, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case BLE_mode_boost:
				pService->start();

				pAdvertising->addServiceUUID(SERVICE_UUID);
				pAdvertising->setScanResponse(true);
				pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
				pAdvertising->setMinPreferred(0x12);
				BLEDevice::startAdvertising();
				break;
			case BLE_mode_notify:
				if (BLE_connected && BLE_buffer_index > -1)
				{
					serial_log("BLE cmd", BLE_value[BLE_buffer_index]);
					pCharacteristic->setValue(BLE_value[BLE_buffer_index]);
					pCharacteristic->notify();
					BLE_buffer_index--;
					vTaskDelay(BLE_notify_send_interval / portTICK_PERIOD_MS);
					BLE_mode(BLE_mode_notify);
				}
				break;
			case BLE_mode_off:
				pService->stop();
				BLEDevice::deinit(true);
				vTaskDelete(NULL);
				break;
			}
		}
	}
}

void VM_task(void * parameter)
{
	uint8_t mode = 0xFF;
	for (;;)
	{
		if (xQueueReceive(queue_VM, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case VM_mode_off:
				digitalWrite(V_motor_PIN, 0);
				vTaskDelete(NULL);
				break;
			case VM_mode_on_short_thrice:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_short / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				vTaskDelay(V_motor_duration_short / portTICK_PERIOD_MS);
			case VM_mode_on_short_twice:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_short / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				vTaskDelay(V_motor_duration_short / portTICK_PERIOD_MS);
			case VM_mode_on_short:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_short / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				break;

			case VM_mode_on_long_thrice:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_long / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				vTaskDelay(V_motor_duration_long / portTICK_PERIOD_MS);
			case VM_mode_on_long_twice:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_long / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				vTaskDelay(V_motor_duration_long / portTICK_PERIOD_MS);
			case VM_mode_on_long:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_long / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				break;

			case VM_mode_on_extra_long_thrice:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_extra / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				vTaskDelay(V_motor_duration_extra / portTICK_PERIOD_MS);
			case VM_mode_on_extra_long_twice:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_extra / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				vTaskDelay(V_motor_duration_extra / portTICK_PERIOD_MS);
			case VM_mode_on_extra_long:
				digitalWrite(V_motor_PIN, 1);
				vTaskDelay(V_motor_duration_extra / portTICK_PERIOD_MS);
				digitalWrite(V_motor_PIN, 0);
				break;
			}
		}
	}
}