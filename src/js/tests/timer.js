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

src/js/tests/timer.js

*/

/* global describe */
/* global expect */
/* global beforeEach */
/* global it */
/* global Timer */

describe('Timer', function () {

  it('should create an upwards Timer object from a POJO', function (done) {
    var timer = new Timer({ id: 1, direction: 'up', label: 'label' });
    expect(timer.id).to.equal(1);
    expect(timer.direction).to.equal('up');
    expect(timer.label).to.equal('label');
    done();
  });

  it('should create a downwards Timer object from a POJO', function (done) {
    var timer = new Timer({ id: 1, direction: 'down', label: 'label', length: 1000, vibrate: 3, repeat: true });
    expect(timer.id).to.equal(1);
    expect(timer.direction).to.equal('down');
    expect(timer.label).to.equal('label');
    expect(timer.length).to.equal(1000);
    expect(timer.vibrate).to.equal(3);
    expect(timer.repeat).to.equal(true);
    done();
  });

  it ('should be able to recreate a DOWN Timer from a generated POJO', function (done) {
    var timer = new Timer({ id: 1, direction: 'down', label: 'label', length: 1000, vibrate: 3, repeat: true });
    var timerRecreated = new Timer(timer.toObject());
    timerMatch(timer, timerRecreated);
    done();
  });

  it ('should be able to recreate an UP Timer from a generated POJO', function (done) {
    var timer = new Timer({ id: 1, direction: 'up', label: 'label' });
    var timerRecreated = new Timer(timer.toObject());
    timerMatch(timer, timerRecreated);
    done();
  });

  it ('should be able to parse a delimited UP string', function (done) {
    var timer = Timer.parse('1_0_label', '_');
    expect(timer.id).to.equal(1);
    expect(timer.isUp()).to.equal(true);
    expect(timer.label).to.equal('label');
    done()
  });

  it ('should be able to parse a delimited DOWN string', function (done) {
    var timer = Timer.parse('1_1_3600_0_3_label', '_');
    expect(timer.id).to.equal(1);
    expect(timer.isUp()).to.equal(false);
    expect(timer.length).to.equal(3600);
    expect(timer.vibrate).to.equal(3);
    expect(timer.repeat).to.equal(false);
    expect(timer.label).to.equal('label');
    done()
  });

  it ('should be able to recreate an UP Timer from a generated string', function (done) {
    var timer = new Timer({ id: 1000, direction: 'up', label: 'label' });
    var timerRecreated = Timer.parse(timer.toString('_'), '_');
    timerMatch(timer, timerRecreated);
    done();
  });

  it ('should be able to recreate an DOWN Timer from a generated string', function (done) {
    var timer1 = new Timer({ id: 1000, direction: 'down', label: 'label', repeat: false, vibrate: 5, length: 500 });
    var timerRecreated1 = Timer.parse(timer1.toString('_'), '_');

    var timer2 = new Timer({ id: 1000, direction: 'down', label: 'label', repeat: true, vibrate: 5, length: 500 });
    var timerRecreated2 = Timer.parse(timer2.toString('_'), '_');

    timerMatch(timer1, timerRecreated1);
    timerMatch(timer2, timerRecreated2);
    done();
  });

  it('should return NULL on an empty string', function (done) {
    expect(Timer.parse('', '_')).to.equal(null);
    done();
  });

  function timerMatch(left, right) {
    expect(left.id).to.equal(right.id);
    expect(left.direction).to.equal(right.direction);
    expect(left.label).to.equal(right.label);
    expect(left.length).to.equal(right.length);
    expect(left.vibrate).to.equal(right.vibrate);
    expect(left.repeat).to.equal(right.repeat);
  }

});