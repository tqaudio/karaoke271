if(SMTG_AAX_SDK_PATH AND SMTG_ADD_VSTGUI)
  include(AddAAXLibrary)
  set(
    base_sources
    ${SDK_ROOT}/public.sdk/source/vst/basewrapper/basewrapper.sdk.cpp
  )
  set(
    Karaoke271aax_sources
    source/aax.cpp
    ../Karaoke271/include/controller.h
    ../Karaoke271/include/constants.h
    ../Karaoke271/include/id.h
    ../Karaoke271/include/parameter.h
    ../Karaoke271/include/processor.h
    ../Karaoke271/include/version.h
    ../Karaoke271/source/controller.cpp
    ../Karaoke271/source/factory.cpp
    ../Karaoke271/source/parameter.cpp
    ../Karaoke271/source/processor.cpp
  )
  set(Karaoke271_sources ${Karaoke271aax_sources} ${base_sources})
  set(target Karaoke271aax)
  smtg_add_aaxplugin(${target} ${Karaoke271_sources})
  target_include_directories(${target} PUBLIC ${VSTGUI_ROOT}/vstgui4)
  target_include_directories(${target} PUBLIC ../Karaoke271/include)
  target_include_directories(${target} PUBLIC ../Karaoke271/source)
  target_link_libraries(${target} PRIVATE base sdk vstgui_support aaxwrapper)

  if(SMTG_MAC)
#    smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/../hoge/resource/Info.plist" PREPROCESS)
  elseif(SMTG_WIN)
    target_sources(${target} PRIVATE ../Karaoke271/resource/plugin.rc)
    # remove warnings from VST2
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  endif()
endif()
