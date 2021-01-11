#define core_0 0
#define core_1 1

//task_priority 0 to (configMAX_PRIORITIES - 1)
#define Neopixel_task_priority			5
#define Matrix_task_priority			5
#define Eye_task_priority				5
#define Nose_task_priority				5
#define Mouth_task_priority				5
#define BZ_task_priority				5
#define Fan_task_priority				5
#define Ctrl_task_priority				6
#define BLE_task_priority				6

//stack size in word
#define Neopixel_task_stack				4096
#define Matrix_task_stack				2048
#define Eye_task_stack					1024
#define Nose_task_stack					1024
#define Mouth_task_stack				1024
#define BZ_task_stack					2048
#define Fan_task_stack					1024
#define Ctrl_task_stack					2048
#define BLE_task_stack					4096

//Task Handle
TaskHandle_t Neopixel_task_handle;
TaskHandle_t Matrix_task_handle;
TaskHandle_t Eye_task_handle;
TaskHandle_t Nose_task_handle;
TaskHandle_t Mouth_task_handle;
TaskHandle_t BZ_task_handle;
TaskHandle_t Fan_task_handle;
TaskHandle_t Ctrl_task_handle;
TaskHandle_t BLE_task_handle;

#define queue_mode_num 256
uint8_t queue_mode[queue_mode_num];  //mode 0 to 255

//Queue Handle
QueueHandle_t queue_Neopixel;
QueueHandle_t queue_Matrix;
QueueHandle_t queue_Eye;
QueueHandle_t queue_Nose;
QueueHandle_t queue_Mouth;
QueueHandle_t queue_BZ;
QueueHandle_t queue_Fan;
QueueHandle_t queue_Ctrl;
QueueHandle_t queue_BLE;

inline void queue_init(void)
{
	queue_Neopixel = xQueueCreate(1, 1);	//1 items, 1 byte each
	queue_Matrix = xQueueCreate(1, 1);		//1 items, 1 byte each
	queue_Eye = xQueueCreate(1, 1);			//1 items, 1 byte each
	queue_Nose = xQueueCreate(1, 1);		//1 items, 1 byte each
	queue_Mouth = xQueueCreate(1, 1);		//1 items, 1 byte each
	queue_BZ = xQueueCreate(1, 1);			//1 items, 1 byte each
	queue_Fan = xQueueCreate(1, 1);			//1 items, 1 byte each
	queue_Ctrl = xQueueCreate(1, 1);		//1 items, 1 byte each
	queue_BLE = xQueueCreate(1, 1);			//1 items, 1 byte each
	
	for (uint16_t i = 0; i < queue_mode_num; i++)
	{
		queue_mode[i] = i;
	}
}

#define Neopixel_mode(mode)		xQueueSend(queue_Neopixel, queue_mode + mode, 0)
#define Matrix_mode(mode)		xQueueSend(queue_Matrix, queue_mode + mode, 0)
#define Eye_mode(mode)			xQueueSend(queue_Eye, queue_mode + mode, 0)
#define Nose_mode(mode)			xQueueSend(queue_Nose, queue_mode + mode, 0)
#define Mouth_mode(mode)		xQueueSend(queue_Mouth, queue_mode + mode, 0)
#define BZ_mode(mode)			xQueueSend(queue_BZ, queue_mode + mode, 0)
#define Fan_mode(mode)			xQueueSend(queue_Fan, queue_mode + mode, 0)
#define Ctrl_mode(mode)			xQueueSend(queue_Ctrl, queue_mode + mode, 0)
#define BLE_mode(mode)			xQueueSend(queue_BLE, queue_mode + mode, 0)

enum _BZ_mode
{
	BZ_mode_boost			= 0xFF,
	BZ_mode_save			= 0xF0,
	BZ_mode_reset,
	BZ_mode_BLE_set,
	BZ_mode_pitch_set,
	BZ_mode_OFF,
	BZ_mode_beep			= 0x00,
	BZ_mode_dobule_beep,
	BZ_mode_rand_one,
	BZ_mode_rand_two,
	BZ_mode_rand_three,
	BZ_mode_music,
};

enum _Fan_mode
{
	Fan_mode_boost			= 0xFF,
	Fan_mode_off			= 0x00,
	Fan_mode_on,
};

enum _Matrix_mode
{
	Matrix_mode_boost		= 0xFF,
	Matrix_mode_brightness	= 0xFE,
	Matrix_mode_scan		= 0x00,
	Matrix_mode_rickroll,
	Matrix_mode_randdot,
	Matrix_mode_vibrate,
};

enum _Eye_mode
{
	Eye_mode_boost			= 0xFF,
	Eye_mode_general		= 0x00,
	Eye_mode_blink,
	Eye_mode_randdot,
	Eye_mode_boop,
	Eye_mode_vibrate,
};

enum _Nose_mode
{
	Nose_mode_boost			= 0xFF,
	Nose_mode_general		= 0x00,
	Nose_mode_randdot,
	Nose_mode_boop,
	Nose_mode_vibrate,
};

enum _Mouth_mode
{
	Mouth_mode_boost		= 0xFF,
	Mouth_mode_general		= 0x00,
	Mouth_mode_randdot,
	Mouth_mode_rickroll,
	Mouth_mode_vibrate,
};

enum _Neopixel_mode
{
	Neopixel_mode_boost		= 0xFF,
	Neopixel_mode_general	= 0x00,
	Neopixel_mode_Brightness,
	Neopixel_mode_set,
};

enum _Ctrl_mode
{
	Ctrl_mode_boost			= 0xFF,
	Ctrl_mode_general		= 0x00,
};

