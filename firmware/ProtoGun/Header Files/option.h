#define Fan_duty_init	100
#define Fan_on_Max_time	30000	//in ms

#define V_motor_duration_long	60
#define V_motor_duration_short	30
#define V_motor_duration_extra	500

#define Matrix_brightness_init	8
#define Neo_brightness_init		60

#define Blink_period_init	6	// Blink_period_init * 2 S
#define	Boop_init			1	//on
#define	Beep_period_init	10	//every 10 blink
#define	Beep_mode_init		1	//dobule beep

#define BLE_notify_send_interval	10	//in ms

#define ADC_gain_init	301000

#define use_butt_AB	0

#define BLE_offline_force_sent 0

#define do_serial	0
#if do_serial
    #define serial_log(header, data) {Serial.print(header);Serial.print(" -> ");Serial.println(data);}
#else
    #define serial_log(header, data) {}
#endif