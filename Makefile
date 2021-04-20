
EXECUTABLES_REQUIRED = make qtconfig clang-format zip cmake 
EXECUTABLES_OPTIONAL = arm-none-eabi-gcc openocd

require:
	@echo "Checking that required programs are installed..."
	@for exe in $(EXECUTABLES_REQUIRED); do \
		command -v $$exe >/dev/null || (echo "ERROR: $$exe is not installed."; exit 1);\
	done
	@for exe in $(EXECUTABLES_OPTIONAL); do \
		command -v $$exe >/dev/null || (echo "WARNING: $$exe is not installed. This is required if you want to run on a physical board.");\
	done
	@echo "Done."

setup: require
	git submodule init
	git submodule update

install:
# Update package repositories
	sudo apt update

# Install these packages using apt
	sudo apt install -y \
		make \
		python3-pip \
		build-essential \
		qt5-default \
		clang-format \
		zip \
		python3-pip \
		x11-apps 

# Remove old cmake package from apt
	sudo apt purge -y cmake 

# Install newer cmake package from pip
	sudo pip3 install --upgrade cmake