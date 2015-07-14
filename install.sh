#! /bin/bash

#Run me as root
#enable the service after installation

cp ./cd00rd.service /etc/systemd/system
cp ./cd00rd /usr/bin/
cp ./lock /usr/bin/

id -u cd00r
if [ $? -eq 1 ]; then
	useradd -M -s /usr/bin/nologin cd00r
fi

chown root:root /usr/bin/cd00rd
chown root:root /usr/bin/lock

mkdir /var/log/cd00rd
chown root:cd00r /var/log/cd00rd
chmod g+w /var/log/cd00rd

chmod u+s /usr/bin/lock
setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/cd00rd
