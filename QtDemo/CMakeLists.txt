wps_package(QtDemo EXECUTABLE)
	wps_add_sources(
		QT_AUTOMOC
		main.cpp
		mainwindow.cpp
		mainwindow.h
		mainwindow.ui
	)

	wps_use_packages(
		Qt5Widgets
	)

wps_end_package()
