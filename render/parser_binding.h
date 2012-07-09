/*
 * Copyright 2008 John-Mark Bell <jmb@netsurf-browser.org>
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

#ifndef _NETSURF_RENDER_PARSER_BINDING_H_
#define _NETSURF_RENDER_PARSER_BINDING_H_

#include <dom/dom.h>
#include <dom/bindings/hubbub/parser.h>

struct box;
struct form;
struct form_control;

typedef enum binding_error {
	BINDING_OK,
	BINDING_NOMEM,
	BINDING_BADENCODING,
	BINDING_ENCODINGCHANGE
} binding_error;

typedef enum binding_encoding_source { 
	ENCODING_SOURCE_HEADER, 
	ENCODING_SOURCE_DETECTED,
	ENCODING_SOURCE_META 
} binding_encoding_source;

typedef enum binding_quirks_mode {
	BINDING_QUIRKS_MODE_NONE,
	BINDING_QUIRKS_MODE_LIMITED,
	BINDING_QUIRKS_MODE_FULL
} binding_quirks_mode;

binding_error binding_create_tree(void **ctx, const char *charset, bool enable_script, dom_script script, void *context);
binding_error binding_destroy_tree(void *ctx);

binding_error binding_parse_chunk(void *ctx, const uint8_t *data, size_t len);
binding_error binding_parse_completed(void *ctx);

const char *binding_get_encoding(void *ctx, binding_encoding_source *source);
dom_document *binding_get_document(void *ctx, binding_quirks_mode *quirks);

struct form *binding_get_forms(void *ctx);
struct form_control *binding_get_control_for_node(void *ctx, dom_node *node);

void binding_destroy_document(dom_document *doc);

#endif

