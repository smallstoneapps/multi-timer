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

src/js/tests/timers.js

*/

/* global describe */
/* global expect */
/* global beforeEach */
/* global it */
/* global MockPebble */
/* global Timers */

/*describe('Timers', function () {

  var _pebble = null;
  var config = {
    delimiters: {
      inner: '`',
      outer: '~'
    }
  };
  var timers = null;

  beforeEach(function () {
    _pebble = window.Pebble;
    window.Pebble = new MockPebble();
    socket = new MockSocket();
    timers = new Timers(config, socket);
  });

  afterEach(function () {
    window.Pebble = _pebble;
  });

  it('should parse incoming timers', function (done) {
    timers.handleMessage('LIST', '0`1`600`0`1`Foo');
    expect(timers.getTimers().length).to.equal(1);
    expect(timers.getTimers()[0].id).to.equal(0);
    expect(timers.getTimers()[0].direction).to.equal('down');
    expect(timers.getTimers()[0].length).to.equal(600);
    expect(timers.getTimers()[0].repeat).to.equal(false);
    expect(timers.getTimers()[0].vibrate).to.equal('short');
    expect(timers.getTimers()[0].label).to.equal('Foo');
    done();
  });

  it('should emit a timers! event', function (done) {
    socket._on('timers!', function (data) {
      expect(data.length).to.equal(1);
      done();
    });
    timers.handleMessage('LIST', '0`1`600`0`1`Foo');
  });

  it('should update labels', function (done) {
    socket._emit('label', { id: 1, label: 'Demo Label' });
    // TODO: Actually test something!
    done();
  });

});*/