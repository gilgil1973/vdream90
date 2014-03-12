NOW=$(date +"%Y.%m.%d")
FILENAME="vdream90bin($NOW).zip"
zip $FILENAME getline httpproxy logserver netc netclient nets netserver vlog
