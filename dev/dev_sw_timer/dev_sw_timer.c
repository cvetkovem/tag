#include <stdlib.h>
#include "dev_hw_timer.h"
#include "timer.h"
#include "dev_cpu.h"

/*!
 * Timers list head pointer
 */
static TimerEvent_t *TimerListHead = NULL;

/*!
 * \brief Adds or replace the head timer of the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be become the new head
 * \param [IN]  remainingTime Remaining time of the previous head to be replaced
 */
static void TimerInsertNewHeadTimer(TimerEvent_t *obj, uint32_t remainingTime);

/*!
 * \brief Adds a timer to the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be added to the list
 * \param [IN]  remainingTime Remaining time of the running head after which the object may be added
 */
static void TimerInsertTimer(TimerEvent_t *obj, uint32_t remainingTime);

/*!
 * \brief Sets a timeout with the duration "timestamp"
 *
 * \param [IN] timestamp Delay duration
 */
static void TimerSetTimeout(TimerEvent_t *obj);

/*!
 * \brief Check if the Object to be added is not already in the list
 *
 * \param [IN] timestamp Delay duration
 * \retval true (the object is already in the list) or false
 */
static bool TimerExists(TimerEvent_t *obj);

/*!
 * \brief Read the timer value of the currently running timer
 *
 * \retval value current timer value
 */
uint32_t TimerGetValue(void);

void TimerInit(TimerEvent_t *obj, void (*callback)(void))
{
    obj->Timestamp = 0;
    obj->ReloadValue = 0;
    obj->IsRunning = false;
    obj->Callback = callback;
    obj->Next = NULL;
}

void TimerStart(TimerEvent_t *obj)
{
    uint32_t elapsedTime = 0;
    uint32_t remainingTime = 0;

    cpuDisable_irq();

    if((obj == NULL) || (TimerExists(obj) == true)) {
        cpuEnable_irq();
        return;
    }

    obj->Timestamp = obj->ReloadValue;
    obj->IsRunning = false;

    if(TimerListHead == NULL) {
        TimerInsertNewHeadTimer(obj, obj->Timestamp);
    } else {
        /* if now timer run, calculate remaining time */
        if(TimerListHead->IsRunning == true) {
            elapsedTime = TimerGetValue();
            if(elapsedTime > TimerListHead->Timestamp) {
                elapsedTime = TimerListHead->Timestamp; // security but should never occur
            }
            remainingTime = TimerListHead->Timestamp - elapsedTime;
        } else {
            remainingTime = TimerListHead->Timestamp;
        }

        if(obj->Timestamp < remainingTime) {
            TimerInsertNewHeadTimer(obj, remainingTime);
        } else {
             TimerInsertTimer(obj, remainingTime);
        }
    }

    cpuEnable_irq();
}

static void TimerInsertTimer(TimerEvent_t *obj, uint32_t remainingTime)
{
    uint32_t aggregatedTimestamp = 0;      // hold the sum of timestamps
    uint32_t aggregatedTimestampNext = 0;  // hold the sum of timestamps up to the next event

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead->Next;

    if(cur == NULL) { // obj comes just after the head
        obj->Timestamp -= remainingTime;
        prev->Next = obj;
        obj->Next = NULL;
    } else {
        aggregatedTimestamp = remainingTime;
        aggregatedTimestampNext = remainingTime + cur->Timestamp;

        while(prev != NULL) {
            if(aggregatedTimestampNext > obj->Timestamp) {
                obj->Timestamp -= aggregatedTimestamp;
                if(cur != NULL) {
                    cur->Timestamp -= obj->Timestamp;
                }
                prev->Next = obj;
                obj->Next = cur;
                break;
            } else {
                prev = cur;
                cur = cur->Next;
                if(cur == NULL) { // obj comes at the end of the list
                    aggregatedTimestamp = aggregatedTimestampNext;
                    obj->Timestamp -= aggregatedTimestamp;
                    prev->Next = obj;
                    obj->Next = NULL;
                    break;
                } else {
                    aggregatedTimestamp = aggregatedTimestampNext;
                    aggregatedTimestampNext = aggregatedTimestampNext + cur->Timestamp;
                }
            }
        }
    }
}

