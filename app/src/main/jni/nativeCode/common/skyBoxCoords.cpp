#include <GLES2/gl2.h>
#include "skyBoxCoords.h"

//
// Created by Bartosz Lach on 17.01.2018.
//


GLfloat *skyBoxCoords::getVertices() {
    return VERTICES;
}

GLfloat *skyBoxCoords::getCoords() {
    return COORDS;
}
