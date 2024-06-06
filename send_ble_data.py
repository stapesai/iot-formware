# Path: send_ble_data.py
# Description: This script sends the wifi and mqtt configuration to the ESP32 via BLE.

import asyncio
from bleak import BleakClient, BleakScanner
from dotenv import load_dotenv
import os

load_dotenv()

data = f"{os.getenv('WIFI_SSID')};{os.getenv('WIFI_PASSWORD')};{os.getenv('MQTT_BROKER_IP')};{os.getenv('MQTT_BROKER_PORT')};{os.getenv('MQTT_USERNAME')};{os.getenv('MQTT_PASSWORD')};{os.getenv('MQTT_TOPIC')}"

device_name = os.getenv("BLE_DEVICE_NAME")
characteristic_uuid = os.getenv('CHARACTERISTIC_UUID')

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
        if client.is_connected:
            print(f"Connected to {device.address}")

            # Split the data into chunks if necessary
            mtu = await client.get_mtu()
            chunk_size = mtu - 3  # BLE limits the payload size (MTU - 3 for overhead)
            for i in range(0, len(data), chunk_size):
                chunk = data[i:i + chunk_size]
                await client.write_gatt_char(characteristic_uuid, data=bytes(chunk, 'utf-8'))
                print(f"Sent chunk: {chunk}")

loop = asyncio.get_event_loop()
loop.run_until_complete(send_data())
