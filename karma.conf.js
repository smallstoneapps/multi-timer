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
      '*.js': 'coverage'
    },
    exclude: [ 'src/main.js' ],
    reporters: [ 'progress', 'osx', 'coverage' ],
    coverageReporter: {
      type : 'html',
      dir : 'coverage/'
    },
    port: 9876,
    colors: true,
    logLevel: config.LOG_INFO,
    autoWatch: true,
    browsers: [ 'PhantomJS' ],
    captureTimeout: 60000,
    singleRun: false
  });

};
