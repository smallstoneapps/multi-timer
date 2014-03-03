'use strict';

angular.module('multiTimerApp').filter('displayTime', function () {

  return function (input) {
    if (typeof(input) !== 'number') {
      return '';
    }
    var minutes = Math.floor(input / 60);
    var seconds = input % 60;
    return ((minutes < 10) ? '0' : '') + minutes + ':' + ((seconds < 10) ? '0' : '') + seconds;
  };

});
