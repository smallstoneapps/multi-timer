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

karma.conf.js

*/

module.exports = function(config) {

  config.set({
    basePath: 'src/js',
    frameworks: [ 'mocha', 'chai', 'sinon' ],
    files: [
      'src/*.js',
      'tests/*.js',
      'tests/mocks/*.js'
    ],
    preprocessors: {
      'src/*.js': 'coverage'
    },
    exclude: [
      'src/main.js'
    ],
    reporters: [ 'progress', 'osx', 'coverage' ],
    coverageReporter: {
      type : 'html',
      dir : 'coverage/'
    },
    port: 9876,
    colors: true,
    logLevel: config.LOG_INFO,
    autoWatch: true,
    customLaunchers: {
      'PhantomJS_Fixed': {
        base: 'PhantomJS',
        options: {
          settings: {
            webSecurityEnabled: false
          }
        }
      }
    },
    browsers: [ 'PhantomJS_Fixed' ],
    captureTimeout: 60000,
    singleRun: false
  });

};
