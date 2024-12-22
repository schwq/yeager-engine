#include "Time.h"
#include "LogEngine.h"
using namespace Yeager;

std::vector<IntervalElapsedTime> IntervalElapsedTimeManager::sIntervals;
std::vector<IntervalElapsedTime> IntervalElapsedTimeManager::sStaticIntervals;

String TimePointType::CurrentTimeFormatToString()
{
  std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  return String(std::ctime(&current_time));
}

String DateType::MonthNumberToString(int month)
{
  switch (month) {
    case 0:
      return "January";
    case 1:
      return "February";
    case 2:
      return "March";
    case 3:
      return "April";
    case 4:
      return "May";
    case 5:
      return "June";
    case 6:
      return "July";
    case 7:
      return "August";
    case 8:
      return "September";
    case 9:
      return "October";
    case 10:
      return "November";
    case 11:
      return "December";
    default:
      return "INVALID_MONTH_NUMBER";
  }
}

String DateType::WeekDayNumberToString(int weekday)
{
  switch (weekday) {
    case 0:
      return "Sunday";
    case 1:
      return "Monday";
    case 2:
      return "Tuesday";
    case 3:
      return "Wednesday";
    case 4:
      return "Thursday";
    case 5:
      return "Friday";
    case 6:
      return "Saturday";
    default:
      return "INVALID_WEEKDAY_NUMBER";
  }
}

Yeager::TimePointType TimePointType::CurrentTimeToTimeType()
{
  TimePointType time;

  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  time_t tt = std::chrono::system_clock::to_time_t(now);
  tm utc_tm = *gmtime(&tt);
  tm local_tm = *localtime(&tt);

  time.Date.Year = local_tm.tm_year + 1900;
  time.Date.Month = local_tm.tm_mon;
  time.Date.Day = local_tm.tm_mday;
  time.Date.WeekDay = local_tm.tm_wday;

  time.Time.Hours = local_tm.tm_hour;
  time.Time.Minutes = local_tm.tm_min;
  time.Time.Seconds = local_tm.tm_sec;

  return time;
}
#ifdef YEAGER_SYSTEM_WINDOWS_x64
TimePointType Yeager::WindowsFILETIMEToTimePoint(const FILETIME& time)
{
  SYSTEMTIME systime = {0};
  FileTimeToSystemTime(&time, &systime);
  TimePointType time_point;
  time_point.Date.Year = systime.wYear;
  time_point.Date.Month = systime.wMonth - 1;
  time_point.Date.WeekDay = systime.wDayOfWeek;
  time_point.Date.Day = systime.wDay;
  time_point.Time.Hours = systime.wHour;
  time_point.Time.Minutes = systime.wMinute;
  time_point.Time.Seconds = systime.wSecond;
  time_point.Time.Millis = systime.wMilliseconds;
  return time_point;
}
#endif

String TimePointType::CurrentTimeFormatToFileFormat()
{
  TimePointType time = TimePointType::CurrentTimeToTimeType();
  String str = std::to_string(time.Date.Year) + "_" + DateType::MonthNumberToString(time.Date.Month) + "_" +
               std::to_string(time.Date.Day) + "_" + std::to_string(time.Time.Hours) + "_" +
               std::to_string(time.Time.Minutes) + "_" + std::to_string(time.Time.Seconds);
  return str;
}

String TimePointType::FormatTimeToString(const TimePointType& time)
{
  return String(FormatFrontZeroStr(time.Date.Day) + "/" + FormatFrontZeroStr(time.Date.Month) + "/" +
                FormatFrontZeroStr(time.Date.Year) + " " + FormatFrontZeroStr(time.Time.Hours) + ":" +
                FormatFrontZeroStr(time.Time.Minutes) + ":" + FormatFrontZeroStr(time.Time.Seconds));
}

String Yeager::FormatFrontZeroStr(Uint number)
{
  return (number < 10) ? String("0" + std::to_string(number)) : std::to_string(number);
}

IntervalElapsedTime IntervalElapsedTimeManager::StartTimeInterval(const String& process)
{
  if (IntervalElapsedTimeManager::sIntervals.size() >= YEAGER_MAX_PROCESS_TIME_MAN) {
    Yeager::Log(WARNING, "The max size of processes in the interval time manager was reached! Max: {}",
                YEAGER_MAX_PROCESS_TIME_MAN);
    return IntervalElapsedTime();
  }

  IntervalElapsedTime time;
  time.mProcessName = process;
  time.mStart = std::chrono::steady_clock::now();
  IntervalElapsedTimeManager::sIntervals.push_back(time);
  return time;
}

