/*
 * Copyright 2008 - 2016 Chris Young <chris@unsatisfactorysoftware.co.uk>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "amiga/os3support.h"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/timer.h>

#include <stdio.h>
#include <stdbool.h>
#include <pbl.h>

#include "utils/errors.h"
#include "utils/log.h"

#include "amiga/misc.h"
#include "amiga/schedule.h"

struct nscallback
{
	struct TimeRequest timereq;
	struct TimeVal tv; /* time we expect the event to occur */
	void *restrict callback;
	void *restrict p;
};

static struct nscallback *tioreq;
struct Device *TimerBase;
#ifdef __amigaos4__
struct TimerIFace *ITimer;
#endif

static APTR restrict pool_nscb = NULL;

static PblHeap *schedule_list;

/**
 * Remove timer event
 *
 * \param  nscb  callback
 *
 * The timer event for the callback is aborted
 */

static void ami_schedule_remove_timer_event(struct nscallback *nscb)
{
	if(!nscb) return;

	if(CheckIO((struct IORequest *)nscb)==NULL)
		AbortIO((struct IORequest *)nscb);

	WaitIO((struct IORequest *)nscb);
}

/**
 * Add timer event
 *
 * \param  nscb  callback
 * \param  t     time in ms
 *
 * NetSurf will be signalled in t ms for this event.
 */

static nserror ami_schedule_add_timer_event(struct nscallback *nscb, int t)
{
	struct TimeVal tv;
	ULONG time_us = t * 1000; /* t converted to �s */

	tv.Seconds = time_us / 1000000;
	tv.Microseconds = time_us % 1000000;

	if(tv.Microseconds >= 1000000) {
		LOG("Microseconds invalid value: %ld", nscb->tv.Microseconds);
	}

	GetSysTime(&nscb->tv);
	AddTime(&nscb->tv, &tv); // now contains time when event occurs (for debug and heap sorting)

	nscb->timereq.Request.io_Command = TR_ADDREQUEST;
	nscb->timereq.Time.Seconds = tv.Seconds; // secs
	nscb->timereq.Time.Microseconds = tv.Microseconds; // micro
	SendIO((struct IORequest *)nscb);

	return NSERROR_OK;
}

/**
 * Locate a scheduled callback
 *
 * \param  callback  callback function
 * \param  p         user parameter, passed to callback function
 * \param  remove    remove callback from the heap
 *
 * A scheduled callback matching both callback and p is returned, or NULL if none present.
 */

static struct nscallback *ami_schedule_locate(void (*callback)(void *p), void *p, bool remove)
{
	PblIterator *iterator;
	struct nscallback *nscb;
	bool found_cb = false;

	/* check there is something on the list */
	if (schedule_list == NULL) return NULL;
	if(pblHeapIsEmpty(schedule_list)) return NULL;

	iterator = pblHeapIterator(schedule_list);

	while ((nscb = pblIteratorNext(iterator)) != -1) {
		if ((nscb->callback == callback) && (nscb->p == p)) {
			if (remove == true) pblIteratorRemove(iterator);
			found_cb = true;
			break;
		}
	};

	pblIteratorFree(iterator);

	if (found_cb == true) return nscb;
		else return NULL;
}

/**
 * Reschedule a callback.
 *
 * \param  nscb  callback
 * \param  t     time in ms
 *
 * The nscallback will be rescheduled for t ms.
 */

static nserror ami_schedule_reschedule(struct nscallback *nscb, int t)
{
	ami_schedule_remove_timer_event(nscb);
	if (ami_schedule_add_timer_event(nscb, t) != NSERROR_OK)
		return NSERROR_NOMEM;

	pblHeapConstruct(schedule_list);
	return NSERROR_OK;
}

/**
 * Unschedule a callback.
 *
 * \param  callback  callback function
 * \param  p         user parameter, passed to callback function
 *
 * All scheduled callbacks matching both callback and p are removed.
 */

static nserror schedule_remove(void (*callback)(void *p), void *p)
{
	struct nscallback *nscb;

	nscb = ami_schedule_locate(callback, p, true);

	if(nscb != NULL) {
		LOG("deleted callback %p", nscb);
		ami_schedule_remove_timer_event(nscb);
		ami_misc_itempool_free(pool_nscb, nscb, sizeof(struct nscallback));
		pblHeapConstruct(schedule_list);
	}

	return NSERROR_OK;
}

static void schedule_remove_all(void)
{
	PblIterator *iterator;
	struct nscallback *nscb;

	if(pblHeapIsEmpty(schedule_list)) return;

	iterator = pblHeapIterator(schedule_list);

	while ((nscb = pblIteratorNext(iterator)) != -1)
	{
		ami_schedule_remove_timer_event(nscb);
		pblIteratorRemove(iterator);
		ami_misc_itempool_free(pool_nscb, nscb, sizeof(struct nscallback));
	};

	pblIteratorFree(iterator);
}

