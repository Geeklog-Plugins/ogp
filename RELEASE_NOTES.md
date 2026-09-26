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

Provider-supplied or locally detectable image dimensions are emitted on the modern provider path. The historical OGP path retains its previous image-dimension behavior.

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
