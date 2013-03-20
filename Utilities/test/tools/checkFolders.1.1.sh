echo "Size of subfolders in : "$1
rfdir $1 | awk '{print $9}' > list.txt
python gen_compute.py $1 list.txt > compute.sh
chmod +x compute.sh
./compute.sh
