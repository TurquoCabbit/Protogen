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

inline void queue_init(void)
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

#if Fan_fsm_log
	#define Fan_mode(mode)	{xQueueSend(queue_Fan, queue_mode + mode, 0); serial_log("Fan_fsm", mode);}
#else
	#define Fan_mode(mode)	xQueueSend(queue_Fan, queue_mode + mode, 0)
#endif
#if ADC_fsm_log
	#define ADC_mode(mode)	{xQueueSend(queue_ADC, queue_mode + mode, 0); serial_log("ADC_fsm", mode);}
#else
	#define ADC_mode(mode)	xQueueSend(queue_ADC, queue_mode + mode, 0)
#endif
#if Ctrl_fsm_log
	#define Ctrl_mode(mode)	{xQueueSend(queue_Ctrl, queue_mode + mode, 0); serial_log("Ctrl_fsm", mode);}
#else
	#define Ctrl_mode(mode)	xQueueSend(queue_Ctrl, queue_mode + mode, 0)
#endif
#if BLE_fsm_log
	#define BLE_mode(mode)	{xQueueSend(queue_BLE, queue_mode + mode, 0); serial_log("BLE_fsm", mode);}
#else
	#define BLE_mode(mode)	xQueueSend(queue_BLE, queue_mode + mode, 0)
#endif
#if VM_fsm_log
	#define VM_mode(mode)	{xQueueSend(queue_VM, queue_mode + mode, 0); serial_log("VM_fsm", mode);}
#else
	#define VM_mode(mode)	xQueueSend(queue_VM, queue_mode + mode, 0)
#endif
#if GUI_fsm_log
	#define GUI_mode(mode)	{xQueueSend(queue_GUI, queue_mode + mode, 0); serial_log("GUI_fsm", mode);}
#else
	#define GUI_mode(mode)	xQueueSend(queue_GUI, queue_mode + mode, 0)
#endif


enum _Fan_mode
{
	Fan_mode_boost		= 0xFF,
	Fan_mode_off		= 0xF0,
	Fan_mode_general	= 0x00,
};

enum _ADC_mode
{
	ADC_mode_pre_boost	= 0xFF,
	ADC_mode_boost		= 0xFE,
	ADC_mode_off		= 0xF0,
	ADC_mode_general	= 0x00,
	ADC_mode_calculation,
	ADC_mode_terminate,
};

enum _Ctrl_mode
{
	Ctrl_mode_boost		= 0xFF,
	Ctrl_mode_off		= 0xF0,
	Ctrl_mode_general	= 0x00,
};

enum _BLE_mode
{
	BLE_mode_boost		= 0xFF,
	BLE_mode_off		= 0xF0,
	BLE_mode_general	= 0x00,
	BLE_mode_notify,
};

enum _VM_mode
{
	VM_mode_boost			= 0xFF,
	VM_mode_off				= 0xF0,
	VM_mode_on_short		= 0x00,
	VM_mode_on_short_twice,
	VM_mode_on_short_thrice,
	VM_mode_on_long			= 0x10,
	VM_mode_on_long_twice,
	VM_mode_on_long_thrice,
	VM_mode_on_extra_long	= 0x20,
	VM_mode_on_extra_long_twice,
	VM_mode_on_extra_long_thrice,
};

enum _GUI_mode
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

#define GUI_temp32_init		0xFFFF0000
#define GUI_temp16_init		0xFF00

typedef struct _system_status
{
	bool Power_Low		:1;
	bool Power_Break	:1;
	bool Fan_ready		:1;
	bool remote			:1;
	uint8_t Battery		:4;
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

_system_status Blaster;

//EEPROM
#define EEPROM_SIZE						64
#define EEPROM_saved					0xAA
enum _EEPROM
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

inline void EEPROM_Save_offline(void)
{
	EEPROM.write(EEPROM_Addr_Saved, EEPROM_saved);
	EEPROM.write(EEPROM_Addr_Fan_duty, Blaster.Fan_duty);
	EEPROM.write(EEPROM_Addr_Face_startup_index, Blaster.Face_startup_index);
	EEPROM.write(EEPROM_Addr_Face_toggle_index, Blaster.Face_toggle_index);
	EEPROM.write(EEPROM_Addr_Matrix_Brightness, Blaster.Matrix_Brightness);
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

inline void EEPROM_Load_offload(void)
{
	Blaster.Fan_duty = EEPROM.read(EEPROM_Addr_Fan_duty);
	Blaster.Face_startup_index = EEPROM.read(EEPROM_Addr_Face_startup_index);
	Blaster.Face_index = Blaster.Face_startup_index;
	Blaster.Face_toggle_index = EEPROM.read(EEPROM_Addr_Face_toggle_index);
	Blaster.ADC_gain = (EEPROM.read(EEPROM_Addr_ADC_gain_HH) << 24) + (EEPROM.read(EEPROM_Addr_ADC_gain_HL) << 16) + (EEPROM.read(EEPROM_Addr_ADC_gain_LH) << 8) + EEPROM.read(EEPROM_Addr_ADC_gain_LL);
}

inline void System_Reset(void)
{
	Blaster.Fan_duty = Fan_duty_init;
	Blaster.Face_index = 0;
	Blaster.Face_startup_index = 0;
	Blaster.Face_toggle_index = 1;
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

inline void System_Reset_offline(void)
{
	Blaster.Fan_duty = Fan_duty_init;
	Blaster.Face_index = 0;
	Blaster.Face_startup_index = 0;
	Blaster.Face_toggle_index = 1;
	Blaster.ADC_gain = ADC_gain_init;

	EEPROM_Save_offline();
	EEPROM.commit();

}

inline void parameter_init(void)
{
	Blaster.Power_Low = 0;
	Blaster.Power_Break = 0;
	Blaster.Fan_ready = 0;
	Blaster.Battery = 5;
	Blaster.remote = 1;
	Blaster.ADC_value = 0;
	Blaster.Fan_duty = Fan_duty_init;
	Blaster.Face_index = 0;
	Blaster.Face_startup_index = 0;
	Blaster.Face_toggle_index = 1;
	Blaster.Matrix_Brightness = Matrix_brightness_init;
	Blaster.Blink_period = Blink_period_init;
	Blaster.Boop = Boop_init;
	Blaster.Beep_period = Beep_period_init;
	Blaster.Beep_mode = Beep_mode_init;
	Blaster.Neo_Brightness = Neo_brightness_init;
	Blaster.ADC_gain = ADC_gain_init;
}