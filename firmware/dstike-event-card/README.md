# UNIT001 / EVENT CARD

Offline event details for a DSTIKE Deauther Watch SE. This sketch is a compact call-sheet for a photography day; it has no Wi-Fi scanning or network-control features.

## Screen flow

`NOW` → `WHERE` → `PLAN` → `CONTACT`

Use the lower button (`DOWN`) or middle button (`OK`) to go forward. Use `UP` to go back.

## Edit the event

Open `DSTIKE_Event_Card.ino` and replace the values in the **EVENT DATA** section. Keep names, addresses, and plan lines short: the OLED is 128 × 64 px.

## Upload

1. Install Arduino IDE and add the ESP8266 board package.
2. Install **U8g2 by olikraus** from Library Manager.
3. Select **NodeMCU 1.0 (ESP-12E Module)**, 80 MHz, 4 MB flash, and the watch's serial port.
4. Connect the watch by USB. If the port does not appear, hold the flash/reset button while connecting.
5. Upload `DSTIKE_Event_Card.ino`.

The configuration assumes SH1106 I²C OLED at `0x3C`, SDA GPIO4/D2, SCL GPIO5/D1, and buttons GPIO12/13/14 — the standard DSTIKE Watch configuration. Confirm visually before flashing: this firmware replaces the current `2.0.1` firmware.

## Restore

Keep a copy of the original `.bin` before uploading anything. You can later re-flash an official firmware image for your exact Watch/SE model.
