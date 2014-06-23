/*!
  * Bowser - a browser detector
  * https://github.com/ded/bowser
  * MIT License | (c) Dustin Diaz 2013
  */

!function (name, definition) {
  if (typeof define == 'function') define(definition)
  else if (typeof module != 'undefined' && module.exports) module.exports['browser'] = definition()
  else this[name] = definition()
}('bowser', function () {
  /**
    * navigator.userAgent =>
    * Chrome:  "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_6_7) AppleWebKit/534.24 (KHTML, like Gecko) Chrome/11.0.696.57 Safari/534.24"
    * Opera:   "Opera/9.80 (Macintosh; Intel Mac OS X 10.6.7; U; en) Presto/2.7.62 Version/11.01"
    * Safari:  "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_7; en-us) AppleWebKit/533.21.1 (KHTML, like Gecko) Version/5.0.5 Safari/533.21.1"
    * IE:      "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/5.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C)"
    * IE>=11:  "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; .NET4.0E; .NET4.0C; Media Center PC 6.0; rv:11.0) like Gecko"
    * Firefox: "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:2.0) Gecko/20100101 Firefox/4.0"
    * iPhone:  "Mozilla/5.0 (iPhone Simulator; U; CPU iPhone OS 4_3_2 like Mac OS X; en-us) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8H7 Safari/6533.18.5"
    * iPad:    "Mozilla/5.0 (iPad; U; CPU OS 4_3_2 like Mac OS X; en-us) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8H7 Safari/6533.18.5",
    * Android: "Mozilla/5.0 (Linux; U; Android 2.3.4; en-us; T-Mobile G2 Build/GRJ22) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1"
    * Touchpad: "Mozilla/5.0 (hp-tabled;Linux;hpwOS/3.0.5; U; en-US)) AppleWebKit/534.6 (KHTML, like Gecko) wOSBrowser/234.83 Safari/534.6 TouchPad/1.0"
    * PhantomJS: "Mozilla/5.0 (Macintosh; Intel Mac OS X) AppleWebKit/534.34 (KHTML, like Gecko) PhantomJS/1.5.0 Safari/534.34"
    */

  var ua = navigator.userAgent
    , t = true
    , ie = /(msie|trident)/i.test(ua)
    , chrome = /chrome|crios/i.test(ua)
    , phantom = /phantom/i.test(ua)
    , safari = /safari/i.test(ua) && !chrome && !phantom
    , iphone = /iphone/i.test(ua)
    , ipad = /ipad/i.test(ua)
    , touchpad = /touchpad/i.test(ua)
    , android = /android/i.test(ua)
    , opera = /opera/i.test(ua) || /opr/i.test(ua)
    , firefox = /firefox/i.test(ua)
    , gecko = /gecko\//i.test(ua)
    , seamonkey = /seamonkey\//i.test(ua)
    , webkitVersion = /version\/(\d+(\.\d+)?)/i
    , firefoxVersion = /firefox\/(\d+(\.\d+)?)/i
    , o

  function detect() {

    if (ie) return {
        name: 'Internet Explorer'
      , msie: t
      , version: ua.match(/(msie |rv:)(\d+(\.\d+)?)/i)[2]
      }
    if (opera) return {
        name: 'Opera'
      , opera: t
      , version: ua.match(webkitVersion) ? ua.match(webkitVersion)[1] : ua.match(/opr\/(\d+(\.\d+)?)/i)[1]
      }
    if (chrome) return {
        name: 'Chrome'
      , webkit: t
      , chrome: t
      , version: ua.match(/(?:chrome|crios)\/(\d+(\.\d+)?)/i)[1]
      }
    if (phantom) return {
        name: 'PhantomJS'
      , webkit: t
      , phantom: t
      , version: ua.match(/phantomjs\/(\d+(\.\d+)+)/i)[1]
      }
    if (touchpad) return {
        name: 'TouchPad'
      , webkit: t
      , touchpad: t
      , version : ua.match(/touchpad\/(\d+(\.\d+)?)/i)[1]
      }
    if (iphone || ipad) {
      o = {
        name : iphone ? 'iPhone' : 'iPad'
      , webkit: t
      , mobile: t
      , ios: t
      , iphone: iphone
      , ipad: ipad
      }
      // WTF: version is not part of user agent in web apps
      if (webkitVersion.test(ua)) {
        o.version = ua.match(webkitVersion)[1]
      }
      return o
    }
    if (android) return {
        name: 'Android'
      , webkit: t
      , android: t
      , mobile: t
      , version: (ua.match(webkitVersion) || ua.match(firefoxVersion))[1]
      }
    if (safari) return {
        name: 'Safari'
      , webkit: t
      , safari: t
      , version: ua.match(webkitVersion)[1]
      }
    if (gecko) {
      o = {
        name: 'Gecko'
      , gecko: t
      , mozilla: t
      , version: ua.match(firefoxVersion)[1]
      }
      if (firefox) {
        o.name = 'Firefox';
        o.firefox = t;
      }
      return o
    }
    if (seamonkey) return {
        name: 'SeaMonkey'
      , seamonkey: t
      , version: ua.match(/seamonkey\/(\d+(\.\d+)?)/i)[1]
      }
    return {}
  }

  var bowser = detect()

  // Graded Browser Support
  // http://developer.yahoo.com/yui/articles/gbs
  if ((bowser.msie && bowser.version >= 8) ||
      (bowser.chrome && bowser.version >= 10) ||
      (bowser.firefox && bowser.version >= 4.0) ||
      (bowser.safari && bowser.version >= 5) ||
      (bowser.opera && bowser.version >= 10.0)) {
    bowser.a = t;
  }

  else if ((bowser.msie && bowser.version < 8) ||
      (bowser.chrome && bowser.version < 10) ||
      (bowser.firefox && bowser.version < 4.0) ||
      (bowser.safari && bowser.version < 5) ||
      (bowser.opera && bowser.version < 10.0)) {
    bowser.c = t
  } else bowser.x = t

  return bowser
});

"use strict";
/*
 * Bugsense JavaScript SDK v1.1.1
 * http://bugsense.com/
 *
 * Copyright 2013 Splunk Inc.
 *
 */

