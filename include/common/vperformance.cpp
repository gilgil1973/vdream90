#include <VPerformance>

// ----------------------------------------------------------------------------
// VPerformanceReportKey
// ----------------------------------------------------------------------------
bool VPerformanceReportKey::operator < (const VPerformanceReportKey& rhs) const
{
  if (this->begMilestone < rhs.begMilestone) return true;
  if (this->begMilestone > rhs.begMilestone) return false;
  if (this->endMileseone < rhs.endMileseone) return true;
  return false;
}

// ----------------------------------------------------------------------------
// VPerformance
// ----------------------------------------------------------------------------
VPerformance::VPerformance()
{
  verbose = false;
  m_log = VLog::getLog();
  clear();
}

VPerformance::~VPerformance()
{
  clear();
}

void VPerformance::report()
{
  if (verbose)
  {
    m_log->trace("beg\tend\tduration");
    foreach (const VPerformanceVerbose& verbose, verboseList)
    {
      if (verbose.begMilestone == 0) continue;
      m_log->trace("%d\t%d\t%llu", verbose.begMilestone, verbose.endMilestone, verbose.duration);
    }
  } else
  {
    m_log->trace("beg\tend\tcount\tduration");
    foreach (const VPerformanceReportKey& key, reportMap.keys())
    {
      if (key.begMilestone == 0) continue;
      VPerformanceReportData& data = reportMap[key];
      m_log->trace("%d\t%d\t%d\t%llu", key.begMilestone, key.endMileseone, data.count, data.totalDuration);
    }
  }
}

void VPerformance::clear()
{
  verboseList.clear();
  this->reportMap.clear();
  lastMilestone = 0;
  lastTick      = tick();
}

void VPerformance::check(int milestone)
{
  check(milestone, tick());
}

void VPerformance::check(int milestone, VTick now)
{
  if (verbose)
  {
    VPerformanceVerbose verbose;
    verbose.begMilestone = lastMilestone;
    verbose.endMilestone = milestone;
    verbose.duration     = now - lastTick;
    verboseList.push_back(verbose);
  } else
  {
    VPerformanceReportKey key;
    key.begMilestone = lastMilestone;
    key.endMileseone = milestone;

    VPerformanceReportData &data = reportMap[key];
    data.count++;
    data.totalDuration += now - lastTick;
  }
  lastMilestone = milestone;
  lastTick      = now;
}

