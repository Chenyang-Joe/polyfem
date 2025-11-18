# IPC Toolkit (https://github.com/ipc-sim/ipc-toolkit)
# License: MIT

if(TARGET ipc::toolkit)
    return()
endif()

message(STATUS "Third-party: creating target 'ipc::toolkit'")

# Use the original IPC Toolkit with TBB, but we'll isolate it to prevent threading conflicts
# This is safer than trying to force C++ threads which may not be fully supported
include(CPM)

# # Option to use local GCP-toolkit for development
# set(GCP_TOOLKIT_LOCAL_PATH "/u/1/chenyang/LearnPolyFEM/GCP-toolkit" CACHE PATH "Path to local GCP-toolkit")

# if(EXISTS "${GCP_TOOLKIT_LOCAL_PATH}/CMakeLists.txt")
#     message(STATUS "Using local GCP-toolkit at: ${GCP_TOOLKIT_LOCAL_PATH}")
#     CPMAddPackage(
#         NAME ipc_toolkit
#         SOURCE_DIR "${GCP_TOOLKIT_LOCAL_PATH}"
#     )
# else()
#     message(STATUS "Local GCP-toolkit not found, using remote version")
#     CPMAddPackage("gh:geometryprocessing/GCP-toolkit#7185ecd8b7b46aadae78db688c6029a82a21dcca")
# endif()
CPMAddPackage("gh:geometryprocessing/GCP-toolkit#7185ecd8b7b46aadae78db688c6029a82a21dcca")
