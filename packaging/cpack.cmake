
IF(MINGW)
		include (InstallRequiredSystemLibraries)

		SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Nootka - to play scores")
		SET(CPACK_PACKAGE_VENDOR "Nootka")
		SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/LICENSE")
		SET(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/LICENSE")

		SET(CPACK_PACKAGE_VERSION_MAJOR "0")
		SET(CPACK_PACKAGE_VERSION_MINOR "7")
		SET(CPACK_PACKAGE_VERSION_PATCH "5")
		SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Nootka ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

		SET(CPACK_NSIS_COMPRESSOR "/FINAL /SOLID lzma")

		SET(CPACK_PACKAGE_INSTALL_DIRECTORY "nootka")
		# There is a bug in NSI that does not handle full unix paths properly. Make
		# sure there is at least one set of four (4) backlasshes.
		SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/picts\\\\logo.bmp")
		SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\nootka.exe,0")
		SET(CPACK_NSIS_DISPLAY_NAME ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
		SET(CPACK_NSIS_HELP_LINK "http://www.nootka.sf.net/")
		SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.nootka.sf.net/")
		SET(CPACK_NSIS_CONTACT "seelook@gmail.com")
		SET(CPACK_NSIS_MODIFY_PATH OFF)
		SET(CPACK_STRIP_FILES "nootka.exe")

		# File types association:
		SET(CPACK_NSIS_DEFINES "!include ${PROJECT_SOURCE_DIR}/packaging\\\\FileAssociation.nsh")

		SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
			Push \\\"ATENDATA\\\"
			Push \\\"$INSTDIR\\\\share\\\\aten\\\"
			Call WriteEnvStr
		")

		SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
			\\\${registerExtension} \\\"Nootka exam level file\\\" \\\".nel\\\" \\\"\\\$INSTDIR\\\\nootka.exe\\\"
			\\\${registerExtension} \\\"Nootka exam results file\\\" \\\".noo\\\" \\\"\\\$INSTDIR\\\\nootka.exe\\\"
		")
		SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
			\\\${unregisterExtension} \\\".nel\\\" \\\"Nootka exam level file\\\"
			\\\${unregisterExtension} \\\".noo\\\" \\\"Nootka exam results file\\\"
		")
		
		include (CPack)
  ENDIF(MINGW)
