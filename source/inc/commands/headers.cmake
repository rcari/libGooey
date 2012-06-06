# Headers for the Gooey::command namespace

SET (
	Gooey_MOC_HDRS
	${Gooey_MOC_HDRS}
	
	# Nothing
)

SET (
	Gooey_HDRS
	${Gooey_HDRS}
	
	${CMAKE_CURRENT_LIST_DIR}/AddBlock.hpp
	${CMAKE_CURRENT_LIST_DIR}/ChangeBlockProperty.hpp
	${CMAKE_CURRENT_LIST_DIR}/Command.hpp
	${CMAKE_CURRENT_LIST_DIR}/InsertBlock.hpp
	${CMAKE_CURRENT_LIST_DIR}/RemoveBlock.hpp
	${CMAKE_CURRENT_LIST_DIR}/SwapBlocks.hpp
)
