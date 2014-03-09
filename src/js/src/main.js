/* globals Pebble */
/* globals AppInfo */
/* globals VersionChecker */

var MultiTimer = function () {

  Pebble.addEventListener('ready', function () {
    VersionChecker.check(AppInfo);
  });

};

if (window.Pebble) {
  MultiTimer();
}