#include <af/array.h>
#include <af/arith.h>
#include <af/traits.hpp>
#include <af/util.h>
#include <ArrayInfo.hpp>
#include "error.hpp"

namespace af
{
    array::array() : arr(0) {}
    array::array(const af_array handle): arr(handle) {}

    static void initEmptyArray(af_array *arr, af_dtype ty,
                               dim_type d0, dim_type d1=1, dim_type d2=1, dim_type d3=1)
    {
        dim_type my_dims[] = {d0, d1, d2, d3};
        AF_THROW(af_create_handle(arr, 4, my_dims, ty));
    }

    template<typename T>
    static void initDataArray(af_array *arr, const T *ptr, af_source_t src,
                               dim_type d0, dim_type d1=1, dim_type d2=1, dim_type d3=1)
    {
        af_dtype ty = (af_dtype)dtype_traits<T>::af_type;
        if (src != afHost) AF_THROW(AF_ERR_INVALID_ARG);

        dim_type my_dims[] = {d0, d1, d2, d3};
        AF_THROW(af_create_array(arr, (const void * const)ptr, 4, my_dims, ty));
    }

    array::array(const dim4 &dims, af_dtype ty) : arr(0)
    {
        initEmptyArray(&arr, ty, dims[0], dims[1], dims[2], dims[3]);
    }

    array::array(dim_type d0, af_dtype ty) : arr(0)
    {
        initEmptyArray(&arr, ty, d0);
    }

    array::array(dim_type d0, dim_type d1, af_dtype ty) : arr(0)
    {
        initEmptyArray(&arr, ty, d0, d1);
    }

    array::array(dim_type d0, dim_type d1, dim_type d2, af_dtype ty) : arr(0)
    {
        initEmptyArray(&arr, ty, d0, d1, d2);
    }

    array::array(dim_type d0, dim_type d1, dim_type d2, dim_type d3, af_dtype ty) : arr(0)
    {
        initEmptyArray(&arr, ty, d0, d1, d2, d3);
    }

    template<typename T>
    array::array(const dim4 &dims, const T *ptr, af_source_t src, dim_type ngfor) : arr(0)
    {
        initDataArray<T>(&arr, ptr, src, dims[0], dims[1], dims[2], dims[3]);
    }

    template<typename T>
    array::array(dim_type d0, const T *ptr, af_source_t src, dim_type ngfor) : arr(0)
    {
        initDataArray<T>(&arr, ptr, src, d0);
    }

    template<typename T>
    array::array(dim_type d0, dim_type d1, const T *ptr,
                 af_source_t src, dim_type ngfor) : arr(0)
    {
        initDataArray<T>(&arr, ptr, src, d0, d1);
    }

    template<typename T>
    array::array(dim_type d0, dim_type d1, dim_type d2, const T *ptr,
                 af_source_t src, dim_type ngfor) : arr(0)
    {
        initDataArray<T>(&arr, ptr, src, d0, d1, d2);
    }

    template<typename T>
    array::array(dim_type d0, dim_type d1, dim_type d2, dim_type d3,
                 const T *ptr, af_source_t src, dim_type ngfor) : arr(0)
    {
        initDataArray<T>(&arr, ptr, src, d0, d1, d2, d3);
    }

    array::~array()
    {
        if (!arr) AF_THROW(af_destroy_array(arr));
    }

    af_dtype array::type() const
    {
        af_dtype my_type;
        AF_THROW(af_get_type(&my_type, arr));
        return my_type;
    }

    dim_type array::elements() const
    {
        dim_type elems;
        AF_THROW(af_get_elements(&elems, arr));
        return elems;
    }

    template<typename T>
    T *array::host() const
    {
        if (type() != (af_dtype)dtype_traits<T>::af_type) {
            AF_THROW(AF_ERR_INVALID_TYPE);
        }

        T *res = new T[elements()];
        AF_THROW(af_get_data_ptr((void *)res, arr));

        return res;
    }