static int ami_schedule_compare(const void *prev, const void *next)
{
	struct nscallback *nscb1 = *(struct nscallback **)prev;
	struct nscallback *nscb2 = *(struct nscallback **)next;

	/**\todo a heap probably isn't the best idea now */
	return CmpTime(&nscb1->tv, &nscb2->tv);
}


/**
 * Process signalled event
 *
 * This implementation only processes the callback that arrives in the message from timer.device.
 */
static bool ami_scheduler_run(struct nscallback *nscb)
{
	void (*callback)(void *p);
	void *p;

	LOG("callback %p", nscb);
	
	/*** vvv Debugging vvv ***/
	struct TimeVal tv;
	GetSysTime(&tv);
	if(CmpTime(&tv, &nscb->tv) > 0) {
		LOG("Expected scheduled time of event has not passed: %ld.%ld < %ld.%ld", tv.Seconds, tv.Microseconds, nscb->tv.Seconds, nscb->tv.Microseconds);
	}
	/*** ^^^ Debugging ^^^ ***/
	
	callback = nscb->callback;
	p = nscb->p;

	schedule_remove(callback, p); /* this does a lookup as we don't know if we're the first item on the heap */

	LOG("Running scheduled callback %p with arg %p", callback, p);
	callback(p);
	LOG("Callback finished...");
	return true;
}

static void ami_schedule_open_timer(struct MsgPort *msgport)
{
#ifdef __amigaos4__
	tioreq = (struct TimeRequest *)AllocSysObjectTags(ASOT_IOREQUEST,
				ASOIOR_Size,sizeof(struct nscallback),
				ASOIOR_ReplyPort,msgport,
				ASO_NoTrack,FALSE,
				TAG_DONE);
#else
	tioreq = (struct nscallback *)CreateIORequest(msgport, sizeof(struct nscallback));
#endif

	OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)tioreq, 0);

	TimerBase = (struct Device *)tioreq->timereq.Request.io_Device;
#ifdef __amigaos4__
	ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
#endif
}

static void ami_schedule_close_timer(void)
{
#ifdef __amigaos4__
	if(ITimer) DropInterface((struct Interface *)ITimer);
#endif
	CloseDevice((struct IORequest *) tioreq);
	FreeSysObject(ASOT_IOREQUEST, tioreq);
}

/* exported interface documented in amiga/schedule.h */
nserror ami_schedule_create(struct MsgPort *msgport)
{
	pool_nscb = ami_misc_itempool_create(sizeof(struct nscallback));
	if(pool_nscb == NULL) return NSERROR_NOMEM;

	ami_schedule_open_timer(msgport);
	schedule_list = pblHeapNew();
	if(schedule_list == PBL_ERROR_OUT_OF_MEMORY) return NSERROR_NOMEM;

	pblHeapSetCompareFunction(schedule_list, ami_schedule_compare);

	return NSERROR_OK;
}

/* exported interface documented in amiga/schedule.h */
void ami_schedule_free(void)
{
	schedule_remove_all();
	pblHeapFree(schedule_list); // this should be empty at this point
	schedule_list = NULL;

	ami_schedule_close_timer();

	ami_misc_itempool_delete(pool_nscb);
}

/* exported function documented in amiga/schedule.h */
nserror ami_schedule(int t, void (*callback)(void *p), void *p)
{
	struct nscallback *nscb;

	if(t == 0) t = 1;

	LOG("Scheduling callback %p with arg %p at time %d", callback, p, t);

	if(schedule_list == NULL) return NSERROR_INIT_FAILED;
	if(t < 0) return schedule_remove(callback, p);

	if ((nscb = ami_schedule_locate(callback, p, false))) {
		return ami_schedule_reschedule(nscb, t);
	}

	nscb = ami_misc_itempool_alloc(pool_nscb, sizeof(struct nscallback));
	if(!nscb) return NSERROR_NOMEM;

	LOG("new nscb %p", nscb);

	*nscb = *tioreq;

	if (ami_schedule_add_timer_event(nscb, t) != NSERROR_OK)
		return NSERROR_NOMEM;

	nscb->callback = callback;
	nscb->p = p;

	pblHeapInsert(schedule_list, nscb);

	return NSERROR_OK;

}

/* exported interface documented in amiga/schedule.h */
void ami_schedule_handle(struct MsgPort *nsmsgport)
{
	/* nsmsgport is the NetSurf message port that
	 * timer.device is sending messages to. */

	struct nscallback *timermsg;

	while((timermsg = (struct nscallback *)GetMsg(nsmsgport))) {
		LOG("timermsg %p", timermsg);
		LOG("timereq err = %d (should be 0)", timermsg->timereq.Request.io_Error);
		ami_scheduler_run(timermsg);
	}
}

