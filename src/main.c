/* live-f1
 *
 * Copyright © 2005 Scott James Remnant <scott@netsplit.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <locale.h>

#include <ne_socket.h>

#include "live-f1.h"
#include "display.h"
#include "http.h"
#include "stream.h"


/* Program name */
const char *program_name = NULL;

/* How verbose to be */
static int verbosity = 10;


int
main (int   argc,
      char *argv[])
{
	CurrentState  state;
	int           sock;

	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);

	program_name = argv[0];

	if (ne_sock_init ()) {
		fprintf (stderr, "%s: %s\n", program_name,
			 _("unable to initialise http library"));
		return 1;
	}

	memset (&state, 0, sizeof (state));
	reset_decryption (&state);

	state.cookie = obtain_auth_cookie ("scott-fia@netsplit.com",
					   "mka773624");
	if (! state.cookie) {
		printf ("Unable to obtain authorisation cookie.\n");
		return 1;
	}

	sock = open_stream ("localhost", 4321);
	if (sock < 0) {
		printf ("Unable to open data stream.\n");
		return 1;
	}

	while (read_stream (&state, sock) > 0)
		;

	close_display ();

	return 0;
}


/**
 * info:
 * @irrelevance: minimum verbosity level to output the message,
 * @format: format string for vprintf.
 *
 * Print the formatted message to standard output if verbosity is high
 * enough.
 **/
int
info (int         irrelevance,
      const char *format, ...)
{
	va_list ap;
	int     ret;

	if (verbosity >= irrelevance) {
		va_start (ap, format);
		if (cursed) {
			unsigned char msg[513];

			ret = vsnprintf ((char *) msg, 512, format, ap);
			popup_message (msg);
		} else {
			ret = vprintf (format, ap);
		}

		va_end (ap);
		return ret;
	} else {
		return 0;
	}
}
