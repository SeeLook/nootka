#!/bin/sh
cat <<EOF
<?xml version="1.0" encoding="UTF-8"?>
  <mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
    <mime-type type="application/x-nootka-noo">
      <sub-class-of type="application/data"/>
      <sub-class-of type="application/bin"/>
      <comment>Nootka exam file</comment>
      <comment xml:lang="pl">Plik egzaminu programu Nootka</comment>
      <icon>$1/share/nootka/picts/nootka-exam.png</icon>
      <glob pattern="*.noo" />    
  </mime-type>
 </mime-info> 
EOF