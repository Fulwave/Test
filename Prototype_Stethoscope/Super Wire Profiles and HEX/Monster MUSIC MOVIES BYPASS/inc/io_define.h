#ifndef __io_define_H__
#define __io_define_H__

#ifdef __cplusplus
extern "C" {
#endif


#define	PO_LEDA(a)	(GPC_1=a)
#define	PO_LEDB(a)	(GPC_0=a)
#define	PO_LEDC(a)	(GPC_11=a)
#define	PO_LEDD(a)	(GPC_10=a)
#define	PO_LEDE(a)	(GPC_13=a)
#define	PO_LEDF(a)	(GPC_12=a)

#define	PO_1761_SDA(a)	(GPA_10=a)
#define	PI_1761_SDA()	(GPA_10)
#define	PO_1761_SCL(a)	(GPA_11=a)

#define	PI_KEY_MODE()	(GPC_2)

#ifdef __cplusplus
}
#endif
#endif /*__io_define_H__*/

