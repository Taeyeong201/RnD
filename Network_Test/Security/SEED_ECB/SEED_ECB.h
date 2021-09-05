#pragma once

#if __alpha__	||	__alpha	||	__i386__	||	i386	||	_M_I86	||	_M_IX86	||	\
	__OS2__		||	sun386	||	__TURBOC__	||	vax		||	vms		||	VMS		||	__VMS || __linux__
#define LITTLE_ENDIAN
#else
#define BIG_ENDIAN
#endif


#define NoRounds         16						// the number of rounds
#define NoRoundKeys      (NoRounds*2)			// the number of round-keys
#define SeedBlockSize    16    					// block length in bytes
#define SeedBlockLen     128   					// block length in bits

// macroses for left or right rotations
#if defined(_MSC_VER)
#define ROTL(x, n)     (_lrotl((x), (n)))		// left rotation
#define ROTR(x, n)     (_lrotr((x), (n)))		// right rotation
#else
#define ROTL(x, n)     (((x) << (n)) | ((x) >> (32-(n))))		// left rotation
#define ROTR(x, n)     (((x) >> (n)) | ((x) << (32-(n))))		// right rotation
#endif

// macroses for converting endianess
#define EndianChange(dwS)                       \
    ( (ROTL((dwS),  8) & (DWORD)0x00ff00ff) |   \
      (ROTL((dwS), 24) & (DWORD)0xff00ff00) )

#define KC0     0x9e3779b9UL
#define KC1     0x3c6ef373UL
#define KC2     0x78dde6e6UL
#define KC3     0xf1bbcdccUL
#define KC4     0xe3779b99UL
#define KC5     0xc6ef3733UL
#define KC6     0x8dde6e67UL
#define KC7     0x1bbcdccfUL
#define KC8     0x3779b99eUL
#define KC9     0x6ef3733cUL
#define KC10    0xdde6e678UL
#define KC11    0xbbcdccf1UL
#define KC12    0x779b99e3UL
#define KC13    0xef3733c6UL
#define KC14    0xde6e678dUL
#define KC15    0xbcdccf1bUL

#include <iostream>
#include <memory>

class SEED_ECB
{
public:
	enum class ENC_DEC {
		DECRYPT = 0,
		ENCRYPT
	};

	/**
	@brief SEED_ECB 생성자
	@param key : 16byte 암호키
	@param mode : SEED_ECB::ENC_DEC
	*/
	SEED_ECB(unsigned char* key, ENC_DEC mode = ENC_DEC::ENCRYPT);
	~SEED_ECB();

	/**
	@brief MODE 변경 ( ENCRYPT or DECRYPT )
	@param mode : SEED_ECB::ENC_DEC
	*/
	void changeMode(ENC_DEC mode);

	/**
	@brief mode 상태에 따라 암복호화 진행
	@param in : Input Buffer ( Source )
	@param out : Output Buffer ( 메모리 할당하지 않은 변수로 전달 )
	메모리가 할당된 변수이면 메모리 누수 발생!
	@param len : Input Buffer의 size
	@return Output Buffer의 Size
	*/
	size_t SEED_ECB_Process(const unsigned char* in, unsigned char*& out, size_t len);

	/**
	@brief mode 상태에 따라 암복호화 진행
	@param in : Input Buffer ( Source )
	@param out : Output Buffer ( 메모리 할당하지 않은 변수로 전달 )
	메모리가 할당된 변수이면 메모리 누수 발생!
	@param len : Input Buffer의 size
	@return Output Buffer의 Size
	*/
	size_t SEED_ECB_32byte_Process(const unsigned char* in, unsigned char*& out, size_t len);

	void printRoundKey();

	//SEED_ECB& operator=(SEED_ECB&& a);
private:
	std::unique_ptr<unsigned int> pRoundKey;
	ENC_DEC mode;

	template <typename T> unsigned char GetB0(T t);
	template <typename T> unsigned char GetB1(T t);
	template <typename T> unsigned char GetB2(T t);
	template <typename T> unsigned char GetB3(T t);

