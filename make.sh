jshint js/main.js || { exit 1; }
jshint pebble/appinfo.json || { exit 1; }
uglifyjs js/main.js -o pebble/src/js/pebble-js-app.js
cd pebble
pebble clean
pebble build || { exit 1; }
rm src/js/pebble-js-app.js || { exit 1; }
pebble install --logs --phone 192.168.0.21