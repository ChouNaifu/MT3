#define _USE_MATH_DEFINES
#include <Novice.h>
#include <cmath>
#include <assert.h>
#include <imgui.h>
#include <iostream>
#include <algorithm>

const char kWindowTitle[] = "GC2A_07";

struct Matrix4x4 {
	float m[4][4];
};

struct Vector3 {
	float x, y, z;
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct Segment {
	Vector3 origin;
	Vector3 diff; 
};

Vector3 Add(const Vector3& v1, const Vector3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vector3 Multiply(const Vector3& v, float s) {
	return { v.x * s, v.y * s, v.z * s };
}

float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

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

Matrix4x4 MakeLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up) {
	Vector3 zaxis = Subtract(target, eye);
	float len = std::sqrt(Dot(zaxis, zaxis));
	zaxis = Multiply(zaxis, 1.0f / len); // normalize

	Vector3 xaxis = Cross(up, zaxis);
	len = std::sqrt(Dot(xaxis, xaxis));
	xaxis = Multiply(xaxis, 1.0f / len);

	Vector3 yaxis = Cross(zaxis, xaxis);

	Matrix4x4 result{};
	result.m[0][0] = xaxis.x; result.m[0][1] = yaxis.x; result.m[0][2] = zaxis.x; result.m[0][3] = 0.0f;
	result.m[1][0] = xaxis.y; result.m[1][1] = yaxis.y; result.m[1][2] = zaxis.y; result.m[1][3] = 0.0f;
	result.m[2][0] = xaxis.z; result.m[2][1] = yaxis.z; result.m[2][2] = zaxis.z; result.m[2][3] = 0.0f;
	result.m[3][0] = -Dot(xaxis, eye); result.m[3][1] = -Dot(yaxis, eye); result.m[3][2] = -Dot(zaxis, eye); result.m[3][3] = 1.0f;
	return result;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	unsigned int color = 0x696969FF;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	// Z方向
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + kGridEvery * xIndex;
		Vector3 start = { x, 0.0f, -kGridHalfWidth };
		Vector3 end = { x, 0.0f,  kGridHalfWidth };
		Vector3 ndcStart = Transform(start, viewProjectionMatrix);
		Vector3 ndcEnd = Transform(end, viewProjectionMatrix);
		Vector3 screenStart = Transform(ndcStart, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEnd, viewportMatrix);
		if (xIndex == 0) {
			color = RED;
		} else if (xIndex == 5) {
			color = BLACK;
		} else if (xIndex == kSubdivision) {
			color = BLUE;
		} else {
			color = 0x696969FF;
		}
		Novice::DrawLine(
			int(screenStart.x), int(screenStart.y),
			int(screenEnd.x), int(screenEnd.y),
			color
		);
	}
	// X方向
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + kGridEvery * zIndex;
		Vector3 start = { -kGridHalfWidth, 0.0f, z };
		Vector3 end = { kGridHalfWidth, 0.0f, z };
		Vector3 ndcStart = Transform(start, viewProjectionMatrix);
		Vector3 ndcEnd = Transform(end, viewProjectionMatrix);
		Vector3 screenStart = Transform(ndcStart, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEnd, viewportMatrix);
		if (zIndex == 0) {
			color = RED;
		} else if (zIndex == 5) {
			color = BLACK;
		} else if (zIndex == kSubdivision) {
			color = BLUE;
		} else {
			color = 0x696969FF;
		}
		Novice::DrawLine(
			int(screenStart.x), int(screenStart.y),
			int(screenEnd.x), int(screenEnd.y),
			color
		);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = 2.0f * float(M_PI) / float(kSubdivision);
	const float kLatEvery = float(M_PI) / float(kSubdivision);

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			Vector3 a = {
				sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon),
				sphere.center.y + sphere.radius * std::sin(lat),
				sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon)
			};
			Vector3 b = {
				sphere.center.x + sphere.radius * std::cos(lat + kLatEvery) * std::cos(lon),
				sphere.center.y + sphere.radius * std::sin(lat + kLatEvery),
				sphere.center.z + sphere.radius * std::cos(lat + kLatEvery) * std::sin(lon)
			};
			Vector3 c = {
				sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon + kLonEvery),
				sphere.center.y + sphere.radius * std::sin(lat),
				sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon + kLonEvery)
			};
			Vector3 aScreen = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 bScreen = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 cScreen = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color);
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(cScreen.x), int(cScreen.y), color);
		}
	}
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float d = Dot(v2, v2);
	assert(d != 0.0f);
	float t = Dot(v1, v2) / d;
	return Multiply(v2, t);
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 segToPoint = Subtract(point, segment.origin);
	float t = Dot(segToPoint, segment.diff) / Dot(segment.diff, segment.diff);
	t = std::clamp(t, 0.0f, 1.0f);
	return Add(segment.origin, Multiply(segment.diff, t));
}

bool IsCollided(const Sphere& s1, const Sphere& s2) {
	Vector3 diff = Subtract(s1.center, s2.center);
	float distSq = Dot(diff, diff);
	float rSum = s1.radius + s2.radius;
	return distSq <= rSum * rSum;
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
#pragma region CameraWork
	Vector3 rotate{};
	Vector3 translate{};
	Vector3 gridCenter{ 0.0f, 0.0f, 0.0f };
	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

#pragma endregion

	Sphere sphere1{ {0.0f, 1.0f, 0.0f}, 1.0f };
	Sphere sphere2{ {1.5f, 1.0f, 0.0f}, 1.0f };

	bool isHit = IsCollided(sphere1, sphere2);
	

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
		
		Vector3 cameraOffset = cameraTranslate;

		Matrix4x4 rotY = MakeRotateYMatrix(cameraRotate.y);
		Matrix4x4 rotX = MakeRotateXMatrix(cameraRotate.x);
		Matrix4x4 rot = Multiply(rotY, rotX);
		Vector3 rotatedOffset = Transform(cameraOffset, rot);

		Vector3 cameraPosition = Add(gridCenter, rotatedOffset);
		isHit = IsCollided(sphere1, sphere2);

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = MakeLookAtMatrix(cameraPosition, gridCenter, { 0.0f, 1.0f, 0.0f });
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);
		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Sphere1 Center", &sphere1.center.x, 0.01f);
		ImGui::DragFloat("Sphere1 Radius", &sphere1.radius, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat3("Sphere2 Center", &sphere2.center.x, 0.01f);
		ImGui::DragFloat("Sphere2 Radius", &sphere2.radius, 0.01f, 0.01f, 10.0f);
		ImGui::End();
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		DrawSphere(sphere1, worldViewProjectionMatrix, viewportMatrix, isHit ? RED : WHITE);
		DrawSphere(sphere2, worldViewProjectionMatrix, viewportMatrix, WHITE);

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