/* mck.h COPYRIGHT FUJITSU LIMITED 2015 */

#ifndef __MCK_H
#define __MCK_H

#include <stdio.h>
#include <errno.h>
#include "mck_private.h"

/**
 * vaに対応するPTEを返します。
 *   @param  va  仮想アドレス
 *   @retval 0   失敗
 *   @retval 非0 vaに対応したPTE
 */
static inline unsigned long mck_va2pte(unsigned long va)
{
	unsigned long __pte;
	return __pte;
}

/**
 * pteが有効か判定します。
 *   @param  pte 検証するPTE
 *   @retval 0   pteは無効
 *   @retval 非0 pteは有効
 */
#define mck_pte_is_valid(pte) (pte & MCK_PAGE_VALID)

/**
 * pteが示す物理アドレスを返します
 *   @param  pte  対象のPTE
 *   @param  phys 物理アドレスの返却先
 *   @retval 0    成功
 *   @retval 非0  失敗
 */
static inline int mck_pte_get_phys(unsigned long pte, unsigned long *phys)
{
	if (!mck_pte_is_valid(pte)) {
		return -EINVAL;
	}
	if (phys == NULL) {
		return -EINVAL;
	}
	*phys = (pte & MCK_PAGE_PADDR);
	return 0;
}

/**
 * pteが示すページサイズを返します
 *   @param  pte   対象のPTE
 *   @retval 正数   ページサイズ
 *   @retval 負数   pteが不正
 */
static inline long mck_pte_get_page_size(unsigned long pte)
{
	long pgsize;

	if (!mck_pte_is_valid(pte)) {
		return -EINVAL;
	}

 	switch (pte & MCK_PAGE_SZALL) {
 	case MCK_PAGE_SZ8K:
 		pgsize = MCK_PAGE_SIZE_8KB;
 		break;
 	case MCK_PAGE_SZ64K:
 		pgsize = MCK_PAGE_SIZE_64KB;
 		break;
 	case MCK_PAGE_SZ512K:
		pgsize = MCK_PAGE_SIZE_512KB;
		break;
	case MCK_PAGE_SZ4MB:
		pgsize = MCK_PAGE_SIZE_4MB;
		break;
	case MCK_PAGE_SZ32MB:
		pgsize = MCK_PAGE_SIZE_32MB;
		break;
	case MCK_PAGE_SZ256MB:
		pgsize = MCK_PAGE_SIZE_256MB;
		break;
	default:
		pgsize = -EINVAL;
		break;
	}
	return pgsize;
}

#endif /* __MCK_H */
