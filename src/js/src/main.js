/* globals Pebble */
/* globals AppInfo */

var MultiTimer = function () {

  Pebble.addEventListener('ready', function () {
  });

  Pebble.addEventListener('showConfiguration', function () {
    // Pebble.openURL('http://pblweb.com/hearts/app/config/?version=' + AppInfo.versionLabel);
  });

};

if (window.Pebble) {
  MultiTimer();
}