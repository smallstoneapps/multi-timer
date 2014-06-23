/* exported Timers */

var Timers = (function () {

  var self = {
    timers: [],
    config: {},
    firebase: null
  };

  return {
    init: init,
    handleMessage: handleMessage,
    getTimers: getTimers
  };

  function init(config, firebase) {
    self.config = config;
    self.timers = [];
    self.firebase = firebase;
    if (self.firebase) {
      self.firebase.child('timers').on('child_changed', handleTimerChanged);
    }
  }

  function handleMessage(operation, data) {
    try {
      switch (operation) {
        case 'LIST':
          var timerStrings = data.split(self.config.delimiters.outer);
          self.timers = [];
          timerStrings.forEach(function (str) {
            if (! str.length) {
              return;
            }
            var timer = parseTimer(str);
            if (null !== timer) {
              self.timers.push(timer);
            }
          });
          syncTimers();
          break;
      }
    }
    catch (ex) {
      console.log(ex);
    }
  }

  function getTimers() {
    return self.timers;
  }

  function handleTimerChanged(snapshot) {
    Pebble.sendAppMessage({
      group: 'TMR',
      operation: 'LABEL',
      data: [ snapshot.val().id, snapshot.val().label.substr(0, 24) ].join(self.config.delimiters.outer)
    });
  }

  function syncTimers() {
    if (! self.firebase) { return; }
    self.firebase.child('timers').remove();
    self.timers.forEach(function (timer) {
      self.firebase.child('timers').push(timer);
    });
    self.firebase.child('state').set('loaded');
  }

  function parseTimer(str) {
    var bits = str.split(self.config.delimiters.inner);
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


}());