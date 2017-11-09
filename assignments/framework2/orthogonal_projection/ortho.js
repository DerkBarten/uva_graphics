/*
 * Student names: Derk Barten
 * Student numbers: 11043075
 *
 */

function myOrtho(left, right, bottom, top, near, far) {
    // Create the translation part
    var mat_orth_trans = [
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        -(left + right) / 2.0, -(bottom + top) / 2.0, -(near + far) / 2.0, 1 
    ];

    // Create the scaling part
    var mat_orth_scale = [
        2.0 / (right - left), 0.0, 0.0, 0.0,
        0.0, 2.0 / (top - bottom), 0.0, 0.0,
        0.0, 0.0, 2.0 / (near - far), 0.0,
        0.0, 0.0, 0.0, 1.0
    ];

    // Combine both parts to create the ortagonal projection matrix
    var mat_orth = m4.identity();
    m4.multiply(mat_orth_scale, mat_orth_trans, mat_orth);

    return mat_orth;
}
