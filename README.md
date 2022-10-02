# cd00r



             _____  ___     
     _______/ / _ \/ _ \____
    / __/ _  / // / // / __/
    \__/\_,_/\___/\___/_/     
     Daemon


	Written by Joao Neves (joaoneves792@gmail.com)
	12-12-2014

	Description:
	Little daemon that sniffs packets on a tcp port waiting
	for a special packet matching certain criteria (has to 
	be injected by the client) and opens the ssh port (but 
        only for the same ip that sent the packet). This action
	is then reverted when another special packet is received.

	Parameters:
	You can pass the name of the network interface you want
	cd00rd to use (if none is supplied then the default one
	is used).

	Installation:
        run make
        run ./install.sh as root
        enable the cd00rd.service in systemd

