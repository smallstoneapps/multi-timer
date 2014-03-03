'use strict';

describe('Filter: displayTime', function () {

  // load the filter's module
  beforeEach(module('multiTimerApp'));

  // initialize a new instance of the filter before each test
  var displayTime;
  beforeEach(inject(function ($filter) {
    displayTime = $filter('displayTime');
  }));

  it('should return an empty string when given no input', function () {
    expect(displayTime()).toBe('');
  });

  it('should format times as mm:ss by default', function () {
    expect(displayTime(5)).toBe('00:05');
    expect(displayTime(60)).toBe('01:00');
    expect(displayTime(0)).toBe('00:00');
    expect(displayTime(123)).toBe('02:03');
    expect(displayTime(9000)).toBe('150:00');
  });

});
