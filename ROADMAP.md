# OGP Plugin Roadmap

## Target

Modernize the OGP plugin for Geeklog 2.2.2 and PHP 8.1+ while preserving its historical Facebook features.

The long-term role of OGP is to become Geeklog's central social metadata renderer:

- Open Graph metadata;
- Twitter/X Cards;
- Facebook metadata and optional Facebook widgets;
- shared fallback metadata for all Geeklog content;
- integration with modern Geeklog plugin interoperability.

OGP must remain optional. Other plugins must continue to work without it.

---

## Architecture principles

### 1. OGP owns social metadata rendering

OGP should be the component responsible for rendering social metadata in the page `<head>`:

- `og:site_name`;
- `og:url`;
- `og:type`;
- `og:locale`;
- `og:title`;
- `og:description`;
- `og:image`;
- `og:image:width`;
- `og:image:height`;
- Twitter/X card metadata;
- Facebook-specific metadata such as `fb:app_id` and `fb:admins`.

The existing `plugin_getheadercode_ogp()` remains the central injection point.

### 2. Content plugins own their business metadata

OGP should not know the internal database structure of modern plugins.

Each plugin should remain responsible for determining:

- title;
- description;
- canonical content URL;
- main image or thumbnail;
- content subtype;
- permissions;
- plugin-specific structured data.

OGP consumes those values and renders social metadata.

### 3. Reuse Geeklog interoperability

Prefer existing Geeklog interoperability mechanisms instead of adding OGP-specific APIs.

Where available, consume:

- `plugin_getiteminfo_PLUGIN()`;
- `plugin_idtourl_PLUGIN()`;
- plugin services exposed through `PLG_invokeService()`;
- standard item identifiers and plugin subtypes.

Avoid direct SQL queries into third-party plugin tables.

### 4. Keep Schema.org in content plugins

OGP must not become a generic structured-data engine.

Schema.org remains owned by the plugin that understands the content model.

Examples:

- Maps: `Place`, `GeoCoordinates`, `Restaurant`, `Hotel`;
- Videos: `VideoObject`;
- Documents: `CreativeWork` / document-specific data;
- MediaGallery: media-specific structured data;
- Forum: discussion-specific structured data.

### 5. Preserve historical Facebook features

Do not remove existing Facebook features during the modernization.

Keep support for:

- Facebook App ID;
- Facebook Admin IDs;
- Facebook SDK loading;
- Facebook Like button;
- Facebook Comments;
- existing content-type configuration.

These features should become optional integration modules rather than define the core architecture.

---

## Phase 1 — Baseline audit

- [ ] Audit current plugin version, install/update paths and configuration.
- [ ] Confirm compatibility with Geeklog 2.2.2.
- [ ] Confirm PHP 8.1 and PHP 8.3 compatibility.
- [ ] Inventory all current Open Graph output.
- [ ] Inventory all Facebook SDK, Like and Comments code.
- [ ] Inventory all direct SQL access to Geeklog core and plugin tables.
- [ ] Inventory URL-based content detection.
- [ ] Identify obsolete Facebook configuration without removing it.
- [ ] Document current behavior before refactoring.

---

## Phase 2 — Internal metadata model

Introduce a normalized internal metadata structure.

Suggested fields:

```php
[
    'title'        => '',
    'description'  => '',
    'url'          => '',
    'type'         => 'website',
    'locale'       => '',
    'image'        => '',
    'image_width'  => 0,
    'image_height' => 0,
    'mime'         => '',
    'plugin'       => '',
    'item_id'      => '',
    'subtype'      => '',
]
```

- [ ] Create internal metadata normalization helpers.
- [ ] Separate metadata collection from HTML rendering.
- [ ] Keep output escaping centralized.
- [ ] Normalize absolute URLs.
- [ ] Normalize image URLs.
- [ ] Normalize empty and fallback values.
- [ ] Add a clear precedence order for metadata sources.

Recommended precedence:

1. plugin-provided metadata;
2. Geeklog core content metadata;
3. page-level metadata already available from Geeklog;
4. OGP configured defaults;
5. site-wide Geeklog defaults.

---

## Phase 3 — Modern Geeklog content discovery

Replace hard-coded knowledge of other plugins where possible.

- [ ] Detect the current Geeklog content/plugin context.
- [ ] Use `plugin_getiteminfo_PLUGIN()` where available.
- [ ] Use `plugin_idtourl_PLUGIN()` for canonical item URLs where appropriate.
- [ ] Use plugin services when Item Info does not expose enough data.
- [ ] Keep permissions enforced by the source plugin.
- [ ] Never bypass plugin permissions with direct table reads.
- [ ] Support namespaced identifiers such as `marker:<id>`.
- [ ] Document the minimum metadata expected from modern plugins.

Initial target plugins:

- [ ] Maps;
- [ ] Documents;
- [ ] Videos;
- [ ] MediaGallery;
- [ ] Forum.

---

## Phase 4 — Legacy compatibility layer

The modern interoperability path must not break historical Geeklog content.

Keep fallback support for:

