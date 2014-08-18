# run config differ hourly
#1 * * * * rancid /usr/bin/rancid-run
# clean out config differ logs
#50 23 * * * rancid find /var/log/rancid -type f -mtime +2 -exec rm {} \;
