/* exported Keen */

var Keen = (function () {

  var _http = null;
  var _pebble = null;
  var _config = null;
  var _appinfo = null;

  return {
    init: init,
    sendEvent: sendEvent
  };

  function init(http, Pebble, config, appinfo, debug) {
    _http = http;
    _pebble = Pebble;
    _config = config;
    _appinfo = appinfo;
    _debug = debug;
  }

  function sendEvent(name, data) {
    if (! data) {
      data = {};
    }
    data.app = { name: _appinfo.shortName, version: _appinfo.versionLabel, uuid: _appinfo.uuid };
    data.user = { accountToken: _pebble.getAccountToken() };
    var url = 'https://api.keen.io/3.0/projects/' + _config.projectId + '/events/' + name + '?api_key=' + _config.writeKey;
    if (_debug) {
      log(url);
      log(JSON.stringify(data));
      return;
    }
    _http.post(url, JSON.stringify(data), function (err, response) {
      if (err) {
        // TODO: Handle error.
      }
      if (! response.created) {
        // TODO: Handle errror.
      }
    });
  }

  function log() {
    var pieces = [ _appinfo.shortName, _appinfo.versionLabel, 'Keen' ];
    pieces = pieces.concat(Array.prototype.slice.call(arguments));
    console.log(pieces.join(' // '));
  }

}());