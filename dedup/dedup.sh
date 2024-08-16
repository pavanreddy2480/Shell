#!/bin/bash
# TODO: Type your shell script here
 # give directory path as an argument otherwise it will take current directory by default
declare -A filecksums

if [ $# -eq 0 ] # if no agruments are given take current directory 
then
    for file in *
    do
    
        [[ -f "$file" ]] && [[ ! -h "$file" ]] || continue
        # Generate the checksum
        cksum=$(cksum <"$file" | tr ' ' _)
        if [[ -n "${filecksums[$cksum]}" ]] && [[ "${filecksums[$cksum]}" != "$file" ]]
        then
            echo "Found '$file' is a duplicate of '${filecksums[$cksum]}'" >&2
            echo ${filecksums[$cksum]} | awk -F'/' '{print $NF}' > tj
            value=`cat tj`
            rm tj
            rm "$file"
            ln -s "$value" "$file"
        else
            filecksums[$cksum]=$file
        fi
    done
else 
    for file in "$@"/*  
    do
        
        [[ -f "$file" ]] && [[ ! -h "$file" ]] || continue
        # echo "HI"
        # Generate the checksum
        cksum=$(cksum <"$file" | tr ' ' _)
        if [[ -n "${filecksums[$cksum]}" ]] && [[ "${filecksums[$cksum]}" != "$file" ]]
        then
            echo "Found '$file' is a duplicate of '${filecksums[$cksum]}'" >&2
            echo ${filecksums[$cksum]} | awk -F'/' '{print $NF}' > filename #storing the filename in  file
            value=`cat filename`
            rm filename
            rm "$file"
            ln -s "$value" "$file"
        else
            filecksums[$cksum]=$file
        fi
    done
fi
