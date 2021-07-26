#! /bin/bash
#
#  script to update all *.ts in lang dir
#

SRC_DIR="../src"


if [ ! -f "$SRC_DIR/main.cpp" ]; then
  echo "Run script from the inside 'lang' directory !!!!"
  exit
fi

for TS in $(ls nootka_*.ts); do
  echo $TS
  lupdate -recursive $SRC_DIR -ts $TS
done

lupdate -no-obsolete -recursive $SRC_DIR -ts template-nootka_xx.ts
