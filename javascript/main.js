;(function () {

  PblAnalytics.init({ uuid: '0db6a55e-b32a-4b03-b037-95637bf306ff', version: '2.5.0' });

  Pebble.addEventListener('ready', pebbleReady);
  Pebble.addEventListener('appmessage', pebbleAppMessage);

  function pebbleReady(e) {
  }

  function pebbleAppMessage(e) {
    var payload = e.payload;
  }

}());