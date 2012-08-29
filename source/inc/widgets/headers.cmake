# Headers for the Gooey::widgets namespace

SET (
	Gooey_MOC_HDRS
	${Gooey_MOC_HDRS}
	
	${CMAKE_CURRENT_LIST_DIR}/BlockPanel.hpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyCheckBox.hpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyDoubleSpinBox.hpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyLineEdit.hpp
	${CMAKE_CURRENT_LIST_DIR}/GooeySpinBox.hpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyTabWidget.hpp
	${CMAKE_CURRENT_LIST_DIR}/Group.hpp
	${CMAKE_CURRENT_LIST_DIR}/GroupForm.hpp
	${CMAKE_CURRENT_LIST_DIR}/LineGroup.hpp
	${CMAKE_CURRENT_LIST_DIR}/property/BoolPropertyWidget.hpp
	${CMAKE_CURRENT_LIST_DIR}/property/FloatingPointPropertyWidget.hpp
	${CMAKE_CURRENT_LIST_DIR}/property/IntegerPropertyWidget.hpp
	${CMAKE_CURRENT_LIST_DIR}/property/PropertyWidget.hpp
	${CMAKE_CURRENT_LIST_DIR}/property/StringPropertyWidget.hpp
)

SET (
	Gooey_HDRS
	${Gooey_HDRS}
	
	# Nothing
)
