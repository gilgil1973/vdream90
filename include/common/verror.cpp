#include <VError>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VError
// ----------------------------------------------------------------------------
QString VError::className()
{
  return getClassName(ti->name());
}

VError::VError(const char* msg, const int code)
{
  this->ti = (std::type_info*)&typeid(*this);
  setErrorMsg(msg);
  setErrorCode(code);
}

VError& VError::operator = (const VError& error)
{
  if (this->code == VERR_OK)
  {
    this->ti = error.ti;
    memcpy(this->msg, error.msg, DEFAULT_ERR_BUF_SIZE);
    this->code = error.code;
  }
  return *this;
}

void VError::clear()
{
  this->ti = (std::type_info*)&typeid(VError);
  memset(this->msg, 0, DEFAULT_ERR_BUF_SIZE);
  this->code = VERR_OK;
}

void VError::setErrorMsg(const char* msg)
{
  if (msg != NULL)
    strcpy_s(this->msg, DEFAULT_ERR_BUF_SIZE, msg);
  else
    memset((void*)this->msg, 0, DEFAULT_ERR_BUF_SIZE);
}

void VError::setErrorCode(int code)
{
  this->code = code;
}
