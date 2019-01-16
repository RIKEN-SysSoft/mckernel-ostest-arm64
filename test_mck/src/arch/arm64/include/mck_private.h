/* mck_private.h COPYRIGHT FUJITSU LIMITED 2015 */

#ifndef __MCK_PRIVATE_H
#define __MCK_PRIVATE_H

#define MCK_PAGE_VALID		0x8000000000000000UL /* Valid TTE            */
#define MCK_PAGE_SZ4MB		0x6000000000000000UL /* 4MB Page             */
#define MCK_PAGE_SZ512K		0x4000000000000000UL /* 512K Page            */
#define MCK_PAGE_SZ64K		0x2000000000000000UL /* 64K Page             */
#define MCK_PAGE_SZ8K		0x0000000000000000UL /* 8K Page              */
#define MCK_PAGE_NFO		0x1000000000000000UL /* No Fault Only        */
#define MCK_PAGE_IE		0x0800000000000000UL /* Invert Endianness    */
#define MCK_PAGE_SOFT2		0x07FC000000000000UL /* Software bits, set 2 */
#define MCK_PAGE_RES1		0x0002000000000000UL /* Reserved             */
#define MCK_PAGE_SZ32MB		0x0001000000000000UL /* (Panther) 32MB page  */
#define MCK_PAGE_SZ256MB	0x2001000000000000UL /* (Panther) 256MB page */
#define MCK_PAGE_SZALL		0x6001000000000000UL /* All pgsz bits        */
#define MCK_PAGE_SN		0x0000800000000000UL /* (Cheetah) Snoop      */
#define MCK_PAGE_RES2		0x00007E0000000000UL /* Reserved             */
#define MCK_PAGE_PADDR		0x000001FFFFFFE000UL /* (s64VIIIfx) pa[40:13]*/
#define MCK_PAGE_SOFT		0x0000000000001F80UL /* Software bits:       */
#define MCK_PAGE_EXEC		0x0000000000001000UL /* Executable SW bit    */
#define MCK_PAGE_MODIFIED	0x0000000000000800UL /* Modified (dirty)     */
#define MCK_PAGE_READ		0x0000000000000200UL /* Readable SW Bit      */
#define MCK_PAGE_FILEOFF	0x0000000000000100UL /* remap file page flag (McK original) */
#define MCK_PAGE_PRESENT	0x0000000000000080UL /* Present              */
#define MCK_PAGE_L		0x0000000000000040UL /* Locked TTE           */
#define MCK_PAGE_CP		0x0000000000000020UL /* Cacheable in P-Cache */
#define MCK_PAGE_CV		0x0000000000000010UL /* Cacheable in V-Cache */
#define MCK_PAGE_E		0x0000000000000008UL /* side-Effect          */
#define MCK_PAGE_P		0x0000000000000004UL /* Privileged Page      */
#define MCK_PAGE_W		0x0000000000000002UL /* Writable             */

#define MCK_PAGE_SIZE_2GB	(0x1L << 31)
#define MCK_PAGE_SIZE_256MB	(0x1L << 28)
#define MCK_PAGE_SIZE_32MB	(0x1L << 25)
#define MCK_PAGE_SIZE_4MB	(0x1L << 22)
#define MCK_PAGE_SIZE_512KB	(0x1L << 19)
#define MCK_PAGE_SIZE_64KB	(0x1L << 16)
#define MCK_PAGE_SIZE_8KB	(0x1L << 13)

#define MCK_SOFT_TRAP_VA2PTE	0x40

#endif /* __MCK_PRIVATE_H */

