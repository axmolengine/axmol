//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any 
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Copy from:
https://github.com/xamarin/xamarin-android/blob/master/src/monodroid/jni/xamarin_getifaddrs.cc
*/
#ifndef YASIO__IFADDRS_HPP
#define YASIO__IFADDRS_HPP

#include "yasio/detail/config.hpp"

#if !(defined(ANDROID) || defined(__ANDROID__)) || __ANDROID_API__ >= 24
#  include <ifaddrs.h>
namespace yasio
{
using ::freeifaddrs;
using ::getifaddrs;
} // namespace yasio
#else
#  include <assert.h>
#  include <errno.h>
#  include <dlfcn.h>
#  include <limits.h>
#  include <stdio.h>
#  include <string.h>
#  include <stdlib.h>

#  include <sys/types.h>
#  include <sys/socket.h>

#  include <unistd.h>
#  include <linux/netlink.h>
#  include <linux/rtnetlink.h>
#  include <linux/if_arp.h>
#  include <netinet/in.h>
#  include <mutex>

/* Some of these aren't defined in android's rtnetlink.h (as of ndk 16). We define values for all of
 * them if they aren't found so that the debug code works properly. We could skip them but future
 * versions of the NDK might include definitions for them.
 * Values are taken from Linux headers shipped with glibc
 */
#  ifndef IFLA_UNSPEC
#    define IFLA_UNSPEC 0
#  endif

#  ifndef IFLA_ADDRESS
#    define IFLA_ADDRESS 1
#  endif

#  ifndef IFLA_BROADCAST
#    define IFLA_BROADCAST 2
#  endif

#  ifndef IFLA_IFNAME
#    define IFLA_IFNAME 3
#  endif

#  ifndef IFLA_MTU
#    define IFLA_MTU 4
#  endif

#  ifndef IFLA_LINK
#    define IFLA_LINK 5
#  endif

#  ifndef IFLA_QDISC
#    define IFLA_QDISC 6
#  endif

#  ifndef IFLA_STATS
#    define IFLA_STATS 7
#  endif

#  ifndef IFLA_COST
#    define IFLA_COST 8
#  endif

#  ifndef IFLA_PRIORITY
#    define IFLA_PRIORITY 9
#  endif

#  ifndef IFLA_MASTER
#    define IFLA_MASTER 10
#  endif

#  ifndef IFLA_WIRELESS
#    define IFLA_WIRELESS 11
#  endif

#  ifndef IFLA_PROTINFO
#    define IFLA_PROTINFO 12
#  endif

#  ifndef IFLA_TXQLEN
#    define IFLA_TXQLEN 13
#  endif

#  ifndef IFLA_MAP
#    define IFLA_MAP 14
#  endif

#  ifndef IFLA_WEIGHT
#    define IFLA_WEIGHT 15
#  endif

#  ifndef IFLA_OPERSTATE
#    define IFLA_OPERSTATE 16
#  endif

#  ifndef IFLA_LINKMODE
#    define IFLA_LINKMODE 17
#  endif

#  ifndef IFLA_LINKINFO
#    define IFLA_LINKINFO 18
#  endif

#  ifndef IFLA_NET_NS_PID
#    define IFLA_NET_NS_PID 19
#  endif

#  ifndef IFLA_IFALIAS
#    define IFLA_IFALIAS 20
#  endif

#  ifndef IFLA_NUM_VF
#    define IFLA_NUM_VF 21
#  endif

#  ifndef IFLA_VFINFO_LIST
#    define IFLA_VFINFO_LIST 22
#  endif

#  ifndef IFLA_STATS64
#    define IFLA_STATS64 23
#  endif

#  ifndef IFLA_VF_PORTS
#    define IFLA_VF_PORTS 24
#  endif

#  ifndef IFLA_PORT_SELF
#    define IFLA_PORT_SELF 25
#  endif

#  ifndef IFLA_AF_SPEC
#    define IFLA_AF_SPEC 26
#  endif

#  ifndef IFLA_GROUP
#    define IFLA_GROUP 27
#  endif

#  ifndef IFLA_NET_NS_FD
#    define IFLA_NET_NS_FD 28
#  endif

#  ifndef IFLA_EXT_MASK
#    define IFLA_EXT_MASK 29
#  endif

