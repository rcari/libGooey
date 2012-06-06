# Sources for namespace Gooey::commands

SET (
	Gooey_SRCS
	${Gooey_SRCS}
	
	${CMAKE_CURRENT_LIST_DIR}/AddBlock.cpp
	${CMAKE_CURRENT_LIST_DIR}/ChangeBlockProperty.cpp
	${CMAKE_CURRENT_LIST_DIR}/Command.cpp
	${CMAKE_CURRENT_LIST_DIR}/InsertBlock.cpp
	${CMAKE_CURRENT_LIST_DIR}/RemoveBlock.cpp
	${CMAKE_CURRENT_LIST_DIR}/SwapBlocks.cpp
)
