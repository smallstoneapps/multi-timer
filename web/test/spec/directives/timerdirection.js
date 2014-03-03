'use strict';

describe('Directive: timerDirection', function () {

  beforeEach(module('multiTimerApp'));

  var element;
  var scope;

  beforeEach(inject(function ($rootScope) {
    scope = $rootScope.$new();
  }));

  it('should show an up arrow with an up timer', inject(function ($compile) {
    scope.timer = { direction: 'up' };
    element = angular.element('<span timer-direction></span>');
    element = $compile(element)(scope);
    expect(element.hasClass('fa')).toBe(true);
    expect(element.hasClass('fa-arrow-up')).toBe(true);
  }));

  it('should show a down arrow with a down timer', inject(function ($compile) {
    scope.timer = { direction: 'down' };
    element = angular.element('<span timer-direction></span>');
    element = $compile(element)(scope);
    expect(element.hasClass('fa')).toBe(true);
    expect(element.hasClass('fa-arrow-down')).toBe(true);
  }));

});
