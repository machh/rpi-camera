#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

/* define uchar, ushort, uint, ulong */
#ifndef uchar
#define uchar       unsigned char
#endif

#ifndef ushort
#define ushort      unsigned short
#endif

#ifndef uint
#define uint        unsigned int
#endif

#ifndef ulong
#define ulong       unsigned long
#endif

/* define [u]int8/16/32/64, uintptr */
#ifndef uint8
#define uint8       unsigned char
#endif

#ifndef uint16
#define uint16      unsigned short
#endif

#ifndef uint32
#define uint32      unsigned int
#endif

#ifndef uint64
#define uint64      unsigned long long
#endif

#ifndef int8
#define int8        signed char
#endif

#ifndef int16
#define int16       signed short
#endif

#ifndef int32
#define int32       signed int
#endif

#ifndef int64
#define int64       signed long long
#endif

/* define float32/64, float_t */
#ifndef float32
#define float32     float
#endif

#ifndef float64
#define float64     double
#endif

/* define macro values */
#ifndef FALSE
#define FALSE	    0
#endif

#ifndef TRUE
#define TRUE	    1  /* TRUE */
#endif

#ifndef NULL
#define	NULL	    0
#endif

#ifndef OFF
#define	OFF	    0
#endif

#ifndef ON
#define	ON	    1  /* ON = 1 */
#endif

#ifndef OK
#define	OK	    1  /* OK = 1 */
#endif

#ifndef ERROR
#define	ERROR	    -1  /* ERROR = -1 */
#endif

#endif /* _TYPEDEFS_H_ */
