#include <VException>
#include <VDebugNew>

#ifdef WIN32

int evalException(const int exception, const EXCEPTION_POINTERS* pinfo)
{
  char *msg;
  // Pass on most exceptions
  switch(exception)
  {
    case EXCEPTION_ACCESS_VIOLATION:         msg = "EXCEPTION_ACCESS_VIOLATION"; break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:    msg = "EXCEPTION_DATATYPE_MISALIGNMENT"; break;
    case EXCEPTION_BREAKPOINT:               msg = "EXCEPTION_BREAKPOINT"; break;
    case EXCEPTION_SINGLE_STEP:              msg = "EXCEPTION_SINGLE_STEP"; break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    msg = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"; break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:     msg = "EXCEPTION_FLT_DENORMAL_OPERAND"; break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       msg = "EXCEPTION_FLT_DIVIDE_BY_ZERO"; break;
    case EXCEPTION_FLT_INEXACT_RESULT:       msg = "EXCEPTION_FLT_INEXACT_RESULT"; break;
    case EXCEPTION_FLT_INVALID_OPERATION:    msg = "EXCEPTION_FLT_INVALID_OPERATION"; break;
    case EXCEPTION_FLT_OVERFLOW:             msg = "EXCEPTION_FLT_OVERFLOW"; break;
    case EXCEPTION_FLT_STACK_CHECK:          msg = "EXCEPTION_FLT_STACK_CHECK"; break;
    case EXCEPTION_FLT_UNDERFLOW:            msg = "EXCEPTION_FLT_UNDERFLOW"; break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:       msg = "EXCEPTION_INT_DIVIDE_BY_ZERO"; break;
    case EXCEPTION_INT_OVERFLOW:             msg = "EXCEPTION_INT_OVERFLOW"; break;
    case EXCEPTION_PRIV_INSTRUCTION:         msg = "EXCEPTION_PRIV_INSTRUCTION"; break;
    case EXCEPTION_IN_PAGE_ERROR:            msg = "EXCEPTION_IN_PAGE_ERROR"; break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:      msg = "EXCEPTION_ILLEGAL_INSTRUCTION"; break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: msg = "EXCEPTION_NONCONTINUABLE_EXCEPTION"; break;
    case EXCEPTION_STACK_OVERFLOW:           msg = "EXCEPTION_STACK_OVERFLOW"; break;
    case EXCEPTION_INVALID_DISPOSITION:      msg = "EXCEPTION_INVALID_DISPOSITION"; break;
    case EXCEPTION_GUARD_PAGE:               msg = "EXCEPTION_GUARD_PAGE"; break;
    case EXCEPTION_INVALID_HANDLE:           msg = "EXCEPTION_INVALID_HANDLE"; break;
    default:                                 msg = "UNKNOWN EXCEPTION";
  }
  LOG_ERROR("******************************************************");
  LOG_ERROR("%s 0x%x", msg, exception);
  LOG_ERROR("Code: 0x%x", pinfo->ExceptionRecord->ExceptionCode);
  LOG_ERROR("Flags: %s", (pinfo->ExceptionRecord->ExceptionFlags & EXCEPTION_NONCONTINUABLE) ? "Not-Continuable" : "Continuable");
  LOG_ERROR("Exception Record: 0x%p", pinfo->ExceptionRecord->ExceptionRecord);
  LOG_ERROR("Address: 0x%p", pinfo->ExceptionRecord->ExceptionAddress);
  LOG_ERROR("Number of Parameters: %d", pinfo->ExceptionRecord->NumberParameters);
  if(exception == (int)EXCEPTION_ACCESS_VIOLATION)
  {
    LOG_ERROR("------------------------------------------------------");
    if(pinfo->ExceptionRecord->ExceptionInformation[0] == 0)
    {
      LOG_ERROR("Attempt to read inaccessible data 0x%x", pinfo->ExceptionRecord->ExceptionInformation[1]);
    }
    else if(pinfo->ExceptionRecord->ExceptionInformation[0] == 1)
    {
      LOG_ERROR("Attempt to write inaccessible data at 0x%x", pinfo->ExceptionRecord->ExceptionInformation[1]);
    }
  }
  LOG_ERROR("******************************************************");
  return EXCEPTION_EXECUTE_HANDLER;
}

#endif // WIN32

#ifdef GTEST
#include <gtest/gtest.h>

int divideByZero()
{
  int i, j, k;
  int debug = 0;
  TRY
  {
    i = 5;
    j = 0;
    k = i / j; // divide by zero occurred.
    printf("%d / %d = %d\n", i, j, k);
    debug = 1;
  } EXCEPT
  {
    LOG_FATAL("divide by zero occurred");
    debug = 2;
  }
  return debug;
}

int accessViolation()
{
  char* p = NULL;
  int debug = 0;
  TRY
  {
    *p = 'a';
    debug = 1;
  } EXCEPT
  {
    LOG_FATAL("access violcation occurred");
    debug = 2;
  }
  return debug;
}

TEST( Exception, exceptionTest )
{
  int debug;

  debug = divideByZero();
  EXPECT_EQ( debug, 2 );

  debug = accessViolation();
  EXPECT_EQ( debug, 2 );
}

#endif // GTEST
