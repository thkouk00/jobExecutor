#!/bin/bash

dirName=${1-`pwd`}
FILE_LIST=`ls ${dirName}/*.txt`
echo "The name of the directory to work in: ${dirName}"

max_count=0
min_count=9999999
pos=0
for file in ${FILE_LIST}
do
	result_lines=`grep 'search:.*path:.*.txt' ${file}`
	IFS=$'\n'							#change delimiter to \n
	flag=0
	for temp in ${result_lines}
	do
		IFS=' '							#change delimiter to ' '
		arr=($temp)						#put line in array in order to take word because we know the position
		if [ "${#temp_word[@]}" -eq "0" ]; then
			index=0
			pos=1
			temp_word[index]=${arr[6]}
			temp_file[index]=$file
		else
			for ((index=0; index<${#temp_word[@]}; index++))
			do
				if [ "${temp_word[$index]}" = "${arr[6]}" ]; then
					if [ "${temp_file[$index]}" == "${file}" ]; then
						echo "***IN HERE***"
						flag=1;
					else
						temp_file[$index]=$file
					fi
					break
				fi
			done
			if [ "$flag" -eq "1" ]; then
				flag=0;
				continue
			fi
			if [ "${index}" -eq "${#temp_word[@]}" ]; then
				temp_word[$pos]=${arr[6]}
				temp_file[$pos]=$file
				index=$pos
				let "pos = pos + 1"
			fi
		fi		
		rr=`grep -o '.txt' <<< "$temp" | wc -l`					#count paths 		
		if [ "${#count[@]}" -ne "0" ]; then
			let "count[$index]= ${count[$index]} + rr"					#inform count
		else
			count[$index]=${rr}
		fi
		if [ "${count[$index]}" -gt "${max_count}" ]; then
			max_count=${count[$index]}
			max_word=${temp_word[$index]}
		fi
	done	
done

for ((i=0; i<${#count[@]}; i++))
do
	if [ "${count[$i]}" -lt "$min_count" ]; then
		min_count=${count[$i]}
		min_word=${temp_word[$i]}
	fi
done

echo "Total number of keywords searched: ${#count[@]}"				#number of words searched , no duplicates

if [ "$max_count" -eq "0" -a "$min_count" -eq "9999999" ]; then				#if no words found
	echo "No statistics for least/most frequently keyword searched."
else
	echo "Keyword most frequently found : ${max_word} [TotalNumFilesFound:${max_count}]"
	echo "Keyword least frequently found : ${min_word} [TotalNumFilesFound:${min_count}]"
fi
