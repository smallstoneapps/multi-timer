'use strict';

angular.module('multiTimerApp')
  .directive('timerDirection', function () {
    return {
      template: '<i></i>',
      restrict: 'A',
      link: function postLink(scope, element, attrs) {
        element.addClass('fa fa-2x');
        switch (scope.timer.direction) {
          case 'up':
            element.addClass('fa-arrow-up');
          break;
          case 'down':
            element.addClass('fa-arrow-down');
          break;
        }
      }
    };
  });
