# OGP

The OGP (Open Graph Protocol) plugin for Geeklog centralizes social metadata
used by Facebook, LinkedIn, X/Twitter and other services when a page URL is
shared.

OGP outputs Open Graph metadata for Geeklog pages and provides an optional
provider API, `OGP_registerSocialMetadata()`, so content plugins can supply
authoritative page-level social metadata without duplicating rendering logic.

## System requirements

- Geeklog 1.6.0 or newer
- PHP 5.6.4 or newer (including PHP 8.1/8.3)

## OGP 2.0 scope

OGP 2.0 focuses on one responsibility: social metadata.

It provides:

- Open Graph metadata;
- Twitter/X card metadata for registered providers;
- image and rich-video social metadata;
- a default social image fallback;
- backward-compatible Geeklog page detection when no provider participates.

It does not provide sharing buttons, reactions, Facebook Like widgets or
Facebook Comments widgets. Those legacy features were removed in 2.0.

Themes such as Eclipse may provide Facebook, X/Twitter or LinkedIn share
buttons independently. Those buttons are complementary to OGP: the theme
initiates the share, while OGP controls the metadata read by the destination
service.

## Interoperability

Compatible content plugins may call `OGP_registerSocialMetadata()` before
Geeklog renders the final document header.

No content plugin is required. If no provider registers metadata, OGP follows
its historical rendering path.

## Install

Install the plugin through Geeklog's Plugin Administration using the
installable archive.
