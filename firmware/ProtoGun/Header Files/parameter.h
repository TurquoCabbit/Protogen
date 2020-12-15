#define core_0 0
#define core_1 1

//task_priority 0 to (configMAX_PRIORITIES - 1)
#define Fan_task_priority				5
#define ADC_task_priority				5
#define Ctrl_task_priority				6
#define BLE_task_priority				5
#define VM_task_priority				5
#define GUI_task_priority				6

//stack size in word
#define Fan_task_stack					1024
#define ADC_task_stack					2048
#define Ctrl_task_stack					1024
#define BLE_task_stack					4096
#define VM_task_stack					1024
#define GUI_task_stack					4096

//Task Handle
TaskHandle_t Fan_task_handle;
TaskHandle_t ADC_task_handle;
TaskHandle_t Ctrl_task_handle;
TaskHandle_t BLE_task_handle;
TaskHandle_t VM_task_handle;
TaskHandle_t GUI_task_handle;

#define queue_mode_num 256
uint8_t queue_mode[queue_mode_num];  //mode 0 to 255

//Queue Handle
QueueHandle_t queue_Fan;
QueueHandle_t queue_ADC;
QueueHandle_t queue_Ctrl;
QueueHandle_t queue_BLE;
QueueHandle_t queue_VM;
QueueHandle_t queue_GUI;

void queue_init(void)
{
	queue_Fan = xQueueCreate(1, 1);       //1 items, 1 byte each
	queue_ADC = xQueueCreate(1, 1);       //1 items, 1 byte each
	queue_Ctrl = xQueueCreate(1, 1);      //1 items, 1 byte each
	queue_BLE = xQueueCreate(1, 1);       //1 items, 1 byte each
	queue_VM = xQueueCreate(1, 1);       //1 items, 1 byte each
	queue_GUI = xQueueCreate(1, 1);       //1 items, 1 byte each

	for (uint16_t i = 0; i < queue_mode_num; i++)
	{
		queue_mode[i] = i;
	}
}

#define Fan_mode(mode)	xQueueSend(queue_Fan, queue_mode + mode, 0)
#define ADC_mode(mode)	xQueueSend(queue_ADC, queue_mode + mode, 0)
#define Ctrl_mode(mode)	xQueueSend(queue_Ctrl, queue_mode + mode, 0)
#define BLE_mode(mode)	xQueueSend(queue_BLE, queue_mode + mode, 0)
#define VM_mode(mode)	xQueueSend(queue_VM, queue_mode + mode, 0)
#define GUI_mode(mode)	xQueueSend(queue_GUI, queue_mode + mode, 0)

#define use_enum 1
#if use_enum
enum Fan_mode
{
	Fan_mode_boost		= 0xFF,
	Fan_mode_off		= 0xF0,
	Fan_mode_general	= 0x00,
};

enum ADC_mode
{
	ADC_mode_boost		= 0xFF,
	ADC_mode_off		= 0xF0,
	ADC_mode_general	= 0x00,
	ADC_mode_calculation
};

enum Ctrl_mode
{
	Ctrl_mode_boost		= 0xFF,
	Ctrl_mode_off		= 0xF0,
	Ctrl_mode_general	= 0x00,
};

enum BLE_mode
{
	BLE_mode_boost		= 0xFF,
	BLE_mode_off		= 0xF0,
	BLE_mode_general	= 0x00,
	BLE_mode_notify,
};

enum VM_mode
{
	VM_mode_boost		= 0xFF,
	VM_mode_off			= 0xF0,
	VM_mode_general		= 0x00,
	VM_mode_on_long,
	VM_mode_on_short,
	VM_mode_on_extra,
};

enum GUI_mode
{
	GUI_mode_boost				= 0xFF,
 	GUI_mode_save				= 0xFE,
 	GUI_mode_sleep				= 0xFD,
	GUI_mode_off				= 0xF0,
	GUI_mode_main				= 0x01,
	GUI_mode_setting,
	GUI_mode_startface			= 0x10,
	GUI_mode_toggleface,
	GUI_mode_Matrix_brightness,
	GUI_mode_blink,
 	GUI_mode_beep,
 	GUI_mode_boop,
 	GUI_mode_Neo_brightness,
 	GUI_mode_Fan,
 	GUI_mode_ADC,
 	GUI_mode_leave,
 	GUI_mode_reset,
 	GUI_mode_beep_mode			= 0x30,
 	GUI_mode_beep_period,
 	GUI_mode_beep_pitch,
 	GUI_mode_signature			= 0xA0,
};
#else
//Fan mode define
#define Fan_mode_boost		0xFF
#define Fan_mode_off		0xF0
#define Fan_mode_general	0x01

