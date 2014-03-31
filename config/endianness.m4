##Determines endianness and other useful things... 

#Check endianness
AC_C_BIGENDIAN(
	AC_SUBST([BIG_ENDIAN_DETECTED], ["#define BIG_ENDIAN_DETECTED 1"]), 
	AC_SUBST([LITTLE_ENDIAN_DETECTED], ["#define LITTLE_ENDIAN_DETECTED 1"]), 
	AC_MSG_ERROR([Unkown endianness]), 
	AC_MSG_ERROR([Unkown endianness])
)


AC_CHECK_HEADER(byteswap.h,
    [AC_SUBST([BYTESWAP_HEADER_DETECTED], ["#define BYTESWAP_HEADER_DETECTED 1"])],
    [AC_MSG_ERROR([Could not find or include byteswap.h])]
    )

