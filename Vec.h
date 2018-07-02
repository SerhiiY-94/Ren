
namespace ren {
    enum e_uninitialized { uninitialize };

	template <typename T, int N>
    class Vec {
        T data_[N];
    public:
        Vec(e_uninitialized) {}
        Vec() : data_{ (T)0 } {}
        explicit Vec(T v) {
            for (int i = 0; i < N; i++) {
                data_[i] = v;
            }
        }

        template <typename... Tail>
        Vec(typename std::enable_if<sizeof...(Tail)+1 == N, T>::type head, Tail... tail)
            : data_{ head, T(tail)... } {
        }
        
        template<typename S>
        explicit Vec(const Vec<S, N> &rhs) {
            for (int i = 0; i < N; i++) {
                data_[i] = (T)rhs.data_[i];
            }
        }

        T &operator[](int i) { return data_[i]; }
        const T &operator[](int i) const { return data_[i]; }

        friend bool operator==(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
            bool res = true;
            for (int i = 0; i < N; i++) {
                if (lhs[i] != rhs[i]) {
                    res = false;
                    break;
                }
            }
            return res;
        }

        Vec<T, N> &operator+=(const Vec<T, N> &rhs) {
            for (int i = 0; i < N; i++) {
                data_[i] += rhs.data_[i];
            }
            return *this;
        }

        Vec<T, N> &operator-=(const Vec<T, N> &rhs) {
            for (int i = 0; i < N; i++) {
                data_[i] -= rhs.data_[i];
            }
            return *this;
        }

        Vec<T, N> &operator*=(const Vec<T, N> &rhs) {
            for (int i = 0; i < N; i++) {
                data_[i] *= rhs.data_[i];
            }
            return *this;
        }

        Vec<T, N> &operator/=(const Vec<T, N> &rhs) {
            for (int i = 0; i < N; i++) {
                data_[i] /= rhs.data_[i];
            }
            return *this;
        }

        friend Vec<T, N> operator-(const Vec<T, N> &v) {
            Vec<T, N> res = { uninitialize };
            for (int i = 0; i < N; i++) {
                res.data_[i] = -v.data_[i];
            }
            return res;
        }

        friend Vec<T, N> operator+(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
            Vec<T, N> res = { uninitialize };
            for (int i = 0; i < N; i++) {
                res.data_[i] = lhs.data_[i] + rhs.data_[i];
            }
            return res;
        }

        friend Vec<T, N> operator-(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
            Vec<T, N> res = { uninitialize };
            for (int i = 0; i < N; i++) {
                res.data_[i] = lhs.data_[i] - rhs.data_[i];
            }
            return res;
        }

        friend Vec<T, N> operator*(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
            Vec<T, N> res = { uninitialize };
            for (int i = 0; i < N; i++) {
                res.data_[i] = lhs.data_[i] * rhs.data_[i];
            }
            return res;
        }

        friend Vec<T, N> operator*(const T lhs, const Vec<T, N> &rhs) {
            Vec<T, N> res = { uninitialize };
            for (int i = 0; i < N; i++) {
                res.data_[i] = lhs * rhs.data_[i];
            }
            return res;
        }

        friend Vec<T, N> operator*(const Vec<T, N> &lhs, const T &rhs) {
            Vec<T, N> res = { uninitialize };
            for (int i = 0; i < N; i++) {
                res.data_[i] = lhs.data_[i] * rhs;
            }
            return res;
        }

        friend Vec<T, N> operator/(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
            Vec<T, N> res = { uninitialize };
            for (int i = 0; i < N; i++) {
                res.data_[i] = lhs.data_[i] / rhs.data_[i];
            }
            return res;
        }
    };

    template <typename T, int N>
    bool operator!=(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
        return !(lhs == rhs);
    }

    template <typename T, int N>
    T Dot(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
        T res = lhs[0] * rhs[0];
        for (int i = 1; i < N; i++) {
            res += lhs[1] * rhs[1];
        }
        return res;
    }

    template <typename T>
    Vec<T, 3> Cross(const Vec<T, 3> &lhs, const Vec<T, 3> &rhs) {
        return Vec<T, 3> { lhs[1] * rhs[2] - rhs[1] * lhs[2],
                           rhs[0] * lhs[2] - lhs[0] * rhs[2],
                           lhs[0] * rhs[1] - rhs[0] * lhs[1] };
    }

    template <typename T, int N>
    T Length(const Vec<T, N> &v) {
        return std::sqrt(Dot(v, v));
    }

    template <typename T, int N>
    T Length2(const Vec<T, N> &v) {
        return Dot(v, v);
    }

    template <typename T, int N>
    T Distance(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
        Vec<T, N> temp = lhs - rhs;
        return std::sqrt(Dot(temp, temp));
    }

    template <typename T, int N>
    T Distance2(const Vec<T, N> &lhs, const Vec<T, N> &rhs) {
        Vec<T, N> temp = lhs - rhs;
        return Dot(temp, temp);
    }

    template <typename T, int N>
    const T *ValuePtr(const Vec<T, N> &v) {
        return &v[0];
    }

    template <typename T>
    Vec<T, 2> MakeVec2(const T *v) { return Vec<T, 2>(v[0], v[1]); }
    template <typename T>
    Vec<T, 3> MakeVec3(const T *v) { return Vec<T, 3>(v[0], v[1], v[2]); }
    template <typename T>
    Vec<T, 4> MakeVec4(const T *v) { return Vec<T, 4>(v[0], v[1], v[2], v[3]); }

    using Vec2i = Vec<int, 2>;
    using Vec3i = Vec<int, 3>;
    using Vec4i = Vec<int, 4>;

    using Vec2f = Vec<float, 2>;
    using Vec3f = Vec<float, 3>;
    using Vec4f = Vec<float, 4>;

    using Vec2d = Vec<double, 2>;
    using Vec3d = Vec<double, 3>;
    using Vec4d = Vec<double, 4>;
}