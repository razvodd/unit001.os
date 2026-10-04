# UNIT001 WATCH - Calendar sync

The watch keeps the next 10 timed events in its own memory. It downloads them
from one separate Google Calendar, then turns Wi-Fi off.

## One-time calendar bridge

1. Create a Google Calendar named exactly: UNIT001 WATCH.
2. Add normal timed events with short Latin titles: MODEL TEST, ANNA, BAPTISM.
   All-day events are ignored.
3. On a computer signed into that Google account open https://script.new.
4. Replace starter code with calendar-bridge/Code.gs from this folder.
5. Deploy - New deployment - Web app. Select Execute as: Me and access: Anyone.
6. Copy the Web app URL and keep it private.

## One-time watch setup

1. In Arduino Library Manager install QRCode by Richard Moore, then upload
   DSTIKE_Event_Card.ino.
2. Hold the watch OK button for 3 seconds.
3. On the phone connect to Wi-Fi UNIT001 SETUP. Password: unit001watch.
4. Open http://192.168.4.1.
5. Enter home Wi-Fi and the Web app URL, then press SAVE.

The watch downloads the next 10 events after setup and then no more than once
per 6 hours near the saved Wi-Fi. Away from Wi-Fi it continues from the saved
schedule. To refresh after calendar edits now, restart it near home Wi-Fi.

Short OK: NEXT EVENT - QR - UNIT001 SYSTEM.
Hold OK 3 seconds: phone setup.
