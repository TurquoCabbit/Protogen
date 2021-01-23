#define ADC_init_average_time	10 		//in ms
#define ADC_init_scan_intervial	1		//in ms
#define ADC_average_time 		5000	//in ms
#define ADC_scan_intervial		50		//in ms

#define Blaster_install 1

//Battery
#define Battery_Break_Voltage_3	9
#define Battery_Low_Voltage_3	10
#define Battery_Full_Voltage_3	12.6

#define Battery_Break_Voltage_1	3
#define Battery_Low_Voltage_1	3.3  
#define Battery_Full_Voltage_1	4.2

typedef struct _Battery_level
{
	uint32_t Break;
	uint32_t Low;
	uint32_t Full;
};

_Battery_level Battery_level;

inline void ADC_init(void)
{
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_0);  //full-scale voltage is 1.1V
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);  //full-scale voltage is 1.1V
}

inline uint32_t ADC_convert(bool remote)
{
	if(remote)
	{
		return (uint32_t)(adc1_get_raw(ADC1_CHANNEL_4) * 1600);
	}
	else
	{
		return (uint32_t)(adc1_get_raw(ADC1_CHANNEL_3) * 1600);
	}
	
}

uint32_t float_int_converter(float temp)
{
	return (uint32_t)(temp + 0.5);
}

inline void Battery_level_init(void)
{
	if (Blaster.remote) //remote
	{
		Battery_level.Break = (uint32_t)(Battery_Break_Voltage_1 * Blaster.ADC_gain + 0.5);
		Battery_level.Low = (uint32_t)(Battery_Low_Voltage_1 * Blaster.ADC_gain + 0.5);
		Battery_level.Full = (uint32_t)(Battery_Full_Voltage_1 * Blaster.ADC_gain + 0.5);
	}
	else //Gun
	{
		Battery_level.Break = (uint32_t)(Battery_Break_Voltage_3 * Blaster.ADC_gain + 0.5);
		Battery_level.Low = (uint32_t)(Battery_Low_Voltage_3 * Blaster.ADC_gain + 0.5);
		Battery_level.Full = (uint32_t)(Battery_Full_Voltage_3 * Blaster.ADC_gain + 0.5);
	}
}