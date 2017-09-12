#ifndef __DEV_RTC_H_INCLUDED__
#define __DEV_RTC_H_INCLUDED__

void rtcInit();
void rtcDeInit();

void rtcSetAlarm(uint16_t timeSleep_sec, void (*func)(void));

#endif /* __DEV_RTC_H_INCLUDED__ */
