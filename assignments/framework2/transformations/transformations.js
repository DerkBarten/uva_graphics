/*
 * Student names: Derk Barten
 * Student numbers: 11043075
 *
 */

function myTranslate(x, y,  z) {
    // Translate by x, y and z.
    var mat = [
               1.0, 0.0, 0.0, 0.0,
               0.0, 1.0, 0.0, 0.0,
               0.0, 0.0, 1.0, 0.0,
               x, y, z, 1.0
              ];

   return mat;
}

function myScale(x, y, z) {
    // Scale by x, y and z.
    var mat = [
               x, 0.0, 0.0, 0.0,
               0.0, y, 0.0, 0.0,
               0.0, 0.0, z, 0.0,
               0.0, 0.0, 0.0, 1.0
              ];

   return mat;
}

// Undefined behaviour when list contains negative numbers
function abs_argmin(list) {
    var min = -1;
    var argmin = -1;
    for (var i = 0; i < length(list); i++) {
        if (list[i] < min|| list[i] == -1) {
            min = list[i];
            argmin = i; 
        }
    }
    return argmin;
}

function cross_product3D(a, b) {
    return [ a[1] * b[2] - a[2] * b[1],
             a[2] * b[0] - a[0] * b[2],
             a[0] * b[1] - a[1] * b[0] ]
}

function vector_length3D(v) {
    return Math.sqrt(Math.pow(v[0], 2) + Math.pow(v[1], 2) + Math.pow(v[2], 2))
}

function myRotate(angle, x, y, z) {
    // Rotate by angle around [x, y, z]^T.
    //
    // 1. Create the orthonormal basis
    //
    var a = [x, y, z];
    a = a / vector_length3D(a);
    argmin_a = abs_argmin([abs(a[0]), abs(a[1]), abs(a[2])]);

    if (argmin_a < 0 || argmin_a >= length(a)) {
        return -1;
    }
    var temp = a; 
    temp[argmin_a] = 1;

    var u = cross_product3D(tmp, a) / vector_length3D(cross_product3D(tmp, a));
    var v = cross_product3D(a, u);
    //
    // 2. Set up the three matrices making up the rotation
    //
    var A = [
             u[0], v[0], a[0], 0.0,
             u[1], v[1], a[1], 0.0,
             u[2], v[2], a[2], 0.0,
             0.0, 0.0, 0.0, 1.0
            ];

    var B = [
             Math.cos(angle), Math.sin(angle), 0.0, 0.0,
             -Math.sin(angle), Math.cos(angle), 0.0, 0.0,
             0.0, 0.0, 1.0, 0.0,
             0.0, 0.0, 0.0, 1.0
            ];

    var C = [
             u[0], u[1], u[2], 0.0,
             v[0], v[1], v[2], 0.0,
             a[0], a[1], a[2], 0.0,
             0.0, 0.0, 0.0, 1.0
            ];


    var mat = m4.multiply(A, m4.multiply(B, C));
    return mat;
}
