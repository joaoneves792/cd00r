#! /bin/sh
# Joao Neves 13-12-2014

GREEN="\e[0;32m"
NORMAL="\e[1;37m"
echo -e "${GREEN}"

echo "             _____  ___     "
echo "     _______/ / _ \/ _ \____"
echo "    / __/ _  / // / // / __/"
echo "    \__/\_,_/\___/\___/_/   "

echo -e "${NORMAL}"
function check_port {
	port=$1
	warping warpenguin.no-ip.org -syn -p $port -n 1 -w 1 | grep timed > /dev/null
	return $?
}

function send_packet {
	warping $1 -n 1 -custom -s 149112667 -a $2 -p 7073 -f SYN > /dev/null
}

HOST=$1
PORT=22
check_port $PORT
PORT_OPEN=$?
if [ $PORT_OPEN -eq 0 ]; then
	echo "Opening TCP port $PORT on $HOST ..."
	while
		send_packet $HOST 777
		sleep 1
		check_port $PORT
		PORT_OPEN=$?
		[ $PORT_OPEN -eq 0 ]
	do echo "Failed to open port, retrying..."; done
	echo "Port $PORT is now open!"
else
	echo "Closing TCP port $PORT on $HOST ..."
	while
		send_packet $HOST 333
		sleep 1
		check_port $PORT
		PORT_OPEN=$?
		[ $PORT_OPEN -eq 1 ]
	do echo "Failed to close port, retying..."; done
	echo "Port $PORT is now closed!"
fi

