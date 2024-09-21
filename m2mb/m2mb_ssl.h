/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_ssl.h

  @brief
    SSL TLS support

  @details
    SSL TLS secure socket APIs

  @notes
    Dependencies:
    #include <m2mb_types.h>
    #include <m2mb_socket.h>
    #include <m2mb_trace.h>

  @author
    Andrea Pirnetti
    Atul Chavan

  @date
    12/11/2019
*/

#ifndef M2M_M2MB_SSL_H
#define M2M_M2MB_SSL_H

/* Global declarations ==========================================================================*/

/* Failure return codes MUST be < 0 */
#define M2MB_SSL_SUCCESS             0   /* Generic Success */
#define M2MB_SSL_FAILURE            -1   /* Generic failure */
#define M2MB_SSL_ARG_FAIL           -6   /* Failure due to bad function param */
#define M2MB_SSL_PLATFORM_FAIL      -7   /* Not used */
#define M2MB_SSL_MEM_FAIL           -8   /* Not used */
#define M2MB_SSL_LIMIT_FAIL         -9   /* Not used */
#define M2MB_SSL_UNSUPPORTED_FAIL  -10   /* Not used */
#define M2MB_SSL_PROTOCOL_FAIL     -12   /* A protocol error occurred */
#define M2MB_SSL_TIMEOUT_FAIL      -13   /* A timeout occurred and MAY be an error */
#define M2MB_SSL_INTERRUPT_FAIL    -14   /* An interrupt occurred and MAY be an error */
#define M2MB_SSL_WRITE_ERROR       -15   /* An error occured while encoding on socket */
#define M2MB_SSL_READ_ERROR        -16   /* An error occured while decoding from socket */
#define M2MB_SSL_END_OF_FILE       -17   /* There is no data to read in SSL */
#define M2MB_SSL_CLOSE_NOTIFY      -18   /* SSL connection has been closed by remote host */
#define M2MB_SSL_CERT_AUTH_FAIL    -35   /* Authentication fails */
#define M2MB_SSL_FULL              -50   /* Not used */
#define M2MB_SSL_ALERT             -54   /* We've decoded an alert */
#define M2MB_SSL_FILE_NOT_FOUND    -55   /* File not found */

#define M2MB_SSL_MAX_CA_LIST        10
#define M2MB_SSL_MAX_CIPHERSUITES    8

/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_SSL_CTXT_HANDLE;
typedef HANDLE M2MB_SSL_CONFIG_HANDLE;
typedef uiHANDLE M2MB_SSL_CONNECTION_HANDLE;

/* TLS protocol version */
typedef enum M2MB_SSL_PROTOCOL_VERSION_E
{
  M2MB_SSL_PROTOCOL_TLS_1_0,
  M2MB_SSL_PROTOCOL_TLS_1_1,
  M2MB_SSL_PROTOCOL_TLS_1_2,
  M2MB_SSL_PROTOCOL_TLS_1_3,
  M2MB_SSL_PROTOCOL_DTLS_1_0,
  M2MB_SSL_PROTOCOL_DTLS_1_2,
  M2MB_SSL_PROTOCOL_TLS /* use highest and allow downgrade */
} M2MB_SSL_PROTOCOL_VERSION_E;

