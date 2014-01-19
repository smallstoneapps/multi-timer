jshint javascript/*.js || { exit 1; }
jshint pebble/appinfo.json || { exit 1; }
cat javascript/pbl-analytics.js javascript/main.js > pebble/src/js/pebble-js-app.js || { exit 1; }
cd pebble
pebble clean
pebble build || { exit 1; }
rm src/js/pebble-js-app.js || { exit 1; }
if [ "$1" = "install" ]; then
    pebble install --logs
fi