for (i = 0; i < N; i += 8) {
    for (j = 0; j < N; j += 8) {
        if (i == j) {
            for (k = i + 7; k >= i; k--) {
                t0 = A[k][j], t1 = A[k][j + 1], t2 = A[k][j + 2], t3 = A[k][j + 3];
                t4 = A[k][j + 4], t5 = A[k][j + 5], t6 = A[k][j + 6], t7 = A[k][j + 7];
                B[k][j] = t0, B[k][j + 1] = t1, B[k][j + 2] = t2, B[k][j + 3] = t3;
                B[k][j + 4] = t4, B[k][j + 5] = t5, B[k][j + 6] = t6, B[k][j + 7] = t7;
                for (l = k + 1; l < i + 8; l++) {
                    t0 = B[k][l], t1 = B[l][k], B[k][l] = t1, B[l][k] = t0;
                }
            }
        } else if (N == 32) {
            for (k = i; k < i + 8; k++) {
                t0 = A[k][j], t1 = A[k][j + 1], t2 = A[k][j + 2], t3 = A[k][j + 3];
                t4 = A[k][j + 4], t5 = A[k][j + 5], t6 = A[k][j + 6], t7 = A[k][j + 7];
                B[j][k] = t0, B[j + 1][k] = t1, B[j + 2][k] = t2, B[j + 3][k] = t3;
                B[j + 4][k] = t4, B[j + 5][k] = t5, B[j + 6][k] = t6, B[j + 7][k] = t7;
            }
        } else {
            for (k = i; k < i + 4; k++) {
                t0 = A[k][j], t1 = A[k][j + 1], t2 = A[k][j + 2], t3 = A[k][j + 3];
                t4 = A[k][j + 4], t5 = A[k][j + 5], t6 = A[k][j + 6], t7 = A[k][j + 7];
                B[j][k] = t0, B[j + 1][k] = t1, B[j + 2][k] = t2, B[j + 3][k] = t3;
                B[j][k + 4] = t4, B[j + 1][k + 4] = t5, B[j + 2][k + 4] = t6, B[j + 3][k + 4] = t7;
            }
            for (k = j; k < j + 4; k++) {
                t0 = B[k][i + 4], t1 = B[k][i + 5], t2 = B[k][i + 6], t3 = B[k][i + 7];
                B[k + 4][i] = t0, B[k + 4][i + 1] = t1, B[k + 4][i + 2] = t2, B[k + 4][i + 3] = t3;
                t4 = A[i + 4][k], t5 = A[i + 5][k], t6 = A[i + 6][k], t7 = A[i + 7][k];
                B[k][i + 4] = t4, B[k][i + 5] = t5, B[k][i + 6] = t6, B[k][i + 7] = t7;
                t0 = A[i + 4][k + 4], t1 = A[i + 5][k + 4], t2 = A[i + 6][k + 4], t3 = A[i + 7][k + 4];
                B[k + 4][i + 4] = t0, B[k + 4][i + 5] = t1, B[k + 4][i + 6] = t2, B[k + 4][i + 7] = t3;
            }
        }
    }
}