var extend = function(obj) {
  each(slice.call(arguments, 1), function(source) {
    if (source) {
      for (var prop in source) {
        obj[prop] = source[prop];
      }
    }
  });
  return obj;
}
var _has = function(object, key) {
  return Object.prototype.hasOwnProperty.call(object, key);
}

// BEGIN - Extracted from Zepto
var escape = encodeURIComponent;

Object.keys = Object.keys || function(o) { 
  var result = []; 
  for(var name in o) { 
    if (o.hasOwnProperty(name)) 
      result.push(name); 
  } 
  return result; 
};

var isObject = function isObject ( instance ) {
  return instance instanceof Object;
};
var array = []

var slice = array.slice;

var isArray = function isArray ( instance ) {
  return instance instanceof Array;
};

var isValidKeyValue = function isValidKeyValue ( instance ) {
  return ( typeof(instance) == 'string' || typeof(instance) == 'number' || typeof(instance) == 'boolean' );
};

/**
 * Simple forEach, implements jQuery/Zepto api, sort of, and most likely breaks with arrays: LOL
 * @param  {Object} obj      To be iterated
 * @param  {Object} iterator Iterator function
 */
var forEach = function forEach ( obj, iterator ) {
  Array.prototype.forEach.call( Object.keys( obj ), function ( key ) {
    iterator( key, obj[ key ] );
  } );
};

var each = function(obj, iterator, context) {
  if (obj == null) return;
  var nativeForEach = Array.prototype.forEach;
  if (nativeForEach && obj.forEach === nativeForEach) {
    obj.forEach(iterator, context);
  } else if (obj.length === +obj.length) {
    for (var i = 0, length = obj.length; i < length; i++) {
      if (iterator.call(context, obj[i], i, obj) === breaker) return;
    }
  } else {
    var keys = _.keys(obj);
    for (var i = 0, length = keys.length; i < length; i++) {
      if (iterator.call(context, obj[keys[i]], keys[i], obj) === breaker) return;
    }
  }
};

var serialize = function serialize ( params, obj, traditional, scope ) {
  var array = isArray( obj );

  forEach( obj, function ( key, value ) {
    if ( scope ) { key = traditional ? scope : scope + '[' + (array ? '' : key) + ']'; }

    // handle data in serializeArray() format
    if ( !scope && array ) {
      params.add( value.name, value.value );
    // recurse into nested objects
    } else if ( traditional ? isArray( value ) : isObject( value ) ) {
      serialize( params, value, traditional, key );
    } else {
      params.add( key, value );
    }
  });
};

var param = function param ( obj, traditional ) {
  var params = [];
  params.add = function( k, v ){ this.push( escape( k ) + '=' + escape( v ) ); };
  serialize( params, obj, traditional );
  // return params.join( '&' ).replace( /%20/g, '+' );
  return params.join( '&' );
};
  // END - Extracted from Zepto

var guid_generator = function GUIDGenerator() {
  var S4 = function () {
    return Math.floor(
      Math.random() * 0x10000 /* 65536 */
    ).toString(16);
  };
  return (
     S4() + S4() + "-" +
     S4() + "-" +
     S4() + "-" +
     S4() + "-" +
     S4() + S4() + S4()
   );
}

// Extracted from Backbone;
// Regular expression used to split event strings.
var eventSplitter = /\s+/;

// Implement fancy features of the Events API such as multiple event
// names `"change blur"` and jQuery-style event maps `{change: action}`
// in terms of the existing API.
var eventsApi = function(obj, action, name, rest) {
  if (!name) return true;

  // Handle event maps.
  if (typeof name === 'object') {
    for (var key in name) {
      obj[action].apply(obj, [key, name[key]].concat(rest));
    }
    return false;
  }

  // Handle space separated event names.
  if (eventSplitter.test(name)) {
    var names = name.split(eventSplitter);
    for (var i = 0, l = names.length; i < l; i++) {
      obj[action].apply(obj, [names[i]].concat(rest));
    }
    return false;
  }

  return true;
};

// A difficult-to-believe, but optimized internal dispatch function for
// triggering events. Tries to keep the usual cases speedy (most internal
// Backbone events have 3 arguments).
var triggerEvents = function(events, args) {
  var ev, i = -1, l = events.length, a1 = args[0], a2 = args[1], a3 = args[2];
  switch (args.length) {
    case 0: while (++i < l) (ev = events[i]).callback.call(ev.ctx); return;
    case 1: while (++i < l) (ev = events[i]).callback.call(ev.ctx, a1); return;
    case 2: while (++i < l) (ev = events[i]).callback.call(ev.ctx, a1, a2); return;
    case 3: while (++i < l) (ev = events[i]).callback.call(ev.ctx, a1, a2, a3); return;
    default: while (++i < l) (ev = events[i]).callback.apply(ev.ctx, args);
  }
};
var _once = function(func) {
  var ran = false, memo;
  return function() {
    if (ran) return memo;
    ran = true;
    memo = func.apply(this, arguments);
    func = null;
    return memo;
  };
};

