#!/bin/sh
folders=(".")
for f in ./**/*.c
do
    dir="${f%/*}"
    filename="${f##*/}"
    chardetectResult="$(chardetect $f)"
    if [[ $chardetectResult == *"GB2312"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f GB2312 -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted GB2312 : $f"
    elif [[ $chardetectResult == *"ascii"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f ASCII -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted ASCII : $f"
    # elif [[ $chardetectResult == *"ISO-8859-1"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-1 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-1 : $f"
    # elif [[ $chardetectResult == *"ISO-8859-9"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-9 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-9 : $f"
    # elif [[ $chardetectResult == *"EUC-KR"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-KR -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-KR : $f"
    # elif [[ $chardetectResult == *"EUC-JP"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-JP -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-JP : $f"
    # elif [[ $chardetectResult == *"Windows-1252"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f WINDOWS-1252 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted WINDOWS-1252 : $f"
    # elif [[ $chardetectResult == *"Big5"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f BIG-5 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted BIG-5 : $f"
    # elif [[ $chardetectResult == *"TIS-620"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f TIS-620 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted TIS-620 : $f"
    elif [[ $chardetectResult != *"utf-8"* ]]; then
        echo "skipped $chardetectResult"
    fi
done
for f in ./*.c
do
    dir="${f%/*}"
    filename="${f##*/}"
    chardetectResult="$(chardetect $f)"
    if [[ $chardetectResult == *"GB2312"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f GB2312 -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted GB2312 : $f"
    elif [[ $chardetectResult == *"ascii"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f ASCII -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted ASCII : $f"
    # elif [[ $chardetectResult == *"ISO-8859-1"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-1 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-1 : $f"
    # elif [[ $chardetectResult == *"ISO-8859-9"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-9 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-9 : $f"
    # elif [[ $chardetectResult == *"EUC-KR"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-KR -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-KR : $f"
    # elif [[ $chardetectResult == *"EUC-JP"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-JP -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-JP : $f"
    # elif [[ $chardetectResult == *"Windows-1252"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f WINDOWS-1252 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted WINDOWS-1252 : $f"
    # elif [[ $chardetectResult == *"Big5"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f BIG-5 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted BIG-5 : $f"
    # elif [[ $chardetectResult == *"TIS-620"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f TIS-620 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted TIS-620 : $f"
    elif [[ $chardetectResult != *"utf-8"* ]]; then
        echo "skipped $chardetectResult"
    fi
done
for f in ./**/*.h
do
    dir="${f%/*}"
    filename="${f##*/}"
    chardetectResult="$(chardetect $f)"
    if [[ $chardetectResult == *"GB2312"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f GB2312 -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted GB2312 : $f"
    elif [[ $chardetectResult == *"ascii"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f ASCII -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted ASCII : $f"
    # elif [[ $chardetectResult == *"ISO-8859-1"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-1 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-1 : $f"
    # elif [[ $chardetectResult == *"ISO-8859-9"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-9 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-9 : $f"
    # elif [[ $chardetectResult == *"EUC-KR"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-KR -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-KR : $f"
    # elif [[ $chardetectResult == *"EUC-JP"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-JP -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-JP : $f"
    # elif [[ $chardetectResult == *"Windows-1252"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f WINDOWS-1252 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted WINDOWS-1252 : $f"
    # elif [[ $chardetectResult == *"Big5"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f BIG-5 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted BIG-5 : $f"
    # elif [[ $chardetectResult == *"TIS-620"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f TIS-620 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted TIS-620 : $f"
    elif [[ $chardetectResult != *"utf-8"* ]]; then
        echo "skipped $chardetectResult"
    fi
done
for f in ./*.h
do
    dir="${f%/*}"
    filename="${f##*/}"
    chardetectResult="$(chardetect $f)"
    if [[ $chardetectResult == *"GB2312"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f GB2312 -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted GB2312 : $f"
    elif [[ $chardetectResult == *"ascii"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f ASCII -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted ASCII : $f"
    # elif [[ $chardetectResult == *"ISO-8859-1"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-1 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-1 : $f"
    # elif [[ $chardetectResult == *"ISO-8859-9"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-9 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-9 : $f"
    # elif [[ $chardetectResult == *"EUC-KR"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-KR -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-KR : $f"
    # elif [[ $chardetectResult == *"EUC-JP"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-JP -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-JP : $f"
    # elif [[ $chardetectResult == *"Windows-1252"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f WINDOWS-1252 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted WINDOWS-1252 : $f"
    # elif [[ $chardetectResult == *"Big5"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f BIG-5 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted BIG-5 : $f"
    # elif [[ $chardetectResult == *"TIS-620"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f TIS-620 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted TIS-620 : $f"
    elif [[ $chardetectResult != *"utf-8"* ]]; then
        echo "skipped $chardetectResult"
    fi
done
for f in ./**/*.dat
do
    dir="${f%/*}"
    filename="${f##*/}"
    chardetectResult="$(chardetect $f)"
    if [[ $chardetectResult == *"GB2312"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f GB2312 -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted GB2312 : $f"
    elif [[ $chardetectResult == *"ascii"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f ASCII -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted ASCII : $f"
    # elif [[ $chardetectResult == *"ISO-8859-1"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-1 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-1 : $f"
    # elif [[ $chardetectResult == *"ISO-8859-9"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-9 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-9 : $f"
    # elif [[ $chardetectResult == *"EUC-KR"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-KR -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-KR : $f"
    # elif [[ $chardetectResult == *"EUC-JP"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-JP -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-JP : $f"
    # elif [[ $chardetectResult == *"Windows-1252"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f WINDOWS-1252 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted WINDOWS-1252 : $f"
    # elif [[ $chardetectResult == *"Big5"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f BIG-5 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted BIG-5 : $f"
    # elif [[ $chardetectResult == *"TIS-620"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f TIS-620 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted TIS-620 : $f"
    elif [[ $chardetectResult != *"utf-8"* ]]; then
        echo "skipped $chardetectResult"
    fi
done
for f in ./*.dat
do
    dir="${f%/*}"
    filename="${f##*/}"
    chardetectResult="$(chardetect $f)"
    if [[ $chardetectResult == *"GB2312"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f GB2312 -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted GB2312 : $f"
    elif [[ $chardetectResult == *"ascii"* ]]; then
        cp $f "${dir}/${filename}.original"
        iconv -c -f ASCII -t UTF-8 $f > "${dir}/${filename}.converted"
        mv "${dir}/${filename}.converted" "${dir}/${filename}"
        echo "converted ASCII : $f"
    # elif [[ $chardetectResult == *"ISO-8859-1"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-1 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-1 : $f"
    # elif [[ $chardetectResult == *"ISO-8859-9"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f ISO-8859-9 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted ISO-8859-9 : $f"
    # elif [[ $chardetectResult == *"EUC-KR"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-KR -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-KR : $f"
    # elif [[ $chardetectResult == *"EUC-JP"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f EUC-JP -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted EUC-JP : $f"
    # elif [[ $chardetectResult == *"Windows-1252"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f WINDOWS-1252 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted WINDOWS-1252 : $f"
    # elif [[ $chardetectResult == *"Big5"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f BIG-5 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted BIG-5 : $f"
    # elif [[ $chardetectResult == *"TIS-620"* ]]; then
    #     cp $f "${dir}/${filename}.original"
    #     iconv -c -f TIS-620 -t UTF-8 $f > "${dir}/${filename}.converted"
    #     mv "${dir}/${filename}.converted" "${dir}/${filename}"
    #     echo "converted TIS-620 : $f"
    elif [[ $chardetectResult != *"utf-8"* ]]; then
        echo "skipped $chardetectResult"
    fi
done
# for ((i=0;i<11;i++))
# do 
# 	for f in ./${folders[i]}/*.c
# 	do
# 	  convFolder="./${folders[i]}/converted"
# 	  rm -rf ${convFolder}
# 	done
# done

# read -p "Done!"
