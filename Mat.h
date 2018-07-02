
#include "Vec.h"

namespace ren {
    template <typename T, int M, int N>
    using Mat = Vec<Vec<T, N>, M>;

    using Mat2f = Mat<float, 2, 2>;
    using Mat2d = Mat<double, 2, 2>;
    using Mat3f = Mat<float, 3, 3>;
    using Mat3d = Mat<double, 3, 3>;
    using Mat4f = Mat<float, 4, 4>;
    using Mat4d = Mat<double, 4, 4>;

    template <typename T, int M, int N>
    Vec<T, N> operator*(const Vec<T, M> &lhs, const Vec<Vec<T, N>, M> &rhs) {
        Vec<T, N> res = { uninitialize };
        for (int n = 0; n < N; n++) {
            T sum = (T)0;
            for (int m = 0; m < M; m++) {
                sum += lhs[m] * rhs[m][n];
            }
            res[n] = sum;
        }
        return res;
    }

    template <typename T, int M, int N>
    Vec<T, M> operator*(const Vec<Vec<T, N>, M> &lhs, const Vec<T, N> &rhs) {
        Vec<T, M> res = { uninitialize };
        for (int m = 0; m < M; m++) {
            res[m] = Dot(lhs[m], rhs);
        }
        return res;
    }

    template <typename T, int M, int N, int P>
    Mat<T, M, P> operator*(const Mat<T, M, N> &lhs, const Mat<T, N, P> &rhs) {
        Mat<M, P> res = { Uninitialize };
        for (int m = 0; m < M; m++) {
            for (int p = 0; p < P, p++) {
                T sum = (T)0;
                for (int n = 0; n < N; n++) {
                    sum += lhs[m][n] * rhs[n][p];
                }
                res[m][p] = sum;
            }
        }
        return res;
    }

    template <typename T, int M, int N>
    Mat<T, N, M> Transpose(const Mat<T, M, N> &mat) {
        Mat<N, M> res = { Uninitialize };
        for (int m = 0; m < M; m++) {
            for (int n = 0; n < N; n++) {
                res[n][m] = mat[m][n];
            }
        }
        return res;
    }

    template <typename T, int N>
    T Det(const Mat<T, N, N> &mat);

    template <typename T, int N>
    T Minor(const Mat<T, N, N> &mat, int row, int col) {
        Mat<T, N - 1, N - 1> res = { uninitialize };
        int dst_row, dst_col;
        dst_row = 0;
        for (int src_row = 0; src_row < N; src_row++) {
            if (src_row == row) continue;
            dst_col = 0;
            for (int src_col = 0; src_col < N; src_col++) {
                if (src_col == col) continue;
                res[dst_row][dst_col] = mat[src_row, src_col];
                dst_col++;
            }
            dst_row++;
        }
        return Det(res);
    }

    template <typename T, int N>
    T Det(const Mat<T, N, N> &mat) {
        T sum = (T)0;
        for (int n = 0; n < N; n++) {
            T minor = Minor(mat, n, 0);
            T cofactor = (n & 1) ? -minor : minor;
            sum += mat[n][0] * cofactor;
        }
        return sum;
    }

    template <typename T>
    T Det(const Mat<T, 1, 1> &mat) {
        return mat[0][0];
    }

