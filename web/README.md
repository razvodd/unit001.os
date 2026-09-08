# UNIT001 Web Layer

Import `styles/index.css` once at the web entry point.

The page shell is `.u-page > .u-page__frame`. Its header, main area, and footer
use the locked five-column grid. Use `.u-span-1` through `.u-span-5` for
content width; do not create local spacing or grid values.

## Candidate values in this layer

The typography scale (`0.75U`, `1U`, `1.25U`) and leading (`1U`, `1.25U`,
`1.5U`, `2U`) are candidate values documented in the grid specification. They
are exposed for visual testing and are not locked by this implementation.

## Preview

Open `index.html` in a browser. It is the public UNIT001.SYSTEMS TRACE screen:
one status surface with cursor coordinates, a short fading pointer trace, and
email. It has no external dependencies.

The 13px system type size, its 1.45 line-height, and the page-specific 2U inset
are candidate web values. All spacing derives from the global UNIT001 `U`
tokens.

## Event system

`/events` is the public event register. `/events/2026` is its yearly view and
`/event/2026/001` is a permanent event address for a physical QR or NFC card.

Event records live in `/data/events/<year>/<id>.json`. The register loads those
records directly, so event metadata is not duplicated between the register and
single-event pages. The full EVENT model and candidate-value record are in
`/docs/EVENT_SYSTEM_0.1.md`.