- [ ] Geeklog stories/articles;
- [ ] topics;
- [ ] Static Pages;
- [ ] Calendar;
- [ ] CalendarJP where still supported;
- [ ] Links;
- [ ] Polls;
- [ ] FileMgmt / Downloads where applicable.

Refactor legacy support behind dedicated adapters so it is isolated from the new metadata engine.

Goal:

```text
Modern plugin metadata available
        |
        +--> use modern interoperability

Modern metadata unavailable
        |
        +--> use legacy OGP adapter
```

---

## Phase 5 — Open Graph renderer

Refactor Open Graph generation into a dedicated renderer.

- [ ] Keep `og:site_name`.
- [ ] Keep `og:url`.
- [ ] Keep `og:type`.
- [ ] Keep `og:locale`.
- [ ] Keep `og:title`.
- [ ] Keep `og:description`.
- [ ] Keep `og:image`.
- [ ] Keep `og:image:width`.
- [ ] Keep `og:image:height`.
- [ ] Add `og:image:type` when MIME information is available.
- [ ] Add optional alternate locales when reliable data exists.
- [ ] Avoid duplicate Open Graph tags on the same page.
- [ ] Ensure all output is escaped consistently.

---

## Phase 6 — Twitter/X Cards

Add first-class Twitter/X Card support.

- [ ] Add `twitter:card`.
- [ ] Add `twitter:title`.
- [ ] Add `twitter:description`.
- [ ] Add `twitter:image`.
- [ ] Support `summary`.
- [ ] Support `summary_large_image`.
- [ ] Choose the card type based on available media.
- [ ] Add optional site/account configuration if useful.
- [ ] Do not make X/Twitter configuration mandatory.

---

## Phase 7 — Facebook integration preservation

Keep all historical Facebook capabilities but isolate them from the metadata core.

### Facebook metadata

- [ ] Preserve `fb:app_id`.
- [ ] Preserve `fb:admins`.
- [ ] Validate configuration safely.

### Facebook SDK

- [ ] Load the SDK only when a Facebook widget actually requires it.
- [ ] Avoid loading the SDK for pure Open Graph output.
- [ ] Keep locale handling.
- [ ] Review SDK version and loading method.

### Facebook Like

- [ ] Preserve Like button support.
- [ ] Preserve existing layout options where still supported.
- [ ] Gracefully handle options no longer supported by Facebook.
- [ ] Avoid breaking existing installations during upgrade.

### Facebook Comments

- [ ] Preserve Facebook Comments support.
- [ ] Preserve configuration where still functional.
- [ ] Do not interfere with Geeklog native comments.
- [ ] Keep Facebook Comments optional.

---

## Phase 8 — Default social image handling

Improve site-wide fallback image behavior.

- [ ] Keep a configurable default social image.
- [ ] Validate that the image URL is absolute.
- [ ] Detect dimensions when locally available.
- [ ] Avoid warnings when remote images cannot be inspected.
- [ ] Support plugin-provided dimensions to avoid unnecessary reads.
- [ ] Prefer plugin thumbnail/preview images over site defaults.
- [ ] Document recommended social image dimensions without enforcing them.

---

## Phase 9 — Integration with Maps

Maps is the first plugin where duplicate social metadata already exists.

Maps should continue to own:

- canonical URLs;
- redirects;
- robots;
- meta descriptions;
- sitemap integration;
- Schema.org;
- 404 handling;
- map/marker SEO titles.

OGP should consume Maps item metadata and render:

- Open Graph;
- Twitter/X;
- Facebook metadata.

Migration plan:

- [ ] Confirm Maps Item Info exposes map and marker title/description/URL.
- [ ] Extend Maps Item Info with image metadata where useful.
- [ ] Let OGP consume namespaced marker identifiers.
- [ ] Keep Maps current OG/Twitter output as temporary fallback.
- [ ] Add duplicate-tag detection.
- [ ] Disable Maps OG/Twitter output when OGP is active and confirmed compatible.
- [ ] Keep Maps fallback behavior when OGP is absent.

---

## Phase 10 — Integration with Documents

- [ ] Expose document title.
- [ ] Expose description.
- [ ] Expose canonical document page URL.
- [ ] Expose preview image where available.
- [ ] Expose MIME type where useful.
- [ ] Respect document permissions.
- [ ] Use the site default image when no preview exists.

OGP must not read Documents tables directly.

---

## Phase 11 — Integration with Videos

- [ ] Expose video title.
- [ ] Expose description.
- [ ] Expose canonical video page URL.
- [ ] Expose thumbnail.
- [ ] Expose MIME/source metadata where useful.
- [ ] Respect video permissions.
- [ ] Keep `VideoObject` and video-specific Schema.org inside Videos.

OGP should render the social preview only.

---

## Phase 12 — Integration with MediaGallery

- [ ] Expose album metadata.
- [ ] Expose individual media metadata.
- [ ] Expose canonical URLs.
- [ ] Expose main image/thumbnail.
- [ ] Expose image dimensions where known.
- [ ] Expose MIME type.
- [ ] Respect album/media permissions.
- [ ] Prefer original social-worthy image or an appropriate generated rendition.

OGP must not bypass MediaGallery ACLs.

---

