#include "error.h"

void yaeger::Error::LogError()
{
  VLOG_F(ERROR, "Error: %s, Code: %u", m_error, m_code);
}