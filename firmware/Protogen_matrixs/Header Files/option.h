//Face
#define Matrix_brightness_Max	15  // 0 to 15
#define Matrix_brightness_init	8  // 0 to 15

#define eye_blink_cycle		6  //eye_blink_cycle * 2 S
#define eye_blink_center	4
#define eye_blink_time		300  //in ms

#define rick_roll_delaytime	50  //in ms

#define rand_dot_Probability	100	//in 1000
#define rand_dot_delaytime		50 //in ms

//Beep
#define Beep_mode_init		1	//double beep
#define Beep_cycle			10   //how many Blink times
#define Beep_pitch_init		0x53  //Octave pitch
#define Beep_interval       50    //interval between each beep in ms
#define beep_time           100   //in ms

//Protosence
#define Protosence_on			1
#define Protosence_scan_cycle	10 //in 10ms
#define Protosence_hold			100	//in 10ms

//neo pixel
#define neo_ring_brightness 60   //0 to 255

//BLE Remote
#define BLE_notify_catch_interval	5		//in ms
#define BLE_reconnect_time		6000	//in 10ms

//Sreial
#define do_serial 1
#if do_serial
    #define serial_log(header, data) {Serial.print(header);Serial.print(" -> ");Serial.println(data);}
#else
    #define serial_log(header, data) {}
#endif