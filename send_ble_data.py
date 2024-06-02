import asyncio
from bleak import BleakClient
from dotenv import load_dotenv
import os

load_dotenv()

data = f"{os.getenv('WIFI_SSID')};{os.getenv('WIFI_PASSWORD')};{os.getenv('MQTT_BROKER_IP')};{os.getenv('MQTT_BROKER_PORT')};{os.getenv('MQTT_USER')};{os.getenv('MQTT_PASSWORD')};{os.getenv('MQTT_TOPIC')}"

async def send_data():
    async with BleakClient(os.getenv("BLE_DEVICE_MAC")) as client:
        if await client.is_connected:
            print(f"Connected to {os.getenv('BLE_ADDRESS')}")
            await client.write_gatt_char(os.getenv('CHARACTERISTIC_UUID'), data=bytes(data, 'utf-8'))
            print("Data sent")

loop = asyncio.get_event_loop()
loop.run_until_complete(send_data())