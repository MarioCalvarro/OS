#!/bin/bash
filename='/etc/passwd'
i=0

while IFS=':' read login_name optional_enc_passwd uid gid user_name user_home user_shell
do
    if [ "$(dirname "$user_home")" == "/home" ]; then
        echo -e '[Entry '$i']'
        echo -e '\tlogin='$login_name'\n\tenc_pass='$optional_enc_passwd'\n\tuid='$uid'\n\tgid='$gid'\n\tuser_name='$user_name'\n\thome='$user_home'\n\tshell='$user_shell'\n'

        i=$(($i + 1))
    fi
done < $filename

grep '/home' /etc/passwd | cut -d: -f6
