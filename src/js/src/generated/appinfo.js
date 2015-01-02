/*

Multi Timer v3.4
http://matthewtole.com/pebble/multi-timer/

----------------------

The MIT License (MIT)

Copyright © 2013 - 2015 Matthew Tole

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

--------------------

src/js/src/generated/appinfo.js

*/

/* exported AppInfo */

var AppInfo = {
  "uuid": "0db6a55e-b32a-4b03-b037-95637bf306ff",
  "shortName": "Multi Timer",
  "longName": "Multi Timer",
  "companyName": "Matthew Tole",
  "versionCode": 1,
  "versionLabel": "3.4",
  "watchapp": {
    "watchface": false
  },
  "appKeys": { },
  "capabilities": [ ],
  "config": {
    "googleAnalytics": { "trackingId": "UA-48246810-3" }
  },
  "delimiters": {
    "inner": "`",
    "outer": "~"
  },
  "debug": false,
  "resources": {
    "media": [
      {
        "menuIcon": true,
        "type": "png",
        "name": "MENU_ICON",
        "file": "images/menu.png"
      },
      {
        "type": "png",
        "name": "ICONS_16",
        "file": "images/icons.png"
      },
      {
        "type": "png",
        "name": "IMAGE_ALARM",
        "file": "images/alarm.png"
      },
      {
        "type": "raw",
        "name": "TEXT_ABOUT",
        "file": "about.txt"
      },
      {
        "characterRegex": "[0-9]",
        "type": "font",
        "name": "FONT_AUDI_70_BOLD",
        "file": "fonts/audi.ttf"
      }
    ]
  }
};