#include "BLEID.h"

#define Buffer_max	20

int8_t BLE_buffer_index = -1;
uint16_t BLE_cmd[Buffer_max];

// The remote service we wish to connect to.
static BLEUUID serviceUUID(SERVICE_UUID);
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID(CHARACTERISTIC_UUID);

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{
	if (BLE_buffer_index < (Buffer_max - 1))
	{
		BLE_buffer_index++;
		BLE_cmd[BLE_buffer_index] = ((pData[1]) << 8) + pData[0];
	}
	BLE_mode(BLE_mode_notify);
}

class MyClientCallback : public BLEClientCallbacks {
	void onConnect(BLEClient* pclient) {
		connected = true;
		BLE_buffer_index = -1;
		#if do_serial
		Serial.println("onConnect");
		#endif
	}

	void onDisconnect(BLEClient* pclient) {
		connected = false;
		#if do_serial
		Serial.println("onDisconnect");
		#endif
	}
};

bool connectToServer() {
	#if do_serial
	Serial.print("Forming a connection to ");
	Serial.println(myDevice->getAddress().toString().c_str());
	#endif

	BLEClient*  pClient = BLEDevice::createClient();
	#if do_serial
	Serial.println(" - Created client");
	#endif

	pClient->setClientCallbacks(new MyClientCallback());

	// Connect to the remove BLE Server.
	pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
	#if do_serial
	Serial.println(" - Connected to server");
	#endif

	// Obtain a reference to the service we are after in the remote BLE server.
	BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
	if (pRemoteService == nullptr) {
		#if do_serial
		Serial.print("Failed to find our service UUID: ");
		Serial.println(serviceUUID.toString().c_str());
		#endif
		pClient->disconnect();
		return false;
	}
	#if do_serial
	Serial.println(" - Found our service");
	#endif


	// Obtain a reference to the characteristic in the service of the remote BLE server.
	pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
	if (pRemoteCharacteristic == nullptr) {
		#if do_serial
		Serial.print("Failed to find our characteristic UUID: ");
		Serial.println(charUUID.toString().c_str());
		#endif
		pClient->disconnect();
		return false;
	}
	Serial.println(" - Found our characteristic");

	// Read the value of the characteristic.
	if (pRemoteCharacteristic->canRead()) {
		uint16_t value = pRemoteCharacteristic->readUInt16();
		#if do_serial
		Serial.print("The characteristic value was: ");
		Serial.println(value);
		#endif
	}

	if (pRemoteCharacteristic->canNotify())
		pRemoteCharacteristic->registerForNotify(notifyCallback);

	connected = true;
	return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
	/**
	  * Called for each advertising BLE server.
	  */
	void onResult(BLEAdvertisedDevice advertisedDevice) {
		#if do_serial
		Serial.print("BLE Advertised Device found: ");
		Serial.println(advertisedDevice.toString().c_str());
		#endif

		// We have found a device, let us now see if it contains the service we are looking for.
		if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

			BLEDevice::getScan()->stop();
			myDevice = new BLEAdvertisedDevice(advertisedDevice);
			doConnect = true;
			doScan = true;

		} // Found our server
	} // onResult
}; // MyAdvertisedDeviceCallbacks



void BLE_init(void)
{
	#if do_serial
	Serial.println("Starting Arduino BLE Client application...");
	#endif
	BLEDevice::init("");

	// Retrieve a Scanner and set the callback we want to use to be informed when we
	// have detected a new device.  Specify that we want active scanning and start the
	// scan to run for 5 seconds.
	BLEScan* pBLEScan = BLEDevice::getScan();
	pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
	pBLEScan->setInterval(1349);
	pBLEScan->setWindow(449);
	pBLEScan->setActiveScan(true);
	pBLEScan->start(2, false);

	if (doConnect == true)
	{
		if (connectToServer())
		{
			#if do_serial
			Serial.println("We are now connected to the Server");
			#endif
		}
		else
		{
			#if do_serial
			Serial.println("Failed to connect to the server");
			#endif
		}
		doConnect = false;
	}
}

#define CMD_Bz			0x000
#define CMD_Setting		0x100
#define CMD_Face		0x200
#define CMD_Neo_pixel	0x300
