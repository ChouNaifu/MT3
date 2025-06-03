#include <Novice.h>
#include<cmath>
#include <assert.h>

const char kWindowTitle[] = "GC2A_07";

struct Matrix4x4 {
	float m[4][4];
};

struct Vector3 {
	float x, y, z;
};

// 1. 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
    Matrix4x4 result{};
    float f = 1.0f / std::tan(fovY / 2.0f);
    result.m[0][0] = f / aspectRatio;
    result.m[1][1] = f;
    result.m[2][2] = farClip / (farClip - nearClip);
    result.m[2][3] = 1.0f;
    result.m[3][2] = -nearClip * farClip / (farClip - nearClip);
    return result;
}

// 2. 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
    Matrix4x4 result{};
    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = 1.0f / (farClip - nearClip);
    result.m[3][0] = (left + right) / (left - right);
    result.m[3][1] = (top + bottom) / (bottom - top);
    result.m[3][2] = nearClip / (nearClip - farClip);
    result.m[3][3] = 1.0f;
    return result;
}

// 3. ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
    Matrix4x4 result{};
    result.m[0][0] = width / 2.0f;
    result.m[1][1] = -height / 2.0f;
    result.m[2][2] = maxDepth - minDepth;
    result.m[3][0] = left + width / 2.0f;
    result.m[3][1] = top + height / 2.0f;
    result.m[3][2] = minDepth;
    result.m[3][3] = 1.0f;
    return result;
}

#pragma region Transformations
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,std::cos(radian),std::sin(radian),0.0f,
		0.0f,-std::sin(radian),std::cos(radian),0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
};

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = {
		std::cos(radian),0.0f,-std::sin(radian),0.0f,
		0.0f,1.0f,0.0f,0.0f,
		std::sin(radian),0.0f,std::cos(radian),0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
};

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = {
			std::cos(radian),std::sin(radian),0.0f,0.0f,
			-std::sin(radian),std::cos(radian),0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f
	};
	return result;
};


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

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	assert(w != 0.0f);

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translate.x, translate.y, translate.z, 1.0f
	};
	return result;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = {
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result;

	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);

	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	result = Multiply(Multiply(scaleMatrix, rotateXYZMatrix), translateMatrix);

	return result;

};
#pragma endregion

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "% .02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label)
{
	for (int row = 0; row < 4; ++row)
	{
		for (int columm = 0; columm < 4; ++columm)
		{
			Novice::ScreenPrintf(
				x + columm * kColumnWidth, y + (row+1) * kRowHeight, "%6.02f", matrix.m[row][columm]);
		}
	}
	Novice::ScreenPrintf(x, y, "%s", label);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 rotate{};
	Vector3 translate{};

	Vector3 cameraPosition{ 0.0f, 0.0f, -10.0f };

	Vector3 v1{ 1.2f, -3.9f, 2.5f };
	Vector3 v2{ 2.8f, 0.4f, -1.3f };
	Vector3 cross = Cross(v1, v2);

	// 三角形ローカル座標
	Vector3 kLocalVertices[3] = {
		{0.0f, 0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{-0.5f, -0.5f, 0.0f}
	};

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

		if (keys[DIK_W]) { translate.z += 0.1f; }
		if (keys[DIK_S]) { translate.z -= 0.1f; }
		if (keys[DIK_A]) { translate.x -= 0.1f; }
		if (keys[DIK_D]) { translate.x += 0.1f; }
		rotate.y += 0.02f;

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		Vector3 screenVertices[3];
		for (int i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::DrawTriangle(
			int(screenVertices[0].x), int(screenVertices[0].y),
			int(screenVertices[1].x), int(screenVertices[1].y),
			int(screenVertices[2].x), int(screenVertices[2].y),
			RED, kFillModeSolid);

		VectorScreenPrintf(0, 0, cross, "Cross");
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