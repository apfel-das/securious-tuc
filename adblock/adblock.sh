#!/bin/bash
# You are NOT allowed to change the files' names!
domainNames="domainNames.txt"
IPAddresses="IPAddresses.txt"
adblockRules="adblockRules"
ip_table="/sbin/iptables"



function handle_error()
{
	msg=$1

	echo "Something went wrong on [$1]"
}




# reset's adblock rules to their initial format..
function reset_rules(){

	#set to accept all types of conns..
	"$ip_table" -P INPUT ACCEPT
	"$ip_table" -P OUTPUT ACCEPT
	"$ip_table" -P FORWARD ACCEPT

	#delete old rules; quick way used..

	"$ip_table" -F 


}




#Configures given domains by resolving their ip; appends in a file to use later..
function resolve_hosts(){

	#get input file, output file..
	input_file=$domainNames
	output_file=$IPAddresses
	local dns_server="8.8.8.8" #google's DNS is quicker than mine; might end up not using that later..
	

	#be sure the file is there..
	if [[ ! -f "$input_file" ]]; then
		echo "File not found, exiting.."
		exit -1
	fi

	#parse, resolve, exclude errors; redirect output as well..

	while IFS= read -r domain || [[ -n "$domain" ]]; 
	do
		
		# could also use "dig -f <filename>" but this takes more time; forcing -tries, -timeout makes thing also quicker in the sake of some results..
		dig @${dns_server} ${domain} +short +tries=1 +time=5 | grep  '[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$' &>> "${output_file}"

			
	done<"$input_file"

	

	

}

#make some blocking rules using iptables and a file specified by $IPAddresses..
function make_rules(){

	input_file=$IPAddresses

	while IFS= read -r ip || [[ -n "$ip" ]]; do
		echo "cc"
	done < ${input_file}

}


function adBlock() {
    if [ "$EUID" -ne 0 ];then
        printf "Please run as root.\n"
        exit 1
    fi
    if [ "$1" = "-domains"  ]; then
        # Configure adblock rules based on the domain names of $domainNames file.
		

		# resolve and lock on process; this ensures procees won't get stoped by a close of terminal..      
        resolve_hosts &
        
        pid=$!
        wait $pid 
		
		if [[ "$?" -eq "0" ]]; then
			echo "New rules are now online.."
		else
			echo "Issue arrised.."
		fi



        true
            
    elif [ "$1" = "-ips"  ]; then
        # Configure adblock rules based on the IP addresses of $IPAddresses file.
       	
       	make_rules



        true
        
    elif [ "$1" = "-save"  ]; then
        # Save rules to $adblockRules file.
        # Write your code here...
        # ...
        # ...
        true
        
    elif [ "$1" = "-load"  ]; then
        # Load rules from $adblockRules file.
        # Write your code here...
        # ...
        # ...
        true

        
    elif [ "$1" = "-reset"  ]; then
        # Reset rules to default settings (i.e. accept all).
        # Write your code here...
        # ...
        # ...

        reset_rules

        true

        
    elif [ "$1" = "-list"  ]; then
        # List current rules.
        # Write your code here...
        # ...
        # ...
        true
        
    elif [ "$1" = "-help"  ]; then
        printf "This script is responsible for creating a simple adblock mechanism. It rejects connections from specific domain names or IP addresses using iptables.\n\n"
        printf "Usage: $0  [OPTION]\n\n"
        printf "Options:\n\n"
        printf "  -domains\t  Configure adblock rules based on the domain names of '$domainNames' file.\n"
        printf "  -ips\t\t  Configure adblock rules based on the IP addresses of '$IPAddresses' file.\n"
        printf "  -save\t\t  Save rules to '$adblockRules' file.\n"
        printf "  -load\t\t  Load rules from '$adblockRules' file.\n"
        printf "  -list\t\t  List current rules.\n"
        printf "  -reset\t  Reset rules to default settings (i.e. accept all).\n"
        printf "  -help\t\t  Display this help and exit.\n"
        exit 0
    else
        printf "Wrong argument. Exiting...\n"
        exit 1
    fi
}

adBlock $1
exit 0