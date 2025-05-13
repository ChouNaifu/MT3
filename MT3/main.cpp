#include <Novice.h>
#include <math.h>
#include <assert.h>

const char kWindowTitle[] = "GC1B_10_チョウ_ナイーフ_タイトル";

struct Vector3 {
    float x, y, z;
};

struct Matrix4x4 {
    float m[4][4];
};

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
    Matrix4x4 result = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        translate.x, translate.y, translate.z, 1.0f
    };
    return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
    Matrix4x4 result = {
        scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return result;
}

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
    Vector3 result;
    float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
    assert(w != 0.0f);

    result.x = (vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0]) / w;
    result.y = (vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1]) / w;
    result.z = (vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2]) / w;

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

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
    const int kColumnWidth = 60;
    Novice::ScreenPrintf(x, y, "%.02f", vector.x);
    Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
    Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
    Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = {0};
    char preKeys[256] = {0};

    // 初期化
    Vector3 translate{ 4.1f, 2.6f, 0.8f };
    Vector3 scale{ 1.5f, 5.2f, 7.3f };
    Vector3 point{ 2.3f, 3.8f, 1.4f };

    Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

    Matrix4x4 transformMatrix = {
        1.0f, 2.0f, 3.0f, 4.0f,
        3.0f, 1.0f, 1.0f, 2.0f,
        1.0f, 4.0f, 2.0f, 3.0f,
        2.0f, 2.0f, 1.0f, 3.0f
    };

    Vector3 transformed = Transform(point, transformMatrix);

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
        const int kRowHeight = 20;
        VectorScreenPrintf(0, 0, transformed, "transformed");
        MatrixScreenPrintf(0, kRowHeight * 2, translateMatrix, "translateMatrix");
        MatrixScreenPrintf(0, kRowHeight * 6, scaleMatrix, "scaleMatrix");
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