var TraceKit = {
  linesOfContext: 11
};
var UNKNOWN_FUNCTION = '';
TraceKit.computeStackTrace = (function computeStackTraceWrapper() {
    var debug = false,
        sourceCache = {};
    function loadSource(url) {
        try {
            var getXHR = function() {
                try {
                    return new window.XMLHttpRequest();
                } catch (e) {
                    // explicitly bubble up the exception if not found
                    return new window.ActiveXObject('Microsoft.XMLHTTP');
                }
            };

            var request = getXHR();
            request.open('GET', url, false);
            request.send('');
            return request.responseText;
        } catch (e) {
            return '';
        }
    }

    function getSource(url) {
        if (!_has(sourceCache, url)) {
            // URL needs to be able to fetched within the acceptable domain. Otherwise,
            // cross-domain errors will be triggered.
            var source = '';
            if (url.indexOf(document.domain) !== -1) {
                source = loadSource(url);
            }
            sourceCache[url] = source ? source.split('\n') : [];
        }

        return sourceCache[url];
    }
    function guessFunctionName(url, lineNo) {
        var reFunctionArgNames = /function ([^(]*)\(([^)]*)\)/,
            reGuessFunction = /['"]?([0-9A-Za-z$_]+)['"]?\s*[:=]\s*(function|eval|new Function)/,
            line = '',
            maxLines = 10,
            source = getSource(url),
            m;

        if (!source.length) {
            return UNKNOWN_FUNCTION;
        }

        // Walk backwards from the first line in the function until we find the line which
        // matches the pattern above, which is the function definition
        for (var i = 0; i < maxLines; ++i) {
            line = source[lineNo - i] + line;

            if (line) {
                if ((m = reGuessFunction.exec(line))) {
                    return m[1];
                } else if ((m = reFunctionArgNames.exec(line))) {
                    return m[1];
                }
            }
        }

        return UNKNOWN_FUNCTION;
    }
    function gatherContext(url, line) {
        var source = getSource(url);

        if (!source.length) {
            return null;
        }

        var context = [],
            // linesBefore & linesAfter are inclusive with the offending line.
            // if linesOfContext is even, there will be one extra line
            // *before* the offending line.
            linesBefore = Math.floor(TraceKit.linesOfContext / 2),
            // Add one extra line if linesOfContext is odd
            linesAfter = linesBefore + (TraceKit.linesOfContext % 2),
            start = Math.max(0, line - linesBefore - 1),
            end = Math.min(source.length, line + linesAfter - 1);

        line -= 1; // convert to 0-based index

        for (var i = start; i < end; ++i) {
            if (source[i]) {
                context.push(source[i]);
            }
        }

        return context.length > 0 ? context : null;
    }

    function escapeRegExp(text) {
        return text.replace(/[\-\[\]{}()*+?.,\\\^$|#]/g, '\\$&');
    }
    function escapeCodeAsRegExpForMatchingInsideHTML(body) {
        return escapeRegExp(body).replace('<', '(?:<|&lt;)').replace('>', '(?:>|&gt;)').replace('&', '(?:&|&amp;)').replace('"', '(?:"|&quot;)').replace(/\s+/g, '\\s+');
    }
    function findSourceInUrls(re, urls) {
        var source, m;
        for (var i = 0, j = urls.length; i < j; ++i) {
            // console.log('searching', urls[i]);
            if ((source = getSource(urls[i])).length) {
                source = source.join('\n');
                if ((m = re.exec(source))) {
                    // console.log('Found function in ' + urls[i]);

                    return {
                        'url': urls[i],
                        'line': source.substring(0, m.index).split('\n').length,
                        'column': m.index - source.lastIndexOf('\n', m.index) - 1
                    };
                }
            }
        }

        // console.log('no match');

        return null;
    }
    function findSourceInLine(fragment, url, line) {
        var source = getSource(url),
            re = new RegExp('\\b' + escapeRegExp(fragment) + '\\b'),
            m;

        line -= 1;

        if (source && source.length > line && (m = re.exec(source[line]))) {
            return m.index;
        }

        return null;
    }
    function findSourceByFunctionBody(func) {
        var urls = [window.location.href],
            scripts = document.getElementsByTagName('script'),
            body,
            code = '' + func,
            codeRE = /^function(?:\s+([\w$]+))?\s*\(([\w\s,]*)\)\s*\{\s*(\S[\s\S]*\S)\s*\}\s*$/,
            eventRE = /^function on([\w$]+)\s*\(event\)\s*\{\s*(\S[\s\S]*\S)\s*\}\s*$/,
            re,
            parts,
            result;

        for (var i = 0; i < scripts.length; ++i) {
            var script = scripts[i];
            if (script.src) {
                urls.push(script.src);
            }
        }

        if (!(parts = codeRE.exec(code))) {
            re = new RegExp(escapeRegExp(code).replace(/\s+/g, '\\s+'));
        }

        else {
            var name = parts[1] ? '\\s+' + parts[1] : '',
                args = parts[2].split(',').join('\\s*,\\s*');

            body = escapeRegExp(parts[3]).replace(/;$/, ';?'); // semicolon is inserted if the function ends with a comment.replace(/\s+/g, '\\s+');
            re = new RegExp('function' + name + '\\s*\\(\\s*' + args + '\\s*\\)\\s*{\\s*' + body + '\\s*}');
        }

        if ((result = findSourceInUrls(re, urls))) {
            return result;
        }
        if ((parts = eventRE.exec(code))) {
            var event = parts[1];
            body = escapeCodeAsRegExpForMatchingInsideHTML(parts[2]);

            // look for a function defined in HTML as an onXXX handler
            re = new RegExp('on' + event + '=[\\\'"]\\s*' + body + '\\s*[\\\'"]', 'i');

            if ((result = findSourceInUrls(re, urls[0]))) {
                return result;
            }

            // look for ???
            re = new RegExp(body);

            if ((result = findSourceInUrls(re, urls))) {
                return result;
            }
        }

        return null;
    }

    function computeStackTraceFromStackProp(ex) {
        if (!ex.stack) {
            return null;
        }

        var chrome = /^\s*at (?:((?:\[object object\])?\S+(?: \[as \S+\])?) )?\(?((?:file|http|https):.*?):(\d+)(?::(\d+))?\)?\s*$/i,
            gecko = /^\s*(\S*)(?:\((.*?)\))?@((?:file|http|https).*?):(\d+)(?::(\d+))?\s*$/i,
            lines = ex.stack.split('\n'),
            stack = [],
            parts,
            element,
            reference = /^(.*) is undefined$/.exec(ex.message);

        for (var i = 0, j = lines.length; i < j; ++i) {
            if ((parts = gecko.exec(lines[i]))) {
                element = {
                    'url': parts[3],
                    'func': parts[1] || UNKNOWN_FUNCTION,
                    'args': parts[2] ? parts[2].split(',') : '',
                    'line': +parts[4],
                    'column': parts[5] ? +parts[5] : null
                };
            } else if ((parts = chrome.exec(lines[i]))) {
                element = {
                    'url': parts[2],
                    'func': parts[1] || UNKNOWN_FUNCTION,
                    'line': +parts[3],
                    'column': parts[4] ? +parts[4] : null
                };
            } else {
                continue;
            }

            if (!element.func && element.line) {
                element.func = guessFunctionName(element.url, element.line);
            }

            if (element.line) {
                element.context = gatherContext(element.url, element.line);
            }

            stack.push(element);
        }

        if (stack[0] && stack[0].line && !stack[0].column && reference) {
            stack[0].column = findSourceInLine(reference[1], stack[0].url, stack[0].line);
        }

        if (!stack.length) {
            return null;
        }

        return {
            'mode': 'stack',
            'name': ex.name,
            'message': ex.message,
            'url': document.location.href,
            'stack': stack,
            'useragent': navigator.userAgent
        };
    }
    function computeStackTraceFromStacktraceProp(ex) {
        // Access and store the stacktrace property before doing ANYTHING
        // else to it because Opera is not very good at providing it
        // reliably in other circumstances.
        var stacktrace = ex.stacktrace;

        var testRE = / line (\d+), column (\d+) in (?:<anonymous function: ([^>]+)>|([^\)]+))\((.*)\) in (.*):\s*$/i,
            lines = stacktrace.split('\n'),
            stack = [],
            parts;

        for (var i = 0, j = lines.length; i < j; i += 2) {
            if ((parts = testRE.exec(lines[i]))) {
                var element = {
                    'line': +parts[1],
                    'column': +parts[2],
                    'func': parts[3] || parts[4],
                    'args': parts[5] ? parts[5].split(',') : [],
                    'url': parts[6]
                };

                if (!element.func && element.line) {
                    element.func = guessFunctionName(element.url, element.line);
                }
                if (element.line) {
                    try {
                        element.context = gatherContext(element.url, element.line);
                    } catch (exc) {}
                }

                if (!element.context) {
                    element.context = [lines[i + 1]];
                }

                stack.push(element);
            }
        }

        if (!stack.length) {
            return null;
        }

        return {
            'mode': 'stacktrace',
            'name': ex.name,
            'message': ex.message,
            'url': document.location.href,
            'stack': stack,
            'useragent': navigator.userAgent
        };
    }
    function computeStackTraceFromOperaMultiLineMessage(ex) {
        var lines = ex.message.split('\n');
        if (lines.length < 4) {
            return null;
        }

        var lineRE1 = /^\s*Line (\d+) of linked script ((?:file|http|https)\S+)(?:: in function (\S+))?\s*$/i,
            lineRE2 = /^\s*Line (\d+) of inline#(\d+) script in ((?:file|http|https)\S+)(?:: in function (\S+))?\s*$/i,
            lineRE3 = /^\s*Line (\d+) of function script\s*$/i,
            stack = [],
            scripts = document.getElementsByTagName('script'),
            inlineScriptBlocks = [],
            parts,
            i,
            len,
            source;

        for (i in scripts) {
            if (_has(scripts, i) && !scripts[i].src) {
                inlineScriptBlocks.push(scripts[i]);
            }
        }

        for (i = 2, len = lines.length; i < len; i += 2) {
            var item = null;
            if ((parts = lineRE1.exec(lines[i]))) {
                item = {
                    'url': parts[2],
                    'func': parts[3],
                    'line': +parts[1]
                };
            } else if ((parts = lineRE2.exec(lines[i]))) {
                item = {
                    'url': parts[3],
                    'func': parts[4]
                };
                var relativeLine = (+parts[1]); // relative to the start of the <SCRIPT> block
                var script = inlineScriptBlocks[parts[2] - 1];
                if (script) {
                    source = getSource(item.url);
                    if (source) {
                        source = source.join('\n');
                        var pos = source.indexOf(script.innerText);
                        if (pos >= 0) {
                            item.line = relativeLine + source.substring(0, pos).split('\n').length;
                        }
                    }
                }
            } else if ((parts = lineRE3.exec(lines[i]))) {
                var url = window.location.href.replace(/#.*$/, ''),
                    line = parts[1];
                var re = new RegExp(escapeCodeAsRegExpForMatchingInsideHTML(lines[i + 1]));
                source = findSourceInUrls(re, [url]);
                item = {
                    'url': url,
                    'line': source ? source.line : line,
                    'func': ''
                };
            }

            if (item) {
                if (!item.func) {
                    item.func = guessFunctionName(item.url, item.line);
                }
                var context = gatherContext(item.url, item.line);
                var midline = (context ? context[Math.floor(context.length / 2)] : null);
                if (context && midline.replace(/^\s*/, '') === lines[i + 1].replace(/^\s*/, '')) {
                    item.context = context;
                } else {
                    // if (context) alert("Context mismatch. Correct midline:\n" + lines[i+1] + "\n\nMidline:\n" + midline + "\n\nContext:\n" + context.join("\n") + "\n\nURL:\n" + item.url);
                    item.context = [lines[i + 1]];
                }
                stack.push(item);
            }
        }
        if (!stack.length) {
            return null; // could not parse multiline exception message as Opera stack trace
        }

        return {
            'mode': 'multiline',
            'name': ex.name,
            'message': lines[0],
            'url': document.location.href,
            'stack': stack,
            'useragent': navigator.userAgent
        };
    }
    function augmentStackTraceWithInitialElement(stackInfo, url, lineNo, message) {
        var initial = {
            'url': url,
            'line': lineNo
        };

        if (initial.url && initial.line) {
            stackInfo.incomplete = false;

            if (!initial.func) {
                initial.func = guessFunctionName(initial.url, initial.line);
            }

            if (!initial.context) {
                initial.context = gatherContext(initial.url, initial.line);
            }

            var reference = / '([^']+)' /.exec(message);
            if (reference) {
                initial.column = findSourceInLine(reference[1], initial.url, initial.line);
            }

            if (stackInfo.stack.length > 0) {
                if (stackInfo.stack[0].url === initial.url) {
                    if (stackInfo.stack[0].line === initial.line) {
                        return false; // already in stack trace
                    } else if (!stackInfo.stack[0].line && stackInfo.stack[0].func === initial.func) {
                        stackInfo.stack[0].line = initial.line;
                        stackInfo.stack[0].context = initial.context;
                        return false;
                    }
                }
            }

            stackInfo.stack.unshift(initial);
            stackInfo.partial = true;
            return true;
        } else {
            stackInfo.incomplete = true;
        }

        return false;
    }
    function computeStackTraceByWalkingCallerChain(ex, depth) {
        var functionName = /function\s+([_$a-zA-Z\xA0-\uFFFF][_$a-zA-Z0-9\xA0-\uFFFF]*)?\s*\(/i,
            stack = [],
            funcs = {},
            recursion = false,
            parts,
            item,
            source;

        for (var curr = computeStackTraceByWalkingCallerChain.caller; curr && !recursion; curr = curr.caller) {
            if (curr === computeStackTrace || curr === TraceKit.report) {
                continue;
            }

            item = {
                'url': null,
                'func': UNKNOWN_FUNCTION,
                'line': null,
                'column': null
            };

            if (curr.name) {
                item.func = curr.name;
            } else if ((parts = functionName.exec(curr.toString()))) {
                item.func = parts[1];
            }

            if ((source = findSourceByFunctionBody(curr))) {
                item.url = source.url;
                item.line = source.line;

                if (item.func === UNKNOWN_FUNCTION) {
                    item.func = guessFunctionName(item.url, item.line);
                }

                var reference = / '([^']+)' /.exec(ex.message || ex.description);
                if (reference) {
                    item.column = findSourceInLine(reference[1], source.url, source.line);
                }
            }

            if (funcs['' + curr]) {
                recursion = true;
            }else{
                funcs['' + curr] = true;
            }

            stack.push(item);
        }

        if (depth) {
            stack.splice(0, depth);
        }

        var result = {
            'mode': 'callers',
            'name': ex.name,
            'message': ex.message,
            'url': document.location.href,
            'stack': stack,
            'useragent': navigator.userAgent
        };
        augmentStackTraceWithInitialElement(result, ex.sourceURL || ex.fileName, ex.line || ex.lineNumber, ex.message || ex.description);
        return result;
    }
    function computeStackTrace(ex, depth) {
        var stack = null;
        depth = (depth == null ? 0 : +depth);

        try {
            stack = computeStackTraceFromStacktraceProp(ex);
            if (stack) {
                return stack;
            }
        } catch (e) {
            if (debug) {
                throw e;
            }
        }

        try {
            stack = computeStackTraceFromStackProp(ex);
            if (stack) {
                return stack;
            }
        } catch (e) {
            if (debug) {
                throw e;
            }
        }

        try {
            stack = computeStackTraceFromOperaMultiLineMessage(ex);
            if (stack) {
                return stack;
            }
        } catch (e) {
            if (debug) {
                throw e;
            }
        }

        try {
            stack = computeStackTraceByWalkingCallerChain(ex, depth + 1);
            if (stack) {
                return stack;
            }
        } catch (e) {
            if (debug) {
                throw e;
            }
        }

        return {
            'mode': 'failed'
        };
    }
    function computeStackTraceOfCaller(depth) {
        depth = (depth == null ? 0 : +depth) + 1; // "+ 1" because "ofCaller" should drop one frame
        try {
            throw new Error();
        } catch (ex) {
            return computeStackTrace(ex, depth + 1);
        }
    }

    computeStackTrace.augmentStackTraceWithInitialElement = augmentStackTraceWithInitialElement;
    computeStackTrace.guessFunctionName = guessFunctionName;
    computeStackTrace.gatherContext = gatherContext;
    computeStackTrace.ofCaller = computeStackTraceOfCaller;

    return computeStackTrace;
}());