#  ifndef IFLA_PROMISCUITY
#    define IFLA_PROMISCUITY 30
#  endif

#  ifndef IFLA_NUM_TX_QUEUES
#    define IFLA_NUM_TX_QUEUES 31
#  endif

#  ifndef IFLA_NUM_RX_QUEUES
#    define IFLA_NUM_RX_QUEUES 32
#  endif

#  ifndef IFLA_CARRIER
#    define IFLA_CARRIER 33
#  endif

#  ifndef IFLA_PHYS_PORT_ID
#    define IFLA_PHYS_PORT_ID 34
#  endif

#  ifndef IFLA_CARRIER_CHANGES
#    define IFLA_CARRIER_CHANGES 35
#  endif

#  ifndef IFLA_PHYS_SWITCH_ID
#    define IFLA_PHYS_SWITCH_ID 36
#  endif

#  ifndef IFLA_LINK_NETNSID
#    define IFLA_LINK_NETNSID 37
#  endif

#  ifndef IFLA_PHYS_PORT_NAME
#    define IFLA_PHYS_PORT_NAME 38
#  endif

#  ifndef IFLA_PROTO_DOWN
#    define IFLA_PROTO_DOWN 39
#  endif

#  ifndef IFLA_GSO_MAX_SEGS
#    define IFLA_GSO_MAX_SEGS 40
#  endif

#  ifndef IFLA_GSO_MAX_SIZE
#    define IFLA_GSO_MAX_SIZE 41
#  endif

#  ifndef IFLA_PAD
#    define IFLA_PAD 42
#  endif

#  ifndef IFLA_XDP
#    define IFLA_XDP 43
#  endif

/* Maximum interface address label size, should be more than enough */
#  define MAX_IFA_LABEL_SIZE 1024

/* We're implementing getifaddrs behavior, this is the structure we use. It is exactly the same as
 * struct ifaddrs defined in ifaddrs.h but since bionics doesn't have it we need to mirror it here.
 */
struct ifaddrs
{
  struct ifaddrs* ifa_next; /* Pointer to the next structure.      */

  char* ifa_name;         /* Name of this network interface.     */
  unsigned int ifa_flags; /* Flags as from SIOCGIFFLAGS ioctl.   */

  struct sockaddr* ifa_addr;    /* Network address of this interface.  */
  struct sockaddr* ifa_netmask; /* Netmask of this interface.          */
  union
  {
    /* At most one of the following two is valid.  If the IFF_BROADCAST
       bit is set in `ifa_flags', then `ifa_broadaddr' is valid.  If the
       IFF_POINTOPOINT bit is set, then `ifa_dstaddr' is valid.
       It is never the case that both these bits are set at once.  */
    struct sockaddr* ifu_broadaddr; /* Broadcast address of this interface. */
    struct sockaddr* ifu_dstaddr;   /* Point-to-point destination address.  */
  } ifa_ifu;
  /* These very same macros are defined by <net/if.h> for `struct ifaddr'.
     So if they are defined already, the existing definitions will be fine.  */
#  ifndef ifa_broadaddr
#    define ifa_broadaddr ifa_ifu.ifu_broadaddr
#  endif
#  ifndef ifa_dstaddr
#    define ifa_dstaddr ifa_ifu.ifu_dstaddr
#  endif
  void* ifa_data; /* Address-specific data (may be unused).  */
};

