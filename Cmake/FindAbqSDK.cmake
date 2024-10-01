# - Find Abaqus SDK library
# Find the Abaqus SDK includes and library
find_path(AbqSDK_PRIVATE_INCLUDE_DIR
        NAMES
        odb_API.h
        PATHS
        /usr/local/include
        /usr/include
        "D:/SIMULIA/EstProducts/2024/win_b64/code/include"
)

find_path(AbqSDK_PRIVATE_PUBLIC_DIR
        NAMES
        SMAOdb
        PATHS
        /usr/local/include
        /usr/include
        "D:/SIMULIA/EstProducts/2024"
)

find_path(AbqSDK_PRIVATE_LIB
        NAMES
        ABQSMAOdbApi.lib
        PATHS
        "D:/SIMULIA/EstProducts/2024/win_b64/code/lib"
)

mark_as_advanced(AbqSDK_PRIVATE_INCLUDE_DIR)
mark_as_advanced(AbqSDK_PRIVATE_PUBLIC_DIR)
mark_as_advanced(AbqSDK_PRIVATE_LIB)

set(ABQ_LIB_LIST
        "ABQSMAAbuBasicUtils"
        "ABQSMAAbuGeom"
        "ABQSMABasAlloc"
        "ABQSMABasCoreUtils"
        "ABQSMABasGenericsLib"
        "ABQSMABasPrfTrkLib"
        "ABQSMABasRtvUtility"
        "ABQSMABasShared"
        "ABQSMABasXmlDocument"
        "ABQSMABasXmlParser"
        "ABQSMAObjSimObjectsMod"
        "ABQSMAOdbApi"
        "ABQSMAOdbAttrEO"
        "ABQSMAOdbAttrEO2"
        "ABQSMAOdbCalcK"
        "ABQSMAOdbCore"
        "ABQSMAOdbCoreGeom"
        "ABQSMAOdbDdbOdb"
        "ABQSMARfmInterface"
        "ABQSMARomDiagEx"
        "ABQSMASglSharedLib"
        "ABQSMASglSimXmlFndLib"
        "ABQSMAShpCore"
        "ABQSMASimBCompress"
        "ABQSMASimBulkAPI"
        "ABQSMASimContainers"
        "ABQSMASimInterface"
        "ABQSMASimManifestSubcomp"
        "ABQSMASimPoolManager"
        "ABQSMASimS2fSubcomp"
        "ABQSMASimSerializerAPI"
        "ABQSMASrvBasic"
        "ABQSMASrvSimXmlConverters"
        "ABQSMAUzlZlib"
#        "CATBBMagic"
#        "CATComBase"
#        "CATComDrvBB"
#        "CATComHTTPEndPoint"
#        "CATComServices"
#        "CATComSidl"
#        "CATComSidlFile"
#        "CATLic"
#        "CATLMjni"
#        "CATP2PBaseUUID"
#        "CATP2PCore"
#        "CATPLMDispatcherItf"
#        "CATPLMDispatcherSpecificItf"
#        "CATScriptEngine"
#        "CATSysCATIAAI"
#        "CATSysCATIASF"
#        "CATSysCommunication"
#        "CATSysDbSettings"
#        "CATSysExternApp"
#        "CATSysMotifDrv"
#        "CATSysMultiThreadingSecured"
#        "CATSysPreview"
#        "CATSysProxy"
#        "CATSysRunBrw"
#        "CATSysTS"
#        "CATSysTSObjectModeler"
#        "CommunicationsUUID"
#        "DSYApplicationMainArch"
#        "DSYSysCnxExit"
#        "DSYSysDlg"
#        "DSYSysDlgWebView"
#        "DSYSysIRDriver"
#        "DSYSysIRManagerPlus"
#        "DSYSysIRMSysAdapter"
#        "DSYSysIRSendReport00"
#        "DSYSysIRSendReportCom"
#        "DSYSysIRSendReportItfPlugin"
#        "DSYSysProgressHandler"
#        "DSYSysTrayIcon"
#        "DSYSysWatchDogHelp"
#        "DSYSysWatchDogRegisterKeys"
#        "DSYSysWMIDriver"
#        "HTTPArch"
#        "InstArch"
#        "JS0CRYPTEXIT"
#        "JS0DLK"
#        "JS0FM"
#        "JS0GROUP"
#        "JS0PCC"
#        "JS0SMT"
#        "lz4_static"
#        "SecurityContext"
#        "SMAAbuCodeGen"
#        "SMABasCodeGen"
#        "SMASimCodeGen"
#        "StringUtilities"
#        "SysSqlite"
#        "SystemTSUUID"
#        "SystemUUID"
#        "WebView2LoaderStatic"

)
set(AbqSDK_PRIVATE_INCLUDE_DIRS "${AbqSDK_PRIVATE_INCLUDE_DIR}" "${AbqSDK_PRIVATE_PUBLIC_DIR}")


set(FOUND_ABQ_LIB_FULL_PATH)
set(ABQ_LIB_TARGETS)
foreach(LIB ${ABQ_LIB_LIST})
    set(temp${LIB})
    find_library(temp${LIB}
            NAMES
            "${LIB}.lib"
            PATHS
            ${AbqSDK_PRIVATE_LIB}
    )
    mark_as_advanced(temp${LIB})
    list(APPEND FOUND_ABQ_LIB_FULL_PATH ${temp${LIB}})

    if (NOT TARGET AbqSDK::${LIB})
        add_library(AbqSDK::${LIB} UNKNOWN IMPORTED)
        set_target_properties(AbqSDK::${LIB}
                PROPERTIES
                IMPORTED_LOCATION ${temp${LIB}}
                INTERFACE_INCLUDE_DIRECTORIES "${AbqSDK_PRIVATE_INCLUDE_DIRS}")
        list(APPEND ABQ_LIB_TARGETS AbqSDK::${LIB})
    endif ()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AbqSDK
        REQUIRED_VARS
        FOUND_ABQ_LIB_FULL_PATH
        AbqSDK_PRIVATE_INCLUDE_DIR)