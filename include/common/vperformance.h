// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_PERFORMANCE_H__
#define __V_PERFORMANCE_H__

#include <VLog>
#include <VTick> // for VDuration // gilgil temp 2013.03.01

// ----------------------------------------------------------------------------
// VPerformanceVerbose
// ----------------------------------------------------------------------------
class VPerformanceVerbose
{
public:
  int       begMilestone;
  int       endMilestone;
  VDuration duration;
};

// ----------------------------------------------------------------------------
// VPerformanceVerboseList
// ----------------------------------------------------------------------------
class VPerformanceVerboseList : public QList<VPerformanceVerbose>
{
};

// ----------------------------------------------------------------------------
// VPerformanceReportKey
// ----------------------------------------------------------------------------
class VPerformanceReportKey
{
public:
  int begMilestone;
  int endMileseone;
  bool operator < (const VPerformanceReportKey& rhs) const;
};

// ----------------------------------------------------------------------------
// VPerformanceReportData
// ----------------------------------------------------------------------------
class VPerformanceReportData
{
public:
  int       count;
  VDuration totalDuration;
};

// ----------------------------------------------------------------------------
// VPerformanceReportMap
// ----------------------------------------------------------------------------
class VPerformanceReportMap : public QMap<VPerformanceReportKey, VPerformanceReportData>
{
};

// ----------------------------------------------------------------------------
// VPerformance
// ----------------------------------------------------------------------------
class VPerformance
{
public:
  VPerformance();
  virtual ~VPerformance();

public:
  bool verbose;

protected:
  VLog* m_log;

public:
  VLog* log()            { return m_log; }
  void setLog(VLog *log) { m_log = log;  }

public:
  VPerformanceVerboseList  verboseList;
  VPerformanceReportMap    reportMap;

  void report();

protected:
  int   lastMilestone;
  VTick lastTick;

public:
  void clear();
  void check(int milestone);
  void check(int milestone, VTick now);
};

#endif // __V_PERFORMANCE_H__
