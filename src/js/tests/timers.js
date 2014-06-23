/* global describe */
/* global expect */
/* global beforeEach */
/* global it */
/* global MockPebble */
/* global Timers */

describe('Timers', function () {

  var _pebble = null;
  var config = {
    delimiters: {
      inner: '`',
      outer: '~'
    }
  };

  beforeEach(function () {
    _pebble = window.Pebble;
    window.Pebble = new MockPebble();
    Timers.init(config);
  });

  afterEach(function () {
    window.Pebble = _pebble;
  });

  it('should parse incoming timers', function (done) {
    Timers.handleMessage('LIST', '0`1`600`0`1`Foo');
    expect(Timers.getTimers().length).to.equal(1);
    expect(Timers.getTimers()[0].id).to.equal(0);
    expect(Timers.getTimers()[0].direction).to.equal('down');
    expect(Timers.getTimers()[0].length).to.equal(600);
    expect(Timers.getTimers()[0].repeat).to.equal(false);
    expect(Timers.getTimers()[0].vibrate).to.equal('short');
    expect(Timers.getTimers()[0].label).to.equal('Foo');
    done();
  });

});