# Sources for namespace Gooey::widgets

SET (
	Gooey_SRCS
	${Gooey_SRCS}
	
	${CMAKE_CURRENT_LIST_DIR}/BlockPanel.cpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyCheckBox.cpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyDoubleSpinBox.cpp
	${CMAKE_CURRENT_LIST_DIR}/GooeyLineEdit.cpp
	${CMAKE_CURRENT_LIST_DIR}/GooeySpinBox.cpp
	${CMAKE_CURRENT_LIST_DIR}/Group.cpp
	${CMAKE_CURRENT_LIST_DIR}/GroupForm.cpp
	${CMAKE_CURRENT_LIST_DIR}/LineGroup.cpp
	${CMAKE_CURRENT_LIST_DIR}/TabWidget.cpp
	${CMAKE_CURRENT_LIST_DIR}/property/BoolPropertyWidget.cpp
	${CMAKE_CURRENT_LIST_DIR}/property/FloatingPointPropertyWidget.cpp
	${CMAKE_CURRENT_LIST_DIR}/property/IntegerPropertyWidget.cpp
	${CMAKE_CURRENT_LIST_DIR}/property/PropertyWidget.cpp
	${CMAKE_CURRENT_LIST_DIR}/property/StringPropertyWidget.cpp
)
