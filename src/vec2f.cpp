#include <cmath>

class Vec2f {
    public:
        float x, y;

        Vec2f(float x, float y) : x(x), y(y) { }
        Vec2f() : x(0), y(0) { }
        float distance(Vec2f other) {
            return (other-(*this)).length();
        }
        float length() {
            sqrt(x*x+y*y);
        }
        Vec2f normal() {
            return (*this)*(1/length());
        }
        Vec2f operator+(const Vec2f &other) {
            return Vec2f(x+other.x, y+other.y);
        }
        void operator+=(const Vec2f &other) {
            x += other.x;
            y += other.y;
        }
        Vec2f operator-(const Vec2f &other) {
            return Vec2f(x-other.x, y-other.y);
        }
        void operator-=(const Vec2f &other) {
            x -= other.x;
            y -= other.y;
        }
        Vec2f operator *(float factor) {
            return Vec2f(x*factor, y*factor);
        }
        void operator *=(float factor) {
            x *= factor;
            y *= factor;
        }
};
