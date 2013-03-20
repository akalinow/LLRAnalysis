echo "Size of folder : "$1
rfdir -r $1 | awk '{s+=$5}END{print s/(1024^3)" GB"}'