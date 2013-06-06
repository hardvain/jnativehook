/* JNativeHook: Global keyboard and mouse hooking for Java.
 * Copyright (C) 2006-2013 Alexander Barker.  All Rights Received.
 * http://code.google.com/p/jnativehook/
 *
 * JNativeHook is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * JNativeHook is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <stdbool.h>
#include <stdio.h>
#include <X11/Xlib.h>

#ifdef XKB
#include <X11/XKBlib.h>
#endif

#ifdef XF86MISC
#include <X11/extensions/xf86misc.h>
#include <X11/extensions/xf86mscstr.h>
#endif

#ifdef XT
#include <X11/Intrinsic.h>

static XtAppContext app_context;
#endif

extern Display *disp;

long int hook_get_auto_repeat_rate() {
	bool successful = false;
	long int value = -1;
	unsigned int delay = 0, rate = 0;

	#ifdef XKB
	// Attempt to acquire the keyboard auto repeat rate using the XKB extension.
	if (!successful) {
		successful = XkbGetAutoRepeatRate(disp, XkbUseCoreKbd, &delay, &rate);
	}
	#endif

	#ifdef XF86MISC
	// Fallback to the XF86 Misc extension if available and other efforts failed.
	if (!successful) {
		XF86MiscKbdSettings kb_info;
		successful = (bool) XF86MiscGetKbdSettings(disp, &kb_info);
		if (successful) {
			delay = (unsigned int) kbdinfo.delay;
			rate = (unsigned int) kbdinfo.rate;
		}
	}
	#endif

	if (successful) {
		value = (long int) delay;
	}

	(void) rate;
	return value;
}

long int hook_get_auto_repeat_delay() {
	bool successful = false;
	long int value = -1;
	unsigned int kb_delay = 0, kb_rate = 0;

	#ifdef XKB
	// Attempt to acquire the keyboard auto repeat rate using the XKB extension.
	if (!successful) {
		successful = XkbGetAutoRepeatRate(disp, XkbUseCoreKbd, &kb_delay, &kb_rate);
	}
	#endif

	#ifdef XF86MISC
	// Fallback to the XF86 Misc extension if available and other efforts failed.
	if (!successful) {
		XF86MiscKbdSettings kb_info;
		successful = (bool) XF86MiscGetKbdSettings(disp, &kb_info);
		if (successful) {
			kb_delay = (unsigned int) kbdinfo.delay;
			kb_rate = (unsigned int) kbdinfo.rate;
		}
	}
	#endif

	if (successful) {
		value = (long int) kb_rate;
	}

	(void) kb_delay;
	return value;
}

long int hook_get_pointer_acceleration_multiplier() {
	long int value = -1;
	int accel_numerator, accel_denominator, threshold;

	XGetPointerControl(disp, &accel_numerator, &accel_denominator, &threshold);
	if (accel_denominator >= 0) {
		value = (long int) accel_denominator;
	}
	(void) accel_numerator;
	(void) threshold;

	return value;
}

long int hook_get_pointer_acceleration_threshold() {
	long int value = -1;
	int accel_numerator, accel_denominator, threshold;

	XGetPointerControl(disp, &accel_numerator, &accel_denominator, &threshold);
	if (threshold >= 0) {
		value = (long int) threshold;
	}
	(void) accel_numerator;
	(void) accel_denominator;

	return value;
}

long int hook_get_pointer_sensitivity() {
	long int value = -1;
	int accel_numerator, accel_denominator, threshold;

	XGetPointerControl(disp, &accel_numerator, &accel_denominator, &threshold);
	if (accel_numerator >= 0) {
		value = (long int) accel_numerator;
	}
	(void) accel_denominator;
	(void) threshold;

	return value;
}

long int hook_get_multi_click_time() {
	long int value = 200;
	int click_time;
	bool successful = false;

	// Try and acquire the multi-click time from the user defined X defaults.
	char *xprop = XGetDefault(disp, "*", "multiClickTime");
	if (xprop != NULL && sscanf(xprop, "%4i", &click_time) != EOF) {
		successful = true;
	}

	xprop = XGetDefault(disp, "OpenWindows", "MultiClickTimeout");
	if (xprop != NULL && sscanf(xprop, "%4i", &click_time) != EOF) {
		successful = true;
	}

	#ifdef XT
	int argc = 0;
	char ** argv = {NULL};

	// Fall back to the X Toolkit extension if available and other efforts failed.
    XtDisplayInitialize(app_context, disp, "JNativeHook", "JNativeHook", NULL, 0, &argc, argv);
	if (!successful) {
		click_time = XtGetMultiClickTime(disp);
		if (click_time >= 0) {
			successful = true;
		}
	}
	#endif

	if (successful) {
		value = (long int) click_time;
	}

	return value;
}