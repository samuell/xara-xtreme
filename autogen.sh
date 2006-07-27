#!/bin/sh

AUTORECONF=${AUTORECONF:-autoreconf}

${AUTORECONF} -f -i
( cd filters/SVGFilter ; ${AUTORECONF} -f -i )
