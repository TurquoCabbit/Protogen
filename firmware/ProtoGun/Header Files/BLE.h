#include "BLEID.h"

#define Buffer_max	20

bool BLE_connected;
int8_t BLE_buffer_index = -1;
uint16_t BLE_value[Buffer_max];
uint16_t BLE_cmd = 0xFFFF;

class MyServerCallback : public BLEServerCallbacks {
	void onConnect(BLEServer* pServer) {
		BLE_connected = true;
		BLE_buffer_index = -1;
		serial_log(0, "onConnect");
	}

	void onDisconnect(BLEServer* pServer) {
		BLE_connected = false;
		serial_log(0, "onDisconnect");
	}
};

void ProtoGun_setCMD(uint16_t cmd)
{
	if (BLE_connected && BLE_buffer_index < (Buffer_max - 1))
	{
		BLE_buffer_index++;
		BLE_value[BLE_buffer_index] = (ProtoGen_ID << 12) + (cmd & 0x0FFF);
	}
}