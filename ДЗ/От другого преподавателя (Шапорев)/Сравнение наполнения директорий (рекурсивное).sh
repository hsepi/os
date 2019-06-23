#!/bin/bash
IFS=$'\n' # Без этого не работает связка for и find, если в именах файлов есть пробелы
USAGE="usage: 1.sh 1_dir 2_dir"
# 1 - чем исполняется скрипт, 2 - пояснение, как запускать сценарий
if [ -n $1 ] && [ -n $2 ] # Если нет 1 и 2 аргументов, то скрипт не выполняется
then
	dir_1="$1"
	dir_2="$2"
	common_files=""  # В этих переменных будут храниться 1) Общие файлы 
	dir1_files=""	 # 2) Уникальные для 1 дир. 3) Уникальные для 2 дир.
	dir2_files=""

	for file in `find $1 -type f` # В этом цикле выискиваем общие файлы и записываем их в переменную 1
	do
		for file_2 in `find $2 -type f`
		do
			file=${file#$dir_1}
			file_2=${file_2#$dir_2}
			if [ $file == $file_2 ]
			then
				common_files=$common_files"$file"$IFS
				break 
			fi	
		done
	done

	for file in `find $dir_1 -type f` # Здесь ищем уникальные файлы для 1 директории и записываем их в переменную 2
	do
		for file_2 in $common_files
		do
			file=${file#$dir_1}
			if [ $file = $file_2 ]
			then
				continue 2
			fi	
		done
		dir1_files=$dir1_files"$file"$IFS
	done

	for file in `find $dir_2 -type f` # Здесь ищем уникальные файлы для 2 директории и записываем их в переменную 3
	do
		for file_2 in $common_files
		do
			file=${file#$dir_2}
			if [ $file = $file_2 ]
			then
				continue 2
			fi	
		done
		dir2_files=$dir2_files"$file"$IFS
	done

	# Далее вывод уникальных файлов
	echo "_____Unique files:_____"
	echo "$IFS[$1]:"
	for file in $dir1_files
	do
		echo $file
	done
	echo "$IFS[$2]:"
	for file in $dir2_files
	do
		echo $file
	done


	echo "$IFS""_____Common files:_____""$IFS"
	for file in $common_files # Цикл для сравнения общих файлов
	do	# Смотрим код вывода cmp (0 - одинаковые, 1 - нет)
		if cmp "$dir_1/$file" "$dir_2/$file" -s # Вывод утилиты перенаправляем в /dev/null, т.к. он нам не нужен
		then
			echo "$file files are similar."
		else
			echo "$file files are different."
		fi
	done
fi
