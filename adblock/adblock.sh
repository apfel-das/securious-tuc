#!/bin/bash
# You are NOT allowed to change the files' names!
domainNames="domainNames_tst.txt"
IPAddresses="IPAddresses.txt"
adblockRules="adblockRules"
ip_table="/sbin/iptables"



function handle_error()
{
	msg=$1

	echo "Something went wrong on [$1]"
}


# the actual ip lookup, uses google's DNS server..
function ip_lookup(){

	local host_name=$1
	local dns_server="8.8.8.8" #google's DNS is quicker than mine..

	#dummies will always exist; catch em..
	if [[ ! -n "$host_name" ]]; then
		echo "No domain provided.. [Hostname empty]"
		exit -1
	fi

	# force custom DNS usage over the predefined; care about IPv4 only..
	host -t A  ${hostname} &>/dev/null ${dns_server}

	# be sure the whole thing worked..
	if [[ "$?" -ne "0" ]]; then
		echo "Something went wrong on [DNS pick]"
		exit -1
	fi

	# get the IPv4 in terms of <[0-255]>.<[0-255]>.<[0-255]>.<[0-255]> as a format;
	local ip_addr="$(host -t A ${host_name} ${dns_server}| awk '/has.*address/{print $NF; exit}')" 

	# this acts more like a return statement, should't det printed at all on stdout..
	echo "$ip_addr"

	


	
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
function resolve_domains(){

	#get input file, output file..
	input_file=$domainNames
	output_file=$IPAddresses
	

	#be sure the file is there..
	if [[ ! -f "$input_file" ]]; then
		echo "File not found, exiting.."
		exit -1
	fi

	# get domains as lines. be sure to read the last one as well..
	while IFS= read -r domain || [[ -n "$domain" ]]; 
	do
		
		# a trick to get the value from function
		retval=$(ip_lookup $domain)
		

		#debug printing..
		if [[ -n "$retval" ]]; then
			echo "$domain -> $retval"
			echo "$retval" &>> "$output_file"
		else
			echo "$domain -> is down.."
		fi

		

	
	done<"$input_file"

	

}




function adBlock() {
    if [ "$EUID" -ne 0 ];then
        printf "Please run as root.\n"
        exit 1
    fi
    if [ "$1" = "-domains"  ]; then
        # Configure adblock rules based on the domain names of $domainNames file.
      
        resolve_domains 
        

        true
            
    elif [ "$1" = "-ips"  ]; then
        # Configure adblock rules based on the IP addresses of $IPAddresses file.
        # Write your code here...
        # ...
        # ...
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