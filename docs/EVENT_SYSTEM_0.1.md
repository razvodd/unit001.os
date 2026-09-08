# UNIT001 EVENT SYSTEM 0.1

## Identity

Every capture is an EVENT in one chronological sequence.

`YEAR / EVENT / ITEM`

- `YEAR` is the calendar year.
- `EVENT` is a permanent three-digit number from `001` to `999` within that year.
- `ITEM` is a permanent three-digit position within an event, from `001` to `999`.
- The event counter resets to `001` only at the start of a new year.
- An event ID and URL never change because of category, client, title, readiness, or filtering.

Examples:

`2026 / [001] / 001`

`001.jpg / 002.jpg / 003.mp4 / 004.wav`

Photo, video, audio, and data items all share the same linear ITEM sequence.

## URLs

- `/events` — event register; resolves to the latest listed year.
- `/events/2026` — register for one year.
- `/event/2026/001` — permanent public event URL for a physical QR or NFC card.

Future years use the same structure: `/events/2027` and `/event/2027/001`.

## Event record

One JSON file is the source of truth for one event:

`/web/data/events/2026/001.json`

The yearly index contains IDs only. It does not duplicate event metadata. The public register and single-event page load the same records.

Required metadata:

- `year`, `id`, `type`, `title`, `name`, `date`, `location`, `status`

Optional metadata:

- `readyDate`, `archiveUrl`, `spaceUrl`, `stats`, `items`

## States

`PROCESSING` is available immediately after capture. It may show previews, timeline items, a 3D space URL, and a planned ready date.

`READY` preserves the same URL and may show item count, shooting time, photos taken, a linear timeline, and `DOWNLOAD ARCHIVE`. The archive URL is external storage, currently intended for Yandex Disk.

## Register and filters

The register lists events in permanent numeric order. Each entry shows:

`[EVENT] / TYPE + NAME / DATE / LOCATION`

Filters are metadata-only: `ALL`, `MODEL`, `ACTOR`, `BAPTISM`, `FAMILY`, `EVENT`. Filtering never changes an event ID, its order, or URL.

## Timeline

The event page renders one sequential timeline. It does not create separate photo, video, or audio galleries. Placeholder items are intentionally empty rectangles with `X` for design/debug mode.

## Candidate decisions

The interface uses existing global candidate type values only: `TYPE/SIZE 0.75U`, `TYPE/SIZE 1.25U`, and `TYPE/LINE 1.5U`. All layout spacing uses the existing global `SPACE/*` tokens. No LOCKED grid or spacing values were changed.
