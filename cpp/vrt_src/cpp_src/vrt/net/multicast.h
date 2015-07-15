/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK.
 *
 * REDHAWK is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#ifndef __MULTICAST_H__
#include <stdlib.h>      // needed for malloc(..) and free(..)
#include <string.h>      // needed for strcmp(..) and memset(..)
#include <unistd.h>
#include <math.h>        // needed for floorf
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h> 
#include <sys/ioctl.h>  // needed to find network interface
#include <sys/types.h>  // needed for select
#include <sys/time.h>   // needed for select
#include <net/if.h>     // see "man 7 netdevice"

// The "struct ip_mreqn" was added to Linux 2.2 and is simply a copy of "struct ip_mreq"
// with an additional "imr_ifindex" field added to the end, it is not present under
// OS X, so we simply bypass the use of "imr_ifindex". It is likely "struct ip_mreqn"
// is missing under BSD too, but this is untested.
#if (defined(__APPLE__) && defined(__MACH__))
# define using_ip_mreq_for_ip_mreqn 1
# define ip_mreqn    ip_mreq
# define imr_address imr_multiaddr
#endif

#ifndef DEFAULT_SO_RCVBUF
  /* Default SO_RCVBUF size in bytes. Older versions of this include file
     used 128 MiB which was a fairly poor choice for all but the highest
     data rates.
   */
# define DEFAULT_SO_RCVBUF 2097152 // 2MiB
#endif

#ifndef DEFAULT_SO_SNDBUF
  /* Default SO_SNDBUF size in bytes. */
# define DEFAULT_SO_SNDBUF -1
#endif

/** Holds the multicast socket information. */
typedef struct multicast_tag {
  /** The POSIX socket (-1 if not open) */
  int sock;

  /** The socket address */
  struct sockaddr_in addr;
} multicast_t;

/** Closes the multicast port. This function is safe to call multiple times.
 *  @param mcast The multicast tag.
 */
static inline void multicast_close (multicast_t &mcast) {
  if (mcast.sock > 0) {
    close(mcast.sock);
    mcast.sock = -1;
  }
}

/** Checks to see if a given multicast is open.
 *  @param mcast The multicast tag.
 *  @return true if open, false if closed.
 */
static inline bool multicast_is_open (const multicast_t &mcast) {
  return (mcast.sock > 0);
}

/** Creates a multicast tag that points to nothing but can be used with the
 *  multicast_is_open(..) and multicast_close(..) functions.
 *  @return The updated multicast tag.
 */
static inline multicast_t multicast_none () {
  multicast_t multicast;
  multicast.sock = -1;
  return multicast;
}

/** <b>Internal use only:</b> Opens the multicast port.
 *  @param iface The interface (e.g. "eth0" or "eth1.123").
 *  @param group The multicast group.
 *  @param port  The multicast port.
 *  @return The multicast tag.
 */
static multicast_t multicast_open_ (const char* iface,
                                    const char* group,
                                    int         port) {
  size_t ii;
  int one = 1;

  multicast_t multicast = { socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) };
  if (multicast.sock < 0) return multicast;
  setsockopt(multicast.sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

  // Enumerate all the devices
  struct ifconf devs = {0};
  devs.ifc_len = 512*sizeof(struct ifreq);
  devs.ifc_buf = (char*)malloc(devs.ifc_len);
  if (devs.ifc_buf == 0) {
      close(multicast.sock);
      multicast.sock = -1;
      return multicast;
  }
  if (ioctl(multicast.sock, SIOCGIFCONF, &devs) < 0) {
      close(multicast.sock);
      multicast.sock = -1;
      free(devs.ifc_buf);
      return multicast;
  }
  for (ii = 0; ii<devs.ifc_len/sizeof(struct ifreq); ii++) {
    // The next line does not work on OS X / GCC 4.2.1 as it does not have the
    // union for ifr_ifrn (possibly affects BSD too). However as "ifr_name" is
    // usually defined as a pre-processor replacement for "ifr_ifrn.ifrn_name"
    // on Linux, the line below it should work.
    //if (strcmp(iface, devs.ifc_req[ii].ifr_ifrn.ifrn_name) == 0) {
    if (strcmp(iface, devs.ifc_req[ii].ifr_name) == 0) {
      struct ifreq dev = devs.ifc_req[ii];
      if(ioctl(multicast.sock, SIOCGIFFLAGS, &dev) < 0) {
          close(multicast.sock);
          multicast.sock = -1;
          free(devs.ifc_buf);
          return multicast;
      }
      if (!(dev.ifr_flags & IFF_UP) || (dev.ifr_flags & IFF_LOOPBACK) ||
          !(dev.ifr_flags & IFF_MULTICAST)) {
          close(multicast.sock);
          multicast.sock = -1;
          free(devs.ifc_buf);
          return multicast;
      }

// OS X does not support SIOCGIFINDEX (see "man 7 netdevice" on Linux), rather
// it provides if_nametoindex(..) (see "man 3 if_nametoindex" on OS X). Although
// if_nametoindex(..) is available on a number of Linux distributions, it is not
// used here due to performance issues (at one point if_nametoindex(..) had very
// high overhead on Linux/BSD) and possible compatibility issues (read this last
// reason as "don't break what we know has worked for years"). It also appears
// that SIOCGIFINDEX isn't available on some some versions of BSD, so hopefully
// this will work there too.
#ifdef SIOCGIFINDEX
      if (ioctl(multicast.sock, SIOCGIFINDEX, &dev) != 0) {
          close(multicast.sock);
          multicast.sock = -1;
          free(devs.ifc_buf);
          return multicast;
      }
#endif

      struct ip_mreqn mreqn;
      memset(&mreqn, 0, sizeof(mreqn));
      if (inet_aton(group, &mreqn.imr_multiaddr) == 0) {
          close(multicast.sock);
          multicast.sock = -1;
          free(devs.ifc_buf);
          return multicast;
      }
      mreqn.imr_address = ((struct sockaddr_in*)(&dev.ifr_addr))->sin_addr;
#if defined(SIOCGIFINDEX) && !defined(using_ip_mreq_for_ip_mreqn)
      mreqn.imr_ifindex = dev.ifr_ifindex;
#endif
      if (setsockopt(multicast.sock, IPPROTO_IP, IP_MULTICAST_IF, &mreqn, sizeof(struct ip_mreqn)) != 0) {
          close(multicast.sock);
          multicast.sock = -1;
          free(devs.ifc_buf);
          return multicast;
      }
      multicast.addr.sin_family = AF_INET;
      multicast.addr.sin_addr.s_addr = mreqn.imr_multiaddr.s_addr;
      multicast.addr.sin_port = htons(port);
      if (bind(multicast.sock, (struct sockaddr*)&multicast.addr, sizeof(struct sockaddr_in)) != 0) {
          close(multicast.sock);
          multicast.sock = -1;
          free(devs.ifc_buf);
          return multicast;
      }
      if (setsockopt(multicast.sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(struct ip_mreqn)) != 0) {
          close(multicast.sock);
          multicast.sock = -1;
          free(devs.ifc_buf);
          return multicast;
      }
      free(devs.ifc_buf);
      return multicast;
    }
  }
  // If we get here, we've failed
  close(multicast.sock);
  free(devs.ifc_buf);
  multicast.sock = -1;
  return multicast;
}

