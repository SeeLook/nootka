#!/bin/sh
cat <<EOF
<?xml version="1.0" encoding="UTF-8"?>
 <mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">

  <mime-type type="application/x-nootka-nel">
    <sub-class-of type="application/data"/>
    <sub-class-of type="application/bin"/>
    <comment>Nootka level file</comment>
    <comment xml:lang="cs">Úroveň zkoušky programu Nootka</comment>
    <comment xml:lang="de">Nootka-Level Datei</comment>
    <comment xml:lang="es">Fichero de nivel de Nootka</comment>
    <comment xml:lang="fr">Fichier de niveau Nootka</comment>
    <comment xml:lang="pl">Poziom egzaminu programu Nootka</comment>
    <comment xml:lang="ru">Файл Нутки с уровнем</comment>
    <icon name="nootka-level">$1/share/pixmaps/nootka-level.png</icon>
    <glob pattern="*.nel"/>
    <acronym>NEL</acronym>
    <expanded-acronym>Nootka Exam Level</expanded-acronym>
  </mime-type>

  <mime-type type="application/x-nootka-noo">
    <sub-class-of type="application/data"/>
    <sub-class-of type="application/bin"/>
    <comment>Nootka exam file</comment>
    <comment xml:lang="cs">Zkušební soubor programu Nootka</comment>
    <comment xml:lang="de">Nootka-Prüfungs Datei</comment>
    <comment xml:lang="es">Fichero de examen de Nootka</comment>
		<comment xml:lang="fr">Fichier d'examen Nootka</comment>
    <comment xml:lang="pl">Plik egzaminu programu Nootka</comment>
    <comment xml:lang="ru">Файл Нутки с экзаменом</comment>
    <icon name="nootka-exam">$1/share/pixmaps/nootka-exam.png</icon>
    <glob pattern="*.noo" />
  </mime-type>

 </mime-info> 
EOF
