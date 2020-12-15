//Pin define
#define FAN_PIN				02   //Fan
#define Roll_A_PIN			12
#define Roll_B_PIN			13
#define But_Enter_PIN		33
#define But_Return_PIN		25
#define But_X_PIN			26
#define But_Y_PIN			27
#define But_Trigger_PIN		17
#define But_A_PIN			00
#define But_B_PIN			35
#define Deter_PIN			37
#define LED_PIN				21
#define ADC_Battery_PIN		32  //ADC4
#define V_motor_PIN			15
#define reserived_PIN_2		22
#define reserived_PIN_3		36	//input only
#define reserived_PIN_1		38	//input only

#define Fan_channel		0

#define Fan_freq		500
#define Fan_on_Max_time	60000	//in ms
#define Fan_scan_cycle_time	50	//in ms

#define LED_channel		2

#define LED_freq		1000

#define roll_up			0x01
#define roll_down		0x0F

#define debouncing_times	5  //in ms
#define But_press			1
#define But_release			2
#define But_CD				3

#define clear_encoder_flag()	{Encoder_down = 0; Encoder_up = 0;}

typedef struct _Button
{
	uint8_t flag;
	uint32_t cnt;
	uint8_t But;
};

_Button But_Trigger, But_Enter, But_Return, But_X, But_Y;

#if use_butt_AB
_Button But_A, But_B;
#endif

bool Encoder_up = 0;
bool Encoder_down = 0;

void Button_stuct_init(_Button *pButton, uint8_t But)
{
	pButton->flag = 0;
	pButton->cnt = 0;
	pButton->But = But;
}

inline void gpio_init(void)
{
	pinMode(FAN_PIN, OUTPUT);
	digitalWrite(FAN_PIN, 0);
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, 0);
	pinMode(V_motor_PIN, OUTPUT);
	digitalWrite(V_motor_PIN, 0);

	pinMode(Deter_PIN, INPUT);

	pinMode(Roll_A_PIN, INPUT_PULLUP);
	pinMode(Roll_B_PIN, INPUT_PULLUP);

	pinMode(But_Enter_PIN, INPUT_PULLUP);
	pinMode(But_Return_PIN, INPUT_PULLUP);
	pinMode(But_X_PIN, INPUT_PULLUP);
	pinMode(But_Y_PIN, INPUT_PULLUP);
	pinMode(But_Trigger_PIN, INPUT_PULLUP);

	adc_gpio_init(ADC_UNIT_1, ADC_CHANNEL_4);

	ledcSetup(Fan_channel, Fan_freq, 10);
	ledcAttachPin(FAN_PIN, Fan_channel);
	ledcWrite(Fan_channel, 511);

	ledcSetup(LED_channel, LED_freq, 10);
	ledcAttachPin(LED_PIN, LED_channel);
	ledcWrite(LED_channel, 0);

	Button_stuct_init(&But_Trigger, But_Trigger_PIN);
	Button_stuct_init(&But_Enter, But_Enter_PIN);
	Button_stuct_init(&But_Return, But_Return_PIN);
	Button_stuct_init(&But_X, But_X_PIN);
	Button_stuct_init(&But_Y, But_Y_PIN);

	#if use_butt_AB
	pinMode(But_A_PIN, INPUT_PULLUP);
	pinMode(But_B_PIN, INPUT_PULLUP);
	Button_stuct_init(&But_A, But_A_PIN);
	Button_stuct_init(&But_B, But_B_PIN);
	#endif
}

void Set_pwm(uint8_t duty, uint8_t channel)
{
	ledcWrite(channel, duty * 1023 / 100);
}

void button_scan(_Button *pButton)
{
	if (!digitalRead(pButton->But))  //first press
	{
		pButton->cnt++;
		if (pButton->cnt > debouncing_times && !pButton->flag)
		{
			pButton->flag = But_press;
		}
	}
	else if (pButton->flag == But_CD)
	{
		pButton->cnt = 0;
		pButton->flag = 0;
	}
	else if (pButton->cnt > debouncing_times)
	{
		pButton->cnt = 0;
		pButton->flag = But_release;
	}
}

bool Get_But(_Button *pButton, uint32_t hold_time = 0)
{
	if (hold_time)
	{
		if (pButton->cnt > hold_time && pButton->flag == But_press)
		{
			pButton->flag = But_CD;
			return 1;
		}
		else if (pButton->flag == But_release)
		{
			pButton->flag = But_CD;
		}
	}
	else if (pButton->flag == But_release)
	{
		pButton->flag = 0;
		return 1;
	}
	return 0;
}

bool Get_But_press(_Button *pButton)
{
	if (pButton->flag == But_press)
	{
		return 1;
	}
	else if(pButton->flag == But_release)
	{
		pButton->flag = 0;
	}
	return 0;
}