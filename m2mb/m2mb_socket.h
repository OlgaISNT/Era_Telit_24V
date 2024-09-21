/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_socket.h

  @brief
    Socket support

  @details
    socket APIs from BSD and Linux

  @notes
    Dependencies:
    #include <m2mb_types.h>
    #include <m2mb_socket.h>
    #include <m2mb_trace.h>

  @author
    M.Francesca Tammaro

  @date
    20/02/2017
*/

#ifndef  M2M_M2MB_SOCKET_API_H
#define  M2M_M2MB_SOCKET_API_H



/* Global declarations ==========================================================================*/
#define M2MB_SOCKET_BSD_INVALID_SOCKET  (M2MB_SOCKET_BSD_SOCKET)(~0)     /* Invalid socket handle */

/* M2M Socket Types */
#define M2MB_SOCKET_BSD_SOCK_STREAM    1   /* Stream socket type used for TCP */
#define M2MB_SOCKET_BSD_SOCK_DGRAM     2   /* Datagram socket type used for UDP */
#define M2MB_SOCKET_BSD_SOCK_RAW       3   /* Raw socket type */

/* M2M Socket Address Families */
#define M2MB_SOCKET_BSD_AF_UNSPEC      0   /* Unspecified Address Family */
#define M2MB_SOCKET_BSD_AF_INET        2   /* Internetwork: UDP, TCP, etc. */
#define M2MB_SOCKET_BSD_AF_INET6      10

/* M2M Socket Protocol Families */
#define M2MB_SOCKET_BSD_PF_UNSPEC      0   /* Unspecified Protocol Family */
#define M2MB_SOCKET_BSD_PF_INET        2   /* Internetwork: UDP, TCP, etc. */

/* M2M Socket Protocols */
#define M2MB_SOCKET_BSD_IPPROTO_IP     0   /* Dummy for IP */
#define M2MB_SOCKET_BSD_IPPROTO_TCP    6   /* Transmission Control Protocol */
#define M2MB_SOCKET_BSD_IPPROTO_UDP   17   /* User Datagram Protocol */
#define M2MB_SOCKET_BSD_IPPROTO_ICMP   1   /* Internet Control Message Protocol */

/* ======================================================================= */

/* Level number for M2MB_SOCKET_bsd_get_sock_opt() and M2MB_SOCKET_bsd_set_sock_opt() to apply to socket itself. */
#define M2MB_SOCKET_BSD_SOL_SOCKET       0xffff          /* options for socket level */

/* M2M Socket Option Flags */
#define M2MB_SOCKET_BSD_SO_DEBUG         0x0001          /* Turn on debugging info recording */
#define M2MB_SOCKET_BSD_SO_ACCEPTCONN    0x0002          /* Socket has had listen() */
#define M2MB_SOCKET_BSD_SO_REUSEADDR     0x0004          /* Allow local address reuse */
#define M2MB_SOCKET_BSD_SO_KEEPALIVE     0x0008          /* Keep connections alive,  not enabled by default */
#define M2MB_SOCKET_BSD_SO_DONTROUTE     0x0010          /* Just use interface addresses */
#define M2MB_SOCKET_BSD_SO_BROADCAST     0x0020          /* Permit sending of broadcast msgs */
#define M2MB_SOCKET_BSD_SO_LINGER        0x0080          /* Linger on close if data present  */
#define M2MB_SOCKET_BSD_SO_OOBINLINE     0x0100          /* Leave received OOB data in line  */
#define M2MB_SOCKET_BSD_SO_DONTLINGER   (INT32)(~M2MB_SOCKET_BSD_SO_LINGER) /* Dont Linger    */
#define M2MB_SOCKET_BSD_SO_SNDBUF        0x1001          /* Send buffer size                       */
#define M2MB_SOCKET_BSD_SO_RCVBUF        0x1002          /* Receive buffer size                    */
#define M2MB_SOCKET_BSD_SO_SNDLOWAT      0x1003          /* Send low-water mark                    */
#define M2MB_SOCKET_BSD_SO_RCVLOWAT      0x1004          /* Receive low-water mark                 */
#define M2MB_SOCKET_BSD_SO_SNDTIMEO      0x1005          /* Send timeout                           */
#define M2MB_SOCKET_BSD_SO_RCVTIMEO      0x1006          /* Receive timeout                        */
#define M2MB_SOCKET_BSD_SO_ERROR         0x1007          /* Get error status and clear             */
#define M2MB_SOCKET_BSD_SO_TYPE          0x1008          /* Get socket type                        */
#define M2MB_SOCKET_BSD_SO_TXDATA        0x1009          /* Get a count of bytes in send buffer    */
#define M2MB_SOCKET_BSD_SO_RXDATA        0x1010          /* Get a count of bytes in receive buffer */