//ADC mode define
#define ADC_mode_boost			0xFF
#define ADC_mode_off			0xF0
#define ADC_mode_general		0x00
#define ADC_mode_calculation	0x01

//Ctrl mode define
#define Ctrl_mode_boost		0xFF
#define Ctrl_mode_off		0xF0
#define Ctrl_mode_general	0x01

//BLE mode define
#define BLE_mode_boost		0xFF
#define BLE_mode_off		0xF0
#define BLE_mode_notify		0x00

//VM mode define
#define VM_mode_boost		0xFF
#define VM_mode_off			0xF0
#define VM_mode_on_long		0x01
#define VM_mode_on_short	0x02
#define VM_mode_on_extra	0x03

//GUI mode define
#define GUI_mode_boost				0xFF
#define GUI_mode_off				0xF0
#define GUI_mode_main				0x01
#define GUI_mode_setting			0x02
#define GUI_mode_startface			0x10
#define GUI_mode_toggleface			GUI_mode_startface + 1
#define GUI_mode_Matrix_brightness	GUI_mode_startface + 2
#define GUI_mode_blink				GUI_mode_startface + 3
#define GUI_mode_beep				GUI_mode_startface + 4
#define GUI_mode_boop				GUI_mode_startface + 5
#define GUI_mode_Neo_brightness		GUI_mode_startface + 6
#define GUI_mode_Fan				GUI_mode_startface + 7
#define GUI_mode_ADC				GUI_mode_startface + 8
#define GUI_mode_leave				GUI_mode_startface + 9
#define GUI_mode_reset				GUI_mode_startface + 10
#define GUI_mode_signature			0xA0
#define GUI_mode_beep_mode			0x30
#define GUI_mode_beep_period		GUI_mode_beep_mode + 1
#define GUI_mode_beep_pitch			GUI_mode_beep_mode + 2
#define GUI_mode_save				0xFE
#define GUI_mode_sleep				0xFD
#endif

#define GUI_temp32_init		0xFFFF0000
#define GUI_temp16_init		0xFF00

typedef struct _system_status
{
	bool Power_Low;
	bool Power_Break;
	bool Fan_ready;
	uint8_t Battery;
	bool remote;
	uint32_t ADC_value;

	uint8_t Fan_duty;
	uint8_t Face_index;
	uint8_t Face_startup_index;
	uint8_t Face_toggle_index;
	uint8_t Matrix_Brightness;
	uint8_t Blink_period;
	uint8_t Boop;
	uint8_t Beep_period;
	uint8_t Beep_mode;
	uint8_t Neo_Brightness;
	uint32_t ADC_gain;
};

_system_status Blaster =
{
	0,	//Power_Low
	0,	//Power_Break
	0,	//Fan_ready
	5,	//Battery
	1,	//remote
	0,	//ADC_value
	Fan_duty_init,	//Fan_duty
	Face_index_general,	//Face_index
	Face_index_general,	//Face_startup_index
	Face_index_general + 1,	//Face_toggle_index
	Matrix_brightness_init,	//Matrix_Brightness
	Blink_period_init,	//Blink_period
	Boop_init,	//Boop
	Beep_period_init,	//Beep_period
	Beep_mode_init,	//Beep_mode
	Neo_brightness_init,//Neo_Brightness
	ADC_gain_init
};

//EEPROM
#define EEPROM_SIZE						64
#define EEPROM_saved					0xAA
#if use_enum
enum EEPROM
{
	EEPROM_Addr_Saved,
	EEPROM_Addr_Fan_duty,
	EEPROM_Addr_Face_startup_index,
	EEPROM_Addr_Face_toggle_index,
	EEPROM_Addr_Matrix_Brightness,
	EEPROM_Addr_Blink_period,
	EEPROM_Addr_Boop,
	EEPROM_Addr_Beep_period,
	EEPROM_Addr_Beep_mode,
	EEPROM_Addr_Neo_brightness,
	EEPROM_Addr_ADC_gain_HH,
	EEPROM_Addr_ADC_gain_HL,
	EEPROM_Addr_ADC_gain_LH,
	EEPROM_Addr_ADC_gain_LL,
};
#else
#define EEPROM_Addr_Saved				0
#define EEPROM_Addr_Fan_duty			1
#define EEPROM_Addr_Face_startup_index	2
#define EEPROM_Addr_Face_toggle_index	3
#define EEPROM_Addr_Matrix_Brightness	4
#define EEPROM_Addr_Blink_period		5
#define EEPROM_Addr_Boop				6
#define EEPROM_Addr_Beep_period			7
#define EEPROM_Addr_Beep_mode			8
#define EEPROM_Addr_Neo_brightness		9
#define EEPROM_Addr_ADC_gain_HH			10
#define EEPROM_Addr_ADC_gain_HL			11
#define EEPROM_Addr_ADC_gain_LH			12
#define EEPROM_Addr_ADC_gain_LL			13
#endif

