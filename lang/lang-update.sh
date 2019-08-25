#! /bin/bash
#
#  script to update all *.ts in lang dir
#

SRC_DIR="../src"


if [ ! -f "$SRC_DIR/main.cpp" ]; then
  echo "Run script into lang dir !!!!"
  exit
fi

for TS in $(ls nootka_*.ts); do
  echo $TS
#   lupdate -recursive $SRC_DIR -ts $TS
done