namespace yasio
{
namespace internal
{
/* This is the message we send to the kernel */
typedef struct
{
  struct nlmsghdr header;
  struct rtgenmsg message;
} netlink_request;

typedef struct
{
  int sock_fd;
  int seq;
  struct sockaddr_nl them;      /* kernel end */
  struct sockaddr_nl us;        /* our end */
  struct msghdr message_header; /* for use with sendmsg */
  struct iovec payload_vector;  /* Used to send netlink_request */
} netlink_session;

/* Turns out that quite a few link types have address length bigger than the 8 bytes allocated in
 * this structure as defined by the OS. Examples are Infiniband or ipv6 tunnel devices
 */
struct sockaddr_ll_extended
{
  unsigned short int sll_family;
  unsigned short int sll_protocol;
  int sll_ifindex;
  unsigned short int sll_hatype;
  unsigned char sll_pkttype;
  unsigned char sll_halen;
  unsigned char sll_addr[24];
};

/* fwds */
static struct ifaddrs* get_link_info(const struct nlmsghdr* message);
static struct ifaddrs* get_link_address(const struct nlmsghdr* message,
                                        struct ifaddrs** ifaddrs_head);

/* implementaions */
static void get_ifaddrs_impl(int (**getifaddrs_impl)(struct ifaddrs** ifap),
                             void (**freeifaddrs_impl)(struct ifaddrs* ifa))
{
  void* libc = nullptr;

  assert(getifaddrs_impl);
  assert(freeifaddrs_impl);

  libc = dlopen("libc.so", RTLD_NOW);
  if (libc)
  {
    *getifaddrs_impl = reinterpret_cast<int (*)(struct ifaddrs**)>(dlsym(libc, "getifaddrs"));
    if (*getifaddrs_impl)
      *freeifaddrs_impl = reinterpret_cast<void (*)(struct ifaddrs*)>(dlsym(libc, "freeifaddrs"));
  }

  if (!*getifaddrs_impl)
  {
    YASIO_LOG("This libc does not have getifaddrs/freeifaddrs, using Xamarin's");
  }
  else
  {
    YASIO_LOG("This libc has getifaddrs/freeifaddrs");
  }
}

static void free_single_ifaddrs(struct ifaddrs** ifap)
{
  struct ifaddrs* ifa = ifap ? *ifap : NULL;
  if (!ifa)
    return;

  if (ifa->ifa_name)
    free(ifa->ifa_name);

  if (ifa->ifa_addr)
    free(ifa->ifa_addr);

  if (ifa->ifa_netmask)
    free(ifa->ifa_netmask);

  if (ifa->ifa_broadaddr)
    free(ifa->ifa_broadaddr);

  if (ifa->ifa_data)
    free(ifa->ifa_data);

  free(ifa);
  *ifap = NULL;
}

static int open_netlink_session(netlink_session* session)
{
  assert(session != 0);

  memset(session, 0, sizeof(*session));
  session->sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (session->sock_fd == -1)
  {
    YASIO_LOG("Failed to create a netlink socket. %s", strerror(errno));
    return -1;
  }

  /* Fill out addresses */
  session->us.nl_family = AF_NETLINK;

  /* We have previously used `getpid()` here but it turns out that WebView/Chromium does the same
     and there can only be one session with the same PID. Setting it to 0 will cause the kernel to
     assign some PID that's unique and valid instead.
     See: https://bugzilla.xamarin.com/show_bug.cgi?id=41860
  */
  session->us.nl_pid    = 0;
  session->us.nl_groups = 0;

  session->them.nl_family = AF_NETLINK;

  if (bind(session->sock_fd, (struct sockaddr*)&session->us, sizeof(session->us)) < 0)
  {
    YASIO_LOG("Failed to bind to the netlink socket. %s", strerror(errno));
    return -1;
  }

  return 0;
}

static int send_netlink_dump_request(netlink_session* session, int type)
{
  netlink_request request;

  memset(&request, 0, sizeof(request));
  request.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
  /* Flags (from netlink.h):
     NLM_F_REQUEST - it's a request message
     NLM_F_DUMP - gives us the root of the link tree and returns all links matching our requested
     AF, which in our case means all of them (AF_PACKET)
  */
  request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT | NLM_F_MATCH;
  request.header.nlmsg_seq   = static_cast<__u32>(++session->seq);
  request.header.nlmsg_pid   = session->us.nl_pid;
  request.header.nlmsg_type  = static_cast<__u16>(type);

  /* AF_PACKET means we want to see everything */
  request.message.rtgen_family = AF_PACKET;

  memset(&session->payload_vector, 0, sizeof(session->payload_vector));
  session->payload_vector.iov_len  = request.header.nlmsg_len;
  session->payload_vector.iov_base = &request;

  memset(&session->message_header, 0, sizeof(session->message_header));
  session->message_header.msg_namelen = sizeof(session->them);
  session->message_header.msg_name    = &session->them;
  session->message_header.msg_iovlen  = 1;
  session->message_header.msg_iov     = &session->payload_vector;

  if (sendmsg(session->sock_fd, (const struct msghdr*)&session->message_header, 0) < 0)
  {
    YASIO_LOG("Failed to send netlink message. %s", strerror(errno));
    return -1;
  }

  return 0;
}

static int append_ifaddr(struct ifaddrs* addr, struct ifaddrs** ifaddrs_head,
                         struct ifaddrs** last_ifaddr)
{
  assert(addr);
  assert(ifaddrs_head);
  assert(last_ifaddr);

