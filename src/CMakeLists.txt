cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)

project(visualmetafont CXX)

if (NOT EMSCRIPTEN)
#  find_package(ZLIB REQUIRED)
#  find_package(PNG REQUIRED)
#  find_package(Freetype REQUIRED)
endif()

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

cmake_policy(SET CMP0071 NEW)

#add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/showIncludes>)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(WEBLIB_NAME VisualMetaFontWasm)

if (NOT EMSCRIPTEN)

  find_package(Qt5 COMPONENTS Core OpenGL Svg PrintSupport Widgets Qml REQUIRED)



  #[[

  get_target_property(
    linked_libs
    Qt5::Widgets
    INTERFACE_LINK_LIBRARIES
    )


  set(debug "${_qt5Widgets_install_prefix}/lib/qtpcre2d.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5ThemeSupportd.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5EventDispatcherSupportd.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5AccessibilitySupportd.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5WindowsUIAutomationSupportd.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5FontDatabaseSupportd.lib;")
  set(nondebug "${_qt5Widgets_install_prefix}/lib/qtpcre2.lib"
    "${_qt5Widgets_install_prefix}/lib/Qt5ThemeSupport.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5EventDispatcherSupport.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5AccessibilitySupport.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5WindowsUIAutomationSupport.lib;"
    "${_qt5Widgets_install_prefix}/lib/Qt5FontDatabaseSupport.lib;")

  set(debug_gen_expr "$<$<CONFIG:Debug>:${debug}>")
  set(nondebug_gen_expr "$<$<NOT:$<CONFIG:Debug>>:${nondebug}>")
  set(gen_expr "${debug_gen_expr};${nondebug_gen_expr}")

  set_target_properties(Qt5::Widgets  PROPERTIES INTERFACE_LINK_LIBRARIES "${gen_expr};${linked_libs}")
  ]]

else()
  set(Qt5_DIR "${QTBuild}/lib/cmake/Qt5")
  set(Qt5Widgets_DIR "${QTBuild}/lib/cmake/Qt5Widgets")
  set(Qt5Gui_DIR "${QTBuild}/lib/cmake/Qt5Gui")
  set(Qt5Core_DIR "${QTBuild}/lib/cmake/Qt5Core")
  set(Qt5OpenGL_DIR "${QTBuild}/lib/cmake/Qt5OpenGL")
  set(Qt5QML_DIR "${QTBuild}/lib/cmake/Qt5QML")
  set(Qt5SVG_DIR "${QTBuild}/lib/cmake/Qt5SVG")
  set(Qt5PrintSupport_DIR "${QTBuild}/lib/cmake/Qt5PrintSupport")
  set(Qt5Network_DIR "${QTBuild}/lib/cmake/Qt5Network")
  set(Qt5EventDispatcherSupport_DIR "${QTBuild}/lib/cmake/Qt5EventDispatcherSupport")
  set(Qt5FontDatabaseSupport_DIR "${QTBuild}/lib/cmake/Qt5FontDatabaseSupport")

  find_package(Qt5 COMPONENTS Core OpenGL Svg PrintSupport Widgets EventDispatcherSupport FontDatabaseSupport REQUIRED)

  get_target_property(
    linked_libs
    Qt5::Widgets
    INTERFACE_LINK_LIBRARIES
    )

  set(gen_expr "${QTBuild}/lib/libqtpcre2.a"
    "${QTBuild}/lib/libqtfreetype.a;"
    "${QTBuild}/lib/libqtlibpng.a;")

  set_target_properties(Qt5::Widgets  PROPERTIES INTERFACE_LINK_LIBRARIES "${gen_expr};${linked_libs}")

endif()

set(Automedina
  automedina/automedina.cpp
  automedina/automedina.h
  automedina/defaultmarkpositions.h
  )

source_group("Automedina" FILES ${Automedina})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/GlyphParser)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/FeaParser)


