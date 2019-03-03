# Plant-o-mat
A device based on the Attiny13 which notify you if your plant needs water.
The firmware of the device is optimized for low power consumption.

# Features
- Notify you if the humidity of your planting pot drops below a adjustable Threshold
- preservment of the last adjusted Threshold even if batterie fails (throught EEPROM)
- Only notify you at day (throught LDR)
- Long batterie live

# Circuit
PB0 = Sound (PWM)
PB1 = Button (for adjusting the Threshold)
PB2 = Humidity Sensor
PB3 = Debug-LED (Should be turned off)
PB4 = LDR

# Usage
1. Make sure the humidity sensor of the Plant-o-mat is firmly conected to the soil.

2. Wait till the humidity-level of the soil reached the desired Threshold.

3. Poweron the device and press the button (you can poweron the device earlier, but i do not recommend this).

4. If the device do not notify you throught a periodic sound, press the button again till the device do so.

5. Watering your plants (do not water the Plant-o-mat, exept he has a waterproof housing).

# Changes & Fixes in the latest version:
 
- Fix issue when setting EEPROM-Trigger up to 4 seconds after startup fails
- Reduce Overhead
