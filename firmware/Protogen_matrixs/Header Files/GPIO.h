//Pin define
#define LED_PIN			15   //Neopixel
#define FAN_PIN			16   //Fan
#define BZ_PIN			18   //bz
#define IR_Rx_PIN		5
#define IR_Tx_PIN		17

#define BZ_channel	0
#define IR_channel	2
#define IR_freq		38000

#define Fan_on()   digitalWrite(FAN_PIN, 1)
#define Fan_off()  digitalWrite(FAN_PIN, 0)


inline void gpio_init(void)
{
	pinMode(FAN_PIN , OUTPUT);
	digitalWrite(FAN_PIN, 0);

	pinMode(BZ_PIN, OUTPUT);
	digitalWrite(BZ_PIN, 0);

	pinMode(IR_Rx_PIN, INPUT_PULLUP);
	
	pinMode(IR_Tx_PIN, OUTPUT);
	ledcAttachPin(IR_Tx_PIN, IR_channel);
	ledcSetup(IR_channel, IR_freq, 8);
	ledcWrite(IR_channel, 0);
}

inline void IR_send_pwm(bool on)
{
	if (on)
	{
		ledcWrite(IR_channel, 30);
	}
	else
	{
		ledcWrite(IR_channel, 0);
	}
}