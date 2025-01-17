/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <resolv.h>
#include <alloca.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>

#if defined(__UCLIBC__) \
 && (__UCLIBC_MAJOR__ == 0 \
 && (__UCLIBC_MINOR__ < 9 || (__UCLIBC_MINOR__ == 9 && __UCLIBC_SUBLEVEL__ < 33)))

#define NS_MAXCDNAME 255 /* max compressed domain name length */
#define NS_MAXLABEL   63 /* max label length */

int dn_encode(const char *src, uint8_t *dst, int length)
{
	uint8_t *buf, *ptr;
	int len;

	if (src == NULL || dst == NULL)
		return -1;

	buf = ptr = alloca(strlen(src) + 2);
	while (src && *src) {
		uint8_t *lenptr = ptr++;
		for (len = 0; *src && *src != '.'; len++)
			*ptr++ = *src++;
		if (len == 0 || len > NS_MAXLABEL)
			return -1;
		*lenptr = len;
		if (*src)
			src++;
	}
	*ptr++ = 0;

	len = ptr - buf;
	if (len > NS_MAXCDNAME || len > length)
		return -1;

	memcpy(dst, buf, len);
	return len;
}

#endif

#if !defined(SOCK_CLOEXEC)

int fflags(int sock, int flags)
{
	int err;

	if (sock < 0)
		return sock;

	if (flags & O_CLOEXEC) {
		if (fcntl(sock, F_SETFD, FD_CLOEXEC) < 0)
			goto error;
	}
	return sock;

error:
	err = errno;
	close(sock);
	errno = err;
	return -1;
}

#endif
