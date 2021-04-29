#!/bin/sh

function make_dot_h() {
    name=$(basename $1)
    name=${name%.*}

    out="src/$name.png"
    h_out="src/images/$name.h"
    convert "$1" -background white -flatten -alpha off -colors 16 "$out"
    python ./.pio/libdeps/esp32dev/epd_driver/scripts/imgconvert.py -i "$out" -o "$h_out" -n "$name"
    rm "$out"
}


for fname in images/*.png; do
    echo "$fname..."
    make_dot_h "$fname"
done
