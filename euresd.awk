awk -F "\"*,\"*" '{print $1" "$2":00",$3,$4,$5,$6}' EURUSD1.csv
