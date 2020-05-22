function(install_header_only)
    install(
        TARGETS ${ARGV0}
        EXPORT ${ARGV0}-config)
    install(
        EXPORT ${ARGV0}-config
        NAMESPACE ${ARGV0}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${ARGV0})
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/${ARGV0}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${ARGV0})
endfunction()
