#pragma once
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

using SDFPtr = float (*)(float, float, float);

/// @brief 単位球SDF
SDFPtr SphereSDF = [](float x, float y, float z) -> float
{
    return x * x + y * y + z * z - 0.5f;
};

/// @brief  Box - exact
SDFPtr BoxSDF = [](float x, float y, float z) -> float
{
    float bx = 0.5f, by = 0.5f, bz = 0.5f; // ボックスサイズ (0.5, 0.5, 0.5)
    float qx = std::abs(x) - bx;
    float qy = std::abs(y) - by;
    float qz = std::abs(z) - bz;
    float maxQ = std::max(std::max(qx, qy), qz);
    return std::sqrt(std::max(qx, 0.0f) * std::max(qx, 0.0f) +
                     std::max(qy, 0.0f) * std::max(qy, 0.0f) +
                     std::max(qz, 0.0f) * std::max(qz, 0.0f)) +
           std::min(maxQ, 0.0f);
};

/// @brief  Round Box - exact
SDFPtr RoundBoxSDF = [](float x, float y, float z) -> float
{
    float bx = 0.5f, by = 0.5f, bz = 0.5f, r = 0.1f;
    float qx = std::abs(x) - bx + r;
    float qy = std::abs(y) - by + r;
    float qz = std::abs(z) - bz + r;
    float maxQ = std::max(std::max(qx, qy), qz);
    return std::sqrt(std::max(qx, 0.0f) * std::max(qx, 0.0f) +
                     std::max(qy, 0.0f) * std::max(qy, 0.0f) +
                     std::max(qz, 0.0f) * std::max(qz, 0.0f)) +
           std::min(maxQ, 0.0f) - r;
};

/// @brief  Box Frame - exact
SDFPtr BoxFrameSDF = [](float x, float y, float z) -> float
{
    float bx = 0.5f, by = 0.5f, bz = 0.5f, e = 0.1f;

    // ポジションをボックスサイズに基づいて調整
    x = std::abs(x) - bx;
    y = std::abs(y) - by;
    z = std::abs(z) - bz;

    // e分広げたボックス
    float qx = std::abs(x + e) - e;
    float qy = std::abs(y + e) - e;
    float qz = std::abs(z + e) - e;

    // 各軸方向におけるフレームのSDFを計算
    float s1 = std::sqrt(
                   std::max(x, 0.0f) * std::max(x, 0.0f) +
                   std::max(qy, 0.0f) * std::max(qy, 0.0f) +
                   std::max(qz, 0.0f) * std::max(qz, 0.0f)) +
               std::min(std::max(x, std::max(qy, qz)), 0.0f);

    float s2 = std::sqrt(
                   std::max(qx, 0.0f) * std::max(qx, 0.0f) +
                   std::max(y, 0.0f) * std::max(y, 0.0f) +
                   std::max(qz, 0.0f) * std::max(qz, 0.0f)) +
               std::min(std::max(qx, std::max(y, qz)), 0.0f);

    float s3 = std::sqrt(
                   std::max(qx, 0.0f) * std::max(qx, 0.0f) +
                   std::max(qy, 0.0f) * std::max(qy, 0.0f) +
                   std::max(z, 0.0f) * std::max(z, 0.0f)) +
               std::min(std::max(qx, std::max(qy, z)), 0.0f);

    // 最小のフレーム距離を返す
    return std::min(std::min(s1, s2), s3);
};

/// @brief  Torus - exact
SDFPtr TorusSDF = [](float x, float y, float z) -> float
{
    float R = 0.5f, r = 0.2f;
    float qx = std::sqrt(x * x + z * z) - R;
    return std::sqrt(qx * qx + y * y) - r;
};

