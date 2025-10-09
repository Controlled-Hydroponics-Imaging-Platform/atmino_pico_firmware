# Atmino pico Firmware

Firmware for the atmino controller within our Controlled Hydroponic Imaging Platform (CHIP)


## Build and installation 

```bash
git clone git@github.com:Controlled-Hydroponics-Imaging-Platform/atmino_pico_firmware.git

cd atmino_pico_firmware

git submodule update --init --recursive --remote

mkdir build && cd build

cmake ..

make
```

then upload the atmino.uf2 file to pico directory