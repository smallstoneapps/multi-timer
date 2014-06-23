/* exported http */

var http = (function () {

  return {
    get: get,
    post: post
  };

  function get(url, query, callback) {
    var req = new XMLHttpRequest();
    url += '?' + serialize(query);
    req.open('GET', url, true);
    req.setRequestHeader('Connection', 'close');
    req.onload = function () {
      if (req.readyState === 4 && req.status === 200) {
        if (req.status === 200) {
          var response = JSON.parse(req.responseText);
          return callback(null, response);
        }
      }
      else {
        return callback(new Error(req.status));
      }
    };
    req.onerror = function () {
      switch (req.status) {
      case 0:
        return callback(new Error('NOT_CONNECTED'));
      case 404:
        return callback(new Error('NOT_FOUND'));
      }
    };
    req.send();

    function serialize(obj) {
      var str = [];
      for(var p in obj) {
        if (obj.hasOwnProperty(p)) {
          str.push(encodeURIComponent(p) + '=' + encodeURIComponent(obj[p]));
        }
      }
      return str.join('&');
    }
  }

  function post(url, body, callback) {
    var req = new XMLHttpRequest();
    req.open('POST', url, true);
    req.setRequestHeader('Connection', 'close');
    req.setRequestHeader('Content-Type', 'application/json');
    req.onload = function () {
      if ([ 200, 201 ].indexOf(req.status) !== -1) {
        var response = JSON.parse(req.responseText);
        return callback(null, response);
      }
      else {
        return callback(new Error(req.status), req.responseText);
      }
    };
    req.onerror = function () {
      switch (req.status) {
      case 0:
        return callback(new Error('NOT_CONNECTED'));
      case 404:
        return callback(new Error('NOT_FOUND'));
      }
    };
    req.send(body);
  }

}());
