#ifndef R2GEOM_H
#define R2GEOM_H

#include <vector>
#include <utility>
#include <iostream>
#include <cmath>
#include "pi.h"

using std::size_t;

const double R2_EPSILON = 1e-8;
const int MAX_INTERPOLATION_RAYS = 256;

// Predefinitions
class Bitmask;
class I2Contour;
class R2Contour;

template <class V = double> class G2Vector {
public:
    V x;
    V y;

    G2Vector(V px = V(), V py = V()):
        x(px),
        y(py)
    {}

    G2Vector<V> operator+(const G2Vector<V>& v) const {
        return G2Vector<V>(x + v.x, y + v.y);
    }

    G2Vector<V>& operator+=(const G2Vector<V>& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    G2Vector<V> operator-(const G2Vector<V>& v) const {
        return G2Vector<V>(x - v.x, y - v.y);
    }

    G2Vector<V> operator-() const {     // Unary minus
        return G2Vector<V>(-x, -y);
    }

    G2Vector<V>& operator-=(const G2Vector<V>& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    G2Vector<V> operator*(V c) const {
        return G2Vector<V>(x*c, y*c);
    }

    G2Vector<V>& operator*=(V c) {
        x *= c;
        y *= c;
        return *this;
    }

    V operator*(const G2Vector<V>& v) const { // Dot product
        return (x*v.x + y*v.y);
    }

    V signedArea(const G2Vector<V>& v) const {
        return (x*v.y - v.x*y);
    }

    V area(const G2Vector<V>& v) const {
        V s = x*v.y - v.x*y;
        if (s < 0)
            s = (-s);
        return s;
    }

    bool operator==(const G2Vector<V>& v) const {
        return (x == v.x && y == v.y);
    }

    bool operator!=(const G2Vector<V>& v) const {
        return !(*this == v);
    }

    bool operator<=(const G2Vector<V>& v) const {
        return (y < v.y || (y <= v.y && x <= v.x));
    }

    bool operator>(const G2Vector<V>& v) const {
        return !(*this <= v);
    }

    bool operator<(const G2Vector<V>& v) const {
        return (y < v.y || (y <= v.y && x < v.x));
    }

    bool operator>=(const G2Vector<V>& v) const {
        return !(*this < v);
    }

    double norm() const {
        return sqrt(double(x)*double(x) + double(y)*double(y));
    }

    double norm2() const {
        return (double(x)*double(x) + double(y)*double(y));
    }

    G2Vector<V> normal() const {
        return G2Vector<V>(-y, x);
    }

    G2Vector<V> normalized() const {
        double l = norm();
        if (l <= 0.)
            return *this;
        return G2Vector<V>(V(x/l), V(y/l));
    }

    G2Vector<V>& normalize() const {
        double l = norm();
        if (l > 0.) {
            x = V(x/l);
            y = V(y/l);
        }
        return *this;
    }

    double angle(const G2Vector<V>& v) const {
        G2Vector<V> n = normal();
        double x = double(v*(*this));
        double y = double(v*n);
        return atan2(y, x);
    }
};

template <class V = double> class G2Point {
public:
    V x;
    V y;

    G2Point(V px = V(), V py = V()):
        x(px),
        y(py)
    {}

    G2Point<V> operator+(const G2Vector<V>& v) const {
        return G2Point<V>(x + v.x, y + v.y);
    }

    G2Point<V>& operator+=(const G2Vector<V>& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    G2Point<V> operator+(const G2Point<V>& v) const {
        return G2Point<V>(x + v.x, y + v.y);
    }

    G2Point<V>& operator+=(const G2Point<V>& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    G2Vector<V> operator-(const G2Point<V>& v) const {
        return G2Vector<V>(x - v.x, y - v.y);
    }

    G2Point<V> operator-() const {      // Unary minus
        return G2Point<V>(-x, -y);
    }

    G2Point<V>& operator-=(const G2Vector<V>& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    G2Point<V>& operator-=(const G2Point<V>& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    G2Point<V> operator*(V c) const {
        return G2Point<V>(x*c, y*c);
    }

    bool operator==(const G2Point<V>& v) const {
        return (x == v.x && y == v.y);
    }

    bool operator!=(const G2Point<V>& v) const {
        return !(*this == v);
    }

    bool operator<=(const G2Point<V>& v) const {
        return (y < v.y || (y <= v.y && x <= v.x));
    }

    bool operator>(const G2Point<V>& v) const {
        return !(*this <= v);
    }

    bool operator<(const G2Point<V>& v) const {
        return (y < v.y || (y == v.y && x < v.x));
    }

    bool operator>=(const G2Point<V>& v) const {
        return !(*this < v);
    }

    static double signedArea(
        const G2Point<V>& a,
        const G2Point<V>& b,
        const G2Point<V>& c
    ) {
        G2Vector<V> ab = b - a;
        G2Vector<V> ac = c - a;
        return double(ab.signedArea(ac))/2.;
    }

    static double area(
        const G2Point<V>& a,
        const G2Point<V>& b,
        const G2Point<V>& c
    ) {
        return fabs(G2Point<V>::signedArea(a, b, c));
    }

    double distance(const G2Point<V>& p) const {
        return (p - *this).norm();
    }

    static double distance(const G2Point<V>& p0, const G2Point<V>& p1) {
        return p0.distance(p1);
    }

    double distanceToLine(
        const G2Point<V>& p, const G2Vector<V>& v
    ) const {
        G2Vector<V> n = v.normal();
        n.normalize();
        return fabs((*this - p)*n);
    }
};

template <class V> G2Vector<V> operator*(
    V c, const G2Vector<V>& p
) {
    return G2Vector<V>(c*p.x, c*p.y);
}

template <class V> G2Point<V> operator*(
    V c, const G2Point<V>& p
) {
    return G2Point<V>(c*p.x, c*p.y);
}

typedef G2Vector<double> R2Vector;
typedef G2Point<double> R2Point;

#endif
