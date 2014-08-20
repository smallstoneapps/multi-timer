/*

Multi Timer v3.0
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)

Copyright © 2013 - 2014 Matthew Tole

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

--------------------

src/js/src/main.js

*/

/* global Pebble */
/* global Analytics */
/* global Keen */
/* global http */
/* global io */
/* global Config */
/* global AppInfo */
/* global Timers */

(function () {

  var ga = null;
  var socket = null;
  var timers = null;
  var firstTime = localStorage.getItem('run-check') === null;
  localStorage.setItem('run-check', true);

  Pebble.addEventListener('ready', function () {
    try {
      ga = new Analytics(Config.googleAnalytics, AppInfo.shortName, AppInfo.versionLabel);
      Keen.init(http, Pebble, Config.keen, AppInfo);
      socket = io.connect(Config.socketUrl);
      timers = new Timers(Config, socket);
      ga.trackEvent('app', 'run');
    }
    catch (ex) {
      Keen.sendEvent('error', { message: ex.toString() });
    }
  });

  Pebble.addEventListener('showConfiguration', function () {
    try {
      Pebble.sendAppMessage({ group: 'TMR', operation: 'LIST', data: '' });
      Pebble.openURL(Config.settingsUrl + Pebble.getAccountToken() + '?version=' + AppInfo.versionLabel + '&firstTime=' + firstTime);
    }
    catch (ex) {
      Keen.sendEvent('error', { message: ex.toString() });
      console.log(ex);
    }
  });

  Pebble.addEventListener('webviewclosed', function () {
    firstTime = false;
  });

  Pebble.addEventListener('appmessage', function (event) {
    switch (event.data.group) {
      case 'TMR':
        if (timers) {
          timers.handleMessage(event.data.operation, event.data.data);
        }
        break;
      default:
        console.log(JSON.stringify(event.data));
    }
  });

}());