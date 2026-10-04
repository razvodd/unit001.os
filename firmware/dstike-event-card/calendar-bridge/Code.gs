/*
 UNIT001 WATCH / GOOGLE CALENDAR BRIDGE
 Deploy this Apps Script as a Web app.
 Output: YYYY-MM-DD|HH:MM|TITLE
*/
const CALENDAR_NAME = 'UNIT001 WATCH';
const TIME_ZONE = 'Europe/Moscow';
function doGet() {
  const calendars = CalendarApp.getCalendarsByName(CALENDAR_NAME);
  if (!calendars.length) return output_('ERROR|CALENDAR_NOT_FOUND');
  const now = new Date();
  const until = new Date(now.getTime() + 90 * 24 * 60 * 60 * 1000);
  const rows = calendars[0].getEvents(now, until)
    .filter(e => !e.isAllDayEvent() && e.getStartTime().getTime() >= now.getTime())
    .sort((a, b) => a.getStartTime() - b.getStartTime())
    .slice(0, 10)
    .map(e => Utilities.formatDate(e.getStartTime(), TIME_ZONE, 'yyyy-MM-dd') + '|' +
      Utilities.formatDate(e.getStartTime(), TIME_ZONE, 'HH:mm') + '|' +
      e.getTitle().toUpperCase().replace(/[|\r\n]/g, ' ').slice(0, 12));
  return output_(rows.join('\n'));
}
function output_(text) {
  return ContentService.createTextOutput(text).setMimeType(ContentService.MimeType.TEXT);
}
