# HealthCore Specification v0.1

## 1. Purpose

HealthCore is a reusable embedded library that defines the common data model used by the Health Framework.

HealthCore contains **no application logic**.

Its only responsibility is to define the data structures shared by all other modules.

HealthCore must be reusable in any ESP32 project without modification.

---

## 2. Responsibilities

HealthCore shall define:

- Common data types
- HealthRecord structure
- Priority definitions
- Freshness definitions
- Common constants
- Parameter identifiers
- Public type definitions

HealthCore shall **not**:

- Start tasks
- Calculate timeouts
- Blink LEDs
- Print to Serial
- Communicate over WiFi
- Allocate background resources

---

## 3. Design Goals

- Small memory footprint
- Easy to understand
- Thread-safe by design (no global mutable state)
- Independent of application logic
- Portable to Arduino and ESP-IDF
- Easy to extend without breaking compatibility

---

## 4. HealthRecord

Each monitored parameter is represented by one HealthRecord.

A HealthRecord contains:

- Parameter ID
- Description
- Current Value
- Unit
- Source
- Last Update Time
- Timeout
- Priority

Example:

```
ID           : S1
Description  : Engine Temperature
Value        : 82.4
Unit         : °C
Source       : CAN
Last Update  : 123456789 ms
Timeout      : 8000 ms
Priority     : HIGH
```

---

## 5. Freshness

Freshness is **never stored**.

It is always calculated by HealthManager.

Possible values:

- UNKNOWN
- FRESH
- OLD

Rules:

- UNKNOWN = Parameter has never been updated.
- FRESH = Last update age ≤ timeout.
- OLD = Last update age > timeout.

---

## 6. Priority

Four reporting priorities:

- LOW
- NORMAL
- HIGH
- CRITICAL

Priority affects presentation order only.

---

## 7. Value

HealthCore does not interpret parameter values.

Examples:

- OK
- ERROR
- ONLINE
- OFFLINE
- FIX3D
- 23.5
- 82.4
- 12.8
- 75%

The meaning belongs to the application.

---

## 8. Parameter ID

Every parameter has a short identifier suitable for Morse transmission.

Examples:

- W
- MQTT
- GPS
- RTC
- S1
- S2
- TEMP
- BAT

---

## 9. Description

Human-readable parameter name.

Examples:

- WiFi Connection
- Engine Temperature
- Battery Voltage
- GPS Receiver

---

## 10. Source

Optional information describing where the data originates.

Examples:

- WiFi
- MQTT
- CAN
- NMEA2000
- BME280
- ADC

---

## 11. Unit

Optional engineering unit.

Examples:

- °C
- V
- A
- rpm
- %
- kn
- m

---

## 12. Dependencies

HealthCore shall not depend on:

- HealthManager
- MorseOutput
- SerialOutput
- OLED
- WiFi
- MQTT
- FreeRTOS tasks

---

## 13. Future Extensions

Reserved for:

- History
- Logging
- JSON export
- MQTT export
- OLED output
- Web UI
- SD card storage

---

## 14. Memory Policy

- Avoid unnecessary dynamic memory allocation.
- Avoid use of `new` and `delete`.
- Avoid mutable global objects.
- Prefer deterministic memory usage.
- Design for long-term reliable operation without memory fragmentation.
