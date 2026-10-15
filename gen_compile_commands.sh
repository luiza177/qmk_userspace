#!/bin/bash

qmk compile --compiledb -e HLC_CIRQUE_TRACKPAD=1 -kb splitkb/halcyon/kyria/rev4 -km custom_hlc_dualencoders
cp ~/qmk_firmware/compile_commands.json /tmp/cdb_trackpad.json
qmk compile --compiledb -e HLC_TFT_DISPLAY=1 -kb splitkb/halcyon/kyria/rev4 -km custom_hlc_dualencoders
cp ~/qmk_firmware/compile_commands.json /tmp/cdb_display.json
jq -s 'add' /tmp/cdb_display.json /tmp/cdb_trackpad.json >~/qmk_userspace/compile_commands.json