#BISON_TARGET(FeaParser FeaParser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/FeaParser/parser.cpp VERBOSE COMPILE_FLAGS -Wcounterexamples)
BISON_TARGET(FeaParser FeaParser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/FeaParser/parser.cpp VERBOSE)
FLEX_TARGET(FeaScanner FeaParser/scanner.l  ${CMAKE_CURRENT_BINARY_DIR}/FeaParser/scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(FeaScanner FeaParser)

set(FeaParser
  FeaParser/parser.y
  FeaParser/scanner.l
  FeaParser/driver.cpp
  FeaParser/driver.h
  FeaParser/feaast.cpp
  FeaParser/feaast.h
  FeaParser/scanner.h
  ${BISON_FeaParser_OUTPUTS}
  ${FLEX_FeaScanner_OUTPUTS}
  )

source_group("FeaParser" FILES ${FeaParser})

BISON_TARGET(GlyphParser GlyphParser/glyphparser.y ${CMAKE_CURRENT_BINARY_DIR}/GlyphParser/glyphparser.cpp VERBOSE)
FLEX_TARGET(GlyphScanner GlyphParser/glyphscanner.l  ${CMAKE_CURRENT_BINARY_DIR}/GlyphParser/glyphscanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(GlyphScanner GlyphParser)

set(GlyphParser
  GlyphParser/glyphdriver.cpp
  GlyphParser/glyphdriver.h
  GlyphParser/glyphscanner.h
  GlyphParser/glyphparser.y
  GlyphParser/glyphscanner.l
  ${BISON_GlyphParser_OUTPUTS}
  ${FLEX_GlyphScanner_OUTPUTS}
  )

source_group("GlyphParser" FILES ${GlyphParser})

set(HTML5
  HTML5/ExportToHTML.cpp
  HTML5/ExportToHTML.h
  )
source_group("HTML5" FILES ${HTML5})

set(Layout_WithoutGUI
  Layout/JustificationContext.cpp
  Layout/JustificationContext.h
  #Layout/GlyphItem.cpp
  Layout/GlyphVis.cpp
  Layout/GlyphVis.h
  #Layout/GraphicsSceneAdjustment.cpp
  #Layout/GraphicsViewAdjustment.cpp
  #Layout/LayoutWindow.cpp
  Layout/QByteArrayOperator.cpp
  Layout/QByteArrayOperator.h
  Layout/Lookup.cpp
  Layout/Lookup.h
  Layout/OtLayout.cpp
  Layout/OtLayout.h
  Layout/Subtable.cpp
  Layout/Subtable.h
  Layout/to_opentype.cpp
  Layout/to_opentype.h
  )



set(Layout
  ${Layout_WithoutGUI}
  Layout/GlyphItem.cpp
  Layout/GraphicsSceneAdjustment.cpp
  Layout/GraphicsViewAdjustment.cpp
  Layout/LayoutWindow.cpp
  )
source_group("Layout" FILES ${Layout})

IF(DEFINED ENV{ENABLE_PDF_GENERATION})
set(Pdf
  Pdf/qpdf.cpp
  Pdf/qpdf.h
  Pdf/quranpdfwriter.cpp
  )
endif()

source_group("Pdf" FILES ${Pdf})

set(QuranText
  qurantext/quran.cpp
  qurantext/quran.h
  qurantext/qurancomplex.cpp
  )
source_group("QuranText" FILES ${QuranText})

set(Resource_Files
  Resources/mpgui.qrc
  )
source_group("Resource Files" FILES ${Resource_Files})

set(Main
  main.cpp
  )
source_group("" FILES ${Main})

set(FontWindow
  fontwindow/fontscene.cpp
  fontwindow/glyphcellheaderitem.cpp
  fontwindow/glyphcellitem.cpp
  fontwindow/mainwindow.cpp
  fontwindow/mdichild.cpp
  )
source_group("FontWindow" FILES ${FontWindow})

set(GlyphWindow
  glyphwindow/commands.cpp
  glyphwindow/commands.h
  glyphwindow/componentitem.cpp
  glyphwindow/contouritem.cpp
  glyphwindow/glyphparametercontroller.cpp
  glyphwindow/glyphscene.cpp
  glyphwindow/glyphview.cpp
  glyphwindow/glyphwindow.cpp
  glyphwindow/guidesitem.cpp
  glyphwindow/imageitem.cpp
  glyphwindow/knotcontrolleditem.cpp
  glyphwindow/knotitem.cpp
  glyphwindow/pairitem.cpp
  glyphwindow/ruleritem.cpp
  glyphwindow/tensiondirectionitem.cpp
  )
source_group("GlyphWindow" FILES ${GlyphWindow})

set(MetaFont
  metafont/font.cpp
  metafont/font.hpp
  metafont/glyph.cpp
  metafont/glyph.hpp
  metafont/exp.hpp
  metafont/metafont.h
  )
source_group("MetaFont" FILES ${MetaFont})



set(ALL_FILES  ${Automedina} ${GlyphWindow} ${FeaParser} ${MetaFont} ${FontWindow}
  ${GlyphParser} ${HTML5} ${Layout} ${Main} ${Pdf} ${QuranText} ${Generated_Files} ${Resource_Files})


set(WEBLIB_FILES  ${Automedina} ${FeaParser}  
  ${Layout_WithoutGUI} ${QuranText} ${Header_Files} ${Resource_Files} weblib/visualmetafontwasm.cpp weblib/quranshaper.h)

add_executable(${WEBLIB_NAME} ${WEBLIB_FILES})

add_executable(${PROJECT_NAME} ${ALL_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/."
  "${CMAKE_CURRENT_SOURCE_DIR}/Layout"
  "${CMAKE_CURRENT_SOURCE_DIR}/FeaParser"
  "${CMAKE_CURRENT_SOURCE_DIR}/GlyphParser"
  "${CMAKE_CURRENT_SOURCE_DIR}/metafont"
  "${CMAKE_CURRENT_SOURCE_DIR}/fontwindow"
  "${CMAKE_CURRENT_SOURCE_DIR}/glyphwindow"
  )

target_include_directories(${WEBLIB_NAME} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/."
  "${CMAKE_CURRENT_SOURCE_DIR}/Layout"
  "${CMAKE_CURRENT_SOURCE_DIR}/FeaParser"
  "${CMAKE_CURRENT_SOURCE_DIR}/GlyphParser"
  "${CMAKE_CURRENT_SOURCE_DIR}/metafont"
  "${CMAKE_CURRENT_SOURCE_DIR}/fontwindow"
  "${CMAKE_CURRENT_SOURCE_DIR}/glyphwindow"
  )

target_include_directories(${PROJECT_NAME} PRIVATE ${FLEX_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/FeaParser ${CMAKE_CURRENT_BINARY_DIR}/GlyphParser)
target_include_directories(${WEBLIB_NAME} PRIVATE ${FLEX_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/FeaParser ${CMAKE_CURRENT_BINARY_DIR}/GlyphParser)

target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_compile_options(${WEBLIB_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

target_compile_definitions(${PROJECT_NAME} PRIVATE HB_NO_PRAGMA_GCC_DIAGNOSTIC_ERROR)
target_compile_definitions(${WEBLIB_NAME} PRIVATE HB_NO_PRAGMA_GCC_DIAGNOSTIC_ERROR DIGITALKHATT_WEBLIB)


if (NOT EMSCRIPTEN)
  #target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_BOOLEAN)
  #target_compile_definitions(${WEBLIB_NAME} PRIVATE HAVE_BOOLEAN)
endif()

IF(DEFINED ENV{ENABLE_PDF_GENERATION})
target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_PDF_GENERATION)
endif()

add_dependencies(${PROJECT_NAME} harfbuzz mplib QtPropertyBrowser)
add_dependencies(${WEBLIB_NAME} harfbuzz mplib)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC harfbuzz mplib QtPropertyBrowser     
  Qt5::Core Qt5::CorePrivate Qt5::GuiPrivate Qt5::Gui Qt5::OpenGL  Qt5::Svg Qt5::PrintSupport Qt5::Widgets
  Qt5::QJpegPlugin
  Qt5::QSvgPlugin
  Qt5::QSvgIconPlugin
  )

target_link_libraries(${WEBLIB_NAME} PUBLIC harfbuzz mplib # QtPropertyBrowser     
  Qt5::Core  # Qt5::Widgets Qt5::CorePrivate Qt5::GuiPrivate Qt5::Gui Qt5::OpenGL  Qt5::Svg Qt5::PrintSupport
  #Qt5::QJpegPlugin Qt5::QSvgPlugin Qt5::QSvgIconPlugin

  )

if (NOT EMSCRIPTEN)

  #target_link_libraries(${PROJECT_NAME} PUBLIC ZLIB::ZLIB
    #Qt5::QWindowsIntegrationPlugin
    #imm32 version Dwmapi Mincore winmm Wtsapi32
  #  )

  #target_link_libraries(${WEBLIB_NAME} PUBLIC ZLIB::ZLIB
    # Freetype::Freetype
    # Qt5::QWindowsIntegrationPlugin
    #imm32 version Dwmapi Mincore winmm Wtsapi32
  #  )

  if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC imm32 version Dwmapi Mincore winmm Wtsapi32 Qt5::QWindowsIntegrationPlugin)
    target_link_libraries(${WEBLIB_NAME} PUBLIC imm32 version Dwmapi Mincore winmm Wtsapi32 Qt5::QWindowsIntegrationPlugin)
  endif (WIN32)
  #[[
  if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG )
    target_link_libraries(${WEBLIB_NAME} PUBLIC PNG::PNG )
  endif()
  ]]

else()

  target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::QWasmIntegrationPlugin Qt5::EventDispatcherSupport Qt5::FontDatabaseSupport)

  target_compile_options(${PROJECT_NAME} PRIVATE
    "SHELL:-s USE_ZLIB=1"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    # "SHELL:-s USE_FREETYPE=1"
    # "SHELL:-s USE_LIBPNG=1"
    )

  target_compile_options(${WEBLIB_NAME} PRIVATE
    # -fsanitize=leak
    "SHELL:-s USE_ZLIB=1"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    # "SHELL:-s USE_FREETYPE=1"
    # "SHELL:-s USE_LIBPNG=1"
    )

  target_link_options(${PROJECT_NAME} PRIVATE --bind
    "SHELL:-s ERROR_ON_UNDEFINED_SYMBOLS=0"
    #"SHELL:-s WASM=1"
    "SHELL:-s FULL_ES2=1"
    "SHELL:-s USE_WEBGL2=1"
    "SHELL:-s NO_EXIT_RUNTIME=0"
    # "SHELL:-s \"BINARYEN_TRAP_MODE='clamp'\""
    "SHELL:-s ALLOW_MEMORY_GROWTH=1" -O3
    "SHELL:-s USE_ZLIB=1"
    "SHELL:-s EXPORT_ES6=1"
    "SHELL:-s USE_ES6_IMPORT_META=0"
    "SHELL:-s \"EXTRA_EXPORTED_RUNTIME_METHODS=['FS','callMain']\""
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s \"EXPORTED_FUNCTIONS=['_main']\""
    #--post-js "${CMAKE_CURRENT_SOURCE_DIR}/post-js.js"
    )

  target_link_options(${WEBLIB_NAME} PRIVATE --bind --use-preload-plugins  # -fsanitize=leak
    "SHELL:-s ERROR_ON_UNDEFINED_SYMBOLS=1"
    #"SHELL:-s WASM=1"
    #"SHELL:-s WASM=0 --separate-asm -s \"SEPARATE_ASM_MODULE_NAME='VMetaFont'\""
    "SHELL:-s ASSERTIONS=1"
    #"SHELL:-s FULL_ES2=1"
    #"SHELL:-s USE_WEBGL2=1"
    "SHELL:-s USE_ES6_IMPORT_META=0"
    "SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
    "SHELL:-s NO_EXIT_RUNTIME=0"
    # "SHELL:-s \"BINARYEN_TRAP_MODE='clamp'\""
    "SHELL:-s ALLOW_MEMORY_GROWTH=1" #-O3
    "SHELL:-s USE_ZLIB=1"
    #"SHELL:-s USE_SDL=2"
    #"SHELL:-s EXPORT_ES6=1"
    "SHELL:-s \"EXTRA_EXPORTED_RUNTIME_METHODS=['FS','print','printErr']\""
    #"SHELL:-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=[\\\"$Browser\\\"]"
    "SHELL:-s MODULARIZE=1"
    #"SHELL:-s TOTAL_MEMORY=1048576000"
    "SHELL:-s ENVIRONMENT=web"
    #"SHELL:-s \"EXPORTED_FUNCTIONS=['_main']\""
    #--post-js "${CMAKE_CURRENT_SOURCE_DIR}/post-js.js"
    )

  if(WASMTYPE STREQUAL "WASM")

    target_link_options(${PROJECT_NAME} PRIVATE
      "SHELL:-s WASM=1"
      "SHELL:-s EXPORT_ES6=1"
      )

    target_link_options(${WEBLIB_NAME} PRIVATE
      "SHELL:-s WASM=1"
      "SHELL:-s EXPORT_ES6=1"
      )

    add_custom_command(TARGET ${WEBLIB_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_BINARY_DIR}/${WEBLIB_NAME}.js" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/app/services/quranservice"
      COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_BINARY_DIR}/${WEBLIB_NAME}.wasm" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
      COMMENT "Copying to output directory")

    #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/app/visualmetafont"
    #	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wasm" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    #	COMMENT "Copying to output directory")

  else()
    target_link_options(${WEBLIB_NAME} PRIVATE
      "SHELL:-s WASM=0"
      "SHELL:-s EXPORT_NAME='VMASM'"
      "SHELL:-s TOTAL_MEMORY=26214400"
      )

    add_custom_command(TARGET ${WEBLIB_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_BINARY_DIR}/${WEBLIB_NAME}.js"		"${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/vmasm"
      COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_BINARY_DIR}/${WEBLIB_NAME}.js.mem"	"${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/vmasm"
      COMMENT "Copying to output directory")

    #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_BINARY_DIR}/${WEBLIB_NAME}.js"		"${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/vmasm"
    #	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_BINARY_DIR}/${WEBLIB_NAME}.js.mem"	"${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/vmasm"
    #	COMMENT "Copying to output directory")


  endif()

  add_custom_command(TARGET ${WEBLIB_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../files/automedina.fea" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../files/ayah.mp" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../files/mfplain.mp" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../files/medinafont.mp" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../files/mpguifont.mp" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../files/myfontbase.mp" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../files/lookups.json" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../files/parameters.json" "${CMAKE_CURRENT_SOURCE_DIR}/../../digitalkhatt.org/ClientApp/src/assets"
    COMMENT "Copying to output directory")

endif()
