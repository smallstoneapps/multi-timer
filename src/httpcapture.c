/* ===========================================================================
 
 Copyright (c) 2013 Edward Patel
 
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
 
 =========================================================================== */

#include "pebble_os.h"
#include "pebble_app.h"

#include "httpcapture.h"

#define http_capture_COOKIE_send  0x70626c63 // 'pblc'
#define http_capture_COOKIE_start 0x70626c64 // 'pbld'

static void *http_capture_app_context_ref;
static bool http_capture_sending = false;
static unsigned char http_capture_frameBuffer[18*168];
static int http_capture_sentLen;
static void (*http_capture_timer_next_handler)(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie);

static void http_capture_send_buffer() {
	int len = 64;
	if (http_capture_sentLen+len > 18*168)
		len = 18*168 - http_capture_sentLen;
	if (len <= 0)
		return;
		
	Tuplet start = TupletInteger(0xFFF9, http_capture_sentLen);
	Tuplet buf = TupletBytes(1000, &http_capture_frameBuffer[http_capture_sentLen], len);

	DictionaryIterator *iter;
	app_message_out_get(&iter);

	if (iter == NULL)
		return;

	dict_write_tuplet(iter, &start);
	dict_write_tuplet(iter, &buf);
	dict_write_end(iter);

	http_capture_sentLen += len;

	app_message_out_send();
	app_message_out_release();	
}

void http_capture_out_sent(DictionaryIterator *sent, void *context) {
	if (http_capture_sending && http_capture_sentLen < 18*168)
		app_timer_send_event(http_capture_app_context_ref, 10, http_capture_COOKIE_send);
	else
		http_capture_sending = false;
}

// The *hack* to get to the framebuffer
struct GContext {
	void **ptr;
};

static void http_capture_make_framebuffer_copy() {
	struct GContext *gctx = app_get_current_graphics_context();
	unsigned char *ptr = (unsigned char *)(*gctx->ptr);
	int len = 0;
	http_capture_sentLen = 0;
	for (int y=0; y<168; y++) {
		for (int x=0; x<18; x++) {
			http_capture_frameBuffer[len++] = *ptr++;
		}
		ptr++; ptr++;
	}
}
 
static void http_capture_timer_handler(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie) {
	if (cookie == http_capture_COOKIE_send) {
		http_capture_send_buffer();
	} else if (cookie == http_capture_COOKIE_start) {
		http_capture_make_framebuffer_copy();
		http_capture_send_buffer();
	} else if (http_capture_timer_next_handler) {
		http_capture_timer_next_handler(app_ctx, handle, cookie);
	}
}

// ----------------------------------------------------------------------
// External API

void http_capture_send(int wait) {
	if (http_capture_sending)
		return;
	if (wait) {
		http_capture_sending = true;
		app_timer_send_event(http_capture_app_context_ref, wait, http_capture_COOKIE_start);
	} else {
		http_capture_make_framebuffer_copy();
		http_capture_sending = true;
		app_timer_send_event(http_capture_app_context_ref, 10, http_capture_COOKIE_send);
	}
}

void http_capture_init(AppContextRef app_ctx) {
	http_capture_app_context_ref = app_ctx;
}

void http_capture_main(PebbleAppHandlers *handlers) {
	http_capture_timer_next_handler = handlers->timer_handler;
	handlers->timer_handler = http_capture_timer_handler;
}
