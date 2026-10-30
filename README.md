# ATOM-CHARGER

Battery monitor for **M5Stack ATOM S3 + Atomic Motion Base V1.2**.

## Arduino IDE
1. Install M5Stack ESP32 board support and select **M5AtomS3**.
2. Install **M5Unified** and **M5AtomicMotion** libraries.
3. Open `ATOM_CHARGER/ATOM_CHARGER.ino` and upload.
4. Open Serial Monitor at 115200 baud.

## Display
- **CHARGING**: measured battery current appears to flow into the battery.
- **DISCHARGING**: measured current appears to flow out of the battery. This is not proof USB is absent.
- **IDLE / UNKNOWN**: current near zero; cannot determine FULL vs unplugged from current alone.
- **%***: approximate lithium-ion battery charge estimated from voltage (not a true fuel gauge).
- Battery voltage and current updated every second.
- Sensor error if INA226 data is unavailable.

## IMPORTANT
The charger circuit operates independently of this program. The program does **not** control charging. Do not connect USB 5V directly to an ESP32 GPIO. For reliable FULL and NO CHARGE indicators, hardware measurement of external 5V or charger-status signals is necessary. Check polarity using Serial Monitor: if charge current is negative, set `CHARGING_CURRENT_SIGN=-1`. Percentage is inaccurate under load or while charging.

**Status: untested initial implementation.** Verify M5AtomicMotion library API, I2C pins, voltage/current units, and battery current polarity with the actual hardware before relying on the readings.

## References
- https://docs.m5stack.com/en/atom/Atomic_Motion_Base_v1.2
- https://github.com/m5stack/M5Atomic-Motion