  if (!*ifaddrs_head)
  {
    *ifaddrs_head = *last_ifaddr = addr;
    if (!*ifaddrs_head)
      return -1;
  }
  else if (!*last_ifaddr)
  {
    struct ifaddrs* last = *ifaddrs_head;

    while (last->ifa_next)
      last = last->ifa_next;
    *last_ifaddr = last;
  }

  addr->ifa_next = NULL;
  if (addr == *last_ifaddr)
    return 0;

  assert(addr != *last_ifaddr);
  (*last_ifaddr)->ifa_next = addr;
  *last_ifaddr             = addr;
  assert((*last_ifaddr)->ifa_next == NULL);

  return 0;
}

static int parse_netlink_reply(netlink_session* session, struct ifaddrs** ifaddrs_head,
                               struct ifaddrs** last_ifaddr)
{
  struct msghdr netlink_reply;
  struct iovec reply_vector;
  struct nlmsghdr* current_message;
  struct ifaddrs* addr;
  int ret                 = -1;
  unsigned char* response = NULL;

  assert(session);
  assert(ifaddrs_head);
  assert(last_ifaddr);

  int buf_size = static_cast<int>(getpagesize());
  YASIO_LOGV("receive buffer size == %d", buf_size);

  response       = (unsigned char*)malloc(sizeof(*response) * buf_size);
  ssize_t length = 0;
  if (!response)
  {
    goto cleanup;
  }

  while (1)
  {
    memset(response, 0, buf_size);
    memset(&reply_vector, 0, sizeof(reply_vector));
    reply_vector.iov_len  = buf_size;
    reply_vector.iov_base = response;

    memset(&netlink_reply, 0, sizeof(netlink_reply));
    netlink_reply.msg_namelen = sizeof(&session->them);
    netlink_reply.msg_name    = &session->them;
    netlink_reply.msg_iovlen  = 1;
    netlink_reply.msg_iov     = &reply_vector;

    length = recvmsg(session->sock_fd, &netlink_reply, 0);
    YASIO_LOGV("  length == %d", static_cast<int>(length));

    if (length < 0)
    {
      YASIO_LOGV("Failed to receive reply from netlink. %s", strerror(errno));
      goto cleanup;
    }

    if (length == 0)
      break;

    for (current_message = (struct nlmsghdr*)response;
         current_message && NLMSG_OK(current_message, static_cast<size_t>(length));
         current_message = NLMSG_NEXT(current_message, length))
    {
      YASIO_LOGV("next message... (type: %u)", current_message->nlmsg_type);
      switch (current_message->nlmsg_type)
      {
        /* See rtnetlink.h */
        case RTM_NEWLINK:
          YASIO_LOGV("  dumping link...");
          addr = get_link_info(current_message);
          if (!addr || append_ifaddr(addr, ifaddrs_head, last_ifaddr) < 0)
          {
            ret = -1;
            goto cleanup;
          }
          YASIO_LOGV("  done");
          break;

        case RTM_NEWADDR:
          YASIO_LOGV("  got an address");
          addr = get_link_address(current_message, ifaddrs_head);
          if (!addr || append_ifaddr(addr, ifaddrs_head, last_ifaddr) < 0)
          {
            ret = -1;
            goto cleanup;
          }
          break;

        case NLMSG_DONE:
          YASIO_LOGV("  message done");
          ret = 0;
          goto cleanup;
          break;

        default:
          YASIO_LOGV("  message type: %u", current_message->nlmsg_type);
          break;
      }
    }
  }

cleanup:
  if (response)
    free(response);
  return ret;
}

static int fill_sa_address(struct sockaddr** sa, struct ifaddrmsg* net_address, void* rta_data,
                           size_t rta_payload_length)
{
  assert(sa);
  assert(net_address);
  assert(rta_data);

  switch (net_address->ifa_family)
  {
    case AF_INET: {
      struct sockaddr_in* sa4;
      assert(rta_payload_length == 4); /* IPv4 address length */
      sa4 = (struct sockaddr_in*)calloc(1, sizeof(*sa4));
      if (!sa4)
        return -1;

      sa4->sin_family = AF_INET;
      memcpy(&sa4->sin_addr, rta_data, rta_payload_length);
      *sa = (struct sockaddr*)sa4;
      break;
    }

    case AF_INET6: {
      struct sockaddr_in6* sa6;
      assert(rta_payload_length == 16); /* IPv6 address length */
      sa6 = (struct sockaddr_in6*)calloc(1, sizeof(*sa6));
      if (!sa6)
        return -1;

      sa6->sin6_family = AF_INET6;
      memcpy(&sa6->sin6_addr, rta_data, rta_payload_length);
      if (IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr) || IN6_IS_ADDR_MC_LINKLOCAL(&sa6->sin6_addr))
        sa6->sin6_scope_id = net_address->ifa_index;
      *sa = (struct sockaddr*)sa6;
      break;
    }

    default: {
      struct sockaddr* sagen;
      assert(rta_payload_length <= sizeof(sagen->sa_data));
      *sa = sagen = (struct sockaddr*)calloc(1, sizeof(*sagen));
      if (!sagen)
        return -1;

      sagen->sa_family = net_address->ifa_family;
      memcpy(&sagen->sa_data, rta_data, rta_payload_length);
      break;
    }
  }

