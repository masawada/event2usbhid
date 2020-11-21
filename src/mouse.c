#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <memory.h>
#include "gadget-hid.h"

/**
 * @fn
 * /dev/input/eventNからイベントを読み込む
 *
 * @param 読み込んだイベントを書き込むポインタ
 */
void getMouseEvent(struct input_event *event) {
	read(0, event, sizeof(*event));
}


/**
 * @fn
 * libcompositeにマウスイベントを書き込む
 * 
 * @param buttons マウスボタンの押され具合
 * @param x X座標の移動量
 * @param y Y座標の移動量
 * @param wheel ホイールの移動量
 */
void sendMouseReport(int buttons, int x, int y, int wheel){
	unsigned char buf[MOUSE_REPORT_SIZE + 1];
	buf[0] = 3;
	buf[1] = buttons;
	buf[2] = x;
	buf[3] = y;
	buf[4] = wheel;

	write(1, buf, MOUSE_REPORT_SIZE + 1);
}


/**
 * @fn
 * マウス処理
 */
void mouse() {
	int x = 0;
	int y = 0;
	int wheel = 0;
	int buttons = 0;

	while(1) {
		struct input_event event;

		getMouseEvent(&event);

		// カーソル/ホイールイベント
		if (event.type == EV_REL) {
			// X座標の移動量をクリッピング
			if (event.code == REL_X) {
				int value;

				if (event.value > 127) {
					value = 127;
				} else {
					value = event.value;
				}

				if (event.value < -127) {
					value = -127;
				} else {
					value = event.value;
				}

				x = value;

			// Y座標の移動量をクリッピング
			} else if(event.code == REL_Y) {
				int value;

				if (event.value > 127) {
					value = 127;
				} else {
					value = event.value;
				}

				if (event.value < -127) {
					value = -127;
				} else {
					value = event.value;
				}

				y = value;

			// ホイールの移動量をクリッピング
			} else if(event.code == 11) {
				int value;

				if (event.value > 127) {
					value = 127;
				} else {
					value = event.value;
				}

				if (event.value < -127) {
					value = -127;
				} else {
					value = event.value;
				}

				// なぜか1カチ=120のため
				wheel = value / 120;

			} 

		// マウスボタンイベント
		} else if(event.type == EV_KEY) {
			if(event.code == BTN_LEFT) {
				if(event.value == 1) {
					buttons |= 1;
				} else {
					buttons &= ~1;
				}
			}

			if(event.code == BTN_RIGHT) {
				if(event.value == 1) {
					buttons |= 2;
				} else {
					buttons &= ~2;
				}
			}

			if(event.code == BTN_MIDDLE) {
				if(event.value == 1) {
					buttons |= 4;
				} else {
					buttons &= ~4;
				}
			}

			// 現状を送信する
			sendMouseReport(buttons, x, y, wheel);

			x = 0;
			y = 0;
			wheel = 0;

		// 同期イベント？
		} else if((event.code == 0) && (event.type == 0) && (event.value == 0)) {

			// 現状を送信する
			sendMouseReport(buttons, x, y, wheel);
			x = 0;
			y = 0;
			wheel = 0;
		}
	}
}