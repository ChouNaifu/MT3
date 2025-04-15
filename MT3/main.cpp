#include <Novice.h>
#include <math.h>
#include <assert.h>

const char kWindowTitle[] = "GC1B_10_チョウ_ナイーフ_タイトル";

struct Matrix4x4 {
    float m[4][4];
};

// 加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
    Matrix4x4 result{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m1.m[i][j] + m2.m[i][j];
        }
    }
    return result;
}

// 減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
    Matrix4x4 result{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m1.m[i][j] - m2.m[i][j];
        }
    }
    return result;
}

// 積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
    Matrix4x4 result{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += m1.m[i][k] * m2.m[k][j];
            }
        }
    }
    return result;
}

// 逆行列

Matrix4x4 Inverse(const Matrix4x4& m) { 
    Matrix4x4 inverse;

    float a = m.m[0][0], b = m.m[0][1], c = m.m[0][2], d = m.m[0][3];
    float e = m.m[1][0], f = m.m[1][1], g = m.m[1][2], h = m.m[1][3];
    float i = m.m[2][0], j = m.m[2][1], k = m.m[2][2], l = m.m[2][3];
    float m4 = m.m[3][0], n = m.m[3][1], o = m.m[3][2], p = m.m[3][3];

    float determinant =
        a * (f * (k * p - o * l) - g * (j * p - n * l) + h * (j * o - n * k)) -
        b * (e * (k * p - o * l) - g * (i * p - m4 * l) + h * (i * o - m4 * k)) +
        c * (e * (j * p - n * l) - f * (i * p - m4 * l) + h * (i * n - m4 * j)) -
        d * (e * (j * o - n * k) - f * (i * o - m4 * k) + g * (i * n - m4 * j));

    assert(determinant != 0.0f);
    float invDet = 1.0f / determinant;

    inverse.m[0][0] = (f * (k * p - o * l) - g * (j * p - n * l) + h * (j * o - n * k)) * invDet;
    inverse.m[0][1] = -(b * (k * p - o * l) - c * (j * p - n * l) + d * (j * o - n * k)) * invDet;
    inverse.m[0][2] = (b * (g * p - o * h) - c * (f * p - n * h) + d * (f * o - n * g)) * invDet;
    inverse.m[0][3] = -(b * (g * l - k * h) - c * (f * l - j * h) + d * (f * k - j * g)) * invDet;

    inverse.m[1][0] = -(e * (k * p - o * l) - g * (i * p - m4 * l) + h * (i * o - m4 * k)) * invDet;
    inverse.m[1][1] = (a * (k * p - o * l) - c * (i * p - m4 * l) + d * (i * o - m4 * k)) * invDet;
    inverse.m[1][2] = -(a * (g * p - o * h) - c * (e * p - m4 * h) + d * (e * o - m4 * g)) * invDet;
    inverse.m[1][3] = (a * (g * l - k * h) - c * (e * l - i * h) + d * (e * k - i * g)) * invDet;

    inverse.m[2][0] = (e * (j * p - n * l) - f * (i * p - m4 * l) + h * (i * n - m4 * j)) * invDet;
    inverse.m[2][1] = -(a * (j * p - n * l) - b * (i * p - m4 * l) + d * (i * n - m4 * j)) * invDet;
    inverse.m[2][2] = (a * (f * p - n * h) - b * (e * p - m4 * h) + d * (e * n - m4 * f)) * invDet;
    inverse.m[2][3] = -(a * (f * l - j * h) - b * (e * l - i * h) + d * (e * j - i * f)) * invDet;

    inverse.m[3][0] = -(e * (j * o - n * k) - f * (i * o - m4 * k) + g * (i * n - m4 * j)) * invDet;
    inverse.m[3][1] = (a * (j * o - n * k) - b * (i * o - m4 * k) + c * (i * n - m4 * j)) * invDet;
    inverse.m[3][2] = -(a * (f * o - n * g) - b * (e * o - m4 * g) + c * (e * n - m4 * f)) * invDet;
    inverse.m[3][3] = (a * (f * k - j * g) - b * (e * k - i * g) + c * (e * j - i * f)) * invDet;

    return inverse;
}
// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
    Matrix4x4 result{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m.m[j][i];
        }
    }
    return result;
}

// 単位行列
Matrix4x4 MakeIdentity4x4() {
    Matrix4x4 result{};
    for (int i = 0; i < 4; ++i) {
        result.m[i][i] = 1.0f;
    }
    return result;
}

// 数値表示
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
    const int kColumnWidth = 60;
    const int kRowHeight = 20;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            Novice::ScreenPrintf(x + kColumnWidth * j, y + kRowHeight * i, "%.02f", matrix.m[i][j]);
        }
    }
    Novice::ScreenPrintf(x + kColumnWidth * 4, y, "%s", label);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = {0};
    char preKeys[256] = {0};

    Matrix4x4 m1 = {
        3.2f, 0.7f, 9.6f, 4.4f,
        5.5f, 1.3f, 7.8f, 2.1f,
        6.9f, 8.0f, 2.6f, 1.0f,
        0.5f, 7.2f, 5.1f, 3.3f
    };

    Matrix4x4 m2 = {
        4.1f, 6.5f, 3.3f, 2.2f,
        8.8f, 0.6f, 9.9f, 7.7f,
        1.1f, 5.5f, 6.6f, 0.0f,
        3.3f, 9.9f, 8.8f, 2.2f
    };

    Matrix4x4 resultAdd = Add(m1, m2);
    Matrix4x4 resultSubtract = Subtract(m1, m2);
    Matrix4x4 resultMultiply = Multiply(m1, m2);
    Matrix4x4 inverseM1 = Inverse(m1);
    Matrix4x4 inverseM2 = Inverse(m2);
    Matrix4x4 transposeM1 = Transpose(m1);
    Matrix4x4 transposeM2 = Transpose(m2);
    Matrix4x4 identity = MakeIdentity4x4();

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        // キー入力を受け取る
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        ///
        /// ↓更新処理ここから
        ///

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///
        const int kRowHeight = 100;
        MatrixScreenPrintf(0, 0, resultAdd, "Add");
        MatrixScreenPrintf(0, kRowHeight, resultSubtract, "Subtract");
        MatrixScreenPrintf(0, kRowHeight * 2, resultMultiply, "Multiply");
        MatrixScreenPrintf(0, kRowHeight * 3, inverseM1, "InverseM1");
        MatrixScreenPrintf(0, kRowHeight * 4, inverseM2, "InverseM2");
        MatrixScreenPrintf(0, kRowHeight * 5, transposeM1, "TransposeM1");
		MatrixScreenPrintf(0, kRowHeight * 6, transposeM2, "TransposeM2");
        MatrixScreenPrintf(400, 0, identity, "Identity");
        ///
        /// ↑描画処理ここまで
        ///

        // フレームの終了
        Novice::EndFrame();

        // ESCキーが押されたらループを抜ける
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    // ライブラリの終了
    Novice::Finalize();
    return 0;
}