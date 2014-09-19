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
/* global Keen */
/* global http */
/* global io */
/* global AppInfo */
/* global Timers */

(function () {

  var socket = null;
  // var timers = null;

  try {
    Pebble.addEventListener('ready', ready);
    Pebble.addEventListener('showConfiguration', showConfiguration);
    Pebble.addEventListener('webviewclosed', webviewClosed);
    Pebble.addEventListener('appmessage', appMessage);
  }
  catch (ex) {
    handleError(ex);
  }

  function ready() {
    try {
      Keen.init(http, Pebble, AppInfo.config.keen, AppInfo, AppInfo.debug);
      socket = io.connect(AppInfo.config.socketUrl);
      Timers.init({
        delimiters: AppInfo.delimiters,
        socket: socket
      });
    }
    catch (ex) {
      handleError(ex);
    }
  }

  function showConfiguration() {
    try {
      Pebble.openURL(AppInfo.config.settingsUrl + Pebble.getAccountToken() + '?version=' + AppInfo.versionLabel);
    }
    catch (ex) {
      handleError(ex);
    }
  }

  function webviewClosed() {
  }

  function appMessage(event) {
    switch (event.data.group) {
      case 'ANL':
        handleAnalytics(event.data.operation, event.data.data);
        break;
      default:
        console.log(JSON.stringify(event.data));
    }
  }

  function handleAnalytics(operation, dataStr) {
    try {
      var data = {};
      dataStr.split(AppInfo.delimiters.outer).forEach(function (str) {
        if (! str.length) {
          return;
        }
        var item = str.split(AppInfo.delimiters.inner);
        if (item.length < 2) {
          return;
        }
        data[item[0]] = item[1];
      });
      Keen.sendEvent(operation, data);
    }
    catch (ex) {
      handleError(ex);
    }
  }

  function handleError(exception) {
    console.log(JSON.stringify(exception));
    Keen.sendEvent('error', { message: exception.toString() });
  }

}());