/** Opens a the multicast client socket for receiving multicast packets.
 *  @param iface The interface (e.g. "eth0" or "eth1.123").
 *  @param group The multicast group.
 *  @param port  The multicast port.
 *  @param size  The SO_RCVBUF size in bytes (default is set via DEFAULT_SO_RCVBUF),
 *               set this to -1 to leave unchanges from the O/S's default.
 *  @return The multicast tag.
 */
static inline multicast_t multicast_client (const char* iface,
                                            const char* group,
                                            int         port,
                                            int         size=DEFAULT_SO_RCVBUF) {
  multicast_t client = multicast_open_(iface, group, port);
  if (client.sock != -1) {
    if (size >= 0) {
      if (setsockopt(client.sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int)) != 0) {
        close(client.sock);
        client.sock = -1;
      }
    }
  }
  return client;
}

/** Receive a multicast datagram.
 *  @param client             The multicast tag for the client.
 *  @param buffer             The buffer to receive the datagram payload. (OUTPUT)
 *  @param bytes              The length of the buffer.
 *  @param timeout_in_seconds The number of seconds to wait before a time-out. Set
 *                            this to 0 for a non-blocking read (i.e. just returns
 *                            if none read).
 *  @return The length of the datagram read in, 0 on a time-out or -1 on error.
 */
static inline ssize_t multicast_receive (multicast_t &client,
                                         void*        buffer,
                                         size_t       bytes,
                                         float        timeout_in_seconds) {
  if (timeout_in_seconds >= 0) {
    int srv = 0;
    struct timeval tv;
    tv.tv_sec = (long)floorf(timeout_in_seconds);
    float tmp = timeout_in_seconds - tv.tv_sec;
    tv.tv_usec = (long)floorf(tmp * 1e6);
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(client.sock, &readset);
    int maxfd = client.sock + 1;
    srv = select(maxfd, &readset, NULL, NULL, &tv);
    if(srv <= 0) return 0;
  }
  return recv(client.sock, buffer, bytes, 0);
}

/** Opens a the multicast server socket for sending multicast packets.
 *  @param iface The interface (e.g. "eth0" or "eth1.123").
 *  @param group The multicast group.
 *  @param port  The multicast port.
 *  @param ttl   The time-to-live value (usually 1).
 *  @param size  The SO_RCVBUF size in bytes (default is set via DEFAULT_SO_SNDBUF),
 *               set this to -1 to leave unchanges from the O/S's default.
 *  @return The multicast tag.
 */
static inline multicast_t multicast_server (const char* iface,
                                            const char* group,
                                            int         port,
                                            uint8_t     ttl,
                                            int         size=DEFAULT_SO_SNDBUF) {
  multicast_t server = multicast_open_(iface, group, port);
  if (server.sock != -1) {
    setsockopt(server.sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    if (size >= 0) {
      if (setsockopt(server.sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(int)) != 0) {
        close(server.sock);
        server.sock = -1;
      }
    }
  }
  return server;
}

/** Transmit a multicast datagram.
 *  @param server             The multicast tag for the server.
 *  @param buffer             The buffer containing the datagram payload.
 *  @param bytes              The length of the buffer.
 *  @return The length of the datagram sent or -1 on error.
 */
static inline ssize_t multicast_transmit (multicast_t &server,
                                          const void*  buffer,
                                          size_t       bytes) {
  return sendto(server.sock, buffer, bytes, 0, 
                (struct sockaddr*)&server.addr, sizeof(server.addr));
}

#define __MULTICAST_H__
#endif
