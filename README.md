# PULPissimo eDRAM Emulator
This repository contains the result of the OPRECOMP Summer of Code 2019 project of Marco Widmer.

## PULPissimo Port to the Xilinx ZCU104 FPGA Board
This part of the project has already been upstreamed to the PULPissimo repository: [pull request](https://github.com/pulp-platform/pulpissimo/pull/68)

## eDRAM Emulator Integration
In this part of the project, the eDRAM emulator from our DAC paper was integrated into PULPissimo.

### Documentation
* DAC paper about the emulator: [doi](https://doi.org/10.1145/3316781.3317830)
* Presentation about the integration into PULPissimo: [_doc/presentation/presentation.pdf_](doc/presentation/presentation.pdf)
* Manual: [_doc/manual/README.md_](doc/manual/README.md)

### How to Use
* Clone the PULPissimo repository and checkout commit `f33ecbcf17b2c7fdf3796d352ba3a8689603243f`
* Follow the PULPissimo readme to setup your environment and the software SDK
* Apply the RTL patch: `git apply rtl/pulpissimo.patch`
* Follow the PULPissimo readme to generate the FPGA bitstream for your FPGA board
* Transfer the bitstream to the FPGA using Vivado, start OpenOCD and monitor the serial port
* Run the demo application in _apps/demo_ using GDB

### For Developers

#### RTL Overview
* Emulator top level file: `ips/edram_emu/rtl/edram_emu.vhd`
* Wrapper: `ips/pulp_soc/rtl/pulp_soc/edram_emu_wrap.sv` (instanciates both the SRAM and the emulated eDRAM)
* Instantiation: in `ips/pulp_soc/rtl/pulp_soc/pulp_soc.sv`

#### RTL Configuration
* Enable the eDRAM emulator: Define `PULP_EDRAM_EMUL` in `rtl/includes/pulp_soc_defines.sv`
* Configure the number of SRAM / eDRAM banks: Set `NB_L2_CHANNELS` in `rtl/includes/pulp_soc_defines.sv`
* Configure the SRAM bank size: Set `L2_BANK_SRAM_SIZE` in `ips/pulp_soc/rtl/pulp_soc/pulp_soc.sv`
* Configure the eDRAM bank size: Set `L2_BANK_EDRAM_SIZE` in `ips/pulp_soc/rtl/pulp_soc/pulp_soc.sv`

## Contact
The eDRAM emulator project is maintained by the Telecommunications Circuits Laboratory at EPFL.
For questions, please contact [Andreas Burg](https://people.epfl.ch/andreas.burg).
