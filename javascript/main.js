;(function () {

  Pebble.addEventListener('ready', pebbleReady);
  Pebble.addEventListener('appmessage', pebbleAppMessage);
  Pebble.addEventListener('webviewclosed', pebbleWebViewClosed);

  function pebbleReady(e) {
  }

  function pebbleAppMessage(e) {
    var payload = e.payload;
  }

  function pebbleWebViewClosed(e) {
  }

}());