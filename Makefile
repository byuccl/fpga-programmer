install:
	git submodule init
	git submodule update

	sudo apt install \
		libtool \
		pkg-config \
		texinfo \
		autoconf \
		automake \
		libusb-1.0-0-dev

	cd third_party/openocd && ./bootstrap
	cd third_party/openocd && ./configure --enable-ftdi
	cd third_party/openocd && make