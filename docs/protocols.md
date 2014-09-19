# Multi Timer Protocols

This document describes how the various pieces of the [Multi Timer][appstore]
application fit together.

Multi Timer is made up of the following pieces:

* Pebble application that runs on the watch.
* PebbleKit JS application that runs on the phone.
* Configuration page that is accessed from the phone and hosted online.
* Hosted database.

## Events

There are several events that require the various parts to communicate with
each other.

### Multi Timer launched on the watch

When a user launches Multi Timer on their watch, it should check with the PKJS
application on the phone to see if it has a more up-to-date configuration than
the watch does.

### Configuration page opened

### Timer label set from configuration

### Timer added on watch

### Timer removed on watch

[appstore]: https://apps.getpebble.com/applications/52d30a1d19412b4d84000025