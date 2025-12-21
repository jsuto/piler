Disclosure policy

If you find a security issue, please contact the project owner at security@mailpiler.com
with the details (ie. piler version, details of the setup, how to exploit the
vulnerability, etc).

Please provide 30 days for verifying the vulnerability, fixing the issue, and
notifying the piler users.

Security configurations

 - Use https for the UI
 - Use STARTTLS for piler-smtp
 - Reset the default passwords for admin and auditor
 - Use the smtp acl feature to restrict SMTP access to the archive, see https://mailpiler.com/blog/smtp-acl-list/index.html