/* TLS ciphersuites */
typedef enum M2MB_SSL_CIPHER_SUITE_E
{
  M2MB_TLS_PSK_WITH_RC4_128_SHA,       /* No Support */
  M2MB_TLS_PSK_WITH_3DES_EDE_CBC_SHA,  /* No Support */
  M2MB_TLS_PSK_WITH_AES_128_CBC_SHA,
  M2MB_TLS_PSK_WITH_AES_256_CBC_SHA,
  M2MB_TLS_PSK_WITH_AES_128_GCM_SHA256,
  M2MB_TLS_PSK_WITH_AES_256_GCM_SHA384,
  M2MB_TLS_PSK_WITH_AES_128_CBC_SHA256,
  M2MB_TLS_PSK_WITH_AES_256_CBC_SHA384,
  M2MB_TLS_RSA_WITH_AES_128_CBC_SHA,
  M2MB_TLS_DHE_RSA_WITH_AES_128_CBC_SHA,
  M2MB_TLS_RSA_WITH_AES_256_CBC_SHA,
  M2MB_TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
  M2MB_TLS_RSA_WITH_AES_128_CBC_SHA256,
  M2MB_TLS_RSA_WITH_AES_256_CBC_SHA256,
  M2MB_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
  M2MB_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
  M2MB_TLS_RSA_WITH_AES_128_GCM_SHA256,
  M2MB_TLS_RSA_WITH_AES_256_GCM_SHA384,
  M2MB_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,
  M2MB_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384,
  M2MB_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA,
  M2MB_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA,
  M2MB_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
  M2MB_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
  M2MB_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA,
  M2MB_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA,
  M2MB_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
  M2MB_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
  M2MB_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
  M2MB_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
  M2MB_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,
  M2MB_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384,
  M2MB_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
  M2MB_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384,
  M2MB_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256,
  M2MB_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384,
  M2MB_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
  M2MB_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
  M2MB_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,
  M2MB_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384,
  M2MB_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
  M2MB_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
  M2MB_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256,
  M2MB_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384,
  M2MB_TLS_RSA_WITH_AES_128_CCM,
  M2MB_TLS_RSA_WITH_AES_256_CCM,
  M2MB_TLS_DHE_RSA_WITH_AES_128_CCM,
  M2MB_TLS_DHE_RSA_WITH_AES_256_CCM,
  M2MB_TLS_RSA_WITH_AES_128_CCM_8,
  M2MB_TLS_RSA_WITH_AES_256_CCM_8,
  M2MB_TLS_DHE_RSA_WITH_AES_128_CCM_8,
  M2MB_TLS_DHE_RSA_WITH_AES_256_CCM_8,
  M2MB_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256,
  M2MB_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
  M2MB_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256,

  /* Additional Cipher Suites TLS v1.3*/
  M2MB_TLS_AES_128_GCM_SHA256,
  M2MB_TLS_AES_256_GCM_SHA384,
  M2MB_TLS_CHACHA20_POLY1305_SHA256,
  M2MB_TLS_AES_128_CCM_SHA256,
  M2MB_TLS_AES_128_CCM_8_SHA256,
} M2MB_SSL_CIPHER_SUITE_E;

typedef enum M2MB_SSL_CERT_TYPE_E
{
  M2MB_SSL_CACERT, /* root CA certificate to authenticate the server */
  M2MB_SSL_CERT,   /* client certificate and private key in case of client authentication */
  M2MB_SSL_PSKTABLE /* PSK table in case of DTLS */
} M2MB_SSL_CERT_TYPE_E;

typedef struct M2MB_SSL_PSK_TABLE_T
{
  UINT32 psk_Size;
  /**< PSK table buffer size. */
  UINT8 *psk_Buf;
  /**< PSK table buffer. */
} M2MB_SSL_PSK_TABLE_T;

typedef struct M2MB_SSL_CERT_T
{
  UINT8 *cert_Buf;
  /**< Client certificate buffer. */
  UINT32 cert_Size;
  /**< Client certificate buffer size. */
  UINT8 *key_Buf;
  /**< Private key buffer. */
  UINT32 key_Size;
  /**< Private key buffer size. */
  UINT8 *pass_Key;
  /**< Password phrase. */
} M2MB_SSL_CERT_T;

typedef struct M2MB_SSL_CA_INFO_T
{
  UINT8 *ca_Buf;
  /**< Certificate authority list buffer. */
  UINT32 ca_Size;
  /**< Certificate authority list buffer size. */
} M2MB_SSL_CA_INFO_T;

typedef struct M2MB_SSL_CA_LIST_T
{
  UINT32 ca_Cnt;
  /**< Certificate authority list count. */
  M2MB_SSL_CA_INFO_T *ca_Info[M2MB_SSL_MAX_CA_LIST];
  /**< Certificate authority list info. */
} M2MB_SSL_CA_LIST_T;

typedef union
{
  M2MB_SSL_CERT_T cert;   /* client certificate and key */
  M2MB_SSL_CA_LIST_T ca_List;  /* CA list in case of server auth */
  M2MB_SSL_PSK_TABLE_T psk_Tbl; /* PSK table */
} M2MB_SSL_SEC_INFO_U;

