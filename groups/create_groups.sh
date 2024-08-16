#!/bin/bash
# this file takes 1 argument and responses.csv as  path
inputcsv=$1
if [ $# == 1 ];then

        cut -d',' -f3,4 ${inputcsv} | sed 's/",""//g' | sed 's/"//g' | sed 1d | sed 's/,/ /g' > tmp.1
        awk -F' ' '{if ($NF == $1) print $1; else if ($1 < $2) print $1 " " $2; else if($1 > $2) print $2 " " $1;}' tmp.1 > sample
        rm tmp.1
        sort sample > sortsample 
        sed 's/ /\n/g' sortsample >merge     #removing the dupliactes
        sort  merge | uniq -d >duplicates
        awk -F ' ' ' 
                    FNR==NR{
                        arr[$1]=1
                    }
                    FNR!=NR{                          #storing invalid student rollnumbers  
                        if($NF!=$1 && (arr[$1]!=1 || arr[$2]!=1))   
                        {
                            print $1"_"$2 >> "invalid"
                            if(arr[$1]!=1 && arr[$2]!=1)print $1 "_" $2 "("$1 "and both are not registered in the course" $2 ")"  >> "rinvalid"
                            else if(arr[$1]!=1)print $1 "_" $2 "("$1 " is not registered in the course)" >> "rinvalid"
                            else if(arr[$2]!=1)print $1 "_" $2 "("$2 " is not registered in the course)" >> "rinvalid"
                        }
                    }               
        ' students_list.csv sample
        awk -F' ' '
                    FNR==NR{
                        arr[$1]=1;
                    }
                    FNR!=NR{
                        if($NF==$1 && arr[$1]!=1)
                        {
                            print $1 >> "tempst"
                        }
                        else if(arr[$1]!=1 && arr[$2]!=1)
                        {
                            print $1 "_" $2 ""  >> "tempst"
                        }
                    }
                    
        ' duplicates sample
        awk -F' ' '
                    FNR==NR{
                        arr[$1]=1;
                    }
                    FNR!=NR{
                        if(arr[$1]!=1) 
                        {
                            print $1 >> "students_groups.csv"
                        }
                    }
                    
        ' invalid tempst
        sort students_list.csv >sortstudent
        sort merge > mresponses
        comm --output-delimiter=+ sortstudent mresponses > notresponded 
        sed '/+/d' notresponded >file_notresponded   # not responded students in file_notresponded
        echo "duplicates are " >&2
        cat duplicates >&2
        echo "invalid ids are " >&2
        cat rinvalid  >&2
        echo "not responded" >&2
        cat file_notresponded >&2
        # echo "valid student ids are"
        # cat students_groups.csv
        rm  tempst duplicates sample  merge  rinvalid invalid file_notresponded sortsample sortstudent notresponded mresponses 

elif [ $# -gt 1 ];then
        echo "too many arguments"
else
        echo "not enough arguments"
fi