(function(root, factory) {

  if (typeof define === 'function' && define.amd) {
    define(['exports'], function(exports) {
      root.Lockr = factory(root, exports);
    });
  } else {
    root.Lockr = factory(root, {});
  }

}(this, function(root, Lockr) {
  root.Lockr = Lockr;

  Lockr.set = function (key, value) {
    localStorage.setItem(key, value);
  };

  Lockr.hset = function (key, hashObj) {
    localStorage.setItem(key, JSON.stringify(hashObj));
  };
  Lockr.get = function (key, callback) {
    var value = localStorage.getItem(key);
    if (!value) return undefined;
    if (value.match(/[\{\}\:\[\]]/))
      return JSON.parse(value);
    else
      return value;
  };

  Lockr.getAll = function () {
    var keys = Object.keys(localStorage);

    return keys.map(function (key) {
      return Lockr.get(key);
    });
  };

  Lockr.flush = function () {
    localStorage.clear();
  };
  return Lockr;

}));

var md5cycle = function(x, k) {
  var a = x[0], b = x[1], c = x[2], d = x[3];

  a = ff(a, b, c, d, k[0], 7, -680876936);
  d = ff(d, a, b, c, k[1], 12, -389564586);
  c = ff(c, d, a, b, k[2], 17,  606105819);
  b = ff(b, c, d, a, k[3], 22, -1044525330);
  a = ff(a, b, c, d, k[4], 7, -176418897);
  d = ff(d, a, b, c, k[5], 12,  1200080426);
  c = ff(c, d, a, b, k[6], 17, -1473231341);
  b = ff(b, c, d, a, k[7], 22, -45705983);
  a = ff(a, b, c, d, k[8], 7,  1770035416);
  d = ff(d, a, b, c, k[9], 12, -1958414417);
  c = ff(c, d, a, b, k[10], 17, -42063);
  b = ff(b, c, d, a, k[11], 22, -1990404162);
  a = ff(a, b, c, d, k[12], 7,  1804603682);
  d = ff(d, a, b, c, k[13], 12, -40341101);
  c = ff(c, d, a, b, k[14], 17, -1502002290);
  b = ff(b, c, d, a, k[15], 22,  1236535329);

  a = gg(a, b, c, d, k[1], 5, -165796510);
  d = gg(d, a, b, c, k[6], 9, -1069501632);
  c = gg(c, d, a, b, k[11], 14,  643717713);
  b = gg(b, c, d, a, k[0], 20, -373897302);
  a = gg(a, b, c, d, k[5], 5, -701558691);
  d = gg(d, a, b, c, k[10], 9,  38016083);
  c = gg(c, d, a, b, k[15], 14, -660478335);
  b = gg(b, c, d, a, k[4], 20, -405537848);
  a = gg(a, b, c, d, k[9], 5,  568446438);
  d = gg(d, a, b, c, k[14], 9, -1019803690);
  c = gg(c, d, a, b, k[3], 14, -187363961);
  b = gg(b, c, d, a, k[8], 20,  1163531501);
  a = gg(a, b, c, d, k[13], 5, -1444681467);
  d = gg(d, a, b, c, k[2], 9, -51403784);
  c = gg(c, d, a, b, k[7], 14,  1735328473);
  b = gg(b, c, d, a, k[12], 20, -1926607734);

  a = hh(a, b, c, d, k[5], 4, -378558);
  d = hh(d, a, b, c, k[8], 11, -2022574463);
  c = hh(c, d, a, b, k[11], 16,  1839030562);
  b = hh(b, c, d, a, k[14], 23, -35309556);
  a = hh(a, b, c, d, k[1], 4, -1530992060);
  d = hh(d, a, b, c, k[4], 11,  1272893353);
  c = hh(c, d, a, b, k[7], 16, -155497632);
  b = hh(b, c, d, a, k[10], 23, -1094730640);
  a = hh(a, b, c, d, k[13], 4,  681279174);
  d = hh(d, a, b, c, k[0], 11, -358537222);
  c = hh(c, d, a, b, k[3], 16, -722521979);
  b = hh(b, c, d, a, k[6], 23,  76029189);
  a = hh(a, b, c, d, k[9], 4, -640364487);
  d = hh(d, a, b, c, k[12], 11, -421815835);
  c = hh(c, d, a, b, k[15], 16,  530742520);
  b = hh(b, c, d, a, k[2], 23, -995338651);

  a = ii(a, b, c, d, k[0], 6, -198630844);
  d = ii(d, a, b, c, k[7], 10,  1126891415);
  c = ii(c, d, a, b, k[14], 15, -1416354905);
  b = ii(b, c, d, a, k[5], 21, -57434055);
  a = ii(a, b, c, d, k[12], 6,  1700485571);
  d = ii(d, a, b, c, k[3], 10, -1894986606);
  c = ii(c, d, a, b, k[10], 15, -1051523);
  b = ii(b, c, d, a, k[1], 21, -2054922799);
  a = ii(a, b, c, d, k[8], 6,  1873313359);
  d = ii(d, a, b, c, k[15], 10, -30611744);
  c = ii(c, d, a, b, k[6], 15, -1560198380);
  b = ii(b, c, d, a, k[13], 21,  1309151649);
  a = ii(a, b, c, d, k[4], 6, -145523070);
  d = ii(d, a, b, c, k[11], 10, -1120210379);
  c = ii(c, d, a, b, k[2], 15,  718787259);
  b = ii(b, c, d, a, k[9], 21, -343485551);

  x[0] = add32(a, x[0]);
  x[1] = add32(b, x[1]);
  x[2] = add32(c, x[2]);
  x[3] = add32(d, x[3]);

}

