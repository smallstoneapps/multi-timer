var MockHttp = function () {
  this.handlers = [];
};

MockHttp.prototype.get = function (url, query, callback) {
  this.handlers.forEach(function (handler) {
    if (handler.regex.test(url)) {
      return handler.callback(url, query, callback);
    }
  });
};

MockHttp.prototype.post = function (url, body, callback) {
  this.handlers.forEach(function (handler) {
    if (handler.regex.test(url)) {
      return handler.callback(url, body, callback);
    }
  });
};

MockHttp.prototype.addHandler = function (regex, callback) {
  if (callback === void 0) {
    callback = regex;
    regex = /.*/;
  }
  this.handlers.push({ regex: regex, callback: callback });
};