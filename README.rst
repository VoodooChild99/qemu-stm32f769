QEMU STM32F769
=============
This is a QEMU fork supporting STM32F69 board emulation.
The simple steps to build this are:

.. code-block:: shell

  mkdir build
  cd build
  cp ../qemu-default-config.sh ./
  chmod +x ./qemu-default-config.sh
  ./qemu-default-config.sh
  make -jNN

By default, AFL system fuzzing option is on, so an AFL repository is required.
The AFL repo is assumed to be located at ../path-to-qemu-repo.

For original QEMU README, refer to README-QEMU.rst :)
