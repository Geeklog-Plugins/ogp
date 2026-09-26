# OGP 2.0.0 release notes

OGP 2.0.0 modernizes social metadata interoperability while preserving the historical Open Graph behavior of the plugin when no modern content provider participates.

## Compatibility

- Geeklog 1.6.0 or newer.
- PHP 5.6.4 or newer.
- Intended validation range for this release: PHP 5.6, 8.1 and 8.3.
- No hard dependency on Maps, Documents, Videos, MediaGallery, Hub, Agent or another optional plugin.

The Geeklog 1.6.0 minimum is retained from the established OGP compatibility line. The PHP minimum is aligned with the maintained OGP/Geeklog baseline used by current Geeklog releases.

## Provider interoperability

OGP 2.0.0 adds the optional `OGP_registerSocialMetadata()` provider API.

A content-owning plugin may register authoritative page social metadata before Geeklog builds the final document header. Supported fields include title, description, canonical URL, Open Graph type, image metadata, Twitter card metadata and rich video metadata.

The first successful registration wins for a request.

When no provider registers metadata, OGP follows its historical URL detection and rendering path. Existing sites therefore do not need any of the new provider plugins installed, enabled or upgraded.

## Rich media

The provider contract supports:

- `og:image:alt`;
- `og:video`;
- `og:video:secure_url`;
- `og:video:type`;
- Twitter/X card, title, description, image and image alt metadata.

Provider-supplied or locally detectable image dimensions are emitted on the modern provider path. The backward-compatible legacy detection path also emits Twitter/X cards, detects local image dimensions, and rejects undersized preview images in favor of the configured default social image.

## Legacy page-detection improvements

The historical Geeklog page-detection path remains available when no provider registers metadata and has been modernized for the 2.0.0 release:

- articles prefer the dedicated `page_title` when available, with the article title as fallback;
- Static Pages prefer `sp_page_title`, with `sp_title` as fallback;
- topic archive pages use Open Graph type `website`;
- article, Static Page and topic descriptions reuse their configured SEO meta descriptions when available;
- Twitter/X cards are emitted alongside Open Graph metadata;
- local image dimensions are detected and emitted;
- very small topic/content images are rejected as social previews and replaced by the configured default social image;
- article SEO-title support remains compatible with Geeklog 1.6.0 by only reading the `page_title` column on Geeklog 1.7.0 or newer.

The legacy path was manually validated on Geeklog 2.1.1 for articles, topic archives and Static Pages, and on Geeklog 2.2.2 for the current Static Pages behavior.

## Legacy social widgets removed

OGP 2.0 removes the obsolete Facebook Like and Facebook Comments integration,
including the Facebook JavaScript SDK loader, Like/Comments autotags, template
variables and their configuration fields.

Sharing UI is intentionally outside OGP's scope. A theme such as Eclipse can
continue to provide Facebook, X/Twitter and LinkedIn share buttons while OGP
provides the metadata those services read from the shared URL.

Existing installations are cleaned during the 1.2.3 to 2.0.0 upgrade. The
historical default social image setting is retained to avoid an unnecessary
configuration migration.

## Upgrade

Upgrades from OGP 1.2.3 to 2.0.0 require no database schema or persisted configuration migration. The plugin version is advanced through the normal Geeklog plugin upgrade mechanism.

Unknown installed versions now stop the upgrade cleanly instead of risking an endless upgrade loop.

## Release engineering

The `develop-2.0` branch validates all shipped PHP and INC files on PHP 5.6, 8.1 and 8.3 before packaging.

Successful branch builds automatically create and commit:

`dist/ogp_2.0.0_1.6.0.zip`

plus its SHA-256 checksum.

The archive contains a single top-level `ogp/` directory and excludes repository-only files such as `.git`, `.github`, `dist` and build working directories.

## Architecture

In line with the Geeklog Memorandum interoperability principles:

- OGP owns social metadata rendering;
- content plugins remain authoritative for their content, canonical URL and Schema.org structured data;
- OGP contains no plugin-specific SQL or hard-coded dependency on optional providers;
- provider integration is opt-in and backward compatible;
- static metadata is exposed through `plugin.json`.
