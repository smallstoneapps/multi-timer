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

src/js/src/timers.js

*/

/* global Timer */
/* global MessageQueue */
/* exported Timers */

var Timers = (function () {

  var timers = [];
  var updateTime = 0;
  var delimInner = '';
  var delimOuter = '';
  var socket = null;

  function init(options) {
    delimOuter = options.delimiters.outer;
    delimInner = options.delimiters.inner;
    Pebble.addEventListener('appmessage', handleMessage);
    socket = options.socket;
    socket.on('connect', socketConnect);
    socket.on('label', socketLabel);

    loadTimers();
    getTimers();
  }

  return {
    init: init
  };

  function handleMessage(event) {
    if (event.data.group !== 'TMR') {
      return;
    }
    switch (event.data.operation) {
      case 'LIST?':
        sendTimers();
        break;
      case 'LIST!':
        setTimers(event.data.data);
        break;
    }
  }

  function loadTimers() {
    var timerString;
    if (! (timerString = localStorage.getItem('timers'))) {
      return;
    }
    timers = [];
    try {
      var storedTimers = JSON.parse(timerString);
      console.log(timerString);
      storedTimers.forEach(function (timer) {
        timers.push(new Timer(timer));
      });
    }
    catch (ex) {
      console.log(JSON.stringify(ex));
    }
  }

  function saveTimers() {
    localStorage.setItem('timers', JSON.stringify(timers.map(function (timer) {
      return timer.toObject();
    })));
  }

  function getTimers() {
    MessageQueue.sendAppMessage({ group: 'TMR', operation: 'LIST?', data: '' });
  }

  function sendTimers() {
    var timerStr = timers.map(function (timer) {
      return timer.toString(delimInner);
    }).join(delimOuter);
    var message = [ updateTime.toString(), timers.length, timerStr ].join(delimOuter);
    MessageQueue.sendAppMessage({ group: 'TMR', operation: 'LIST!', data: message });
  }

  function setTimers(data) {
    timers = [];
    var pieces = data.split(delimOuter);
    pieces.forEach(function (piece) {
      var timer = Timer.parse(piece, delimInner);
      if (timer) {
        timers.push(timer);
      }
    });
    saveTimers();
    uploadTimers();
  }

  function uploadTimers() {
    socket.emit('timers!', timers);
  }

  function socketConnect() {
    socket.emit('id', { token: Pebble.getAccountToken(), type: 'app' });
  }

  function socketLabel(data) {
    Pebble.sendAppMessage({
      group: 'TMR',
      operation: 'LABEL',
      data: [ data.id, data.label ].join(delimOuter)
    });
  }

}());
