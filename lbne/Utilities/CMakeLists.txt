
set( lbne_util_lib_list LArFFT_service
                        LArProperties_service
			Geometry
			Geometry_service
			Utilities
  			${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_PERSISTENCY_COMMON}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_FRAMEWORK_SERVICES_BASIC}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
			${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}
			${ART_UTILITIES}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
			${ROOT_BASIC_LIB_LIST}
    )


simple_plugin( SignalShapingServiceLBNE10kt  "service"
               ${lbne_util_lib_list}
	       BASENAME_ONLY
        )
             
 
simple_plugin( SignalShapingServiceLBNE34kt  "service"
               ${lbne_util_lib_list}
	       BASENAME_ONLY
        )
              

simple_plugin( SignalShapingServiceLBNE35t  "service"
               ${lbne_util_lib_list}
	       BASENAME_ONLY
        )

simple_plugin( FileCatalogMetadataLBNE "service"
               ${lbne_util_lib_list}
	       BASENAME_ONLY
	)

install_headers()
install_fhicl()
install_source()

add_subdirectory(test)

