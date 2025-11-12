#pragma once
#include "../OS-ImGui/OS-ImGui_Struct.h"
#include <cmath>

namespace Math
{
    // Calculate angle between two positions
    Vec3 CalcAngle(const Vec3& src, const Vec3& dst)
    {
        Vec3 angle;
        Vec3 delta = dst - src;

        // Calculate the angle in degrees
        angle.x = asinf(delta.z / delta.Length()) * (180.0f / 3.14159265358979323846f);
        angle.y = atan2f(delta.y, delta.x) * (180.0f / 3.14159265358979323846f);
        angle.z = 0.0f;

        return angle;
    }

    // Normalize angles to keep them within valid range
    void NormalizeAngles(Vec3& angle)
    {
        // Normalize pitch
        while (angle.x > 89.0f) angle.x -= 180.0f;
        while (angle.x < -89.0f) angle.x += 180.0f;

        // Normalize yaw
        while (angle.y > 180.0f) angle.y -= 360.0f;
        while (angle.y < -180.0f) angle.y += 360.0f;

        // Roll is usually not used in FPS games
        angle.z = 0.0f;
    }

    // Convert degrees to radians
    float DegToRad(float degrees)
    {
        return degrees * (3.14159265358979323846f / 180.0f);
    }

    // Convert radians to degrees
    float RadToDeg(float radians)
    {
        return radians * (180.0f / 3.14159265358979323846f);
    }

    // Linear interpolation between two values
    float Lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }

    // Clamp value between min and max
    float Clamp(float value, float min, float max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    // Calculate distance between two 3D points
    float Distance(const Vec3& a, const Vec3& b)
    {
        return a.DistanceTo(b);
    }

    // Check if point is within field of view
    bool IsInFOV(const Vec3& viewAngle, const Vec3& targetAngle, float fov)
    {
        Vec3 delta = targetAngle - viewAngle;
        NormalizeAngles(delta);
        
        float distance = sqrtf(delta.x * delta.x + delta.y * delta.y);
        return distance <= fov;
    }
}
