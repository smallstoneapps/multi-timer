# Multi Timer Protocols

This document describes how the various pieces of the [Multi Timer][1] application
fit together.

Multi Timer is made up of the following pieces:

* Pebble application that runs on the watch.
* PebbleKit JS application that runs on the phone.
* Configuration page that is accessed from the phone and hosted online.
* Firebase data store.

## Events

There are several events that require the various parts to communicate with each other.

### Multi Timer launched on the watch

When a user launches Multi Timer on their watch, it should check with the PKJS application on the phone to see if it has a more up-to-date configuration than the watch does. The PKJS application should in turn check with the Firebase data store to see if that has been updated also.

### Configuration page opened

The PKJS code should request an up-to-date list of timers and send the results to Firebase.

### Timer label set

The Firebase will trigger an update that the PKJS code can listen for and send the updated details to the watch.

### Timer added on watch

The watch sends the timer information to PKJS which sends it to Firebase.

### Timer removed on watch

The watch tells PKJS which timer has been removed and it updates Firebase.

[1]: https://apps.getpebble.com/applications/52d30a1d19412b4d84000025