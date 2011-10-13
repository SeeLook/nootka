
# mime-types associations under linux
# was worked fine
if(UNIX AND NOT APPLE)
  add_custom_target(install-mime
#     INSTALL(FILES mime/nootka.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
#     INSTALL(FILES picts/nootka.svg DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")
#     INSTALL( CODE"
#       execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/mime )
#       execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mime/install-mime.sh ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/mime ${CMAKE_CURRENT_SOURCE_DIR}/mime)" )
# 
#     INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mime DESTINATION "${CMAKE_INSTALL_PREFIX}/share/nootka")
#     find_program(XDG-MIME_EXECUTABLE xdg-mime)
#     find_program(XDG-ICON_EXECUTABLE xdg-icon-resource)
#     find_program(XDG-MENU_EXECUTABLE xdg-desktop-menu)
#     INSTALL( CODE"
#       execute_process(COMMAND echo 'Installing mime-types' )
#       execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor ${CMAKE_CURRENT_BINARY_DIR}/mime/nootka.xml)
#       execute_process(COMMAND ${XDG-MENU_EXECUTABLE} install --novendor ${CMAKE_CURRENT_SOURCE_DIR}/mime/nootka.desktop)
#       execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop application/x-nootka-noo)
#       execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop application/x-nootka-nel)
#       execute_process(COMMAND ${XDG-ICON_EXECUTABLE} install --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/pixmaps/levelCreator.png application-x-nootka-nel)
#       execute_process(COMMAND ${XDG-ICON_EXECUTABLE} install --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/pixmaps/nootka-exam.png application-x-nootka-noo)"
#     )
# 
  )
  
  add_custom_target(uninstall-mime
#      COMMAND echo "uninstalling mime-types"
#      COMMAND ${XDG-MIME_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/nootka/mime/nootka.xml
#      COMMAND ${XDG-MENU_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/applications/nootka.desktop
#      COMMAND ${XDG-ICON_EXECUTABLE} uninstall --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/pixmaps/levelCreator.png application-x-nootka-nel
#      COMMAND ${XDG-ICON_EXECUTABLE} uninstall --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/pixmaps/nootka-exam.png application-x-nootka-noo
  )
# 
endif(UNIX AND NOT APPLE)

