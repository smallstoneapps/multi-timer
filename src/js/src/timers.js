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

/* exported Timers */

var Timers = (function () {

  function Timers(config, socket) {
    this.config = config;
    this.timers = [];
    this.socket = socket;
    this.socket.on('connect', function () {
      this.socket.emit('id', { token: Pebble.getAccountToken(), type: 'app' });
    }.bind(this));
    this.socket.on('label', function (data) {
      Pebble.sendAppMessage({ group: 'TMR', operation: 'LABEL', data: [ data.id, data.label ].join(this.config.delimiters.outer) });
    }.bind(this));
  }

  Timers.prototype.handleMessage = function(operation, data) {
    try {
      switch (operation) {
        case 'LIST':
          var timerStrings = data.split(this.config.delimiters.outer);
          this.timers = [];
          timerStrings.forEach(function (str) {
            if (! str.length) {
              return;
            }
            var timer = parseTimer.call(this, str);
            if (null !== timer) {
              this.timers.push(timer);
            }
          }.bind(this));
          this.socket.emit('timers!', this.timers);
          break;
      }
    }
    catch (ex) {
      console.log(ex);
    }
  };

  Timers.prototype.getTimers = function() {
    return this.timers;
  };

  function parseTimer(str) {
    var bits = str.split(this.config.delimiters.inner);
    if (bits.length <= 1) {
      return null;
    }
    var timer = {};
    timer.id = parseInt(bits[0], 16);
    timer.direction = bits[1] === '0' ? 'up' : 'down';
    switch (timer.direction) {
      case 'up':
        timer.label = bits[2];
        break;
      case 'down':
        timer.length = parseInt(bits[2], 10);
        timer.repeat = bits[3] === '1';
        timer.vibrate = vibrationString(parseInt(bits[4], 10));
        timer.label = bits[5];
        break;
    }
    return timer;
  }

  function vibrationString(vibe) {
    var vibeStrings = [
      'off',
      'short',
      'long',
      'double',
      'triple',
      'continuous'
    ];
    return vibeStrings[vibe];
  }

  return Timers;

}());