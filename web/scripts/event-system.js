const ROOT = "/";
const FILTERS = ["ALL", "MODEL", "ACTOR", "BAPTISM", "FAMILY", "EVENT"];

const getJson = async (path) => {
  const response = await fetch(`${ROOT}${path.replace(/^\//, "")}`);
  if (!response.ok) throw new Error(`Unable to load ${path}`);
  return response.json();
};

const formatDate = (date) => date.split("-").reverse().join(".");
const eventLabel = (event) => `${event.title} / ${event.name}`;
const eventPath = (event) => `/event/${event.year}/${event.id}/`;

const element = (tag, className, text) => {
  const node = document.createElement(tag);
  if (className) node.className = className;
  if (text !== undefined) node.textContent = text;
  return node;
};

const renderHeader = (root, eventsLink = true) => {
  const header = element("header", "event-system__header");
  const home = element("a", "event-system__home", "UNIT001.SYSTEMS");
  home.href = "/";
  header.append(home);
  if (eventsLink) {
    const events = element("a", "event-system__events-link", "EVENTS");
    events.href = "/events/";
    header.append(events);
  }
  root.append(header);
};

const renderYearNavigation = (root, years, currentYear) => {
  const nav = element("nav", "event-system__nav");
  nav.setAttribute("aria-label", "Event years");
  const list = element("div", "event-system__years");
  years.forEach((year) => {
    const link = element("a", "event-system__year", year);
    link.href = `/events/${year}/`;
    if (year === currentYear) link.setAttribute("aria-current", "page");
    list.append(link);
  });
  nav.append(list);
  root.append(nav);
};

const loadYearEvents = async (year) => {
  const index = await getJson(`/data/events/${year}/index.json`);
  const records = await Promise.all(index.events.map((id) => getJson(`/data/events/${year}/${id}.json`)));
  return records.sort((a, b) => Number(a.id) - Number(b.id));
};

const renderRegisterEntry = (event) => {
  const entry = element("a", "event-register__entry");
  entry.href = eventPath(event);
  entry.append(element("span", "event-register__id", `[${event.id}]`));
  const metadata = element("span", "event-register__metadata");
  metadata.append(
    element("span", "", eventLabel(event)),
    element("span", "", formatDate(event.date)),
    element("span", "", event.location)
  );
  entry.append(metadata);
  return entry;
};

const renderRegister = (root, events) => {
  const register = element("section", "event-register");
  const filters = element("div", "event-register__filters");
  filters.setAttribute("aria-label", "Event filters");
  const entries = element("div", "event-register__entries");

  const paint = (filter) => {
    entries.replaceChildren(...events
      .filter((event) => filter === "ALL" || event.type === filter)
      .map((event) => {
        const item = element("div", "event-register__item");
        item.append(renderRegisterEntry(event));
        return item;
      }));
    [...filters.querySelectorAll("button")].forEach((button) => {
      button.setAttribute("aria-pressed", String(button.value === filter));
    });
  };

  FILTERS.forEach((filter) => {
    const button = element("button", "event-filter", filter);
    button.type = "button";
    button.value = filter;
    button.addEventListener("click", () => paint(filter));
    filters.append(button);
  });
  register.append(filters, entries);
  root.append(register);
  paint("ALL");
};

const renderPlaceholder = () => element("div", "event-placeholder", "X");

const renderItemMedia = (item) => {
  if (!item.src) return renderPlaceholder();
  if (item.type === "VIDEO") {
    const video = element("video", "event-media");
    video.src = item.src;
    video.controls = true;
    return video;
  }
  if (item.type === "AUDIO") {
    const audio = element("audio", "event-media");
    audio.src = item.src;
    audio.controls = true;
    return audio;
  }
  const image = element("img", "event-media");
  image.src = item.src;
  image.alt = `Event item ${item.id}`;
  return image;
};

const renderTimeline = (root, event) => {
  if (!event.items.length) return;
  const section = element("section", "event-timeline");
  section.setAttribute("aria-label", "Event timeline");
  const list = element("ol", "event-timeline__list");
  event.items.forEach((item) => {
    const row = element("li", "event-timeline__item");
    row.append(element("span", "event-timeline__item-number", item.id));
    const content = element("div", "event-timeline__item-content");
    content.append(element("p", "", `[${item.type}]`));
    content.append(renderItemMedia(item));
    row.append(content);
    list.append(row);
  });
  section.append(list);
  root.append(section);
};

const renderEvent = async (root) => {
  const [, , year, id] = window.location.pathname.split("/");
  const event = await getJson(`/data/events/${year}/${id}.json`);
  renderHeader(root);

  const top = element("section", "event-single__top");
  const eventId = element("p", "event-single__id", `[${event.id}]`);
  const metadata = element("div", "event-single__metadata");
  metadata.append(
    element("p", "", eventLabel(event)),
    element("p", "", formatDate(event.date)),
    element("p", "", event.location)
  );
  top.append(eventId, metadata);
  root.append(top);

  const state = element("section", "event-single__state");
  state.setAttribute("aria-label", "Event status");
  state.append(element("p", "", event.status));
  if (event.status === "PROCESSING" && event.readyDate) {
    state.append(element("p", "", `READY / ${formatDate(event.readyDate)}`));
  }
  root.append(state);

  if (event.status === "READY") {
    const stats = element("section", "event-single__stats");
    stats.setAttribute("aria-label", "Event statistics");
    if (event.stats.items !== null) stats.append(element("p", "", `${event.stats.items} ITEMS`));
    if (event.stats.shootingTime) stats.append(element("p", "", `${event.stats.shootingTime} SHOOTING TIME`));
    if (event.stats.photosTaken !== null) stats.append(element("p", "", `${event.stats.photosTaken} PHOTOS TAKEN`));
    root.append(stats);
    if (event.archiveUrl) {
      const archive = element("a", "event-single__archive", "DOWNLOAD ARCHIVE");
      archive.href = event.archiveUrl;
      archive.target = "_blank";
      archive.rel = "noreferrer";
      root.append(archive);
    }
  }

  if (event.spaceUrl) {
    const space = element("a", "event-single__archive", "3D SPACE");
    space.href = event.spaceUrl;
    space.target = "_blank";
    space.rel = "noreferrer";
    root.append(space);
  }
  renderTimeline(root, event);
};

const renderIndex = async (root) => {
  const registry = await getJson("/data/events/index.json");
  const requestedYear = root.dataset.year;
  const year = requestedYear || registry.years.at(-1);
  renderHeader(root, false);
  renderYearNavigation(root, registry.years, year);
  const events = await loadYearEvents(year);
  renderRegister(root, events);
};

const main = document.querySelector("[data-event-system]");
if (main) {
  main.classList.add("event-system");
  const render = main.dataset.eventSystem === "single" ? renderEvent : renderIndex;
  render(main).catch(() => {
    main.replaceChildren(element("p", "", "EVENT NOT FOUND"));
  });
}