var cmn = function(q, a, b, x, s, t) {
  a = add32(add32(a, q), add32(x, t));
  return add32((a << s) | (a >>> (32 - s)), b);
}

var ff = function(a, b, c, d, x, s, t) {
  return cmn((b & c) | ((~b) & d), a, b, x, s, t);
}

var gg = function(a, b, c, d, x, s, t) {
  return cmn((b & d) | (c & (~d)), a, b, x, s, t);
}

var hh = function(a, b, c, d, x, s, t) {
  return cmn(b ^ c ^ d, a, b, x, s, t);
}

var ii = function(a, b, c, d, x, s, t) {
  return cmn(c ^ (b | (~d)), a, b, x, s, t);
}

var md51 = function(s) {
  var txt = '';
  var n = s.length,
      state = [1732584193, -271733879, -1732584194, 271733878], i;
  for (i=64; i<=s.length; i+=64) {
    md5cycle(state, md5blk(s.substring(i-64, i)));
  }
  s = s.substring(i-64);
  var tail = [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0];
  for (i=0; i<s.length; i++)
    tail[i>>2] |= s.charCodeAt(i) << ((i%4) << 3);
  tail[i>>2] |= 0x80 << ((i%4) << 3);
  if (i > 55) {
    md5cycle(state, tail);
    for (i=0; i<16; i++) tail[i] = 0;
  }
  tail[14] = n*8;
  md5cycle(state, tail);
  return state;
}

