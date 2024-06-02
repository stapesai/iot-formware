import asyncio
from bleak import BleakClient, BleakScanner
from dotenv import load_dotenv
import os

load_dotenv()

data = f"{os.getenv('WIFI_SSID')};{os.getenv('WIFI_PASSWORD')};{os.getenv('MQTT_BROKER_IP')};{os.getenv('MQTT_BROKER_PORT')};{os.getenv('MQTT_USER')};{os.getenv('MQTT_PASSWORD')};{os.getenv('MQTT_TOPIC')}"

device_name = os.getenv("BLE_DEVICE_NAME")

async def find_device_by_name(name):
    devices = await BleakScanner.discover()
    for device in devices:
        if device.name == name:
            return device
    return None

async def send_data():
    device = await find_device_by_name(device_name)
    if not device:
        print(f"Device with name {device_name} not found")
        return

    async with BleakClient(device.address) as client:
        if await client.is_connected():
            print(f"Connected to {device.address}")
            await client.write_gatt_char(os.getenv('CHARACTERISTIC_UUID'), data=bytes(data, 'utf-8'))
            print("Data sent")

loop = asyncio.get_event_loop()
loop.run_until_complete(send_data())
