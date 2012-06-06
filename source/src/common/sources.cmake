# Sources for namespace Gooey::common

SET (
	Gooey_SRCS
	${Gooey_SRCS}
	
	${CMAKE_CURRENT_LIST_DIR}/BlockAction.cpp
	${CMAKE_CURRENT_LIST_DIR}/BlockDrag.cpp
	${CMAKE_CURRENT_LIST_DIR}/BlockMenu.cpp
	${CMAKE_CURRENT_LIST_DIR}/BlockModel.cpp
	${CMAKE_CURRENT_LIST_DIR}/FlowLayout.cpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyItemDelegate.cpp
	${CMAKE_CURRENT_LIST_DIR}/LibraryFilterProxyModel.cpp
	${CMAKE_CURRENT_LIST_DIR}/LibraryListModel.cpp
	${CMAKE_CURRENT_LIST_DIR}/LibraryListTreeModel.cpp
	${CMAKE_CURRENT_LIST_DIR}/LibraryTreeModel.cpp
	${CMAKE_CURRENT_LIST_DIR}/LinkBlockToView.cpp
	${CMAKE_CURRENT_LIST_DIR}/PropertyWidgetProxy.cpp
)

IF (APPLE)
SET (
	Gooey_SRCS
	${Gooey_SRCS}
	
	${CMAKE_CURRENT_LIST_DIR}/GooeyStyle.cpp
)
ENDIF (APPLE)
