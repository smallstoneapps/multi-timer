# Multi Timer Analytics

Multi Timer uses [Keen](http://keen.io) for recording analytics pertaining to
the usage of the app. These analytics are used by the developer of the app,
Matthew Tole, to improve the app.

The data sent to Keen is tagged using the Pebble account token feature, which
is unique to each user and app. This means that while I can track an individual
user's behaviour, I cannot identify them or track them across multiple apps.

I also create a unique, random session ID each time the app is launched.

## Events

Here are all of the events that are sent to Keen, with a brief description where
applicable.

### app : General Events

This group of event is used for tracking general events, such as the app
starting, and exiting, and when the app has been updated.

#### app.start
#### app.end
#### app.updated

#### menu_click

* add_timer
* controls
* settings
* about
* start_all
* pause_all
* reset_all
* clear_timers

#### tiw

* about