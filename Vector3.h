#include <cmath>
#include <ostream>

class Vector3 {
    private:
        float x, y, z;
    public:
        Vector3() {};

        Vector3(float _x, float _y, float _z) {
            x = _x; y = _y; z = _z;
        }

        friend Vector3 operator +(const Vector3 &v, const Vector3 &w) {
            return Vector3(v.x + w.x, v.y + w.y, v.z + w.z);
        };

        friend Vector3 operator -(const Vector3 &v, const Vector3 &w) {
            return Vector3(v.x - w.x, v.y - w.y, v.z - w.z);
        };

        friend float operator *(const Vector3 &v, const Vector3 &w) {
            return v.x * w.x + v.y * w.y + v.z * w.z;
        };

        friend Vector3 operator *(const Vector3 &v, float s) {
            return Vector3(v.x*s, v.y*s, v.z*s);
        };
        friend Vector3 operator *(float s, const Vector3 &v) {
            return Vector3(v.x*s, v.y*s, v.z*s);
        };

        float norm() {
            return sqrt(x*x + y*y + z*z);
        };

        friend std::ostream& operator<< (std::ostream& out, const Vector3& v) {
            out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
            return out;
        };

        friend class Triangle;
};
