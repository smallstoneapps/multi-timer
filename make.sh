jshint pebble/appinfo.json || { exit 1; }
cd pebble
pebble clean
pebble build || { exit 1; }
if [ "$1" = "install" ]; then
    pebble install --logs
fi