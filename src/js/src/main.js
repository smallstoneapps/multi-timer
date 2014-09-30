/* global AppInfo */
/* global Analytics */
/* global Pebble */

var MultiTimer = (function () {

  var analytics = null;

  function start() {
    analytics = new Analytics(AppInfo.googleAnalytics.trackingId, AppInfo.shortName, AppInfo.versionLabel);
    if (! AppInfo.debug) { analytics.trackEvent('app', 'start'); }
  }

  return {
    start: start
  };

}());

Pebble.addEventListener('ready', MultiTimer.start);