  return 0;
}

static int fill_ll_address(struct sockaddr_ll_extended** sa, struct ifinfomsg* net_interface,
                           void* rta_data, size_t rta_payload_length)
{
  assert(sa);
  assert(net_interface);

  /* Always allocate, do not free - caller may reuse the same variable */
  *sa = reinterpret_cast<sockaddr_ll_extended*>(calloc(1, sizeof(**sa)));
  if (!*sa)
    return -1;

  (*sa)->sll_family = AF_PACKET; /* Always for physical links */

  /* The assert can only fail for Iniband links, which are quite unlikely to be found
   * in any mobile devices
   */
  YASIO_LOGV("rta_payload_length == %d; sizeof sll_addr == %d; hw type == 0x%X",
             static_cast<int>(rta_payload_length), static_cast<int>(sizeof((*sa)->sll_addr)),
             net_interface->ifi_type);
  if (rta_payload_length > sizeof((*sa)->sll_addr))
  {
    YASIO_LOG("Address is too long to place in sockaddr_ll (%d > %d)",
              static_cast<int>(rta_payload_length), static_cast<int>(sizeof((*sa)->sll_addr)));
    free(*sa);
    *sa = NULL;
    return -1;
  }

  if (rta_payload_length > UCHAR_MAX)
  {
    YASIO_LOG("Payload length too big to fit in the address structure");
    free(*sa);
    *sa = NULL;
    return -1;
  }

  (*sa)->sll_ifindex = net_interface->ifi_index;
  (*sa)->sll_hatype  = net_interface->ifi_type;
  (*sa)->sll_halen   = static_cast<unsigned char>(rta_payload_length);
  memcpy((*sa)->sll_addr, rta_data, rta_payload_length);

  return 0;
}

static struct ifaddrs* find_interface_by_index(int index, struct ifaddrs** ifaddrs_head)
{
  struct ifaddrs* cur;
  if (!ifaddrs_head || !*ifaddrs_head)
    return NULL;

  /* Normally expensive, but with the small amount of links in the chain we'll deal with it's not
   * worth the extra houskeeping and memory overhead
   */
  cur = *ifaddrs_head;
  while (cur)
  {
    if (cur->ifa_addr && cur->ifa_addr->sa_family == AF_PACKET &&
        ((struct sockaddr_ll_extended*)cur->ifa_addr)->sll_ifindex == index)
      return cur;
    if (cur == cur->ifa_next)
      break;
    cur = cur->ifa_next;
  }

