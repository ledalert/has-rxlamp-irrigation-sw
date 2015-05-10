
struct tm datetime = {
	.tm_year = 2015 - 1900,
	.tm_month = 0,
	.tm_mday = 1,
	.tm_hour = 0,
	.tm_min = 0,
	.tm_sec = 0,
	.tm_wday = 4,	//2015-01-01 is a thursday
	.tm_yday = 0,
	.tm_isdst = 0,
};

void time_increment(struct tm* time) {
	if (++time->tm_sec == 60) {
		time->tm_sec = 0;
		if (++time->tm_min == 60) {
			time->tm_min = 0;
			if (++time->tm_hour == 24) {
				time->tm_hour = 0;
				if (++time->tm_day == days_of_month[time->tm_month]) {
					time->tm_day = 0;
					if (++time->tm_month == 12) {
						time->tm_month = 0;
						time->tm_yday = 0;
						time->tm_year++;
					}
				}
				time->tm_yday++;
				if (++time->wday == 7) {
					time->wday = 0;
				}
			}
		}
	}
}