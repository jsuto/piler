## Setup

- Needs a user with read privileges

```
GRANT SELECT ON piler.metadata TO 'piler_ro'@'localhost';
```

- Example

```
./check_piler_mailrate.sh -h localhost -u piler_ro -p yourpassword -w 10:200 -c 5:500 -t 1:h --no-ssl
```

- Command definition for Nagios

```
define command {
	command_name	check_mailpiler
	command_line	/usr/local/nagios/libexec/check_mailpiler.sh -h $HOSTADDRESS$ -u $ARG1$ -p $ARG2$ -w $ARG3$ -c $ARG4$ -t $ARG5$ --no-ssl
}
```

- Service definition

```
define service {
	use						generic-service
	host_name				piler.example.com
	service_description		MailPiler - Incoming mails
	check_command			check_mailpiler!sqluser!sqlpass!10:200!5:500!1:h
}
```
