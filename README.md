# Tests for libcsp on Zephyr RTOS

## How to build

```
mkdir workspace
cd workspace
pipenv shell
pip3 install west
west init -m https://github.com/spacecubics/libcsp-zephyr
west update
pip3 install -r zephyr/scripts/requirements-base.txt
west build -b qemu_cortex_m3 libcsp-zephyr
```
