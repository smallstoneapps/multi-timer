'use strict';

angular.module('multiTimerApp').controller('MainCtrl', function ($scope) {

  $scope.timers = [
    { direction: 'up' },
    { direction: 'down', duration: 600 },
  ];

});