    void array::host(void *data) const
    {
        AF_THROW(af_get_data_ptr(data, arr));
    }

    af_array array::get() const
    {
        return arr;
    }

    // Helper functions
    dim4 array::dims() const
    {
        ArrayInfo info = getInfo(arr);
        return info.dims();
    }

    dim_type array::dims(unsigned dim) const
    {
        ArrayInfo info = getInfo(arr);
        return info.dims()[dim];
    }

    unsigned array::numdims() const
    {
        ArrayInfo info = getInfo(arr);
        return info.ndims();
    }

    size_t array::bytes() const
    {
        ArrayInfo info = getInfo(arr);
        return info.elements() * sizeof(type());
    }

    array array::copy() const
    {
        af_array *other = 0;
        AF_THROW(af_copy_array(other, arr));
        return array(*other);
    }

    bool array::isempty() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isEmpty();
    }

    bool array::isscalar() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isScalar();
    }

    bool array::isvector() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isVector();
    }

    bool array::isrow() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isRow();
    }

    bool array::iscolumn() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isColumn();
    }

    bool array::iscomplex() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isComplex();
    }

    bool array::isdouble() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isDouble();
    }

    bool array::issingle() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isSingle();
    }

    bool array::isrealfloating() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isRealFloating();
    }

    bool array::isfloating() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isFloating();
    }

    bool array::isinteger() const
    {
        ArrayInfo info = getInfo(arr);
        return info.isInteger();
    }

    array constant(double val, const dim4 &dims, af_dtype type)
    {
        af_array res;
        AF_THROW(af_constant(&res, val, dims.ndims(), dims.get(), type));
        return array(res);
    }

    array constant(af_cdouble val, const dim4 &dims)
    {
        double val_[] = {real(val), imag(val)};
        af_array res;
        AF_THROW(af_constant_c64(&res, val_, dims.ndims(), dims.get()));
        return array(res);
    }

    array constant(af_cfloat val, const dim4 &dims)
    {
        float val_[] = {real(val), imag(val)};
        af_array res;
        AF_THROW(af_constant_c32(&res, val_, dims.ndims(), dims.get()));
        return array(res);
    }

    array constant(double val, const dim_type d0, af_dtype ty)
    {
        return constant(val, dim4(d0), ty);
    }

    array constant(double val, const dim_type d0,
                         const dim_type d1, af_dtype ty)
    {
        return constant(val, dim4(d0, d1), ty);
    }

    array constant(double val, const dim_type d0,
                         const dim_type d1, const dim_type d2, af_dtype ty)
    {
        return constant(val, dim4(d0, d1, d2), ty);
    }

    array constant(double val, const dim_type d0,
                         const dim_type d1, const dim_type d2,
                         const dim_type d3, af_dtype ty)
    {
        return constant(val, dim4(d0, d1, d2, d3), ty);
    }


    array randu(const dim4 &dims, af_dtype type)
    {
        af_array res;
        AF_THROW(af_randu(&res, dims.ndims(), dims.get(), type));
        return array(res);
    }

    array randu(const dim_type d0, af_dtype ty)
    {
        return randu(dim4(d0), ty);
    }

    array randu(const dim_type d0,
                const dim_type d1, af_dtype ty)
    {
        return randu(dim4(d0, d1), ty);
    }

    array randu(const dim_type d0,
                const dim_type d1, const dim_type d2, af_dtype ty)
    {
        return randu(dim4(d0, d1, d2), ty);
    }

    array randu(const dim_type d0,
                const dim_type d1, const dim_type d2,
                const dim_type d3, af_dtype ty)
    {
        return randu(dim4(d0, d1, d2, d3), ty);
    }

    array randn(const dim4 &dims, af_dtype type)
    {
        af_array res;
        AF_THROW(af_randn(&res, dims.ndims(), dims.get(), type));
        return array(res);
    }

    array randn(const dim_type d0, af_dtype ty)
    {
        return randn(dim4(d0), ty);
    }

    array randn(const dim_type d0,
                const dim_type d1, af_dtype ty)
    {
        return randn(dim4(d0, d1), ty);
    }

    array randn(const dim_type d0,
                const dim_type d1, const dim_type d2, af_dtype ty)
    {
        return randn(dim4(d0, d1, d2), ty);
    }

    array randn(const dim_type d0,
                const dim_type d1, const dim_type d2,
                const dim_type d3, af_dtype ty)
    {
        return randn(dim4(d0, d1, d2, d3), ty);
    }

    array array::as(af_dtype type) const
    {
        af_array out;
        AF_THROW(af_cast(&out, this->get(), type));
        return array(out);
    }

