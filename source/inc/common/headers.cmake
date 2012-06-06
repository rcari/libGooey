# Headers for the Gooey::common namespace

SET (
	Gooey_MOC_HDRS
	${Gooey_MOC_HDRS}
	
	${CMAKE_CURRENT_LIST_DIR}/BlockAction.hpp
	${CMAKE_CURRENT_LIST_DIR}/BlockMenu.hpp
	${CMAKE_CURRENT_LIST_DIR}/BlockModel.hpp
	${CMAKE_CURRENT_LIST_DIR}/BlockModelPrivate.hpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyItemDelegate.hpp
	${CMAKE_CURRENT_LIST_DIR}/LibraryListModel.hpp
	${CMAKE_CURRENT_LIST_DIR}/LibraryListTreeModel.hpp
	${CMAKE_CURRENT_LIST_DIR}/LibraryTreeModel.hpp
	${CMAKE_CURRENT_LIST_DIR}/PropertyWidgetProxy.hpp
)

SET (
	Gooey_HDRS
	${Gooey_HDRS}
	
	${CMAKE_CURRENT_LIST_DIR}/BlockDrag.hpp
	${CMAKE_CURRENT_LIST_DIR}/FlowLayout.hpp
	${CMAKE_CURRENT_LIST_DIR}/LibraryFilterProxyModel.hpp
	${CMAKE_CURRENT_LIST_DIR}/LinkBlockToView.hpp
)

IF (APPLE)
SET (
	Gooey_HDRS
	${Gooey_HDRS}
	
	${CMAKE_CURRENT_LIST_DIR}/GooeyStyle.hpp
)
ENDIF (APPLE)
