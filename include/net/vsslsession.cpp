#include <VSslSession>

// ----------------------------------------------------------------------------
// VSslSession
// ----------------------------------------------------------------------------
VSslSession::VSslSession(void* owner) : VNetSession(owner)
{
  con        = NULL;
  sbio       = NULL;
  handle     = INVALID_SOCKET;
  ctx        = NULL;
  tcpSession = NULL;
}

VSslSession::~VSslSession()
{
  close();
  if (con != NULL)
  {
    SSL_free(con);
    con = NULL;
  }

  if (sbio != NULL)
  {
    // BIO_free(sbio); // do not call BIO_free // by gilgil 2014.02.28
    sbio = NULL;
  }
}

void apps_ssl_info_callback(const SSL* s, int where, int ret)
{
  const char *str;
  int w;
  w=where& ~SSL_ST_MASK;
  if (w & SSL_ST_CONNECT) str="SSL_connect";
  else if (w & SSL_ST_ACCEPT) str="SSL_accept";
  else str="undefined";
  if (where & SSL_CB_LOOP)
  {
    LOG_DEBUG("%s:%s",str,SSL_state_string_long(s));
  } else
  if (where & SSL_CB_ALERT)
  {
    str=(where & SSL_CB_READ)?"read":"write";
    LOG_DEBUG("SSL3 alert %s:%s:%s",
      str,
      SSL_alert_type_string_long(ret),
      SSL_alert_desc_string_long(ret));
  } else
  if (where & SSL_CB_EXIT)
  {
    if (ret == 0)
    {
      LOG_DEBUG("%s:failed in %s", str,SSL_state_string_long(s));
    } else
    if (ret < 0)
    {
      LOG_DEBUG("%s:error in %s", str,SSL_state_string_long(s));
    }
  }
}

bool VSslSession::doOpen()
{
  // --------------------------------------------------------------------------
  // check sock(must be set in other place_
  // --------------------------------------------------------------------------
  if (handle == INVALID_SOCKET)
  {
    SET_ERROR(VSslError, "INVALID_SOCKET", VERR_HANDLE_IS_ZERO);
    return false;
  }

  // --------------------------------------------------------------------------
  // check ctx(must be set in other place
  // --------------------------------------------------------------------------
  if (ctx == NULL)
  {
    SET_ERROR(VSslError, "ctx is NULL", VERR_HANDLE_IS_ZERO);
    return false;
  }

  // --------------------------------------------------------------------------
  // set con
  // --------------------------------------------------------------------------
  if (con != NULL)
  {
    SSL_free(con);
  }
  con = SSL_new(ctx);
  SSL_set_ex_data(con, 0, this);

  // --------------------------------------------------------------------------
  // set sbio
  // --------------------------------------------------------------------------
  if (sbio != NULL)
  {
    // BIO_free(sbio); // do not call BIO_free // by gilgil 2014.02.28
  }
  sbio = BIO_new_socket((int)handle, BIO_NOCLOSE);
  SSL_set_bio(con, sbio, sbio);

  // --------------------------------------------------------------------------
  // debug
  // --------------------------------------------------------------------------
  // ----- gilgil temp 2011.11.03 -----
  /*
  {
    Log* log = getLog();
    if (log != NULL && log->debugEnabled)
      SSL_CTX_set_info_callback(ctx, apps_ssl_info_callback);
  }
  */
  // ----------------------------------

  return true;
}

// ----- gilgil temp 2014.02.25 -----
/*
#ifdef linux
#include <sys/socket.h> // for shutdown // gilgil temp 2011.10.31
#endif // linux
#ifndef SD_BOTH
#define SD_BOTH 0x02
*/
// ----------------------------------
bool VSslSession::doClose()
{
  ctx = NULL;
  if (con != NULL)
  {
    SSL_shutdown(con);
    SSL_clear(con);
  }
  if (sbio != NULL)
  {
    BIO_flush(sbio);
    // BIO_shutdown_wr(sbio); // gilgil temp 2014.02.28
  }

// ----- gilgil temp 2014.02.28 -----
/*
#ifdef WIN32
  shutdown(handle, SD_BOTH);
#endif // WIN32
*/
// ----------------------------------
  handle = INVALID_SOCKET;
  return true;
}

int VSslSession::doRead(char* buf, int size)
{
  int res = SSL_read(con, buf, size);
  // sleep(1); // gilgil temp 2014.02.28
  if (res < 0)
  {
    SET_DEBUG_ERROR(VSslError, qformat("SSL_read return %d", res), SSL_get_error(con, res));
    return VERR_FAIL;
  }
  if (res == 0) // SSL_ERROR_ZERO_RETURN?
  {
    SET_DEBUG_ERROR(VSslError, "SSL_read return zero", SSL_get_error(con, res));
    return VERR_FAIL;
  }
  return res;
}

int VSslSession::doWrite(char* buf, int size)
{
  int res = SSL_write(con, buf, size);
  if (res < 0)
  {
    SET_ERROR(VSslError, "SSL_write return zero", SSL_get_error(con, res));
    return VERR_FAIL;
  }
  return res;
}

Ip VSslSession::getLocalIP()
{
  LOG_ASSERT(tcpSession != NULL);
  return tcpSession->getLocalIP();
}

Ip VSslSession::getRemoteIP()
{
  LOG_ASSERT(tcpSession != NULL);
  return tcpSession->getRemoteIP();
}

int VSslSession::getLocalPort()
{
  LOG_ASSERT(tcpSession != NULL);
  return tcpSession->getLocalPort();
}

int VSslSession::getRemotePort()
{
  LOG_ASSERT(tcpSession != NULL);
  return tcpSession->getRemotePort();
}

bool VSslSession::operator == (const VSslSession& rhs) const
{
  return tcpSession == rhs.tcpSession;
}