  return NULL;
}

static char* get_interface_name_by_index(int index, struct ifaddrs** ifaddrs_head)
{
  struct ifaddrs* iface = find_interface_by_index(index, ifaddrs_head);
  if (!iface || !iface->ifa_name)
    return NULL;

  return iface->ifa_name;
}

static int get_interface_flags_by_index(int index, struct ifaddrs** ifaddrs_head)
{
  struct ifaddrs* iface = find_interface_by_index(index, ifaddrs_head);
  if (!iface)
    return 0;

  return static_cast<int>(iface->ifa_flags);
}

static int calculate_address_netmask(struct ifaddrs* ifa, struct ifaddrmsg* net_address)
{
  if (ifa->ifa_addr && ifa->ifa_addr->sa_family != AF_UNSPEC &&
      ifa->ifa_addr->sa_family != AF_PACKET)
  {
    uint32_t prefix_length      = 0;
    uint32_t data_length        = 0;
    unsigned char* netmask_data = NULL;

    switch (ifa->ifa_addr->sa_family)
    {
      case AF_INET: {
        struct sockaddr_in* sa = (struct sockaddr_in*)calloc(1, sizeof(struct sockaddr_in));
        if (!sa)
          return -1;

        ifa->ifa_netmask = (struct sockaddr*)sa;
        prefix_length    = net_address->ifa_prefixlen;
        if (prefix_length > 32)
          prefix_length = 32;
        data_length  = sizeof(sa->sin_addr);
        netmask_data = (unsigned char*)&sa->sin_addr;
        break;
      }

      case AF_INET6: {
        struct sockaddr_in6* sa = (struct sockaddr_in6*)calloc(1, sizeof(struct sockaddr_in6));
        if (!sa)
          return -1;

        ifa->ifa_netmask = (struct sockaddr*)sa;
        prefix_length    = net_address->ifa_prefixlen;
        if (prefix_length > 128)
          prefix_length = 128;
        data_length  = sizeof(sa->sin6_addr);
        netmask_data = (unsigned char*)&sa->sin6_addr;
        break;
      }
    }

    if (ifa->ifa_netmask && netmask_data)
    {
      /* Fill the first X bytes with 255 */
      uint32_t prefix_bytes = prefix_length / 8;
      uint32_t postfix_bytes;

      if (prefix_bytes > data_length)
      {
        errno = EINVAL;
        return -1;
      }
      postfix_bytes = data_length - prefix_bytes;
      memset(netmask_data, 0xFF, prefix_bytes);
      if (postfix_bytes > 0)
        memset(netmask_data + prefix_bytes + 1, 0x00, postfix_bytes);
      YASIO_LOGV(
          "   calculating netmask, prefix length is %u bits (%u bytes), data length is %u bytes",
          prefix_length, prefix_bytes, data_length);
      if (prefix_bytes + 2 < data_length)
        /* Set the rest of the mask bits in the byte following the last 0xFF value */
        netmask_data[prefix_bytes + 1] =
            static_cast<unsigned char>(0xff << (8 - (prefix_length % 8)));
    }
  }

  return 0;
}

static struct ifaddrs* get_link_address(const struct nlmsghdr* message,
                                        struct ifaddrs** ifaddrs_head)
{
  ssize_t length = 0;
  struct rtattr* attribute;
  struct ifaddrmsg* net_address;
  struct ifaddrs* ifa = NULL;
  struct sockaddr** sa;
  size_t payload_size;

  assert(message);
  net_address = reinterpret_cast<ifaddrmsg*>(NLMSG_DATA(message));
  length      = static_cast<ssize_t>(IFA_PAYLOAD(message));
  YASIO_LOGV("   address data length: %u", (unsigned int)length);
  if (length <= 0)
  {
    goto error;
  }

  ifa = reinterpret_cast<ifaddrs*>(calloc(1, sizeof(*ifa)));
  if (!ifa)
  {
    goto error;
  }

  // values < 0 are never returned, the cast is safe
  ifa->ifa_flags = static_cast<unsigned int>(
      get_interface_flags_by_index(static_cast<int>(net_address->ifa_index), ifaddrs_head));

