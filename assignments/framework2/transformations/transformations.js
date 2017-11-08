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
    for (var i = 0; i < list.length; i++) {
        if (list[i] < min || min == -1) {
            min = list[i];
            argmin = i; 
        }
    }
    return argmin;
}

function cross_product3D(v, w) {
    return [ v[1] * w[2] - v[2] * w[1],
             v[2] * w[0] - v[0] * w[2],
             v[0] * w[1] - v[1] * w[0] ];
}

function vector_length3D(v) {
    return Math.sqrt(Math.pow(v[0], 2) + Math.pow(v[1], 2) + Math.pow(v[2], 2));
}

function vector_divide(v, value) {
    for (var i = 0; i < v.length; i++) {
        v[i] = v[i] / value;
    }
    return v;
}

function vector_dotproduct3D(v1, v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

function myRotate(angle, x, y, z) {
    // Rotate by angle around [x, y, z]^T.
    //
    // 1. Create the orthonormal basis
    //
    var a = [x, y, z];
    w = vector_divide(a, vector_length3D(a));
    //console.log("Value of w: " + w);
    argmin_w = abs_argmin([Math.abs(w[0]), Math.abs(w[1]), Math.abs(w[2])]);

    //console.log("Argmin of w: " + argmin_w);
    if (argmin_w < 0 || argmin_w >= w.length) {
        //console.log("Error: argmin out of bouds");
        return -1;
    }
    var t = w.slice(); 
    t[argmin_w] = 1;
    //console.log("t: " + t);

    var u = vector_divide(cross_product3D(t, w), vector_length3D(cross_product3D(t, w)));
    //console.log("u: " + u);
    //console.log("w dot u: " + vector_dotproduct3D(w, u));
    

    var v = cross_product3D(w, u);
    //console.log("v: " + v);
    //console.log("v dot u: " + vector_dotproduct3D(v, u));
    
    //
    // 2. Set up the three matrices making up the rotation
    //
    var A = [
        u[0], u[1], u[2], 0.0,
        v[0], v[1], v[2], 0.0,
        a[0], a[1], a[2], 0.0,
        0.0, 0.0, 0.0, 1.0
    ];

    var B = [
             Math.cos(angle), Math.sin(angle), 0.0, 0.0,
             -Math.sin(angle), Math.cos(angle), 0.0, 0.0,
             0.0, 0.0, 1.0, 0.0,
             0.0, 0.0, 0.0, 1.0
            ];

    var C = [    
        u[0], v[0], a[0], 0.0,
        u[1], v[1], a[1], 0.0,
        u[2], v[2], a[2], 0.0,
        0.0, 0.0, 0.0, 1.0
    ];

    var mat = m4.multiply(A, m4.multiply(B, C));
    return mat;
}
