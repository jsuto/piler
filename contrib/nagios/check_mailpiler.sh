#!/bin/bash

# =========================
# Defaults
# =========================
MYSQL_HOST="localhost"
MYSQL_USER=""
MYSQL_PASS=""
MYSQL_DB="piler"

MYSQL_SSL_OPTION="--ssl" # default: SSL enabled

WARN_RANGE=""
CRIT_RANGE=""

TIME_VALUE=1
TIME_UNIT="m" # default: minutes

# =========================
# Error helper
# =========================
die() {
	echo "UNKNOWN - $1"
	exit 3
}

# =========================
# Usage
# =========================
usage() {
	echo "Usage:"
	echo "  $0 -u USER -p PASS [OPTIONS]"
	echo
	echo "Required:"
	echo "  -u, --user           MySQL user (read-only)"
	echo "  -p, --password       MySQL password"
	echo
	echo "Optional:"
	echo "  -h, --host           MySQL host (default: localhost)"
	echo "  -w, --warning        Warning range (Nagios range syntax)"
	echo "  -c, --critical       Critical range (Nagios range syntax)"
	echo "  -t, --timewindow     Time window VALUE[:UNIT]"
	echo "                       UNIT = m (minutes), h (hours), d (days)"
	echo "                       Default: 1:m"
	echo "      --no-ssl         Disable SSL for MySQL connection"
	echo "      --help           Show this help"
	exit 3
}

# =========================
# Range Parser (for Sanity Check)
# =========================
parse_range() {
	local range="$1"
	local start=0
	local end="999999999"

	[[ "$range" =~ ^@ ]] && range="${range#@}"

	if [[ "$range" == *:* ]]; then
		start="${range%%:*}"
		end="${range##*:}"
	else
		start="0"
		end="$range"
	fi

	[[ -z "$start" || "$start" == "~" ]] && start="-999999999"
	[[ -z "$end" ]] && end="999999999"
	
	echo "$start $end"
}

# =========================
# Nagios range check
# returns 0 = OK, 1 = ALERT
# Sets RANGE_SIDE to "low" or "high"
# =========================
check_range() {
	local value="$1"
	local range="$2"
	RANGE_SIDE=""

	[[ -z "$range" || "$range" == "0" ]] && return 0

	local invert=0
	local start=""
	local end=""

	[[ "$range" =~ ^@ ]] && invert=1 && range="${range#@}"

	if [[ "$range" == *:* ]]; then
		start="${range%%:*}"
		end="${range##*:}"
	else
		start="0"
		end="$range"
	fi

	[[ -z "$start" ]] && start="0"
	[[ "$start" == "~" ]] && start="-inf"
	[[ -z "$end" ]] && end="+inf"

	local in_range=1

	if [[ "$start" != "-inf" ]] && (( $(awk "BEGIN{print ($value < $start)}") )); then
		in_range=0
		RANGE_SIDE="low"
	fi

	if [[ "$end" != "+inf" ]] && (( $(awk "BEGIN{print ($value > $end)}") )); then
		in_range=0
		RANGE_SIDE="high"
	fi

	if [[ $invert -eq 1 ]]; then
		if [[ $in_range -eq 1 ]]; then
			RANGE_SIDE="inside_range"
			return 1
		else
			return 0
		fi
	else
		[[ $in_range -eq 1 ]] && return 0 || return 1
	fi
}

# =========================
# getopt parsing
# =========================
PARSED=$(getopt \
-o h:u:p:w:c:t: \
--long host:,user:,password:,warning:,critical:,timewindow:,no-ssl,help \
-- "$@")

[[ $? -ne 0 ]] && die "invalid command line arguments"

eval set -- "$PARSED"

while true; do
	case "$1" in
		-h|--host)
			MYSQL_HOST="$2"; shift 2;;
		-u|--user)
			MYSQL_USER="$2"; shift 2;;
		-p|--password)
			MYSQL_PASS="$2"; shift 2;;
		-w|--warning)
			WARN_RANGE="$2"; shift 2;;
		-c|--critical)
			CRIT_RANGE="$2"; shift 2;;
		-t|--timewindow)
			IFS=: read TIME_VALUE TIME_UNIT <<< "$2"
			[[ -z "$TIME_UNIT" ]] && TIME_UNIT="m"
			shift 2;;
		--no-ssl)
			MYSQL_SSL_OPTION="--skip-ssl"; shift;;
		--help)
			usage;;
		--)
			shift; break;;
		*)
			die "internal getopt error";;
	esac
