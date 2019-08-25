#! /bin/bash
#
#  script to update all *.ts in lang dir
#



if [ ! -f  nootka_pl.ts ]; then 
  echo "Run script into lang dir !!!!"
  exit
fi

for TS in $(ls nootka_*.ts); do
  if [ ! "$(echo $TS|grep _xx )" ]; then
    echo ${TS%.*}
    lrelease ${TS%.*}.ts -qm ${TS%.*}.qm
  fi
done
