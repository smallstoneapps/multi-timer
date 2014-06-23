/* global Pebble */
/* global Config */
/* global AppInfo */
/* global Analytics */
/* global Keen */
/* global Firebase */
/* global http */
/* global Bugsense */

/* global Timers */

(function () {

  var self = {
    ga: null,
    firebase: null,
    config: Config
  };

  Pebble.addEventListener('ready', function () {
    try {
      setup();
    }
    catch (ex) {
      console.log(ex);
      try {
        Bugsense.notify(ex);
      }
      catch (ex2) {
        console.log(ex2);
      }
    }
  });

  Pebble.addEventListener('showConfiguration', function () {
    self.firebase.child('state').set('loading');
    Pebble.sendAppMessage({ group: 'TMR', operation: 'LIST', data: '' });
    Pebble.openURL(self.config.settingsUrl + '#?token=' + Pebble.getAccountToken());
  });

  Pebble.addEventListener('appmessage', function (event) {
    switch (event.data.group) {
      case 'TMR':
        Timers.handleMessage(event.data.operation, event.data.data);
        break;
      default:
        console.log(JSON.stringify(event.data));
    }
  });

  function setup() {
    Bugsense.initAndStartSession({ apiKey: self.config.bugsense.key, appVersion: AppInfo.versionLabel, userIdentifier: Pebble.getAccountToken() });
    self.ga = new Analytics('UA-48246810-3', 'Multi Timer', AppInfo.versionLabel);
    Keen.init(http, Pebble, Config.keen, AppInfo);
    self.firebase = new Firebase(self.config.firebase.root + Pebble.getAccountToken());
    self.firebase.child('lastRun').set(Date.now());
    self.firebase.child('version').set(AppInfo.versionLabel);
    Timers.init(self.config, self.firebase);
    console.log([].butts.foo);
  }

}());