enum _BLE_mode
{
	BLE_mode_boost			= 0xFF,
	BLE_mode_reconnect		= 0x00,
	BLE_mode_notify,
};

void * ptr_dummy;

typedef struct _sheet
{
	uint8_t pitch;
	uint8_t octave;
	uint8_t note;
	bool joined;
};//pitch,  octave,  beat,  if joined note

typedef struct _music
{
	_sheet *sheet;
	uint16_t tempo;
	uint16_t length;
	uint16_t note[6];
	uint8_t ring_time;
};

typedef struct _cnt
{
	uint16_t	Blink;
	uint16_t	Beep;
	uint16_t	Protosence;
	uint16_t	animate;
	uint16_t	BLE_reconnect;
};

typedef struct _system
{
	uint8_t matrix_refresh;
	uint16_t Blink_period;
	uint8_t Beep_mode;
	uint8_t Beep_pitch;
	uint16_t Beep_period;
	uint8_t Protosence_flag;
	uint8_t Matrix_Brightness;
	uint8_t Startup_Face;
	uint8_t Neo_Brightness;


	bool show_eye;
	bool show_nose;
	bool show_mouth;
	bool animate_on;
	bool music_playing;
};

_cnt cnt;
_system Protogen;

#define	Matrix_refresh()	Protogen.matrix_refresh = 1

//EEPROM
#define EEPROM_SIZE						64
#define EEPROM_saved					0xAA
enum _EEPROM
{
	EEPROM_Addr_Saved,
	EEPROM_Addr_Blink_period,
	EEPROM_Addr_Beep_mode,
	EEPROM_Addr_Beep_pitch,
	EEPROM_Addr_Beep_period,
	EEPROM_Addr_Protosence_flag,
	EERPON_Addr_Matrix_brightness,
	EEPROM_Addr_Startup_Face,
	EERPON_Addr_Neo_brightness,
};

inline void EEPROM_Save(void)
{
	EEPROM.write(EEPROM_Addr_Saved, EEPROM_saved);
	EEPROM.write(EEPROM_Addr_Blink_period, Protogen.Blink_period);
	EEPROM.write(EEPROM_Addr_Beep_mode, Protogen.Beep_mode);
	EEPROM.write(EEPROM_Addr_Beep_pitch, Protogen.Beep_pitch);
	EEPROM.write(EEPROM_Addr_Beep_period, Protogen.Beep_period);
	EEPROM.write(EEPROM_Addr_Protosence_flag, Protogen.Protosence_flag);
	EEPROM.write(EERPON_Addr_Matrix_brightness, (uint32_t)Protogen.Matrix_Brightness);
	EEPROM.write(EEPROM_Addr_Startup_Face, (uint32_t)Protogen.Startup_Face);
	EEPROM.write(EERPON_Addr_Neo_brightness, (uint32_t)Protogen.Neo_Brightness);

	EEPROM.commit();
}

inline void EEPROM_Load(void)
{
	Protogen.Blink_period = EEPROM.read(EEPROM_Addr_Blink_period);
	Protogen.Beep_mode = EEPROM.read(EEPROM_Addr_Beep_mode);
	Protogen.Beep_pitch = EEPROM.read(EEPROM_Addr_Beep_pitch);
	Protogen.Beep_period = EEPROM.read(EEPROM_Addr_Beep_period);
	Protogen.Protosence_flag = EEPROM.read(EEPROM_Addr_Protosence_flag);
	Protogen.Matrix_Brightness = EEPROM.read(EERPON_Addr_Matrix_brightness);
	Protogen.Startup_Face = EEPROM.read(EEPROM_Addr_Startup_Face);
	Protogen.Neo_Brightness = EEPROM.read(EERPON_Addr_Neo_brightness);
}

inline void System_Reset(void)
{
	Protogen.Blink_period = eye_blink_cycle;
	Protogen.Beep_mode = Beep_mode_init;
	Protogen.Beep_pitch = Beep_pitch_init;
	Protogen.Beep_period = Beep_cycle;
	Protogen.Protosence_flag = Protosence_on;
	Protogen.Matrix_Brightness = Matrix_brightness_init;
	Protogen.Startup_Face = 0;
	Protogen.Neo_Brightness = neo_ring_brightness;
	Protogen.show_eye = 1;
	Protogen.show_nose = 1;
	Protogen.show_mouth = 1;
	Protogen.animate_on = 0;
	Protogen.music_playing = 0;

	cnt.Blink = 0;
	cnt.animate = 0;
	cnt.Beep = 0;
	cnt.Protosence = 0;

	EEPROM.write(EEPROM_Addr_Saved, 0);
	EEPROM.commit();
}

inline void parameter_init(void)
{
	cnt.Blink			= 0;
	cnt.Beep			= 0;
	cnt.Protosence	 	= 0;
	cnt.animate		 	= 0;
	cnt.BLE_reconnect 	= 0;

	Protogen.matrix_refresh = 1;
	Protogen.Blink_period = eye_blink_cycle;
	Protogen.Beep_mode = Beep_mode_init;
	Protogen.Beep_pitch = Beep_pitch_init;
	Protogen.Beep_period = Beep_cycle;
	Protogen.Protosence_flag = Protosence_on;
	Protogen.Matrix_Brightness = Matrix_brightness_init;
	Protogen.Startup_Face = 0;
	Protogen.Neo_Brightness = neo_ring_brightness;
	Protogen.show_eye = 1;
	Protogen.show_nose = 1;
	Protogen.show_mouth = 1;
	Protogen.animate_on = 0;
	Protogen.music_playing = 0;
}