  attribute = IFA_RTA(net_address);
  YASIO_LOGV("   reading attributes");
  while (RTA_OK(attribute, length))
  {
    payload_size = RTA_PAYLOAD(attribute);
    YASIO_LOGV("     attribute payload_size == %u", (unsigned int)payload_size);
    sa = NULL;

    switch (attribute->rta_type)
    {
      case IFA_LABEL: {
        size_t room_for_trailing_null = 0;

        YASIO_LOGV("     attribute type: LABEL");
        if (payload_size > MAX_IFA_LABEL_SIZE)
        {
          payload_size           = MAX_IFA_LABEL_SIZE;
          room_for_trailing_null = 1;
        }

        if (payload_size > 0)
        {
          ifa->ifa_name = (char*)malloc(payload_size + room_for_trailing_null);
          if (!ifa->ifa_name)
          {
            goto error;
          }

          memcpy(ifa->ifa_name, RTA_DATA(attribute), payload_size);
          if (room_for_trailing_null)
            ifa->ifa_name[payload_size] = '\0';
        }
        break;
      }

      case IFA_LOCAL:
        YASIO_LOGV("     attribute type: LOCAL");
        if (ifa->ifa_addr)
        {
          /* P2P protocol, set the dst/broadcast address union from the original address.
           * Since ifa_addr is set it means IFA_ADDRESS occured earlier and that address
           * is indeed the P2P destination one.
           */
          ifa->ifa_dstaddr = ifa->ifa_addr;
          ifa->ifa_addr    = 0;
        }
        sa = &ifa->ifa_addr;
        break;

      case IFA_BROADCAST:
        YASIO_LOGV("     attribute type: BROADCAST");
        if (ifa->ifa_dstaddr)
        {
          /* IFA_LOCAL happened earlier, undo its effect here */
          free(ifa->ifa_dstaddr);
          ifa->ifa_dstaddr = NULL;
        }
        sa = &ifa->ifa_broadaddr;
        break;

      case IFA_ADDRESS:
        YASIO_LOGV("     attribute type: ADDRESS");
        if (ifa->ifa_addr)
        {
          /* Apparently IFA_LOCAL occured earlier and we have a P2P connection
           * here. IFA_LOCAL carries the destination address, move it there
           */
          ifa->ifa_dstaddr = ifa->ifa_addr;
          ifa->ifa_addr    = NULL;
        }
        sa = &ifa->ifa_addr;
        break;

      case IFA_UNSPEC:
        YASIO_LOGV("     attribute type: UNSPEC");
        break;

      case IFA_ANYCAST:
        YASIO_LOGV("     attribute type: ANYCAST");
        break;

      case IFA_CACHEINFO:
        YASIO_LOGV("     attribute type: CACHEINFO");
        break;

      case IFA_MULTICAST:
        YASIO_LOGV("     attribute type: MULTICAST");
        break;

      default:
        YASIO_LOGV("     attribute type: %u", attribute->rta_type);
        break;
    }

    if (sa)
    {
      if (fill_sa_address(sa, net_address, RTA_DATA(attribute), RTA_PAYLOAD(attribute)) < 0)
      {
        goto error;
      }
    }

    attribute = RTA_NEXT(attribute, length);
  }

  /* glibc stores the associated interface name in the address if IFA_LABEL never occured */
  if (!ifa->ifa_name)
  {
    char* name =
        get_interface_name_by_index(static_cast<int>(net_address->ifa_index), ifaddrs_head);
    YASIO_LOGV("   address has no name/label, getting one from interface");
    ifa->ifa_name = name ? strdup(name) : NULL;
  }
  YASIO_LOGV("   address label: %s", ifa->ifa_name);

  if (calculate_address_netmask(ifa, net_address) < 0)
  {
    goto error;
  }

  return ifa;

error : {
  /* errno may be modified by free, or any other call inside the free_single_xamarin_ifaddrs
   * function. We don't care about errors in there since it is more important to know how we
   * failed to obtain the link address and not that we went OOM. Save and restore the value
   * after the resources are freed.
   */
  int errno_save = errno;
  free_single_ifaddrs(&ifa);
  errno = errno_save;
  return NULL;
}
}

static struct ifaddrs* get_link_info(const struct nlmsghdr* message)
{
  ssize_t length;
  struct rtattr* attribute;
  struct ifinfomsg* net_interface;
  struct ifaddrs* ifa             = NULL;
  struct sockaddr_ll_extended* sa = NULL;

  assert(message);
  net_interface = reinterpret_cast<ifinfomsg*>(NLMSG_DATA(message));
  length        = static_cast<ssize_t>(message->nlmsg_len - NLMSG_LENGTH(sizeof(*net_interface)));
  if (length <= 0)
  {
    goto error;
  }

  ifa = reinterpret_cast<ifaddrs*>(calloc(1, sizeof(*ifa)));
  if (!ifa)
  {
    goto error;
  }