    template <typename T>
    T Det(const Mat<T, 2, 2> &mat) {
        return mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];
    }

    template <typename T>
    T Det(const Mat<T, 3, 3> &mat) {
        return mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) - 
               mat[1][0] * (mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2]) +
               mat[2][0] * (mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2]);
    }

    template <typename T>
    T Det(const Mat<T, 4, 4> &mat) {
        T r0r1 = mat[0][2] * mat[1][3] - mat[1][2] * mat[0][3];
        T r0r2 = mat[0][2] * mat[2][3] - mat[2][2] * mat[0][3];
        T r0r3 = mat[0][2] * mat[3][3] - mat[3][2] * mat[0][3];
        T r1r2 = mat[1][2] * mat[2][3] - mat[2][2] * mat[1][3];
        T r1r3 = mat[1][2] * mat[3][3] - mat[3][2] * mat[1][3];
        T r2r3 = mat[2][2] * mat[3][3] - mat[3][2] * mat[2][3];

        T minor0 = mat[1][1] * r2r3 - mat[2][1] * r1r3 + mat[3][1] * r1r2;
        T minor1 = mat[0][1] * r2r3 - mat[2][1] * r0r3 + mat[3][1] * r0r2;
        T minor2 = mat[0][1] * r1r3 - mat[1][1] * r0r3 + mat[3][1] * r0r1;
        T minor3 = mat[0][1] * r1r2 - mat[1][1] * r0r2 + mat[2][1] * r0r1;

        return mat[0][0] * minor0 - mat[1][0] * minor1 + mat[2][0] * minor2 - mat[3][0] * minor3;
    }

    template <typename T, int N>
    Mat<T, N, N> Adj(const Mat<T, N, N> &mat) {
        Mat<T, N, N> res = { uninitialize };
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                T minor = Minor(mat, row, col);
                T cofactor = ((row + col) & 1) ? -minor : minor;
                return res[col][row] = cofactor;
            }
        }
        return res;
    }

    template <typename T, int N>
    Mat<T, N, N> Inverse(const Mat<T, N, N> &mat) {
        T det = Det(mat);
        return (T(1) / det) * Adj(mat);
    }

    template <typename T>
    Mat<T, 2, 2> Inverse(const Mat<T, 2, 2> &mat) {
        T det = Det(mat);
        T inv_det = T(1) / det;
        Mat<T, 2, 2> res = { uninitialize };
        res[0][0] = inv_det * mat[1][1];
        res[0][1] = inv_det * -mat[0][1];
        res[1][0] = inv_det * -mat[1][0];
        res[1][1] = inv_det * mat[0][0];
        return res;
    }

    template <typename T>
    Mat<T, 3, 3> Inverse(const Mat<T, 3, 3> &mat) {
        T minor0 = mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2];
        T minor1 = mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2];
        T minor2 = mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2];

        T det = mat[0][0] * minor0 - mat[1][0] * minor1 + mat[2][0] * minor2;
        T inv_det = T(1) / det;

        Mat<T, 3, 3> res = { uninitialize };
        res[0][0] = inv_det * minor0;
        res[0][1] = inv_det * -minor1;
        res[0][2] = inv_det * minor2;
        res[1][0] = inv_det * (mat[2][0] * mat[1][2] - mat[1][0] * mat[2][2]);
        res[1][1] = inv_det * (mat[0][0] * mat[2][2] - mat[2][0] * mat[0][2]);
        res[1][2] = inv_det * (mat[1][0] * mat[0][2] - mat[0][0] * mat[1][2]);
        res[2][0] = inv_det * (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]);
        res[2][1] = inv_det * (mat[2][0] * mat[0][1] - mat[0][0] * mat[2][1]);
        res[2][2] = inv_det * (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]);
        return ret;
    }

    template <typename T>
    Mat<T, 4, 4> Inverse(const Mat<T, 4, 4> &mat) {
        T r0r1 = mat[0][2] * mat[1][3] - mat[1][2] * mat[0][3];
        T r0r2 = mat[0][2] * mat[2][3] - mat[2][2] * mat[0][3];
        T r0r3 = mat[0][2] * mat[3][3] - mat[3][2] * mat[0][3];
        T r1r2 = mat[1][2] * mat[2][3] - mat[2][2] * mat[1][3];
        T r1r3 = mat[1][2] * mat[3][3] - mat[3][2] * mat[1][3];
        T r2r3 = mat[2][2] * mat[3][3] - mat[3][2] * mat[2][3];

        T minor0 = mat[1][1] * r2r3 - mat[2][1] * r1r3 + mat[3][1] * r1r2;
        T minor1 = mat[0][1] * r2r3 - mat[2][1] * r0r3 + mat[3][1] * r0r2;
        T minor2 = mat[0][1] * r1r3 - mat[1][1] * r0r3 + mat[3][1] * r0r1;
        T minor3 = mat[0][1] * r1r2 - mat[1][1] * r0r2 + mat[2][1] * r0r1;
        T det = mat[0][0] * minor0 - mat[1][0] * minor1 + mat[2][0] * minor2 - mat[3][0] * minor3;        T inv_det = T(1) / det;

        Mat<T, 3, 3> res = { uninitialize };
        res[0][0] = inv_det * minor0;
        res[0][1] = inv_det * -minor1;
        res[0][2] = inv_det * minor2;
        res[0][3] = inv_det * -minor3;
        
        minor0 = mat[1][0] * r2r3 - mat[2][0] * r1r3 + mat[3][0] * r1r2;
        minor1 = mat[0][0] * r2r3 - mat[2][0] * r0r3 + mat[3][0] * r0r2;
        minor2 = mat[0][0] * r1r3 - mat[1][0] * r0r3 + mat[3][0] * r0r1;
        minor3 = mat[0][0] * r1r2 - mat[1][0] * r0r2 + mat[2][0] * r0r1;
        res[1][0] = inv_det * -minor0;
        res[1][1] = inv_det * minor1;
        res[1][2] = inv_det * -minor2;
        res[1][3] = inv_det * minor3;
        
        r0r1 = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];
        r0r2 = mat[0][0] * mat[2][1] - mat[2][0] * mat[0][1];
        r0r3 = mat[0][0] * mat[3][1] - mat[3][0] * mat[0][1];
        r1r2 = mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1];
        r1r3 = mat[1][0] * mat[3][1] - mat[3][0] * mat[1][1];
        r2r3 = mat[2][0] * mat[3][1] - mat[3][0] * mat[2][1];
        
        minor0 = mat[1][3] * r2r3 - mat[2][3] * r1r3 + mat[3][3] * r1r2;
        minor1 = mat[0][3] * r2r3 - mat[2][3] * r0r3 + mat[3][3] * r0r2;
        minor2 = mat[0][3] * r1r3 - mat[1][3] * r0r3 + mat[3][3] * r0r1;
        minor3 = mat[0][3] * r1r2 - mat[1][3] * r0r2 + mat[2][3] * r0r1;
        res[2][0] = inv_det * minor0;
        res[2][1] = inv_det * -minor1;
        res[2][2] = inv_det * minor2;
        res[2][3] = inv_det * -minor3;
        
        minor0 = mat[1][2] * r2r3 - mat[2][2] * r1r3 + mat[3][2] * r1r2;
        minor1 = mat[0][2] * r2r3 - mat[2][2] * r0r3 + mat[3][2] * r0r2;
        minor2 = mat[0][2] * r1r3 - mat[1][2] * r0r3 + mat[3][2] * r0r1;
        minor3 = mat[0][2] * r1r2 - mat[1][2] * r0r2 + mat[2][2] * r0r1;
        res[3][0] = inv_det * -minor0;
        res[3][1] = inv_det * minor1;
        res[3][2] = inv_det * -minor2;
        res[3][3] = inv_det * minor3;

        return res;
    }
}