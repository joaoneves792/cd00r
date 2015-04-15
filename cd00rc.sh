#! /bin/zsh
# Joao Neves 13-12-2014

echo "\e[0;32m"

echo "             _____  ___     "
echo "     _______/ / _ \/ _ \____"
echo "    / __/ _  / // / // / __/"
echo "    \__/\_,_/\___/\___/_/   "

echo "\e[1;37m"
if [[ $1 = "open" ]]; then
	echo "Opening TCP port 22 on $2 ..."
	warping $2 -n 1 -custom -s 149112667 -a 777 -p 7073 -f SYN
elif [[ $1 = "close" ]]; then
	echo "Closing TCP port 22 on $2 ..."
	warping $2 -n 1 -custom -s 149112667 -a 333 -p 7073 -f SYN
fi