#define M2MB_SOCKET_BSD_TCP_NODELAY      0x01            /* Don't delay send to coalesce packets   */
#define M2MB_SOCKET_BSD_TCP_KEEPALIVE    0x03            /* TCP keepalive timer  */

/* ======================================================================= */

/* M2M Internet address.*/

/* Any internet address. */
#define M2MB_SOCKET_BSD_INADDR_ANY           (UINT32) 0x00000000

/* Loopback internet address. */
#define M2MB_SOCKET_BSD_INADDR_LOOPBACK    (UINT32) 0x7f000001

/* Broadcast internet address. */
#define M2MB_SOCKET_BSD_INADDR_BROADCAST     (UINT32) 0xffffffff

/* ======================================================================= */


/* M2M Socket constants for M2MB_SOCKET_bsd_shutdown() */
#define M2MB_SOCKET_BSD_SHUT_RD    0x00    /* Read socket */
#define M2MB_SOCKET_BSD_SHUT_WR    0x01    /* Write socket */
#define M2MB_SOCKET_BSD_SHUT_RDWR  0x02    /* Read Write socket */


/* ======================================================================= */

/* M2M Socket I/O control options for  m2mb_socket_bsd_ioctl() */

/* command to get the number of bytes to read */
#define M2MB_SOCKET_BSD_FIONREAD        0

/* command to select the blocking or non-blocking mode */
#define M2MB_SOCKET_BSD_FIONBIO         1


/* ======================================================================= */

/* M2M Socket Error Types */
/* Errors can be retrieved via the m2mb_socket_errno() */

#define M2MB_SOCKET_BSD_SOCKET_ERROR        (-1)
#define M2MB_SOCKET_BSD_SOCKNOERROR            0
#define M2MB_SOCKET_BSD_EUNDEFINED             1
#define M2MB_SOCKET_BSD_EACCES                 2
#define M2MB_SOCKET_BSD_EADDRINUSE             3
#define M2MB_SOCKET_BSD_EADDRNOTAVAIL          4
#define M2MB_SOCKET_BSD_EAFNOSUPPORT           5
#define M2MB_SOCKET_BSD_EALREADY               6
#define M2MB_SOCKET_BSD_EBADF                  7
#define M2MB_SOCKET_BSD_ECONNABORTED           8
#define M2MB_SOCKET_BSD_ECONNREFUSED           9
#define M2MB_SOCKET_BSD_ECONNRESET            10
#define M2MB_SOCKET_BSD_EDESTADDRREQ          11
#define M2MB_SOCKET_BSD_EFAULT                12
#define M2MB_SOCKET_BSD_EHOSTDOWN             13
#define M2MB_SOCKET_BSD_EHOSTUNREACH          14
#define M2MB_SOCKET_BSD_EINPROGRESS           15
#define M2MB_SOCKET_BSD_EINTR                 16
#define M2MB_SOCKET_BSD_EINVAL                17
#define M2MB_SOCKET_BSD_EISCONN               18
#define M2MB_SOCKET_BSD_EMFILE                19
#define M2MB_SOCKET_BSD_EMSGSIZE              20
#define M2MB_SOCKET_BSD_ENETDOWN              21
#define M2MB_SOCKET_BSD_ENETRESET             22
#define M2MB_SOCKET_BSD_ENETUNREACH           23
#define M2MB_SOCKET_BSD_ENOBUFS               24
#define M2MB_SOCKET_BSD_ENOPROTOOPT           25
#define M2MB_SOCKET_BSD_ENOTCONN              26
#define M2MB_SOCKET_BSD_ENOTSOCK              27
#define M2MB_SOCKET_BSD_EOPNOTSUPP            28
#define M2MB_SOCKET_BSD_EPFNOSUPPORT          29
#define M2MB_SOCKET_BSD_EPROTONOSUPPORT       30
#define M2MB_SOCKET_BSD_EPROTOTYPE            31
#define M2MB_SOCKET_BSD_ESHUTDOWN             32
#define M2MB_SOCKET_BSD_ESOCKTNOSUPPORT       33
#define M2MB_SOCKET_BSD_ETIMEDOUT             34
#define M2MB_SOCKET_BSD_EWOULDBLOCK           35