#define INSTANTIATE(T)  \
    template array::array<T>(const dim4 &dims, const T *ptr, af_source_t src, dim_type ngfor);\
    template array::array<T>(dim_type d0, const T *ptr, af_source_t src, dim_type ngfor);\
    template array::array<T>(dim_type d0, dim_type d1, const T *ptr, af_source_t src, dim_type ngfor);\
    template array::array<T>(dim_type d0, dim_type d1, dim_type d2, const T *ptr, af_source_t src, dim_type ngfor);\
    template array::array<T>(dim_type d0, dim_type d1, dim_type d2, dim_type d3, const T *ptr, af_source_t src, dim_type ngfor);\
    template T *array::host<T>() const;


    INSTANTIATE(af_cdouble)
    INSTANTIATE(af_cfloat)
    INSTANTIATE(double)
    INSTANTIATE(float)
    INSTANTIATE(unsigned)
    INSTANTIATE(int)
    INSTANTIATE(unsigned char)
    INSTANTIATE(char)

#undef INSTANTIATE

    ///////////////////////////////////////////////////////////////////////////
    // Operator Overloading for array
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // Operator =
    ///////////////////////////////////////////////////////////////////////////
    array array::operator=(const array &other)
    {
        // FIXME
        if (this->get() == other.get()) {
            return *this;
        }
        if(this->get() != 0) {
            AF_THROW(af_destroy_array(this->get()));
        }

        af_array temp = 0;
        AF_THROW(af_weak_copy(&temp, other.get()));
        this->arr = temp;
        return *this;
    }

    array array::operator=(const double &value)
    {
        if(this->get() != 0) {
            AF_THROW(af_destroy_array(this->get()));
        }

        af_array temp = 0;
        array cst = constant(value, this->dims(), this->type());
        AF_THROW(af_weak_copy(&temp, cst.get()));
        this->arr = temp;
        return *this;
    }

    array array::operator=(const af_cdouble &value)
    {
        if(this->get() != 0) {
            AF_THROW(af_destroy_array(this->get()));
        }

        af_array temp = 0;
        array cst = constant(value, this->dims());
        AF_THROW(af_weak_copy(&temp, cst.get()));
        this->arr = temp;
        return *this;
    }

    array array::operator=(const af_cfloat &value)
    {
        if(this->get() != 0) {
            AF_THROW(af_destroy_array(this->get()));
        }

        af_array temp = 0;
        array cst = constant(value, this->dims());
        AF_THROW(af_weak_copy(&temp, cst.get()));
        this->arr = temp;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Operator +=, -=, *=, /=
    ///////////////////////////////////////////////////////////////////////////
#define INSTANTIATE(op, func)                                               \
    array array::operator op(const array &other)                            \
    {                                                                       \
        return *this = *this op other;                                      \
    }                                                                       \
    array array::operator op(const double &value)                           \
    {                                                                       \
        array cst = constant(value, this->dims(), this->type());            \
        return *this = *this op cst;                                        \
    }                                                                       \
    array array::operator op(const af_cdouble &value)                       \
    {                                                                       \
        array cst = constant(value, this->dims());                          \
        return *this = *this op cst;                                        \
    }                                                                       \
    array array::operator op(const af_cfloat &value)                        \
    {                                                                       \
        array cst = constant(value, this->dims());                          \
        return *this = *this op cst;                                        \
    }                                                                       \

    INSTANTIATE(+=, af_add)
    INSTANTIATE(-=, af_sub)
    INSTANTIATE(*=, af_mul)
    INSTANTIATE(/=, af_div)

#undef INSTANTIATE

    ///////////////////////////////////////////////////////////////////////////
    // Operator +, -, *, /
    ///////////////////////////////////////////////////////////////////////////
#define INSTANTIATE(op, func)                                               \
    array array::operator op(const array &other) const                      \
    {                                                                       \
        af_array out;                                                       \
        AF_THROW(func(&out, this->get(), other.get()));                     \
        return array(out);                                                  \
    }                                                                       \
    array array::operator op(const double &value) const                     \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, this->dims(), this->type());            \
        AF_THROW(func(&out, this->get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array array::operator op(const af_cdouble &value) const                 \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, this->dims());                          \
        AF_THROW(func(&out, this->get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array array::operator op(const af_cfloat &value) const                  \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, this->dims());                          \
        AF_THROW(func(&out, this->get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array operator op(const double &value, const array &other)              \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, other.dims(), other.type());            \
        AF_THROW(func(&out, other.get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array operator op(const af_cdouble &value, const array& other)          \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, other.dims());                          \
        AF_THROW(func(&out, other.get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array operator op(const af_cfloat &value, const array& other)           \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, other.dims());                          \
        AF_THROW(func(&out, other.get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \

    INSTANTIATE(+, af_add)
    INSTANTIATE(-, af_sub)
    INSTANTIATE(*, af_mul)
    INSTANTIATE(/, af_div)

#undef INSTANTIATE

    ///////////////////////////////////////////////////////////////////////////
    // Operator ==, !=, < <=, >, >=
    ///////////////////////////////////////////////////////////////////////////
#define INSTANTIATE(op, func)                                               \
    array array::operator op(const array &other) const                      \
    {                                                                       \
        af_array out;                                                       \
        AF_THROW(func(&out, this->get(), other.get()));                     \
        return array(out);                                                  \
    }                                                                       \
    array array::operator op(const bool &value) const                       \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, this->dims(), this->type());            \
        AF_THROW(func(&out, this->get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array array::operator op(const int &value) const                        \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, this->dims(), this->type());            \
        AF_THROW(func(&out, this->get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array array::operator op(const double &value) const                     \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, this->dims(), this->type());            \
        AF_THROW(func(&out, this->get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array array::operator op(const af_cdouble &value) const                 \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, this->dims());                          \
        AF_THROW(func(&out, this->get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array array::operator op(const af_cfloat &value) const                  \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, this->dims());                          \
        AF_THROW(func(&out, this->get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array operator op(const bool &value, const array &other)                \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, other.dims(), other.type());            \
        AF_THROW(func(&out, other.get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array operator op(const int &value, const array &other)                 \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, other.dims(), other.type());            \
        AF_THROW(func(&out, other.get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array operator op(const double &value, const array &other)              \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, other.dims(), other.type());            \
        AF_THROW(func(&out, other.get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array operator op(const af_cdouble &value, const array& other)          \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, other.dims());                          \
        AF_THROW(func(&out, other.get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \
    array operator op(const af_cfloat &value, const array& other)           \
    {                                                                       \
        af_array out;                                                       \
        array cst = constant(value, other.dims());                          \
        AF_THROW(func(&out, other.get(), cst.get()));                       \
        return array(out);                                                  \
    }                                                                       \

    INSTANTIATE(==, af_eq)
    INSTANTIATE(!=, af_neq)
    INSTANTIATE(< , af_lt)
    INSTANTIATE(<=, af_le)
    INSTANTIATE(> , af_gt)
    INSTANTIATE(>=, af_ge)

#undef INSTANTIATE
}
