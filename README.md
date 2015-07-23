# lthread

It's like pthread, but instead of even trying to be POSIX compliant, it targets
only (reasonable new) Linux kernels. Furthermore, this project is aimed to aid
anyone wanting to learn about synchronization primitives. You can think of the
`L` in `lthread` standing for Linux or Learning.

# Compiling

	mkdir bin
	cd bin
	cmake ..
	cd ..
	make -Cbin

# Running tests

    make -Cbin test