inline void EEPROM_Save(void)
{
	EEPROM.write(EEPROM_Addr_Saved, EEPROM_saved);
	EEPROM.write(EEPROM_Addr_Fan_duty, Blaster.Fan_duty);
	EEPROM.write(EEPROM_Addr_Face_startup_index, Blaster.Face_startup_index);
	EEPROM.write(EEPROM_Addr_Face_toggle_index, Blaster.Face_toggle_index);
	EEPROM.write(EEPROM_Addr_Matrix_Brightness, Blaster.Matrix_Brightness);
	EEPROM.write(EEPROM_Addr_Blink_period, Blaster.Blink_period);
	EEPROM.write(EEPROM_Addr_Boop, Blaster.Boop);
	EEPROM.write(EEPROM_Addr_Beep_period, Blaster.Beep_period);
	EEPROM.write(EEPROM_Addr_Beep_mode, Blaster.Beep_mode);
	EEPROM.write(EEPROM_Addr_Neo_brightness, Blaster.Neo_Brightness);
	EEPROM.write(EEPROM_Addr_ADC_gain_HH, (Blaster.ADC_gain >> 24) & 0xFF);
	EEPROM.write(EEPROM_Addr_ADC_gain_HL, (Blaster.ADC_gain >> 16) & 0xFF);
	EEPROM.write(EEPROM_Addr_ADC_gain_LH, (Blaster.ADC_gain >> 8) & 0xFF);
	EEPROM.write(EEPROM_Addr_ADC_gain_LL, Blaster.ADC_gain);

	EEPROM.commit();
}

inline void EEPROM_Load(void)
{
	Blaster.Fan_duty = EEPROM.read(EEPROM_Addr_Fan_duty);
	Blaster.Face_startup_index = EEPROM.read(EEPROM_Addr_Face_startup_index);
	Blaster.Face_index = Blaster.Face_startup_index;
	Blaster.Face_toggle_index = EEPROM.read(EEPROM_Addr_Face_toggle_index);
	Blaster.Matrix_Brightness = EEPROM.read(EEPROM_Addr_Matrix_Brightness);
	Blaster.Blink_period = EEPROM.read(EEPROM_Addr_Blink_period);
	Blaster.Boop = EEPROM.read(EEPROM_Addr_Boop);
	Blaster.Beep_period = EEPROM.read(EEPROM_Addr_Beep_period);
	Blaster.Beep_mode = EEPROM.read(EEPROM_Addr_Beep_mode);
	Blaster.Neo_Brightness = EEPROM.read(EEPROM_Addr_Neo_brightness);
	Blaster.ADC_gain = (EEPROM.read(EEPROM_Addr_ADC_gain_HH) << 24) + (EEPROM.read(EEPROM_Addr_ADC_gain_HL) << 16) + (EEPROM.read(EEPROM_Addr_ADC_gain_LH) << 8) + EEPROM.read(EEPROM_Addr_ADC_gain_LL);
}

inline void System_Reset(void)
{
	Blaster.Fan_duty = Fan_duty_init;
	Blaster.Face_index = Face_index_general;
	Blaster.Face_startup_index = Face_index_general;
	Blaster.Face_toggle_index = Face_index_general + 1;
	Blaster.Matrix_Brightness = Matrix_brightness_init;
	Blaster.Blink_period = Blink_period_init;
	Blaster.Boop = Boop_init;
	Blaster.Beep_period = Beep_period_init;
	Blaster.Beep_mode = Beep_mode_init;
	Blaster.Neo_Brightness = Neo_brightness_init;
	Blaster.ADC_gain = ADC_gain_init;

	EEPROM.write(EEPROM_Addr_Saved, 0);
	EEPROM.commit();
}