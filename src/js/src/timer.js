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

src/js/src/timer.js

*/

/* exported Timer */
/* exported parserTimer */


function Timer(obj) {
  this.id = obj.id;
  this.direction = obj.direction;
  this.label = obj.label;
  if (this.isDown()) {
    this.length = obj.length;
    this.vibrate = obj.vibrate;
    this.repeat = obj.repeat;
  }
}

Timer.prototype.toObject = function () {
  var obj = {
    id: this.id,
    direction: this.direction,
    label: this.label
  };
  if (this.isDown()) {
    obj.length = this.length;
    obj.vibrate = this.vibrate;
    obj.repeat = this.repeat;
  }
  return obj;
};

Timer.prototype.toString = function (delim) {
  var bits = [];
  bits.push(this.id.toString(10));
  bits.push(this.isUp() ? '0' : '1');
  if (this.isDown()) {
    bits.push(this.length);
    bits.push(this.repeat ? '1' : '0');
    bits.push(this.vibrate);
  }
  bits.push(this.label);
  return bits.join(delim);
};

Timer.prototype.isUp = function () {
  return this.direction === 'up';
};

Timer.prototype.isDown = function () {
  return this.direction === 'down';
};

Timer.parse = function (str, delim) {
  var bits = str.split(delim);
  if (bits.length <= 1) {
    return null;
  }
  var obj = {
    id: parseInt(bits[0], 10),
    direction: (bits[1] === '0') ? 'up' : 'down'
  };
  if (obj.direction === 'up') {
    obj.label = bits[2];
  }
  else {
    obj.length = parseInt(bits[2], 10);
    obj.repeat = bits[3] === '1';
    obj.vibrate = parseInt(bits[4], 10);
    obj.label = bits[5];
  }
  return new Timer(obj);
};