	template <typename T> 
	void SEED_KeySched(T &L0, T &L1, T &R0, T &R1, T* K);

	template <typename T>
	void RoundKeyUpdate0(T *K, T& A, T& B, T& C, T& D, const unsigned long long KC);
	template <typename T>
	void RoundKeyUpdate1(T *K, T& A, T& B, T& C, T& D, const unsigned long long KC);

	static unsigned int SS0[256];
	static unsigned int SS1[256];
	static unsigned int SS2[256];
	static unsigned int SS3[256];

	/**
	@brief 16 Byte 암복호화
	@param in : input Buffer (16 Byte)
	@param out : output Buffer (16 Byte)
	*/
	void SEED_16byte_Process(unsigned char* in, unsigned char* out);

	/**
	@brief Private Key로 Round Key 생성
	@param pdwRoundKey : Round Key
	@param pbUserKey : Private Key
	*/
	void SEED_KeySchedKey(unsigned char* pbUserKey);

};

template<typename T>
inline unsigned char  SEED_ECB::GetB0(T t)
{
	return (unsigned char)(t);
}

template<typename T>
inline unsigned char  SEED_ECB::GetB1(T t)
{
	return (unsigned char)((t) >> 8);
}

template<typename T>
inline unsigned char  SEED_ECB::GetB2(T t)
{
	return (unsigned char)((t) >> 16);
}

template<typename T>
inline unsigned char  SEED_ECB::GetB3(T t)
{
	return (unsigned char)((t) >> 24);
}

template<typename T>
inline void SEED_ECB::SEED_KeySched(T &L0, T &L1, T& R0, T& R1, T* K)
{
	T T0 = R0 ^ (K)[0];
	T T1 = R1 ^ (K)[1];
	T1 ^= T0;
	T1 = SS0[GetB0(T1)] ^ SS1[GetB1(T1)] ^ SS2[GetB2(T1)] ^ SS3[GetB3(T1)];
	T0 = (T0 + T1) & 0xffffffff;
	T0 = SS0[GetB0(T0)] ^ SS1[GetB1(T0)] ^ SS2[GetB2(T0)] ^ SS3[GetB3(T0)];
	T1 = (T1 + T0) & 0xffffffff;
	T1 = SS0[GetB0(T1)] ^ SS1[GetB1(T1)] ^ SS2[GetB2(T1)] ^ SS3[GetB3(T1)];
	T0 = (T0 + T1) & 0xffffffff;
	L0 ^= T0; L1 ^= T1;
}

template<typename T>
inline void SEED_ECB::RoundKeyUpdate0(T *K, T &A, T &B, T &C, T &D, const unsigned long long KC)
{
	unsigned long long T0 = A + C - KC;
	unsigned long long T1 = B + KC - D;
	(K)[0] = SS0[GetB0(T0)] ^ SS1[GetB1(T0)] ^ SS2[GetB2(T0)] ^ SS3[GetB3(T0)];   
	(K)[1] = SS0[GetB0(T1)] ^ SS1[GetB1(T1)] ^ SS2[GetB2(T1)] ^ SS3[GetB3(T1)];   
	T0 = A;                                     
	A = (A >> 8) ^ (B << 24);
	B = (B >> 8) ^ (T)(T0 << 24);
}

template<typename T>
inline void SEED_ECB::RoundKeyUpdate1(T *K, T& A, T& B, T& C, T& D, const unsigned long long KC)
{
	unsigned long long T0 = A + C - KC;
	unsigned long long T1 = B + KC - D;
	(K)[0] = SS0[GetB0(T0)] ^ SS1[GetB1(T0)] ^ SS2[GetB2(T0)] ^ SS3[GetB3(T0)];
	(K)[1] = SS0[GetB0(T1)] ^ SS1[GetB1(T1)] ^ SS2[GetB2(T1)] ^ SS3[GetB3(T1)];
	T0 = C;
	C = (C << 8) ^ (D >> 24);
	D = (D << 8) ^ (T)(T0 >> 24);
}
