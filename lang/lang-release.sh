#! /bin/bash
#
#  script to update all *.ts in lang dir
#



if [ ! -f  nootka_pl.ts ]; then 
  echo "Run script into lang dir !!!!"
  exit
fi

for TS in $(ls *.ts); do
  echo ${TS%.*}
  lrelease-qt5 ${TS%.*}.ts -qm ${TS%.*}.qm
done