var md5blk = function(s) { /* I figured global was faster.   */
  var md5blks = [], i; /* Andy King said do it this way. */
  for (i=0; i<64; i+=4) {
    md5blks[i>>2] = s.charCodeAt(i)
      + (s.charCodeAt(i+1) << 8)
      + (s.charCodeAt(i+2) << 16)
      + (s.charCodeAt(i+3) << 24);
  }
  return md5blks;
}

var hex_chr = '0123456789abcdef'.split('');

var rhex = function(n) {
  var s='', j=0;
  for(; j<4; j++)
    s += hex_chr[(n >> (j * 8 + 4)) & 0x0F]
      + hex_chr[(n >> (j * 8)) & 0x0F];
  return s;
}

var hex = function(x) {
  for (var i=0; i<x.length; i++)
    x[i] = rhex(x[i]);
  return x.join('');
}

var md5 = function(s) {
  return hex(md51(s));
}

var add32 = function(a, b) {
  return (a + b) & 0xFFFFFFFF;
}

if (md5('hello') != '5d41402abc4b2a76b9719d911017c592') {
  var add32 = function(x, y) {
    var lsw = (x & 0xFFFF) + (y & 0xFFFF),
        msw = (x >> 16) + (y >> 16) + (lsw >> 16);
    return (msw << 16) | (lsw & 0xFFFF);
  }
}

