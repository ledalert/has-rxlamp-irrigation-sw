/*! \file time.h
    \brief Time keeping and converting include file
 
*/
#ifndef _TIME_H_

/*! Broken down time */
struct tm {
    int tm_sec;         /*!< seconds */
    int tm_min;         /*!< minutes */
    int tm_hour;        /*!< hours */
    int tm_mday;        /*!< day of the month */
    int tm_mon;         /*!< month */
    int tm_year;        /*!< year */
    int tm_wday;        /*!< day of the week */
    int tm_yday;        /*!< day in the year */
    int tm_isdst;       /*!< daylight saving time */
};

/*! Timestamp */
struct sw_timer_system_time {
	int epoch;			/*!< Epoch time */
	int ms;				/*!< Remaining milliseconds */
};

int time_tm_from_epoch(struct tm* result, int epoch);


#define _TIME_H_
#endif