static void TimerInsertNewHeadTimer(TimerEvent_t *obj, uint32_t remainingTime)
{
    TimerEvent_t* cur = TimerListHead;

    if(cur != NULL) {
        cur->Timestamp = remainingTime - obj->Timestamp;
        cur->IsRunning = false;
    }

    obj->Next = cur;
    obj->IsRunning = true;
    TimerListHead = obj;
    TimerSetTimeout(TimerListHead);
}

void TimerIrqHandler(void)
{
    uint32_t elapsedTime = 0;

    if(TimerListHead == NULL) {
            return;  // Only necessary when the standard timer is used as a time base
    }

    elapsedTime = TimerGetValue();

    if(elapsedTime > TimerListHead->Timestamp) {
        TimerListHead->Timestamp = 0;
    } else {
        TimerListHead->Timestamp -= elapsedTime;
    }

    while((TimerListHead != NULL) && (TimerListHead->Timestamp == 0)) {
        TimerEvent_t* elapsedTimer = TimerListHead;
        TimerListHead = TimerListHead->Next;

        if(elapsedTimer->Callback != NULL) {
            elapsedTimer->Callback();
        }
    }

    // start the next TimerListHead if it exists
    if(TimerListHead != NULL) {
        TimerListHead->IsRunning = true;
        TimerSetTimeout(TimerListHead);
    }
}

void TimerStop(TimerEvent_t *obj)
{
    cpuDisable_irq();

    uint32_t elapsedTime = 0;
    uint32_t remainingTime = 0;

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead;

    // List is empty or the Obj to stop does not exist
    if((TimerListHead == NULL) || (obj == NULL)) {
        cpuEnable_irq();
        return;
    }

    if(TimerListHead == obj) { // Stop the Head
        if(TimerListHead->IsRunning == true) { // The head is already running
            elapsedTime = TimerGetValue();

            if(elapsedTime > obj->Timestamp) {
                elapsedTime = obj->Timestamp;
            }

            remainingTime = obj->Timestamp - elapsedTime;

            if(TimerListHead->Next != NULL) {
                TimerListHead->IsRunning = false;
                TimerListHead = TimerListHead->Next;
                TimerListHead->Timestamp += remainingTime;
                TimerListHead->IsRunning = true;
                TimerSetTimeout(TimerListHead);
            } else {
                TimerListHead = NULL;
            }
        } else { // Stop the head before it is started
            if(TimerListHead->Next != NULL) {
                remainingTime = obj->Timestamp;
                TimerListHead = TimerListHead->Next;
                TimerListHead->Timestamp += remainingTime;
            } else {
                TimerListHead = NULL;
            }
        }
    } else { // Stop an object within the list
        remainingTime = obj->Timestamp;

        while(cur != NULL) {
            if(cur == obj) {
                if(cur->Next != NULL) {
                    cur = cur->Next;
                    prev->Next = cur;
                    cur->Timestamp += remainingTime;
                } else {
                    cur = NULL;
                    prev->Next = cur;
                }
                break;
            } else {
                prev = cur;
                cur = cur->Next;
            }
        }
    }

    cpuEnable_irq();
}

static bool TimerExists(TimerEvent_t *obj)
{
    TimerEvent_t* cur = TimerListHead;

    while(cur != NULL) {
        if(cur == obj) {
            return true;
        }
        cur = cur->Next;
    }

    return false;
}

void TimerReset(TimerEvent_t *obj)
{
    TimerStop(obj);
    TimerStart(obj);
}

void TimerSetValue(TimerEvent_t *obj, uint32_t value)
{
    uint32_t minValue = 0;

    TimerStop(obj);

    minValue = TimerHwGetMinimumTimeout();

    if(value < minValue) {
        value = minValue;
    }

    obj->Timestamp = value;
    obj->ReloadValue = value;
}

uint32_t TimerGetValue(void)
{
    return TimerHwGetElapsedTime();
}

TimerTime_t TimerGetCurrentTime(void)
{
    return TimerHwGetTime();
}

static void TimerSetTimeout(TimerEvent_t *obj)
{
    TimerHwStart(obj->Timestamp);
}
