var Analytics = function(analyticsId, appName, appVersion) {
  this.analyticsId = analyticsId;
  this.appName = appName;
  this.appVersion = appVersion;
  this.analyticsUserId = Pebble.getAccountToken();
};

Analytics.prototype._trackGA = function(type, params) {
  var req = new XMLHttpRequest();
  var url = 'http://www.google-analytics.com/collect';
  var trackingParams = 'v=1';
  trackingParams += '&tid=' + this.analyticsId;
  trackingParams += '&cid=' + this.analyticsUserId;
  trackingParams += '&t=' + type;
  trackingParams += '&an=' + this.appName;
  trackingParams += '&av=' + this.appVersion;

  for (var parameterKey in params) {
    if (params.hasOwnProperty(parameterKey)) {
      trackingParams += '&'+ parameterKey + '=' + params[parameterKey];
    }
  }

  req.open('POST', url, true);
  req.setRequestHeader('Content-length', trackingParams.length);
  req.send(trackingParams);
};

Analytics.prototype.trackScreen = function (screenName) {
  this._trackGA('appview', {'cd': screenName});
};

Analytics.prototype.trackEvent = function (category, action) {
  this._trackGA('event', {'ec': category, 'ea': action});
};

// If require() will work some day. This would be nessecary.
// module.exports.Analytics = Analytics;