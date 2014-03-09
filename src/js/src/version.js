/* globals Pebble */
/* exported VersionChecker */

var VersionChecker = (function () {

  return {
    check: check
  };

  function check(app) {
    var lastCheck = window.localStorage.getItem('version-check', 0);
    if (lastCheck >= app.versionCode) {
      return;
    }
    var url = 'http://pblweb.com/api/v1/version/' + app.uuid + '.json?current=' + app.versionLabel;
    var req = new XMLHttpRequest();
    req.open('GET', url, true);
    req.onload = function () {
      if (req.readyState === 4 && req.status === 200) {
        if (req.status === 200) {
          var response = JSON.parse(req.responseText);
          window.localStorage.setItem('version-check', app.versionCode);
          if (response.newer) {
            Pebble.showSimpleNotificationOnPebble('Multi Timer ' + response.version, 'There is a new version of Multi Timer on the Pebble appstore.\nRemove and reload this app from your locker to update.');
          }
        }
      }
    };
    req.send(null);
  }

}());