## Phase 13 — Integration with Forum

- [ ] Expose topic title.
- [ ] Expose a safe excerpt of the first visible post.
- [ ] Expose canonical topic URL.
- [ ] Expose topic image only when explicitly supported.
- [ ] Fall back to site social image otherwise.
- [ ] Respect forum/category/topic permissions.
- [ ] Never expose private topic metadata to unauthorized visitors.

---

## Phase 14 — Duplicate metadata prevention

Centralization is only useful if duplicates are avoided.

- [ ] Detect duplicate OGP-generated tags.
- [ ] Define how OGP coexists with plugin-generated social tags during migration.
- [ ] Provide a feature flag for compatibility mode.
- [ ] Document which plugin versions can delegate social metadata to OGP.
- [ ] Add diagnostics for duplicate Open Graph/Twitter tags.
- [ ] Never remove canonical or Schema.org metadata owned by content plugins.

---

## Phase 15 — Administration and configuration

Reorganize configuration into clear sections.

Suggested structure:

### Social Metadata

- Enable Open Graph;
- Enable Twitter/X Cards;
- default social image;
- fallback description behavior;
- card type preference.

### Facebook Integration

- Facebook App ID;
- Facebook Admin IDs;
- load Facebook SDK;
- Facebook Like;
- Facebook Comments;
- legacy widget options.

### Compatibility

- modern interoperability;
- legacy content adapters;
- duplicate protection;
- debug/diagnostic mode.

- [ ] Preserve existing configuration values during upgrade.
- [ ] Add migration logic only where needed.
- [ ] Avoid resetting historical Facebook settings.

---

## Phase 16 — Diagnostics

Add a lightweight diagnostic mode for administrators.

Possible diagnostics:

- detected content provider;
- item identifier;
- metadata source;
- selected title;
- selected description;
- selected image;
- selected canonical URL;
- fallback reason;
- duplicate metadata warning.

Do not expose diagnostics publicly.

---

## Phase 17 — Security and privacy

- [ ] Escape every metadata value.
- [ ] Validate URLs.
- [ ] Avoid arbitrary HTML in metadata.
- [ ] Respect source-plugin ACLs.
- [ ] Do not expose hidden/private content metadata.
- [ ] Avoid SSRF-style remote image probing.
- [ ] Prefer plugin-provided image dimensions for remote resources.
- [ ] Review Facebook SDK privacy implications and document them.

---

## Phase 18 — Multisite support

Ensure configuration remains site-specific in Geeklog multisite deployments.

- [ ] Test separate site URLs.
- [ ] Test separate social images.
- [ ] Test different Facebook App IDs per site.
- [ ] Test different locales.
- [ ] Avoid shared-state assumptions across sites.
- [ ] Verify compatibility with multi-database Geeklog setups.

---

## Phase 19 — Tests

Minimum test matrix:

### Geeklog

- [ ] Geeklog 2.2.2.

### PHP

- [ ] PHP 8.1;
- [ ] PHP 8.2;
- [ ] PHP 8.3.

### Content

- [ ] homepage;
- [ ] story;
- [ ] topic;
- [ ] Static Page;
- [ ] Maps map;
- [ ] Maps marker;
- [ ] Document;
- [ ] Video;
- [ ] MediaGallery album;
- [ ] MediaGallery media;
- [ ] Forum topic.

### Social output

- [ ] Open Graph title;
- [ ] description;
- [ ] URL;
- [ ] image;
- [ ] image dimensions;
- [ ] Twitter/X Card;
- [ ] Facebook App ID;
- [ ] Facebook Admin IDs.

### Compatibility

- [ ] OGP installed;
- [ ] OGP disabled;
- [ ] source plugin without modern Item Info;
- [ ] source plugin with modern Item Info;
- [ ] no duplicate metadata.

---

## Phase 20 — Documentation

- [ ] Rewrite README around the new role of OGP.
- [ ] Document retained Facebook features.
- [ ] Document modern plugin interoperability.
- [ ] Document fallback behavior.
- [ ] Document how third-party plugins can expose metadata.
- [ ] Document Maps/Documents/Videos/MediaGallery/Forum integration.
- [ ] Document migration from older OGP versions.
- [ ] Document privacy considerations for Facebook widgets.

---

## Release strategy

### OGP 1.x maintenance

Keep current behavior stable for existing users.

Only critical fixes should be backported once the 2.x development line is established.

### OGP 2.0

Target:

- Geeklog 2.2.2;
- PHP 8.1+;
- modern plugin interoperability;
- centralized Open Graph rendering;
- Twitter/X Cards;
- preserved Facebook Like/Comments integration;
- no mandatory dependency for other plugins.

---

## Non-goals

OGP 2.0 should not become:

- a full SEO plugin;
- a sitemap generator;
- a canonical URL engine;
- a Schema.org generator for every plugin;
- a replacement for content-plugin permissions;
- a direct database consumer of modern third-party plugins;
- a mandatory dependency for Geeklog plugins.

Its scope should remain focused:

> collect trusted social metadata from Geeklog content and plugins, then render consistent Open Graph, Twitter/X and optional Facebook integration across the site.
