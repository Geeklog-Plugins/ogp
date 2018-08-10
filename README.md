# ogp
The OGP (Open Graph Protocol) plugin for Geeklog allows you to output <og:> tags for SNS like Facebook.
The OGP plugin supports <a href="http://ogp.me/">Open Graph Protocol</a>(OGP) and allows you to run your Geeklog site 
in cooperation with web sites supporting OGP such as <a href="https://developers.facebook.com/docs/opengraph/">Facebook</a>
by producing &lt;meta property="og:***" content="***"&gt; tags.

Besides, the plugin allows you to display Facebook Like buttons and/or Facebook comments in your articles, static pages,
calendar events, link categories, poll items and download files.  You can set the kinds of contents in which to show
Facebook Like button and/or Facebook comments in Configuration.

## System requirements
- Geeklog-1.6.0 or newer (compatible with Geeklog-2.2.0)
- PHP-5.0.0 or newer

## Before installation
	
Confirm your Facebook user ID.  In case you don't know your ID, <a href="https://apps.facebook.com/what-is-my-user-id/">This application</a> is handy.
If you want to use Facebook Like button or Facebook comments, you have to acquire an application ID at <a href="https://developers.facebook.com/apps">Facebook Developers</a> like this.

1. Click on <strong>+ Create New App</strong>.
2. In the dialog, enter some string (which DOES NOT include "Facebook") as "App Name", select your locale, check "Facebook terms of use" 
and click on "continue".
3. In the following screen, you have to get security check.  Enter two words in the CAPTCHA dialog.  If words are hard to discern, then keep clicking "display other words" till you get ones you can recognize.
4. In the following screen, write down your <strong>App ID</strong>.  Next, click on <strong>Website</strong> just below <strong>Select how your app integrates with Facebook</strong> and enter your site URL and save.
	
## Install 

Please see "install.html" under admin/docs directory for detail.
