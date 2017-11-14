/*
 * Student Names: Derk Barten
 * Student Numbers: 11043075
 *
 */

// Normalize a 3D vector
function vector_norm3D(v){
    var len = vector_length3D(v);
    v[0] = v[0] / len;
    v[1] = v[1] / len;
    v[2] = v[2] / len;
    return v;
}

// Compute the cross product for 3D vectors
function cross_product3D(v, w) {
    return [ v[1] * w[2] - v[2] * w[1],
             v[2] * w[0] - v[0] * w[2],
             v[0] * w[1] - v[1] * w[0] ];
}

// Compute the dotproduct of 3D vectors
function vector_dotproduct3D(v1, v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

// Multiply a vector with a scalar
function vector_scale(v, s) {
    for (var i = 0; i < v.length; i++) {
        v[i] = v[i] * s;
    }
}

// Compute the vector length for 3D vectors
function vector_length3D(v) {
    return Math.sqrt(Math.pow(v[0], 2) + Math.pow(v[1], 2) + Math.pow(v[2], 2));
}

function myLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ) {

    // Translate everything relative to the camera
    var translate = [
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        -eyeX, -eyeY, -eyeZ, 1.0
    ];

    // cz is the vector pointing  from the camera to the center
    var cz = vector_norm3D([centerX - eyeX, centerY - eyeY, centerZ - eyeZ]);

    // cx is ortagonal to the vector up and cz
    var cx = vector_norm3D(cross_product3D(cz, [upX, upY, upZ]));

    // cy is ortogonal to cx and cz
    var cy = cross_product3D(cx, cz);

    // Checks to see if the basis is orthagonal
    /*
    console.debug("cx: " + cx);
    console.debug("cy: " + cy);
    console.debug("cz: " + cz);

    console.debug("cx length: " + vector_length3D(cx));
    console.debug("cy length: " + vector_length3D(cy));
    console.debug("cz length: " + vector_length3D(cz));

    console.debug("cx dot cy " + vector_dotproduct3D(cx, cy));
    console.debug("cx dot cz " + vector_dotproduct3D(cx, cz));
    console.debug("cy dot cz " + vector_dotproduct3D(cy, cz));
    */

    var R_inv = [
        cx[0], cy[0], -cz[0], 0.0,
        cx[1], cy[1], -cz[1], 0.0,
        cx[2], cy[2], -cz[2], 0.0,
        0.0, 0.0, 0.0, 1
    ];
    
    mat = m4.identity();
    m4.multiply(R_inv, translate, mat);

    return mat;
}