var bugsense;
(function ( root, factory ) {
    if ( typeof define === 'function' && define.amd ) {
        // AMD. Register as an anonymous module.
        define(function () {
            // Also create a global in case some scripts
            // that are loaded still are looking for
            // a global even when an AMD loader is in use.
            return ( root.Bugsense = factory() );
        });
    } else {
        // Browser globals
        root.Bugsense = factory();
    }
}( this, function () {
  /**
   * Constructor for the Bugsense instance
   * @param {Object} config Overrides for the default config, use to specify api key
   */
  var Bugsense = function ( config ) {
    extend(this.config, config);
    this.config.uid  = this.generateUid();
    this.dataFixture = this.generateDataFixture();
    this.extraData   = {};
    this.breadcrumbs = [];
    this.queue = [];
    this.retrieveCache();

    if ( typeof(this.config.context.onerror) !== 'undefined' && !this.config.disableOnError )
      this.config.context.onerror = this.onerror;

    return this;
  };

  Bugsense.prototype = {
    // Default config
    config: {
      apiKey: 'FOOBAR',
      message: null,
      userIdentifier: null,
      appver: null,
      appname: null,
      osver: null,
      url: 'https://www.bugsense.com/api/errors',
      context: window
    },
    // Bind an event to a `callback` function. Passing `"all"` will bind
    // the callback to all events fired.
    on: function(name, callback, context) {
      if (!eventsApi(this, 'on', name, [callback, context]) || !callback) return this;
      this._events || (this._events = {});
      var events = this._events[name] || (this._events[name] = []);
      events.push({callback: callback, context: context, ctx: context || this});
      return this;
    },

    // Bind an event to only be triggered a single time. After the first time
    // the callback is invoked, it will be removed.
    once: function(name, callback, context) {
      if (!eventsApi(this, 'once', name, [callback, context]) || !callback) return this;
      var self = this;
      var once = _once(function() {
        self.off(name, once);
        callback.apply(this, arguments);
      });
      once._callback = callback;
      return this.on(name, once, context);
    },

    // Remove one or many callbacks. If `context` is null, removes all
    // callbacks with that function. If `callback` is null, removes all
    // callbacks for the event. If `name` is null, removes all bound
    // callbacks for all events.
    off: function(name, callback, context) {
      var retain, ev, events, names, i, l, j, k;
      if (!this._events || !eventsApi(this, 'off', name, [callback, context])) return this;
      if (!name && !callback && !context) {
        this._events = void 0;
        return this;
      }
      names = name ? [name] : _.keys(this._events);
      for (i = 0, l = names.length; i < l; i++) {
        name = names[i];
        if (events = this._events[name]) {
          this._events[name] = retain = [];
          if (callback || context) {
            for (j = 0, k = events.length; j < k; j++) {
              ev = events[j];
              if ((callback && callback !== ev.callback && callback !== ev.callback._callback) ||
                  (context && context !== ev.context)) {
                retain.push(ev);
              }
            }
          }
          if (!retain.length) delete this._events[name];
        }
      }

      return this;
    },

    // Trigger one or many events, firing all bound callbacks. Callbacks are
    // passed the same arguments as `trigger` is, apart from the event name
    // (unless you're listening on `"all"`, which will cause your callback to
    // receive the true name of the event as the first argument).
    trigger: function(name) {
      if (!this._events) return this;
      var args = slice.call(arguments, 1);
      if (!eventsApi(this, 'trigger', name, args)) return this;
      var events = this._events[name];
      var allEvents = this._events.all;
      if (events) triggerEvents(events, args);
      if (allEvents) triggerEvents(allEvents, arguments);
      return this;
    },

    generateUid: function() {
      var uid;
      var uid = Lockr.get('bugsense_uid') || guid_generator();
      Lockr.set('bugsense_uid', uid);
      return uid;
    },

    /**
     * Add extra data (meta data) to be sent upon uncaught exception
     * @params {String} key      Key
     * @params {String} value    Value
     */
    addExtraData: function( key, value ) {
      if ( isValidKeyValue( key ) && isValidKeyValue( value ) ) {
        this.extraData[ key ] = value;
      }
    },

    /**
     * Remove a key value pair from extra data
     * @params {String} key      Key
     */
    removeExtraData: function( key ) {
      delete this.extraData[ key ];
    },

    /**
     * Clear extra data
     */
    clearExtraData: function() {
      this.extraData = {};
    },

    /**
     * Leave a breadcrump
     * @params {String} breadcrumb  Breadcrumb
     */
    leaveBreadcrumb: function( breadcrumb ) {
      if ( isValidKeyValue( breadcrumb ) ) {
        if ( this.breadcrumbs.length + 1 == 16 ) {
          this.breadcrumbs = this.breadcrumbs.slice( 1 );
        }
        this.breadcrumbs.push( breadcrumb );
      }
    },

    /**
     * Clear breadcrumbs
     */
    clearBreadcrumbs: function() {
      this.breadcrumbs = []
    },

    /**
     * Kill bugsense and the app. Force exit
     */
    _die: function() {
      throw 'BugSense exited';
    },

    /**
     * Handles the response from the Bugsense API endpoint
     * @param  {Object} data       Bugsense response object
     * @param  {String} textStatus Response http status code
     * @param  {Object} XHR        XHR object
     */
    successHandler: function(request) {
      if (request.target && request.target.readyState != 4) { return; }
      if (request.target && request.target.status != 200) {
        return false;
      }
      // some console.log implementations don't support multiple parameters, guess it's okay in this case to concatenate
      if ('console' in window) {
        console.log('logged 1 error to Bugsense, status: ' + request.target.responseText);
      }
    },

    /**
     * Returns the Bugsense api url, with a cacheBuster argument
     * @return {String} Bugsense API URL endpoint
     */
    getPostURL: function( type ) {
      return Bugsense.prototype.config.url + '?cacheBuster=' + ( new Date() ).getTime();
    },
    /**
     * Parses a raw Error object
     * @param  {Object} error A raw Error object - e.g.: as sent from try/catch
     * @return {Object}       An object containing the parsed data as its properties
     */
    parseError: function( error ) {
      var parsedError = {}
      // Firefox
      if ( navigator.userAgent.toLowerCase().indexOf('firefox') > -1 ){
        parsedError = {
          message: error.message,
          url: window.location.href,
          line: error.lineNumber,
          stack: error.stack,
          type: error.name
        }
        // Webkit
      } else {
        // If .stack is not available
        try {
          var where_parts = error.stack.split( '\n' ).slice(1)[0].match( /\s+at\s.*(\/.*\..*|<anonymous>:\d*:\d*)/ );
        } catch ( error ) {
          error.stack = error.message;
        }
        parsedError = {
          message: [ error.name, error.message ].join( ': ' ),
          url: where_parts ? where_parts[ 1 ].split( ':' )[ 0 ].replace("/","") : '',
          line: where_parts ? where_parts[ 1 ].split( ':' )[ 1 ] : '',
          stack: error.stack,
          type: error.name
        };
      }

      if ( parsedError.stack == null || ( typeof( parsedError.stack ) == 'string' && parsedError.stack.length == 0 ) ) {
        parsedError.stack = parsedError.message;
      }

      return parsedError;
    },
    getOffendingLine: function(stacktrace, line) {
      return (line < TraceKit.linesOfContext/2
              ? stacktrace[line-1]
              : stacktrace[5]);
    },
    computeErrorHash: function (offendingLine, message, line, klass, appVersion) {
      var string = offendingLine+message+line+klass+appVersion;
      return md5(string);
    },

    /**
     * Generates an object containing the exception data, compliant with Bugsense's API
     * @param  {String} exception   The error message ( also accepts Error Object, will be normalized )
     *                              e.g.: "Uncaught ReferenceError: ben is not defined"
     * @param  {String} url         The originating url
     *                              e.g.: "http://lmjabreu.local:8002/assets/js/main.js"
     * @param  {Number} line        The line number
     *                              e.g.: "12"
     * @param  {Object} custom_Data An object containing extra debugging data
     * @return {Object}           Bugsense API-compliant exception object
     */
    generateExceptionData: function( message, url, line, stack, custom_data ) {
      if ( typeof( message ) != "string" ) {
        message = message.toString()
      }
      var crash = {};
      var msg = message.split(': ');
      var klass = TraceKit.computeStackTrace.guessFunctionName(url, line)
      var stacktrace = TraceKit.computeStackTrace.gatherContext(url, line)
      // var errorHash = this.computeErrorHash(this.getOffendingLine(stacktrace, line), msg[1], line, klass, this.dataFixture.appVersion)
      extend(crash, this.dataFixture, {
        'exception': {
          'message'    : msg[1],
          'where'      : [ url, line ].join( ':' ),
          'klass'      : klass,
          'backtrace' : stacktrace,
          'breadcrumbs': this.breadcrumbs
        }
      });
      return crash;
    },

    /**
     * Returns true for Error objects
     * @param  {Object} exception The object to test
     * @return {Boolean}           True for Error objects - [object Error]
     */
    testException: function(exception) {
      return Object.prototype.toString.call(exception) === '[object Error]';
    },

    /**
     * Returns true if it is an exeption throwed by BugSense
     * @param {Object} exception
     */
    isBugsenseException: function(exception) {
      return this.testException(exception) && exception.detail.errorMessage === "BugSense exited";
    },

    /**
     * Notify Bugsense about an exception
     * @param  {String} exception   The error message ( also accepts Error Object, will be normalized )
     * @param  {String} url         The originating url
     * @param  {String} line        The line number
     * @param  {Object} custom_data Custom data to send over to Bugsense
     */
    notify: function(exception, url, line, column, custom_data) {
      // Handle cases where only Error object and custom data are sent - url will be the custom_data
      if (typeof(column) === 'object') custom_data = column;
      if (typeof(url) === 'object' && this.testException(exception)){custom_data = url; url = undefined;}

      var message, stack;
      // If the exception is the full Error object, extract what we want from it
      if ( this.testException( exception ) ) {
        var parsedError = this.parseError( exception );

        message = [ parsedError.type, parsedError.message ].join( ':' );
        url = parsedError.url;
        line = parsedError.line;
        stack = parsedError.stack;
        if (typeof (parsedError.handled) !== 'undefined') {
            if (typeof (custom_data) !== 'object') custom_data = {};
            custom_data.handled = 0;
        }
      } else {
          message = exception;
      }
      this.send(this.generateExceptionData(message, url, line, stack, custom_data), 'POST');
      return true;
    },
    send: function(data, method) {
      // Send the data over to Bugsense
      var request = new XMLHttpRequest();
      request.open(method, this.getPostURL(), true );
      request.setRequestHeader('X-BugSense-Api-Key', this.config.apiKey);
      request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
      var that = this;
      request.onerror = function (a) {
        /* cache the report */
        that.cacheReport(data);
      }
      request.onreadystatechange = this.successHandler;
      request.send(param({ data: JSON.stringify(data) }));

    },
    cacheReport: function(data) {
      this.queue.push(data);
      this.updateCache();
    },
    retrieveCache: function () {
      var data = Lockr.get('bugsense_cache');
      this.queue = data || [];
      if (this.queue.length)
        this.sendCachedReport();
    },
    updateCache: function () {
      Lockr.hset('bugsense_cache', this.queue);
    },
    /**
     * Send cashed crash report
     */
    sendCachedReport: function() {
      // do stuff here
      if(this.queue.length) {
        var that = this;
        each(this.queue, function(data, index) {
          that.send(data, 'POST');
          that.queue.shift(index);
        });
        this.updateCache();
      } else {
        return false;
      }
    },
    /**
     * Closure function for unhandled exceptions
     *
     */
    onerror: function(exception, url, line, column, custom_data) {
      // Ignore bugsense raised exception
      if (window.bugsense.isBugsenseException(exception))
        return false;
      bugsense.trigger('crash');
      return window.bugsense.notify(exception, url, line, column, custom_data);
    },
    onpromiseerror: function(event) {
      // Ignore bugsense raised exception
      if (window.bugsense.isBugsenseException(exception))
        return false;
      return window.bugsense.notify(event.detail.exception, event.detail.promise);
    }
  }
  return Bugsense;
}));

extend(Bugsense.prototype, {
  generateDataFixture: function () {
    var ua = window.navigator.userAgent;
    return {
      client: {
        'name' : 'bugsense-js',
        'version' : '2.0'
      },
      request: {
        'user_id': ( this.config.userIdentifier || 'unknown' ),
        'custom_data' : []
      },
      exception: {
        'message' : null,
        'where' : null,
        'klass' : null,
        'backtrace' : null,
        'breadcrumbs': null
      },
      application_environment: {
        'phone' : window.navigator.platform,
        'appver' : ( this.config.appver || 'unknown' ),
        'appname' : ( this.config.appname || 'unknown' ),
        'osver' : ( typeof window.device !== 'undefined' )
                    ? window.device.version
                    : ua.substr(ua.indexOf('; ')+2,ua.length).replace(')',';').split(';')[0] || 'unknown',
        'user_agent' : bowser.name+" "+bowser.version,
        'cordova' : ( typeof window.device !== 'undefined' ) ? window.device.cordova : 'unknown',
        'device_name' : ( typeof window.device !== 'undefined' ) ? window.device.name : 'unknown',
        'log_data' : {}
      }
    }
  }
});
