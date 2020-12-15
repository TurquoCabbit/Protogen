//Resulotion 240x135

TFT_eSPI tft = TFT_eSPI();

#define Font_set_Large()	tft.setFreeFont(&FreeMonoBold24pt7b)
#define Font_set_Mid()		tft.setFreeFont(&FreeMonoBold18pt7b)
#define Font_set_min()		tft.setFreeFont(&FreeMonoBold12pt7b)

#define LCD_sleep_time	60000

#define Batt_refresh_time	5000
#define main_refresh_time	1
#define info_refresh_time	1
#define Face_resume_time	10000

#define setting_list_top	10

const char *Setting_list[setting_list_top + 1] = {
	" Start UP",
	" Toggle",
	" Matrix\n Bright",
	" Blink",
	" BEEP",
	" BOOP",
	" Neopixel\n Bright",
	" Fan Duty",
	" ADC Cal",
	" Leave",
	" Reset"
};

const char *Leave_list[2] = {
	" Don't\n save",
	" Save"
};

const char *Beep_list[3] = {
	" Mode",
	" Period",
	" Pitch"
};

const char *Beep_mode_list[5] = {
	" Single",
	" Double",
	" Rand one",
	" Rand two",
	" Rand three"
};

const char *Boop_list[2] = {
	"  OFF",
	"  ON"
};

typedef struct _cnt
{
	uint8_t main;
	uint8_t info;
	uint16_t Batt;
	uint16_t Face_resume;
	uint16_t ADC;
};

_cnt LCD_refresh_cnt = { 0 };

uint32_t LCD_sleep_cnt = 0;

inline void LCD_init(void)
{
	#if do_serial
	Serial.println("LCD_init_START");
	#endif

	tft.begin();               // init LCD
	tft.fillScreen(TFT_BLACK);  // clean screen
	tft.setSwapBytes(true);

	#if do_serial
	Serial.println("LCD_init_DONE");
	#endif
}

void LCD_show_Battery(const uint16_t *pic)
{
	tft.pushImage(0, 0, 135, 50, pic);
}

void LCD_show_BLE(const uint16_t *pic)
{
	tft.pushImage(0, 190, 135, 50, pic);
}

void LCD_show_lit(const uint16_t *pic)
{
	tft.pushImage(0, 190, 135, 50, pic, TFT_BLACK);
}

void LCD_show_main(const uint16_t *pic)
{
	tft.pushImage(0, 50, 135, 140, pic);
}

void LCD_show_list(const char * list[], uint8_t index, uint8_t top)
{
	tft.fillScreen(TFT_BLACK);  // clean screen

	if (index > 0)
	{
		Font_set_min();
		tft.setCursor(0, 20);
		tft.setTextColor(TFT_SILVER);
		tft.setTextSize(1);
		tft.printf(list[index - 1]);
	}
	if (index < top)
	{
		Font_set_min();
		tft.setCursor(0, 200);
		tft.setTextColor(TFT_SILVER);
		tft.setTextSize(1);
		tft.printf(list[index + 1]);
	}

	Font_set_min();
	tft.setCursor(0, 110);
	tft.setTextColor(TFT_YELLOW);
	tft.setTextSize(1);
	tft.printf(list[index]);
}

void LCD_show_title(const char * list[], uint8_t index)
{
	Font_set_min();
	tft.setCursor(0, 20);
	tft.setTextColor(TFT_YELLOW);
	tft.setTextSize(1);
	tft.printf(list[index]);
}