/* ======================================================================= */

/* Errors used by the DNS API functions, h_errno can be one of them */

#define M2MB_SOCKET_HOST_NOT_FOUND  210
#define M2MB_SOCKET_NO_DATA         211
#define M2MB_SOCKET_NO_RECOVERY     212
#define M2MB_SOCKET_TRY_AGAIN       213
#define M2MB_SOCKET_ERANGE          214


/* Global typedefs ==============================================================================*/
/* Socket identifier */
typedef iHANDLE M2MB_SOCKET_BSD_SOCKET;

/* Structure used for manipulating linger option. */
struct M2MB_SOCKET_BSD_LINGER
{
  INT32     l_onoff;                    /* option on/off */
  INT32     l_linger;                   /* linger time */
};

/* Structure used by TCP/IP stack to store most addresses. */
struct M2MB_SOCKET_BSD_SOCKADDR
{
  UINT8   _internal_sa_len;            /* INTERNAL USE ONLY */
  UINT8   sa_family;
  CHAR   sa_data[14];
};

/* ======================================================================= */

/* Structure for storing Internet address. */
struct M2MB_SOCKET_BSD_IN_ADDR
{
  UINT32 s_addr;              /* 32 bits inet address */
};

/* Socket address, internet style. */
struct M2MB_SOCKET_BSD_SOCKADDR_IN
{
  UINT8         _internal_sin_len;  /* INTERNAL USE ONLY */
  UINT8         sin_family;         /* M2M Socket Protocol Families, e.g. M2MB_SOCKET_BSD_PF_INET. */
  UINT16          sin_port;         /* 16 bits port number. */
  struct M2MB_SOCKET_BSD_IN_ADDR  sin_addr; /* 32 bits inet address (IP). */
  CHAR          sin_zero[8];        /* INTERNAL USE ONLY */
};

struct M2MB_SOCKET_BSD_IN6_ADDR
{
  UINT32 s_addr[4];
};

struct M2MB_SOCKET_BSD_IPV6_ADDR
{
  union
  {
    UINT8   addr8[16];
    UINT16  addr16[8];
    UINT32  addr32[4];
  } v6_v;

#define addr8_s     v6_v.addr8
#define addr16_s    v6_v.addr16
#define addr32_s    v6_v.addr32

};

/* Socket address, internet style. */
struct M2MB_SOCKET_BSD_SOCKADDR_IN6
{
  UINT8   _internal_sin6_len;          /* INTERNAL USE ONLY */
  UINT8   sin6_family;                 /* M2M Socket Protocol Families, e.g. M2MB_SOCKET_BSD_PF_INET. */
  UINT16  sin6_port;                   /* 16 bits port number. */
  UINT32  sin6_flowinfo;
  struct M2MB_SOCKET_BSD_IPV6_ADDR  sin6_addr; /* 32 bits inet address (IP). */
  UINT32  sin6_scope_id;
};

/* Structure returned by network data base library. */
struct M2MB_SOCKET_BSD_HOSTENT
{
  CHAR  *h_name;       /* Official name of host */
  CHAR **h_aliases;    /* Pointer to struct of aliases */
  INT32  h_addrtype;   /* Host address type, equals M2MB_SOCKET_BSD_AF_INET */
  INT32  h_length;     /* Length of address */
  CHAR **h_addr_list;  /* Pointer to array of pointers with inet v4 addresses */
};