typedef enum M2MB_SSL_AUTH_TYPE_E
{
  M2MB_SSL_NO_AUTH,
  M2MB_SSL_SERVER_AUTH,
  M2MB_SSL_SERVER_CLIENT_AUTH
} M2MB_SSL_AUTH_TYPE_E;

typedef struct M2MB_SSL_CONFIG_T
{
  M2MB_SSL_PROTOCOL_VERSION_E ProtVers; /* SSL TLS protocol version */
  M2MB_SSL_AUTH_TYPE_E AuthType; /* SSL TLS auth type */
  M2MB_SSL_CIPHER_SUITE_E *CipherSuites; /* ciphersuites set */
  UINT8 CipherSuitesNum;        /* note: max 8 allowed */
} M2MB_SSL_CONFIG_T;

typedef enum
{
  M2MB_SSL_NAME_CHECK           = 0,
  M2MB_SSL_NAME_SNI             = 1,
  M2MB_SSL_DTLS_SET_SOCKET_NAME = 2,
  ENUM_TO_INT( M2MB_SSL_CONF_REQUEST_E )
} M2MB_SSL_CONF_REQUEST_E;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/

/*!
  @brief
    creates SSL TLS context

  @details
    creates SSL TLS context that permits to load certificates(m2mb_ssl_cert_load) if needed and to create
    a secure socket connection(m2mb_ssl_secure_socket)

  @return
    On success, handle of type M2MB_SSL_CTXT_HANDLE is returned otherwise returns NULL

  @note
    <Notes>

  @b
    Example
  @code
    void main(void)
    {
      M2MB_SSL_CTXT_HANDLE hSSLCtx = NULL;
      hSSLCtx =  m2mb_ssl_create_ctxt();
      if(hSSLCtx == NULL)
      {
        printf("m2mb_ssl_create_ctxt() failed");
      }
      else
      {
        printf("m2mb_ssl_create_ctxt() succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_SSL_CTXT_HANDLE m2mb_ssl_create_ctxt( void );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    creates an SSL TLS configuration

  @details
    creates an SSL TLS configuration with specified configuration parameters and returns an handler
    to be used when creating secure socket connection(m2mb_ssl_secure_socket)

  @param[in] M2MB_SSL_CONFIG_T sslConfig
    specifies SSL configuration parameters(TLS version, auth type and ciphersuites)

  @param[out] INT32 *result
    returns result < 0 in case of error

  @return
    On success, handle of type M2MB_SSL_CONFIG_HANDLE is returned

  @note
    <Notes>

  @b
    Example
  @code
    void main(void)
    {
      INT32 res;
      M2MB_SSL_CONFIG_T SSLConfig;
      M2MB_SSL_AUTH_TYPE_E SSL_AUTH_MODE;
      M2MB_SSL_CIPHER_SUITE_E CipherSuite[2];
      M2MB_SSL_CONFIG_HANDLE hSSLConfig;

      SSL_AUTH_MODE = M2MB_SSL_SERVER_AUTH;
      SSLConfig.ProtVers = M2MB_SSL_PROTOCOL_TLS_1_2;
      SSLConfig.CipherSuites = CipherSuite;
      SSLConfig.CipherSuites[0] = M2MB_TLS_RSA_WITH_AES_128_CBC_SHA;
      SSLConfig.CipherSuites[1] = M2MB_TLS_RSA_WITH_AES_256_CBC_SHA;
      SSLConfig.CipherSuitesNum = 2;
      SSLConfig.AuthType = SSL_AUTH_MODE;

      hSSLConfig = m2mb_ssl_create_config( SSLConfig, &res );
      if( (hSSLConfig == NULL ) || ( res != 0 ) )
      {
        printf("m2mb_ssl_create_config() failed");
      }
      else
      {
        printf("m2mb_ssl_create_config() succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_SSL_CONFIG_HANDLE m2mb_ssl_create_config( M2MB_SSL_CONFIG_T sslConfig, INT32 *result );

/*!
  @brief
    deletes an SSL TLS configuration

  @details

  @param[in] M2MB_SSL_CONFIG_HANDLE sslConfigHndl
    specifies the config handler to be deleted

  @return
    returns 0 on success otherwise returns < 0

  @note
    <Notes>

  @b
    Example
  @code
    void main( void )
    {
      INT32 res;
      M2MB_SSL_CONFIG_T SSLConfig;
      M2MB_SSL_AUTH_TYPE_E SSL_AUTH_MODE;
      M2MB_SSL_CIPHER_SUITE_E CipherSuite[2];
      M2MB_SSL_CONFIG_HANDLE hSSLConfig;

      SSL_AUTH_MODE = M2MB_SSL_SERVER_AUTH;
      SSLConfig.ProtVers = M2MB_SSL_PROTOCOL_TLS_1_2;
      SSLConfig.CipherSuites = CipherSuite;
      SSLConfig.CipherSuites[0] = M2MB_TLS_RSA_WITH_AES_128_CBC_SHA;
      SSLConfig.CipherSuites[1] = M2MB_TLS_RSA_WITH_AES_256_CBC_SHA;
      SSLConfig.CipherSuitesNum = 2;
      SSLConfig.AuthType = SSL_AUTH_MODE;

      hSSLConfig = m2mb_ssl_create_config( SSLConfig, &res );
      if( ( hSSLConfig == NULL ) || ( res != 0 ) )
      {
        printf("m2mb_ssl_create_config() failed");
      }
      else
      {
        printf("m2mb_ssl_create_config() succeeded");
      }

      res = m2mb_ssl_delete_config( hSSLConfig);
      if(res != 0 )
      {
        printf("m2mb_ssl_delete_config() failed");
      }
      else
      {
        printf("m2mb_ssl_delete_config() succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_delete_config( M2MB_SSL_CONFIG_HANDLE sslConfigHndl );

/*!
  @brief
    creates secure socket connection

  @details
    creates secure socket connection retrieving context and configuration parameters and linking
    a secure socket connection id to socket fd

  @param[in] M2MB_SSL_CONFIG_HANDLE sslConfigHndl
    specifies the  SSL configuration handler previously created through m2mb_ssl_create_config
  @param[in] M2MB_SSL_CONTEXT_HANDLE sslCtxtHndl
    specifies the SSL context previously created through m2mb_ssl_create_ctxt
  @param[in] M2MB_SOCKET_BSD_SOCKET socket_fd
    specifies the socket fd

  @param[out] INT32 *result
    returns result < 0 in case of error

  @return
    On success, handle of type M2MB_SSL_CONNECTION_HANDLE is returned, to be used by successive
    APIs like m2mb_ssl_connect, m2mb_ssl_write, m2mb_ssl_read, m2mb_ssl_shutdown
    otherwise returns NULL
  @note
    M2MB_SSL_CONNECTION_HANDLE is valid until m2mb_ssl_shutdown() is called;

  @b
    Example
  @code
    #define SERVER_PORT_SERVER_AUTH     20518
    #define SERVER_IP IP_Nimurta_Ext // for Nimurta
    void main(void)
    {
      static struct M2MB_SOCKET_BSD_SOCKADDR_IN stSockAddr, sock_name;
      INT32 argp = 0; // 0 is for blocking, 1 is not blocking ( set timeout below )
      M2MB_SOCKET_BSD_SOCKET socket;
      static struct M2MB_SOCKET_BSD_TIMEVAL RecvTimeOut = {0,0};
      CHAR sockIPaddr[32] ="";
      INT32 sock_len = 0;
      M2MB_SSL_CTXT_HANDLE hSSLCtx = NULL;
      M2MB_SSL_CONNECTION_HANDLE hSSLConn;
      M2MB_SOCKET_BSD_SOCKET socket;
      INT32 res;
      M2MB_SSL_CONFIG_T SSLConfig;
      M2MB_SSL_AUTH_TYPE_E SSL_AUTH_MODE;
      M2MB_SSL_CIPHER_SUITE_E CipherSuite[2];
      M2MB_SSL_CONFIG_HANDLE hSSLConfig;
      SSL_AUTH_MODE = M2MB_SSL_SERVER_AUTH;
      SSLConfig.ProtVers = M2MB_SSL_PROTOCOL_TLS_1_2;
      SSLConfig.CipherSuites = CipherSuite;
      SSLConfig.CipherSuites[0] = M2MB_TLS_RSA_WITH_AES_128_CBC_SHA;
      SSLConfig.CipherSuites[1] = M2MB_TLS_RSA_WITH_AES_256_CBC_SHA;
      SSLConfig.CipherSuitesNum = 2;
      SSLConfig.AuthType = SSL_AUTH_MODE;

      socket = m2mb_socket_bsd_socket(M2MB_SOCKET_BSD_PF_INET, M2MB_SOCKET_BSD_SOCK_STREAM,
      M2MB_SOCKET_BSD_IPPROTO_TCP);
      if (M2MB_SOCKET_BSD_INVALID_SOCKET == socket)
      {
        printf( "invalid socket");
        printf("errno >> %d \r\n",m2mb_socket_errno());
        return;
      }

      if ( m2mb_socket_set_cid( socket, 2 ) != 0 )
      {
        printf( "Socket not set to ctx: %d", 2 );
        printf("errno >> %d \r\n",m2mb_socket_errno());
        return;
      }

      memset(&stSockAddr, 0, sizeof(struct M2MB_SOCKET_BSD_SOCKADDR_IN));
      memset(&sock_name,  0, sizeof(struct M2MB_SOCKET_BSD_SOCKADDR_IN));

      stSockAddr.sin_port = m2mb_socket_bsd_htons( SERVER_PORT_SERVER_AUTH );
      stSockAddr.sin_family = M2MB_SOCKET_BSD_PF_INET;
      m2mb_socket_bsd_inet_pton( M2MB_SOCKET_BSD_AF_INET, SERVER_IP, &stSockAddr.sin_addr.s_addr );
      m2mb_os_taskSleep( M2MB_OS_MS2TICKS( 1000 ) );
      res = m2mb_socket_bsd_connect(socket, (struct M2MB_SOCKET_BSD_SOCKADDR*)&stSockAddr,
              sizeof(struct M2MB_SOCKET_BSD_SOCKADDR_IN));
      if(-1 == res)
      {
          printf( "m2mb_socket_bsd_connect failed on port %d",SERVER_PORT_SERVER_AUTH );
          printf("errno >> %d \r\n",m2mb_socket_errno());
          return;
      }
      res = m2mb_socket_bsd_get_sock_name(socket ,(struct M2MB_SOCKET_BSD_SOCKADDR *)&sock_name, &sock_len );
      m2mb_socket_bsd_inet_ntop( M2MB_SOCKET_BSD_AF_INET, &sock_name.sin_addr.s_addr, ( CHAR * )&( sockIPaddr)
      , sizeof( sockIPaddr ) );
      printf( "Local  IP  : %s\r\n", sockIPaddr );
      printf( "Local  PORT: %u\r\n", m2mb_socket_bsd_ntohs(sock_name.sin_port) );

      argp=0; //not blocking argp=1, blocking argp=0
      m2mb_socket_bsd_ioctl(socket, M2MB_SOCKET_BSD_FIONBIO, &argp);
      RecvTimeOut.m_tv_sec = 30; // 30 seconds.
      m2mb_socket_bsd_set_sock_opt(socket, M2MB_SOCKET_BSD_SOL_SOCKET, M2MB_SOCKET_BSD_SO_RCVTIMEO,
      &RecvTimeOut, sizeof(RecvTimeOut));

      hSSLConfig = m2mb_ssl_create_config( SSLConfig, &res );
      if( ( hSSLConfig == NULL ) || ( res != 0 ) )
      {
        printf("m2mb_ssl_create_config() failed");
      }
      else
      {
        printf("m2mb_ssl_create_config() succeeded");
      }
      hSSLCtx =  m2mb_ssl_create_ctxt();
      if(hSSLCtx == NULL)
      {
        printf("m2mb_ssl_create_ctxt() failed");
      }
      else
      {
        printf("m2mb_ssl_create_ctxt() succeeded");
      }

      hSSLConn = m2mb_ssl_secure_socket( hSSLConfig,hSSLCtx,socket,&res );
      if(hSSLCtx == NULL)
      {
        printf("m2mb_ssl_create_ctxt() failed");
      }
      else
      {
        printf("m2mb_ssl_create_ctxt() succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_SSL_CONNECTION_HANDLE m2mb_ssl_secure_socket( M2MB_SSL_CONFIG_HANDLE sslConfigHndl, M2MB_SSL_CTXT_HANDLE sslCtxtHndl, M2MB_SOCKET_BSD_SOCKET socket_fd, INT32 *result );

/*!
  @brief
    performs SSL TLS connection

  @details
    performs SSL TLS handshake over sslConnectionHandle

  @param[in] M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle
    specifies the  sslConnectionHandle previously returned by m2mb_ssl_secure_socket

  @return
    0 on success otherwise returns < 0

  @note
    <Notes>

  @b
    Example
  @code
    void main(void)
    {
      INT32 res;
      // hSSLConn of type M2MB_SSL_CONNECTION_HANDLE returned by m2mb_ssl_secure_socket
      res = m2mb_ssl_connect( hSSLConn );
      if( res != 0 )
      {
        printf("m2mb_ssl_connect FAILED error %d  ",res);
      }
      else
      {
        printf("m2mb_ssl_connect succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_connect( M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle );

/*!
  @brief
    writes len bytes from buf into sslConnectionHandle SSL TLS connection

  @details

  @param[in] M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle
    specifies the sslConnectionHandle
  @param[in]  UINT8 *buf
    specifies the input buffer
  @param[in]  UINT32 len
    specifies the input len

  @return
    On success, number of bytes written otherwise returns < 0

  @note
    <Notes>

  @b
    Example
  @code
    #define ECHO_STRING (const CHAR*) "my appzone message\n"
    void main(void)
    {
      INT32 res;
      CHAR*  query = NULL;
      const INT32 querylen=100;
      query = (CHAR*)m2mb_os_calloc(querylen*sizeof(CHAR));
      strcpy(query,ECHO_STRING);

      // hSSLConn of type M2MB_SSL_CONNECTION_HANDLE returned by m2mb_ssl_secure_socket
      res = m2mb_ssl_write( hSSLConn, query, querylen );

      if(res < 0)
      {
        printf("m2mb_ssl_write FAILED");
      }
      else
      {
        printf("m2mb_ssl_write succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_write( M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle, void *buf, UINT32 len );

/*!
  @brief
    read len bytes from sslConnectionHandle SSL TLS connection into buf

  @details

  @param[in] M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle
    specifies the sslConnectionHandle
  @param[in]  UINT8 *buf
    specifies the input buffer
  @param[in]  UINT32 len
    specifies the input len

  @return
    On success, number of bytes read otherwise returns <= 0

  @note
    <Notes>

  @b
    Example
  @code
    void main(void)
    {
      INT32 res;
      CHAR* buf = NULL;
      const INT32 buflen=100;

      buf = (CHAR*) m2mb_os_calloc(buflen*sizeof(CHAR));
      // hSSLConn of type M2MB_SSL_CONNECTION_HANDLE returned by m2mb_ssl_secure_socket
      res = m2mb_ssl_read(hSSLConn,buf,buflen);
      if(res <= 0)
      {
        printf("nothing read");
      }
      else
      {
        printf("m2mb_ssl_read >> %s \r\n",buf);
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_read( M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle, void *buf, UINT32 len );

/*!
  @brief
    shutdown SSL TLS connection

  @details

  @param[in] M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle
    specifies the sslConnectionHandle

  @return
    0 on success

  @note
    It always returns error in case connection was fail in m2mb_ssl_connect.
    This function executes also free of sslConnectionHandle.

  @b
    Example
  @code
    void main(void)
    {
      INT32 res;
      // hSSLConn of type M2MB_SSL_CONNECTION_HANDLE returned by m2mb_ssl_secure_socket
      res = m2mb_ssl_shutdown(hSSLConn);
      if(res != 0)
      {
        printf("m2mb_ssl_shutdown FAILED");
      }
      else
      {
        printf("m2mb_ssl_shutdown succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_shutdown( M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle );

/*!
  @brief
    store certificates from buffer

  @details
    store CA list or client certificate and key or PSK table depending on first parameter(SSL cert type)

  @param[in] M2MB_SSL_CERT_TYPE_E sslCertType
    specifies the  SSL certificate type
  @param[in] M2MB_SSL_SEC_INFO_U sslSecInfo
    specifies the  SSL certificate(CA list or client certificate and key or PSK table depending
    on first parameter)
  @param[in] CHAR *name
    specifies name of the SSL certificate

  @return
    0 on success otherwise returns < 0

  @note
    client key should be in .pem or .der format

  @b
    Example
  @code
    #define CACERTFILE "CA_20170710_1732_sha2_2048.crt"
    #define SSLCERTCA "mycacert"
    void main(void)
    {
      INT32 res;
      INT32 fd = -1;
      struct M2MB_STAT st ={0x00};
      UINT8 CA_BUF[4096] = "";
      printf("ca cert file %s \r\n",CACERTFILE);

      if (0 ==m2mb_fs_stat(CACERTFILE, &st))
      {
        printf("file size: %u\r\n",  st.st_size);
      }

      fd = m2mb_fs_open(CACERTFILE,
          M2MB_O_RDONLY   //open in read only mode
      );
      if (fd == -1 )
      {
        printf("Cannot open file %s \r\n",CACERTFILE);
        exit:
      }

      printf("Reading content from file. Size: %u\r\n", st.st_size);
      res = m2mb_fs_read(fd, CA_BUF, st.st_size);

      if (res != st.st_size)
      {
        printf("Failed reading buffer into file.\r\n");
        m2mb_fs_close(fd);
      }
      else
      {
        printf("Buffer successfully received from file. %d bytes were loaded.\r\n", res);
      }

      res = m2mb_fs_close(fd);
      printf("res: %d", res);

      SSL_info.ca_List.ca_Cnt = 1;
      SSL_info.ca_List.ca_Info[0] = &ca_Info[0];
      SSL_info.ca_List.ca_Info[0]->ca_Size =  st.st_size;
      SSL_info.ca_List.ca_Info[0]->ca_Buf = CA_BUF;
      res = m2mb_ssl_cert_store( M2MB_SSL_CACERT,SSL_info,(CHAR*) SSLCERTCA );
      if(res != 0)
      {
        printf("m2mb_ssl_cert_store FAILED");
      }
      else
      {
        printf("m2mb_ssl_cert_store succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_cert_store( M2MB_SSL_CERT_TYPE_E sslCertType, M2MB_SSL_SEC_INFO_U sslSecInfo, CHAR *name );

/*!
  @brief
    load certificates in the SSL context

  @details
    load in the SSL context CA list or client cert and key or PSK table previously
    stored with m2mb_ssl_cert_store

  @param[in] M2MB_SSL_CTXT_HANDLE sslCtxtHndl
    specifies the  SSL context handle of the context to be loaded
  @param[in] M2MB_SSL_CERT_TYPE_E sslCertType
    specifies the  SSL certificate type
  @param[in] CHAR *name
    specifies name of the SSL certificate

  @return
    0 on success otherwise returns < 0

  @note
    <Notes>

  @b
    Example
  @code
    #define SSLCERTCA "mycacert"
    void main(void)
    {
      INT32 res;
      // hSSLCtx of type M2MB_SSL_CTXT_HANDLE return by m2mb_ssl_create_ctxt()
      res = m2mb_ssl_cert_load( hSSLCtx,M2MB_SSL_CACERT,(CHAR*) SSLCERTCA );
      if(res != 0)
      {
        printf("m2mb_ssl_cert_load FAILED");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

INT32 m2mb_ssl_cert_load( M2MB_SSL_CTXT_HANDLE sslCtxtHndl, M2MB_SSL_CERT_TYPE_E sslCertType, CHAR *name );

/*!
  @brief
    delete certificates

  @details
    delete CA list or client certificate and key or PSK table depending on first parameter(SSL cert type)

  @param[in] CHAR *name
    specifies name of the SSL certificate to be deleted

  @return
    0 on success otherwise returns < 0

  @note
    <Notes>

  @b
    Example
  @code
    #define SSLCERTCA "mycacert"
    void main(void)
    {
      INT32 res;
      // SSLCERTCA is name given to CA cert in m2mb_ssl_cert_store() API
      res = m2mb_ssl_cert_delete( M2MB_SSL_CACERT, (CHAR*)SSLCERTCA );
      if(res==0)
      {
        printf("m2mb_ssl_cert_delete PASS");
      }
      else
      {
        printf("m2mb_ssl_cert_delete failed with code %d",res);
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_cert_delete( M2MB_SSL_CERT_TYPE_E sslCertType, CHAR *name );

/*!
  @brief
    delete context

  @details

  @param[in] M2MB_SSL_CTXT_HANDLE sslCtxtHndl
    specifies handle of the context to be deleted

  @return

  @note
    <Notes>

  @b
    Example
  @code
    void main(void)
    {
      m2mb_ssl_delete_ctxt( hSSLCtx );//hSSLCtx of type M2MB_SSL_CTXT_HANDLE return by m2mb_ssl_create_ctxt()
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void m2mb_ssl_delete_ctxt( M2MB_SSL_CTXT_HANDLE sslCtxtHndl );

/*!
  @brief
    to configure the parameters

  @details
    this API can be used to set configurations for Domain name check, SNI and DTLS.

    for Domain name check, use this API to pass domain name.
    for SNI, use this API to pass server name.
    for DTLS, use this API to set sock address.

  @param[in] M2MB_SSL_CONFIG_HANDLE sslConfigHndl
    specifies the  SSL configuration handler previously created through m2mb_ssl_create_config
  @param[in] INT32 cmd
    Integer value of type M2MB_SSL_CONF_REQUEST_E, specifies the action.
  @param[in] void *argp
    pointer to the value to set for that action in cmd

  @return
    0 on success otherwise returns < 0

  @note
    <Notes>

  @b
    Example
  @code
    void main(void)
    {
      INT32 ret;
      struct M2MB_SOCKET_BSD_SOCKADDR *sock_addr;
      struct M2MB_SOCKET_BSD_SOCKADDR_IN stSockAddrClient;
      sock_addr = (struct M2MB_SOCKET_BSD_SOCKADDR *)&stSockAddrClient;
      //hSSLConfig is of type M2MB_SSL_CONFIG_HANDLE return by m2mb_ssl_create_config()
      ret=m2mb_ssl_config(hSSLConfig,2,sock_addr);
      if(ret != 0)
      {
        printf("m2mb_ssl_config failed");
      }
      else
      {
        printf("m2mb_ssl_config succeeded");
      }
      // hSSLConfig is of type M2MB_SSL_CONFIG_HANDLE return by m2mb_ssl_create_config()
      ret=m2mb_ssl_config(hSSLConfig,1,"www.telit.com");
      if(ret != 0)
      {
        printf("m2mb_ssl_config failed");
      }
      else
      {
        printf("m2mb_ssl_config succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_config( M2MB_SSL_CONFIG_HANDLE sslConfigHndl, INT32 cmd, void *argp );

/*!
  @brief
    to get pending bytes.

  @details

  @param[in] M2MB_SOCKET_BSD_SOCKET socket_fd
    socket descriptor
  @param[in] M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle
    SSL Connection Handle

  @return
    pending bytes value on success.
  @return
    negative value in case of fail.

  @note
    <Notes>

  @b
    Example
  @code
    void main( void )
    {
      INT32 res;
      res = m2mb_ssl_get_pending_bytes( socket_fd, sslConnectionHandle);  // socket_fd is socket fd obtained from m2mb_socket_bsd_socket() API
                                                                         // and sslConnectionHandle is connection handle obtained from m2mb_ssl_secure_socket() API
      if( res < 0)
      {
        printf("m2mb_ssl_get_pending_bytes failed ");
      }
      else
      {
        printf("m2mb_ssl_get_pending_bytes: %d ", res);
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_ssl_get_pending_bytes( M2MB_SOCKET_BSD_SOCKET socket_fd, M2MB_SSL_CONNECTION_HANDLE sslConnectionHandle );


#endif /* M2M_M2MB_SSL_H */
