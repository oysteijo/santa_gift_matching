#ifndef __AVG_HAPPINESS_H__
#define __AVG_HAPPINESS_H__
#include <stdint.h>
#define n_children 1000000
#define n_child_pref 1000
#define n_gift_type 1000
extern const uint16_t wishlist[n_children][10];
extern const int32_t childlist[1000][n_child_pref];

float avg_happiness(uint16_t *giftlist);
#endif /* __AVG_HAPPINESS_H__ */