/// @brief  Capped Torus - exact
SDFPtr CappedTorusSDF = [](float x, float y, float z) -> float
{
    float scX = 1.0f, scY = 0.5f, ra = 0.5f, rb = 0.1f;
    x = std::abs(x);
    float k = (scY * x > scX * y) ? (scX * x + scY * y) / (scX * scX + scY * scY) : std::sqrt(x * x + y * y);
    return std::sqrt(x * x + y * y + z * z + ra * ra - 2.0f * ra * k) - rb;
};

/// @brief  Link - exact
SDFPtr LinkSDF = [](float x, float y, float z) -> float
{
    float le = 0.5f, r1 = 0.3f, r2 = 0.1f;
    y = std::max(std::abs(y) - le, 0.0f);
    float qx = std::sqrt(x * x + y * y) - r1;
    return std::sqrt(qx * qx + z * z) - r2;
};

/// @brief  Infinite Cylinder - exact
SDFPtr CylinderSDF = [](float x, float, float z) -> float
{
    float cx = 0.0f, cy = 0.0f, cz = 0.5f;
    return std::sqrt((x - cx) * (x - cx) + (z - cy) * (z - cy)) - cz;
};

/// @brief SDF - Repeating Transforming Shape
SDFPtr RepeatingShapeSDF = [](float x, float y, float z) -> float
{
    float time = 3;
    glm::vec3 p = glm::vec3(x, y, z);
    p.x -= 4.0f;
    p.z += time * 3.0f;
    p = glm::mod(p, glm::vec3(8.0f)) - glm::vec3(4.0f);

    for (int j = 0; j < 3; j++)
    {
        p = glm::vec3(
            glm::abs(p.x),
            glm::abs(p.y),
            glm::abs(p.z));
        glm::vec2 xy = glm::vec2(p.x, p.y) - glm::vec2(0.3f);
        glm::vec2 yz = glm::vec2(p.y, p.z) - glm::vec2(std::sin(time * 2.0f) * 0.3f + 0.1f);
        glm::vec2 xz = glm::vec2(p.x, p.z) - glm::vec2(0.2f);
        p.x = xy.x;
        p.y = xy.y;
        p.y = yz.x;
        p.z = yz.y;
        p.x = xz.x;
        p.z = xz.y;
    }

    return glm::length(glm::cross(p, glm::vec3(0.5f))) - 1.7f;
};

/// @brief 2D回転行列を返す関数
glm::mat2 rot(float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::mat2(c, -s, s, c);
}

/// @brief 2D座標の繰り返し (pmod)
glm::vec2 pmod(glm::vec2 p, float size)
{
    return glm::mod(p + size * 0.5f, size) - size * 0.5f;
}

/// @brief SDF - Repeating Transforming Shape
SDFPtr RepeatingTransformSDF = [](float x, float y, float z) -> float
{
    float time = 3;
    glm::vec3 p = glm::vec3(x, y, z);

    // 回転
    {
        glm::vec2 xy = rot(time * 0.3f) * glm::vec2(p.x, p.y);
        p.x = xy.x;
        p.y = xy.y;
    }
    {
        {
            glm::vec2 xy = pmod(glm::vec2(p.x, p.y), 10.0f);
            p.x = xy.x;
            p.y = xy.y;
        }
        p.y -= 2.0f;
        {
            glm::vec2 yz = pmod(glm::vec2(p.y, p.z), 12.0f);
            p.y = yz.x;
            p.z = yz.y;
        }
        p.z -= 10.0f;

        // 繰り返し処理
        for (int i = 0; i < 4; i++)
        {
            glm::vec2 xy = pmod(glm::vec2(p.x, p.y), 10.0f);
            p.x = xy.x;
            p.y = xy.y;
            p.y -= 2.0f;
            glm::vec2 yz = pmod(glm::vec2(p.y, p.z), 12.0f);
            p.y = yz.x;
            p.z = yz.y;
            p.z -= 10.0f;
        }

        // SDF計算
        return glm::dot(glm::abs(p), glm::normalize(glm::vec3(13.0f, 1.0f, 7.0f))) - 7.f;
    }
};
