#!/bin/sh
cat <<EOF
<?xml version="1.0" encoding="UTF-8"?>
 <mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="application/x-nootka-nel">
    <sub-class-of type="application/data"/>
    <sub-class-of type="application/bin"/>
    <comment>Nootka exam's level</comment>
    <comment xml:lang="pl">Poziom egzaminu programu Nootka</comment>
    <icon>$1/share/nootka/picts/levelCreator.png</icon>
    <glob pattern="*.nel"/>
    <acronym>NEL</acronym>
    <expanded-acronym>Nootka Exam's Level</expanded-acronym>
  </mime-type>
 </mime-info> 
EOF