#ifndef WALLE_FORMAT_DETAIL_BIGNUM_H_
#define WALLE_FORMAT_DETAIL_BIGNUM_H_

#include <walle/config/base.h>
#include <walle/buffer/buffer_ref.h>
#include <cstdint>

namespace walle {

namespace format_detail {

class Bignum {
public: 
    Bignum();
    
    /** 
     * @brief  assign a uint16_t value to the obj.
     * @note   
     * @param  v: the value to assign.
     * @retval None
     */
    void assign_uint16(uint16_t v);

    /** 
     * @brief  assign a uint32_t value to the obj.
     * @note   
     * @param  v: the value to assign.
     * @retval None
     */
    void assign_uint32(uint32_t v);

    /** 
     * @brief  assign a uint64_t value to the obj.
     * @note   
     * @param  v: the value to assign.
     * @retval None
     */
    void assign_uint64(uint64_t v);

    /** 
     * @brief  assign an other obj value to the obj.
     * @note   
     * @param  &v: the object to assign
     * @retval None
     */
    void assign_bignum(const Bignum &v);

    /** 
     * @brief  
     * @note   
     * @param  v: 
     * @retval None
     */
    void assign_decimal_string(walle::BufferRef<const char> v);

    /** 
     * @brief  
     * @note   
     * @param  v: 
     * @retval None
     */
    void assign_hex_string(walle::BufferRef<const char> v);

    /** 
     * @brief  assign a uint16_t based exponent number.
     * @note   
     * @param  base: like 2 ^ 12, 2 is the base. 
     * @param  exponent:  like 2 ^ 12, 12 is the exponent. 
     * @retval None
     */
    void assign_power_uint16(uint16_t base, int exponent);

    /** 
     * @brief  
     * @note   
     * @retval None
     */
    void add_uint64(uint64_t v);

    /** 
     * @brief  
     * @note   
     * @param  &v: 
     * @retval None
     */
    void add_bignum(const Bignum &v);

    /** 
     * @brief  
     * @note   
     * @param  &v: 
     * @retval None
     */
    void subtract_bignum(const Bignum &v);

    /** 
     * @brief  
     * @note   
     * @retval None
     */
    void square();

    /** 
     * @brief  
     * @note   
     * @param  shiftCount: 
     * @retval None
     */
    void shift_left(int shiftCount);

    /** 
     * @brief  
     * @note   
     * @param  factor: 
     * @retval None
     */
    void multi_by_uint32(uint32_t factor);

    /** 
     * @brief  
     * @note   
     * @param  factor: 
     * @retval None
     */
    void multi_by_uint64(uint64_t factor);

    /** 
     * @brief  
     * @note   
     * @param  exponent: 
     * @retval None
     */
    void  multi_by_power_of_ten(int exponent);
    /** 
     * @brief  
     * @note   
     * @retval None
     */
    void times_10() 
    {
        return multi_by_uint32(10);
    }

    uint16_t divide_modulo_int_bignum(const Bignum& other);
    /** 
     * @brief  
     * @note   
     * @param  &v: 
     * @retval 
     */
    uint16_t divide_module_by_bignum(const Bignum &v);

    /** 
     * @brief  
     * @note   
     * @retval 
     */
    bool to_hex_string(char* buffer, int buffer_size) const;

    /** 
     * @brief  
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @retval 
     */
    static int compare(const Bignum &a, const Bignum &b);

    /** 
     * @brief  
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @retval 
     */
    static bool equal(const Bignum &a, const Bignum &b)
    {
        return compare(a, b) == 0;
    }

    /** 
     * @brief  
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @retval 
     */
    static bool less_equal(const Bignum &a, const Bignum &b)
    {
        return compare(a, b) <= 0;          
    }

    /** 
     * @brief  
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @retval 
     */
    static bool less(const Bignum &a, const Bignum &b)
    {
        return compare(a, b) < 0;
    }

    /** 
     * @brief  
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @param  &c: 
     * @retval 
     */
    static int plus_compare(const Bignum &a, const Bignum &b, const Bignum &c);

    /** 
     * @brief  
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @param  &c: 
     * @retval 
     */
    static bool plus_equal(const Bignum &a, const Bignum &b, const Bignum &c)
    {
        return plus_compare(a, b, c) == 0;
    }

    /** 
     * @brief  
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @param  &c: 
     * @retval 
     */
    static bool plus_less_equal(const Bignum &a, const Bignum &b, const Bignum &c)
    {
        return plus_compare(a, b, c) <= 0;
    }

    /** 
     * @brief  
     * @note   
     * @param  &a: 
     * @param  &b: 
     * @param  &c: 
     * @retval 
     */
    static bool plus_less(const Bignum &a, const Bignum &b, const Bignum &c)
    {
        return plus_compare(a, b, c) < 0;
    }
public: 

    /** 
     * @note   584 = 128 * 28. We can represent 2^3584 > 10^1000 accurately.
     * This bignum can encode much bigger numbers, since it contains an
     * exponent.
     */
    static const int kMaxSignificantBits = 3584;

private:

    typedef uint32_t Chunk;
    typedef uint64_t DoubleChunk;

    static const int kChunkSize = sizeof(Chunk) * 8;
    static const int kDoubleChunkSize = sizeof(DoubleChunk) * 8;
    /** 
     * @note  With bigit size of 28 we loose some bits, but a double still fits easily
     * into two chunks, and more importantly we can use the Comba multiplication.  
     */
    static const int kBigitSize = 28;
    static const Chunk kBigitMask = (1 << kBigitSize) - 1;
    /** 
     * @note Every instance allocates kBigitLength chunks on the stack. Bignums cannot 
     * grow. There are no checks if the stack-allocated space is sufficient.  
     */
    static const int kBigitCapacity = kMaxSignificantBits / kBigitSize;

private:
    void align(const Bignum& other);
    void clamp();
    bool is_clamped() const;
    void zero();
    void bigits_shift_left(int shiftAmount);
    int bigit_length() const { return _used_bigits + _exponent; }
    Chunk bigit_at(int index) const;
    void subtract_times(const Bignum& other, int factor);
    void ensure_capcity(int size);
    WALLE_NON_COPYABLE(Bignum);
private:
    Chunk                               _bigits_buffer[kBigitCapacity];
    BufferRef<Chunk>                    _bigits;
    int                                 _used_bigits;
    int                                 _exponent;
    
};


} //namespace format_detail
} //namespace walle
#endif //WALLE_FORMAT_DETAIL_BIGNUM_H_