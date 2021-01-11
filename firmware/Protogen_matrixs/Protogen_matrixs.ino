#include <esp_task_wdt.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <EEPROM.h>
#include <WS2812FX.h>
#include <WS2812Driver.h>
#include <Adafruit_LEDBackpack.h>

#include "./Header Files/option.h"
#include "./Header Files/parameter.h"
#include "./Header Files/GPIO.h"
#include "./Header Files/matrix.h"
#include "./Header Files/Face.h"
#include "./Header Files/neopixel.h"
#include "./Header Files/Bz.h"
#include "./Header Files/music.h"
#include "./Header Files/BLE.h"

void setup()
{
	Serial.begin(115200);
	#if do_serial
	Serial.println("start up");
	#endif
	gpio_init();

	parameter_init();

	queue_init();

	EEPROM.begin(EEPROM_SIZE);

	/*
	Core_0	Eye, Nose, Mouth, BZ, Fan, Ctrl, BLE
	core_1	Neopixel, Matrix
	*/

	xTaskCreatePinnedToCore(
		Neopixel_task, /* Function to implement the task */
		"Neopixel_task", /* Name of the task */
		Neopixel_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		Neopixel_task_priority,  /* Priority of the task */
		&Neopixel_task_handle,  /* Task handle. */
		core_1);
	
	xTaskCreatePinnedToCore(
		Matrix_task, /* Function to implement the task */
		"Matrix_task", /* Name of the task */
		Matrix_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		Matrix_task_priority,  /* Priority of the task */
		&Matrix_task_handle,  /* Task handle. */
		core_1);

	xTaskCreatePinnedToCore(
		Eye_task, /* Function to implement the task */
		"Eye_task", /* Name of the task */
		Eye_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		Eye_task_priority,  /* Priority of the task */
		&Eye_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		Nose_task, /* Function to implement the task */
		"Nose_task", /* Name of the task */
		Nose_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		Nose_task_priority,  /* Priority of the task */
		&Nose_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		Mouth_task, /* Function to implement the task */
		"Mouth_task", /* Name of the task */
		Mouth_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		Mouth_task_priority,  /* Priority of the task */
		&Mouth_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		BZ_task, /* Function to implement the task */
		"BZ_task", /* Name of the task */
		BZ_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		BZ_task_priority,  /* Priority of the task */
		&BZ_task_handle,  /* Task handle. */
		core_0);

	xTaskCreatePinnedToCore(
		Fan_task, /* Function to implement the task */
		"Fan_task", /* Name of the task */
		Fan_task_stack,  /* Stack size in words */
		NULL,  /* Task input parameter */
		Fan_task_priority,  /* Priority of the task */
		&Fan_task_handle,  /* Task handle. */
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

	#if do_serial
	Serial.println("create task done");
	#endif
	
	if (EEPROM.read(EEPROM_Addr_Saved) == EEPROM_saved)
	{
		EEPROM_Load();
		#if do_serial
		Serial.println("Load EEPROM done");
		#endif
	}

	BLE_mode(BLE_mode_boost);
}
    
void loop()
{
	//core 1
	vTaskDelete(NULL); //Kill Loop()
}

void Neopixel_task(void * parameter)
{
	uint8_t mode = 0xFF;
	WS2812FX *neo_ring;
	neo_ring = new WS2812FX(neo_ring_single + 5, LED_PIN, NEO_GRBW + NEO_KHZ800);
	for (;;)
	{
		if (xQueueReceive(queue_Neopixel, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Neopixel_mode_boost:
				ws2812driver.init(neo_ring);
				neo_ring->setCustomShow([]() {ws2812driver.customShow(); });
				#if do_serial
				Serial.println("Neopixel_mode_boost");
				#endif
				vTaskDelay(1 / portTICK_PERIOD_MS);
				Neopixel_mode(Neopixel_mode_set);
				break;

			case Neopixel_mode_general:
				neo_ring->service();
				vTaskDelay(1 / portTICK_PERIOD_MS);
				Neopixel_mode(Neopixel_mode_general);
				break;
				
			case Neopixel_mode_Brightness:
				neo_ring->pause();
				neo_ring->setBrightness(Protogen.Neo_Brightness);
				neo_ring->resume();
				Neopixel_mode(Neopixel_mode_general);
				break;
				
			case Neopixel_mode_set:
				neo_ring->stop();
				neo_ring->setBrightness(Protogen.Neo_Brightness);
				neo_ring->setSpeed((Face_index.neo[1] << 8) + Face_index.neo[2]);
				neo_ring->setColor(Face_index.neo[3], Face_index.neo[4], Face_index.neo[5], Face_index.neo[6]);
				if (Face_index.neo[0] < FX_MODE_MINE)
				{
					neo_ring->setMode(Face_index.neo[0]);
					neo_ring->start();
					Neopixel_mode(Neopixel_mode_general);
				}
				else
				{
					Neopixel_mode(Face_index.neo[0]);
				}
				break;
			}
		}
	}
}

void Matrix_task(void * parameter)
{
	uint8_t mode = 0xFF;
	for (;;)
	{
		if(xQueueReceive(queue_Matrix, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Matrix_mode_boost:
				matrix_init();
				matrix_brightness(Protogen.Matrix_Brightness);
				clear_face_buffer(index_eye);
				clear_face_buffer(index_nose);
				clear_face_buffer(index_mouth);
				#if do_serial
				Serial.println("Matrix_mode_boost");
				#endif
				vTaskDelay(1 / portTICK_PERIOD_MS);
				Matrix_mode(Matrix_mode_scan + *Face_index.animate_mode);
				break;

			case Matrix_mode_brightness:
				matrix_brightness(Protogen.Matrix_Brightness);
				Matrix_refresh();
				vTaskDelay(1 / portTICK_PERIOD_MS);
				Matrix_mode(Matrix_mode_scan);
				break;

			case Matrix_mode_scan:
				if (Protogen.matrix_refresh)
				{
					matrix_show_face(Face_index, Protogen.show_eye, Protogen.show_nose, Protogen.show_mouth);
					Protogen.matrix_refresh = 0;
				}
				vTaskDelay(1 / portTICK_PERIOD_MS);
				Matrix_mode(Matrix_mode_scan);
				break;

			case Matrix_mode_rickroll:
				Mouth_mode(Mouth_mode_rickroll);
				vTaskDelay(1 / portTICK_PERIOD_MS);
				Matrix_mode(Matrix_mode_scan);
				break;

			case Matrix_mode_randdot:
				srand(xTaskGetTickCount());
				Face_index.eye = Face_buffer.eye;
				Face_index.nose = Face_buffer.nose;
				Face_index.mouth = Face_buffer.mouth;
				Eye_mode(Eye_mode_randdot);
				Nose_mode(Nose_mode_randdot);
				Mouth_mode(Mouth_mode_randdot);
				vTaskDelay(1 / portTICK_PERIOD_MS);
				Matrix_mode(Matrix_mode_scan);
				break;
				
			case Matrix_mode_vibrate:
				srand(xTaskGetTickCount());
				Face_index.eye = Face_buffer.eye;
				Face_index.nose = Face_buffer.nose;
				Face_index.mouth = Face_buffer.mouth;
				Eye_mode(Eye_mode_vibrate);
				Nose_mode(Nose_mode_vibrate);
				Mouth_mode(Mouth_mode_vibrate);
				vTaskDelay(1 / portTICK_PERIOD_MS);
				Matrix_mode(Matrix_mode_scan);
				break;
			}
		}
	}
}

void Eye_task(void * parameter)
{
	uint8_t mode = 0xFF;
	uint8_t i;
	uint8_t blink_cnt = 0;
	uint16_t blink_temp = 0;
	for (;;)
	{
		if (xQueueReceive(queue_Eye, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Eye_mode_general:
				Face_index.eye = Face_current.eye;
				Matrix_refresh();
				clear_face_buffer(index_eye);
				break;

			case Eye_mode_boop:
				Face_index.eye = eye[2];	// <
				Matrix_refresh();
				break;

			case Eye_mode_blink:
				blink_cnt = matrix_eye_blink(Face_index.eye, *Face_index.center);
				
				for (i = 0; i < blink_cnt; i++)
				{
					Face_index.eye = eye_blink[i];
					Matrix_refresh();
					vTaskDelay(eye_blink_time / (blink_cnt * 2) / portTICK_PERIOD_MS);
				}

				Face_index.eye = eye[0];
				Matrix_refresh();

				vTaskDelay(eye_blink_time / (blink_cnt) / portTICK_PERIOD_MS);

				for (i = 1; i <= blink_cnt; i++)
				{
					Face_index.eye = eye_blink[blink_cnt - i];
					Matrix_refresh();
					vTaskDelay(eye_blink_time / (blink_cnt * 2) / portTICK_PERIOD_MS);
				}
				Face_index.eye = Face_current.eye;
				Matrix_refresh();
				break;

			case Eye_mode_randdot:
				if (blink_temp != Protogen.Blink_period)
				{
					blink_temp = Protogen.Blink_period;
				}
				if (Protogen.animate_on)
				{
					matrix_rand_dot(Face_current.eye, index_eye);
					Matrix_refresh();
					vTaskDelay(rand_dot_delaytime / portTICK_PERIOD_MS);
					Eye_mode(Eye_mode_randdot);
				}
				else
				{
					Protogen.Blink_period = blink_temp;
					cnt.Blink = 0;
					Eye_mode(Eye_mode_general);
				}
				break;
				
			case Eye_mode_vibrate:
				if (blink_temp != Protogen.Blink_period)
				{
					blink_temp = Protogen.Blink_period;
				}

				if (Protogen.animate_on)
				{
					matrix_vibrate(Face_current.eye, index_eye);
					Matrix_refresh();
					vTaskDelay(rand_dot_delaytime / portTICK_PERIOD_MS);
					Eye_mode(Eye_mode_randdot);
				}
				else
				{
					Protogen.Blink_period = blink_temp;
					cnt.Blink = 0;
					Eye_mode(Eye_mode_general);
				}

				break;
			}
		}
	}
}

void Nose_task(void * parameter)
{
	uint8_t mode = 0xFF;
	for (;;)
	{
		if (xQueueReceive(queue_Nose, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Nose_mode_general:
				Face_index.nose = Face_current.nose;
				Matrix_refresh();
				clear_face_buffer(index_nose);
				break;

			case Nose_mode_boop:
				Face_index.nose = nose[1];	// <
				Matrix_refresh();
				break;

			case Nose_mode_randdot:
				if (Protogen.animate_on)
				{
					matrix_rand_dot(Face_current.nose, index_nose);
					Matrix_refresh();
					vTaskDelay(rand_dot_delaytime / portTICK_PERIOD_MS);
					Nose_mode(Nose_mode_randdot);
				}
				else
				{
					Nose_mode(Nose_mode_general);
				}
				break;

			case Nose_mode_vibrate:
				if (Protogen.animate_on)
				{
					matrix_vibrate(Face_current.nose, index_nose);
					Matrix_refresh();
					vTaskDelay(rand_dot_delaytime / portTICK_PERIOD_MS);
					Nose_mode(Nose_mode_randdot);
				}
				else
				{
					Nose_mode(Nose_mode_general);
				}
				break;
			}
		}
	}
}

void Mouth_task(void * parameter)
{
	uint8_t mode = 0xFF;
	String rick = "We're no strangers to love You know the rules and so do I A full commitment's what I'm thinking of You wouldn't get this from any other guy I just wanna tell you how I'm feeling Gotta make you understand Never gonna give you up Never gonna let you down Never gonna run around and desert you Never gonna make you cry Never gonna say goodbye Never gonna tell a lie and hurt you We've known each other for so long Your heart's been aching but you're too shy to say it Inside we both know what's been going on We know the game and we're gonna play it And if you ask me how I'm feeling Don't tell me you're too blind to see Never gonna give you up Never gonna let you down Never gonna run around and desert you Never gonna make you cry Never gonna say goodbye Never gonna tell a lie and hurt you Never gonna give you up Never gonna let you down Never gonna run around and desert you Never gonna make you cry Never gonna say goodbye Never gonna tell a lie and hurt you Never gonna give, never gonna give (Give you up) (Ooh) Never gonna give, never gonna give (Give you up) We've known each other for so long Your heart's been aching but you're too shy to say it Inside we both know what's been going on We know the game and we're gonna play it I just wanna tell you how I'm feeling Gotta make you understand Never gonna give you up Never gonna let you down Never gonna run around and desert you Never gonna make you cry Never gonna say goodbye Never gonna tell a lie and hurt you Never gonna give you up Never gonna let you down Never gonna run around and desert you Never gonna make you cry Never gonna say goodbye Never gonna tell a lie and hurt you Never gonna give you up Never gonna let you down Never gonna run around and desert you";
	uint32_t rick_roll_shift = rick.length() * 6;
	for (;;)
	{
		if (xQueueReceive(queue_Mouth, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Mouth_mode_general:
				Face_index.mouth = Face_current.mouth;
				Matrix_refresh();
				clear_face_buffer(index_mouth);
				break;

			case Mouth_mode_rickroll:
				if (Protogen.show_mouth)
				{
					Protogen.show_mouth = 0;
				}

				if (Protogen.animate_on)
				{
					matrix_mouth_L_F.setTextSize(1);
					matrix_mouth_L_F.setTextWrap(false);
					matrix_mouth_L_F.setRotation(1);

					matrix_mouth_L_B.setTextSize(1);
					matrix_mouth_L_B.setTextWrap(false);
					matrix_mouth_L_B.setRotation(3);

					matrix_mouth_R_F.setTextSize(1);
					matrix_mouth_R_F.setTextWrap(false);
					matrix_mouth_R_F.setRotation(3);

					matrix_mouth_R_B.setTextSize(1);
					matrix_mouth_R_B.setTextWrap(false);
					matrix_mouth_R_B.setRotation(1);

					matrix_mouth_L_F.clear();
					matrix_mouth_L_B.clear();
					matrix_mouth_R_F.clear();
					matrix_mouth_R_B.clear();

					matrix_mouth_L_F.setCursor(rick_roll_shift - rick.length() * 6 - 32, 0);
					matrix_mouth_L_B.setCursor(rick_roll_shift - rick.length() * 6 - 48, 0);
					matrix_mouth_R_F.setCursor(rick_roll_shift - rick.length() * 6 - 16, 0);
					matrix_mouth_R_B.setCursor(rick_roll_shift - rick.length() * 6, 0);

					matrix_mouth_L_F.print(rick);
					matrix_mouth_L_B.print(rick);
					matrix_mouth_R_F.print(rick);
					matrix_mouth_R_B.print(rick);

					matrix_mouth_L_F.writeDisplay();
					matrix_mouth_L_B.writeDisplay();
					matrix_mouth_R_F.writeDisplay();
					matrix_mouth_R_B.writeDisplay();

					rick_roll_shift--;
					if (rick_roll_shift == 0)
					{
						rick_roll_shift = rick.length() * 6;
					}
					vTaskDelay(rick_roll_delaytime / portTICK_PERIOD_MS);
					Mouth_mode(Mouth_mode_rickroll);
				}
				else
				{
					Protogen.show_mouth = 1;
					Matrix_refresh();
				}
				break;

			case Mouth_mode_randdot:
				if (Protogen.animate_on)
				{
					matrix_rand_dot(Face_current.mouth, index_mouth);
					Matrix_refresh();
					vTaskDelay(rand_dot_delaytime / portTICK_PERIOD_MS);
					Mouth_mode(Mouth_mode_randdot);
				}
				else
				{
					Mouth_mode(Mouth_mode_general);
				}
				break;

			case Mouth_mode_vibrate:
				if (Protogen.animate_on)
				{
					matrix_vibrate(Face_current.mouth, index_mouth);
					Matrix_refresh();
					vTaskDelay(rand_dot_delaytime / portTICK_PERIOD_MS);
					Mouth_mode(Mouth_mode_randdot);
				}
				else
				{
					Mouth_mode(Mouth_mode_general);
				}
				break;
			}
		}
	}
}

void Fan_task(void * parameter)
{
	uint8_t mode = 0xFF;
	for (;;)
	{
		if (xQueueReceive(queue_Fan, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Fan_mode_off:
				Fan_off();
				break;
			case Fan_mode_on:
				Fan_on();
				break;
			}
		}
	}
}

void BZ_task(void * parameter)
{
	uint8_t mode = 0xFF;
	uint8_t rand_pitch;
	uint16_t i, j;
	_music * play_now;
	for(;;)
	{    
		if(xQueueReceive(queue_BZ, &mode, portMAX_DELAY) == pdTRUE)
		{ 
			switch (mode)
			{
			case BZ_mode_boost:
				ledcAttachPin(BZ_PIN, BZ_channel);
				#if do_serial
				Serial.println("BZ_mode_boost");
				#endif
				break;

			case BZ_mode_OFF:
				play_now = (_music *)(ptr_dummy);
				ledcWrite(BZ_channel, 0);
				Protogen.music_playing = 0;
				break;
				
			case BZ_mode_beep:
				BZ_music(BZ_channel, Protogen.Beep_pitch & 0x0F, Protogen.Beep_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				break;

			case BZ_mode_dobule_beep:
				BZ_music(BZ_channel, Protogen.Beep_pitch & 0x0F, Protogen.Beep_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, Protogen.Beep_pitch & 0x0F, Protogen.Beep_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				break;

			case BZ_mode_rand_one:
				srand(xTaskGetTickCount());
				rand_pitch = rand_beep();
				BZ_music(BZ_channel, rand_pitch & 0x0F, rand_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				break;

			case BZ_mode_rand_two:
				srand(xTaskGetTickCount());
				rand_pitch = rand_beep();
				BZ_music(BZ_channel, rand_pitch & 0x0F, rand_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				rand_pitch = rand_beep();
				BZ_music(BZ_channel, rand_pitch & 0x0F, rand_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				break;

			case BZ_mode_rand_three:
				srand(xTaskGetTickCount());
				rand_pitch = rand_beep();
				BZ_music(BZ_channel, rand_pitch & 0x0F, rand_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				rand_pitch = rand_beep();
				BZ_music(BZ_channel, rand_pitch & 0x0F, rand_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				rand_pitch = rand_beep();
				BZ_music(BZ_channel, rand_pitch & 0x0F, rand_pitch >> 4);
				vTaskDelay(Beep_interval / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				break;
				
			case BZ_mode_music:
				/*
				for (i = 1; i < megalovania_length; i++)
				{
					if (!megalovania[i - 1][3])  //if not last note joined note
					{
						BZ_music(BZ_channel, megalovania[i][0], megalovania[i][1]);
					}
					vTaskDelay(beep_time / portTICK_PERIOD_MS);
					if (!megalovania[i][3])  //if not joined note
					{
						ledcWrite(BZ_channel, 0);
					}
					j = 0;
					while (j < 8)
					{
						if (megalovania[i][2] >> j == 1)
						{
							break;
						}
						j++;
					}
					vTaskDelay(megalovania_note_time[j] / portTICK_PERIOD_MS);
				}
				*/
				if(*(Face_current.piece))
				{
					play_now = music_ptr_rack[*(Face_current.piece) - 1];
					Protogen.music_playing = 1;
					for(i = 1; i < play_now->length; i++)
					{
						if (!play_now->sheet[i - 1].joined)  //if last note not joined note
						{
							BZ_music(BZ_channel, play_now->sheet[i].pitch, play_now->sheet[i].octave);
						}
						vTaskDelay(play_now->ring_time / portTICK_PERIOD_MS);
						if (!play_now->sheet[i].joined)  //if not joined note
						{
							ledcWrite(BZ_channel, 0);
						}
						j = 0;
						while (j < 8)
						{
							if (play_now->sheet[i].note >> j == 0x01)
							{
								break;
							}
							j++;
						}
						vTaskDelay((play_now->note[j] - play_now->ring_time) / portTICK_PERIOD_MS);
					}
					Protogen.music_playing = 0;
				}
				BZ_mode(BZ_mode_OFF);
				break;

			case BZ_mode_save:
				BZ_music(BZ_channel, 1);
				vTaskDelay(500 / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				vTaskDelay(50 / portTICK_PERIOD_MS);
				break;

			case BZ_mode_reset:
				BZ_music(BZ_channel, 7, 3);
				vTaskDelay(300 / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				vTaskDelay(100 / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 1, 4);
				vTaskDelay(300 / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				vTaskDelay(50 / portTICK_PERIOD_MS);
				break;

			case BZ_mode_BLE_set:
				BZ_music(BZ_channel, 1, 5);
				vTaskDelay(200 / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 5, 5);
				vTaskDelay(200 / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				vTaskDelay(50 / portTICK_PERIOD_MS);
				break;

			case BZ_mode_pitch_set:
				BZ_music(BZ_channel, Protogen.Beep_pitch & 0x0F, Protogen.Beep_pitch >> 4);
				vTaskDelay(25 / portTICK_PERIOD_MS);
				BZ_music(BZ_channel, 0);
				break;
			}
		}
	}
}

void Ctrl_task(void * parameter)
{
	uint8_t mode = 0xFF;
	bool IR_send_toggle = 0;
	uint16_t booped = 0;
	TickType_t tick;
	tick = xTaskGetTickCount();
	for (;;)
	{
		if (xQueueReceive(queue_Ctrl, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case Ctrl_mode_boost:
				matrix_face_set(face_ptr_rack[Protogen.Startup_Face]);
				Neopixel_mode(Neopixel_mode_boost);
				Matrix_mode(Matrix_mode_boost);
				BZ_mode(BZ_mode_boost);
				Fan_mode(Fan_mode_on);
				#if do_serial
				Serial.println("Ctrl_mode_boost");
				#endif
				vTaskDelay(5 / portTICK_PERIOD_MS);
				Ctrl_mode(Ctrl_mode_general);
				break;

			case Ctrl_mode_general:
				if (Protogen.Blink_period)
				{
					if (cnt.Blink >= Protogen.Blink_period * 200)
					{
						cnt.Blink = 0;
						Eye_mode(Eye_mode_blink);

						if (Protogen.Beep_period)
						{
							if (cnt.Beep >= Protogen.Beep_period)
							{
								cnt.Beep = 0;
								if(!Protogen.music_playing)
								{
									BZ_mode(BZ_mode_beep + Protogen.Beep_mode);
								}
							}
							else
							{
								cnt.Beep++;
							}
						}
					}
					else
					{
						cnt.Blink++;
					}
				}

				if(Protogen.Protosence_flag)
				{
					if (cnt.Protosence >= Protosence_scan_cycle)
					{
						cnt.Protosence = 0;
						IR_send_toggle = ~IR_send_toggle;
						IR_send_pwm(IR_send_toggle);
					}
					else
					{
						cnt.Protosence++;
					}

					if (!digitalRead(IR_Rx_PIN) && !booped)	//IR Boop receive
					{
						booped++;
						Eye_mode(Eye_mode_boop);
						Nose_mode(Nose_mode_boop);
					}
					else if (booped > Protosence_hold)
					{
						booped = 0;
						Eye_mode(Eye_mode_general);
						Nose_mode(Nose_mode_general);
					}
					else if (booped > 0)
					{
						booped++;
					}
				}

				if (Protogen.animate_on && *Face_current.animate_time)
				{
					if (cnt.animate >= *Face_current.animate_time * 100)
					{
						cnt.animate = 0;
						Protogen.animate_on = 0;
					}
					else
					{
						cnt.animate++;
					}
				}

				if (!connected)
				{
					if (cnt.BLE_reconnect == BLE_reconnect_time)
					{
						cnt.BLE_reconnect = 0;
						BLE_mode(BLE_mode_reconnect);
					}
					else
					{
						cnt.BLE_reconnect++;
					}
				}
				vTaskDelayUntil(&tick, 10 / portTICK_PERIOD_MS);
				Ctrl_mode(Ctrl_mode_general);
				break;
			}
		}
	}
}

void BLE_task(void * parameter)
{
	uint8_t mode = 0xFF;
	for (;;)
	{
		if (xQueueReceive(queue_BLE, &mode, portMAX_DELAY) == pdTRUE)
		{
			switch (mode)
			{
			case BLE_mode_boost:
				BLE_init();
				#if do_serial
				Serial.println("BLE_mode_boost");
				#endif
				Ctrl_mode(Ctrl_mode_boost);
				break;

			case BLE_mode_reconnect:
				if (!connected)
				{
					BLE_init();
				}
				break;

			case BLE_mode_notify:
				if (BLE_buffer_index > -1 && (BLE_cmd[BLE_buffer_index] & 0xF000) == (ProtoGen_ID << 12))
				{
					#if do_serial
					Serial.print("BLE cmd : ");
					Serial.println(BLE_cmd[BLE_buffer_index], HEX);
					#endif
					switch (BLE_cmd[BLE_buffer_index] & 0xF00)
					{
					case CMD_Bz:	//BZ
						switch (BLE_cmd[BLE_buffer_index] & 0xF0)
						{
						case 0x00:	//Beep and music
							BZ_mode(BZ_mode_beep + Protogen.Beep_mode);
							break;
						case 0x10:	//mode
							Protogen.Beep_mode = BLE_cmd[BLE_buffer_index] & 0x0F;
							break;
						case 0x20:	//pitch
							if (BLE_cmd[BLE_buffer_index] == 0x020)	//change pitch up
							{
								if (Protogen.Beep_pitch < 0x8C)
								{
									if ((Protogen.Beep_pitch & 0x0F) == 0x0C)
									{
										Protogen.Beep_pitch += 0x05;
									}
									else
									{
										Protogen.Beep_pitch++;
									}
								}
							}
							else if (BLE_cmd[BLE_buffer_index] == 0x021)	//change pitch down
							{
								if (Protogen.Beep_pitch > 0x01)
								{
									if ((Protogen.Beep_pitch) & 0x0F == 0x01)
									{
										Protogen.Beep_pitch -= 0x05;
									}
									else
									{
										Protogen.Beep_pitch--;
									}
								}
							}
							else if (BLE_cmd[BLE_buffer_index] == 0x022)	//Reset pitch to default
							{
								Protogen.Beep_pitch = Beep_pitch_init;
							}
							#if do_serial
							Serial.print("Beep note : ");
							Serial.print(Protogen.Beep_pitch & 0x0F);
							Serial.print(", Octave : ");
							Serial.println((Protogen.Beep_pitch >> 4) & 0xFF);
							#endif
							BZ_mode(BZ_mode_pitch_set);
							break;
						case 0x30:	//period
							Protogen.Beep_period = BLE_cmd[BLE_buffer_index] & 0x0F;
							cnt.Beep = 0;
							break;
						}
						break;

					case CMD_Setting:	//Setting
						switch (BLE_cmd[BLE_buffer_index] & 0xF0)
						{
						case 0x00:	//Matrix Brightness
							Protogen.Matrix_Brightness = BLE_cmd[BLE_buffer_index] & 0x0F;
							Matrix_mode(Matrix_mode_brightness);
							break;
						case 0x10:	//Startup face L
							Protogen.Startup_Face &= 0xF0;
							Protogen.Startup_Face += (BLE_cmd[BLE_buffer_index] & 0x0F);
							break;
						case 0x20:	//Startup face H
							Protogen.Startup_Face &= 0x0F;
							Protogen.Startup_Face += (BLE_cmd[BLE_buffer_index] << 4) & 0xF0;
							break;
						case 0x30:	//Blink period
							Protogen.Blink_period = BLE_cmd[BLE_buffer_index] & 0x0F;
							cnt.Blink = 0;
							break;
						case 0x40:	//Boop
							Protogen.Protosence_flag = BLE_cmd[BLE_buffer_index] & 0x0F;
							cnt.Protosence = 0;
							break;
						case 0xA0:	//play animate
							if (*Face_current.animate_mode)
							{
								if (!Protogen.animate_on)
								{
									Protogen.animate_on = 1;
									cnt.animate = 0;
									Matrix_mode(Matrix_mode_scan + *Face_current.animate_mode);
								}
								else
								{
									Protogen.animate_on = 0;
									cnt.animate = 0;
									Face_index.eye = Face_current.eye;
									Face_index.nose = Face_current.nose;
									Face_index.mouth = Face_current.mouth;
									Protogen.show_eye = 1;
									Protogen.show_nose = 1;
									Protogen.show_mouth = 1;
									clear_face_buffer(index_eye);
									clear_face_buffer(index_nose);
									clear_face_buffer(index_mouth);
									Matrix_refresh();
									Matrix_mode(Matrix_mode_scan);
								}

							}
							break;
						case 0xF0:	//save or reset
							if (BLE_cmd[BLE_buffer_index] == 0x1FF)	//Reset
							{
								BZ_mode(BZ_mode_reset);
								System_Reset();
								matrix_face_set(face_ptr_rack[Protogen.Startup_Face]);
								Matrix_mode(Matrix_mode_scan + *Face_current.animate_mode);
							}
							else if (BLE_cmd[BLE_buffer_index] == 0x1F0)	//Save
							{
								BZ_mode(BZ_mode_save);
								EEPROM_Save();
								cnt.Blink = 0;
								cnt.animate = 0;
								cnt.Beep = 0;
								cnt.Protosence = 0;
							}
							break;
						}
						break;

					case CMD_Face:	//Face
						matrix_face_set(face_ptr_rack[BLE_cmd[BLE_buffer_index] & 0xFF]);
						BZ_mode(BZ_mode_BLE_set);
						Matrix_mode(Matrix_mode_scan + *Face_current.animate_mode);
						Neopixel_mode(Neopixel_mode_set);
						break;

					case CMD_Neo_pixel:
						Protogen.Neo_Brightness = BLE_cmd[BLE_buffer_index] & 0xFF;
						Neopixel_mode(Neopixel_mode_Brightness);
						break;
					}
					BLE_buffer_index--;
				}
				break;
			}
		}
	}
}