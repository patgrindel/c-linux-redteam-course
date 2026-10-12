/* checks_cron.h -- scheduled jobs from /etc/crontab and /etc/cron.*. */
#ifndef CHECKS_CRON_H
#define CHECKS_CRON_H

#include "report.h"

void check_cron(report_t *rep);

#endif /* CHECKS_CRON_H */