/* Structure used in M2MB_SOCKET_bsd_select() call. */
struct M2MB_SOCKET_BSD_TIMEVAL
{
  INT32 m_tv_sec;       /* seconds */
  INT32 m_tv_usec;      /* microseconds */
};

/* FD set size used by M2MB_SOCKET_bsd_select(). */
#define M2MB_SOCKET_BSD_FD_SETSIZE      32

/* Note: This is already under investigation.

    FD set used by m2mb_socket_bsd_select().
    Users please do not access to this struct directly, instead use the APIs
    m2mb_socket_bsd_fd_zero_func, m2mb_socket_bsd_fd_set_func,
    m2mb_socket_bsd_fd_clr_func,m2mb_socket_bsd_fd_isset_func */
typedef struct M2MB_SOCKET_BSD_FD_SET
{
  INT32  fd_count;                                       /* How many are SET? */
  UINT32 fd_array[M2MB_SOCKET_BSD_FD_SETSIZE];  /* Bit map of SOCKET Descriptors. */
} M2MB_SOCKET_BSD_FD_SET_T;



/* Global functions =============================================================================*/
/* ======================================================================= */

/* M2M BSD socket interface */

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    create an endpoint for communication

  @details
    creates an unbound socket in a communications domain,
    and returns a file descriptor that can be used in later function calls that operate on socket

  @param[in] domain
    specifies the communications domain in which a socket is to be created:
    M2MB_SOCKET_BSD_AF_INET  - IPv4 Internet protocols
    M2MB_SOCKET_BSD_AF_INET6  - IPv6 Internet protocols
  @param[in] type
    specifies the type of socket to be created:
    M2MB_SOCKET_BSD_SOCK_STREAM     - TCP
    M2MB_SOCKET_BSD_SOCK_DGRAM      - UDP
  @param[in] protocol
    specifies a particular protocol to be used with the socket:
    M2MB_SOCKET_BSD_IPPROTO_TCP
    M2MB_SOCKET_BSD_IPPROTO_UDP
    specifying a protocol of 0 causes m2mb_socket_bsd_socket() to use an unspecified default protocol
    appropriate for the requested socket type

  @return
    On success, a handle for the new socket is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_SOCKET_BSD_SOCKET m2mb_socket_bsd_socket( INT32 domain, INT32 type, INT32 protocol );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    bind the address with the socket

  @details
    bind is typically used on the server side, and associates a socket with a socket address structure, i.e.
    a specified local port number and IP address

  @param[in] s
    specifies the file descriptor of the socket to be bound
  @param[in] name
    points to a sockaddr structure containing the address to be bound to the socket.
    The length and format of the address depend on the address family of the socket:
    M2MB_SOCKET_BSD_SOCKADDR_IN is used for IPv4 Internet protocols
    M2MB_SOCKET_BSD_SOCKADDR_IN6 is used for IPv6 Internet protocols
  @param[in] namelen
    specifies the length of the sockaddr structure pointed to by name

  @return
    On success, 0 is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_bind( M2MB_SOCKET_BSD_SOCKET s, struct M2MB_SOCKET_BSD_SOCKADDR *name, INT32 namelen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    connect a socket

  @details
    Establishes a connection to the specified address

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] name
    points to a sockaddr structure containing the peer address.
    The length and format of the address depend on the address family of the socket:
    M2MB_SOCKET_BSD_SOCKADDR_IN is used for IPv4 Internet protocols
    M2MB_SOCKET_BSD_SOCKADDR_IN6 is used for IPv6 Internet protocols
  @param[in] namelen
    specifies the length of the sockaddr structure pointed to by name

  @return
    On success, 0 is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_connect( M2MB_SOCKET_BSD_SOCKET s, const struct M2MB_SOCKET_BSD_SOCKADDR *name, INT32 namelen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    listen for connections on a socket

  @details
    marks the socket referred to by sockfd as a passive socket, that is,
    as a socket that will be used to accept incoming connection requests using m2mb_socket_bsd_accept

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] backlog
    defines the maximum length to which the queue of pending connections for s may grow

  @return
    On success, 0 is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_listen( M2MB_SOCKET_BSD_SOCKET s, INT32 backlog );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    accept a connection on a socket

  @details
    allows an incoming connection attempt on the specified socket. Accept is used on the server side.
    It accepts a received incoming attempt to create a new TCP connection from the remote client,
    and creates a new socket associated with the socket address pair of this connection

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] name
    it is a pointer to a sockaddr structure. This structure is filled in with the address of the peer socket
  @param[in] namelen
    it is a value-result argument: the caller must initialize it to contain the size (in bytes) of the structure pointed to by name;
    on return it will contain the actual size of the peer address

  @return
    On success, a handle for the new socket is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_SOCKET_BSD_SOCKET m2mb_socket_bsd_accept( M2MB_SOCKET_BSD_SOCKET s, struct M2MB_SOCKET_BSD_SOCKADDR *name, INT32 *namelen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    close a socket

  @details
    closes a file descriptor, so that it no longer refers to any file and may be reused

  @param[in] s
    specifies the file descriptor of the socket

  @return
    On success, 0 is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_close( M2MB_SOCKET_BSD_SOCKET s );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    get the name of the peer socket

  @details
    retrieves the peer address of the specified socket, store this address in the sockaddr structure pointed to by name,
    and store the length of this address in the object pointed to by namelen

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] name
    is a pointer to a sockaddr structure. This structure is filled in with the address of the peer socket
  @param[in] namelen
    it is a value-result argument: the caller must initialize it to contain the size (in bytes) of the structure pointed to by name;
    on return it will contain the actual size of the peer address

  @return
    On success, 0 is returned On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_get_peer_name( M2MB_SOCKET_BSD_SOCKET s, struct M2MB_SOCKET_BSD_SOCKADDR *name, INT32 *namelen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    get socket name

  @details
    retrieve the local address of the specified socket, store this address in the sockaddr structure pointed to by name,
    and store the length of this address in the object pointed to by namelen

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] name
    is a pointer to a sockaddr structure. This structure is filled in with the local address of the socket
  @param[in] namelen
    it is a value-result argument: the caller must initialize it to contain the size (in bytes) of the structure pointed to by name;
    on return it will contain the actual size of the peer address

  @return
    On success, 0 is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_get_sock_name( M2MB_SOCKET_BSD_SOCKET s, struct M2MB_SOCKET_BSD_SOCKADDR *name, INT32 *namelen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    receive a message from a socket

  @details
    This function should be invoked to receive the data on the specified socket.
    Depending on the socket configuration, this function can perform in a blocking or non-blocking mode.
    In case of blocking, this function will not return until data is received (not neccessarily all requested data).
    In case of non-blocking, this function will return the data pending on the specified socket,
    but will not wait for additional data (in case the size requested is not reached)

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] buf
    Pointer to a buffer for received message
  @param[in] len
    Number of bytes to receive
  @param[in] flags
    only 0 supported

  @return
    number of bytes received, or -1 if an error occurred. The return value will be 0 when the peer has performed an orderly shutdown

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_recv( M2MB_SOCKET_BSD_SOCKET s, void *buf, INT32 len, INT32 flags );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    receive a message from a socket

  @details
    This function receives the data on the given socket and stores the source address.
    This function shall be used only for datagram sockets.
    Depending on the socket configuration, this function can perform in a blocking or non-blocking mode.
    In case of blocking, this function will not return until data is received (not neccessarily all requested data).
    In case of non-blocking, this function will return the data pending on the specified socket,
    but will not wait for additional data (in case the size requested is not reached)

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] buf
    Pointer to a buffer for received message
  @param[in] len
    Number of bytes to receive
  @param[in] flags
    only 0 supported
  @param[in] from
    is a pointer to a sockaddr structure. This structure is filled in with the address of the peer socket
  @param[in] fromlen
    it is a value-result argument: the caller must initialize it to contain the size (in bytes) of the structure pointed to by addr;
    on return it will contain the actual size of the peer address

  @return
    number of bytes received, or -1 if an error occurred.
    The return value will be 0 when the peer has performed an orderly shutdown

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_recv_from( M2MB_SOCKET_BSD_SOCKET s, void *buf, INT32 len, INT32 flags,
                                 struct M2MB_SOCKET_BSD_SOCKADDR *from, INT32 *fromlen );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    send a message on a socket

  @details
    This function should be invoked to send data using the specified socket.
    Depending on the socket configuration, this function can perform in a blocking or non-blocking mode.
    In case of blocking, this function will not return until data is sent (not neccessarily all requested data).
    In case of non-blocking, this function will send the data using only the stack available resources
    and will not wait for resources to be free

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] buf
    pointer to a buffer containing the message to be sent
  @param[in] len
    number of bytes to send
  @param[in] flags
    only 0 supported

  @return
    number of characters sent. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_send( M2MB_SOCKET_BSD_SOCKET s, const void *buf, INT32 len, INT32 flags );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    send a message on a socket

  @details
    This function should be invoked to send data using the specified socket.
    Depending on the socket configuration, this function can perform in a blocking or non-blocking mode.
    In case of blocking, this function will not return until data is sent (not neccessarily all requested data).
    In case of non-blocking, this function will send the data using only the stack available resources
    and will not wait for resources to be free

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] buf
    pointer to a buffer containing message to be sent
  @param[in] len
    number of bytes to send
  @param[in] flags
    only 0 supported
  @param[in] to
    points to a sockaddr structure containing the peer address.
    The length and format of the address depend on the address family of the socket
  @param[in] tolen
    specifies the length of the sockaddr structure pointed to by to

  @return
    number of characters sent. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_send_to( M2MB_SOCKET_BSD_SOCKET s, const void *buf, INT32 len, INT32 flags,
                               const struct M2MB_SOCKET_BSD_SOCKADDR *to, INT32 tolen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    get options from sockets

  @details
    This function is used to retrieve the current value of a particular socket option for the specified socket,
    see m2mb_socket_option_flags socket options

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] level
    protocol level the option exists at
  @param[in] optname
    name of the option
  @param[in] optval
    pointer to a buffer in which the value for the requested option is to be returned
  @param[in] optlen
    option length in bytes

  @return
    On success,  is returned0. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_get_sock_opt( M2MB_SOCKET_BSD_SOCKET s, INT32 level, INT32 optname, void *optval, INT32 *optlen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    set options on sockets

  @details
    This function is used to set a particular socket option for the specified socket, see m2mb_socket_option_flags socket options

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] level
    protocol level the option exists at
  @param[in] optname
    name of the option
  @param[in] optval
    pointer to a buffer with the requested option to be set
  @param[in] optlen
    option length in bytes

  @return
    On success, 0 is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_set_sock_opt( M2MB_SOCKET_BSD_SOCKET s, INT32 level, INT32 optname, const void *optval, INT32 optlen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    IO control function

  @details
    used to set a socket to blocking or nonblocking mode (M2MB_SOCKET_BSD_FIONBIO)
    or to retrieve all pending data (M2MB_SOCKET_BSD_FIONREAD)

  @param[in] cmd
    name of the command: M2MB_SOCKET_BSD_FIONREAD,M2MB_SOCKET_BSD_FIONBIO
  @param[in] argp
    pointer to the value (input parameter or output parameter)

  @return
    On success, 0 is returned. On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

INT32 m2mb_socket_bsd_ioctl( M2MB_SOCKET_BSD_SOCKET s, INT32 cmd, void *argp );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    synchronous I/O multiplexing

  @details
    This function is used to prune a provided list of sockets for those that are: ready to read, ready to write or have errors

  @param[in] nfds
    nfds is the highest-numbered file descriptor in any of the three sets, plus 1
  @param[in] readfds
    set of file descriptors that will be watched to see if characters become available for reading.
    On exit, the set is modified in place to indicate which file descriptors actually changed status.
  @param[in] writefds
    set of file descriptors that will be watched to see if a write will not block.
    On exit, the set is modified in place to indicate which file descriptors actually changed status.
  @param[in] exceptfds
    set of file descriptors that will be watched for exceptions.
    On exit, the set is modified in place to indicate which file descriptors actually changed status.
  @param[in] timeout
    specifies the minimum interval that m2mb_socket_bsd_select() should block waiting for a file descriptor to become ready

  @return
    number of sockets which had an event occur at and become ready

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_socket_bsd_select( INT32 nfds, M2MB_SOCKET_BSD_FD_SET_T *readfds, M2MB_SOCKET_BSD_FD_SET_T *writefds,
                              M2MB_SOCKET_BSD_FD_SET_T *exceptfds, const struct M2MB_SOCKET_BSD_TIMEVAL *timeout );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    used for m2mb_socket_bsd_select()

  @details
    clears a set of file descriptors

  @param[in] set
    set of file descriptors

  @return

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void m2mb_socket_bsd_fd_zero_func( M2MB_SOCKET_BSD_FD_SET_T *set );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    used for m2mb_socket_bsd_select()

  @details
    add a file descriptor to a set

  @param[in] fd
    file descriptor
  @param[in] set
    set of file descriptors

  @return
    <Return value description>

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
void m2mb_socket_bsd_fd_set_func( INT32 fd, M2MB_SOCKET_BSD_FD_SET_T *set );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    used for m2mb_socket_bsd_select()

  @details
    remove a file descriptor from a set

  @param[in] fd
    file descriptor
  @param[in] set
    set of file descriptors

  @return
    <Return value description>

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
void m2mb_socket_bsd_fd_clr_func( INT32 fd, M2MB_SOCKET_BSD_FD_SET_T *set );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    used for m2mb_socket_bsd_select()

  @details
    tests to see if a file descriptor is part of a set

  @param[in] fd
    file descriptor
  @param[in] set
    set of file descriptors

  @return
    On success, 0 is returned if the socket is not a member;
    1 is returned if the socket is a member.  On error, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
UINT8 m2mb_socket_bsd_fd_isset_func( INT32 fd, M2MB_SOCKET_BSD_FD_SET_T *set );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Gets the last error occured

  @details
    Gets the last error occured. Should be on a thread bases

  @param[in]

  @return
    socket error code

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
INT32 m2mb_socket_errno( void );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    convertion between host and network byte order

  @details
    converts the unsigned short integer x from host byte order to network byte order

  @param[in] x
    unsigned short integer to be converted

  @return
    result of the conversion

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
UINT16  m2mb_socket_bsd_htons( UINT16 x ); /* Host to network byte order (short) */

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    convertion between host and network byte order

  @details
    onverts the unsigned short integer x from network byte order to host byte order

  @param[in] x
    unsigned short integer to be converted

  @return
    result of the conversion

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
UINT16  m2mb_socket_bsd_ntohs( UINT16 x ); /* Network to host byte order (short) */

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    convertion between host and network byte order

  @details
    converts the unsigned integer x from host byte order to network byte order.

  @param[in] x
    unsigned integer to be converted

  @return
    result of the conversion

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
UINT32  m2mb_socket_bsd_htonl( UINT32 x ); /* Host to network byte order (long) */

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    convertion between host and network byte order

  @details
    converts the unsigned integer x from network byte order to host byte order.

  @param[in] x
    unsigned integer to be converted

  @return
    result of the conversion

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
UINT32  m2mb_socket_bsd_ntohl( UINT32 x ); /* Network to host byte order (long) */

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    convert IPv4 and IPv6 addresses from binary to text form

  @details
    this function converts the network address structure src in the af address family into a character string.
    The resulting string is copied to the buffer pointed to by dst, which must be a non-NULL pointer.
    The caller specifies the number of bytes available in this buffer in the argument size

  @param[in] af
    Address family. M2MB_SOCKET_BSD_AF_INET for IPv4, M2MB_SOCKET_BSD_AF_INET6 for IPv6
  @param[in] src
    Pointer to IPv4 or IPv6 address
  @param[in] dst
    Pointer to output buffer to contain IPv4/IPv6 address string
  @param[in] size
    Size of output buffer in bytes

  @return
    on success, returns a non-NULL pointer to dst. NULL is returned if there was an error

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
CHAR *m2mb_socket_bsd_inet_ntop( INT32 af, const void *src, CHAR *dst, SIZE_T size );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    convert IPv4 and IPv6 addresses from text to binary form

  @details
    converts the character string src into a network address structure in the af address family,
    then copies the network address structure to dst

  @param[in] af
    Address family. M2MB_SOCKET_BSD_AF_INET for IPv4, M2MB_SOCKET_BSD_AF_INET6 for IPv6
  @param[in] src
    IPv4 or IPv6 address string (null-terminated)
  @param[in] dst
    Resulting IPv4/IPv6 address: struct M2MB_SOCKET_BSD_IN_ADDR in case of IPv4, struct M2MB_SOCKET_BSD_IPV6_ADDR in case of IPv6

  @return
    returns 1 on success (network address was successfully converted).
    0 is returned if src does not contain a character string representing a valid network address in the specified address family.
    If af does not contain a valid address family, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
INT32 m2mb_socket_bsd_inet_pton( INT32 af, const CHAR *src, void *dst );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    get network host entry

  @details
    implements a "standard" Unix version of gethostbyname2_r() but with parameter "cid"
    to specify the PDP context to be used for transferring data

  @param[in] name
    either a hostname, or an IPv4 address in standard dot notation or an IPv6 address in colon notation
  @param[in] af
    address family. M2MB_SOCKET_BSD_AF_INET for IPv4, M2MB_SOCKET_BSD_AF_INET6 for IPv6
  @param[in] ret
    pre-allocated struct where to store the result
  @param[in] buf
    pre-allocated buffer where to store additional data
  @param[in] buflen
    the size of buf
  @param[in] result
    pointer to a hostent pointer that is set to ret on success and set to zero on error
  @param[in] h_errnop
    pointer to an int where to store errors (instead of modifying the global h_errno)
  @param[in] cid
   PDP context to be used

  @return
    0 on success, non-zero on error, additional error information is stored in *h_errnop instead of h_errno

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
INT32 m2mb_socket_bsd_get_host_by_name_2_r_cid( const CHAR *name, INT32 af,
    struct M2MB_SOCKET_BSD_HOSTENT *ret, CHAR *buf, SIZE_T buflen,
    struct M2MB_SOCKET_BSD_HOSTENT **result, INT32 *h_errnop, UINT8 cid );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    link a socket to a cid

  @details
    specifies the PDP context to be used for transferring data on the socket

  @param[in] s
    specifies the file descriptor of the socket
  @param[in] cid
    PDP context to be used

  @return
    on success, 0 is returned. On error, -1 is returned

  @note
    This API is implemented through a bind() of the socket to the local IP address.
    Therefore, a call to this API and m2mb_socket_bsd_bind() can lead to an error.
    If m2mb_socket_bsd_bind() is needed, it is suggested to avoid m2mb_socket_set_cid() and call only
    m2mb_socket_bsd_bind(), with the local IP related to the specified cid.
    The local IP address can be obtained through m2mb_pdp_get_my_ip( handle,cid... ).

  @b
    if( m2mb_socket_set_cid( fd, cid ) == -1 )

  @code
    INT32 fd;
    UINT8 cid;
    M2MB_RESULT_E res = M2MB_RESULT_SUCCESS;

    fd = m2mb_socket_bsd_socket( M2MB_SOCKET_BSD_PF_INET,
                                 M2MB_SOCKET_BSD_SOCK_STREAM,
                                 M2MB_SOCKET_BSD_IPPROTO_TCP );
    if( 0 < fd )
    {
      if( m2mb_socket_set_cid( fd, cid ) == -1 )
      {
        m2mb_trace_printf( M2MB_TC_FTPC, M2MB_TL_ERROR, "set cid returned error" );
        ret = M2MB_RESULT_FAIL;
      }
    }
    return ret;
  @endcode
*/
INT32 m2mb_socket_set_cid( M2MB_SOCKET_BSD_SOCKET s, UINT8 cid );


#endif  /* M2M_M2MB_SOCKET_API_H */
