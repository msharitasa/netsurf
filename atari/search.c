/*
 * Copyright 2013 Ole Loots <ole@monochrom.net>
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
 *
 * Module Description:
 *
 *
 *
 */


#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "desktop/gui.h"
#include "desktop/browser.h"
#include "desktop/browser_private.h"
#include "desktop/search.h"
#include "utils/log.h"
#include "utils/messages.h"
#include "atari/gui.h"
#include "atari/rootwin.h"
#include "atari/misc.h"
#include "atari/search.h"
#include "atari/gemtk/gemtk.h"
#include "atari/res/netsurf.rsh"

extern struct gui_window * input_window;


static void nsatari_search_set_status(bool found, void *p);
static void nsatari_search_set_hourglass(bool active, void *p);
static void nsatari_search_add_recent(const char *string, void *p);
void nsatari_search_set_forward_state(bool active, void *p);
void nsatari_search_set_back_state(bool active, void *p);

static struct gui_search_callbacks nsatari_search_callbacks = {
	nsatari_search_set_forward_state,
	nsatari_search_set_back_state,
	nsatari_search_set_status,
	nsatari_search_set_hourglass,
	nsatari_search_add_recent
};


/**
* Change the displayed search status.
* \param found  search pattern matched in text
* \param p the pointer sent to search_verify_new() / search_create_context()
*/

void nsatari_search_set_status(bool found, void *p)
{
	LOG(("%p set status: %d\n", p, found));
	// TODO: maybe update GUI
}

/**
* display hourglass while searching
* \param active start/stop indicator
* \param p the pointer sent to search_verify_new() / search_create_context()
*/

void nsatari_search_set_hourglass(bool active, void *p)
{
	SEARCH_FORM_SESSION s = (SEARCH_FORM_SESSION)p;
	LOG(("active: %d, session: %p", active, p));
	if (active)
		gui_window_set_pointer(s->bw->window, GUI_POINTER_PROGRESS);
	else
		gui_window_set_pointer(s->bw->window, GUI_POINTER_DEFAULT);
}

/**
* add search string to recent searches list
* front is at liberty how to implement the bare notification
* should normally store a strdup() of the string;
* core gives no guarantee of the integrity of the const char *
* \param string search pattern
* \param p the pointer sent to search_verify_new() / search_create_context()
*/

void nsatari_search_add_recent(const char *string, void *p)
{
	LOG(("%p add recent: %s\n", p, string));
}

/**
* activate search forwards button in gui
* \param active activate/inactivate
* \param p the pointer sent to search_verify_new() / search_create_context()
*/

void nsatari_search_set_forward_state(bool active, void *p)
{
	SEARCH_FORM_SESSION s = (SEARCH_FORM_SESSION)p;
	/* deactivate back cb */
	LOG(("%p: set forward state: %d\n", p, active));
	// TODO: update gui
}

/**
* activate search back button in gui
* \param active activate/inactivate
* \param p the pointer sent to search_verify_new() / search_create_context()
*/

void nsatari_search_set_back_state(bool active, void *p)
{
	SEARCH_FORM_SESSION s = (SEARCH_FORM_SESSION)p;
	/* deactivate back cb */
	LOG(("%p: set back state: %d\n", p, active));
	// TODO: update gui
}


static int apply_form(OBJECT *obj, struct s_search_form_state *s)
{
	char * cstr;

	assert(s != NULL);

	s->flags = 0;

	if( (obj[TOOLBAR_CB_CASESENSE].ob_state & OS_SELECTED) != 0 )
		s->flags |= SEARCH_FLAG_CASE_SENSITIVE;
	if( (obj[TOOLBAR_CB_SHOWALL].ob_state & OS_SELECTED) != 0 )
		s->flags |= SEARCH_FLAG_SHOWALL;

	cstr = get_text(obj, TOOLBAR_TB_SRCH);
	snprintf(s->text, 11, "%s", cstr);
	return ( 0 );

}

static void set_text(OBJECT *obj, short idx, char * text, int len )
{
	char spare[255];

	if( len > 254 )
		len = 254;
	if( text != NULL ){
		strncpy(spare, text, 254);
	} else {
		strcpy(spare, "");
	}

	set_string(obj, idx, spare);
}

void nsatari_search_session_destroy(struct s_search_form_session *s)
{
	if (s != NULL) {
		LOG((""));
		browser_window_search_destroy_context(s->bw);
		free(s);
	}
}

/* checks for search parameters changes */
static bool search_session_compare(struct s_search_form_session *s, OBJECT *obj)
{
	bool check;
	uint32_t flags_old;
	uint32_t flags_mask = SEARCH_FLAG_SHOWALL | SEARCH_FLAG_CASE_SENSITIVE;
	struct s_search_form_state cur;

	assert(s != NULL && obj != NULL);

	flags_old = s->state.flags;

	apply_form(obj, &cur);
	if ((cur.flags&flags_mask) != (flags_old&flags_mask)) {
		return( true );
	}

	char * cstr;
	cstr = get_text(obj, TOOLBAR_TB_SRCH);
	if (cstr != NULL){
		if (strcmp(cstr, (char*)&s->state.text) != 0) {
			return (true);
		}
	}

	return( false );
}


void nsatari_search_perform(struct s_search_form_session *s, OBJECT *obj,
		search_flags_t f)
{

	bool fwd;
	search_flags_t flags = f;

	assert(s!=null);

	if(search_session_compare(s, obj)){
		printf("reset search form\n");
		browser_window_search_destroy_context(s->bw);
		apply_form(obj, &s->state);
	} else {

	}

	/* get search direction manually: */
	if ( (f&SEARCH_FLAG_FORWARDS) != 0 )
		s->state.flags |= SEARCH_FLAG_FORWARDS;
	else
		s->state.flags &= (~SEARCH_FLAG_FORWARDS);

	if( browser_window_search_verify_new(s->bw, &nsatari_search_callbacks, s) ){
		printf("searching for: %s\n", get_text(obj, TOOLBAR_TB_SRCH));
		browser_window_search_step(s->bw, s->state.flags,
									get_text(obj, TOOLBAR_TB_SRCH));
	}

}


struct s_search_form_session * nsatari_search_session_create(OBJECT * obj,
		struct browser_window *bw)
{
	struct s_search_form_session *sfs;

	sfs = calloc(1, sizeof(struct s_search_form_session));

	assert(sfs);

	sfs->bw = bw;
	apply_form(obj, &sfs->state);

	browser_window_search_destroy_context(bw);

	return(sfs);
}
