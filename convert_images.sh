#!/bin/sh

function make_dot_h() {
    name=$(basename $1)
    name=${name%.*}

    out="src/$name.png"
    h_out="src/images/$name.h"
#    convert input.jpg -remap eink-3color.png -type truecolor BMP3:output.bmp
    magick "$1" -dither FloydSteinberg -define dither:diffusion-amount=100% -background white -flatten -alpha off -remap palette.png -colors 16 "$out"
    python ./imgconvert.py -i "$out" -o "$h_out" -n "$name"
    #rm "$out"
}

if [ -z "$1" ]; then

    for fname in images/wallpaper*.png; do
        echo "$fname..."
        make_dot_h "$fname"
    done
else
    make_dot_h $*
fi