  ifa->ifa_flags = net_interface->ifi_flags;
  attribute      = IFLA_RTA(net_interface);
  while (RTA_OK(attribute, length))
  {
    switch (attribute->rta_type)
    {
      case IFLA_IFNAME:
        ifa->ifa_name = strdup(reinterpret_cast<const char*>(RTA_DATA(attribute)));
        if (!ifa->ifa_name)
        {
          goto error;
        }
        break;

      case IFLA_BROADCAST:
        YASIO_LOGV("   interface broadcast (%u bytes)", (unsigned int)RTA_PAYLOAD(attribute));
        if (fill_ll_address(&sa, net_interface, RTA_DATA(attribute), RTA_PAYLOAD(attribute)) < 0)
        {
          goto error;
        }
        ifa->ifa_broadaddr = (struct sockaddr*)sa;
        break;

      case IFLA_ADDRESS:
        YASIO_LOGV("   interface address (%u bytes)", (unsigned int)RTA_PAYLOAD(attribute));
        if (fill_ll_address(&sa, net_interface, RTA_DATA(attribute), RTA_PAYLOAD(attribute)) < 0)
        {
          goto error;
        }
        ifa->ifa_addr = (struct sockaddr*)sa;
        break;

      default:;
    }

    attribute = RTA_NEXT(attribute, length);
  }
  YASIO_LOGV("link flags: 0x%X", ifa->ifa_flags);
  return ifa;

error:
  if (sa)
    free(sa);
  free_single_ifaddrs(&ifa);

  return NULL;
}
typedef int (*getifaddrs_impl_fptr)(struct ifaddrs**);
typedef void (*freeifaddrs_impl_fptr)(struct ifaddrs* ifa);

static getifaddrs_impl_fptr getifaddrs_impl   = NULL;
static freeifaddrs_impl_fptr freeifaddrs_impl = NULL;

static void getifaddrs_init() { get_ifaddrs_impl(&getifaddrs_impl, &freeifaddrs_impl); }
} // namespace internal

/* The getifaddrs/freeifaddrs
 * We don't use 'struct ifaddrs' since that doesn't exist in Android's bionic, but since our
 * version of the structure is 100% compatible we can just use it instead
 */
inline void freeifaddrs(struct ifaddrs* ifa)
{
  struct ifaddrs *cur, *next;

  if (!ifa)
    return;

  if (internal::freeifaddrs_impl)
  {
    (*internal::freeifaddrs_impl)(ifa);
    return;
  }

  cur = ifa;
  while (cur)
  {
    next = cur->ifa_next;
    internal::free_single_ifaddrs(&cur);
    cur = next;
  }
}
inline int getifaddrs(struct ifaddrs** ifap)
{
  static std::mutex _getifaddrs_init_lock;
  static bool _getifaddrs_initialized;

  if (!_getifaddrs_initialized)
  {
    std::lock_guard<std::mutex> lock(_getifaddrs_init_lock);
    if (!_getifaddrs_initialized)
    {
      internal::getifaddrs_init();
      _getifaddrs_initialized = true;
    }
  }

  int ret = -1;

  if (internal::getifaddrs_impl)
    return (*internal::getifaddrs_impl)(ifap);

  if (!ifap)
    return ret;

  *ifap                        = NULL;
  struct ifaddrs* ifaddrs_head = 0;
  struct ifaddrs* last_ifaddr  = 0;
  internal::netlink_session session;

  if (internal::open_netlink_session(&session) < 0)
  {
    goto cleanup;
  }

  /* Request information about the specified link. In our case it will be all of them since we
     request the root of the link tree below
  */
  if ((internal::send_netlink_dump_request(&session, RTM_GETLINK) < 0) ||
      (internal::parse_netlink_reply(&session, &ifaddrs_head, &last_ifaddr) < 0) ||
      (internal::send_netlink_dump_request(&session, RTM_GETADDR) < 0) ||
      (internal::parse_netlink_reply(&session, &ifaddrs_head, &last_ifaddr) < 0))
  {
    freeifaddrs(ifaddrs_head);
    goto cleanup;
  }

  ret   = 0;
  *ifap = ifaddrs_head;

cleanup:
  if (session.sock_fd >= 0)
  {
    close(session.sock_fd);
    session.sock_fd = -1;
  }

  return ret;
}
} // namespace yasio

#endif

#endif