done

# =========================
# Validation
# =========================
[[ -z "$MYSQL_USER" ]] && die "MySQL user missing"
[[ -z "$MYSQL_PASS" ]] && die "MySQL password missing"
[[ ! "$TIME_VALUE" =~ ^[0-9]+$ ]] && die "time value must be numeric"
[[ "$TIME_VALUE" -le 0 ]] && die "time window value must be greater than 0"

# Sanity Check for Ranges
if [[ -n "$WARN_RANGE" && -n "$CRIT_RANGE" ]]; then
	read w_start w_end < <(parse_range "$WARN_RANGE")
	read c_start c_end < <(parse_range "$CRIT_RANGE")

	if (( $(awk "BEGIN{print ($c_end < $w_end)}") )); then
		die "Configuration error: Critical high limit ($c_end) cannot be lower than Warning high limit ($w_end)"
	fi

	if (( $(awk "BEGIN{print ($c_start > $w_start)}") )); then
		die "Configuration error: Critical low limit ($c_start) cannot be higher than Warning low limit ($w_start)"
	fi
fi

case "$TIME_UNIT" in
	m)
		SQL_INTERVAL="MINUTE"
		TOTAL_MINUTES=$TIME_VALUE
		;;
	h)
		SQL_INTERVAL="HOUR"
		TOTAL_MINUTES=$(( TIME_VALUE * 60 ))
		;;
	d)
		SQL_INTERVAL="DAY"
		TOTAL_MINUTES=$(( TIME_VALUE * 1440 ))
		;;
	*)
		die "invalid time unit '$TIME_UNIT' (use m|h|d)"
		;;
esac

# =========================
# SQL Query
# =========================
SQL="SELECT COUNT(*) FROM metadata WHERE arrived > UNIX_TIMESTAMP(NOW() - INTERVAL $TIME_VALUE $SQL_INTERVAL);"
SQL_OUTPUT=$(mysql -N -s $MYSQL_SSL_OPTION -h "$MYSQL_HOST" -u "$MYSQL_USER" -p"$MYSQL_PASS" "$MYSQL_DB" -e "$SQL" 2>&1)

[[ $? -ne 0 ]] && die "database query failed on host=$MYSQL_HOST: $SQL_OUTPUT"

COUNT="$SQL_OUTPUT"
[[ ! "$COUNT" =~ ^[0-9]+$ ]] && die "unexpected query result: '$COUNT'"

# =========================
# Calculate mails/min
# =========================
MAILS_PER_MIN=$(awk -v c="$COUNT" -v m="$TOTAL_MINUTES" 'BEGIN { if (m>0) printf "%.2f", c/m; else print "0.00" }')

# =========================
# Status evaluation
# =========================
STATUS="OK"
EXITCODE=0
REASON=""

check_range "$COUNT" "$CRIT_RANGE"
if [[ $? -ne 0 ]]; then
	STATUS="CRITICAL"
	EXITCODE=2
	[[ "$RANGE_SIDE" == "low" ]] && REASON=" (too few mails)"
	[[ "$RANGE_SIDE" == "high" ]] && REASON=" (too many mails)"
else
	check_range "$COUNT" "$WARN_RANGE"
	if [[ $? -ne 0 ]]; then
		STATUS="WARNING"
		EXITCODE=1
		[[ "$RANGE_SIDE" == "low" ]] && REASON=" (too few mails)"
		[[ "$RANGE_SIDE" == "high" ]] && REASON=" (too many mails)"
	fi
fi

# =========================
# Output + Perfdata
# =========================
echo "$STATUS - $COUNT mails in last ${TIME_VALUE}${TIME_UNIT} (${MAILS_PER_MIN} mails/min)${REASON} | mails_per_min=${MAILS_PER_MIN};;;0; mail_count=${COUNT};${WARN_RANGE};${CRIT_RANGE};0;"

exit $EXITCODE
