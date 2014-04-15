NOW=$(date +"%Y.%m.%d")
FILENAME="vdream90bin($NOW).zip"
zip $FILENAME getline logserver netc netclient nets netserver vlog webproxy