IntervalElapsedTime IntervalElapsedTimeManager::EndTimeInterval(const String& process)
{
  using tm = IntervalElapsedTimeManager;
  auto it = std::find_if(tm::sIntervals.begin(), tm::sIntervals.end(),
                         [&](IntervalElapsedTime& time) { return time.mProcessName == process; });

  if (it != tm::sIntervals.end()) {
    it->mEnd = std::chrono::steady_clock::now();
    it->mDiff = std::chrono::duration_cast<std::chrono::microseconds>(it->mEnd - it->mStart);
    return (*it);
  }

  Yeager::Log(ERROR, "Cannot find process {} in the intervals of time!", process);
  return IntervalElapsedTime();
}

IntervalElapsedTime IntervalElapsedTimeManager::StaticStartTimeInterval(const String& process,
                                                                        std::thread::id thread_id)
{
  if (IntervalElapsedTimeManager::sStaticIntervals.size() >= YEAGER_MAX_PROCESS_TIME_MAN) {
    Yeager::Log(WARNING, "The max size of processes in the interval time manager was reached! Max: {}",
                YEAGER_MAX_PROCESS_TIME_MAN);
    return IntervalElapsedTime();
  }

  IntervalElapsedTime time;
  time.mProcessName = process;
  time.mStart = std::chrono::steady_clock::now();
  time.mThreadId = thread_id;
  IntervalElapsedTimeManager::sStaticIntervals.push_back(time);
  return time;
}

IntervalElapsedTime IntervalElapsedTimeManager::StaticEndTimeInterval(const String& process, std::thread::id thread_id)
{
  using tm = IntervalElapsedTimeManager;
  auto it = std::find_if(tm::sStaticIntervals.begin(), tm::sStaticIntervals.end(), [&](IntervalElapsedTime& time) {
    return time.mProcessName == process && time.mThreadId == thread_id;
  });

  if (it != tm::sStaticIntervals.end()) {
    it->mEnd = std::chrono::steady_clock::now();
    it->mDiff = std::chrono::duration_cast<std::chrono::microseconds>(it->mEnd - it->mStart);
    it->mEnded = true;
    return (*it);
  }
}

IntervalElapsedTime IntervalElapsedTimeManager::StaticStartTimeInterval(const String& process)
{
  if (IntervalElapsedTimeManager::sStaticIntervals.size() >= YEAGER_MAX_PROCESS_TIME_MAN) {
    Yeager::Log(WARNING, "The max size of processes in the interval time manager was reached! Max: {}",
                YEAGER_MAX_PROCESS_TIME_MAN);
    return IntervalElapsedTime();
  }

  IntervalElapsedTime time;
  time.mProcessName = process;
  time.mStart = std::chrono::steady_clock::now();
  IntervalElapsedTimeManager::sStaticIntervals.push_back(time);
  return time;
}

IntervalElapsedTime IntervalElapsedTimeManager::StaticEndTimeInterval(const String& process)
{
  using tm = IntervalElapsedTimeManager;
  auto it = std::find_if(tm::sStaticIntervals.begin(), tm::sStaticIntervals.end(),
                         [&](IntervalElapsedTime& time) { return time.mProcessName == process; });

  if (it != tm::sStaticIntervals.end()) {
    it->mEnd = std::chrono::steady_clock::now();
    it->mDiff = std::chrono::duration_cast<std::chrono::microseconds>(it->mEnd - it->mStart);
    it->mEnded = true;
    return (*it);
  }

  Yeager::Log(ERROR, "Cannot find process {} in the intervals of time!", process);
  return IntervalElapsedTime();
}

std::vector<IntervalElapsedTime>* IntervalElapsedTimeManager::GetIntervals()
{
  return &IntervalElapsedTimeManager::sIntervals;
}

std::vector<IntervalElapsedTime>* IntervalElapsedTimeManager::GetStaticIntervals()
{
  return &IntervalElapsedTimeManager::sStaticIntervals;
}

void IntervalElapsedTimeManager::ResetIntervals()
{
  IntervalElapsedTimeManager::sIntervals.clear();
}